#include "state.h"

#include "../../arama/logger.h"
#include "../../kernel/kernel_utils.h"

#include <coreinit/memorymap.h>
#include <coreinit/cache.h>

// MAGIC!!!! Idk why or what but it works
#define patchAddress reinterpret_cast<void*>(0x0113F4C4) // = AVMDrcScanMode + 0x44

void ConsoleState::Set(State state)
{
    Logger::printf("ConsoleState::Set(%s)", (state == ConsoleState::PAUSED) ? "PAUSE" : "RESUME");

    uint32_t value = 0;

    kernel::memcpy(kernel::physical(patchAddress), kernel::physical(&state), sizeof(state));
    DCFlushRange(patchAddress, sizeof(state));
}

void ConsoleState::Get(const Socket* socket)
{
    Logger::print("ConsoleState::Get");

    uint32_t state = 0;

    kernel::memcpy(kernel::physical(&state), kernel::physical(patchAddress), sizeof(state));

    socket->send((state == ConsoleState::PAUSED));
}
