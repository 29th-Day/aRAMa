#pragma once

#define COPY_SERVICE_ADDRESS 0x10100000

#include <cstdint>

namespace CopyService
{
    extern uint32_t* const desination;
    extern uint32_t* const value;

    void start();

    void stop();
}
