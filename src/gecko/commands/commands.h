#pragma once

#include <cstdint>

#define DATA_BUFFER_SIZE (uint32_t)0x5000

#define CHECK_ERROR(x) if (!(x)) return

enum Command : uint8_t
{
    /**
     * @param uint32_t address to write to
     * @param int32_t last byte get set at address
     */
    WRITE_8 = 0x01,

    /**
     * @param uint32_t address to write to
     * @param int32_t last 2 bytes get set at address
     */
    WRITE_16 = 0x02,

    /**
     * @param uint32_t address to write to
     * @param int32_t bytes get set at address
     */
    WRITE_32 = 0x03,

    /**
     * @param uint32_t start address
     * @param uint32_t end address
     *
     * @return `uint8_t[N]` - memory dump
     */
    READ_MEMORY = 0x04,

    /**
     * @param uint32_t start address
     * @param uint32_t end address
     * @param uint32_t use kernel to read memory
     *
     * @return `uint8_t[N]` - memory dump
     */
    READ_MEMORY_KERNEL = 0x05,

    /**
     * @param uint32_t start address
     * @param uint32_t end address
     *
     * @return `uint8_t[N]` - `true` if valid
     */
    VALIDATE_ADDRESS_RANGE = 0x06,

    /**
     * @param uint32_t ?
     * @param uint32_t ?
     * @param uint32_t ?
     *
     * @return ?
     */
    MEMORY_DISASSEMBLE = 0x08,

    /**
     * @brief
     *
     */
    READ_MEMORY_COMPRESSED = 0x09, // TODO Remove command when done and integrate in read memory

    /**
     * @param uint32_t address to write to
     * @param uint32_t value
     */
    KERNEL_WRITE = 0x0B,

    /**
     * @param uint32_t address to read from
     */
    KERNEL_READ = 0x0C,

    /**
     * No params. Not implemted.
     */
    TAKE_SCREEN_SHOT = 0x0D, // TODO Finish this

    /**
     * @brief
     *
     */
    UPLOAD_MEMORY = 0x41,

    /**
     * @return `uint8_t` - always `1` when server is active
     */
    SERVER_STATUS = 0x50,

    /**
     * @return `uint32_t` - size of data buffer
     */
    GET_DATA_BUFFER_SIZE = 0x51,

    /**
     * @brief
     *
     */
    READ_FILE = 0x52,

    /**
     * @brief
     *
     */
    READ_DIRECTORY = 0x53,

    /**
     * @brief
     *
     */
    REPLACE_FILE = 0x54, // TODO Test this

    /**
     * @return `uint32_t` - address of code handler
     */
    GET_CODE_HANDLER_ADDRESS = 0x55,

    /**
     * @brief
     *
     */
    READ_THREADS = 0x56,

    /**
     * @brief
     *
     */
    ACCOUNT_IDENTIFIER = 0x57,
    // WRITE_SCREEN = 0x58, // TODO Exception DSI

    /**
     * @brief
     *
     */
    FOLLOW_POINTER = 0x60,

    /**
     * @param int64_t(*)(...) address to function to execute
     * @param int32_t Argument 1
     * @param int32_t Argument 2
     * @param int32_t Argument 3
     * @param int32_t Argument 4
     * @param int32_t Argument 5
     * @param int32_t Argument 6
     * @param int32_t Argument 7
     * @param int32_t Argument 8
     *
     * @return `int64_t` - result of function
     */
    REMOTE_PROCEDURE_CALL = 0x70,

    /**
     * @param uint8_t num bytes of offsets + module + symbol
     * @param uint32_t offset to module name (always 8)
     * @param uint32_t offset to symbol name (8 + module name length)
     * @param char[N] module name
     * @param char[M] symbol name
     * @param uint8_t type of symbol to extract (function or data)
     *
     * @return `uint32_t` - address of symbol
     */
    GET_SYMBOL = 0x71,

    /**
     * @param uint32_t starting address
     * @param int32_t value to search for
     * @param uint32_t max length for search
     *
     * @return `uint32_t` address of found value. Not found: `0`
     */
    MEMORY_SEARCH_32 = 0x72,

    /**
     * @param uint32_t starting address
     * @param uint32_t length
     * @param uint32_t use kernel to read memory
     * @param uint32_t max number of results
     * @param uint32_t if search should be aligned to pattern
     * @param uint32_t pattern length
     * @param uint8_t[N] pattern
     *
     * @return `uint32_t[M]` – occurences * 4 + addresses where pattern was found
     */
    ADVANCED_MEMORY_SEARCH = 0x73,

    /**
     * @brief
     *
     */
    EXECUTE_ASSEMBLY = 0x81,

    /**
     * @brief Pause console. No parameters.
     */
    PAUSE_CONSOLE = 0x82,

    /**
     * @brief Resume console. No parameters.
     */
    RESUME_CONSOLE = 0x83,

    /**
     * @return `uint8_t` – `true` if console is paused
     */
    IS_CONSOLE_PAUSED = 0x84,

    /**
     * @brief
     *
     */
    SERVER_VERSION = 0x99,

    /**
     * @brief
     *
     */
    GET_OS_VERSION = 0x9A,

    /**
     * @brief
     *
     */
    SET_DATA_BREAKPOINT = 0xA0,

    /**
     * @brief
     *
     */
    SET_INSTRUCTION_BREAKPOINT = 0xA2,

    /**
     * @brief
     *
     */
    TOGGLE_BREAKPOINT = 0xA5,

    /**
     * @brief
     *
     */
    REMOVE_ALL_BREAKPOINTS = 0xA6,

    /**
     * @brief
     *
     */
    POKE_REGISTERS = 0xA7,

    /**
     * @brief
     *
     */
    GET_STACK_TRACE = 0xA8,

    /**
     * @brief
     *
     */
    GET_ENTRY_POINT_ADDRESS = 0xB1,

    /**
     * @brief
     *
     */
    RUN_KERNEL_COPY_SERVICE = 0xCD,

    /**
     * @brief
     *
     */
    IOSU_HAX_READ_FILE = 0xD0,

    /**
     * @return `uint32_t` - version hash of gecko
     */
    GET_VERSION_HASH = 0xE0,

    /**
     * @brief
     *
     */
    PERSIST_ASSEMBLY = 0xE1,

    /**
     * @brief
     *
     */
    CLEAR_ASSEMBLY = 0xE2,

    UNKNOWN = 0xFF
};
