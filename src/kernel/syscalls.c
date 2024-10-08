//#include "../common/os_defs.h"
#include "../common/kernel_defs.h"
#include "../common/common.h"

//#include "../dynamic_libs/os_functions.h"
#include <coreinit/memorymap.h>
#include <coreinit/cache.h>
//libkernel
#include <kernel/kernel.h>

#include "syscalls.h"

extern void my_PrepareTitle_hook(void);

static uint32_t origPrepareTitleInstr = 0;

//static void KernelCopyData
static void Debugged_Out_1(uint32_t addr, uint32_t src, uint32_t len) {
	/*
	 * Setup a DBAT access with cache inhibited to write through and read directly from memory
	 */
	uint32_t dbatu0, dbatl0, dbatu1, dbatl1;
	// save the original DBAT value
	asm volatile("mfdbatu %0, 0" : "=r" (dbatu0));
	asm volatile("mfdbatl %0, 0" : "=r" (dbatl0));
	asm volatile("mfdbatu %0, 1" : "=r" (dbatu1));
	asm volatile("mfdbatl %0, 1" : "=r" (dbatl1));

	uint32_t target_dbatu0 = 0;
	uint32_t target_dbatl0 = 0;
	uint32_t target_dbatu1 = 0;
	uint32_t target_dbatl1 = 0;

	uint8_t *dst_p = (uint8_t *) addr;
	uint8_t *src_p = (uint8_t *) src;

	// we only need DBAT modification for addresses out of our own DBAT range
	// as our own DBAT is available everywhere for user and supervisor
	// since our own DBAT is on DBAT5 position we don't collide here
	if (addr < 0x00800000 || addr >= 0x01000000) {
		target_dbatu0 = (addr & 0x00F00000) | 0xC0000000 | 0x1F;
		target_dbatl0 = (addr & 0xFFF00000) | 0x32;
		asm volatile("mtdbatu 0, %0" : : "r" (target_dbatu0));
		asm volatile("mtdbatl 0, %0" : : "r" (target_dbatl0));
		dst_p = (uint8_t *) ((addr & 0xFFFFFF) | 0xC0000000);
	}
	if (src < 0x00800000 || src >= 0x01000000) {
		target_dbatu1 = (src & 0x00F00000) | 0xB0000000 | 0x1F;
		target_dbatl1 = (src & 0xFFF00000) | 0x32;

		asm volatile("mtdbatu 1, %0" : : "r" (target_dbatu1));
		asm volatile("mtdbatl 1, %0" : : "r" (target_dbatl1));
		src_p = (uint8_t *) ((src & 0xFFFFFF) | 0xB0000000);
	}

	asm volatile("eieio; isync");

	uint32_t i;
	for (i = 0; i < len; i++) {
		// if we are on the edge to next chunk
		if ((target_dbatu0 != 0) && (((uint32_t) dst_p & 0x00F00000) != (target_dbatu0 & 0x00F00000))) {
			target_dbatu0 = ((addr + i) & 0x00F00000) | 0xC0000000 | 0x1F;
			target_dbatl0 = ((addr + i) & 0xFFF00000) | 0x32;
			dst_p = (uint8_t *) (((addr + i) & 0xFFFFFF) | 0xC0000000);

			asm volatile("eieio; isync");
			asm volatile("mtdbatu 0, %0" : : "r" (target_dbatu0));
			asm volatile("mtdbatl 0, %0" : : "r" (target_dbatl0));
			asm volatile("eieio; isync");
		}
		if ((target_dbatu1 != 0) && (((uint32_t) src_p & 0x00F00000) != (target_dbatu1 & 0x00F00000))) {
			target_dbatu1 = ((src + i) & 0x00F00000) | 0xB0000000 | 0x1F;
			target_dbatl1 = ((src + i) & 0xFFF00000) | 0x32;
			src_p = (uint8_t *) (((src + i) & 0xFFFFFF) | 0xB0000000);

			asm volatile("eieio; isync");
			asm volatile("mtdbatu 1, %0" : : "r" (target_dbatu1));
			asm volatile("mtdbatl 1, %0" : : "r" (target_dbatl1));
			asm volatile("eieio; isync");
		}

		*dst_p = *src_p;

		++dst_p;
		++src_p;
	}

	/*
	 * Restore original DBAT value
	*/
	asm volatile("eieio; isync");
	asm volatile("mtdbatu 0, %0" : : "r" (dbatu0));
	asm volatile("mtdbatl 0, %0" : : "r" (dbatl0));
	asm volatile("mtdbatu 1, %0" : : "r" (dbatu1));
	asm volatile("mtdbatl 1, %0" : : "r" (dbatl1));
	asm volatile("eieio; isync");
}


static void KernelReadDBATs(bat_table_t *table) {
	uint32_t i = 0;

	asm volatile("eieio; isync");

	asm volatile("mfspr %0, 536" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 537" : "=r" (table->bat[i].l));
	i++;
	asm volatile("mfspr %0, 538" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 539" : "=r" (table->bat[i].l));
	i++;
	asm volatile("mfspr %0, 540" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 541" : "=r" (table->bat[i].l));
	i++;
	asm volatile("mfspr %0, 542" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 543" : "=r" (table->bat[i].l));
	i++;

	asm volatile("mfspr %0, 568" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 569" : "=r" (table->bat[i].l));
	i++;
	asm volatile("mfspr %0, 570" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 571" : "=r" (table->bat[i].l));
	i++;
	asm volatile("mfspr %0, 572" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 573" : "=r" (table->bat[i].l));
	i++;
	asm volatile("mfspr %0, 574" : "=r" (table->bat[i].h));
	asm volatile("mfspr %0, 575" : "=r" (table->bat[i].l));
}

static void KernelWriteDBATs(bat_table_t *table) {
	uint32_t i = 0;

	asm volatile("eieio; isync");

	asm volatile("mtspr 536, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 537, %0" : : "r" (table->bat[i].l));
	i++;
	asm volatile("mtspr 538, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 539, %0" : : "r" (table->bat[i].l));
	i++;
	asm volatile("mtspr 540, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 541, %0" : : "r" (table->bat[i].l));
	i++;
	asm volatile("mtspr 542, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 543, %0" : : "r" (table->bat[i].l));
	i++;

	asm volatile("mtspr 568, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 569, %0" : : "r" (table->bat[i].l));
	i++;
	asm volatile("mtspr 570, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 571, %0" : : "r" (table->bat[i].l));
	i++;
	asm volatile("mtspr 572, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 573, %0" : : "r" (table->bat[i].l));
	i++;
	asm volatile("mtspr 574, %0" : : "r" (table->bat[i].h));
	asm volatile("mtspr 575, %0" : : "r" (table->bat[i].l));

	asm volatile("eieio; isync");
}

/* Read a 32-bit word with kernel permissions 

This is identical to kern_read in KernelModule
*/
uint32_t __attribute__ ((noinline)) kern_read(const void *addr) {
	uint32_t result;
	asm volatile (
			"li 3,1\n"
			"li 4,0\n"
			"li 5,0\n"
			"li 6,0\n"
			"li 7,0\n"
			"lis 8,1\n"
			"mr 9,%1\n"
			"li 0,0x3400\n"
			"mr %0,1\n"
			"sc\n"
			"nop\n"
			"mr 1,%0\n"
			"mr %0,3\n"
	:    "=r"(result)
	:    "b"(addr)
	:    "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
	"11", "12"
	);

	return result;
}

/* Write a 32-bit word with kernel permissions */
void __attribute__ ((noinline)) kern_write(void *addr, uint32_t value) {
	asm volatile (
	"li 3,1\n"
			"li 4,0\n"
			"mr 5,%1\n"
			"li 6,0\n"
			"li 7,0\n"
			"lis 8,1\n"
			"mr 9,%0\n"
			"mr %1,1\n"
			"li 0,0x3500\n"
			"sc\n"
			"nop\n"
			"mr 1,%1\n"
	:
	:    "r"(addr), "r"(value)
	:    "memory", "ctr", "lr", "0", "3", "4", "5", "6", "7", "8", "9", "10",
	"11", "12"
	);
}

//I think this is handled by WUT and isn't needed anymore
void KernelSetupSyscalls(void) {
	//! assign 1 so that this variable gets into the retained .data section
	static uint8_t ucSyscallsSetupRequired = 1;
	if (!ucSyscallsSetupRequired)
		return;

	ucSyscallsSetupRequired = 0;

	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl1 + (0x36 * 4)), (uint32_t) KernelReadDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl2 + (0x36 * 4)), (uint32_t) KernelReadDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl3 + (0x36 * 4)), (uint32_t) KernelReadDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl4 + (0x36 * 4)), (uint32_t) KernelReadDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl5 + (0x36 * 4)), (uint32_t) KernelReadDBATs);

	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl1 + (0x37 * 4)), (uint32_t) KernelWriteDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl2 + (0x37 * 4)), (uint32_t) KernelWriteDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl3 + (0x37 * 4)), (uint32_t) KernelWriteDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl4 + (0x37 * 4)), (uint32_t) KernelWriteDBATs);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl5 + (0x37 * 4)), (uint32_t) KernelWriteDBATs);

	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl1 + (0x25 * 4)), (uint32_t) KernelCopyData);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl2 + (0x25 * 4)), (uint32_t) KernelCopyData);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl3 + (0x25 * 4)), (uint32_t) KernelCopyData);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl4 + (0x25 * 4)), (uint32_t) KernelCopyData);
	kern_write((void *) (OS_SPECIFICS->addr_KernSyscallTbl5 + (0x25 * 4)), (uint32_t) KernelCopyData);

	//! write our hook to the
	uint32_t addr_my_PrepareTitle_hook = ((u32) my_PrepareTitle_hook) | 0x48000003;
	DCFlushRange(&addr_my_PrepareTitle_hook, sizeof(addr_my_PrepareTitle_hook));

	//SC0x25_KernelCopyData((uint32_t) &origPrepareTitleInstr, (uint32_t) addr_PrepareTitle_hook, 4);
	KernelCopyData(&origPrepareTitleInstr, addr_PrepareTitle_hook,4);
	//SC0x25_KernelCopyData((uint32_t) addr_PrepareTitle_hook, (uint32_t) OSEffectiveToPhysical(&addr_my_PrepareTitle_hook), 4);
	KernelCopyData(addr_PrepareTitle_hook, OSEffectiveToPhysical(&addr_my_PrepareTitle_hook), 4);
}


void KernelRestoreInstructions(void) {
	if (origPrepareTitleInstr != 0)
		KernelCopyData((uint32_t) addr_PrepareTitle_hook, (uint32_t) &origPrepareTitleInstr, 4);
}
