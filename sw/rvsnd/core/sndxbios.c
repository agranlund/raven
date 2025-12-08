/*-------------------------------------------------------------------------------
 * rvsnd falcon xbios wrapper
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/

#include "sys.h"
#include "mixer.h"

/* todo: most of this is a mixture or WIP and temporary hacks for now */

#define XBDEBUG 0

/* GSXB considers the data argument to be attenuation           */
/*   0 = least attenuation = highest volume                     */
/* 255 = most attenuation  = lowest volume                      */
/*                                                              */
/* TOS.HYP and MilanBlaster specifies this argument as Gain     */
/* I do not know if they mean 0 is <max> and 255 is <min>       */
/* If they don't then there could be a problem with GSXB doing  */
/* the exact opposite of MilanBlaster / what TOS.HYP says       */
/*                                                              */
/* If we're going to create a GSXB cookie to claim compatible   */
/* then we might want to do what GSXB does (right or wrong)     */

#define SNDCMD_IS_ATTENUATION 1


/*------------------------------------------------------------------------------*/
/* $80 : locksnd                                                                */
/*------------------------------------------------------------------------------*/
int32_t xb_locksnd(void) {
#if XBDEBUG
    dprintf("xb_locksnd\n");
#endif
    return 128;
}

/*------------------------------------------------------------------------------*/
/* $81 : unlocksnd                                                              */
/*------------------------------------------------------------------------------*/
int32_t xb_unlocksnd(void) {
#if XBDEBUG
    dprintf("xb_unlocksnd\n");
#endif
    return 129;
}

/*------------------------------------------------------------------------------*/
/* $82 : soundcmd                                                               */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; int16_t data; int32_t data2; } xb_soundcmd_args;
int32_t xb_soundcmd(xb_soundcmd_args* args) {
    bool enquire = (args->data == -1) ? true : false;
#if XBDEBUG
    dprintf("xb_soundcmd %04x %04x %08lx\n", args->mode, args->data, args->data2);
#endif
    switch (args->mode) {

        /* Falcon DAC output attenuation */
        case 0: case 1: {
            uint8_t val = (255 - (args->data & 0xff));
            if (!enquire) { mixer_SetValueById(SYSMIXER_ID_PCM, val); }
            return (255 - mixer_GetValueById(SYSMIXER_ID_PCM));
        }

        /* Falcon ADC input gain */
        case 2: case 3: {
            return 0;
        }

        /* inputs enable / disable */
        case 4: {
            static uint16_t v = 0x41fe;
            if (!enquire) {
                v = args->data;
            }
            return v;
        }

        /* ADC input source */
        case 5: {
            return 0;
        }

        /* prescale */
        case 6: {
            return 0;
        }

        /* direct sample rate (MilanBlaster/GSXB) */
        case 7: {
            return 0;
        }

        /* sample format (MilanBlaster/GSXB) */
        case 8: {   /* 8bit */
            static uint16_t v = 0x2;
            if (!enquire) { v = args->data; }
            return v;
        }
        case 9: {   /* 16bit */
            static uint16_t v = 0x9;
            if (!enquire) { v = args->data; }
            return v;
        }
        case 10: {  /* 24bit */
            static uint16_t v = 0x9;
            if (!enquire) { v = args->data; }
            return v;
        }
        case 11: {  /* 32bit */
            static uint16_t v = 0x9;
            if (!enquire) { v = args->data; }
            return v;
        }

        /* mixer controls (MilanBlater/GSXB) */
        case 12: case 13:   /* master   */
        case 14: case 15:   /* mic      */
        case 16: case 17:   /* fm       */
        case 18: case 19:   /* line     */
        case 20: case 21:   /* cd       */
        case 22: case 23:   /* tv       */
        case 24: case 25: { /* aux      */
            static const uint16_t map[] = {
                SYSMIXER_ID_MASTER,
                SYSMIXER_ID_MIC,
                SYSMIXER_ID_FM,
                SYSMIXER_ID_LINE,
                SYSMIXER_ID_CD,
                SYSMIXER_ID_TV,
                SYSMIXER_ID_AUX
            };
            uint16_t id = map[(args->mode - 12) >> 1];
            uint8_t val = (args->data <= 0) ? 0 : (args->data >= 255) ? 255 : args->data;
#if SNDCMD_IS_ATTENUATION
            if (!enquire) { mixer_SetValueById(id, 255 - val); }
            return 255 - mixer_GetValueById(id);
#else
            if (!enquire) { mixer_SetValueById(id, val); }
            return mixer_GetValueById(id);
#endif            
        }
    }
    return 130;
}

/*------------------------------------------------------------------------------*/
/* $83 : setbuffer                                                              */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t reg; void* begaddr; void* endaddr;} xb_setbuffer_args;
int32_t xb_setbuffer(xb_setbuffer_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_setbuffer %04x %08lx %08lx\n", args->reg, args->begaddr, args->endaddr);
#endif
    return 131;
}

/*------------------------------------------------------------------------------*/
/* $84 : setmode                                                                */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; } xb_setmode_args;
int32_t xb_setmode(xb_setmode_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_setmode %04x\n", args->mode);
#endif
    return 132;
}

/*------------------------------------------------------------------------------*/
/* $85 : settracks                                                              */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t playtracks; int16_t rectracks; } xb_settracks_args;
int32_t xb_settracks(xb_settracks_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_settracks %04x %04x\n", args->playtracks, args->rectracks);
#endif
    return 133;
}

/*------------------------------------------------------------------------------*/
/* $86 : setmontracks                                                           */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t montrack; } xb_setmontracks_args;
int32_t xb_setmontracks(xb_setmontracks_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_setmontracks %04x %04x\n", args->montrack);
#endif
    return 134;
}

/*------------------------------------------------------------------------------*/
/* $87 : setinterrupt                                                           */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t src_inter; int16_t cause; } xb_setinterrupt_args;
int32_t xb_setinterrupt(xb_setinterrupt_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_setinterrupt %04x %04x\n", args->src_inter, args->cause);
#endif
    return 135;
}

/*------------------------------------------------------------------------------*/
/* $88 : buffoper                                                               */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; } xb_buffoper_args;
int32_t xb_buffoper(xb_buffoper_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_buffoper %04x\n", args->mode);
#endif
    return 136;
}

/*------------------------------------------------------------------------------*/
/* $89 : dsptristate                                                            */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t dspxmit; int16_t dsprec; } xb_dsptristate_args;
int32_t xb_dsptristate(xb_dsptristate_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_dsptristate %04x\n", args->dspxmit, args->dsprec);
#endif
    return 137;
}

/*------------------------------------------------------------------------------*/
/* $8A : gpio                                                                   */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; int16_t data; } xb_gpio_args;
int32_t xb_gpio(xb_gpio_args* args) {
    /* this is sometimes used to detect the presence of an external */
    /* clock on Falcon, used for standard pc sound frequencies      */
    (void)args;
#if XBDEBUG
    dprintf("xb_gpio %04x\n", args->mode, args->data);
#endif
    return 138;
}

/*------------------------------------------------------------------------------*/
/* $8B : devconnect                                                             */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t src; int16_t dst; int16_t srcclk; int16_t prescale; int16_t protcol; } xb_devconnect_args;
int32_t xb_devconnect(xb_devconnect_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_devconnect %04x\n", args->src, args->dst, args->srcclk, args->prescale, args->protcol);
#endif
    return 139;
}

/*------------------------------------------------------------------------------*/
/* $8C : sndstatus                                                              */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t reset; } xb_sndstatus_args;
int32_t xb_sndstatus(xb_sndstatus_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_sndstatus %04x\n", args->reset);
#endif
    switch (args->reset) {
        case 0: case 1: {
            if (args->reset) {
                /*
                - Gain and attentuation are zeroed
                - Old matrix connections are reset
                - ADDERIN is disabled
                - Mode is set to 8-Bit Stereo
                - Play and record tracks are set to 0
                - Monitor track is set to 0
                - Interrupts are disabled
                - Buffer operation is disabled
                */
            }
            /* return dac+adc error status, always ok */
            return 0;
        }

        /* inquire bit-depth (MilanBlaster / GSXB) */
        case 2: {
            return
                (1 << 0) |  /*  8bit, supported */
                (1 << 1) |  /* 16bit, supported */
                (0 << 2) |  /* 24bit, not supported */
                (0 << 3);   /* 32bit, not supported */
        }
        /* inquire available inputs */
        case 3: {
            /* todo: we should ask the mixer about this... */
            return
                (0 << 0) |  /* adc      */
                (1 << 1) |  /* dac      */
                (1 << 2) |  /* mic      */
                (1 << 3) |  /* fm       */
                (1 << 4) |  /* line     */
                (1 << 5) |  /* cd       */
                (1 << 6) |  /* tv       */
                (1 << 7);   /* aux      */
        }

        /* inquire available inputs for ADC */
        case 4: {
            /* todo: we should ask the mixer about this...  */
            /*       when and if we support recording.      */
            return 0;
        }

        /* inquire duplex operation */
        case 5: {
            return 0;   /* simultaneous record playback not possible */
        }

        /* inquire 8bit sample format */
        case 8: {
            return 0x3; /* signed + unsigned */
        }

        /* inquire 16bit sample format */
        case 9: {
            return 0x7; /* signed + unsigned + little endian + big endian */
        }
        
        /* inquire 24/32 bit sample format */
        case 10: case 11: {
            return 0x0; /* we don't support them */
        }

        /* inquire value of falcon register ff8900 */
        case 0x8900: {
            return 0;
        }

        /* inquire value of falcon register ff8902 */
        case 0x8902: {
            return 0;
        }

        /* inquire value of falcon register ff890e */
        case 0x890e: {
            return 0;
        }

        /* inquire value of falcon register ff8920 */
        case 0x8920: {
            return 0;
        }

    }
    return 140;
}

/*------------------------------------------------------------------------------*/
/* $8D : buffptr                                                                */
/*------------------------------------------------------------------------------*/
typedef struct {int32_t* ptr; } xb_buffptr_args;
int32_t xb_buffptr(xb_buffptr_args* args) {
    (void)args;
#if XBDEBUG
    dprintf("xb_buffptr %08lx\n", (uint32_t)args->ptr);
#endif
    return 141;
}
