#pragma once

#include "commands.h"
#include "../../tcp/server.h"

namespace ConsoleState
{
    enum State : uint32_t
    {
        PAUSED = 0x3800'0001,
        RUNNING = 0x3800'0000
    };

    void Set(State state);

    void Get(const Socket* socket);
};
