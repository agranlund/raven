//---------------------------------------------------------------------
// ikbd.c
// eiffel and ikbd emulation
//
//  Assumes input in the following formats:
//      Keyboard: PS2 (set2)
//      Mouse:    PS2 Intellimouse 5-button
//      Joystick: Atari (but extra buttons in bits [6:4] are ok)
//
// Device handlers are responsible for converting to the above
// formats if needed. eg; the USB keyboard handler converts from
// HID to PS2 before passing the data over here.
//
// Compatible with Eiffel (set3) keyboard usertables
//
// todo:
//  - is joy1-fire/rmb exclusive to the selected mode or does it
//    go to both regardless of mouse/joystick mode?
//  - test all the different modes and fix bugs
//  - implement the many remaining sections marked with "todo"
//
// TT/MSTe : mouse pin 5 (button3) is connected to joy1-up
//
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "temps.h"
#include "ikbd.h"

#define JOYSTICK_MODE_OFF           0
#define JOYSTICK_MODE_EVENT         1
#define JOYSTICK_MODE_INTERROGATE   2
#define JOYSTICK_MODE_MONITOR       3
#define JOYSTICK_MODE_FIREBUTTON    4
#define JOYSTICK_MODE_KEYCODE       5

#define MOUSE_MODE_OFF              0
#define MOUSE_MODE_RELATIVE         1
#define MOUSE_MODE_ABSOLUTE         2
#define MOUSE_MODE_KEYCODE          3

#define MOUSE_ACTION_ON_RELEASE     (1 << 0)
#define MOUSE_ACTION_ON_PRESS       (1 << 1)
#define MOUSE_ACTION_AS_KEYCODE     (1 << 2)

const uint32_t ikbd_baudrates[8] = { 7812, 15625, 31250, 62500, 125000, 250000, 500000, 1000000 };

//---------------------------------------------------------------------
typedef struct
{
    int32_t xabs;
    int32_t yabs;
    int16_t xrel;
    int16_t yrel;
    int16_t zrel;
    uint8_t btns;
    uint8_t babs;
} mouse_state_t;

typedef struct {
    uint8_t args;
    bool (*func)(uint8_t* data, uint8_t len);
} ikbd_cmd_t;

typedef struct {
    uint8_t key;
    uint8_t idx;
} ikbd_keymap_t;

typedef struct
{
    uint8_t mode : 2;
    uint8_t action : 3;
    uint8_t invert : 1;
    uint8_t disabled : 1;
    uint8_t xscale;
    uint8_t yscale;
    int32_t xmax;
    int32_t ymax;
    uint8_t xthreshold;     // relative mouse mode threshold
    uint8_t ythreshold;
    uint8_t xkeydelta;      // keycode mouse mode threshold
    uint8_t ykeydelta;
} ikbd_mouse_t;

typedef struct
{
    uint8_t mode : 3;
    uint8_t rate;
    uint8_t scan;
    uint8_t fire;
    uint8_t rx;
    uint8_t ry;
    uint8_t tx;
    uint8_t ty;
    uint8_t vx;
    uint8_t vy;
} ikbd_joystick_t;

typedef struct
{
    uint8_t paused : 1;
    ikbd_mouse_t mouse;
    ikbd_joystick_t joy;
} ikbd_t;

static ikbd_t ikbd;
static mouse_state_t mouse_state;
static uint8_t joy_state[2];
static uint8_t joy_state_old[2];

extern const ikbd_keymap_t ps2keymap[0x90 * 2];
extern const ikbd_cmd_t* hostcommands[16];

#define EVENT_QUEUE_SIZE 128
static uint8_t eventqueue[EVENT_QUEUE_SIZE];
static uint8_t eventqueue_pos;

#define ikbd_timeout    2000        /* mid packet timeout */
#define ikbd_rbufmask   0xff        /* interrupt receive ringbuffer */
#define ikbd_packetsize 64          /* packet buffer */

static volatile uint8_t rbuf[ikbd_rbufmask + 1];
static volatile uint8_t rbufrd;
static volatile uint8_t rbufwr;

static inline uint8_t   rbuf_avail(void) { return (rbufwr >= rbufrd) ? (rbufwr - rbufrd) : (1 + rbufwr + (ikbd_rbufmask - rbufrd)); }
static inline uint8_t   rbuf_get(void) { uint8_t d = rbuf[rbufrd]; rbufrd = ((rbufrd + 1) & ikbd_rbufmask); return d; }

static inline bool      wbuf_avail(uint8_t c)   { return (eventqueue_pos < (EVENT_QUEUE_SIZE - c)); }
static inline void      wbuf_put(uint8_t b)     { eventqueue[eventqueue_pos++] = b; }


//---------------------------------------------------------------------
static inline void ikbd_Send(unsigned char b) {
    CH559UART1SendByte(b);
}

void ikbd_QueueKey(uint8_t b) {
    if (wbuf_avail(1)) {
        wbuf_put(b);
    }
#ifdef DEBUG
    else { TRACE("ikbd_Queue full\n"); }
#endif    
}
void ikbd_QueueKeyPressRelease(uint8_t b) {
    if (wbuf_avail(2)) {
        wbuf_put(b);
        wbuf_put(b | 0x80);
    }
#ifdef DEBUG
    else { TRACE("ikbd_Queue full\n"); }
#endif    
}
void ikbd_QueueRelativeMouseEvent(void) {
    if (wbuf_avail(3)) {
        wbuf_put(IKBD_REPORT_MOUSE_REL | ((mouse_state.btns & 1) << 1) | ((mouse_state.btns >> 1) & 1));
        wbuf_put(mouse_state.xrel >> 8);
        wbuf_put(mouse_state.yrel >> 8);
        mouse_state.xrel -= (mouse_state.xrel & 0xff00);
        mouse_state.yrel -= (mouse_state.yrel & 0xff00);
    }
#ifdef DEBUG
    else { TRACE("ikbd_Queue full\n"); }
#endif    
}
void ikbd_QueueAbsoluteMouseEvent(bool interrogate) {
    uint8_t btns = mouse_state.btns & 3;
    uint8_t babs = mouse_state.babs;
    if (!interrogate && (btns == babs)) {
        return;
    }

    uint8_t buf[6];
    buf[0] = IKBD_REPORT_MOUSE_ABS;
    buf[1] = 0;
    buf[2] = ((mouse_state.xabs >> 16) & 0xff);
    buf[3] = ((mouse_state.xabs >> 8) & 0xff);
    buf[4] = ((mouse_state.yabs >> 16) & 0xff);
    buf[5] = ((mouse_state.yabs >> 8) & 0xff);

    if ( (btns & 2) && !(babs & 2)) { buf[1] |= (1 << 0); }
    if (!(btns & 2) &&  (babs & 2)) { buf[1] |= (1 << 1); }
    if ( (btns & 1) && !(babs & 1)) { buf[1] |= (1 << 2); }
    if (!(btns & 1) &&  (babs & 1)) { buf[1] |= (1 << 3); }
    mouse_state.babs = btns;

    if (interrogate) {
        for (int i=0; i<6; i++) {
            ikbd_Send(buf[i]);
        }
    } else {
        if (wbuf_avail(6)) {
            for (int i=0; i<6; i++) {
                wbuf_put(buf[i]);
            }
        }
#ifdef DEBUG
        else { TRACE("ikbd_Queue full\n"); }
#endif    
    }
}



//---------------------------------------------------------------------

void InitIkbdState(void)
{
    ikbd.paused = false;
    ikbd.joy.mode = JOYSTICK_MODE_EVENT;
    ikbd.joy.rate = 1;
    ikbd.joy.scan = 0;
    ikbd.joy.fire = 0;
    ikbd.joy.rx = 0;
    ikbd.joy.ry = 0;
    ikbd.joy.tx = 0;
    ikbd.joy.ty = 0;
    ikbd.joy.vx = 0;
    ikbd.joy.vy = 0;
    ikbd.mouse.mode = MOUSE_MODE_RELATIVE;
    ikbd.mouse.action = 0;
    ikbd.mouse.invert = false;
    ikbd.mouse.xmax = 640;
    ikbd.mouse.ymax = 480;
    ikbd.mouse.xscale = 1;
    ikbd.mouse.yscale = 1;
    ikbd.mouse.xthreshold = 1;
    ikbd.mouse.ythreshold = 1;
    ikbd.mouse.xkeydelta = 1;
    ikbd.mouse.ykeydelta = 1;

    memset((void*)&mouse_state, 0, sizeof(mouse_state_t));
    joy_state[0] = joy_state[1] = 0;
    joy_state_old[0] = joy_state_old[1] = 0;

    eventqueue_pos = 0;
    rbufrd = 0;
    rbufwr = 0;
}

void InitIkbd(void)
{
    TRACE("InitIkbd");
    InitIkbdState();
    CH559UART1Init(BAUD_IKBD);
    uint8_t lcr = SER1_LCR;         // unlock ier register
    SER1_LCR &= ~bLCR_DLAB;
    SER1_IER |= bIER_RECV_RDY;      // interrupt on recieve
    SER1_LCR = lcr;                 // restore lock
    SER1_MCR |= bMCR_OUT2;          // enable interrupt requests
    IE_UART1 = 1;                   // enable uart1 interrupts
}

void ResetIkbd(void)
{
    TRACE("ResetIkbd");
    InitIkbdState();
    // todo: some kind of delay here
    ikbd_QueueKey(0xF0);
}

void IkbdInterrupt(void) __interrupt(INT_NO_UART1) {
    while (SER1_LSR & bLSR_DATA_RDY) {
        uint8_t rbufwr_next = ((rbufwr + 1) & ikbd_rbufmask);
        if (rbufwr_next != rbufrd) {
            rbuf[rbufwr] = SER1_RBR;
            rbufwr = rbufwr_next;
        } else {
            // buffer overrun, loose incomming data
            break;
        }
    }
}

void ProcessHostCommands(void)
{
    static uint8_t avail_last = 0;
    static uint32_t timeout = 0;

    // check if anything is available from interrupt ringbufer
    uint8_t avail = rbuf_avail();

    // early out if no change since last time
    if (avail == avail_last) {
        // detect mid-packet timeout
        if (avail && elapsed(timeout) > ikbd_timeout) {
            TRACE("package timeout");
            rbufrd = rbufwr;
        }
        return;
    }

    // process all received packets
    while (avail) {
        uint8_t cmd = rbuf[rbufrd];
        const ikbd_cmd_t* page = hostcommands[cmd >> 4];
        const ikbd_cmd_t* desc = &page[cmd & 0x0f];

        static uint8_t packet[ikbd_packetsize];
        if (desc->args < avail) {
            // get data into local packet buffer
            uint8_t size = 1 + desc->args;
            for (uint8_t i=0; i<size; i++) {
                packet[i] = rbuf_get();
            }
            // call command handler
            ikbd.paused = 0;    // any command received implicitly resumes
            if (!desc->func(packet, size)) {
                // bail out if there's not enough data for variable length commands
                break;
            } else {
                // recheck avail properly in case packet func took from rbuf
                avail = rbuf_avail();
            }
        } else {
            // there's not enough data for the current command so exit
            break;
        }
    }
    avail_last = avail;

    // reset mid-packet timeout timer
    timeout = msnow;
}

void ProcessJoysticks(void) {

    if (ikbd.joy.mode == JOYSTICK_MODE_OFF) {
        // nothing to do here
    }
    else if (ikbd.joy.mode == JOYSTICK_MODE_INTERROGATE) {
        // nothing to do here
    }
    else if (ikbd.joy.mode == JOYSTICK_MODE_EVENT) {
        if (joy_state[0] != joy_state_old[0]) {
            if (wbuf_avail(2)) {
                TRACE("queue %02x %02x", IKBD_REPORT_JOY0, joy_state[0]);
                wbuf_put(IKBD_REPORT_JOY0);
                wbuf_put(joy_state[0]);
            }
        }
        if (joy_state[1] != joy_state_old[1]) {
            if (wbuf_avail(2)) {
                TRACE("queue %02x %02x", IKBD_REPORT_JOY1, joy_state[1]);
                wbuf_put(IKBD_REPORT_JOY1);
                wbuf_put(joy_state[1]);
            }
        }
    }
    else if (ikbd.joy.mode == JOYSTICK_MODE_KEYCODE) {
#if 0        
        if (wbuf_avail(2)) {
            if (joy_state[0] != joy_state_old[0]) {
                // todo: this should consider rx, tx, vx
                if (joy_state[0] & 0b00000001) {
                    if (!(joy_state_old[0] & 0b00000001)) {
                        wbuf_put(IKBD_KEY_UP); wbuf_put(IKBD_KEY_UP | 0x80);
                    }
                }
                else if (joy_state[0] & 0b00000010) {
                    if (!(joy_state_old[0] & 0b00000010)) {
                        wbuf_put(IKBD_KEY_DOWN); wbuf_put(IKBD_KEY_DOWN | 0x80);
                    }
                }
                // todo: this should consider ry, ty, vy
                if (joy_state[0] & 0b00000100) {
                    if (!(joy_state_old[0] & 0b00000100)) {
                        wbuf_put(IKBD_KEY_LEFT); wbuf_put(IKBD_KEY_LEFT | 0x80);
                    }
                }
                else if (joy_state[0] & 0b00001000) {
                    if (!(joy_state_old[0] & 0b00001000)) {
                        wbuf_put(IKBD_KEY_RIGHT); wbuf_put(IKBD_KEY_RIGHT | 0x80);
                    }
                }
                // fire button
                if ((joy_state[0] & 0b10000000) && !(joy_state_old[0] & 0b10000000)) {
                    wbuf_put(0x74); wbuf_put(0x74 | 0x80);
                }
            }
            // only fire buttons for joystick1
            if ((joy_state[1] & 0b10000000) && !(joy_state_old[1] & 0b10000000)) {
                wbuf_put(0x75); wbuf_put(0x75 | 0x80);
            }
        }
#endif            
    }
    else if (ikbd.joy.mode == JOYSTICK_MODE_MONITOR) {
        if (!ikbd.paused) { // monitoring is dropped when paused
            static uint32_t lastms = 0;
            if ((elapsed(lastms) / 10) >= ikbd.joy.rate) {
                lastms = msnow;
                if (wbuf_avail(2)) {
                    wbuf_put(((joy_state[0] & 0b10000000) >> 6) | ((joy_state[1] & 0b10000000) >> 7));
                    wbuf_put(((joy_state[0] & 0b00001111) << 4) | (joy_state[1] & 0b00001111));
                }
            }
        }
    }
    else if (ikbd.joy.mode == JOYSTICK_MODE_FIREBUTTON) {
        if (!ikbd.paused) { // monitoring is dropped when paused
            // todo: this is technically not the correct scanrate but who cares
            static uint32_t lastms = 0;
            if (msnow != lastms) {
                lastms = msnow;
                ikbd.joy.fire >>= 1;
                ikbd.joy.fire |= (joy_state[1] & 0b10000000);
                ikbd.joy.scan = (ikbd.joy.scan + 1) & 7;
                if (ikbd.joy.scan == 0) {
                    if (wbuf_avail(1)) {
                        wbuf_put(ikbd.joy.fire);
                    }
                    ikbd.joy.fire = 0;
                }
            }
        }
    }

    joy_state_old[0] = joy_state[0];
    joy_state_old[1] = joy_state[1];
}

void QueueEiffelStatusFrameKey(uint8_t key) {
    if (wbuf_avail(8) && (key > 0x00) && (key < 0x80)) {
        ikbd_QueueKey(IKBD_REPORT_STATUS);
        ikbd_QueueKey(IKBD_EIFFEL_STATUS_KEY);
        ikbd_QueueKey(0x00);
        ikbd_QueueKey(0x00);
        ikbd_QueueKey(0x00);
        ikbd_QueueKey(0x00);
        ikbd_QueueKey(0x00);
        ikbd_QueueKey(key);
    }
}

void ProcessMouse(void) {
    static uint8_t btns_old;

    // no mouse input when disabled, or when ikbd is in joystick monitor mode
    if ((ikbd.mouse.mode == MOUSE_MODE_OFF) || (ikbd.joy.mode == JOYSTICK_MODE_MONITOR) || (ikbd.joy.mode == JOYSTICK_MODE_FIREBUTTON)) {
        return;
    }

    uint8_t btns = mouse_state.btns;

    // event reporting
    if (ikbd.mouse.mode == MOUSE_MODE_ABSOLUTE) {
        mouse_state.xrel = 0;
        mouse_state.yrel = 0;
    }
    else
    {
        int32_t xticks = (mouse_state.xrel >> 8);
        int32_t yticks = (mouse_state.yrel >> 8);
        mouse_state.xrel -= (xticks << 8);
        mouse_state.yrel -= (yticks << 8);
        if (ikbd.mouse.mode == MOUSE_MODE_RELATIVE)
        {
            if ((ABS(xticks) >= ikbd.mouse.xthreshold) || ((ABS(yticks) >= ikbd.mouse.ythreshold)) || (btns != btns_old))
            {
                if (wbuf_avail(3))
                {
                    uint8_t cmd = 0b11111000 | ((btns & 1) << 1) | ((btns >> 1) & 1);
                    TRACE("queue %02x %ld %ld", cmd, xticks, yticks);
                    wbuf_put(cmd);
                    wbuf_put(xticks);
                    wbuf_put(yticks);
                }
            }
        }
        else if (ikbd.mouse.mode == MOUSE_MODE_KEYCODE)
        {
            if (ikbd.mouse.xkeydelta)
            {
                while (xticks >= ikbd.mouse.xkeydelta) {
                    ikbd_QueueKeyPressRelease(IKBD_KEY_RIGHT);
                    xticks--;
                }
                while (xticks <= -ikbd.mouse.xkeydelta) {
                    ikbd_QueueKeyPressRelease(IKBD_KEY_LEFT);
                    xticks++;
                }
            }
            if (ikbd.mouse.ykeydelta)
            {
                while (yticks >= ikbd.mouse.xkeydelta) {
                    ikbd_QueueKeyPressRelease(IKBD_KEY_DOWN);
                    yticks--;
                }
                while (yticks <= -ikbd.mouse.xkeydelta) {
                    ikbd_QueueKeyPressRelease(IKBD_KEY_UP);
                    yticks++;
                }
            }
        }
    }

    // button action: absolute report
    if ((ikbd.mouse.action & 3) && (ikbd.mouse.mode == MOUSE_MODE_ABSOLUTE)) {
        ikbd_QueueAbsoluteMouseEvent(false);
    }

    // button action: keycode
    #define mbtn_down(x)            (btns&(1<<(x)))
    #define mbtn_changed(x)         ((btns&(1<<(x))) != (btns_old&(1<<(x))))
    #define eiffel_code_valid(x)    ((x > 0x00) & (x < 0x80))
    if (((ikbd.mouse.action & (1 << 2)) || (ikbd.mouse.mode == MOUSE_MODE_KEYCODE))) {
        if (mbtn_changed(0)) { ikbd_QueueKey(0x74 | mbtn_down(0) ? 0x00 : 0x80); }
        if (mbtn_changed(1)) { ikbd_QueueKey(0x75 | mbtn_down(1) ? 0x00 : 0x80); }
    }
 
    // eiffel buttons 3,4,5
    if (mbtn_changed(2) && eiffel_code_valid(Settings.EiffelMouse.Button3)) { ikbd_QueueKey(Settings.EiffelMouse.Button3 | mbtn_down(2) ? 0x00 : 0x80); }
    if (mbtn_changed(3) && eiffel_code_valid(Settings.EiffelMouse.Button4)) { ikbd_QueueKey(Settings.EiffelMouse.Button4 | mbtn_down(3) ? 0x00 : 0x80); }
    if (mbtn_changed(4) && eiffel_code_valid(Settings.EiffelMouse.Button5)) { ikbd_QueueKey(Settings.EiffelMouse.Button5 | mbtn_down(4) ? 0x00 : 0x80); }

    // eiffel scrollwheel
    int16_t wheely = (mouse_state.zrel >> 8) * Settings.EiffelMouse.WheelRepeat;
    mouse_state.zrel -= (mouse_state.zrel & 0xff00);
    if (wheely != 0)
    {
        #define wheelrepeat_max 7
        if (wheely < 0) {
            for (wheely = (wheely < -wheelrepeat_max) ? -wheelrepeat_max : wheely; wheely != 0; wheely++) {
                if (wbuf_avail(2) && eiffel_code_valid(Settings.EiffelMouse.WheelUp)) {
                    ikbd_QueueKey(Settings.EiffelMouse.WheelUp);
                    ikbd_QueueKey(Settings.EiffelMouse.WheelUp | 0x80);
                }
                //QueueEiffelStatusFrameKey(IKBD_KEY_WHEELUP);
            }
        }
        else {
            for (wheely = (wheely > wheelrepeat_max) ? wheelrepeat_max : wheely; wheely != 0; wheely--) {
                if (wbuf_avail(2) && eiffel_code_valid(Settings.EiffelMouse.WheelDown)) {
                    ikbd_QueueKey(Settings.EiffelMouse.WheelDown);
                    ikbd_QueueKey(Settings.EiffelMouse.WheelDown | 0x80);
                }
                //QueueEiffelStatusFrameKey(IKBD_KEY_WHEELDN);
            }
        }
    }

    // simulate TT/MSTe 3-button mouse behavior (mouse:mmb -> joy1:up)
    ikbd_JoyUpdate(1, joy_state[1] | ((btns >> 2) & 0b00000001));

    btns_old = btns;
}


void ProcessIkbd(void)
{
    ProcessHostCommands();

    static uint32_t report_time = 0;
    const uint32_t report_ms = 8;
    if (elapsed(report_time) >= report_ms) {
        report_time = msnow;
        ProcessJoysticks();
        ProcessMouse();
    }

    // flush queued event reports
    if (eventqueue_pos && !ikbd.paused) {
        for (int i=0; i<eventqueue_pos; i++) {
            ikbd_Send(eventqueue[i]);
        }
        eventqueue_pos = 0;
    }
}


//---------------------------------------------------------------------
// joystick input
// expected in Atari format, with support for additional buttons
//
// bits:
//  7 = button1
//  6 = button2
//  5 = button3
//  4 = button4
//  3 = right
//  2 = left
//  1 = down
//  0 = up
//---------------------------------------------------------------------
void ikbd_JoyUpdate(uint8_t idx, uint8_t data) {
    joy_state[idx & 1] = data;
}

//---------------------------------------------------------------------
// mouse input
//---------------------------------------------------------------------
void ikbd_MouseUpdate(int16_t x, int16_t y, int16_t z, uint8_t b) {
    // update relative and absolute counters
    y = ikbd.mouse.invert ? -y : y;
    int32_t xrel = CLAMP((mouse_state.xrel + x), -(127<<8), (127<<8));
    int32_t yrel = CLAMP((mouse_state.yrel + y), -(127<<8), (127<<8));
    int32_t zrel = CLAMP((mouse_state.zrel + z), -(127<<8), (127<<8));
    if (ikbd.mouse.xscale > 0) xrel /= ikbd.mouse.xscale;
    if (ikbd.mouse.yscale > 0) yrel /= ikbd.mouse.yscale;
    mouse_state.xabs = CLAMP((mouse_state.xabs + xrel), 0, ikbd.mouse.xmax);
    mouse_state.yabs = CLAMP((mouse_state.yabs + yrel), 0, ikbd.mouse.ymax);
    mouse_state.xrel = xrel;
    mouse_state.yrel = yrel;
    mouse_state.zrel = zrel;

    // process directly if button status was changed
    if (mouse_state.btns != b) {
        ProcessMouse();
        mouse_state.btns = b;
    }
}


//---------------------------------------------------------------------
// keyboard input
//---------------------------------------------------------------------

void ikbd_AtariKey(uint8_t key) {
    if ((key & 0x7f) == 0) {
        return;
    }

    // no keyboard when in joystick monitoring mode
    if ((ikbd.joy.mode == JOYSTICK_MODE_MONITOR) || (ikbd.joy.mode == JOYSTICK_MODE_FIREBUTTON)) {
        return;
    }

    TRACE("ikbd: st %02x", key);
    if (wbuf_avail(1)) {
        wbuf_put(key);
    }
}

void ikbd_AtariKeyDown(uint8_t key) {
    if (key < 0x80) {
        ikbd_AtariKey(key);
    } else {
        // todo: eiffel status frame key
    }
}

void ikbd_AtariKeyUp(uint8_t key) {
    if (key < 0x80) {
        ikbd_AtariKey(key | 0x80);
    } else {
        // todo: eiffel status frame key
    }
}

uint8_t ikbd_Ps2ToAtariKey(uint16_t ps2key) {
    uint16_t tablepos = ps2key & 0xff;
    if (tablepos >= 0x90) {
        return 0;
    }
    if ((ps2key & 0xff00) == 0xE000) {
        tablepos += 0x90;
    }

    // look up atari key from usertable
    uint8_t atarikey = ps2keymap[tablepos].idx;
    if (atarikey) {
        atarikey = Settings.EiffelKeymap[atarikey];
    }

    // look up atari key from default table
    if (!atarikey) {
        atarikey = ps2keymap[tablepos].key;
    }

    TRACE("ikbd: ps2 %04x offs %04x key %02x", ps2key, tablepos, atarikey);
    return atarikey;
}

void ikbd_Ps2KeyDown(uint16_t ps2key) {
    TRACE("make  ");
    ikbd_AtariKeyDown(ikbd_Ps2ToAtariKey(ps2key));
}

void ikbd_Ps2KeyUp(uint16_t ps2key) {
    TRACE("break ");
    ikbd_AtariKeyUp(ikbd_Ps2ToAtariKey(ps2key));
}


//---------------------------------------------------------------------
//
// commands
//
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// IKBD commands
//---------------------------------------------------------------------
bool cmd_null(uint8_t* data, uint8_t size) {        // null
    return true;
}
bool cmd_0x07(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_ACTION
    ikbd.mouse.action = data[1];
    return true;
}
bool cmd_0x08(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_MODE_RELATIVE
    ikbd.mouse.mode = MOUSE_MODE_RELATIVE;
    return true;
}
bool cmd_0x09(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_MODE_ABSOLUTE
    ikbd.mouse.mode = MOUSE_MODE_ABSOLUTE;
    ikbd.mouse.xmax = ((uint32_t)data[1]<<16 | (uint32_t)data[2]<<8);
    ikbd.mouse.ymax = ((uint32_t)data[3]<<16 | (uint32_t)data[4]<<8);
    mouse_state.babs = mouse_state.btns;
    mouse_state.xabs = 0;
    mouse_state.yabs = 0;
    return true;
}

bool cmd_0x0A(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_MODE_KEYCODE
    ikbd.mouse.mode = MOUSE_MODE_KEYCODE;
    ikbd.mouse.xkeydelta = data[1];
    ikbd.mouse.ykeydelta = data[2];
    return true;
}
bool cmd_0x0B(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_THRESHOLD
    if (ikbd.mouse.mode == MOUSE_MODE_RELATIVE) {
        ikbd.mouse.xthreshold = data[1];
        ikbd.mouse.xthreshold = data[2];
    }
    return true;
}
bool cmd_0x0C(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_SCALE
    if (ikbd.mouse.mode == MOUSE_MODE_ABSOLUTE) {
        ikbd.mouse.xscale = data[1];
        ikbd.mouse.yscale = data[2];
    }
    return true;
}
bool cmd_0x0D(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_POLL
    if (ikbd.mouse.mode == MOUSE_MODE_ABSOLUTE) {
        ikbd_QueueAbsoluteMouseEvent(true);
    }
    return true;
}
bool cmd_0x0E(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_POSITION
    if (ikbd.mouse.mode == MOUSE_MODE_ABSOLUTE) {
        mouse_state.xabs = (((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 8));
        mouse_state.yabs = (((uint32_t)data[4] << 16) | ((uint32_t)data[5] << 8));
        if (mouse_state.xabs > ikbd.mouse.xmax) mouse_state.xabs = ikbd.mouse.xmax;
        if (mouse_state.yabs > ikbd.mouse.ymax) mouse_state.yabs = ikbd.mouse.ymax;
    }
    return true;
}
bool cmd_0x0F(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_Y_BOTTOM
    ikbd.mouse.invert = true;
    return true;
}
bool cmd_0x10(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_Y_TOP
    ikbd.mouse.invert = false;
    return true;
}
bool cmd_0x12(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_MODE_DISABLE
    ikbd.mouse.mode = MOUSE_MODE_OFF;
    return true;
}
bool cmd_0x13(uint8_t* data, uint8_t size) {        // IKBD_CMD_PAUSE
    ikbd.paused = true;
    return true;
}
bool cmd_0x14(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_MODE_EVENT
    ikbd.mouse.mode = MOUSE_MODE_OFF;
    ikbd.joy.mode = JOYSTICK_MODE_EVENT;
    return true;
}
bool cmd_0x15(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_MODE_POLL
    ikbd.mouse.mode = MOUSE_MODE_OFF;
    ikbd.joy.mode = JOYSTICK_MODE_INTERROGATE;
    return true;
}
bool cmd_0x16(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_POLL
    ikbd_Send(IKBD_REPORT_JOYS);
    ikbd_Send((ikbd.mouse.mode == MOUSE_MODE_OFF) ? joy_state[0] : 0x00);
    ikbd_Send(joy_state[1]);
    return true;
}
bool cmd_0x17(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_MODE_MONITOR
    ikbd.mouse.mode = MOUSE_MODE_OFF;
    ikbd.joy.mode = JOYSTICK_MODE_MONITOR;
    ikbd.joy.rate = data[1];
    return true;
}
bool cmd_0x18(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_MODE_BUTTON
    ikbd.mouse.mode = MOUSE_MODE_OFF;
    ikbd.joy.mode = JOYSTICK_MODE_FIREBUTTON;
    return true;
}
bool cmd_0x19(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_MODE_KEYCODE
    ikbd.mouse.mode = MOUSE_MODE_OFF;
    ikbd.joy.mode = JOYSTICK_MODE_KEYCODE;
    ikbd.joy.rx = data[1];
    ikbd.joy.ry = data[2];
    ikbd.joy.tx = data[3];
    ikbd.joy.ty = data[4];
    ikbd.joy.vx = data[5];
    ikbd.joy.vy = data[6];
    return true;
}
bool cmd_0x1A(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_MODE_DISABLE
    ikbd.joy.mode = JOYSTICK_MODE_OFF;
    return true;
}
bool cmd_0x1B(uint8_t* data, uint8_t size) {        // IKBD_CMD_TIME_SET
    // unsupported
    // yy = data[1];
    // mm = data[2];
    // dd = data[3];
    // hh = data[4];
    // mm = data[5];
    // ss = data[6];
    return true;
}
bool cmd_0x1C(uint8_t* data, uint8_t size) {        // IKBD_CMD_TIME_GET
    // unsupported
    ikbd_Send(IKBD_REPORT_TIME);
    ikbd_Send(0x00);    // yy
    ikbd_Send(0x00);    // mm
    ikbd_Send(0x00);    // dd
    ikbd_Send(0x00);    // hh
    ikbd_Send(0x00);    // mm
    ikbd_Send(0x00);    // ss
    return true;
}
bool cmd_0x20(uint8_t* data, uint8_t size) {        // IKBD_CMD_MEM_LOAD
    // unsupported, consumes but does not store the data
    uint8_t count = data[3];
    uint8_t avail = rbuf_avail();
    if (avail >= count) {
        //uint16_t addr = (data[1] << 8) | data[2];
        for (int i=0; i<count; i++) {
            (void)rbuf_get();
        }
        return true;
    }
    return false;
}
bool cmd_0x21(uint8_t* data, uint8_t size) {        // IKBD_CMD_MEM_READ
    // unsupported, just returns zeroes
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(0x20);
    //uint16_t addr = (data[1] << 8) | data[2];
    for (int i=0; i<6; i++) {
        ikbd_Send(0x00);
    }
    return true;
}
bool cmd_0x22(uint8_t* data, uint8_t size) {        // IKBD_CMD_MEM_EXEC
    // unsupported, cannot execute 6301 code
    return true;
}
bool cmd_0x80(uint8_t* data, uint8_t size) {        // IKBD_CMD_RESET
    ResetIkbd();    
    return true;
}

//---------------------------------------------------------------------
// IKBD status commands
//---------------------------------------------------------------------

bool cmd_0x87(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_GET_ACTION
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(0x07);
    ikbd_Send(ikbd.mouse.action);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}
bool cmd_0x88(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_GET_MODE_RELATIVE
    ikbd_Send(IKBD_REPORT_STATUS);                  // IKBD_CMD_MOUSE_GET_MODE_ABSOLUTE
    switch (ikbd.mouse.mode) {                      // IKBD_CMD_MOUSE_GET_MODE_KEYCODE
        case MOUSE_MODE_RELATIVE:
            ikbd_Send(0x08);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            break;
        case MOUSE_MODE_ABSOLUTE:
            ikbd_Send(0x09);
            ikbd_Send((ikbd.mouse.xmax >> 16) & 0xff);
            ikbd_Send((ikbd.mouse.xmax >>  8) & 0xff);
            ikbd_Send((ikbd.mouse.ymax >> 16) & 0xff);
            ikbd_Send((ikbd.mouse.ymax >>  8) & 0xff);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            break;
        case MOUSE_MODE_KEYCODE:
            ikbd_Send(0x0A);
            ikbd_Send(ikbd.mouse.xkeydelta);
            ikbd_Send(ikbd.mouse.ykeydelta);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            break;
        default:
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            ikbd_Send(0x00);
            break;
    }
    return true;
}
bool cmd_0x8B(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_GET_THRESHOLD
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(0x0B);
    ikbd_Send(ikbd.mouse.xthreshold);
    ikbd_Send(ikbd.mouse.ythreshold);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}
bool cmd_0x8C(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_GET_SCALE
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(0x0C);
    ikbd_Send(ikbd.mouse.xscale);
    ikbd_Send(ikbd.mouse.yscale);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}
bool cmd_0x8F(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_GET_Y_BOTTOM
    ikbd_Send(IKBD_REPORT_STATUS);                  // IKBD_CMD_MOUSE_GET_Y_TOP
    ikbd_Send(ikbd.mouse.invert ? 0x0F : 0x10);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}
bool cmd_0x92(uint8_t* data, uint8_t size) {        // IKBD_CMD_MOUSE_GET_DISABLED
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send((ikbd.mouse.mode == MOUSE_MODE_OFF) ? 0x12 : 0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}
bool cmd_0x94(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_GET_MODE_EVENT
    ikbd_Send(IKBD_REPORT_STATUS);                  // IKBD_CMD_JOY_GET_MODE_POLL
    switch (ikbd.joy.mode) {                        // IKBD_CMD_JOY_GET_MODE_KEYCODE
        case JOYSTICK_MODE_EVENT:
            ikbd_Send(0x14);
            break;
        case JOYSTICK_MODE_INTERROGATE:
            ikbd_Send(0x15);
            break;
        case JOYSTICK_MODE_KEYCODE:
            ikbd_Send(0x19);
            break;
        default:
            ikbd_Send(0x00);
            break;
    }
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}
bool cmd_0x9A(uint8_t* data, uint8_t size) {        // IKBD_CMD_JOY_GET_DISABLED
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send((ikbd.joy.mode == JOYSTICK_MODE_OFF) ? 0x1A : 0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    ikbd_Send(0x00);
    return true;
}

//---------------------------------------------------------------------
// Eiffel commands
//---------------------------------------------------------------------
bool cmd_0x03(uint8_t* data, uint8_t size) {        // IKBD_CMD_EIFFEL_GET_TEMP
    // temp1 (eiffel)
    uint16_t adc; uint8_t res, deg, status;
    GetTemps(0, &status, &adc, &res, &deg);
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(IKBD_EIFFEL_STATUS_TEMP);
    ikbd_Send(deg);
    ikbd_Send(adc >> 3);
    ikbd_Send((status & TEMP_STATUS_FAN0) ? 0x01 : 0x00);
    ikbd_Send(Settings.EiffelTemp[0].High);
    ikbd_Send(Settings.EiffelTemp[0].Low);
    ikbd_Send(res);
    // temp2 (ckbd extension)
    GetTemps(1, &status, &adc, &res, &deg);
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(IKBD_CKBD_STATUS_TEMP);
    ikbd_Send(deg);
    ikbd_Send(adc >> 3);
    ikbd_Send((status & TEMP_STATUS_FAN1) ? 0x01 : 0x00);
    ikbd_Send(Settings.EiffelTemp[1].High);
    ikbd_Send(Settings.EiffelTemp[1].Low);
    ikbd_Send(res);
    // version (ckbd extension)
    ikbd_Send(IKBD_REPORT_STATUS);
    ikbd_Send(IKBD_CKBD_STATUS_VERSION);
#if defined(BOARD_RAVEN_A1) || defined(BOARD_PROTO)
    ikbd_Send(0xA1);
#elif defined(BOARD_RAVEN_A2)
    ikbd_Send(0xA2);
#else
    ikbd_Send(0x00);
#endif
    ikbd_Send(0x00);
    ikbd_Send((BUILD_VERSION & 0xff000000) >> 24);
    ikbd_Send((BUILD_VERSION & 0x00ff0000) >> 16);
    ikbd_Send((BUILD_VERSION & 0x0000ff00) >>  8);
    ikbd_Send((BUILD_VERSION & 0x000000ff) >>  0);
    return true;
}
bool cmd_0x04(uint8_t* data, uint8_t size) {        // IKBD_CMD_EIFFEL_PROG_TEMP
    uint8_t idx = data[1];
    uint8_t val = data[2];
    if (idx == 0) {
        Settings.EiffelTemp[0].High = val;
    } else if (idx == 1) {
        Settings.EiffelTemp[0].Low = val;
    } else if (idx < 26) {
        idx -= 2;
        if (idx & 1) {
            Settings.EiffelTemp[0].Temp[idx >> 1] = val;
        } else {
            Settings.EiffelTemp[0].Rctn[idx >> 1] = val;
        }
    }
    Settings.Changed++;
    return true;
}
bool cmd_0x05(uint8_t* data, uint8_t size) {        // IKBD_CMD_EIFFEL_PROG_KEY
    // todo
    // idx = data[1];
    // val = data[2];
    // if idx is 0xff, val is set2 or set3
    return true;
}
bool cmd_0x06(uint8_t* data, uint8_t size) {        // IKBD_CMD_EIFFEL_PROG_MOUSE
    // todo
    // idx = data[1];
    // val = data[2];
    return true;
}
bool cmd_0x23(uint8_t* data, uint8_t size) {        // IKBD_CMD_EIFFEL_LCD
    uint8_t len = data[1];
    if (len == 0x00) {
        // unsupported: lock lcd command
        return true;
    } else if (len == 0xff) {
        // unsupported: unlock lcd command
        return true;
    } else {
        // unsupported: send lcd data
        uint8_t avail = rbuf_avail();
        if (avail >= len) {
            for (int i=0; i<len; i++) {
                (void)rbuf_get();
            }
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------
// CKBD commands
//---------------------------------------------------------------------
bool cmd_0x2A(uint8_t* data, uint8_t size) {        // IKBD_CMD_CKBD_PROG_TEMP
    // todo
    // idx = data[1];
    // val = data[2];
    return true;
}
bool cmd_0x2B(uint8_t* data, uint8_t size) {        // IKBD_CMD_CKBD_PROG_SETTING
    uint8_t idx = data[1];
    uint8_t val = data[2];
    if ((idx == 0xff) && (val == 0xff)) {
        // reset all settings to default
        InitSettings(true);
    }
    return true;
}
bool cmd_0x2C(uint8_t* data, uint8_t size) {        // IKBD_CMD_CKBD_PROG_FIRMWARE
    // todo
    return true;
}
bool cmd_0x2D(uint8_t* data, uint8_t size) {        // IKBD_CMD_CKBD_BOOTLOADER
    bool bootloader = (data[1] != 0) ? true : false;
    reset(bootloader);
    return true;
}
bool cmd_0x2E(uint8_t* data, uint8_t size) {        // IKBD_CMD_CKBD_RESET
    // unsupported
    return true;
}
bool cmd_0x2F(uint8_t* data, uint8_t size) {        // IKBD_CMD_CKBD_POWEROFF
    // todo
    return true;
}


//---------------------------------------------------------------------
// command function jumptable
//---------------------------------------------------------------------
const ikbd_cmd_t hostcommands_null[16] = {
    {0, cmd_null},      // 0x0 : 
    {0, cmd_null},      // 0x1 : 
    {0, cmd_null},      // 0x2 : 
    {0, cmd_null},      // 0x3 : 
    {0, cmd_null},      // 0x4 : 
    {0, cmd_null},      // 0x5 : 
    {0, cmd_null},      // 0x6 : 
    {0, cmd_null},      // 0x7 : 
    {0, cmd_null},      // 0x8 : 
    {0, cmd_null},      // 0x9 : 
    {0, cmd_null},      // 0xA : 
    {0, cmd_null},      // 0xB : 
    {0, cmd_null},      // 0xC : 
    {0, cmd_null},      // 0xD : 
    {0, cmd_null},      // 0xE : 
    {0, cmd_null},      // 0xF : 
};

const ikbd_cmd_t hostcommands_0x00[16] = {
    {0, cmd_null},      // 0x00 :
    {0, cmd_null},      // 0x01 :
    {0, cmd_null},      // 0x02 :
    {0, cmd_0x03},      // 0x03 : IKBD_CMD_EIFFEL_GET_TEMP
    {2, cmd_0x04},      // 0x04 : IKBD_CMD_EIFFEL_PROG_TEMP
    {2, cmd_0x05},      // 0x05 : IKBD_CMD_EIFFEL_PROG_KEY
    {2, cmd_0x06},      // 0x06 : IKBD_CMD_EIFFEL_PROG_MOUSE
    {1, cmd_0x07},      // 0x07 : IKBD_CMD_MOUSE_ACTION
    {0, cmd_0x08},      // 0x08 : IKBD_CMD_MOUSE_MODE_RELATIVE
    {4, cmd_0x09},      // 0x09 : IKBD_CMD_MOUSE_MODE_ABSOLUTE
    {2, cmd_0x0A},      // 0x0A : IKBD_CMD_MOUSE_MODE_KEYCODE
    {2, cmd_0x0B},      // 0x0B : IKBD_CMD_MOUSE_THRESHOLD
    {2, cmd_0x0C},      // 0x0C : IKBD_CMD_MOUSE_SCALE
    {0, cmd_0x0D},      // 0x0D : IKBD_CMD_MOUSE_POLL
    {5, cmd_0x0E},      // 0x0E : IKBD_CMD_MOUSE_POSITION
    {0, cmd_0x0F},      // 0x0F : IKBD_CMD_MOUSE_Y_BOTTOM
};
const ikbd_cmd_t hostcommands_0x10[16] = {
    {0, cmd_0x10},      // 0x10 : IKBD_CMD_MOUSE_Y_TOP
    {0, cmd_null},      // 0x11 : IKBD_CMD_RESUME
    {0, cmd_0x12},      // 0x12 : IKBD_CMD_MOUSE_MODE_DISABLE
    {0, cmd_0x13},      // 0x13 : IKBD_CMD_PAUSE
    {0, cmd_0x14},      // 0x14 : IKBD_CMD_JOY_MODE_EVENT
    {0, cmd_0x15},      // 0x15 : IKBD_CMD_JOY_MODE_POLL
    {0, cmd_0x16},      // 0x16 : IKBD_CMD_JOY_POLL
    {1, cmd_0x17},      // 0x17 : IKBD_CMD_JOY_MODE_MONITOR
    {0, cmd_0x18},      // 0x18 : IKBD_CMD_JOY_MODE_BUTTON
    {6, cmd_0x19},      // 0x19 : IKBD_CMD_JOY_MODE_KEYCODE
    {0, cmd_0x1A},      // 0x1A : IKBD_CMD_JOY_MODE_DISABLE
    {6, cmd_0x1B},      // 0x1B : IKBD_CMD_TIME_SET
    {0, cmd_0x1C},      // 0x1C : IKBD_CMD_TIME_GET
    {0, cmd_null},      // 0x1D :
    {0, cmd_null},      // 0x1E :
    {0, cmd_null},      // 0x1F :
};
const ikbd_cmd_t hostcommands_0x20[16] = {
    {3, cmd_0x20},      // 0x20 : IKBD_CMD_MEM_LOAD
    {2, cmd_0x21},      // 0x21 : IKBD_CMD_MEM_READ
    {2, cmd_0x22},      // 0x22 : IKBD_CMD_MEM_EXEC
    {1, cmd_0x23},      // 0x23 : IKBD_CMD_EIFFEL_LCD
    {0, cmd_null},      // 0x24 : 
    {0, cmd_null},      // 0x25 :
    {0, cmd_null},      // 0x26 :
    {0, cmd_null},      // 0x27 :
    {0, cmd_null},      // 0x28 :
    {0, cmd_null},      // 0x29 : 
    {2, cmd_0x2A},      // 0x2A : IKBD_CMD_CKBD_PROG_TEMP
    {2, cmd_0x2B},      // 0x2B : IKBD_CMD_CKBD_PROG_SETTING
    {0, cmd_0x2C},      // 0x2C : IKBD_CMD_CKBD_PROG_FIRMWARE
    {1, cmd_0x2D},      // 0x2D : IKBD_CMD_CKBD_BOOTLOADER
    {0, cmd_0x2E},      // 0x2E : IKBD_CMD_CKBD_RESET
    {0, cmd_0x2F},      // 0x2F : IKBD_CMD_CKBD_POWEROFF
};
const ikbd_cmd_t hostcommands_0x80[16] = {
    {1, cmd_0x80},      // 0x80 : IKBD_CMD_RESET
    {0, cmd_null},      // 0x81 : 
    {0, cmd_null},      // 0x82 : 
    {0, cmd_null},      // 0x83 : 
    {0, cmd_null},      // 0x84 : 
    {0, cmd_null},      // 0x85 : 
    {0, cmd_null},      // 0x86 : 
    {0, cmd_0x87},      // 0x87 : IKBD_CMD_MOUSE_GET_ACTION
    {0, cmd_0x88},      // 0x88 : IKBD_CMD_MOUSE_GET_MODE_RELATIVE
    {0, cmd_0x88},      // 0x89 : IKBD_CMD_MOUSE_GET_MODE_ABSOLUTE (same as 0x88)
    {0, cmd_0x88},      // 0x8A : IKBD_CMD_MOUSE_GET_MODE_KEYCODE (same as 0x88)
    {0, cmd_0x8B},      // 0x8B : IKBD_CMD_MOUSE_GET_THRESHOLD
    {0, cmd_0x8C},      // 0x8C : IKBD_CMD_MOUSE_GET_SCALE
    {0, cmd_null},      // 0x8D : 
    {0, cmd_null},      // 0x8E : 
    {0, cmd_0x8F},      // 0x8F : IKBD_CMD_MOUSE_GET_Y_BOTTOM
};
const ikbd_cmd_t hostcommands_0x90[16] = {
    {0, cmd_0x8F},      // 0x90 : IKBD_CMD_MOUSE_GET_Y_TOP (same as 0x8F)
    {0, cmd_null},      // 0x91 : 
    {0, cmd_0x92},      // 0x92 : IKBD_CMD_MOUSE_GET_DISABLED
    {0, cmd_null},      // 0x93 : 
    {0, cmd_0x94},      // 0x94 : IKBD_CMD_JOY_GET_MODE_EVENT
    {0, cmd_0x94},      // 0x95 : IKBD_CMD_JOY_GET_MODE_POLL (same as 0x94)
    {0, cmd_null},      // 0x96 : 
    {0, cmd_null},      // 0x97 : 
    {0, cmd_null},      // 0x98 : 
    {0, cmd_0x94},      // 0x99 : IKBD_CMD_JOY_GET_MODE_KEYCODE (same as 0x94)
    {0, cmd_0x9A},      // 0x9A : IKBD_CMD_JOY_GET_DISABLED
    {0, cmd_null},      // 0x9B : 
    {0, cmd_null},      // 0x9C : 
    {0, cmd_null},      // 0x9D : 
    {0, cmd_null},      // 0x9E : 
    {0, cmd_null},      // 0x9F : 
};

const ikbd_cmd_t* hostcommands[16] = {
    hostcommands_0x00,  // 0x00
    hostcommands_0x10,  // 0x10
    hostcommands_0x20,  // 0x20
    hostcommands_null,  // 0x30
    hostcommands_null,  // 0x40
    hostcommands_null,  // 0x50
    hostcommands_null,  // 0x60
    hostcommands_null,  // 0x70
    hostcommands_0x80,  // 0x80
    hostcommands_0x90,  // 0x90
    hostcommands_null,  // 0xA0
    hostcommands_null,  // 0xB0
    hostcommands_null,  // 0xC0
    hostcommands_null,  // 0xD0
    hostcommands_null,  // 0xE0
    hostcommands_null,  // 0xF0
};





//---------------------------------------------------------------------
// PS2 (set2) -> Atari Ikbd table
// The second value, when not null, is the offset in Eiffel
// compatible custom keymaps which the user can upload to flashdata.
//---------------------------------------------------------------------
const ikbd_keymap_t ps2keymap[0x90 * 2] = {
// regular codes
    { IKBD_KEY_NONE,        0x00 }, // 0x00
    { IKBD_KEY_F9,          0x47 }, // 0x01
    { IKBD_KEY_NONE,        0x00 }, // 0x02
    { IKBD_KEY_F5,          0x27 }, // 0x03
    { IKBD_KEY_F3,          0x17 }, // 0x04
    { IKBD_KEY_F1,          0x07 }, // 0x05
    { IKBD_KEY_F2,          0x0F }, // 0x06
    { IKBD_KEY_F12,         0x5E }, // 0x07 (undo)
    { IKBD_KEY_NONE,        0x00 }, // 0x08
    { IKBD_KEY_F10,         0x4F }, // 0x09
    { IKBD_KEY_F8,          0x3F }, // 0x0a
    { IKBD_KEY_F6,          0x2F }, // 0x0b
    { IKBD_KEY_F4,          0x1F }, // 0x0c
    { IKBD_KEY_TAB,         0x00 }, // 0x0d
    { IKBD_KEY_TILDE,       0x0E }, // 0x0e
    { IKBD_KEY_NONE,        0x00 }, // 0x0f
    { IKBD_KEY_NONE,        0x00 }, // 0x10
    { IKBD_KEY_ALTGR,       0x39 }, // 0x11 (alt)
    { IKBD_KEY_LSHIFT,      0x12 }, // 0x12
    { IKBD_KEY_NONE,        0x00 }, // 0x13
    { IKBD_KEY_CTRL,        0x11 }, // 0x14
    { IKBD_KEY_Q,           0x15 }, // 0x15
    { IKBD_KEY_1,           0x16 }, // 0x16
    { IKBD_KEY_NONE,        0x00 }, // 0x17
    { IKBD_KEY_NONE,        0x00 }, // 0x18
    { IKBD_KEY_NONE,        0x00 }, // 0x19
    { IKBD_KEY_Z,           0x00 }, // 0x1a
    { IKBD_KEY_S,           0x00 }, // 0x1b
    { IKBD_KEY_A,           0x00 }, // 0x1c
    { IKBD_KEY_W,           0x00 }, // 0x1d
    { IKBD_KEY_2,           0x1E }, // 0x1e
    { IKBD_KEY_NONE,        0x00 }, // 0x1f
    { IKBD_KEY_NONE,        0x00 }, // 0x20
    { IKBD_KEY_C,           0x00 }, // 0x21
    { IKBD_KEY_X,           0x00 }, // 0x22
    { IKBD_KEY_D,           0x00 }, // 0x23
    { IKBD_KEY_E,           0x24 }, // 0x24
    { IKBD_KEY_4,           0x25 }, // 0x25
    { IKBD_KEY_3,           0x26 }, // 0x26
    { IKBD_KEY_NONE,        0x00 }, // 0x27
    { IKBD_KEY_NONE,        0x00 }, // 0x28
    { IKBD_KEY_SPACE,       0x00 }, // 0x29
    { IKBD_KEY_V,           0x00 }, // 0x2a
    { IKBD_KEY_F,           0x00 }, // 0x2b
    { IKBD_KEY_T,           0x00 }, // 0x2c
    { IKBD_KEY_R,           0x00 }, // 0x2d
    { IKBD_KEY_5,           0x2E }, // 0x2e
    { IKBD_KEY_NONE,        0x00 }, // 0x2f
    { IKBD_KEY_NONE,        0x00 }, // 0x30
    { IKBD_KEY_N,           0x00 }, // 0x31
    { IKBD_KEY_B,           0x00 }, // 0x32
    { IKBD_KEY_H,           0x00 }, // 0x33
    { IKBD_KEY_G,           0x00 }, // 0x34
    { IKBD_KEY_Y,           0x00 }, // 0x35
    { IKBD_KEY_6,           0x36 }, // 0x36
    { IKBD_KEY_NONE,        0x00 }, // 0x37
    { IKBD_KEY_NONE,        0x00 }, // 0x38
    { IKBD_KEY_NONE,        0x00 }, // 0x39
    { IKBD_KEY_M,           0x3A }, // 0x3a
    { IKBD_KEY_J,           0x00 }, // 0x3b
    { IKBD_KEY_U,           0x00 }, // 0x3c
    { IKBD_KEY_7,           0x3D }, // 0x3d
    { IKBD_KEY_8,           0x3E }, // 0x3e
    { IKBD_KEY_NONE,        0x00 }, // 0x3f
    { IKBD_KEY_NONE,        0x00 }, // 0x40
    { IKBD_KEY_COMMA,       0x41 }, // 0x41
    { IKBD_KEY_K,           0x00 }, // 0x42
    { IKBD_KEY_I,           0x00 }, // 0x43
    { IKBD_KEY_O,           0x00 }, // 0x44
    { IKBD_KEY_0,           0x45 }, // 0x45
    { IKBD_KEY_9,           0x46 }, // 0x46
    { IKBD_KEY_NONE,        0x00 }, // 0x47
    { IKBD_KEY_NONE,        0x00 }, // 0x48
    { IKBD_KEY_PERIOD,      0x49 }, // 0x49
    { IKBD_KEY_SLASH,       0x4A }, // 0x4a
    { IKBD_KEY_L,           0x00 }, // 0x4b
    { IKBD_KEY_SEMICOLON,   0x4C }, // 0x4c
    { IKBD_KEY_P,           0x00 }, // 0x4d
    { IKBD_KEY_DASH,        0x4E }, // 0x4e
    { IKBD_KEY_NONE,        0x00 }, // 0x4f
    { IKBD_KEY_NONE,        0x00 }, // 0x50
    { IKBD_KEY_NONE,        0x00 }, // 0x51
    { IKBD_KEY_APOSTROPHE,  0x52 }, // 0x52
    { IKBD_KEY_BSLASH,      0x53 }, // 0x53
    { IKBD_KEY_LSQB,        0x54 }, // 0x54
    { IKBD_KEY_EQUAL,       0x55 }, // 0x55
    { IKBD_KEY_NONE,        0x00 }, // 0x56
    { IKBD_KEY_NONE,        0x00 }, // 0x57
    { IKBD_KEY_CAPSLOCK,    0x00 }, // 0x58
    { IKBD_KEY_RSHIFT,      0x59 }, // 0x59
    { IKBD_KEY_RETURN,      0x00 }, // 0x5a
    { IKBD_KEY_RSQB,        0x5B }, // 0x5b
    { IKBD_KEY_NONE,        0x00 }, // 0x5c
    { IKBD_KEY_BSLASH,      0x00 }, // 0x5d
    { IKBD_KEY_NONE,        0x00 }, // 0x5e
    { IKBD_KEY_NONE,        0x00 }, // 0x5f
    { IKBD_KEY_NONE,        0x00 }, // 0x60
    { IKBD_KEY_ISO,         0x13 }, // 0x61
    { IKBD_KEY_NONE,        0x00 }, // 0x62
    { IKBD_KEY_NONE,        0x00 }, // 0x63
    { IKBD_KEY_NONE,        0x00 }, // 0x64
    { IKBD_KEY_NONE,        0x00 }, // 0x65
    { IKBD_KEY_BACKSPACE,   0x00 }, // 0x66
    { IKBD_KEY_NONE,        0x00 }, // 0x67
    { IKBD_KEY_NONE,        0x00 }, // 0x68
    { IKBD_KEY_KP_1,        0x00 }, // 0x69
    { IKBD_KEY_NONE,        0x00 }, // 0x6a
    { IKBD_KEY_KP_4,        0x00 }, // 0x6b
    { IKBD_KEY_KP_7,        0x00 }, // 0x6c
    { IKBD_KEY_NONE,        0x00 }, // 0x6d
    { IKBD_KEY_NONE,        0x00 }, // 0x6e
    { IKBD_KEY_NONE,        0x00 }, // 0x6f
    { IKBD_KEY_KP_0,        0x00 }, // 0x70
    { IKBD_KEY_KP_PERIOD,   0x00 }, // 0x71
    { IKBD_KEY_KP_2,        0x00 }, // 0x72
    { IKBD_KEY_KP_5,        0x00 }, // 0x73
    { IKBD_KEY_KP_6,        0x00 }, // 0x74
    { IKBD_KEY_KP_8,        0x00 }, // 0x75
    { IKBD_KEY_ESC,         0x00 }, // 0x76
    { IKBD_KEY_NUMLOCK,     0x76 }, // 0x77
    { IKBD_KEY_F11,         0x56 }, // 0x78 (help)
    { IKBD_KEY_KP_PLUS,     0x00 }, // 0x79
    { IKBD_KEY_KP_3,        0x00 }, // 0x7a
    { IKBD_KEY_KP_MINUS,    0x00 }, // 0x7b
    { IKBD_KEY_KP_ASTERISK, 0x00 }, // 0x7c
    { IKBD_KEY_KP_9,        0x00 }, // 0x7d
    { IKBD_KEY_SCROLL_LOCK, 0x5F }, // 0x7e
    { IKBD_KEY_NONE,        0x00 }, // 0x7f
    { IKBD_KEY_NONE,        0x00 }, // 0x80
    { IKBD_KEY_NONE,        0x00 }, // 0x81
    { IKBD_KEY_NONE,        0x00 }, // 0x82
    { IKBD_KEY_F7,          0x37 }, // 0x83
    { IKBD_KEY_PRINTSCREEN, 0x57 }, // 0x84
    { IKBD_KEY_NONE,        0x00 }, // 0x85
    { IKBD_KEY_NONE,        0x00 }, // 0x86
    { IKBD_KEY_NONE,        0x00 }, // 0x87
    { IKBD_KEY_NONE,        0x00 }, // 0x88
    { IKBD_KEY_NONE,        0x00 }, // 0x89
    { IKBD_KEY_NONE,        0x00 }, // 0x8a
    { IKBD_KEY_NONE,        0x00 }, // 0x8b
    { IKBD_KEY_NONE,        0x00 }, // 0x8c
    { IKBD_KEY_NONE,        0x00 }, // 0x8d
    { IKBD_KEY_NONE,        0x00 }, // 0x8e
    { IKBD_KEY_NONE,        0x00 }, // 0x8f
// extended codes
    { IKBD_KEY_NONE,        0x00 }, // 0x00
    { IKBD_KEY_NONE,        0x00 }, // 0x01
    { IKBD_KEY_NONE,        0x00 }, // 0x02
    { IKBD_KEY_NONE,        0x00 }, // 0x03
    { IKBD_KEY_NONE,        0x00 }, // 0x04
    { IKBD_KEY_NONE,        0x00 }, // 0x05
    { IKBD_KEY_NONE,        0x00 }, // 0x06
    { IKBD_KEY_NONE,        0x00 }, // 0x07
    { IKBD_KEY_NONE,        0x00 }, // 0x08
    { IKBD_KEY_NONE,        0x00 }, // 0x09
    { IKBD_KEY_NONE,        0x00 }, // 0x0a
    { IKBD_KEY_NONE,        0x00 }, // 0x0b
    { IKBD_KEY_NONE,        0x00 }, // 0x0c
    { IKBD_KEY_NONE,        0x00 }, // 0x0d
    { IKBD_KEY_NONE,        0x00 }, // 0x0e
    { IKBD_KEY_NONE,        0x00 }, // 0x0f
    { IKBD_KEY_WWW_FIND,    0x00 }, // 0x10
    { IKBD_KEY_ALTGR,       0x39 }, // 0x11 (alt)
    { IKBD_KEY_NONE,        0x00 }, // 0x12
    { IKBD_KEY_NONE,        0x00 }, // 0x13
    { IKBD_KEY_CTRL,        0x11 }, // 0x14
    { IKBD_KEY_MEDIA_PREV,  0x00 }, // 0x15
    { IKBD_KEY_NONE,        0x00 }, // 0x16
    { IKBD_KEY_NONE,        0x00 }, // 0x17
    { IKBD_KEY_WWW_FAVS,    0x00 }, // 0x18
    { IKBD_KEY_NONE,        0x00 }, // 0x19
    { IKBD_KEY_NONE,        0x00 }, // 0x1a
    { IKBD_KEY_NONE,        0x00 }, // 0x1b
    { IKBD_KEY_NONE,        0x00 }, // 0x1c
    { IKBD_KEY_NONE,        0x00 }, // 0x1d
    { IKBD_KEY_NONE,        0x00 }, // 0x1e
    { IKBD_KEY_LWIN,        0x8B }, // 0x1f
    { IKBD_KEY_WWW_REFRESH, 0x00 }, // 0x20
    { IKBD_KEY_MEDIA_VOLDN, 0x00 }, // 0x21
    { IKBD_KEY_NONE,        0x00 }, // 0x22
    { IKBD_KEY_MEDIA_MUTE,  0x00 }, // 0x23
    { IKBD_KEY_NONE,        0x00 }, // 0x24
    { IKBD_KEY_NONE,        0x00 }, // 0x25
    { IKBD_KEY_NONE,        0x00 }, // 0x26
    { IKBD_KEY_RWIN,        0x8C }, // 0x27
    { IKBD_KEY_WWW_STOP,    0x00 }, // 0x28
    { IKBD_KEY_NONE,        0x00 }, // 0x29
    { IKBD_KEY_NONE,        0x00 }, // 0x2a
    { IKBD_KEY_MEDIA_CALC,  0x00 }, // 0x2b
    { IKBD_KEY_NONE,        0x00 }, // 0x2c
    { IKBD_KEY_NONE,        0x00 }, // 0x2d
    { IKBD_KEY_NONE,        0x00 }, // 0x2e
    { IKBD_KEY_APP,         0x8D }, // 0x2f
    { IKBD_KEY_WWW_FWD,     0x00 }, // 0x30
    { IKBD_KEY_NONE,        0x00 }, // 0x31
    { IKBD_KEY_MEDIA_VOLUP, 0x00 }, // 0x32
    { IKBD_KEY_NONE,        0x00 }, // 0x33
    { IKBD_KEY_MEDIA_PLAY,  0x00 }, // 0x34
    { IKBD_KEY_NONE,        0x00 }, // 0x35
    { IKBD_KEY_NONE,        0x00 }, // 0x36
    { IKBD_KEY_POWER,       0x80 }, // 0x37
    { IKBD_KEY_WWW_BACK,    0x00 }, // 0x38
    { IKBD_KEY_NONE,        0x00 }, // 0x39
    { IKBD_KEY_WWW_HOME,    0x00 }, // 0x3a
    { IKBD_KEY_MEDIA_STOP,  0x00 }, // 0x3b
    { IKBD_KEY_NONE,        0x00 }, // 0x3c
    { IKBD_KEY_NONE,        0x00 }, // 0x3d
    { IKBD_KEY_NONE,        0x00 }, // 0x3e
    { IKBD_KEY_SLEEP,       0x7F }, // 0x3f
    { IKBD_KEY_MEDIA_COMP,  0x00 }, // 0x40
    { IKBD_KEY_NONE,        0x00 }, // 0x41
    { IKBD_KEY_NONE,        0x00 }, // 0x42
    { IKBD_KEY_NONE,        0x00 }, // 0x43
    { IKBD_KEY_NONE,        0x00 }, // 0x44
    { IKBD_KEY_NONE,        0x00 }, // 0x45
    { IKBD_KEY_NONE,        0x00 }, // 0x46
    { IKBD_KEY_NONE,        0x00 }, // 0x47
    { IKBD_KEY_MEDIA_EMAIL, 0x00 }, // 0x48
    { IKBD_KEY_NONE,        0x00 }, // 0x49
    { IKBD_KEY_KP_SLASH,    0x00 }, // 0x4a
    { IKBD_KEY_NONE,        0x00 }, // 0x4b
    { IKBD_KEY_NONE,        0x00 }, // 0x4c
    { IKBD_KEY_MEDIA_NEXT,  0x00 }, // 0x4d
    { IKBD_KEY_NONE,        0x00 }, // 0x4e
    { IKBD_KEY_NONE,        0x00 }, // 0x4f
    { IKBD_KEY_MEDIA_SELECT,0x00 }, // 0x50
    { IKBD_KEY_NONE,        0x00 }, // 0x51
    { IKBD_KEY_NONE,        0x00 }, // 0x52
    { IKBD_KEY_NONE,        0x00 }, // 0x53
    { IKBD_KEY_NONE,        0x00 }, // 0x54
    { IKBD_KEY_NONE,        0x00 }, // 0x55
    { IKBD_KEY_NONE,        0x00 }, // 0x56
    { IKBD_KEY_NONE,        0x00 }, // 0x57
    { IKBD_KEY_NONE,        0x00 }, // 0x58
    { IKBD_KEY_NONE,        0x00 }, // 0x59
    { IKBD_KEY_KP_ENTER,    0x00 }, // 0x5a
    { IKBD_KEY_NONE,        0x00 }, // 0x5b
    { IKBD_KEY_NONE,        0x00 }, // 0x5c
    { IKBD_KEY_NONE,        0x00 }, // 0x5d
    { IKBD_KEY_WAKE,        0x81 }, // 0x5e
    { IKBD_KEY_NONE,        0x00 }, // 0x5f
    { IKBD_KEY_NONE,        0x00 }, // 0x60
    { IKBD_KEY_NONE,        0x00 }, // 0x61
    { IKBD_KEY_NONE,        0x00 }, // 0x62
    { IKBD_KEY_NONE,        0x00 }, // 0x63
    { IKBD_KEY_NONE,        0x00 }, // 0x64
    { IKBD_KEY_NONE,        0x00 }, // 0x65
    { IKBD_KEY_NONE,        0x00 }, // 0x66
    { IKBD_KEY_NONE,        0x00 }, // 0x67
    { IKBD_KEY_NONE,        0x00 }, // 0x68
    { IKBD_KEY_END,         0x65 }, // 0x69
    { IKBD_KEY_NONE,        0x00 }, // 0x6a
    { IKBD_KEY_LEFT,        0x00 }, // 0x6b
    { IKBD_KEY_HOME,        0x00 }, // 0x6c
    { IKBD_KEY_NONE,        0x00 }, // 0x6d
    { IKBD_KEY_NONE,        0x00 }, // 0x6e
    { IKBD_KEY_NONE,        0x00 }, // 0x6f
    { IKBD_KEY_INSERT,      0x00 }, // 0x70
    { IKBD_KEY_DELETE,      0x00 }, // 0x71
    { IKBD_KEY_DOWN,        0x00 }, // 0x72
    { IKBD_KEY_NONE,        0x00 }, // 0x73
    { IKBD_KEY_RIGHT,       0x00 }, // 0x74
    { IKBD_KEY_UP,          0x00 }, // 0x75
    { IKBD_KEY_NONE,        0x00 }, // 0x76
    { IKBD_KEY_NONE,        0x00 }, // 0x77
    { IKBD_KEY_NONE,        0x00 }, // 0x78
    { IKBD_KEY_NONE,        0x00 }, // 0x79
    { IKBD_KEY_PAGEDOWN,    0x6D }, // 0x7a
    { IKBD_KEY_NONE,        0x00 }, // 0x7b
    { IKBD_KEY_PRINTSCREEN, 0x00 }, // 0x7c
    { IKBD_KEY_PAGEUP,      0x6F }, // 0x7d
    { IKBD_KEY_PAUSE,       0x62 }, // 0x7e
    { IKBD_KEY_NONE,        0x00 }, // 0x7f
    { IKBD_KEY_NONE,        0x00 }, // 0x80
    { IKBD_KEY_NONE,        0x00 }, // 0x81
    { IKBD_KEY_NONE,        0x00 }, // 0x82
    { IKBD_KEY_NONE,        0x00 }, // 0x83
    { IKBD_KEY_NONE,        0x00 }, // 0x84
    { IKBD_KEY_NONE,        0x00 }, // 0x85
    { IKBD_KEY_NONE,        0x00 }, // 0x86
    { IKBD_KEY_NONE,        0x00 }, // 0x87
    { IKBD_KEY_NONE,        0x00 }, // 0x88
    { IKBD_KEY_NONE,        0x00 }, // 0x89
    { IKBD_KEY_NONE,        0x00 }, // 0x8a
    { IKBD_KEY_NONE,        0x00 }, // 0x8b
    { IKBD_KEY_NONE,        0x00 }, // 0x8c
    { IKBD_KEY_NONE,        0x00 }, // 0x8d
    { IKBD_KEY_NONE,        0x00 }, // 0x8e
    { IKBD_KEY_NONE,        0x00 }, // 0x8f
};

