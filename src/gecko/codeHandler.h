#pragma once

#include <cstdint>

#define CODE_HANDLER_INSTALL_ADDRESS 0x010F'4000U

namespace CodeHandler
{
    void Install();

    void CodeHandlerFunction();

    void test(uint32_t code, ...);
};
