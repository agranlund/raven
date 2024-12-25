#ifndef _ISA_RW_H_
#define _ISA_RW_H_

#include "isa_core.h"


/* big endian */
extern uint8_t _ISA_API inp_be(uint16l_t port);
extern void _ISA_API outp_be(uint16l_t port, uint8l_t data);
extern uint16_t _ISA_API inpw_be(uint16l_t port);
extern void _ISA_API outpw_be(uint16l_t port, uint16l_t data);
extern void _ISA_API outp_be_buf(uint16l_t port, uint8_t* data, int32_t count);
extern void _ISA_API outpw_be_buf(uint16l_t port, uint16_t* data, int32_t count);
extern void _ISA_API inp_be_buf(uint16l_t port, uint8_t* data, int32_t count);
extern void _ISA_API inpw_be_buf(uint16l_t port, uint16_t* data, int32_t count);
extern void _ISA_API outp_be_seq(uint16_t* seq, int32_t count);
extern void _ISA_API outpw_be_seq(uint16_t* seq, int32_t count);

/* little endian lane swapped */
#define inp_lels inp_be
#define outp_lels outp_be
extern uint16_t _ISA_API inpw_lels(uint16l_t port);
extern void _ISA_API outpw_lels(uint16l_t port, uint16l_t data);
#define inp_lels_buf inp_be_buf
#define outp_lels_buf outp_be_buf
extern void _ISA_API outpw_lels_buf(uint16l_t port, uint16_t* data, int32_t count);
extern void _ISA_API inpw_lels_buf(uint16l_t port, uint16_t* data, int32_t count);
#define outp_lels_seq outp_be_seq
extern void _ISA_API outpw_lels_seq(uint16_t* seq, int32_t count);

/* little endian address swapped */
extern uint8_t _ISA_API inp_leas(uint16l_t port);
extern void _ISA_API outp_leas(uint16l_t port, uint8l_t data);
extern uint16_t _ISA_API inpw_leas(uint16l_t port);
extern void _ISA_API outpw_leas(uint16l_t port, uint16l_t data);
extern void _ISA_API outp_leas_buf(uint16l_t port, uint8_t* data, int32_t count);
extern void _ISA_API outpw_leas_buf(uint16l_t port, uint16_t* data, int32_t count);
extern void _ISA_API inp_leas_buf(uint16l_t port, uint8_t* data, int32_t count);
extern void _ISA_API inpw_leas_buf(uint16l_t port, uint16_t* data, int32_t count);
extern void _ISA_API outp_leas_seq(uint16_t* seq, int32_t count);
extern void _ISA_API outpw_leas_seq(uint16_t* seq, int32_t count);


#endif /* _ISA_RW_H_ */
