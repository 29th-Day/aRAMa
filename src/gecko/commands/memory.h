#pragma once

#include "commands.h"
#include "../../tcp/server.h"


namespace Memory
{
    void Read(const Socket* socket);

    template <typename T>
    void Write(const Socket* socket);

    void ReadKernel(const Socket* socket);

    void WriteKernel(const Socket* socket);

    void Search32(const Socket* socket);

    void SearchEx(const Socket* socket);

    void Disassemble(const Socket* socket);

    void Upload(const Socket* socket);
};
