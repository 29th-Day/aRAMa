#pragma once

#include "commands.h"
#include "../../tcp/server.h"

namespace Debug
{
    void DataBreakpoint(const Socket* socket);

    void InstructionBreakpoint(const Socket* socket);

    void ToggleBreakpoint(const Socket* socket);

    void RemoveAllBreakpoints(const Socket* socket);

    void StackTrace(const Socket* socket);
}
