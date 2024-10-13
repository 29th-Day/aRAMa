// front facing API

#pragma once

#include <cstdint>

#define GECKU_INSTALL_ADDRESS 0x010F'4000U

/**
 * @brief Cheat code handler
 *
 * aRAMa's implementation of the Gecko Cheat Code Handler.
 */
namespace GeckU
{
    // FIXME: debug only
    void test(uint32_t* const begin, uint32_t* const end);

    void start();

    void stop();
};
