/*-------------------------------------------------------------------------------
 * rvsnd : mpu401 isa driver
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

#include "driver.h"

static isa_t* bus = 0;
static uint16_t mpu401_port = 0;

#define SUPPORT_PNP             0
#define SUPPORT_PNP_SUBDEVICE   0 && SUPPORT_PNP

/* -------------------------------------------------------------------- */
#define MPU401_REG_DATA         0
#define MPU401_REG_COMMAND      1
#define MPU401_REG_STATUS       1
#define MPU401_STAT_READ        0x80
#define MPU401_STAT_WRITE       0x40

static bool mpu401_CheckStatus(uint8_t stat) {
    return ((bus->inp(mpu401_port + MPU401_REG_STATUS) & stat) == 0) ? true : false;
}

static bool mpu401_WaitStatus(uint8_t stat) {
    int32_t timeout = 100000L;
    while(timeout) {
        if (mpu401_CheckStatus(stat)) {
            return true;
        }
        delayus(5);
        timeout -= 1L;
    }
    return false;
}

static bool mpu401_WaitAck(void) {
    int32_t timeout = 100L;
    while(timeout) {
        if (!mpu401_WaitStatus(MPU401_STAT_READ)) {
            return false;
        }
        if (bus->inp(mpu401_port + MPU401_REG_DATA) == 0xFE) {
            return true;
        }
        timeout -= 1L;
    }
    return false;
}

static bool mpu401_WriteCommand(uint8_t data) {
    if (mpu401_port && mpu401_WaitStatus(MPU401_STAT_WRITE)) {
        bus->outp(mpu401_port + MPU401_REG_COMMAND, data);
        return true;
    }
    return false;
}

static void mpu401_WriteData(uint8_t data) {
    if (mpu401_port && mpu401_WaitStatus(MPU401_STAT_WRITE)) {
        bus->outp(mpu401_port + MPU401_REG_DATA, data);
    }
}

static uint8_t mpu401_ReadData(void) {
    if (mpu401_port && mpu401_WaitStatus(MPU401_STAT_READ)) {
        return bus->inp(mpu401_port + MPU401_REG_DATA);
    }
    return 0x00;
}

static bool mpu401_Detect(uint16_t port) {
    mpu401_port = port;
    if (!mpu401_WriteCommand(0xff)) {   /* reset */
        goto fail;
    }
    if (!mpu401_WaitAck()) {            /* wait ack */
        goto fail;
    }
    if (!mpu401_WriteCommand(0x3f)) {   /* uart mode */
        goto fail;
    }
    return true;
fail:
    mpu401_port = 0;
    return false;
}


/* -------------------------------------------------------------------- */
static int32_t cdecl miditx_st(void) { return mpu401_CheckStatus(MPU401_STAT_WRITE) ? -1L : 0L; }
static void cdecl miditx_tx(uint32_t c) { mpu401_WriteData(c); }
static int32_t cdecl midirx_st(void) { return mpu401_CheckStatus(MPU401_STAT_READ) ? -1L : 0L; }
static int32_t cdecl midirx_rx(void) { return (uint32_t)mpu401_ReadData(); }

static const char* dnames[] = {"MPU401", 0};
static rvdev_miditx_t txdev = {
    RVDEV_MIDI_OUT, 0, 0,
    dnames,
    0, 0,
    miditx_st,
    miditx_tx
};

static rvdev_midirx_t rxdev = {
    RVDEV_MIDI_IN, 0, 0,
    dnames,
    0, 0,
    midirx_st,
    midirx_rx 
};


/* -------------------------------------------------------------------- */
int32_t init(void) {
    bool use_pnp = true;
    bool use_probe = true;
    static const uint16_t probe_ports[] = { 0x330, /*0x300,*/ 0 };

    /* isa bus required */
    bus = rvsnd->isa;
    if (!bus) {
        return -1;
    }

    /* todo: port override from inifile */
    mpu401_port = 0;

    /* ask pnp for mpu401 compatible device */
    #if SUPPORT_PNP
    if (!mpu401_port && use_pnp) {
        uint16_t type, idx;
        static const struct { const char* id; const uint16_t io; } pnpids[] = {
            { "PNPB006", 0 },   /* mpu401 */
            #if SUPPORT_PNP_SUBDEVICE
            { "PNPB001", 2 },   /* soundblaster 2.0 compatible */
            { "PNPB002", 2 },   /* soundblaster pro compatible */
            { "PNPB003", 2 },   /* soundblaster 16  compatible */
            { "ESS1868", 2 },   /* */
            { "ESS1869", 2 },   /* */
            #endif /* SUPPORT_PNP_SB */
            { 0,         0 }
        };
        for (type = 0; !mpu401_port && pnpids[type].id; type++) {
            for (idx = 0; !mpu401_port && (idx < 16); idx++) {
                isa_dev_t* dev = bus->find_dev(pnpids[type].id, idx);
                if (!dev || mpu401_Detect(dev->port[pnpids[type].io])) {
                    break;
                }
            }
        }
    }
    #endif

    /* manually probe at common mpu401 ports */
    if (!mpu401_port && use_probe) {
        uint16_t idx;
        for (idx = 0; !mpu401_port && probe_ports[idx]; idx++) {
            if (mpu401_Detect(probe_ports[idx])) {
                break;
            }
        }
    }

    if (!mpu401_port) {
        return -1;
    }

    /* publish driver */
    txdev.addr = bus->iobase + mpu401_port;
    rxdev.addr = bus->iobase + mpu401_port;
    dev_publish(&txdev);
    dev_publish(&rxdev);
    return 0;
}
