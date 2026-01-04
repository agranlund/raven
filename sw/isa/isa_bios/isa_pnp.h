#ifndef _ISA_PNP_H_
#define _ISA_PNP_H_

/*#include "isa_bios.h"*/

#define PNP_MAX_NAME        64
#define PNP_MAX_CARDS       8
#define PNP_MAX_DEVS        8
#define PNP_MAX_CONFS       16
#define PNP_MAX_IDS         5

typedef struct {
    uint8_t         conf;                           /* 1 x 1 =  1 */
    uint8_t         reserved;                       /* 1 x 1 =  1 */
    uint16_t        iobase[ISA_MAX_DEV_PORT];       /* 8 x 2 = 16 */
    uint32_t        membase[ISA_MAX_DEV_MEM];       /* 4 * 2 = 16 */
    uint8_t         irq[ISA_MAX_DEV_IRQ];           /* 2 x 1 =  2 */
    uint8_t         dma[ISA_MAX_DEV_DMA];           /* 2 x 1 =  2 */
} pnp_setting_t;    /* 38 bytes */

typedef struct {
    uint32_t        base_min;
    uint32_t        base_max;
    uint32_t        length;
    uint32_t        align;
    uint16_t        flags;
} pnp_range_t;

typedef struct {
    void*           next;
    uint8_t         id;
    uint8_t         reserved;
    uint16_t        flags;
    uint8_t         nio, nmem, nirq, ndma;
    pnp_range_t*    iorange;
    pnp_range_t*    memrange;
    uint32_t*       irqmask;
    uint16_t*       dmamask;
} pnp_conf_t;

typedef struct {
    void*           parent;
    void*           next;
    char*           name;
    uint32_t*       ids;
    uint16_t        flags;
    uint8_t         csn;
    uint8_t         ldn;
    pnp_conf_t*     confs;
    pnp_setting_t*  settings;
} pnp_device_t;

typedef struct {
    void*           next;
    char*           name;
    uint16_t        flags;
    uint32_t        id;
    uint32_t        sn;
    uint8_t         pnp_version;
    uint8_t         card_version;
    uint8_t         csn;
    uint8_t         ndevices;
    pnp_device_t*   devices;
} pnp_card_t;


extern bool             pnp_init(void);
extern int              pnp_enumerate(void);
extern int              pnp_configure(void);

/* utils */
extern const char*      pnp_id_to_string(uint32_t id);
extern uint32_t         pnp_string_to_id(const char* str);
extern pnp_card_t*      pnp_get_card(uint32_t id);

/* card functions */
extern pnp_device_t*    pnp_card_get_device(pnp_card_t* card, uint32_t id);

/* device functions */
extern pnp_card_t*      pnp_device_get_card(pnp_device_t* dev);
extern pnp_setting_t*   pnp_device_get_settings(pnp_device_t* dev);
extern pnp_conf_t*      pnp_device_get_conf(pnp_device_t*, uint8_t id);
extern bool             pnp_device_configure(pnp_device_t* dev);
extern void             pnp_device_init_settings(pnp_device_t* dev, uint8_t conf_id);

/* lowlevel, generally do not touch */
extern bool             pnp_config_begin(uint8_t csn, uint8_t ldn);
extern void             pnp_config_end(void);
extern void             pnp_write_reg(uint8_t addr, uint8_t data);
extern uint8_t          pnp_read_reg(uint8_t addr);

/* setup, generally do not touch */
extern pnp_card_t*      pnp_create_card(uint32_t id);
extern pnp_device_t*    pnp_create_device(pnp_card_t* card, uint32_t id);
extern pnp_conf_t*      pnp_create_conf(pnp_device_t* dev);

#endif /* _ISA_PNP_H_ */
