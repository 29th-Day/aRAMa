#include "gecko.h"
#include "commands/commands.h"
#include "commands/memory.h"
#include "commands/system.h"
#include "commands/state.h"

#include "codeHandler.h"

#include "../arama/logger.h"

#include <coreinit/memorymap.h>

void SendStatus(const Socket socket)
{
    uint8_t status = 1;
    send(socket, &status, sizeof(status), 0);
}

void SendBufferSize(const Socket socket)
{
    uint32_t size = DATA_BUFFER_SIZE;
    send(socket, &size, sizeof(size), 0);
}

void SendCodeHandlerAddress(const Socket socket)
{
    uint32_t address = CODE_HANDLER_INSTALL_ADDRESS;
    Logger::printf("code handler address: 0x%08x", address);
    send(socket, &address, sizeof(address), 0);
}

void SendVersionHash(const Socket socket)
{
    uint32_t hash = GECKO_VERSION_HASH;
    Logger::printf("version hash: 0x%08x", hash);
    send(socket, &hash, sizeof(hash), 0);
}

void runGecko(const Socket socket)
{
    Command cmd = Command::UNKNOWN;
    while (readwait(socket, cmd))
    {
        Logger::printf("Command: 0x%02x", cmd);

        switch (cmd)
        {
        case Command::WRITE_8:
            Write8(socket);
            break;
        case Command::READ_MEMORY:
            ReadMemory(socket);
            break;
        case Command::READ_MEMORY_KERNEL:
            ReadMemoryKernel(socket);
            break;
        case Command::SERVER_STATUS:
            SendStatus(socket);
            break;
        case Command::GET_DATA_BUFFER_SIZE:
            SendBufferSize(socket);
            break;
        case Command::GET_CODE_HANDLER_ADDRESS:
            SendCodeHandlerAddress(socket);
            break;
        case Command::REMOTE_PROCEDURE_CALL:
            ExecuteProcedure(socket);
            break;
        case Command::GET_SYMBOL:
            GetSymbol(socket);
            break;
        case Command::MEMORY_SEARCH_32:
            Search32(socket);
        case Command::ADVANCED_MEMORY_SEARCH:
            AdvancedSearch(socket);
            break;
        case Command::PAUSE_CONSOLE:
            PauseConsole(socket);
            break;
        case Command::RESUME_CONSOLE:
            ResumeConsole(socket);
            break;
        case Command::IS_CONSOLE_PAUSED:
            GetConsoleState(socket);
            break;
        case Command::GET_VERSION_HASH:
            SendVersionHash(socket);
            break;
        default:
            Logger::print("Unknown cmd");
            break;
        }

        Logger::print("--   cmd finish   --");
    }
}

void InstallCodeHandler()
{

}
