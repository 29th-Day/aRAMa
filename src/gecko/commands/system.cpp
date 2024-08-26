#include "system.h"

#include "../../arama/logger.h"

#include <coreinit/dynload.h>
#include <coreinit/title.h>
#include <coreinit/memorymap.h>

#include <memory>

struct SymbolOffset
{
    uint32_t Module;
    uint32_t Symbol;
};

typedef int64_t GenericFunction(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);

void System::GetSymbol(const Socket socket)
{
    uint8_t size = 0;
    uint8_t type = 0;
    SymbolOffset offset;

    CHECK_ERROR(readwait(socket, size));
    CHECK_ERROR(readwait(socket, offset));
    size -= sizeof(SymbolOffset);
    // account for reduced offset
    offset.Symbol -= offset.Module;
    offset.Module = 0;

    auto buffer = std::make_unique<char[]>(size);
    CHECK_ERROR(size == read(socket, buffer.get(), size));

    CHECK_ERROR(readwait(socket, type));

    char* module = &buffer[offset.Module];
    char* symbol = &buffer[offset.Symbol];

    uint32_t address = 0;
    OSDynLoad_Module osModule;
    OSDynLoad_Error error = OS_DYNLOAD_OK;
    error = OSDynLoad_Acquire(module, &osModule);
    error = OSDynLoad_FindExport(osModule, (OSDynLoad_ExportType)type, symbol, (void**)&address);
    OSDynLoad_Release(osModule);

    Logger::printf("%s::%s = 0x%08x", module, symbol, address);

    send(socket, &address, sizeof(address), 0);
}

void System::ExecuteProcedure(const Socket socket)
{
    GenericFunction* function;
    uint32_t args[8];

    CHECK_ERROR(readwait(socket, function));
    // CHECK_ERROR(readwait(socket, args));
    // CHECK_ERROR(read(socket, args, sizeof(args)) == sizeof(args));
    CHECK_ERROR(readwait(socket, args, sizeof(args)));

    int64_t result = function(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);

    Logger::printf("0x%016llx = %p(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)", result, function, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);

    send(socket, &result, sizeof(result), 0);
}

void System::ValidateAddressRange(const Socket socket)
{
    uint32_t start = 0, end = 0;

    CHECK_ERROR(readwait(socket, start));
    CHECK_ERROR(readwait(socket, end));

    bool valid = __OSValidateAddressSpaceRange(1, start, end - start + 1);

    send(socket, &valid, sizeof(valid), 0);
}
