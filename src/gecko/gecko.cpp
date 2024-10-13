#include "gecko.h"
#include "commands/commands.h"
#include "commands/memory.h"
#include "commands/system.h"
#include "commands/state.h"
#include "commands/fs.h"
#include "commands/os.h"

#include "version.h"
#include "../geckU/geckU.h"
#include "copyService.h"

// #include <avm/drc.h>

#include <vector>

#include "../arama/logger.h"

void skip(const Socket* socket, ssize_t bytes)
{
    uint8_t tmp;
    while (bytes-- > 0)
    {
        CHECK_ERROR(socket->recv(tmp));
    }
}

constexpr uint32_t CONTAINER = 0x1506B501;
constexpr uint32_t HEALTH = 0x1506B503;

void runGecko(const Socket socket)
{
    Command cmd = Command::UNKNOWN;
    while (socket.recv(cmd))
    {
        Logger::printf("\nCommand: 0x%02x", cmd);

        switch (cmd)
        {
        case Command::WRITE_8:
            Memory::Write<uint8_t>(&socket);
            break;
        case Command::WRITE_16:
            Memory::Write<uint16_t>(&socket);
            break;
        case Command::WRITE_32:
            Memory::Write<uint32_t>(&socket);
            break;
        case Command::READ_MEMORY:
            Memory::Read(&socket);
            break;
        case Command::READ_MEMORY_KERNEL:
            Memory::ReadKernel(&socket);
            break;
        case Command::VALIDATE_ADDRESS_RANGE:
            System::ValidateAddressRange(&socket);
            break;
        case Command::MEMORY_DISASSEMBLE:
            Logger::printf("MEMORY_DISASSEMBLE: not implemted");
            // doesnt work i think
            // at least different than tcpGecko
            // Memory::Disassemble(&socket);
            break;
        case READ_MEMORY_COMPRESSED:
            skip(&socket, sizeof(uint32_t)*2);
            Logger::printf("READ_MEMORY_COMPRESSED: deprecated");
            break;
        case Command::KERNEL_WRITE:
            Memory::WriteKernel(&socket);
            break;
        case Command::KERNEL_READ:
            Memory::ReadKernel(&socket);
            break;
        case Command::TAKE_SCREEN_SHOT:
            Logger::printf("TAKE_SCREEN_SHOT: not implemted");
            // OS::TakeScreenshot(&socket);
            break;
        case Command::UPLOAD_MEMORY:
            Memory::Upload(&socket);
            break;
        case Command::SERVER_STATUS:
            socket.send<uint8_t>(1);
            break;
        case Command::GET_DATA_BUFFER_SIZE:
            socket.send(DATA_BUFFER_SIZE);
            break;
        case Command::READ_FILE:
            FS::ReadFile(&socket);
            break;
        case Command::READ_DIRECTORY:
            FS::ReadDirectory(&socket);
            break;
        case Command::REPLACE_FILE:
            FS::WriteFile(&socket);
            break;
        case Command::GET_CODE_HANDLER_ADDRESS:
        {
            // Logger::printf("handler address: 0x%08x", CodeHandler::);
            socket.send(CODE_HANDLER_INSTALL_ADDRESS);
            break;
        }
        case Command::READ_THREADS:
            // Logger::printf("READ_THREADS: not implemted");
            OS::GetThreads(&socket);
            break;
        case Command::ACCOUNT_IDENTIFIER:
            OS::AccountIdentifier(&socket);
            break;
        case Command::FOLLOW_POINTER: // TODO
            // I dont quite get this but ...
            // TEST: not tested
            System::FollowPointer(&socket);
            break;
        case Command::REMOTE_PROCEDURE_CALL:
            System::ExecuteProcedure(&socket);
            break;
        case Command::GET_SYMBOL:
            System::GetSymbol(&socket);
            break;
        case Command::MEMORY_SEARCH_32:
            Memory::Search32(&socket);
            break;
        case Command::ADVANCED_MEMORY_SEARCH:
            Memory::SearchEx(&socket);
            break;
        case Command::EXECUTE_ASSEMBLY: // TODO
            Logger::printf("EXECUTE_ASSEMBLY: not implemted");
            // not yet tested
            // System::ExecuteAssembly(&socket);
            break;
        case Command::PAUSE_CONSOLE:
            ConsoleState::Set(ConsoleState::PAUSED);
            break;
        case Command::RESUME_CONSOLE:
            ConsoleState::Set(ConsoleState::RUNNING);
            break;
        case Command::IS_CONSOLE_PAUSED:
            ConsoleState::Get(&socket);
            break;
        case Command::SERVER_VERSION:
            socket.send(GECKO_SERVER_VERSION, sizeof(GECKO_SERVER_VERSION));
            break;
        case Command::GET_OS_VERSION:
            // not tcpGecko compliant but no magic and contains more information
            OS::Version(&socket);
            break;
        case Command::SET_DATA_BREAKPOINT: // TODO
            skip(&socket, sizeof(uint32_t) + sizeof(bool)*2);
            Logger::printf("SET_DATA_BREAKPOINT: not implemted");
            break;
        case Command::SET_INSTRUCTION_BREAKPOINT: // TODO
            skip(&socket, sizeof(uint32_t));
            Logger::printf("SET_INSTRUCTION_BREAKPOINT: not implemted");
            break;
        case Command::TOGGLE_BREAKPOINT: // TODO
            skip(&socket, sizeof(uint32_t));
            Logger::printf("TOGGLE_BREAKPOINT: not implemted");
            break;
        case Command::REMOVE_ALL_BREAKPOINTS: // TODO
            Logger::printf("REMOVE_ALL_BREAKPOINTS: not implemted");
            break;
        case Command::POKE_REGISTERS: // TODO
            skip(&socket, 4 * 32 + 8 * 32);
            Logger::printf("POKE_REGISTERS: not implemted");
            break;
        case Command::GET_STACK_TRACE: // TODO
            Logger::printf("GET_STACK_TRACE: not implemted");
            break;
        case Command::GET_ENTRY_POINT_ADDRESS: // TODO
            Logger::printf("GET_ENTRY_POINT_ADDRESS: not implemted");
            break;
        case Command::RUN_KERNEL_COPY_SERVICE:
            // Logger::printf("RUN_KERNEL_COPY_SERVICE: not implemted");
            CopyService::start();
            break;
        case Command::IOSU_HAX_READ_FILE: // TODO
            Logger::printf("IOSU_HAX_READ_FILE: not implemted");
            break;
        case Command::GET_VERSION_HASH:
            socket.send<uint32_t>(GECKO_VERSION_HASH);
            break;
        case Command::PERSIST_ASSEMBLY: // TODO
            // skip "string"
            Logger::printf("PERSIST_ASSEMBLY: not implemted");
            break;
        case Command::CLEAR_ASSEMBLY: // TODO
            Logger::printf("CLEAR_ASSEMBLY: not implemted");
            break;
        default:
            Logger::print("Unknown cmd");
            break;
        }

        Logger::print("--   cmd finish   --");
    }
}
