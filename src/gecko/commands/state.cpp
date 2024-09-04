#include "state.h"

#include "../../arama/logger.h"
#include "../../kernel/kernel_utils.h"

#include <avm/drc.h>
#include <coreinit/memorymap.h>
#include <coreinit/cache.h>

// local 

// I dont know wtf this magic does
#define patchAddress reinterpret_cast<void*>(reinterpret_cast<uint32_t>(AVMGetDRCScanMode) + 0x44)

// ---

void ConsoleState::Set(uint32_t state)
{
    Logger::printf("address: 0x%08x", patchAddress);
    kernel::memcpy(patchAddress, kernel::physical(&state), sizeof(state));
}

void ConsoleState::Get(const Socket* socket)
{
    uint32_t state = 0;

    kernel::memcpy(kernel::physical(&state), patchAddress, sizeof(state));

    Logger::printf("current state: 0x%08x", state);

    // bool tmp = state == ConsoleState::PAUSED;
    // send(socket, &tmp, sizeof(uint8_t), 0);
    socket->send((state == ConsoleState::PAUSED));
}
