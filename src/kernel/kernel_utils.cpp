#include "kernel_utils.h"

#include <kernel/kernel.h>
#include <coreinit/memorymap.h>
#include <coreinit/cache.h>

#define addrof(x) reinterpret_cast<uint32_t>(x)

void* kernel::memcpy(void* dst, const void* src, const uint32_t count)
{
    KernelCopyData(addrof(dst), addrof(src), count);
    DCFlushRange(dst, count);
    return dst;
}

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

void* kernel::physical(const void* pointer)
{
    return reinterpret_cast<void*>(OSEffectiveToPhysical(reinterpret_cast<uint32_t>(pointer)));
}

uint32_t kernel::physical(const uint32_t address)
{
    return OSEffectiveToPhysical(address);
}
