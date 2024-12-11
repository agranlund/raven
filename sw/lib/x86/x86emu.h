
#ifndef _X86_EMU_H_
#define _X86_EMU_H_

#ifdef RAVEN_ROM
#include "lib.h"
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "x86core.h"

#define x86_swap16(x)    __builtin_bswap16((x))
#define x86_swap32(x)    __builtin_bswap32((x))

static inline void      x86_Run(struct X86EMU* emu)                                     { emu->_X86EMU_run(emu); }
static inline void      x86_Call(struct X86EMU* emu, uint16_t seg, uint16_t offs)       { emu->_X86EMU_call(emu, seg, offs); }
static inline void      x86_Int(struct X86EMU* emu, uint8_t num)                        { emu->_X86EMU_int(emu, num); }

static inline uint8_t   x86_ReadByte(struct X86EMU *emu, uint32_t addr)                 { return emu->emu_rdb(emu, addr); }
static inline uint16_t  x86_ReadWord(struct X86EMU *emu, uint32_t addr)                 { return emu->emu_rdw(emu, addr); }
static inline uint8_t   x86_ReadLong(struct X86EMU *emu, uint32_t addr)                 { return emu->emu_rdl(emu, addr); }

static inline void      x86_WriteByte(struct X86EMU *emu, uint32_t addr, uint8_t  val)  { emu->emu_wrb(emu, addr, val); }
static inline void      x86_WriteWord(struct X86EMU *emu, uint32_t addr, uint16_t val)  { emu->emu_wrw(emu, addr, val); }
static inline void      x86_WriteLong(struct X86EMU *emu, uint32_t addr, uint32_t val)  { emu->emu_wrl(emu, addr, val); }

static inline void      x86_PushWord(struct X86EMU *emu, uint16_t val)                  { emu->x86.R_ESP -= 2; x86_WriteWord(emu, (((uint32_t) emu->x86.R_SS) << 4) + emu->x86.R_SP, val); }

static inline uint16_t  x86_Segment(uint32_t addr)                                      { return (uint16_t) ((addr >> 4) & 0xf000); }
static inline uint16_t  x86_Offset(uint32_t addr)                                       { return (uint16_t) (addr & 0xffff); }
static inline uint32_t  x86_Linear(uint16_t seg, uint16_t off)                          { return (uint32_t) ((((uint32_t) (seg & 0xf000)) << 4) | off); }


extern void             x86_Create(struct X86EMU *emu, void* ram_ptr, uint32_t ram_size, uint32_t isa_io, uint32_t isa_ram);


#endif // _X86_EMU_H_
