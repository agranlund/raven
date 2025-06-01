#ifndef __IKBD_H__
#define __IKBD_H__
#include "ps2.h"

extern void InitIkbd(void);
extern void ProcessIkbd(void);
extern void IkbdInterrupt(void) __interrupt(INT_NO_UART1);

extern void ikbd_Ps2KeyDown(uint16_t ps2key);
extern void ikbd_Ps2KeyUp(uint16_t ps2key);

extern void ikbd_MouseUpdate(int16_t x, int16_t y, int16_t z, uint8_t b);
extern void ikbd_JoyUpdate(uint8_t idx, uint8_t data);

static inline int16_t ScaleToIkbd(int16_t v, uint16_t s) {
    int32_t vv = (((int32_t)v) * s);
    return (int16_t) (vv < fint16_min) ? fint16_min : (vv > fint16_max) ? fint16_max : vv;
}


// 50hz; 512 cycles per line, 313 lines
// 160256 cycles/frame
// 20ms/frame
// 8012 cycles/ms
//
//  ikbd reset time = at least 50000 cycles = 56084 cycles = 7ms
//  ikbd send time ~1frame = 150000 cycles = 18,7ms
//  
// 60hz; 508 cycles per line, 262 lines


//--------------------------------------------------
// standard ikbd keycodes
//--------------------------------------------------
#define IKBD_KEY_NONE           0x00
#define IKBD_KEY_ESC            0x01
#define IKBD_KEY_1              0x02
#define IKBD_KEY_2              0x03
#define IKBD_KEY_3              0x04
#define IKBD_KEY_4              0x05
#define IKBD_KEY_5              0x06
#define IKBD_KEY_6              0x07
#define IKBD_KEY_7              0x08
#define IKBD_KEY_8              0x09
#define IKBD_KEY_9              0x0A
#define IKBD_KEY_0              0x0B
#define IKBD_KEY_DASH           0x0C
#define IKBD_KEY_EQUAL          0x0D
#define IKBD_KEY_BACKSPACE      0x0E
#define IKBD_KEY_TAB            0x0F
#define IKBD_KEY_Q              0x10
#define IKBD_KEY_W              0x11
#define IKBD_KEY_E              0x12
#define IKBD_KEY_R              0x13
#define IKBD_KEY_T              0x14
#define IKBD_KEY_Y              0x15
#define IKBD_KEY_U              0x16
#define IKBD_KEY_I              0x17
#define IKBD_KEY_O              0x18
#define IKBD_KEY_P              0x19
#define IKBD_KEY_LSQB           0x1A    /* left square bracket */
#define IKBD_KEY_RSQB           0x1B    /* right square bracket */
#define IKBD_KEY_RETURN         0x1C
#define IKBD_KEY_CTRL           0x1D
#define IKBD_KEY_A              0x1E
#define IKBD_KEY_S              0x1F
#define IKBD_KEY_D              0x20
#define IKBD_KEY_F              0x21
#define IKBD_KEY_G              0x22
#define IKBD_KEY_H              0x23
#define IKBD_KEY_J              0x24
#define IKBD_KEY_K              0x25
#define IKBD_KEY_L              0x26
#define IKBD_KEY_SEMICOLON      0x27
#define IKBD_KEY_APOSTROPHE     0x28
#define IKBD_KEY_TILDE          0x29
#define IKBD_KEY_LSHIFT         0x2A
#define IKBD_KEY_BSLASH         0x2B
#define IKBD_KEY_Z              0x2C
#define IKBD_KEY_X              0x2D
#define IKBD_KEY_C              0x2E
#define IKBD_KEY_V              0x2F
#define IKBD_KEY_B              0x30
#define IKBD_KEY_N              0x31
#define IKBD_KEY_M              0x32
#define IKBD_KEY_COMMA          0x33
#define IKBD_KEY_PERIOD         0x34
#define IKBD_KEY_SLASH          0x35
#define IKBD_KEY_RSHIFT         0x36
#define IKBD_KEY_UNUSED_37      0x37    /* unused */
#define IKBD_KEY_ALT            0x38
#define IKBD_KEY_SPACE          0x39
#define IKBD_KEY_CAPSLOCK       0x3A
#define IKBD_KEY_F1             0x3B
#define IKBD_KEY_F2             0x3C
#define IKBD_KEY_F3             0x3D
#define IKBD_KEY_F4             0x3E
#define IKBD_KEY_F5             0x3F
#define IKBD_KEY_F6             0x40
#define IKBD_KEY_F7             0x41
#define IKBD_KEY_F8             0x42
#define IKBD_KEY_F9             0x43
#define IKBD_KEY_F10            0x44
#define IKBD_KEY_UNUSED_45      0x45    /* unused */
#define IKBD_KEY_UNUSED_46      0x46    /* unused */
#define IKBD_KEY_HOME           0x47
#define IKBD_KEY_UP             0x48
#define IKBD_KEY_UNUSED_49      0x49    /* unused */
#define IKBD_KEY_KP_MINUS       0x4A
#define IKBD_KEY_LEFT           0x4B
#define IKBD_KEY_UNUSED_4C      0x4C    /* unused */
#define IKBD_KEY_RIGHT          0x4D
#define IKBD_KEY_KP_PLUS        0x4E
#define IKBD_KEY_UNUSED_4F      0x4F    /* unused */
#define IKBD_KEY_DOWN           0x50
#define IKBD_KEY_UNUSED_51      0x51    /* unused */
#define IKBD_KEY_INSERT         0x52
#define IKBD_KEY_DELETE         0x53
#define IKBD_KEY_UNUSED_54      0x54    /* unused */
#define IKBD_KEY_UNUSED_55      0x55    /* unused */
#define IKBD_KEY_UNUSED_56      0x56    /* unused */
#define IKBD_KEY_UNUSED_57      0x57    /* unused */
#define IKBD_KEY_UNUSED_58      0x58    /* unused */
#define IKBD_KEY_UNUSED_59      0x59    /* unused */
#define IKBD_KEY_UNUSED_5A      0x5A    /* unused */
#define IKBD_KEY_UNUSED_5B      0x5B    /* unused */
#define IKBD_KEY_UNUSED_5C      0x5C    /* unused */
#define IKBD_KEY_UNUSED_5D      0x5D    /* unused */
#define IKBD_KEY_UNUSED_5E      0x5E    /* unused */
#define IKBD_KEY_UNUSED_5F      0x5F    /* unused */
#define IKBD_KEY_ISO            0x60
#define IKBD_KEY_UNDO           0x61
#define IKBD_KEY_HELP           0x62
#define IKBD_KEY_KP_LP          0x63    /* left parenthesis )*/
#define IKBD_KEY_KP_RP          0x64    /* right parenthesis )*/
#define IKBD_KEY_KP_SLASH       0x65
#define IKBD_KEY_KP_ASTERISK    0x66
#define IKBD_KEY_KP_7           0x67
#define IKBD_KEY_KP_8           0x68
#define IKBD_KEY_KP_9           0x69
#define IKBD_KEY_KP_4           0x6A
#define IKBD_KEY_KP_5           0x6B
#define IKBD_KEY_KP_6           0x6C
#define IKBD_KEY_KP_1           0x6D
#define IKBD_KEY_KP_2           0x6E
#define IKBD_KEY_KP_3           0x6F
#define IKBD_KEY_KP_0           0x70
#define IKBD_KEY_KP_PERIOD      0x71
#define IKBD_KEY_KP_ENTER       0x72
#define IKBD_KEY_UNUSED_73      0x73    /* unused */
#define IKBD_KEY_UNUSED_74      0x74    /* unused */
#define IKBD_KEY_UNUSED_75      0x75    /* unused */
#define IKBD_KEY_UNUSED_76      0x76    /* unused */
#define IKBD_KEY_UNUSED_77      0x77    /* unused */
#define IKBD_KEY_UNUSED_78      0x78    /* unused */
#define IKBD_KEY_UNUSED_79      0x79    /* unused */
#define IKBD_KEY_UNUSED_7A      0x7A    /* unused */
#define IKBD_KEY_UNUSED_7B      0x7B    /* unused */
#define IKBD_KEY_UNUSED_7C      0x7C    /* unused */
#define IKBD_KEY_UNUSED_7D      0x7D    /* unused */
#define IKBD_KEY_UNUSED_7E      0x7E    /* unused */
#define IKBD_KEY_UNUSED_7F      0x7F    /* unused */

//--------------------------------------------------
// ikbd special codes
//--------------------------------------------------
#define IKBD_REPORT_STATUS      0xF6
#define IKBD_REPORT_MOUSE_ABS   0xF7
#define IKBD_REPORT_MOUSE_REL   0xF8        /* to 0xFB, lsbits are button status */
#define IKBD_REPORT_TIME        0xFC
#define IKBD_REPORT_JOYS        0xFD        /* both joysticks */
#define IKBD_REPORT_JOY0        0xFE        /* joystick 0 event */
#define IKBD_REPORT_JOY1        0xFF        /* joystick 1 event */

//--------------------------------------------------
// eiffel aliases
//--------------------------------------------------
#define IKBD_KEY_ALTGR          IKBD_KEY_ALT
#define IKBD_KEY_F11            IKBD_KEY_HELP
#define IKBD_KEY_F12            IKBD_KEY_UNDO

//--------------------------------------------------
// eiffel extended keycodes
//--------------------------------------------------
#define IKBD_KEY_WHEELUP        IKBD_KEY_UNUSED_59  /* can use status frame */
#define IKBD_KEY_WHEELDN        IKBD_KEY_UNUSED_5A  /* can use status frame */
#define IKBD_KEY_WHEELLT        IKBD_KEY_UNUSED_5C  /* can use status frame */
#define IKBD_KEY_WHEELRT        IKBD_KEY_UNUSED_5D  /* can use status frame */
#define IKBD_KEY_BUTTON3        IKBD_KEY_UNUSED_37
#define IKBD_KEY_BUTTON4        IKBD_KEY_UNUSED_5E
#define IKBD_KEY_BUTTON5        IKBD_KEY_UNUSED_5F

#define IKBD_KEY_SCROLL_LOCK    IKBD_KEY_UNUSED_4C
#define IKBD_KEY_PAGEUP         IKBD_KEY_UNUSED_45
#define IKBD_KEY_PAGEDOWN       IKBD_KEY_UNUSED_46
#define IKBD_KEY_END            IKBD_KEY_UNUSED_55
#define IKBD_KEY_PRINTSCREEN    IKBD_KEY_UNUSED_49
#define IKBD_KEY_PAUSE          IKBD_KEY_UNUSED_4F
#define IKBD_KEY_NUMLOCK        IKBD_KEY_UNUSED_54
#define IKBD_KEY_LWIN           IKBD_KEY_UNUSED_56
#define IKBD_KEY_RWIN           IKBD_KEY_UNUSED_57
#define IKBD_KEY_APP            IKBD_KEY_UNUSED_58

#define IKBD_KEY_POWER          IKBD_KEY_UNUSED_73  /* can use status frame */
#define IKBD_KEY_SLEEP          IKBD_KEY_UNUSED_74  /* can use status frame */
#define IKBD_KEY_WAKE           IKBD_KEY_UNUSED_75  /* can use status frame */

//--------------------------------------------------
// eiffel status frame keys
// make:  F6 05 00 00 00 00 00 (xx-80)
// break: F6 05 00 00 00 00 00 xx
//--------------------------------------------------
#define IKBD_KEY_MEDIA_NEXT     0xCD
#define IKBD_KEY_MEDIA_PREV     0x95
#define IKBD_KEY_MEDIA_STOP     0xBB
#define IKBD_KEY_MEDIA_PLAY     0xB4
#define IKBD_KEY_MEDIA_MUTE     0xA3
#define IKBD_KEY_MEDIA_VOLUP    0xB2
#define IKBD_KEY_MEDIA_VOLDN    0xA1
#define IKBD_KEY_MEDIA_SELECT   0xD0
#define IKBD_KEY_MEDIA_EMAIL    0xC8
#define IKBD_KEY_MEDIA_CALC     0xAB
#define IKBD_KEY_MEDIA_COMP     0xC0
#define IKBD_KEY_WWW_FIND       0x90
#define IKBD_KEY_WWW_HOME       0xBA
#define IKBD_KEY_WWW_BACK       0xB8
#define IKBD_KEY_WWW_FWD        0xB0
#define IKBD_KEY_WWW_STOP       0xA8
#define IKBD_KEY_WWW_REFRESH    0xA0
#define IKBD_KEY_WWW_FAVS       0x98



//--------------------------------------------------
// atari commands
//--------------------------------------------------
#define IKBD_CMD_MOUSE_ACTION               0x07    /* <00000mss> */
#define IKBD_CMD_MOUSE_MODE_RELATIVE        0x08    
#define IKBD_CMD_MOUSE_MODE_ABSOLUTE        0x09    
#define IKBD_CMD_MOUSE_MODE_KEYCODE         0x0A    /* <xmsb> <xlsb> <ymsb> <ylsb> */
#define IKBD_CMD_MOUSE_THRESHOLD            0x0B    /* <dx> <dy> */
#define IKBD_CMD_MOUSE_SCALE                0x0C    /* <x> <y> */
#define IKBD_CMD_MOUSE_POLL                 0x0D    
#define IKBD_CMD_MOUSE_POSITION             0x0E    /* <xmsb> <xlsb> <ymsb> <ylsb> */
#define IKBD_CMD_MOUSE_Y_BOTTOM             0x0F    
#define IKBD_CMD_MOUSE_Y_TOP                0x10    
#define IKBD_CMD_RESUME                     0x11    
#define IKBD_CMD_MOUSE_MODE_DISABLE         0x12    
#define IKBD_CMD_PAUSE                      0x13    
#define IKBD_CMD_JOY_MODE_EVENT             0x14    
#define IKBD_CMD_JOY_MODE_POLL              0x15    
#define IKBD_CMD_JOY_POLL                   0x16    
#define IKBD_CMD_JOY_MODE_MONITOR           0x17    /* <rate> */
#define IKBD_CMD_JOY_MODE_BUTTON            0x18    
#define IKBD_CMD_JOY_MODE_KEYCODE           0x19    /* <rx> <ry> <tx> <ty> <vx> <vy> */
#define IKBD_CMD_JOY_MODE_DISABLE           0x1A    
#define IKBD_CMD_TIME_SET                   0x1B    /* <yy> <mm> <dd> <hh> <mm> <ss> */
#define IKBD_CMD_TIME_GET                   0x1C    
#define IKBD_CMD_MEM_LOAD                   0x20    /* <addr_msb> <addr_lsb> <val> */
#define IKBD_CMD_MEM_READ                   0x21    /* <addr_msb> <addr_lsb> */
#define IKBD_CMD_MEM_EXEC                   0x22    /* <addr_msb> <addr_lsb> */
#define IKBD_CMD_RESET                      0x80    /* <0x01> */

//--------------------------------------------------
// atari status commands
//--------------------------------------------------
#define IKBD_CMD_MOUSE_GET_ACTION           0x87    
#define IKBD_CMD_MOUSE_GET_MODE_RELATIVE    0x88    
#define IKBD_CMD_MOUSE_GET_MODE_ABSOLUTE    0x89    
#define IKBD_CMD_MOUSE_GET_MODE_KEYCODE     0x8A    
#define IKBD_CMD_MOUSE_GET_THRESHOLD        0x8B    
#define IKBD_CMD_MOUSE_GET_SCALE            0x8C    
#define IKBD_CMD_MOUSE_GET_Y_BOTTOM         0x8F    
#define IKBD_CMD_MOUSE_GET_Y_TOP            0x90    
#define IKBD_CMD_MOUSE_GET_DISABLED         0x92    
#define IKBD_CMD_JOY_GET_MODE_EVENT         0x94    
#define IKBD_CMD_JOY_GET_MODE_POLL          0x95    
#define IKBD_CMD_JOY_GET_MODE_KEYCODE       0x99    
#define IKBD_CMD_JOY_GET_DISABLED           0x9A

//--------------------------------------------------
// eiffel command extensions
//--------------------------------------------------
#define IKBD_CMD_EIFFEL_GET_TEMP            0x03
#define IKBD_CMD_EIFFEL_PROG_TEMP           0x04    /* <index> <code> */
#define IKBD_CMD_EIFFEL_PROG_KEY            0x05    /* <index> <code> */
#define IKBD_CMD_EIFFEL_PROG_MOUSE          0x06    /* <index> <code> */
#define IKBD_CMD_EIFFEL_LCD                 0x23    /* <len> <data...> */

//--------------------------------------------------
// eiffel status command identifiers
//--------------------------------------------------
#define IKBD_EIFFEL_STATUS_TEMP             0x03
#define IKBD_EIFFEL_STATUS_KEY              0x05

//--------------------------------------------------
// ckbd command extensions
//--------------------------------------------------
#define IKBD_CMD_CKBD_PROG_TEMP             0x2A
#define IKBD_CMD_CKBD_PROG_SETTING          0x2B    /* <settings...> */
#define IKBD_CMD_CKBD_PROG_FIRMWARE         0x2C
#define IKBD_CMD_CKBD_BOOTLOADER            0x2D
#define IKBD_CMD_CKBD_RESET                 0x2E
#define IKBD_CMD_CKBD_POWEROFF              0x2F

//--------------------------------------------------
// ckbd status command identifiers
//--------------------------------------------------
#define IKBD_CKBD_STATUS_TEMP               0x2A
#define IKBD_CKBD_STATUS_SETTING            0x2B
#define IKBD_CKBD_STATUS_VERSION            0x2C


#endif /* __IKBD_H__ */
