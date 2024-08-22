#pragma once

#include <coreinit/debug.h>
// #include <string.h>
#include <whb/log.h>

#include <string>
#include <format>

#define btos(x) x ? "true" : "false"

namespace Logger
{
    enum LogginFlags : int
    {
        CAFE = 1 << 0,
        CONSOLE = 1 << 1,
        MODULE = 1 << 2,
        UDP = 1 << 3,
    };

    inline LogginFlags operator|(LogginFlags a, LogginFlags b)
    {
        return static_cast<LogginFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    void init(LogginFlags flags);

    void deinit();

    constexpr auto print = WHBLogPrint;
    constexpr auto printf = WHBLogPrintf;
};
