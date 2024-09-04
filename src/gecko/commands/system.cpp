#include "system.h"

#include "../../arama/logger.h"
#include "../../kernel/kernel_utils.h"

#include <coreinit/dynload.h>
#include <coreinit/title.h>
#include <coreinit/memorymap.h>
#include <coreinit/thread.h>
#include <coreinit/interrupts.h>
#include <coreinit/scheduler.h>

#include <memory>
#include <vector>

struct SymbolOffset
{
    uint32_t Module;
    uint32_t Symbol;
};

typedef int64_t GenericFunction(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);

void System::GetSymbol(const Socket* socket)
{
    uint8_t size, type;
    SymbolOffset offset;

    CHECK_ERROR(socket->recv(size));
    CHECK_ERROR(socket->recv(offset));
    size -= sizeof(SymbolOffset);
    // account for reduced offset
    offset.Symbol -= offset.Module;
    offset.Module = 0;

    // auto buffer = std::make_unique<char[]>(size);
    std::vector<char> buffer;
    buffer.reserve(size);

    CHECK_ERROR(socket->recv(buffer.data(), size));
    CHECK_ERROR(socket->recv(type));

    char* module = &buffer[offset.Module];
    char* symbol = &buffer[offset.Symbol];

    uint32_t address = 0;
    OSDynLoad_Module osModule;
    OSDynLoad_Error error = OS_DYNLOAD_OK;
    error = OSDynLoad_Acquire(module, &osModule);
    error = OSDynLoad_FindExport(osModule, (OSDynLoad_ExportType)type, symbol, (void**)&address);
    OSDynLoad_Release(osModule);

    Logger::printf("%s::%s = 0x%08x", module, symbol, address);

    // send(socket, &address, sizeof(address), 0);
    socket->send(address);
}

void System::ExecuteProcedure(const Socket* socket)
{
    GenericFunction* function;
    uint32_t args[8];

    CHECK_ERROR(socket->recv(function));
    // CHECK_ERROR(readwait(socket, args));
    // CHECK_ERROR(read(socket, args, sizeof(args)) == sizeof(args));
    CHECK_ERROR(socket->recv(args, sizeof(args)));

    int64_t result = function(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);

    Logger::printf("0x%016llx = %p(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)", result, function, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);

    // send(socket, &result, sizeof(result), 0);
    socket->send(result);
}

void System::ValidateAddressRange(const Socket* socket)
{
    uint32_t start, end;

    CHECK_ERROR(socket->recv(start));
    CHECK_ERROR(socket->recv(end));

    bool valid = __OSValidateAddressSpaceRange(1, start, end - start + 1);

    // send(socket, &valid, sizeof(valid), 0);
    socket->send(valid);
}

#define INVALID_ADDRESS -1

void System::FollowPointer(const Socket* socket)
{
    uint32_t address, offsetsCount;

    CHECK_ERROR(socket->recv(address));
    CHECK_ERROR(socket->recv(offsetsCount));

    auto offsets = std::vector<uint32_t>(offsetsCount);
    CHECK_ERROR(socket->recv(offsets.data(), offsetsCount * sizeof(uint32_t)));

    // uint32_t destination = address;

    if (OSIsAddressValid(address))
    {
        for (const auto offset: offsets)
        {
            uint32_t value = *reinterpret_cast<uint32_t*>(address);
            address = value + offset;

            if (!OSIsAddressValid(address))
            {
                address = INVALID_ADDRESS;
                break;
            }
        }
    }
    else // if (offsetsCount > 0)
    {
        address = INVALID_ADDRESS;
    }

    // send(socket, &address, sizeof(address), 0);
    socket->send(address);
}

void System::ExecuteAssembly(const Socket* socket)
{
    uint32_t length;
    CHECK_ERROR(socket->recv(length));

    auto data = std::vector<uint8_t>(length);
    CHECK_ERROR(socket->recv(data.data(), length * sizeof(uint8_t)));

    kernel::execute(data.data(), length);
}

void System::GetThreads(const Socket* socket)
{
    uint32_t count = OSCheckActiveThreads();

    Logger::printf("%s | count: %u", __FUNCTION__, count);

    if (count <= 0)
    {
        socket->send(0);
        return;
    }

    std::vector<OSThread*> threads;
    threads.reserve(count);

    OSThread* currentThread = OSGetCurrentThread();
    OSThread* thread = OSGetDefaultThread(1);

    bool state = OSDisableInterrupts();
    __OSLockScheduler(currentThread);

    for (uint32_t i = 0; thread && i < count; i++)
    {
        threads.push_back(thread);
        thread = thread->activeLink.next;
    }

    __OSUnlockScheduler(currentThread);
    OSRestoreInterrupts(state);

    // int i = 0;
    // Logger::printf("Thread List:");

    socket->send(threads.size());

    // this works and is (imho) more sensible then the tcpGecko stuff
    // send(socket, threads.data(), threads.size() * sizeof(OSThread*), 0);
}
