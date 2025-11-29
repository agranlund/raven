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

/*------------------------------------------------------------------------------*/
/* $80 : locksnd                                                                */
/*------------------------------------------------------------------------------*/
int32_t xb_locksnd(void) {
    return 128;
}

/*------------------------------------------------------------------------------*/
/* $81 : unlocksnd                                                              */
/*------------------------------------------------------------------------------*/
int32_t xb_unlocksnd(void) {
    return 129;
}

/*------------------------------------------------------------------------------*/
/* $82 : soundcmd                                                               */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; int16_t data; int32_t data2; } xb_soundcmd_args;
int32_t xb_soundcmd(xb_soundcmd_args* args) {
    (void)args;
    return 130;
}

/*------------------------------------------------------------------------------*/
/* $83 : setbuffer                                                              */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t reg; void* begaddr; void* endaddr;} xb_setbuffer_args;
int32_t xb_setbuffer(xb_setbuffer_args* args) {
    (void)args;
    return 131;
}

/*------------------------------------------------------------------------------*/
/* $84 : setmode                                                                */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; } xb_setmode_args;
int32_t xb_setmode(xb_setmode_args* args) {
    (void)args;
    return 132;
}

/*------------------------------------------------------------------------------*/
/* $85 : settracks                                                              */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t playtracks; int16_t rectracks; } xb_settracks_args;
int32_t xb_settracks(xb_settracks_args* args) {
    (void)args;
    return 133;
}

/*------------------------------------------------------------------------------*/
/* $86 : setmontracks                                                           */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t montrack; } xb_setmontracks_args;
int32_t xb_setmontracks(xb_setmontracks_args* args) {
    (void)args;
    return 134;
}

/*------------------------------------------------------------------------------*/
/* $87 : setinterrupt                                                           */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t src_inter; int16_t cause; } xb_setinterrupt_args;
int32_t xb_setinterrupt(xb_setinterrupt_args* args) {
    (void)args;
    return 135;
}

/*------------------------------------------------------------------------------*/
/* $88 : buffoper                                                               */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; } xb_buffoper_args;
int32_t xb_buffoper(xb_buffoper_args* args) {
    (void)args;
    return 136;
}

/*------------------------------------------------------------------------------*/
/* $89 : dsptristate                                                            */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t dspxmit; int16_t dsprec; } xb_dsptristate_args;
int32_t xb_dsptristate(xb_dsptristate_args* args) {
    (void)args;
    return 137;
}

/*------------------------------------------------------------------------------*/
/* $8A : gpio                                                                   */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t mode; int16_t data; } xb_gpio_args;
int32_t xb_gpio(xb_gpio_args* args) {
    (void)args;
    return 138;
}

/*------------------------------------------------------------------------------*/
/* $8B : devconnect                                                             */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t src; int16_t dst; int16_t srcclk; int16_t prescale; int16_t protcol; } xb_devconnect_args;
int32_t xb_devconnect(xb_devconnect_args* args) {
    (void)args;
    return 139;
}

/*------------------------------------------------------------------------------*/
/* $8C : sndstatus                                                              */
/*------------------------------------------------------------------------------*/
typedef struct {int16_t reset; } xb_sndstatus_args;
int32_t xb_sndstatus(xb_sndstatus_args* args) {
    (void)args;
    return 140;
}

/*------------------------------------------------------------------------------*/
/* $8D : buffptr                                                                */
/*------------------------------------------------------------------------------*/
typedef struct {int32_t* ptr; } xb_buffptr_args;
int32_t xb_buffptr(xb_buffptr_args* args) {
    (void)args;
    return 141;
}
