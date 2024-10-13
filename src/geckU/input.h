#pragma once

#include <cstdint>

namespace Input
{
    enum ControllerType : uint32_t
    {
        WiiUGamePad,
        WiiMote,
        WiiMote_Nunchuck,
        Classic_ProController
    };

    enum ControllerPort : uint32_t
    {
        PORT_1,
        PORT_2,
        PORT_3,
        PORT_4
    };
    /*
    enum WiiUGamePad
    {
        A = 0x00008000,
        B = 0x00004000,
        X = 0x00002000,
        Y = 0x00001000,
        DP_Left = 0x00000800,
        DPAD_Right = 0x00000400,
        DPAD_Up = 0x00000200,
        DPAD_Down = 0x00000100,
        ZL = 0x00000080,
        ZR = 0x00000040,
        L = 0x00000020,
        R = 0x00000010,
        Plus = 0x00000008,
        Minus = 0x00000004,
        Home = 0x00000002,
        Sync = 0x00000001,
        TV = 0x00010000,
        RS_Button = 0x00020000,
        LS_Button = 0x00040000,
        RS_Left = 0x04000000,
        RS_Right = 0x02000000,
        RS_Up = 0x01000000,
        RS_Down = 0x00800000,
        LS_Left = 0x40000000,
        LS_Right = 0x20000000,
        LS_Up = 0x10000000,
        LS_Down = 0x08000000
    };

    enum WiiMote
    {
        A = 0x00000800,
        B = 0x00000400,
        One = 0x00000200,
        Two = 0x00000100,
        DP_Left = 0x00000001,
        DPAD_Right = 0x00000002,
        DPAD_Up = 0x00000008,
        DPad_Down = 0x00000004,
        Plus = 0x00000010,
        Minus = 0x00001000,
        Home = 0x00008000
    };

    enum WiiMoteNunchuck
    {
        A = 0x00000800,
        B = 0x00000400,
        One = 0x00000200,
        Two = 0x00000100,
        DP_Left = 0x00000001,
        DPAD_Right = 0x00000002,
        DPAD_Up = 0x00000008,
        DPad_Down = 0x00000004,
        Plus = 0x00000010,
        Minus = 0x00001000,
        Home = 0x00008000,
        Z = 0x00002000,
        C = 0x00004000,
        Nunchuck_Left = 0x00000001,
        Nunchuck_Right = 0x00000002,
        Nunchuck_Up = 0x00000008,
        Nunchuck_Down = 0x00000004
    };

    enum ClassicController
    {
        A = 0x00000010,
        B = 0x00000040,
        X = 0x00000008,
        Y = 0x00000020,
        DP_Left = 0x00000002,
        DPAD_Right = 0x00008000,
        DPAD_Up = 0x00000001,
        DPad_Down = 0x00004000,
        ZL = 0x00000080,
        ZR = 0x00000004,
        L = 0x00002000,
        R = 0x00000200,
        Plus = 0x00000400,
        Minus = 0x00001000,
        Home = 0x00000800,
        RS_Left = 0x00100000,
        RS_Right = 0x00200000,
        RS_Up = 0x00800000,
        RS_Down = 0x00400000,
        LS_Left = 0x00010000,
        LS_Right = 0x00020000,
        LS_Up = 0x00080000,
        LS_Down = 0x00040000
    };

    enum ProController
    {
        A = 0x00000010,
        B = 0x00000040,
        X = 0x00000008,
        Y = 0x00000020,
        DP_Left = 0x00000002,
        DPAD_Right = 0x00008000,
        DPAD_Up = 0x00000001,
        DPad_Down = 0x00004000,
        ZL = 0x00000080,
        ZR = 0x00000004,
        L = 0x00002000,
        R = 0x00000200,
        Plus = 0x00000400,
        Minus = 0x00001000,
        Home = 0x00000800,
        RS_Button = 0x00001000,
        LS_Button = 0x00002000,
        RS_Left = 0x00400000,
        RS_Right = 0x00800000,
        RS_Up = 0x02000000,
        RS_Down = 0x01000000,
        LS_Left = 0x00040000,
        LS_Right = 0x00080000,
        LS_Up = 0x00200000,
        LS_Down = 0x00100000
    };
    */

    void Init();
    void Deinit();

    uint32_t pressed(ControllerType type, ControllerPort port);
};
