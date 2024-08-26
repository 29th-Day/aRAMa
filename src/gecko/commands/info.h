#pragma once

#include "commands.h"
#include "../../tcp/server.h"

namespace Info
{
    void Status(const Socket socket);

    void BufferSize(const Socket socket);

    void CodeHandlerAddress(const Socket socket);

    void VersionHash(const Socket socket);
};
