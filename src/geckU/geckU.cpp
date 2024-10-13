#include "geckU.h"
#include "input.h"

#include "../arama/logger.h"
// #include "../kernel/kernel_utils.h"

#include <gx2/event.h>

#include <cstdint>
#include <cstring>

#include <array>
#include <vector>
#include <stack>
#include <thread>
#include <memory>
#include <atomic>

#define DATA_SIZE_8 0
#define DATA_SIZE_16 1
#define DATA_SIZE_32 2

#define SET_VALUE(address, value, type) *reinterpret_cast<type*>(address) = static_cast<type>(value)


/**
 * @brief Access information of geckU opcodes
 *
 * @link https://bullywiiplaza.website/wiiu/codetypes_EN.html
 * @note Works because Wii U is big endian!
 */
union GeckUCode
{
    uint32_t v;
    struct
    {
        // codetype
        uint32_t type : 8;
        // uses pointer?
        uint32_t pointer : 4;
        // data size
        uint32_t size : 4;
        // amount of something
        uint32_t N : 16;
    };
    struct
    {
        uint32_t : 20;
        Input::ControllerType type : 4;
        uint32_t : 4;
        Input::ControllerPort port : 4;
    } input;
};

namespace Level
{
    enum Type
    {
        BASE,
        BRANCH,
        LOOP
    };

    enum Status
    {
        RUN,
        SKIP
    };

    Status operator!(Status status)
    {
        return (status == Status::RUN) ? Status::SKIP : Status::RUN;
    }

    struct Info
    {
        Type type;
        Status status;
        uint32_t* begin;
        uint32_t iter;

        Info(Type type, Status status)
        {
            this->type = type;
            this->status = status;
            this->begin = nullptr;
            this->iter = 0;
        }

        Info(Type type, Status status, uint32_t* begin, uint32_t iter)
        {
            this->type = type;
            this->status = status;
            this->begin = begin;
            this->iter = iter;
        }
    };
}

enum CodeType : uint32_t
{
    NOP = 0x00, // not in tcpGecko
    RAM_WRITE = 0xAA, // modified from 0x00
    STRING_WRITE = 0x01,
    SKIP_WRITE = 0x02,
    MEMORY_FILL = 0x20,
    MEMORY_COPY = 0x21,
    CORRUPTOR = 0xF0,

    // Conditionals
    IF_EQUAL = 0x03,
    IF_NOT_EQUAL = 0x04,
    IF_GREATER = 0x05,
    IF_LESS = 0x06,
    IF_GREATER_EQUAL = 0x07,
    IF_LESS_EQUAL = 0x08,
    IF_LOGICAL_AND = 0x09,
    IF_LOGICAL_OR = 0x0A,
    IF_BETWEEN = 0x0B,
    SKIP_AFTER_FRAMES = 0x0C,
    RESET_FRAMES_IF = 0x0D,
    IF_INPUT = 0x0E,
    NEGATE_IF = 0x0F, // currently more like invert

    // Pointer
    LOAD_POINTER = 0x30,

    // Loops
    LOOP = 0x80,

    // End
    END = 0xD0, // named "Terminator" in TcpGecko
    END_LOOP = 0xD1, // named "Loop Terminator" in TcpGecko
    //
    UNKNOWN = 0xFF // max
};

// https://bullywiiplaza.website/wiiu/codetypes_EN.html

static std::unique_ptr<std::thread> geckuThread = nullptr;
static std::atomic_bool running = false;


#define CHECK_STATUS(x) if (x.top().status == Level::Status::SKIP) continue

// FIXME: THIS IS SOME UNSAFE TESTING BS!!!
#define panic(reason) Logger::printf("PANIC (%d) at 0x%08x + %u : %s", __LINE__, begin, current - begin, reason); return

void runHandler(uint32_t* const begin, uint32_t* const end)
{
    Logger::printf("CodeHandler | begin: 0x%08x - end: 0x%08x", begin, end);

    std::stack<Level::Info> level;
    level.push(Level::Info(Level::BASE, Level::RUN));

    Input::Init();

    for (uint32_t frame = 0; running; ++frame)
    {
        // handler main loop
        for (uint32_t* current = begin; current < end; ++current)
        {
            GeckUCode code = { *current };
            switch (code.type)
            {
            case CodeType::NOP:
            {
                ++current;
                break;
            }
            case CodeType::RAM_WRITE:
            {
                if (code.pointer)
                {
                    // use pointer
                }
                else
                {
                    // uint8_t size = DATA_SIZE(*current);
                    uint32_t address = *(++current);
                    uint32_t value = *(++current);
                    ++current;

                    CHECK_STATUS(level);

                    Logger::printf("RAM_WRITE_%u: 0x%08x <- %x", code.size, address, value);

                    switch (code.size)
                    {
                    case DATA_SIZE_8:
                        SET_VALUE(address, value, uint8_t);
                        break;
                    case DATA_SIZE_16:
                        SET_VALUE(address, value, uint16_t);
                        break;
                    case DATA_SIZE_32:
                        SET_VALUE(address, value, uint32_t);
                        break;
                    }
                }
                break;
            }
            case CodeType::STRING_WRITE:
            {
                if (code.pointer)
                {
                    // use pointer
                }
                else
                {
                    uint32_t bytes = code.N;
                    uint32_t address = *(++current);
                    void* buffer = ++current;
                    current += ((bytes / 8) + 1) * 2 - 1;

                    CHECK_STATUS(level);

                    Logger::printf("STRING_WRITE: 0x%08x (%u / 0x%x)", address, bytes, bytes);

                    std::memcpy(reinterpret_cast<void*>(address), buffer, bytes);

                }
                break;
            }
            case CodeType::SKIP_WRITE:
            {
                if (code.pointer)
                {
                    // use pointer
                }
                else
                {
                    uint32_t size = code.size;
                    uint32_t count = code.N;
                    uint32_t address = *(++current);
                    uint32_t value = *(++current);
                    uint32_t offset = *(++current);
                    uint32_t increment = *(++current);
                    ++current;

                    CHECK_STATUS(level);

                    Logger::printf("SKIP_WRITE_%u: 0x%08x (+ 0x%x) <- 0x%x (+ 0x%x) (%u / 0x%x)", size, address, offset, value, increment, count, count);

                    // NOT TESTED YET
                    while (count-- > 0)
                    {
                        switch (size)
                        {
                        case DATA_SIZE_8:
                            SET_VALUE(address, value, uint8_t);
                            break;
                        case DATA_SIZE_16:
                            SET_VALUE(address, value, uint16_t);
                            break;
                        case DATA_SIZE_32:
                            SET_VALUE(address, value, uint32_t);
                            break;
                        }
                        address += offset;
                        value += increment;
                    }

                }
                break;
            }
            case CodeType::MEMORY_FILL:
            {

                if (code.pointer)
                {

                }
                else
                {
                    uint32_t value = *(++current);
                    uint32_t* address = reinterpret_cast<uint32_t*>(*(++current));
                    uint32_t length = *(++current);

                    CHECK_STATUS(level);

                    Logger::printf("MEMORY_FILL: 0x%08x - 0x%08x <- 0x%08x", address, address + length, value);

                    while (length-- > 0)
                    {
                        *address = value;
                        ++address;
                    }
                }
                break;
            }
            case CodeType::MEMORY_COPY: // TEST: not tested
            {
                if (code.pointer)
                {

                }
                else
                {
                    uint32_t src = *(++current);
                    uint32_t dst = *(++current);
                    uint32_t bytes = *(++current);

                    CHECK_STATUS(level);

                    Logger::printf("MEMORY_COPY: 0x%08x + (0x%x) -> 0x%08x + (0x%x)", src, bytes, dst, bytes);

                    std::memcpy(reinterpret_cast<void*>(dst), reinterpret_cast<void*>(src), bytes);

                }
                break;
            }
            case CodeType::CORRUPTOR:
            {
                // no pointer version
                uint32_t start = *(++current);
                uint32_t end = *(++current);
                uint32_t value = *(++current);
                uint32_t replace = *(++current);
                ++current;

                Logger::printf("CORRUPTOR: 0x%08x - 0x%08x: 0x%08x -> 0x%08x", start, end, value, replace);

                for (uint32_t i = start; i < end; ++i)
                {
                    if (*reinterpret_cast<uint32_t*>(i) == value)
                    {
                        *reinterpret_cast<uint32_t*>(i) = replace;
                        break;
                    }
                }
                break;
            }
            // CONDITIONALS
            case CodeType::IF_EQUAL:
            case CodeType::IF_NOT_EQUAL:
            case CodeType::IF_GREATER: // TEST: not tested
            case CodeType::IF_LESS: // TEST: not tested
            case CodeType::IF_GREATER_EQUAL: // TEST: not tested
            case CodeType::IF_LESS_EQUAL: // TEST: not tested
            case CodeType::IF_LOGICAL_AND: // TEST: not tested
            case CodeType::IF_LOGICAL_OR: // TEST: not tested
            {
                if (code.pointer)
                {

                }
                else
                {
                    uint32_t address = *(++current);
                    uint32_t compare = *(++current);
                    ++current;

                    CHECK_STATUS(level);

                    uint32_t value;
                    switch (code.size)
                    {
                    case DATA_SIZE_8:
                        value = static_cast<uint32_t>(*reinterpret_cast<uint8_t*>(address));
                        break;
                    case DATA_SIZE_16:
                        value = static_cast<uint32_t>(*reinterpret_cast<uint16_t*>(address));
                        break;
                    case DATA_SIZE_32:
                        value = *reinterpret_cast<uint32_t*>(address);
                        break;
                    }

                    switch (code.type)
                    {
                    case CodeType::IF_EQUAL:
                        Logger::printf("IF_EQUAL: 0x%08x -> 0x%x == 0x%x", address, value, compare);
                        level.push(Level::Info(Level::BRANCH, (value == compare) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_NOT_EQUAL:
                        Logger::printf("IF_NOT_EQUAL: 0x%08x -> 0x%x != 0x%x", address, value, compare);
                        level.push(Level::Info(Level::BRANCH, (value != compare) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_GREATER:
                        Logger::printf("IF_GREATER: 0x%08x -> 0x%x > 0x%x", address, value, compare);
                        level.push(Level::Info(Level::BRANCH, (value > compare) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_LESS:
                        Logger::printf("IF_LESS: 0x%08x -> 0x%x < 0x%x", address, value, compare);
                        level.push(Level::Info(Level::BRANCH, (value < compare) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_GREATER_EQUAL:
                        Logger::printf("IF_GREATER_EQUAL: 0x%08x -> 0x%x >= 0x%x", address, value, compare);
                        level.push(Level::Info(Level::BRANCH, (value >= compare) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_LESS_EQUAL:
                        Logger::printf("IF_LESS_EQUAL: 0x%08x -> 0x%x <= 0x%x", address, value, compare);
                        level.push(Level::Info(Level::BRANCH, (value <= compare) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_LOGICAL_AND:
                        Logger::printf("IF_LOGICAL_AND: 0x%08x -> 0x%x == (0x%x & 0x%x)", address, value, compare, value);
                        level.push(Level::Info(Level::BRANCH, (value == (value & compare)) ? Level::RUN : Level::SKIP));
                        break;
                    case CodeType::IF_LOGICAL_OR:
                        Logger::printf("IF_LOGICAL_OR: 0x%08x -> 0x%x == (0x%x | 0x%x)", address, value, compare, value);
                        level.push(Level::Info(Level::BRANCH, (value == (value | compare)) ? Level::RUN : Level::SKIP));
                        break;
                    }
                }
                break;
            }
            case CodeType::IF_BETWEEN: // TEST: not tested
            {
                if (code.pointer)
                {

                }
                else
                {
                    uint32_t address = *(++current);
                    uint32_t min = *(++current);
                    uint32_t max = *(++current);

                    CHECK_STATUS(level);

                    uint32_t value;
                    switch (code.size)
                    {
                    case DATA_SIZE_8:
                        value = static_cast<uint32_t>(*reinterpret_cast<uint8_t*>(address));
                        break;
                    case DATA_SIZE_16:
                        value = static_cast<uint32_t>(*reinterpret_cast<uint16_t*>(address));
                        break;
                    case DATA_SIZE_32:
                        value = *reinterpret_cast<uint32_t*>(address);
                        break;
                    }

                    Logger::printf("IF_BETWEEN: 0x%08x -> 0x%x in [0x%x, 0x%x]", address, value, min, max);

                    level.push(Level::Info(Level::BRANCH, (value >= min && value <= max) ? Level::RUN : Level::SKIP));
                }
                break;
            }
            case CodeType::SKIP_AFTER_FRAMES: // TEST: not tested
            {
                uint32_t time = *(++current);

                CHECK_STATUS(level);

                Logger::printf("SKIP_AFTER_FRAMES: 0x%x < 0x%x", time, frame);

                level.push(Level::Info(Level::BRANCH, (time < frame) ? Level::RUN : Level::SKIP));
                break;
            }
            case CodeType::RESET_FRAMES_IF: // TEST: not tested
            {
                uint32_t* address = reinterpret_cast<uint32_t*>(*(++current));

                CHECK_STATUS(level);

                Logger::printf("RESET_FRAMES_IF: 0x%08x -> 0x%x == 0x%x", address, *address, code.N);

                if (*address == code.N)
                    frame = 0;

                break;
            }

            case CodeType::IF_INPUT:
            {
                uint32_t input = *(++current);

                CHECK_STATUS(level);

                uint32_t pressed = Input::pressed(code.input.type, code.input.port);

                Logger::printf("IF_INPUT: 0x%08x == 0x%08x", input, pressed);

                level.push(Level::Info(Level::BRANCH, (input == pressed) ? Level::RUN : Level::SKIP));

                break;
            }

            case CodeType::NEGATE_IF: // TEST: not tested
            {
                ++current;

                if (level.top().type != Level::Type::BRANCH)
                {
                    panic("expected BRANCH");
                }

                // if (level.top().status == LevelStatus::SKIP)
                    // level.top().status = LevelStatus::RUN;

                level.top().status = !level.top().status;

                break;
            }

            case CodeType::LOOP: // TEST: not tested
            {
                uint32_t iter = *(++current);

                CHECK_STATUS(level);

                level.push(Level::Info(Level::LOOP, (iter > 0) ? Level::RUN : Level::SKIP, current, iter));

                break;
            }

            case CodeType::END: // TEST: not tested
            {
                ++current;

                if (level.top().type != Level::BRANCH)
                {
                    panic("expected BRANCH");
                }

                Logger::print("END");
                level.pop();

                break;
            }

            case CodeType::END_LOOP:  // TEST: not tested
            {
                ++current;

                if (level.top().type != Level::LOOP)
                {
                    panic("expected LOOP");
                }

                if (level.top().status == Level::RUN && level.top().iter > 0)
                {
                    --level.top().iter;
                    current = level.top().begin;
                }
                else
                {
                    level.pop();
                }
                break;
            }

            case CodeType::LOAD_POINTER: // TEST: not tested
            {
                uint32_t value = *(++current);
                uint32_t min = *(++current);
                uint32_t max = *(++current);

                CHECK_STATUS(level);

                if (code.pointer)
                {
                    if ()
                }

                break;
            }

            default:
                panic("UNKNOWN INSTRUCTION");
            }
        } // handler loop

        if (level.top().type != Level::BASE)
        {
            Logger::printf("PANIC: expected BASE");
            return;
        }

        // 15 fps
        // FIXME: everything else blocks the GamePad when I use IF_INPUT
        GX2WaitForVsync();
        GX2WaitForVsync();
        GX2WaitForVsync();
        GX2WaitForVsync();
    } // main loop

    Input::Deinit();

    // if (level.empty())
    // {
    //     // somewhere a bug occured!
    //     // base level should never be popped!
    // }
}

void GeckU::start()
{
    if (geckuThread)
        return;

    Logger::printf("GeckU::start is NOT YET IMPLEMTED");

    // running = true;
    // geckuThread = std::make_unique<std::thread>(runHandler, );
}

void GeckU::test(uint32_t* const begin, uint32_t* const end)
{
    if (geckuThread)
        return;

    // Logger::printf("GeckU::start is NOT YET IMPLEMTED");

    running = true;
    geckuThread = std::make_unique<std::thread>(runHandler, begin, end);
}

void GeckU::stop()
{
    if (!geckuThread)
        return;

    running = false;
    if (geckuThread->joinable())
        geckuThread->join();
    geckuThread = nullptr;
}
