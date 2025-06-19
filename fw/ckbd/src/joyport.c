//-------------------------------------------------------------------------
// joyport.c
// Legacy joystick and mouse
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "joyport.h"
#include "ikbd.h"

// In Amiga mode; read the pins as if 1 and 4 are swapped.
//      ST  AM  RV
// 1    XB  YB  XB <--
// 2    XA  XA  XA
// 3    YA  YA  YA
// 4    YB  XB  YB < --
// 5    --  mb  --
// 6    lb  lb  lb
// 7    5v  5v  5v
// 8    gn  gn  gn
// 9    rb  rb  -- (use j1 fire)


// todo: perhaps generate 256 byte table for faster single-table lookup.
//       value = (delta + 1)
// ya0.yb0.xa0.xb0.ya1.yb1.xa1.xb1
// [old][cur]
//
// pins = (Px & mask) >> shift)  (xa.xb.ya.yb)
//
// state = (state << 4) | pins;
// move = quad[state];
// move_x = (move & 0xf) - 1;
// move_y = (move >> 4) - 1;
//


const int8_t decodeQuadrature[] =
{ //     00  01  10  11
/* 00 */  0,  0, -1, +1,
/* 01 */  0,  0, -1, +1,
/* 10 */ +1, -1,  0,  0,
/* 11 */ +1, -1,  0,  0,
};

static volatile int16_t jmouse_xrel = 0;
static volatile int16_t jmouse_yrel = 0;

void JoyportInterrupt(void) {
    static uint8_t xaxis = 0;
    static uint8_t yaxis = 0;

#if defined(BOARD_PROTO) // todo: remove once real boards arrive
    uint8_t p4 = P4_IN;
    uint8_t p0 = P0;

    if (Settings.LegacyMouseAmiga) {
        xaxis = (xaxis << 2) | ((p4 & 0b00000001) | ((p0 & 0b00000100) >> 1));
        yaxis = (yaxis << 2) | ((p4 & 0b00000010) | ((p0 & 0b00001000) >> 3));
        jmouse_xrel += decodeQuadrature[xaxis & 0b00001111];
        jmouse_yrel -= decodeQuadrature[yaxis & 0b00001111];
    } else {
        xaxis = (xaxis << 2) | (p4 & 0b00000011);
        yaxis = (yaxis << 2) | ((p0>>2) & 0b00000011);
        jmouse_xrel += decodeQuadrature[xaxis & 0b00001111];
        jmouse_yrel += decodeQuadrature[yaxis & 0b00001111];
    }
#else
    uint8_t p4 = ~P4_IN;
    if (Settings.LegacyMouseAmiga) {
        xaxis = (xaxis << 2) | ((p4 & 0b00000001) | ((p4 & 0b00000100) >> 1));
        yaxis = (yaxis << 2) | ((p4 & 0b00000010) | ((p4 & 0b00001000) >> 3));
        jmouse_xrel -= decodeQuadrature[xaxis & 0b00001111];
        jmouse_yrel += decodeQuadrature[yaxis & 0b00001111];
    } else {
        xaxis = (xaxis << 2) | (p4 & 0b00000011);
        yaxis = (yaxis << 2) | ((p4>>2) & 0b00000011);
        jmouse_xrel -= decodeQuadrature[xaxis & 0b00001111];
        jmouse_yrel -= decodeQuadrature[yaxis & 0b00001111];
    }
#endif
}

void InitJoyport(void) {
}

// ----------------------------------------------------------------------
void ProcessJoyport(void) {
    static uint8_t joy0_old, joy1_old;
    static int16_t mouse_xold, mouse_yold;
    static uint8_t mouse_bold;

#if defined(BOARD_PROTO) // todo: remove once real boards arrive
    uint8_t p4 = ~P4_IN;
    uint8_t p0 = ~P0;

    uint8_t joy0 =  ((p4 & 0b00000010) >> 1) |  // up
                    ((p4 & 0b00000001) << 1) |  // down
                    ((p0 & 0b00001000) >> 1) |  // left
                    ((p0 & 0b00000100) << 1) |  // right
                    ((p4 & 0b00010000) << 3);   // fire

    uint8_t joy1 =  ((p4 & 0b00000100) >> 2) |  // up
                    ((p4 & 0b00001000) >> 2) |  // down
                    ((p0 & 0b00100000) >> 3) |  // left
                    ((p0 & 0b01000000) >> 3) |  // right
                    ((p0 & 0b10000000) << 0);   // fire
#else
    uint8_t p4 = ~P4_IN;
    uint8_t p2 = ~P2;
    uint8_t joy0 = (p4 & 0b00001111) | ((p4 & 0b00010000) << 3);
    uint8_t joy1 = (p2 & 0b00001111) | ((p2 & 0b00010000) << 3);
#endif

    // update joysticks 0
    if (joy0 != joy0_old) {
        joy0_old = joy0;
        ikbd_JoyUpdate(0, joy0);
    }

    // update joysticks 1
    if (joy1 != joy1_old) {                    
        joy1_old = joy1;
        ikbd_JoyUpdate(1, joy1);
    }

    // update mouse
    EA = 0;
    int16_t mouse_x = jmouse_xrel; jmouse_xrel = 0;
    int16_t mouse_y = jmouse_yrel; jmouse_yrel = 0;
    EA = 1;
    uint8_t mouse_b = ((joy0 & 0b10000000) >> 7) | ((joy1 & 0b10000000) >> 6);

    if ((mouse_x != mouse_xold) || (mouse_y != mouse_yold) || (mouse_b != mouse_bold)) {
        mouse_x = ScaleToIkbd(mouse_x, Settings.LegacyMouseScale);
        mouse_y = ScaleToIkbd(mouse_y, Settings.LegacyMouseScale);
        ikbd_MouseUpdate(mouse_x, mouse_y, 0, mouse_b);
        mouse_xold = mouse_x;
        mouse_yold = mouse_y;
        mouse_bold = mouse_b;
    }
}
