#pragma once

#include "commands.h"
#include "../../tcp/server.h"

namespace OS
{
    void Version(const Socket* socket);

    void GetThreads(const Socket* socket);
};
