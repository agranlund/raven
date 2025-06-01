//---------------------------------------------------------------------
// ps2.c
// keyboard and mouse in ps2 port
//
// todo: reset rdwrbuffer on device connect/disconnect
// and probably errors which throws the stream out of sync too
//
// todo: detect out of sync mouse packets by resetting the packet
// state if we have been stuck at the same byte for too long.
//
// likewise for keyboard if we are stuck with only an E0 or E1
// for too long.
//---------------------------------------------------------------------
#if !defined(DISABLE_PS2)

#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "ps2.h"
#include "ikbd.h"

// hardware ports
#define PS2_HWPORT          P3
#define PS2_HWDIR           P3_DIR
#define PS2M_HWBIT_CLK      2
#define PS2K_HWBIT_CLK      3
#define PS2M_HWBIT_DTA      6
#define PS2K_HWBIT_DTA      7
#define PS2M_HWMASK_CLK     (1<<PS2M_HWBIT_CLK)
#define PS2K_HWMASK_CLK     (1<<PS2K_HWBIT_CLK)
#define PS2M_HWMASK_DTA     (1<<PS2M_HWBIT_DTA)
#define PS2K_HWMASK_DTA     (1<<PS2K_HWBIT_DTA)

// common ps2
#define PS2_BUFFER_SIZE     64
#define PS2_ERROR_TIME_MS   100
#define PS2_SHIFTREG_INIT   (1<<11)
#define PS2_BUFFER_MASK     (PS2_BUFFER_SIZE-1)
#define PS2_DETECT_RATE     2000

#define PS2_REPLY_ERROR     0xFF
#define PS2_REPLY_RESEND    0xFE
#define PS2_REPLY_ACK       0xFA
#define PS2_REPLY_ECHO      0xEE

// keyboard
#define PS2K_CMD_LED        0xED
#define PS2K_CMD_ECHO       0xEE
#define PS2K_CMD_SET        0xF0
#define PS2K_CMD_ID         0xF2
#define PS2K_CMD_RESEND     0xFE
#define PS2K_CMD_RESET      0xFF
#define PS2K_CMD_DISABLE    0xF4
#define PS2K_CMD_ENABLE     0xF5

#define PS2K_FLAG_EXT0      (1<<0)
#define PS2K_FLAG_BREAK     (1<<1)
#define PS2K_FLAG_EXT1      (1<<2)

// mouse
#define PS2M_CMD_RESET      0xFF
#define PS2M_CMD_RESEND     0xFE
#define PS2M_CMD_DEFAULT    0xF6
#define PS2M_CMD_DISABLE    0xF5
#define PS2M_CMD_ENABLE     0xF4
#define PS2M_CMD_RATE       0xF3
#define PS2M_CMD_ID         0xF2
#define PS2M_CMD_REMOTE     0xF0
#define PS2M_CMD_WRAP_ON    0xEE
#define PS2M_CMD_WRAP_OFF   0xEC
#define PS2M_CMD_READ       0xEB
#define PS2M_CMD_STREAM     0xEA
#define PS2M_CMD_STATUS     0xE9
#define PS2M_CMD_RES        0xE8
#define PS2M_CMD_SCALE2     0xE7
#define PS2M_CMD_SCALE1     0xE6 

#define PS2M_TYPE_2BTN      0x00
#define PS2M_TYPE_3BTN      0x01
#define PS2M_TYPE_5BTN      0x02

#define PS2_STATE_OFF       0xff
#define PS2_STATE_BUSY      0x01
#define PS2_STATE_IDLE      0x00

static __xdata uint8_t ps2m_buf[PS2_BUFFER_SIZE];
static __xdata uint8_t ps2m_type = PS2M_TYPE_2BTN;
static __xdata volatile uint8_t ps2m_rdpos = 0;
static __xdata volatile uint8_t ps2m_wrpos = 0;
static __xdata volatile uint8_t ps2m_state = PS2_STATE_OFF;

static __xdata uint8_t ps2k_buf[PS2_BUFFER_SIZE];
static __xdata uint8_t ps2k_curleds = 0;
static __xdata uint8_t ps2k_newleds = 1;    // scroll lock on
static __xdata volatile uint8_t ps2k_rdpos = 0;
static __xdata volatile uint8_t ps2k_wrpos = 0;
static __xdata volatile uint8_t ps2k_state = PS2_STATE_OFF;


// ----------------------------------------------------------------------
// PS2 mouse clock interrupt
// ----------------------------------------------------------------------
void PS2Interrupt0(void) __interrupt(INT_NO_INT0) {
    ps2m_state = PS2_STATE_BUSY;
    static uint32_t err = 0;
    static uint16_t reg = PS2_SHIFTREG_INIT;
    static uint8_t parity = 0;

    if (err) {
        if (elapsed(err) < PS2_ERROR_TIME_MS) {
            ps2m_state = PS2_STATE_IDLE;
            err = msnow;
            return;
        } else {
            err = 0;
        }
    }

    uint16_t dta = (PS2_HWPORT >> PS2M_HWBIT_DTA) & 1;
    parity = dta ^ parity;
    reg = (reg >> 1) | (dta << 11);
    if (reg & 1) {
        // verify parity, start and stop bit
        if (parity || (reg & 2) || !((reg & (1 << 11)))) {
            TRACE("ps2 mouse err: %02x p:%d start:%d stop:%d", (reg >> 2) & 0xff, parity, (reg >> 1) & 1, (reg >> 11) & 1);
            err = msnow;
        } else {
            // store in mouse stream
            unsigned char wrpos_next = ((ps2m_wrpos + 1) & PS2_BUFFER_MASK);
            if (wrpos_next != ps2m_rdpos) {
                ps2m_buf[ps2m_wrpos] = (reg >> 2);
                ps2m_wrpos = wrpos_next;
            } else {
                // todo: handle overflow
            }
        }
        ps2m_state = PS2_STATE_IDLE;
        reg = PS2_SHIFTREG_INIT;
        parity = 0;
    }
}


// ----------------------------------------------------------------------
// PS2 keyboard clock interrupt
// ----------------------------------------------------------------------
void PS2Interrupt1(void) __interrupt(INT_NO_INT1) {
    ps2k_state = PS2_STATE_BUSY;
    static uint32_t err = 0;
    static uint16_t reg = PS2_SHIFTREG_INIT;
    static uint8_t parity = 0;

    if (err) {
        if (elapsed(err) < PS2_ERROR_TIME_MS) {
            ps2k_state = PS2_STATE_IDLE;
            err = msnow;
            return;
        } else {
            err = 0;
        }
    }

    uint16_t dta = (PS2_HWPORT >> PS2K_HWBIT_DTA) & 1;
    parity = dta ^ parity;
    reg = (reg >> 1) | (dta << 11);
    if (reg & 1) {
        // verify parity, start and stop bit
        if (parity || (reg & 2) || !((reg & (1 << 11)))) {
            TRACE("err: %02x p:%d start:%d stop:%d", (reg >> 2) & 0xff, parity, (reg >> 1) & 1, (reg >> 11) & 1);
            err = msnow;
        } else {
            // store in keyboard stream
            unsigned char wrpos_next = ((ps2k_wrpos + 1) & PS2_BUFFER_MASK);
            if (wrpos_next != ps2k_rdpos) {
                ps2k_buf[ps2k_wrpos] = (reg >> 2);
                ps2k_wrpos = wrpos_next;
            } else {
                // todo: handle overflow
            }
        }
        ps2k_state = PS2_STATE_IDLE;
        reg = PS2_SHIFTREG_INIT;
        parity = 0;
    }
}

// ----------------------------------------------------------------------
// blocking wait for input port value, with timeout
// ----------------------------------------------------------------------
static bool ps2_wait_port(const uint8_t mask, const uint8_t val) {
    uint32_t timeout = 1000;
    while(timeout) {
        if ((PS2_HWPORT & mask) == val) {
            return true;
        }
        timeout--;
    }
    return false;
}

// ----------------------------------------------------------------------
// blocking send, with timeout
// ----------------------------------------------------------------------
bool ps2_send_only(const uint8_t data, const uint8_t mask_clk, const uint8_t mask_dta) {
    PS2_HWPORT &= ~mask_clk;     // bring clock line low for at least 100us
    PS2_HWDIR |= mask_clk;
    delayus(100);
    PS2_HWPORT &= ~mask_dta;
    PS2_HWDIR |= mask_dta;  // bring data line low (start bit)
    delayus(100);
    PS2_HWDIR &= ~mask_clk; // release clock line

    uint8_t parity = 1;
    uint16_t bits = data | (1<<9);  // 8 bits + parity + stop

    for (int i=0; i<10; i++) {
        // find next bit to send
        uint8_t bit = (i == 8) ? parity : (bits & 1);
        parity ^= bit;
        bits >>= 1;
        // wait for device to bring clock low
        if (!ps2_wait_port(mask_clk, 0)) {
            goto failed;
        }
        // send bit to device
        if (bit) {
            PS2_HWPORT |= mask_dta;
        } else {
            PS2_HWPORT &= ~mask_dta;
        }
        // wait for device to release clock
        if (!ps2_wait_port(mask_clk, mask_clk)) {
            goto failed;
        }
    }

    // release data line and wait for device to pull it low
    PS2_HWDIR &= ~mask_dta;
    if (!ps2_wait_port((uint8_t)mask_dta, 0)) {
        goto failed;
    }

    // wait for device to pull clock low
    if (!ps2_wait_port(mask_clk, 0)) {
        goto failed;
    }

    // wait for device to release data+clock
    if (!ps2_wait_port((mask_clk|mask_dta), (mask_clk|mask_dta))) {
        goto failed;
    }

    return true;

failed:
    //TRACE("send fail");
     // release clock+data lines
    PS2_HWDIR &= ~(mask_clk|mask_dta);
    return false;
}

// ----------------------------------------------------------------------
// blocking receive, with timeout
// ----------------------------------------------------------------------
uint8_t ps2_recv(uint8_t mask_clk, uint8_t mask_dta) {
    uint16_t data = (1 << 11);
    while(!(data & 1)) {
        if (!ps2_wait_port(mask_clk, 0)) { goto failed; }
        data = (data >> 1) | ((PS2_HWPORT & mask_dta) ? (1 << 11) : 0);
        if (!ps2_wait_port(mask_clk, mask_clk)) { goto failed; }
    }
    return (data>>2);
failed:
    return PS2_REPLY_ERROR;
}

// ----------------------------------------------------------------------
// blocking send, with retries and timeout.
// returns reply or 0xff on failure
// ----------------------------------------------------------------------
uint8_t ps2_send(const uint8_t data, const uint8_t mask_clk, const uint8_t mask_dta) {
    uint8_t reply = PS2_REPLY_ERROR;
    uint8_t retries = 0;
    while (retries < 3) {
        if (ps2_send_only(data, mask_clk, mask_dta)) {
            reply = ps2_recv(mask_clk, mask_dta);
            if (reply < PS2_REPLY_RESEND) { return reply; }
        } retries++;
    } return reply;
}


// ----------------------------------------------------------------------
//
// mouse
//
// ----------------------------------------------------------------------
static inline void ps2mouse_di(void) { EX0 = 0; }
static inline void ps2mouse_ei(void) { IE0 = 0; IT0 = 1; EX0 = 1; }
static inline bool ps2mouse_trylock(void) { if (ps2m_state != PS2_STATE_IDLE) { return false; } ps2mouse_di(); if (ps2m_state != PS2_STATE_IDLE) { ps2mouse_ei(); return false; } return true; }
static inline void ps2mouse_lock(void) { while (1) { if (ps2mouse_trylock()) { return; } } }
static inline void ps2mouse_unlock(void) { ps2mouse_ei(); }
static inline uint8_t ps2mouse_send(const uint8_t data) { return ps2_send(data, (const uint8_t)PS2M_HWMASK_CLK, (const uint8_t)PS2M_HWMASK_DTA); }
static inline uint8_t ps2mouse_recv(void) { return ps2_recv((const uint8_t)PS2M_HWMASK_CLK, (const uint8_t)PS2M_HWMASK_DTA); }

// ----------------------------------------------------------------------
// initialize keyboard
// ----------------------------------------------------------------------
static void InitMouse(void) {
    ps2mouse_di();

    // detect
    uint8_t id = 0xFF;
    ps2m_state = PS2_STATE_OFF;
    ps2m_type = PS2M_TYPE_2BTN;
    ps2m_rdpos = ps2m_wrpos = 0;
    if (ps2mouse_send(PS2M_CMD_ID) == PS2_REPLY_ACK) {
        id = ps2mouse_recv();
        // detect Intellimouse (3 button)
        ps2mouse_send(PS2M_CMD_RATE); ps2mouse_send(200);
        ps2mouse_send(PS2M_CMD_RATE); ps2mouse_send(100);
        ps2mouse_send(PS2M_CMD_RATE); ps2mouse_send(80);
        if (ps2mouse_send(PS2M_CMD_ID) == PS2_REPLY_ACK) {
            id = ps2mouse_recv();
        }
        // detect Intellimouse (5 button)
        ps2mouse_send(PS2M_CMD_RATE); ps2mouse_send(200);
        ps2mouse_send(PS2M_CMD_RATE); ps2mouse_send(200);
        ps2mouse_send(PS2M_CMD_RATE); ps2mouse_send(80);
        if (ps2mouse_send(PS2M_CMD_ID) == PS2_REPLY_ACK) {
            id = ps2mouse_recv();
        }
        switch(id) {
            case 0x00: ps2m_type = PS2M_TYPE_2BTN; break;
            case 0x03: ps2m_type = PS2M_TYPE_3BTN; break;
            case 0x04: ps2m_type = PS2M_TYPE_5BTN; break;
                break;
            default:
                id = 0xff;
                break;
        }
    }

    if (id != 0xFF) {
        TRACE("ps2 mouse connected: $%02x%02x", ps2m_type, id);
        ps2m_state = PS2_STATE_IDLE;
        ps2mouse_send(PS2M_CMD_STREAM);
        if (ps2mouse_send(PS2M_CMD_RES) == PS2_REPLY_ACK) {
            ps2mouse_send(2);
        }
        if (ps2mouse_send(PS2M_CMD_RATE) == PS2_REPLY_ACK) {
            ps2mouse_send(40);
        }
        ps2mouse_send(PS2M_CMD_SCALE1);
        ps2mouse_send(PS2M_CMD_ENABLE);
        ps2mouse_ei();
    }
}

// ----------------------------------------------------------------------
// parse mouse stream
// ----------------------------------------------------------------------

static bool ParseMouse(void) {

    uint8_t need = (ps2m_type == PS2M_TYPE_2BTN) ? 3 : 4;
    uint8_t avail = (ps2m_rdpos < ps2m_wrpos) ? (ps2m_wrpos - ps2m_rdpos) : ((ps2m_wrpos + PS2_BUFFER_SIZE) - ps2m_rdpos);
    if (avail < need) {
        return false;
    }

    uint8_t data[4];
    for (int i=0; i<need; i++) {
        data[i] = ps2m_buf[(ps2m_rdpos + i) & PS2_BUFFER_MASK];
    }
    ps2m_rdpos = (ps2m_rdpos + need) & PS2_BUFFER_MASK;

    #define yo (data[0] & 0b10000000)
    #define xo (data[0] & 0b01000000)
    #define ys (data[0] & 0b00100000)
    #define xs (data[0] & 0b00010000)
    #define zs (data[3] & 0b00001000)


    // 0: yo xo ys xs 11 mb rb lb
    // 1: x7 x6 x5 x4 x3 x2 x1 x0
    // 2: y7 y6 y5 y4 y3 y2 y1 y0
    int16_t x =  (fint16_t) (uint16_t) ((xo ? 255 : data[1]) | (xs ? 0xff00 : 0x0000));
    int16_t y = -(fint16_t) (uint16_t) ((yo ? 255 : data[2]) | (ys ? 0xff00 : 0x0000));
    int16_t z = 0;
    uint8_t b = data[0] & 0b00000111;
    if (ps2m_type == PS2M_TYPE_3BTN) {
        // 3: z7 z6 z5 z4 z3 z2 z1 z0
        z = (int16_t) data[3];
    } else if (ps2m_type == PS2M_TYPE_5BTN) {
        // 3: 00 00 b5 b4 z3 z2 z1 z0
        z = (int8_t) ((data[3] & 7) | (zs ? 0xf8 : 0x00));
        b |= ((data[3] & 0b00110000) >> 1);
    }

    /*
    TRACE("ps2m: %d : [%02x:%02x:%02x:%02x] %d %d %d %02x", 
        ps2m_type, data[0], data[1], data[2], data[3],
        mouse.x, mouse.y, mouse.z, mouse.b);
    */

    x = ScaleToIkbd(x, Settings.PS2MouseScale);
    y = ScaleToIkbd(y, Settings.PS2MouseScale);
    z = (z << 8);
    ikbd_MouseUpdate(x, y, z, b);
    return true;
}


// ----------------------------------------------------------------------
//
// Keyboard
//
// ----------------------------------------------------------------------
static inline void ps2keyb_di(void) { EX1 = 0; }
static inline void ps2keyb_ei(void) { IE1 = 0; IT1 = 1; EX1 = 1; }
static inline bool ps2keyb_trylock(void) { if (ps2k_state != PS2_STATE_IDLE) { return false; } ps2keyb_di(); if (ps2k_state != PS2_STATE_IDLE) { ps2keyb_ei(); return false; } return true; }
static inline void ps2keyb_lock(void) { while (1) { if (ps2keyb_trylock()) { return; } } }
static inline void ps2keyb_unlock(void) { ps2keyb_ei(); }
static inline uint8_t ps2keyb_send(const uint8_t data) { return ps2_send(data, (const uint8_t)PS2K_HWMASK_CLK, (const uint8_t)PS2K_HWMASK_DTA); }
static inline uint8_t ps2keyb_recv(void) { return ps2_recv((const uint8_t)PS2K_HWMASK_CLK, (const uint8_t)PS2K_HWMASK_DTA); }

// ----------------------------------------------------------------------
// parse keyboard stream
// ----------------------------------------------------------------------
typedef struct {
    union {
        struct {
            uint8_t code;
            uint8_t flag;
        };
        uint16_t data;
    };
    uint16_t repeat;
} kbpacket_t;

static __xdata kbpacket_t ps2k_packet = {0, 0};

static bool ParseKeyboard(void) {

    // grab byte from keyboard stream
    ps2k_packet.code = ps2k_buf[ps2k_rdpos];
    ps2k_rdpos = (ps2k_rdpos + 1) & PS2_BUFFER_MASK;

    // parse byte
    if (ps2k_packet.code == 0xE0) {
        // one byte extended flag
        ps2k_packet.flag |= PS2K_FLAG_EXT0;
    } else if (ps2k_packet.code == 0xE1) {
        // two byte extended flag, only used for pause/break key
        ps2k_packet.flag |= (PS2K_FLAG_EXT0 | PS2K_FLAG_EXT1);
    } else if (ps2k_packet.code == 0xF0) {
        // release flag
        ps2k_packet.flag |= PS2K_FLAG_BREAK;
    } else if (ps2k_packet.flag & PS2K_FLAG_EXT1) {
        // handle two byte extended packet
        if (ps2k_packet.flag & PS2K_FLAG_EXT0) {
            // ignore first code
            ps2k_packet.flag &= ~PS2K_FLAG_EXT0;
        } else {
            // last code received
            // todo: we could handle pause/break key here
            ps2k_packet.data = 0;
        }
    } else {
        // handle normal or extended packet
        const uint16_t code = ps2k_packet.code | ((ps2k_packet.flag & PS2K_FLAG_EXT0) ? 0xE000 : 0x0000);
        const bool make = !(ps2k_packet.flag & PS2K_FLAG_BREAK);
        const bool repeated = (code == ps2k_packet.repeat);
        bool ignore = make && repeated;
        ps2k_packet.repeat = make ? code : repeated ? 0 : ps2k_packet.repeat;
        if (!ignore) {
            //TRACE("ps2k: %02x:%02x : %04x", ps2k_packet.code, ps2k_packet.flag, code);
            if (make) {
                ikbd_Ps2KeyDown(code);
            } else {
                ikbd_Ps2KeyUp(code);
            }
        }
/*
        if (code == 0x0e) {
            extern void IspMain(void);
            IspMain();
        }
*/        

        ps2k_packet.data = 0;
    }

    return true;
}

// ----------------------------------------------------------------------
// initialize keyboard
// ----------------------------------------------------------------------
static void InitKeyboard(void) {
    ps2keyb_di();
    ps2k_packet.data = 0;
    ps2k_packet.repeat = 0;
    ps2k_rdpos = ps2k_wrpos = 0;
    ps2keyb_send(0xF5);                 // disable scanning
    if (ps2keyb_send(PS2K_CMD_ECHO) == PS2_REPLY_ECHO) {   // detect
#ifdef DEBUG
        uint16_t id0 = 0xff;
        uint16_t id1 = 0xff;
        if (ps2keyb_send(PS2K_CMD_ID) == PS2_REPLY_ACK) {
            id0 = ps2keyb_recv();
            id1 = ps2keyb_recv();
        }
        TRACE("ps2 keyboard connected: $%02x%02x", id0, id1);
#endif
        if (ps2keyb_send(PS2K_CMD_LED) == PS2_REPLY_ACK) {  // set leds
            ps2keyb_send(ps2k_curleds);
        }
        if (ps2keyb_send(PS2K_CMD_SET) == PS2_REPLY_ACK) {  // select scancode set 2
            ps2keyb_send(0x02);
        }
        ps2k_state = PS2_STATE_IDLE;
        ps2keyb_send(0xF4); // enable scanning
        ps2keyb_ei();   // enable interrupts on clk falling edge
    }
}


// ----------------------------------------------------------------------
//
// Global interface
//
// ----------------------------------------------------------------------
void InitPS2(void) {
    P3_DIR &= ~(PS2K_HWMASK_CLK | PS2K_HWMASK_DTA | PS2M_HWMASK_CLK | PS2M_HWMASK_DTA);
    P3_PU  &= ~(PS2K_HWMASK_CLK | PS2K_HWMASK_DTA | PS2M_HWMASK_CLK | PS2M_HWMASK_DTA);
    InitMouse();
    InitKeyboard();
}

// ----------------------------------------------------------------------
void ProcessPS2(void) {
    __xdata static uint32_t keyblast = 0;
    __xdata static uint32_t mouselast = 0;

    // keyboard input
    if (ps2k_state != PS2_STATE_OFF) {
        while(ps2k_rdpos != ps2k_wrpos) {
            if (!ParseKeyboard()) {
                break;
            } else {
                keyblast = msnow;
            }
        }
        // keyboard leds
        if ((ps2k_newleds != ps2k_curleds) && ps2keyb_trylock()) {
            if (ps2keyb_send(PS2K_CMD_LED) == PS2_REPLY_ACK) {
                ps2keyb_send(ps2k_newleds);
                keyblast = msnow;
            }
            ps2keyb_unlock();
            ps2k_curleds = ps2k_newleds;
        }
    }

    // enumerate keyboard
    if (elapsed(keyblast) > PS2_DETECT_RATE) {
        keyblast = msnow;
        if (ps2k_state == PS2_STATE_OFF) {
            InitKeyboard();
        } else {
            if (ps2keyb_trylock()) {
                ps2k_packet.data = 0;
                ps2k_packet.repeat = 0;
                if (ps2keyb_send(PS2K_CMD_ECHO) == PS2_REPLY_ECHO) {
                    ps2keyb_unlock();
                } else {
                    ps2k_state = PS2_STATE_OFF;
                    TRACE("ps2 keyboard disconnected");
                }
            }
        }
    }

    // mouse input
    if (ps2m_state != PS2_STATE_OFF) {
        while (ps2m_rdpos != ps2m_wrpos) {
            if (!ParseMouse()) {
                break;
            } else {
                mouselast = msnow;
            }
        }
    }

    // enumerate mouse
    if (elapsed(mouselast) > PS2_DETECT_RATE) {
        mouselast = msnow;
        if (ps2m_state == PS2_STATE_OFF) {
            InitMouse();
        } else {
            if (ps2mouse_trylock()) {
                ps2m_rdpos = ps2m_wrpos;
                if (ps2mouse_send(PS2M_CMD_ID) == PS2_REPLY_ACK) {
                    ps2mouse_recv();
                    ps2mouse_unlock();
                } else {
                    ps2m_state = PS2_STATE_OFF;
                    TRACE("ps2 mouse disconnected");
                }
            }
        }
    }
}

void UpdatePs2KeyboardLed(uint8_t led) {
    ps2k_newleds = led;
}


#endif /* !DISABLE_PS2 */
