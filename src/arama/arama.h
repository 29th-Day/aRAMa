#pragma once

#include <cstdint>

#define ARAMA_PLUGIN_NAME "aRAMa"

#ifndef ARAMA_VERSION
#define ARAMA_VERSION 0x00'00'00'01
#endif

namespace Version
{
    inline uint8_t major() { return (ARAMA_VERSION >> 16) & 0xFF; }
    inline uint8_t minor() { return (ARAMA_VERSION >> 8) & 0xFF; }
    inline uint8_t patch() { return ARAMA_VERSION & 0xFF; }
};

bool isRunningAllowedTitleID();
