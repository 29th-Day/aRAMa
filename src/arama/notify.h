#pragma once

/**
 * Maximal notification length of formatted string.
 */
#define NOTIFICATIONS_MAX_LENGTH 256

namespace Notifications
{
    void init();
    void deinit();

    void show(const char* str);
    void showf(const char* fmt, ...);
}
