/*
 * RFC1055 Serial Line Internet Protocol driver.
 */

#include <stdint.h>
#include <stdbool.h>

bool slipdev_init(unsigned short serial_dev, unsigned long serial_speed);
void slipdev_done(void);
void slipdev_send(void);
uint16_t slipdev_poll(void);
