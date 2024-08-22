#include "state.h"

#include "../../arama/logger.h"
#include "../../kernel/kernel_utils.h"

#include <avm/drc.h>
#include <coreinit/memorymap.h>
#include <coreinit/cache.h>

// local 

// I need help...or sleep...whatever
#define patchAddress reinterpret_cast<void*>(reinterpret_cast<uint32_t>(AVMGetDRCScanMode) + 0x44)

// ---

void PauseConsole(const Socket socket)
{
    uint32_t state = ConsoleState::PAUSED;
    kernel::memcpy(patchAddress, kernel::physical(&state), sizeof(state));
}

void ResumeConsole(const Socket socket)
{
    uint32_t state = ConsoleState::RUNNING;
    kernel::memcpy(patchAddress, kernel::physical(&state), sizeof(state));
}

void GetConsoleState(const Socket socket)
{
    uint32_t state = 0;

    kernel::memcpy(kernel::physical(&state), patchAddress, sizeof(state));

    Logger::printf("current state: 0x%08x", state);

    uint8_t tmp = static_cast<uint8_t>(state == ConsoleState::PAUSED);
    send(socket, &tmp, sizeof(uint8_t), 0);
}
