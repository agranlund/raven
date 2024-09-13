/*
 * nvram.c - Non-Volatile RAM access
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "cookie.h"
#include "machine.h"
#include "vectors.h"
#include "nvram.h"
#include "biosmem.h"
#include "raven.h"

#if CONF_WITH_NVRAM

#define NVRAM_ADDR_REG  0xffff8961L
#define NVRAM_DATA_REG  0xffff8963L

#define NVRAM_RTC_SIZE  14          /* first 14 registers are RTC */
#define NVRAM_START     NVRAM_RTC_SIZE

#if defined(MACHINE_RAVEN)
#define NVRAM_SIZE      24          /* 24 bytes are RAM (starting at real register 8) */
#define NVRAM_USER_SIZE 22          /* of which the user may access 22 */
#else
#define NVRAM_SIZE      50          /* remaining 50 are RAM */
#define NVRAM_USER_SIZE 48          /* of which the user may access 48 */
#endif
#define NVRAM_CKSUM     NVRAM_USER_SIZE /* and the last 2 are checksum */

/*
 * on the TT, resetting NVRAM causes TOS3 to zero it.
 * on the Falcon and FireBee, it is set to zeroes except for a small
 * portion (see below) starting at NVRAM_INIT_START.
 *
 * we do the same to avoid problems booting Atari TOS and EmuTOS on the
 * same system.
 */
#define NVRAM_INIT_START    8
const UBYTE nvram_init[] = { 0x00, 0x2f, 0x20, 0xff, 0xff, 0xff };

int has_nvram;

#if defined(MACHINE_RAVEN)

static inline UBYTE nvram_read(int index) { return raven_nvram_readb(index); }
static inline void nvram_write(int index, UBYTE value) { return raven_nvram_writeb(index, value); }
static inline void nvram_detect(void) { return raven_nvram_detect(); }

#else

static inline UBYTE nvram_read(int index)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;
    *addr_reg = index + NVRAM_START;
    return *data_reg;    
}

static inline void nvram_write(int index, UBYTE value)
{
    volatile UBYTE *addr_reg = (volatile UBYTE *)NVRAM_ADDR_REG;
    volatile UBYTE *data_reg = (volatile UBYTE *)NVRAM_DATA_REG;
    *addr_reg = index + NVRAM_START;
    *data_reg=value;    
}

static inline void nvram_detect(void)
{
    if (check_read_byte(NVRAM_ADDR_REG))
        has_nvram = 1;
    else has_nvram = 0;
}

#endif


/*
 * detect_nvram - detect the NVRAM
 */
void detect_nvram(void)
{
    return nvram_detect();
}

/*
 * get_nvram_rtc - read the realtime clock from NVRAM
 */
UBYTE get_nvram_rtc(int index)
{
    int ret_value = 0;

    if (has_nvram)
    {
        if ((index >= 0) && (index < NVRAM_RTC_SIZE))
        {
            ret_value = nvram_read(index);
        }
    }

    return ret_value;
}

/*
 * set_nvram_rtc - set the realtime clock in NVRAM
 */
void set_nvram_rtc(int index, int data)
{
    if (has_nvram)
    {
        if ((index >= 0) && (index < NVRAM_RTC_SIZE))
        {
            nvram_write(index, data);
        }
    }
}

/*
 * compute_sum - internal checksum handling
 */
static UWORD compute_sum(void)
{
    UBYTE sum;
    int i;

    for (i = 0, sum = 0; i < NVRAM_USER_SIZE; i++)
    {
        sum += nvram_read(i + NVRAM_START);
    }

    return MAKE_UWORD(~sum, sum);
}

/*
 * get_sum - read checksum from NVRAM
 */
static UWORD get_sum(void)
{
    UWORD sum;
    sum = nvram_read(NVRAM_START + NVRAM_CKSUM) << 8;
    sum |= nvram_read(NVRAM_START + NVRAM_CKSUM + 1);
    return sum;
}

/*
 * set_sum - write checksum to NVRAM
 */
static void set_sum(UWORD sum)
{
    nvram_write(NVRAM_START + NVRAM_CKSUM, HIBYTE(sum));
    nvram_write(NVRAM_START + NVRAM_CKSUM + 1, LOBYTE(sum));
}

/*
 * nvmaccess - XBIOS read/write/reset NVRAM
 *
 * Arguments:
 *
 *   type   - 0:read, 1:write, 2:reset
 *   start  - start address for operation
 *   count  - count of bytes
 *   buffer - buffer for operations
 */
WORD nvmaccess(WORD type, WORD start, WORD count, UBYTE *buffer)
{
    int i;

    if (!has_nvram)
        return 0x2E;

    if (type == 2)      /* reset all */
    {
        for (i = 0; i < NVRAM_USER_SIZE; i++)
        {
            nvram_write(i + NVRAM_START, 0);
        }
        if (cookie_mch == MCH_TT)
            set_sum(compute_sum());
        else
            nvmaccess(1,NVRAM_INIT_START,ARRAY_SIZE(nvram_init),CONST_CAST(UBYTE *,nvram_init));
        return 0;
    }

    if ((buffer == NULL) || (start < 0) || (count < 1) || ((start + count) > NVRAM_USER_SIZE))
        return -5;

    switch(type) {
    case 0:         /* read */
        {
            UWORD expected = compute_sum();
            UWORD actual = get_sum();

            if (expected != actual)
            {
                KDEBUG(("wrong nvram: expected=0x%04x actual=0x%04x\n", expected, actual));
                /* wrong checksum, return error code */
                return -12;
            }
            for (i = start; i < start + count; i++)
            {
                *buffer++ = nvram_read(i + NVRAM_START);
            }
        }
        break;
    case 1:         /* write */
        for (i = start; i < start + count; i++)
        {
            nvram_write(i + NVRAM_START, *buffer++);
        }
        set_sum(compute_sum());
        break;
    default:
        /* wrong operation code! */
        return -5;
    }

    return 0;
}

#endif  /* CONF_WITH_NVRAM */
