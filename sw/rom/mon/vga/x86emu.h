
#ifndef _X86_EMU_H_
#define _X86_EMU_H_

#include "x86core.h"

typedef union
{
   struct {
      unsigned long eax, ebx, ecx, edx, esi, edi, cflag;
   } e;
   struct {
      unsigned short ax_hi, ax;
      unsigned short bx_hi, bx;
      unsigned short cx_hi, cx;
      unsigned short dx_hi, dx;
      unsigned short si_hi, si;
      unsigned short di_hi, di;
      unsigned short cflag_hi, cflag;
   } x;
   struct {
      unsigned short ax_hi; unsigned char ah, al;
      unsigned short bx_hi; unsigned char bh, bl;
      unsigned short cx_hi; unsigned char ch, cl;
      unsigned short dx_hi; unsigned char dh, dl;
   } h;
} x86_regs_t;

typedef struct
{
   unsigned short es, cs, ss, ds, fs, gs;
} x86_sregs_t;

extern struct X86EMU* x86emu;

#if !defined(EXCLUDE_X86CORE)
struct X86EMU* x86_Init(void* ram_ptr, uint32_t ram_size, uint32_t isa_io, uint32_t isa_ram);
#else
struct X86EMU* x86_Init(struct X86EMU* emu);
#endif

struct X86EMU* x86_Get();

void x86_Call(uint16_t seg, uint16_t off);
void x86_Run(uint16_t off, x86_regs_t* regs, x86_sregs_t* sregs);
void x86_Int(uint8_t num, x86_regs_t* regs);

void x86_GetRegs(x86_regs_t* regs);
void x86_SetRegs(x86_regs_t* regs);

void x86_GetSRegs(x86_sregs_t* sregs);
void x86_SetSRegs(x86_sregs_t* sregs);


static inline uint8_t   x86_ReadByte(struct X86EMU *emu, uint32_t addr) { return emu->emu_rdb(emu, addr); }
static inline uint16_t  x86_ReadWord(struct X86EMU *emu, uint32_t addr) { return emu->emu_rdw(emu, addr); }
static inline uint8_t   x86_ReadLong(struct X86EMU *emu, uint32_t addr) { return emu->emu_rdl(emu, addr); }

static inline void      x86_WriteByte(struct X86EMU *emu, uint32_t addr, uint8_t  val) { emu->emu_wrb(emu, addr, val); }
static inline void      x86_WriteWord(struct X86EMU *emu, uint32_t addr, uint16_t val) { emu->emu_wrw(emu, addr, val); }
static inline void      x86_WriteLong(struct X86EMU *emu, uint32_t addr, uint32_t val) { emu->emu_wrl(emu, addr, val); }

static inline void      x86_PushWord(struct X86EMU *emu, uint16_t val) { emu->x86.R_ESP -= 2; x86_WriteWord(emu, (((uint32_t) emu->x86.R_SS) << 4) + emu->x86.R_SP, val); }


#endif // _X86_EMU_H_
