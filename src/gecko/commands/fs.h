#pragma once

#include "commands.h"
#include "../../tcp/server.h"

#define FS_SD_CARD "fs:/vol/external1"

namespace FS
{
    void ReadFile(const Socket* socket);

    void ReadDirectory(const Socket* socket);

    void WriteFile(const Socket* socket);
};
