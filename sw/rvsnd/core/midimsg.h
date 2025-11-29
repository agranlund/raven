/*
Copyright 2018 Jacob Vosmaer
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * this code has been modified and adjusted for rvsnd
 * (c)2025 Anders Granlund
 */

#ifndef _MIDIMSG_H_
#define _MIDIMSG_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t midipacket_t;

typedef union {
    struct {
        uint8_t type;
        uint8_t data[3];
    } msg;
    midipacket_t packet;
} midimsg_t;

typedef struct {
    uint8_t status; /* parse status */
    uint8_t param;  /* parse parameter */
    midimsg_t msg;  /* complete message */
} midiparser_t;

#define MIDIMSG_NONE    0
#define MIDIMSG_SIZE1   1
#define MIDIMSG_SIZE2   2
#define MIDIMSG_SIZE3   3

#define MIDIMSG_SIZE    3
#define MIDIMSG_RT      (1<<5)
#define MIDIMSG_SYS     (1<<6)
#define MIDIMSG_SYSEX   (1<<7)

static bool midi_parse(midiparser_t* p, uint8_t b) {
    p->msg.packet = 0;
    if (b >= 0xf8) {
        /* realtime messages, no data, does not reset parser */
        /* can happen inside other longer messages */
        p->msg.msg.type = MIDIMSG_SIZE1 | MIDIMSG_RT;
        p->msg.msg.data[0] = b;
        return true;
    }
    else if (b >= 0xf7) {
        /* end of sysex message */
        p->msg.msg.type = MIDIMSG_SIZE1 | MIDIMSG_SYSEX;
        p->msg.msg.data[0] = b;
        p->status = 0;
        return true;
    }
    else if (b >= 0xf4) {
        /* system messages, no data, resets parser */
        p->msg.msg.type = MIDIMSG_SIZE1 | MIDIMSG_SYS;
        p->msg.msg.data[0] = b;
        p->status = 0;
        return true;
    }
    else if (b == 0xf0) {
        /* start of sysex message */
        p->msg.msg.type = MIDIMSG_SIZE1 | MIDIMSG_SYSEX;
        p->msg.msg.data[0] = b;
        p->status = b;
        return true;
    }
    else if (b >= 0x80) {
        /* start of message which can have one or more parameter */
        p->status = b;
        p->param = 0;
    }
    else {
        /* parameter data */
        if (p->status == 0xf0) {
            p->msg.msg.type = MIDIMSG_SIZE1 | MIDIMSG_SYSEX;
            p->msg.msg.data[0] = b;
            return true;
        }
        else if (((p->status >= 0xc0) && (p->status < 0xe0)) || ((p->status > 0xf0) && (p->status != 0xf2))) {
            /* one parameter message */
            p->msg.msg.type = MIDIMSG_SIZE2;
            p->msg.msg.data[0] = p->status;
            p->msg.msg.data[1] = b;
            return true;
        }
        else if (p->status && (p->param & 0x80)) {
            /* second of two parameters */
            p->msg.msg.type = MIDIMSG_SIZE3;
            p->msg.msg.data[0] = p->status;
            p->msg.msg.data[1] = p->param & 0x7F;
            p->msg.msg.data[2] = b;
            p->param = 0;
            return true;
        }
        else {
            /* first of two parameters */
            p->param = b | 0x80;
        }
    }
    return false;
}

#endif /* _MIDIMSG_H_ */
