#pragma once

#include "commands.h"
#include "../../tcp/server.h"

namespace System
{
    void GetSymbol(const Socket* socket);

    void ExecuteProcedure(const Socket* socket);

    void ValidateAddressRange(const Socket* socket);

    void FollowPointer(const Socket* socket);

    void ExecuteAssembly(const Socket* socket);
};
