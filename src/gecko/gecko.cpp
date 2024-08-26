#include "gecko.h"
#include "commands/commands.h"
#include "commands/memory.h"
#include "commands/system.h"
#include "commands/state.h"
#include "commands/info.h"

#include "codeHandler.h"

#include "../arama/logger.h"

void skip(const Socket socket, ssize_t bytes)
{
    uint8_t tmp;
    while (bytes-- > 0)
    {
        CHECK_ERROR(readwait(socket, tmp));
    }
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
            Memory::Write<uint8_t>(socket);
            break;
        case Command::WRITE_16:
            Memory::Write<uint16_t>(socket);
            break;
        case Command::WRITE_32:
            Memory::Write<uint32_t>(socket);
            break;
        case Command::READ_MEMORY:
            Memory::Read(socket);
            break;
        case Command::READ_MEMORY_KERNEL:
            Memory::ReadKernel(socket);
            break;
        case Command::VALIDATE_ADDRESS_RANGE:
            System::ValidateAddressRange(socket);
            break;
        case Command::MEMORY_DISASSEMBLE:
            skip(socket, sizeof(uint32_t)*3);
            Logger::printf("MEMORY_DISASSEMBLE: not implemted");
            break;
        case READ_MEMORY_COMPRESSED:
            skip(socket, sizeof(uint32_t)*2);
            Logger::printf("READ_MEMORY_COMPRESSED: deprecated");
            break;
        case Command::KERNEL_WRITE:
            Memory::WriteKernel(socket);
            break;
        case Command::KERNEL_READ:
            Memory::ReadKernel(socket);
            break;
        case Command::TAKE_SCREEN_SHOT:
            Logger::printf("READ_MEMORY_COMPRESSED: not implemted");
            break;
        case Command::UPLOAD_MEMORY:
            skip(socket, sizeof(uint32_t)*2);
            // need to skip other stuff after too but idc
            Logger::printf("UPLOAD_MEMORY: not implemted");
            break;
        case Command::SERVER_STATUS:
            Info::Status(socket);
            break;
        case Command::GET_DATA_BUFFER_SIZE:
            Info::BufferSize(socket);
            break;
        case Command::READ_FILE:
            // skip "string"
            Logger::printf("READ_FILE: not implemted");
            break;
        case Command::READ_DIRECTORY:
            // skip "string"
            Logger::printf("READ_DIRECTORY: not implemted");
            break;
        case Command::REPLACE_FILE:
            // skip "string"
            Logger::printf("REPLACE_FILE: not implemted");
            break;
        case Command::GET_CODE_HANDLER_ADDRESS:
            Info::CodeHandlerAddress(socket);
            break;
        case Command::READ_THREADS:
            Logger::printf("READ_THREADS: not implemted");
            break;
        case Command::ACCOUNT_IDENTIFIER:
            Logger::printf("ACCOUNT_IDENTIFIER: not implemted");
            break;
        case Command::FOLLOW_POINTER:
            skip(socket, sizeof(uint32_t)*2);
            // need to skip other stuff after too but idc
            Logger::printf("FOLLOW_POINTER: not implemted");
            break;
        case Command::REMOTE_PROCEDURE_CALL:
            System::ExecuteProcedure(socket);
            break;
        case Command::GET_SYMBOL:
            System::GetSymbol(socket);
            break;
        case Command::MEMORY_SEARCH_32:
            Memory::Search32(socket);
            break;
        case Command::ADVANCED_MEMORY_SEARCH:
            Memory::SearchEx(socket);
            break;
        case Command::EXECUTE_ASSEMBLY:
            // skip "string"
            Logger::printf("EXECUTE_ASSEMBLY: not implemted");
            break;
        case Command::PAUSE_CONSOLE:
            ConsoleState::Pause(socket);
            break;
        case Command::RESUME_CONSOLE:
            ConsoleState::Resume(socket);
            break;
        case Command::IS_CONSOLE_PAUSED:
            ConsoleState::GetState(socket);
            break;
        case Command::SERVER_VERSION:
            Logger::printf("SERVER_VERSION: not implemted");
            break;
        case Command::GET_OS_VERSION:
            Logger::printf("GET_OS_VERSION: not implemted");
            break;
        case Command::SET_DATA_BREAKPOINT:
            skip(socket, sizeof(uint32_t) + sizeof(bool)*2);
            Logger::printf("SET_DATA_BREAKPOINT: not implemted");
            break;
        case Command::SET_INSTRUCTION_BREAKPOINT:
            skip(socket, sizeof(uint32_t));
            Logger::printf("SET_INSTRUCTION_BREAKPOINT: not implemted");
            break;
        case Command::TOGGLE_BREAKPOINT:
            skip(socket, sizeof(uint32_t));
            Logger::printf("TOGGLE_BREAKPOINT: not implemted");
            break;
        case Command::REMOVE_ALL_BREAKPOINTS:
            Logger::printf("REMOVE_ALL_BREAKPOINTS: not implemted");
            break;
        case Command::POKE_REGISTERS:
            skip(socket, 4 * 32 + 8 * 32);
            Logger::printf("POKE_REGISTERS: not implemted");
            break;
        case Command::GET_STACK_TRACE:
            Logger::printf("GET_STACK_TRACE: not implemted");
            break;
        case Command::GET_ENTRY_POINT_ADDRESS:
            Logger::printf("GET_ENTRY_POINT_ADDRESS: not implemted");
            break;
        case Command::RUN_KERNEL_COPY_SERVICE:
            Logger::printf("RUN_KERNEL_COPY_SERVICE: not implemted");
            break;
        case Command::IOSU_HAX_READ_FILE:
            Logger::printf("IOSU_HAX_READ_FILE: not implemted");
            break;
        case Command::GET_VERSION_HASH:
            Info::VersionHash(socket);
            break;
        case Command::PERSIST_ASSEMBLY:
            // skip "string"
            Logger::printf("PERSIST_ASSEMBLY: not implemted");
            break;
        case Command::CLEAR_ASSEMBLY:
            Logger::printf("CLEAR_ASSEMBLY: not implemted");
            break;
        default:
            Logger::print("Unknown cmd");
            break;
        }

        Logger::print("--   cmd finish   --");
    }
}
