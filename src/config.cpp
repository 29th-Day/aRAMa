#include <coreinit/title.h>
#include <wups/storage.h>
#include <wups.h>
#include "config.h"
#include "arama.h"

// //certified C++ moment
// const char aRAMaConfig::active_id = '0';
// const char aRAMaConfig::sd_codes_id = '1';
// const char aRAMaConfig::notifs_id = '2';
// const char aRAMaConfig::code_hand_id = '3';
// const char aRAMaConfig::autosave_id = '4';
// const char aRAMaConfig::caffiine_id = '5';
// const char aRAMaConfig::saviine_id = '6';
// const char aRAMaConfig::tcpgecko_id = '7';

// uint32_t aRAMaConfig::settings = 0;
// uint32_t aRAMaConfig::ip_address = false;
// uint32_t aRAMaConfig::local_code_amount = 0;

aRAMaConfig::aRAMaConfig()
{
    settings = std::map<const char*, bool>{
        {"aRAMa", false},
        {"SD Codes", false},
        {"Notifications", false},
        {"Code Handler", false},
        {"Save codes to SD", false},
        {"Caffiine", false},
        {"Saviine", false},
        {"TcpGecko", false}
    };

    LoadSettings();
    SaveSettings();
}

aRAMaConfig::~aRAMaConfig()
{
    plugin_cleanup();
}


void aRAMaConfig::LoadSettings()
{

    for (auto& [key, value] : settings)
    {
        if (WUPSStorageAPI::GetOrStoreDefault(key, value, value) != WUPS_STORAGE_ERROR_SUCCESS)
        {
            singletons.log->print("aRAMa: Retrieving settings failed");
        }
    }
}

void aRAMaConfig::SaveSettings()
{
    for (auto& [key, value] : settings)
    {
        if (WUPSStorageAPI::Store(key, value) != WUPS_STORAGE_ERROR_SUCCESS)
        {
            singletons.log->print("aRAMa: Retrieving settings failed");
        }
    }

    if (WUPSStorageAPI_SaveStorage(false) != WUPS_STORAGE_ERROR_SUCCESS)
    {
        singletons.log->print("aRAMa: Error occured in saving settings!");
    }
}

void aRAMaConfig::settings_changed(ConfigItemBoolean* item, bool new_value)
{
    settings[item->identifier] = new_value;
}

void aRAMaConfig::plugin_setup()
{
    //If aRAMa shoudn't be active, immedeately end function
    if (!settings["aRAMa"])
    {
        plugin_cleanup();
        return;
    }

    //SD codes get priority, regardless of being online
    if (settings["SD Codes"])
    {
        //Init Gecko so it can load SD codes

        singletons.log->print_w_notif("aRAMa is active, checking for local codes for this title...\n", 8);

        //Check SD codes

        //deinit if offline and no codes are found
    }

    if (!settings["TcpGecko"])
    {
        if (!settings["SD Codes"])
        {
            //If aRAMa is active and offline, but SD codes are disabled, no reason
            //to keep it loaded in cause it'll sit there doing nothing
            plugin_cleanup();
            return;
        }
        singletons.code_handler->start_server();
        //Start aRAMa offline, with only SD codes
        return;
    }
    //If we're not offline, we must be online :bigbrain:
    else
    {
        singletons.gecko->start_server();
        //This setting has not changed, and it returns the opposite of the previous check,
        //So it can be used to check for Gecko being initialized
        if (!settings["SD Codes"])
        {
            //Run SD codes
        }
    }

    // Activate TCP Gecko if it's enabled and not already activated
    if (settings["TcpGecko"] && singletons.gecko == nullptr)
    {
        singletons.gecko = new GeckoProcessor;
        singletons.gecko->start_server();
    }
}

void aRAMaConfig::plugin_cleanup()
{
    if (!settings["TcpGecko"] && singletons.gecko != nullptr)
    {
        singletons.gecko->stop_server();
        delete singletons.gecko;
        singletons.gecko = nullptr;
    }
    if (singletons.code_handler != nullptr && !settings["Code Handler"])
    {
        singletons.code_handler->stop_server();
        delete singletons.code_handler;
        singletons.code_handler = nullptr;
    }
    if (singletons.caffiine != nullptr && !settings["Caffiine"])
    {
        singletons.caffiine->stop_server();
        delete singletons.caffiine;
        singletons.caffiine = nullptr;
    }
    if (singletons.saviine != nullptr && !settings["Saviine"])
    {
        singletons.saviine->stop_server();
        delete singletons.saviine;
        singletons.saviine = nullptr;
    }
}

WUPSConfigAPICallbackStatus aRAMaConfig::open_config(WUPSConfigCategoryHandle root)
{
    LoadSettings();
    WUPSConfigCategoryHandle base_category;

    WUPSConfigAPICreateCategoryOptionsV1 base_opts;

    base_opts.name = ARAMA_PLUGIN_NAME;

    WUPSConfigAPI_Category_Create(base_opts, &base_category);

    WUPSConfigCategoryHandle arama_category;
    WUPSConfigAPICreateCategoryOptionsV1 arama_opts;
    arama_opts.name = "aRAMa settings";

    WUPSConfigAPI_Category_Create(arama_opts, &arama_category);


    //Then we register categories, such as the settings for aRAMa
    WUPSConfigAPI_Category_AddCategory(base_category, arama_category);

    for (const auto& [key, value] : settings)
    {
        WUPSConfigItemBoolean_AddToCategoryEx(arama_category,
            key, key, false,
            value, &settings_changed, "Enabled", "Disabled");
    }

    //If there are SD codes and it's enabled, lode that category too
    if (aRAMaConfig::settings["SD Codes"] && local_code_amount > 0)
    {
        uint64_t current_title_id = OSGetTitleID();
        uint8_t iterate = 1;

        WUPSConfigCategoryHandle codes_category;

        //Todo: Have it say the name of the app here instead of "this title"
        //WUPSConfig_AddCategoryByNameHandled(base, "SD Codes for this title", &codes_category);
    }
    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void aRAMaConfig::close_config()
{
    SaveSettings();
    plugin_cleanup();
    plugin_setup();
}
