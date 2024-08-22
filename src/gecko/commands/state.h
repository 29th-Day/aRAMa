#pragma once

#include "commands.h"
#include "../../tcp/server.h"

enum ConsoleState : uint32_t
{
    PAUSED = 0x3800'0001,
    RUNNING = 0x3800'0000
};

void PauseConsole(const Socket socket);

void ResumeConsole(const Socket socket);

void GetConsoleState(const Socket socket);
