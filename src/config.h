#ifndef ARAMA_CONFIG_H
#define ARAMA_CONFIG_H

#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config.h>

#include <map>

class aRAMaConfig
{
private:
    //The raw bitfield of settings fetched from and saved to WUPS storage.
    // static uint32_t settings;

public:
    // enum ARAMA_SETTINGS
    // {
    //     ACTIVE = 1 << 0,
    //     SD_CODES_ACTIVE = 1 << 1,
    //     NOTIFICATIONS_ON = 1 << 2,
    //     CODE_HANDLER = 1 << 3,
    //     AUTO_STORE_CODES = 1 << 4,
    //     ENABLE_CAFFIINE = 1 << 5,
    //     ENABLE_SAVIINE = 1 << 6,
    //     ENABLE_TCPGECKO = 1 << 7,
    // };

    // static const char active_id;
    // static const char sd_codes_id;
    // static const char notifs_id;
    // static const char code_hand_id;
    // static const char autosave_id;
    // static const char caffiine_id;
    // static const char saviine_id;
    // static const char tcpgecko_id;

    //IP address of the Wii U
    uint32_t ip_address;

    //Amount of SD codes detected for the currently running title
    uint32_t local_code_amount;

    std::map<const char*, bool> settings;

    // bool* aRAMa_active;
    // bool* sdCodes_active;
    // bool* notifications_active;
    // bool* codeHandler_active;
    // bool* save_new_codes;
    // bool* caffiine_active;
    // bool* saviine_active;
    // bool* tcpGecko_active;

    aRAMaConfig();
    ~aRAMaConfig();

    //wrapper for the config API to set any of the settings
    static void settings_changed(ConfigItemBoolean* item, bool new_value);

    //Create/fetch all the settings in WUPS storage
    void LoadSettings();
    //Save the settings to WUPS storage
    void SaveSettings();

    //This function is responsible for loading up all the servers that should be active,
    //with extra checks to only load up some things if other necessary things are active.
    void plugin_setup();
    //This function cleans up all the servers that shouldn't be active anymore
    void plugin_cleanup();

    WUPSConfigAPICallbackStatus open_config(WUPSConfigCategoryHandle root);
    void close_config();

};


#endif
