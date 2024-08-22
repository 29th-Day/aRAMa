#include "config.h"
#include "logger.h"

#include <wups.h>

const std::string Config::aRAMa = "Enable aRAMa?";
const std::string Config::sdCheats = "Use cheats from SD card?";

std::map<std::string, bool> Config::options{
    {aRAMa, false},
    {sdCheats, false}
};

WUPSStorageError Config::Load()
{
    WUPSStorageError error = WUPS_STORAGE_ERROR_SUCCESS;

    for (auto& [key, value] : Config::options)
    {
        error = WUPSStorageAPI::GetOrStoreDefault(key, value, value);

        if (error != WUPS_STORAGE_ERROR_SUCCESS)
        {
            Logger::printf("Error while loading config: %s", key);
        }
    }

    return error;
}

WUPSStorageError Config::Save()
{
    for (const auto& [key, value] : Config::options)
    {
        WUPSStorageAPI::Store(key, value);
    }

    return WUPSStorageAPI::SaveStorage();
}

void Config::print_options()
{
    Logger::print("-----");
    for (const auto& [key, value] : Config::options)
    {
        Logger::printf("%s: %s", key.c_str(), btos(value));
    }
    Logger::print("-----");
}
