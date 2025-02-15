/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 *
 */

/**
 * \file
 * SLIP protocol implementation
 * \author Adam Dunkels <adam@dunkels.com>
 */

/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup slip Serial Line IP (SLIP) protocol
 * @{
 *
 * The SLIP protocol is a very simple way to transmit IP packets over
 * a serial line. It does not provide any framing or error control,
 * and is therefore not very widely used today.
 *
 * This SLIP implementation requires two functions for accessing the
 * serial device: slipdev_char_poll() and slipdev_char_put(). These
 * must be implemented specifically for the system on which the SLIP
 * protocol is to be run.
 */

/*
 * This is a generic implementation of the SLIP protocol over an RS232
 * (serial) device.
 *
 * Huge thanks to Ullrich von Bassewitz <uz@cc65.org> of cc65 fame for
 * and endless supply of bugfixes, insightsful comments and
 * suggestions, and improvements to this code!
 */

#include <string.h>
#include <osbind.h>
#include <mint/ostruct.h>
#include <sys/ioctl.h>
#include "slip_dev.h"
#include "uip/uip.h"
#include "logging.h"

#define SLIP_END     0xC0
#define SLIP_ESC     0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD

static uint8_t slip_buf[UIP_BUFSIZE];

static uint16_t len, tmplen, dev;
static uint8_t lastc;

/**
 * Put a character on the serial device.
 *
 * This function is used by the SLIP implementation to put a character
 * on the serial device. It must be implemented specifically for the
 * system on which the SLIP implementation is to be run.
 *
 * \param c The character to be put on the serial device.
 */
static void slipdev_char_put(uint8_t c)
{
  while (!Bcostat(dev)) { /* XXX should maybe time out? */ }
  Bconout(dev, c);
}

/**
 * Poll the serial device for a character.
 *
 * This function is used by the SLIP implementation to poll the serial
 * device for a character. It must be implemented specifically for the
 * system on which the SLIP implementation is to be run.
 *
 * The function should return immediately regardless if a character is
 * available or not. If a character is available it should be placed
 * at the memory location pointed to by the pointer supplied by the
 * argument c.
 *
 * \param c A pointer to a byte that is filled in by the function with
 * the received character, if available.
 *
 * \retval 0 If no character is available.
 * \retval Non-zero If a character is available.
 */
static int slipdev_char_poll(uint8_t *c)
{
  if (Bconstat(dev)) {
    *c = Bconin(dev) & 0xff;
    return 1;
  }
  return 0;
}

/*-----------------------------------------------------------------------------------*/
/**
 * Send the packet in the uip_buf and uip_appdata buffers using the
 * SLIP protocol.
 *
 * The first 40 bytes of the packet (the IP and TCP headers) are read
 * from the uip_buf buffer, and the following bytes (the application
 * data) are read from the uip_appdata buffer.
 *
 */
/*-----------------------------------------------------------------------------------*/
void slipdev_send(void)
{
  uint16_t i;
  uint8_t *ptr;
  uint8_t c;

  slipdev_char_put(SLIP_END);

  ptr = &uip_buf[UIP_LLH_LEN];
  for(i = 0; i < uip_len; ++i) {
    if(i == UIP_TCPIP_HLEN) {
      ptr = (char *)uip_appdata;
    }
    c = *ptr++;
    switch(c) {
    case SLIP_END:
      slipdev_char_put(SLIP_ESC);
      slipdev_char_put(SLIP_ESC_END);
      break;
    case SLIP_ESC:
      slipdev_char_put(SLIP_ESC);
      slipdev_char_put(SLIP_ESC_ESC);
      break;
    default:
      slipdev_char_put(c);
      break;
    }
  }
  slipdev_char_put(SLIP_END);
}



/*-----------------------------------------------------------------------------------*/
/** 
 * Poll the SLIP device for an available packet.
 *
 * This function will poll the SLIP device to see if a packet is
 * available. It uses a buffer in which all avaliable bytes from the
 * RS232 interface are read into. When a full packet has been read
 * into the buffer, the packet is copied into the uip_buf buffer and
 * the length of the packet is returned.
 *
 * \return The length of the packet placed in the uip_buf buffer, or
 * zero if no packet is available.
 */
/*-----------------------------------------------------------------------------------*/
uint16_t slipdev_poll(void)
{
  uint8_t c;

  while(slipdev_char_poll(&c)) {
    switch(c) {
    case SLIP_ESC:
      lastc = c;
      break;

    case SLIP_END:
      lastc = c;
      /* End marker found, we copy our input buffer to the uip_buf
      buffer and return the size of the packet we copied. */      
      memcpy(&uip_buf[UIP_LLH_LEN], slip_buf, len);

      tmplen = len;
      len = 0;
      return tmplen;

    default:
      if(lastc == SLIP_ESC) {
        lastc = c;
        /* Previous read byte was an escape byte, so this byte will be
           interpreted differently from others. */
        switch(c) {
        case SLIP_ESC_END:
          c = SLIP_END;
          break;
        case SLIP_ESC_ESC:
          c = SLIP_ESC;
          break;
        }
      } else {
        lastc = c;
      }

      slip_buf[len] = c;
      ++len;

      if(len > UIP_BUFSIZE) {
        len = 0;
      }

      break;
    }
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the SLIP module.
 */
/*-----------------------------------------------------------------------------------*/
static struct {
  unsigned long bps;
  unsigned short code;
} rate_table[] = {
  { 921600, 92},    /* not in system headers, supported by some targets */
  { 460800, 46},
  { 230400, 10},
  { 115200, 11},
  { 57600,  12},
  { 19200,  B19200},
  { 0 }
};

bool slipdev_init(unsigned short serial_dev, unsigned long serial_speed)
{
  unsigned short old_aux;
  unsigned short speed = 0;
  int i;

  /* validate / convert speed to Rsconf code */
  for (i = 0; rate_table[i].bps != 0; i++) {
    if (rate_table[i].bps == serial_speed) {
      speed = rate_table[i].code;
      break;
    }
  }
  if (speed == 0) {
    LOG_WARN("serial speed %lu not recognised, valid values are: \r\n", serial_speed);
    for (i = 0; rate_table[i].bps != 0; i++) {
      LOG_WARN("  %lu\r\n", rate_table[i].bps);
    }
    return false;
  }

  /* map device to aux so that Rsconf can do its thing */
  old_aux = Bconmap(serial_dev);
  if (old_aux == 0) {
    LOG_WARN("BIOS device %u does not exist\r\n", serial_dev);
    return false;
  }
  /* configure and remap the old dev back */
  //Rsconf(speed, FLOW_HARD, RS_1STOP | RS_8BITS, RS_RECVENABLE, 0, 0);
  Rsconf(speed, 0, RS_1STOP | RS_8BITS, RS_RECVENABLE, 0, 0);
  speed -= Rsconf(BAUD_INQUIRE, 0, 0, 0, 0, 0);
  Bconmap(old_aux);

  if (speed != 0) {
    LOG_WARN("serial speed %lu not supported by port %d\r\n", serial_speed, serial_dev);
    return false;
  }

  /* save the now-configured device */
  dev = serial_dev;

  /* init the state machines */
  lastc = len = 0;

  INFO("done.\r\n");
  return true;
}

void slipdev_done(void)
{
  /* nothing really to do here... */
}

/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */