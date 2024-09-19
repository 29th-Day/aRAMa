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

#define PPC_DISASM_MAX_BUFFER 64

// local methods

void extractMemory(const Socket* socket, uint32_t start, uint32_t end, bool useKernel)
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
        // CHECK_ERROR(send(socket, buffer, length+1, 0) > 0);
        CHECK_ERROR(socket->send(buffer, length+1));

        start += length;
    }
}

// ---

void Memory::Read(const Socket* socket)
{
    uint32_t start = 0, end = 0;

    CHECK_ERROR(socket->recv(start));
    CHECK_ERROR(socket->recv(end));

    Logger::printf("%s | %p - %p (%i)",
        __FUNCTION__, start, end, end - start);

    extractMemory(socket, start, end, false);
}

template <typename T>
void Memory::Write(const Socket* socket)
{
    uint8_t* ptr = nullptr;
    uint32_t value = 0;

    CHECK_ERROR(socket->recv(ptr));
    CHECK_ERROR(socket->recv(value));

    Logger::printf("Write%u: 0x%08x  <- 0x%02x", sizeof(T) * 8, ptr, value);

    *ptr = static_cast<T>(value);
    DCFlushRange(ptr, sizeof(T));
}

template void Memory::Write<uint8_t>(const Socket* socket);
template void Memory::Write<uint16_t>(const Socket* socket);
template void Memory::Write<uint32_t>(const Socket* socket);

void Memory::ReadKernel(const Socket* socket)
{
    uint32_t start = 0, end = 0, useKernel = 0;

    CHECK_ERROR(socket->recv(start));
    CHECK_ERROR(socket->recv(end));
    CHECK_ERROR(socket->recv(useKernel));

    Logger::printf("%s | %p - %p (%i) [kernel: %s]",
        __FUNCTION__, start, end, end - start, btos(useKernel));

    extractMemory(socket, start, end, useKernel);
}

void Memory::WriteKernel(const Socket* socket)
{
    uint32_t* address;
    uint32_t value;

    CHECK_ERROR(socket->recv(address));
    CHECK_ERROR(socket->recv(value));

    Logger::printf("%s | address: 0x%08x <- 0x%08x", __FUNCTION__, address, value);

    kernel::memcpy(address, kernel::physical(&value), sizeof(value));
}

void Memory::Search32(const Socket* socket)
{
    uint32_t startAddress = 0, length = 0, found = 0;
    int32_t value;

    CHECK_ERROR(socket->recv(startAddress));
    CHECK_ERROR(socket->recv(value));
    CHECK_ERROR(socket->recv(length));

    for (uint32_t index = startAddress; index < startAddress + length; index += sizeof(index))
    {
        // Logger::printf("%p -> 0x%08x", index, *reinterpret_cast<int32_t*>(index));
        if (*reinterpret_cast<int32_t*>(index) == value)
        {
            found = index;
            break;
        }
    }

    // send(socket, &found, sizeof(found), 0);
    socket->send(found);
}

void Memory::SearchEx(const Socket* socket)
{
    uint32_t startAddress, length, useKernel, maxResults, aligned, patternLength;

    CHECK_ERROR(socket->recv(startAddress));
    CHECK_ERROR(socket->recv(length));
    CHECK_ERROR(socket->recv(useKernel));
    CHECK_ERROR(socket->recv(maxResults));
    CHECK_ERROR(socket->recv(aligned));
    CHECK_ERROR(socket->recv(patternLength));

    Logger::printf("startAddress: 0x%08x", startAddress);
    Logger::printf("length: 0x%08x", length);
    Logger::printf("useKernel: 0x%08x", useKernel);
    Logger::printf("maxResults: 0x%08x", maxResults);
    Logger::printf("aligned: 0x%08x", aligned);
    Logger::printf("patternLength: 0x%08x", patternLength);

    // auto pattern = std::make_unique<uint8_t[]>(patternLength);
    // auto pattern = std::vector<uint8_t>();
    std::vector<uint8_t> pattern;
    pattern.reserve(patternLength);
    // CHECK_ERROR(read(socket, pattern.get(), patternLength) == patternLength);
    CHECK_ERROR(socket->recv(pattern.data(), patternLength));

    std::vector<uint32_t> found;
    // found.reserve(maxResults); // idk, maybe 

    uint32_t increment = aligned ? patternLength : 1;
    bool same = false;

    for (uint32_t address = startAddress; address < startAddress + length; address += increment)
    {
        if (useKernel)
        {
            same = kernel::memcmp(
                reinterpret_cast<void*>(address),
                kernel::physical(pattern.data()), patternLength) == 0;
        }
        else
        {
            same = memcmp(
                reinterpret_cast<void*>(address),
                pattern.data(), patternLength) == 0;
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

    // uint32_t bytes = static_cast<uint32_t>(found.size()) * 4;
    // send(socket, &bytes, sizeof(bytes), 0);

    // insert number of elements (in bytes -> *4) in the beginning
    found.insert(found.begin(), found.size() * sizeof(uint32_t));
    // send(socket, found.data(), found.size() * sizeof(uint32_t), 0);
    socket->send(found.data(), found.size() * sizeof(uint32_t));
}

struct OpCode
{
    uint32_t value;
    bool status;
    uint32_t length;
    char buffer[PPC_DISASM_MAX_BUFFER];
};

void Memory::Disassemble(const Socket* socket)
{
    uint32_t* start;
    uint32_t* end;
    uint32_t idk;

    CHECK_ERROR(socket->recv(start));
    CHECK_ERROR(socket->recv(end));
    CHECK_ERROR(socket->recv(idk));

    Logger::printf("%s", __FUNCTION__);

    auto opCodes = std::vector<OpCode>();
    for (uint32_t* current = start; current < end; current += sizeof(uint32_t))
    {
        OpCode op = OpCode();

        op.status = DisassemblePPCOpcode(
            current,
            op.buffer,
            PPC_DISASM_MAX_BUFFER,
            OSGetSymbolName,
            DISASSEMBLE_PPC_FLAGS_NONE);

        if (op.status)
            op.length = strnlen(op.buffer, PPC_DISASM_MAX_BUFFER);

        opCodes.push_back(op);
    }

    socket->send(opCodes.size());
    for (auto op: opCodes)
    {
        uint32_t bytes = sizeof(op.value) + sizeof(op.status);

        if (op.status)
            bytes += sizeof(op.length) + op.length;

        socket->send(&op, bytes);
    }
}

void Memory::Upload(const Socket* socket)
{
    uint8_t* current;
    uint8_t* end;

    CHECK_ERROR(socket->recv(current));
    CHECK_ERROR(socket->recv(end));

    uint8_t buffer[DATA_BUFFER_SIZE] = { 0 };

    for (uint32_t length = 0; current < end; current += length)
    {
        length = end - current;
        if (length > DATA_BUFFER_SIZE)
            length = DATA_BUFFER_SIZE;

        CHECK_ERROR(socket->recv(buffer, length));
        kernel::memcpy(current, kernel::physical(buffer), length);
    }

}
