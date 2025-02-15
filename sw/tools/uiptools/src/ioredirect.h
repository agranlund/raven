#ifndef __IOREDIRECT_H__
#define __IOREDIRECT_H__

#include <stdint.h>

void ioredirect_start(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
void ioredirect_stop();
void ioredirect_appcall();

struct __attribute__ ((__packed__)) ioredirect_state {
  struct uip_udp_conn *conn;
};

#endif // __IOREDIRECT_H__
