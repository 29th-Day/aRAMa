#include "notify.h"

#include <notifications/notifications.h>
#include <stdio.h>
#include <cstdarg>
#include <cstring>

static bool _init = false;

void Notifications::init()
{
    if (_init) return;

    NotificationModule_InitLibrary();
    NotificationModule_SetDefaultValue(
        NOTIFICATION_MODULE_NOTIFICATION_TYPE_INFO, NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT, 5.0);
    _init = true;
}

void Notifications::deinit()
{
    if (!_init) return;

    NotificationModule_DeInitLibrary();
    _init = false;
}

void Notifications::show(const char* str)
{
    NotificationModule_AddInfoNotification(str);
}

void Notifications::showf(const char* fmt, ...)
{
    if (!_init) return;

    char buffer[NOTIFICATIONS_MAX_LENGTH];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    NotificationModule_AddInfoNotification(buffer);
}
