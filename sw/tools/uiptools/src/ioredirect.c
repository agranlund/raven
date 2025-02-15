#include "ioredirect.h"
#include "logging.h"

#include "uip_arp.h"
#include "uip-split.h"
#include "drivers/driver.h"

#include <stdint.h>
#include <string.h>

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#define trap_13_ww(n, a)                        \
__extension__                                   \
({                                              \
    register long retvalue __asm__("d0");       \
    short _a = (short)(a);                      \
                                                \
    __asm__ volatile                            \
    (                                           \
        "movw   %2,sp@-\n\t"                    \
        "movw   %1,sp@-\n\t"                    \
        "trap   #13\n\t"                        \
        "addql  #4,sp"                          \
    : "=r"(retvalue)            /* outputs */   \
    : "g"(n), "r"(_a)           /* inputs  */   \
    : __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2"    /* clobbered regs */   \
      AND_MEMORY                                \
    );                                          \
    retvalue;                                   \
})

#define trap_13_www(n, a, b)                    \
__extension__                                   \
({                                              \
    register long retvalue __asm__("d0");       \
    short _a = (short)(a);                      \
    short _b = (short)(b);                      \
                                                \
    __asm__ volatile                            \
    (                                           \
        "movw   %3,sp@-\n\t"                    \
        "movw   %2,sp@-\n\t"                    \
        "movw   %1,sp@-\n\t"                    \
        "trap   #13\n\t"                        \
        "addql  #6,sp"                          \
    : "=r"(retvalue)            /* outputs */   \
    : "g"(n), "r"(_a), "r"(_b)      /* inputs  */       \
    : __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2"    /* clobbered regs */   \
      AND_MEMORY                                \
    );                                          \
    retvalue;                                   \
})

#define Bconstat_passthrough(dev) (short)trap_13_ww((short)(0x8001),(short)(dev))
#define Bconin_passthrough(dev) (long)trap_13_ww((short)(0x8002),(short)(dev))
#define Bconout_passthrough(dev,c) (long)trap_13_www((short)(0x8003),(short)(dev),(short)((c) & 0xFF))
#define Bcostat_passthrough(dev) (short)trap_13_ww((short)(0x8008),(short)(dev))

void ioredirect_trap_install(void);
void ioredirect_trap_restore(void);

struct uip_udp_conn* conn = NULL;

uint8_t buffer[256];
uint8_t buffer_read_ptr;
uint8_t buffer_write_ptr;

uint8_t ioredirect_out_buff[2048];
uint8_t* ioredirect_out_ptr = NULL;
size_t ioredirect_out_len = 0;

static void
buffer_add_byte(uint8_t data)
{
    if (buffer_write_ptr >= buffer_read_ptr) {
        buffer[buffer_write_ptr] = data;
        buffer_write_ptr++;
    } else if ((buffer_write_ptr + 1) < buffer_write_ptr) {
        buffer[buffer_write_ptr] = data;
        buffer_write_ptr++;
    } else {
        /* buffer full: drop*/
    }
}

static inline uint32_t
buffer_get_byte()
{
    if (buffer_read_ptr != buffer_write_ptr) {
        uint32_t keycode = buffer[buffer_read_ptr];
        buffer_read_ptr++;
        return keycode;
    }
    return 0;
}

static uint32_t
buffer_get_scancode()
{
    return (buffer_get_byte() << 23) | (buffer_get_byte() << 16)
        | (buffer_get_byte() << 8) | buffer_get_byte();
}

int32_t buffer_can_read()
{
    return buffer_read_ptr != buffer_write_ptr ? -1 : 0;
}

static inline void
receive_udp_data()
{
    while(uip_len = driver_poll()) {
        if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
            uip_arp_arpin();
            if(uip_len > 0) {
                driver_send();
                continue;
            }
        } else if(BUF->type == htons(UIP_ETHTYPE_IP)) {
            uip_udp_receive_conn(conn);
        }
    }
}

static inline void
send_udp_data()
{
    uip_udp_periodic_conn(conn);
    if(uip_len > 0) {
        ip_packet_output();
    }
}

static inline void
send_udp_data_packet(size_t len)
{
    ioredirect_out_len = len;
    send_udp_data();
}

static inline void
send_udp_done_packet()
{
    ioredirect_out_len = 1;
    ioredirect_out_ptr = ioredirect_out_buff;
    ioredirect_out_ptr[0] = 0xff;
    send_udp_data();
}

void ioredirect_cconws(const uint8_t* buf)
{
    ioredirect_out_ptr = buf;
    send_udp_data_packet(strlen(buf));
}

int32_t ioredirect_Bconin (int16_t dev)
{
    if (dev == 2) {
        while(1) {
            receive_udp_data();

            /* Check for new remote input */
            if (buffer_can_read()) {
                return buffer_get_scancode();
            }

            /* Check for new local input */
            if (Bconstat_passthrough(dev)) {
                return Bconin_passthrough(dev);
            }
        }
    }

    return Bconin_passthrough(dev);
}

void ioredirect_Bconout (int32_t dev, int32_t c)
{
    if (dev == 2) {
        // try to colapse escape code to send fewer packets
        if (ioredirect_out_len == 3 && ioredirect_out_buff[0] == 0x1b) {
            ioredirect_out_buff[ioredirect_out_len] = (uint8_t)c;
            ioredirect_out_ptr = ioredirect_out_buff;
            ioredirect_out_len++;
            send_udp_data();
        } else if (ioredirect_out_len == 2 && ioredirect_out_buff[0] == 0x1b) {
            ioredirect_out_buff[ioredirect_out_len] = (uint8_t)c;
            ioredirect_out_ptr = ioredirect_out_buff;
            ioredirect_out_len++;
        } else if (ioredirect_out_len == 1 && ioredirect_out_buff[0] == 0x1b) {
            ioredirect_out_buff[ioredirect_out_len] = (uint8_t)c;
            ioredirect_out_ptr = ioredirect_out_buff;
            ioredirect_out_len++;
            if (c != 'Y') {
                send_udp_data();
            }
        } else {
            ioredirect_out_buff[0] = (uint8_t)c;
            ioredirect_out_ptr = ioredirect_out_buff;
            if (c != 0x1b) {
                send_udp_data_packet(1);
            } else {
                ioredirect_out_len = 1;
            }
        }
        return;
    }
    Bconout_passthrough(dev, c);
}

int16_t ioredirect_Bconstat (int16_t dev)
{
    receive_udp_data();

    if (dev == 2) {
        int32_t r = 0;
        r = buffer_can_read();
        if (r) {
            return r;
        }
    }
    return Bconstat_passthrough(dev);
}

int32_t ioredirect_Bcostat (int16_t dev)
{
    if (dev == 2) {
        return -1;
    }
    return Bcostat_passthrough(dev);
}

void ioredirect_appcall()
{
    if(uip_newdata() && uip_len) {
        for (uint16_t i = 0; i < uip_len; i++) {
            buffer_add_byte(((uint8_t*)uip_appdata)[i]);
        }
    } else if (uip_poll() && conn && ioredirect_out_len && ioredirect_out_ptr) {
        uip_send(ioredirect_out_ptr, ioredirect_out_len);
        ioredirect_out_ptr = NULL;
        ioredirect_out_len = 0;
    }
}

void ioredirect_start(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4)
{
    conn = NULL;
    buffer_read_ptr = 0;
    buffer_write_ptr = 0;

    uip_ipaddr_t addr;
    uip_ipaddr(addr, ip1, ip2, ip3, ip4);
    conn = uip_udp_new(&addr, HTONS(33334));
    if(conn != NULL) {
        uip_udp_bind(conn, HTONS(33332));
    }

    ioredirect_trap_install();
}

void ioredirect_stop()
{
    ioredirect_trap_restore();
    send_udp_done_packet();

    if(conn != NULL) {
        uip_udp_remove(conn);
    }
}
