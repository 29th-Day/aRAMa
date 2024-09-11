#include "menu.h"
#include "config.h"
#include "arama.h"
#include "logger.h"
#include "version.h"
#include "../tcp/server.h"

#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
#include <wups/config/WUPSConfigItemStub.h>
#include <nn/ac/ac_cpp.h>

#include <format>

void modified(ConfigItemBoolean* item, bool new_value)
{
    Config::options[std::string(item->identifier)] = new_value;
}

WUPSConfigAPIStatus PluginMenu::Init()
{
    Config::Load();

    WUPSConfigAPIOptionsV1 options;
    options.name = ARAMA_PLUGIN_NAME;

    return WUPSConfigAPI_Init(options, PluginMenu::Open, PluginMenu::Close);
}

WUPSConfigAPICallbackStatus PluginMenu::Open(WUPSConfigCategoryHandle root)
{
    WUPSConfigAPIStatus error = WUPSCONFIG_API_RESULT_SUCCESS;

    for (const auto& [key, value] : Config::options)
    {
        error = WUPSConfigItemBoolean_AddToCategoryEx(root,
            key.c_str(), key.c_str(), false,
            value, &modified, "Yes", "No");

        if (error != WUPSCONFIG_API_RESULT_SUCCESS)
        {
            Logger::printf("Error Code: %i while building menu: %s", error, key);
        }
    }

    error = WUPSConfigItemStub_AddToCategory(root, std::string(49, '=').c_str());

    uint32_t ip = 0;
    nn::ac::GetAssignedAddress(&ip);
    error = WUPSConfigItemStub_AddToCategory(root,
        std::format("IP: {}.{}.{}.{}",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF,
            ip & 0xFF).c_str());

    error = WUPSConfigItemStub_AddToCategory(root,
        std::format("TCP PORT: {}", TCP::PORT).c_str());

    return (error == WUPSCONFIG_API_RESULT_SUCCESS) ?
        WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS :
        WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
}

void PluginMenu::Close()
{
    Config::Save();
}
