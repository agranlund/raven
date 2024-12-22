#ifndef cpu_h
#define cpu_h

#include <stdint.h>
#include <stdbool.h>
#include "musashi/m68k.h"

/* Read/write macros */
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) |          \
(BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) |         \
((BASE)[(ADDR)+1]<<16) |        \
((BASE)[(ADDR)+2]<<8) |     \
(BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL)&0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff;       \
(BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff;      \
(BASE)[(ADDR)+1] = ((VAL)>>16)&0xff;    \
(BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;     \
(BASE)[(ADDR)+3] = (VAL)&0xff


extern unsigned int cpu_read_byte(unsigned int address);
extern unsigned int cpu_read_word(unsigned int address);
extern unsigned int cpu_read_long(unsigned int address);
extern void cpu_write_byte(unsigned int address, unsigned int value);
extern void cpu_write_word(unsigned int address, unsigned int value);
extern void cpu_write_long(unsigned int address, unsigned int value);

extern void cpu_callback_trap(uint32_t vector);
extern void cpu_callback_instruction(void);
extern void cpu_callback_pc_changed(uint32_t pc);

extern void cpu_pulse_reset(void);
extern void cpu_set_fc(unsigned int fc);
extern int  cpu_irq_ack(int level);

extern bool cpu_load(uint8_t* bin, uint32_t tpasize, const char * p_cmdlin, uint32_t parent_pd);
extern uint32_t cpu_init(int argc, char **argv, char **envp);

#define cpu_run(cycles)     m68k_execute(cycles)

#endif /* cpu_h */
