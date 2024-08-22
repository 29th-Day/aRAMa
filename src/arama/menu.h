#pragma once

// #include <wups/storage.h>
#include <wups.h>

namespace PluginMenu
{
    WUPSConfigAPIStatus Init();

    WUPSConfigAPICallbackStatus Open(WUPSConfigCategoryHandle root);

    void Close();
};
