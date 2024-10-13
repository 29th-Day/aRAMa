#include "input.h"

#include "../arama/logger.h"

#include <vpad/input.h>

void Input::Init()
{
    VPADInit();
}

void Input::Deinit()
{
    VPADShutdown();
}

uint32_t Input::pressed(ControllerType type, ControllerPort port)
{
    switch (type)
    {
    case ControllerType::WiiUGamePad:
    {
        if (port != ControllerPort::PORT_1)
        {
            Logger::printf("GamePad must use PORT_1 (0)");
            return 0;
        }

        VPADStatus status;
        VPADReadError error = VPAD_READ_SUCCESS;

        VPADRead(VPAD_CHAN_0, &status, 1, &error);

        if (error != VPAD_READ_SUCCESS)
        {
            Logger::printf("ERROR while reading GamePad input: %i", error);
            return 0;
        }

        return status.hold;
    }

    default:
        Logger::printf("UNKNOWN CONTROLLER type %d", type);
        return 0;
    }
}
