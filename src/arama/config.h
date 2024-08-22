#pragma once

#include <string>
#include <map>

#include <wups.h>

namespace Config
{
    extern const std::string aRAMa;
    extern const std::string sdCheats;
    extern std::map<std::string, bool> options;

    WUPSStorageError Load();
    WUPSStorageError Save();

    void print_options();
}
