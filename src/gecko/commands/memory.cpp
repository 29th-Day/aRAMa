#include "memory.h"

#include "../../arama/logger.h"
#include "../../kernel/kernel_utils.h"

#include <coreinit/cache.h>
#include <coreinit/memorymap.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <cstring>

#define ONLY_ZEROES_FOUND 0xB0
#define NON_ZEROES_FOUND 0xBD

// local methods

void extractMemory(const Socket socket, uint32_t start, uint32_t end, bool useKernel)
{
    uint8_t buffer[DATA_BUFFER_SIZE] = { 0 };
    void* bufferAddress = kernel::physical(buffer+1);

    uint32_t length = 0;
    while (start < end)
    {
        length = std::min(end - start, DATA_BUFFER_SIZE);

        if (useKernel)
        {
            kernel::memcpy(bufferAddress,
                reinterpret_cast<void*>(start), length);
        }
        else
        {
            memcpy(buffer+1, reinterpret_cast<void*>(start), length);
        }

        Logger::printf("length: %u / 0x%08x", length, length);
        Logger::printf("0x%08x - 0x%08x", start, start + length);
        Logger::printf("[1]: 0x%02x, [%u]: 0x%02x", buffer[1], length, buffer[length]);

        buffer[0] = NON_ZEROES_FOUND;
        CHECK_ERROR(send(socket, buffer, length+1, 0) > 0);

        start += length;
    }
}

bool validateRange(uint32_t start, uint32_t end)
{
    return __OSValidateAddressSpaceRange(1, start, end - start + 1);
}

// ---

void Write8(const Socket socket)
{
    uint32_t address = 0;
    int32_t value = 0;
    CHECK_ERROR(readwait(socket, address));
    CHECK_ERROR(readwait(socket, value));

    Logger::printf("Write8: %p  <- %i (0x%x)", address, value, OSEffectiveToPhysical(address));

    // KernelCopyData(OSEffectiveToPhysical((uint32_t)ptr), (uint32_t)ptr, 1);
    // DCFlushRange(ptr, 1);
}

void ReadMemory(const Socket socket)
{
    uint32_t start = 0, end = 0;
    uint8_t buffer[DATA_BUFFER_SIZE] = { 0 };

    CHECK_ERROR(readwait(socket, start));
    CHECK_ERROR(readwait(socket, end));

    Logger::printf("%p - %p (%i) [%p - %p]",
        start, end, end - start);

    extractMemory(socket, start, end, false);
}

void ReadMemoryKernel(const Socket socket)
{
    uint32_t start = 0, end = 0, useKernel = 0;

    CHECK_ERROR(readwait(socket, start));
    CHECK_ERROR(readwait(socket, end));
    CHECK_ERROR(readwait(socket, useKernel));

    Logger::printf("%p - %p (%i)",
        start, end, end - start);

    extractMemory(socket, start, end, static_cast<bool>(useKernel));
}

void ValidateAddressRange(const Socket socket)
{
    uint32_t start = 0, end = 0;

    CHECK_ERROR(readwait(socket, start));
    CHECK_ERROR(readwait(socket, end));

    bool valid = validateRange(start, end);

    send(socket, &valid, sizeof(valid), 0);
}

void Search32(const Socket socket)
{
    uint32_t startAddress = 0, length = 0, found = 0;
    int32_t value;

    CHECK_ERROR(readwait(socket, startAddress));
    CHECK_ERROR(readwait(socket, value));
    CHECK_ERROR(readwait(socket, length));

    if (validateRange(startAddress, startAddress+length))
    {
        Logger::print("valid address");
        // normal copy / pointer
    }

    if (OSIsAddressValid(startAddress) && OSIsAddressValid(startAddress + length))
    {
        Logger::print("valid address");
        // normal copy / pointer
    }
    else
    {
        Logger::print("invalid address (likely out of bound for memory area?)");
        // need kernel copy
    }

    for (uint32_t index = startAddress; index < startAddress + length; index += sizeof(index))
    {
        // Logger::printf("%p -> 0x%08x", index, *reinterpret_cast<int32_t*>(index));
        if (*reinterpret_cast<int32_t*>(index) == value)
        {
            found = index;
            break;
        }
    }

    send(socket, &found, sizeof(found), 0);
}

void AdvancedSearch(const Socket socket)
{
    uint32_t startAddress, length, useKernel, maxResults, aligned, patternLength;

    CHECK_ERROR(readwait(socket, startAddress));
    CHECK_ERROR(readwait(socket, length));
    CHECK_ERROR(readwait(socket, useKernel));
    CHECK_ERROR(readwait(socket, maxResults));
    CHECK_ERROR(readwait(socket, aligned));
    CHECK_ERROR(readwait(socket, patternLength));

    Logger::printf("startAddress: 0x%08x", startAddress);
    Logger::printf("length: 0x%08x", length);
    Logger::printf("useKernel: 0x%08x", useKernel);
    Logger::printf("maxResults: 0x%08x", maxResults);
    Logger::printf("aligned: 0x%08x", aligned);
    Logger::printf("patternLength: 0x%08x", patternLength);

    auto pattern = std::make_unique<uint8_t[]>(patternLength);
    CHECK_ERROR(read(socket, pattern.get(), patternLength) == patternLength);

    std::vector<uint32_t> found;

    uint32_t increment = aligned ? patternLength : 1;
    bool same = false;

    for (uint32_t address = startAddress; address < startAddress + length; address += increment)
    {
        if (useKernel)
        {
            same = kernel::memcmp(
                reinterpret_cast<void*>(address),
                kernel::physical(pattern.get()), patternLength) == 0;
        }
        else
        {
            same = memcmp(
                reinterpret_cast<void*>(address),
                pattern.get(), patternLength) == 0;
        }

        if (same)
        {
            found.push_back(address);

            if (found.size() == maxResults)
                break;
        }
    }

    Logger::printf("#found: %u", found.size());
    for (const uint32_t& a : found)
        Logger::printf("found: 0x%08x", a);

    uint32_t bytes = static_cast<uint32_t>(found.size()) * 4;
    send(socket, &bytes, sizeof(bytes), 0);
    send(socket, reinterpret_cast<uint8_t*>(found.data()),
        bytes, 0);
}
