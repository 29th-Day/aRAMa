#pragma once

#include "commands.h"
#include "../../tcp/server.h"

namespace OS
{
    void TakeScreenshot(const Socket* socket);

    void Version(const Socket* socket);

    void AccountIdentifier(const Socket* socket);

    void GetThreads(const Socket* socket);
};
