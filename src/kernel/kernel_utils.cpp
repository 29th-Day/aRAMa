#include "kernel_utils.h"

#include <kernel/kernel.h>
#include <coreinit/memorymap.h>
#include <coreinit/cache.h>

#include "../arama/logger.h"

#include <algorithm>

#define addrof(x) reinterpret_cast<uint32_t>(x)

/**
 * Copies a specified number of bytes from the source memory location to the destination memory location via kernel. Similar to `std::memcpy`.
 *
 * @attention Function requires physical addresses!
 *
 * @param dst Pointer to the destination.
 * @param src Pointer to the source.
 * @param count Number of bytes to copy.
 * @return Pointer to the destination memory location.
 */
void* kernel::memcpy(void* dst, const void* src, const uint32_t count)
{
    KernelCopyData(addrof(dst), addrof(src), count);
    // DCFlushRange(dst, count); // this is in TcpGecko, but idk if it's needed
    return dst;
}

/**
 * Compares memory blocks. Similar to `std::memcmp`.
 *
 * @param lhs: Pointer to the first memory block.
 * @param rhs: Pointer to the second memory block.
 * @param count: Number of bytes to compare.
 * @return `0` if all bytes are equal. `-1` if first differing byte of `lhs` is smaller than `rhs` else `1`.
 */
int kernel::memcmp(const void* lhs, const void* rhs, uint32_t count)
{

    const uint8_t* l = reinterpret_cast<const uint8_t*>(lhs);
    const uint8_t* r = reinterpret_cast<const uint8_t*>(rhs);

    uint8_t buffer[2] = { 0, 0 };
    uint32_t bufferAddress = reinterpret_cast<uint32_t>(kernel::physical(buffer));

    while (count-- > 0)
    {
        KernelCopyData(bufferAddress, addrof(l++), sizeof(uint8_t));
        KernelCopyData(bufferAddress + 1, addrof(r++), sizeof(uint8_t));

        if (buffer[0] < buffer[1])
            return -1;
        if (buffer[0] > buffer[1])
            return 1;
    }

    return 0;
}

/**
 * @brief Convert local / effective pointer to physical pointer.
 *
 * @param pointer Effective pointer.
 * @return Physical pointer.
 */
void* kernel::physical(const void* pointer)
{
    return reinterpret_cast<void*>(OSEffectiveToPhysical(reinterpret_cast<uint32_t>(pointer)));
}

/**
 * @brief Convert local / effective memory address to physical memory address.
 *
 * @param address Effective address.
 * @return Physical address.
 */
uint32_t kernel::physical(const uint32_t address)
{
    return OSEffectiveToPhysical(address);
}

/**
 * @brief Execute raw assembly???? NOT TESTED!
 *
 * @param assembly Raw assembly block.
 * @param size Size of block.
 */
void kernel::execute(uint8_t* assembly, uint32_t size)
{
    // write assembly to executable region
    void* address = reinterpret_cast<void*>(0x1000'0000 - size);
    memcpy(address, assembly, size);

    // cast to function and execute
    reinterpret_cast<void(*)()>(address)();

    std::fill_n(assembly, size, 0);
    memcpy(address, assembly, size);
}
