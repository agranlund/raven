#ifndef _GEMDOS_H_
#define _GEMDOS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define GEMDOS_E_OK 	(0)
#define GEMDOS_EFILNF	(-33)
#define GEMDOS_E_EACCDN (-36)

/* Basepage (PD) offsets */
#define OFF_P_LOWTPA  (0)
#define OFF_P_HITPA   (4)
#define OFF_P_TBASE   (8)
#define OFF_P_TLEN    (12)
#define OFF_P_DBASE   (16)
#define OFF_P_DLEN    (20)
#define OFF_P_BBASE   (24)
#define OFF_P_BLEN    (28)
#define OFF_P_DTA     (32)
#define OFF_P_PARENT  (36)
#define OFF_P_RESRVD0 (40)
#define OFF_P_ENV     (44)
#define OFF_P_RESRVD1 (48)
#define OFF_P_CMDLIN  (128)

typedef struct file_s
{
    FILE* fd;

    char* fname;

    struct
    {
    	enum
	    {
	    	not_term = 0,
	        normal,
	        escaped,
	        extended,
	        set_xy,
	    } state;

	    char tmp;
    } term;

    int32_t (*writer) (int16_t handle, int32_t count, void *buf);
    int32_t (*reader) (int16_t handle, int32_t count, void *buf);

} file_t;

extern void 	Mfree(uint32_t block);
extern uint32_t Malloc(int32_t bytes);
extern uint32_t gemdos_dispatch(uint16_t opcode, uint32_t pd);
extern void 	gemdos_init(uint8_t* ram, uint32_t ramsize);

#endif /* _GEMDOS_H_ */
