#include "arama.h"
#include "logger.h"

#include "coreinit/title.h"

// http://wiiubrew.org/wiki/Title_database

#define SYSTEM_APPS 0x00050010'0000'0000
#define SYSTEM_DATA_ARCHIVE 0x0005001B'0000'0000
#define SYSTEM_APPLET 0x00050030'0000'0000
#define SYSTEM_UPDATE 0x0005000E'0000'0000

bool isRunningAllowedTitleID()
{
    switch (OSGetTitleID() & 0xFFFF'FFFF'0000'0000)
    {
    case SYSTEM_APPS:
    case SYSTEM_DATA_ARCHIVE:
    case SYSTEM_APPLET:
    case SYSTEM_UPDATE:
        return false;
    default:
        return true;
    }
}
