#ifndef _ISA_RW_H_
#define _ISA_RW_H_

#include "isa_bios.h"

/* big endian */
extern uint8_t _ISA_API inp_be(uint16l_t port);
extern void _ISA_API outp_be(uint16l_t port, uint8l_t data);
extern uint16_t _ISA_API inpw_be(uint16l_t port);
extern void _ISA_API outpw_be(uint16l_t port, uint16l_t data);

/* little endian lane swapped */
#define inp_lels inp_be
#define outp_lels outp_be
extern uint16_t _ISA_API inpw_lels(uint16l_t port);
extern void _ISA_API outpw_lels(uint16l_t port, uint16l_t data);

/* little endian address swapped */
extern uint8_t _ISA_API inp_leas(uint16l_t port);
extern void _ISA_API outp_leas(uint16l_t port, uint8l_t data);
extern uint16_t _ISA_API inpw_leas(uint16l_t port);
extern void _ISA_API outpw_leas(uint16l_t port, uint16l_t data);


#endif /* _ISA_RW_H_ */
