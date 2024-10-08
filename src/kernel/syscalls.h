#ifndef __SYSCALLS_H_
#define __SYSCALLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>
#include "../common/kernel_defs.h"

void KernelSetupSyscalls(void);

void KernelRestoreInstructions(void);

//KernelCopyData in libkernel
void SC0x25_KernelCopyData(uint32_t addr, uint32_t src, uint32_t len);

//KernelReadSRs in libkernel
void SC0x36_KernelReadDBATs(bat_table_t *table);

void SC0x37_KernelWriteDBATs(bat_table_t *table);

uint32_t __attribute__ ((noinline)) kern_read(const void *addr);

void __attribute__ ((noinline)) kern_write(void *addr, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif