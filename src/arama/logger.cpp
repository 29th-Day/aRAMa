#include "logger.h"

#include <whb/log_cafe.h>
#include <whb/log_console.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

#include <format>

static Logger::LogginFlags _flags;

void Logger::init(LogginFlags flags)
{
    _flags = flags;

    if (_flags & LogginFlags::CAFE) WHBLogCafeInit();
    if (_flags & LogginFlags::MODULE) WHBLogModuleInit();
    if (_flags & LogginFlags::CONSOLE) WHBLogConsoleInit();
    if (_flags & LogginFlags::UDP) WHBLogUdpInit();

}

void Logger::deinit()
{
    if (_flags & LogginFlags::CAFE) WHBLogCafeDeinit();
    if (_flags & LogginFlags::CONSOLE) WHBLogConsoleFree();
    if (_flags & LogginFlags::MODULE) WHBLogModuleDeinit();
    if (_flags & LogginFlags::UDP) WHBLogUdpDeinit();

    _flags = (LogginFlags)0;
}
