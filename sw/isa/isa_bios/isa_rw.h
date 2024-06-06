#ifndef _ISA_RW_H_
#define _ISA_RW_H_

#include "isa_core.h"


// big endian
extern uint8 inp_be(uint16 port);
extern void outp_be(uint16 port, uint8 data);
extern uint16 inpw_be(uint16 port);
extern void outpw_be(uint16 port, uint16 data);
extern void outp_be_buf(uint16 port, uint8* data, int count);
extern void outpw_be_buf(uint16 port, uint16* data, int count);
extern void inp_be_buf(uint16 port, uint8* data, int count);
extern void inpw_be_buf(uint16 port, uint16* data, int count);
extern void outp_be_seq(uint16* seq, int count);
extern void outpw_be_seq(uint16* seq, int count);

// little endian lane swapped
#define inp_lels inp_be
#define outp_lels outp_be
extern uint16 inpw_lels(uint16 port);
extern void outpw_lels(uint16 port, uint16 data);
#define inp_lels_buf inp_be_buf
#define outp_lels_buf outp_be_buf
extern void outpw_lels_buf(uint16 port, uint16* data, int count);
extern void inpw_lels_buf(uint16 port, uint16* data, int count);
#define outp_lels_seq outp_be_seq
extern void outpw_lels_seq(uint16* seq, int count);

// little endian address swapped
extern uint8 inp_leas(uint16 port);
extern void outp_leas(uint16 port, uint8 data);
extern uint16 inpw_leas(uint16 port);
extern void outpw_leas(uint16 port, uint16 data);
extern void outp_leas_buf(uint16 port, uint8* data, int count);
extern void outpw_leas_buf(uint16 port, uint16* data, int count);
extern void inp_leas_buf(uint16 port, uint8* data, int count);
extern void inpw_leas_buf(uint16 port, uint16* data, int count);
extern void outp_leas_seq(uint16* seq, int count);
extern void outpw_leas_seq(uint16* seq, int count);


#endif // _ISA_RW_H_
