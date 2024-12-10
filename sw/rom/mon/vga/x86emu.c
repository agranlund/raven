
#include "x86emu.h"

#define X86_VGA_RAM         0x0A0000
#define X86_VGA_ROM         0x0C0000
#define X86_SYS_BIOS        0x0F0000

#define x86mem_swap16(x)    __builtin_bswap16((x))
#define x86mem_swap32(x)    __builtin_bswap32((x))

#define x86isa_swap16(x)    __builtin_bswap16((x))
#define x86isa_swap32(x)    __builtin_bswap32((x))

#define x86printf(...)      printf(__VA_ARGS__)

struct X86EMU* x86emu = 0;

// --------------------------------------------------------------------------------------------------------

#if !defined(EXCLUDE_X86CORE)

#define ENABLE_DIRECT_VGA_MEM_READ      0
#define ENABLE_DIRECT_VGA_MEM_WRITE     1
#define ENABLE_BLOCK_BIOS_WRITES        1
#define ENABLE_WRAP_1MB_ADDRESS_SPACE   0

static inline uint8_t  isa_rdb(uint32_t addr) { return *((volatile uint8_t  *)(addr)); }
static inline uint16_t isa_rdw(uint32_t addr) { return x86isa_swap16(*((volatile uint16_t *)(addr))); }
static inline uint32_t isa_rdl(uint32_t addr) { return x86isa_swap32(*((volatile uint32_t *)(addr))); }
static inline void isa_wrb(uint32_t addr, uint8_t data)  { *((volatile uint8_t  *)(addr)) = data; }
static inline void isa_wrw(uint32_t addr, uint16_t data) { *((volatile uint16_t *)(addr)) = x86isa_swap16(data); }
static inline void isa_wrl(uint32_t addr, uint32_t data) { *((volatile uint32_t *)(addr)) = x86isa_swap32(data); }

static uint8_t  x86_inb(struct X86EMU *emu, uint16_t port) { return isa_rdb(emu->isa_iobase + port); }
static uint16_t x86_inw(struct X86EMU *emu, uint16_t port) { return isa_rdw(emu->isa_iobase + port); }
static uint32_t x86_inl(struct X86EMU *emu, uint16_t port) { return isa_rdl(emu->isa_iobase + port); }
static void x86_outb(struct X86EMU *emu, uint16_t port, uint8_t  val) { isa_wrb(emu->isa_iobase + port, val); }
static void x86_outw(struct X86EMU *emu, uint16_t port, uint16_t val) { isa_wrw(emu->isa_iobase + port, val); }
static void x86_outl(struct X86EMU *emu, uint16_t port, uint32_t val) { isa_wrl(emu->isa_iobase + port, val); }

static void x86_halt(struct X86EMU* emu) {
    x86dbg("\n");
    longjmp(emu->exec_state, 1);
}

static uint8_t x86_rdb(struct X86EMU *emu, uint32_t addr) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86dbg("rdb %08x\n", addr);
        x86_halt(emu);
        return 0;
    }
    else if (ENABLE_DIRECT_VGA_MEM_READ && addr >= 0xa0000 && addr < 0xc0000) {
        return isa_rdb(emu->isa_membase + addr);
    } else {
        return emu->mem_base[addr];
    }
}
static uint16_t x86_rdw(struct X86EMU *emu, uint32_t addr) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86dbg("rdw %08x\n", addr);
        x86_halt(emu);
        return 0;
    } else if (ENABLE_DIRECT_VGA_MEM_READ && addr >= 0xa0000 && addr < 0xc0000) {
        return isa_rdw(emu->isa_membase + addr);
    } else {
        return x86mem_swap16(*((uint16_t*)(emu->mem_base + addr)));
    }
}
static uint32_t x86_rdl(struct X86EMU *emu, uint32_t addr) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86dbg("rdl %08x\n", addr);
        x86_halt(emu);
        return 0;
    } else if (ENABLE_DIRECT_VGA_MEM_READ && addr >= 0xa0000 && addr < 0xc0000) {
        return isa_rdl(emu->isa_membase + addr);
    } else {
        return x86mem_swap32(*((uint32_t*)(emu->mem_base + addr)));
    }
}
static void x86_wrb(struct X86EMU *emu, uint32_t addr, uint8_t val) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86dbg("wrb %08x\n", addr);
        x86_halt(emu);
    } else if (ENABLE_DIRECT_VGA_MEM_WRITE && addr >= 0xa0000 && addr < 0xc0000) {
        isa_wrb(emu->isa_membase + addr, val);
    } else if (!ENABLE_BLOCK_BIOS_WRITES || addr < 0xa0000) {
        emu->mem_base[addr] = val;
    }
}
static void x86_wrw(struct X86EMU *emu, uint32_t addr, uint16_t val) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86dbg("wrw %08x\n", addr);
        x86_halt(emu);
        return;
    } else if (ENABLE_DIRECT_VGA_MEM_WRITE && addr >= 0xa0000 && addr < 0xc0000) {
        isa_wrw(emu->isa_membase + addr, val);
    } else if (!ENABLE_BLOCK_BIOS_WRITES || addr < 0xa0000) {
        *((uint16_t*)(emu->mem_base + addr)) = x86mem_swap16(val);
    }
}
static void x86_wrl(struct X86EMU *emu, uint32_t addr, uint32_t val) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86dbg("wrl %08x\n", addr);
        x86_halt(emu);
        return;
    } else if (ENABLE_DIRECT_VGA_MEM_WRITE && addr >= 0xa0000 && addr < 0xc0000) {
        isa_wrl(emu->isa_membase + addr, val);
    } else if (!ENABLE_BLOCK_BIOS_WRITES || addr < 0xa0000) {
        *((uint32_t*)(emu->mem_base + addr)) = x86mem_swap32(val);
    }
}

static void x86_int(struct X86EMU *emu, int num)
{
    int bios_int = 1;
    switch (num)
    {
        case 0x10:  // video interrupt
        case 0x42:  // video interrupt
        case 0x6d:  // vga internal interrupt
        {
            // vga write string
            if (emu->x86.register_a.I8_reg.h_reg == 0x13)
            {
                int num_chars = emu->x86.register_c.I16_reg.x_reg;
                int seg = emu->x86.register_es;
                int off = emu->x86.register_bp.I16_reg.x_reg;
                int str = (seg << 4) + off;
                for (int i = 0; i < num_chars; i++) {
                    x86printf("%c", * (char *)(emu->mem_base + str + i));
                }
            }
            uint32_t vec = (uint32_t) (x86_ReadWord(emu, num << 2) + (x86_ReadWord(emu, (num << 2) + 2) << 4));
            //x86dbg("int %02x : %08x\n", num, vec);
            if (vec == 0) {
                x86err("uninitialised int vector\n");
            } else if (vec == 0xff065) {
                bios_int = 0;
            } else if (vec >= 0x00100000) {
                // done by cirrus logic GD5426
                bios_int = 0;
            }
        } break;

        case 0x15:
            bios_int = 0;
            break;

        case 0x16:
            bios_int = 1;
            break;

        case 0x1a:
            //ret = x86_pcibios_handler(emu);
            bios_int = 0;
            break;

        case 0xe6:
            bios_int = 1;
            break;

        default:
            x86dbg("unhandled interrupt 0x%x\n", num);
            break;
    }

    if (bios_int)
    {
        uint32_t eflags;
        eflags = emu->x86.R_EFLG;
        x86_PushWord(emu, eflags);
        x86_PushWord(emu, emu->x86.R_CS);
        x86_PushWord(emu, emu->x86.R_IP);
        emu->x86.R_CS = x86_ReadWord(emu, (num << 2) + 2);
        emu->x86.R_IP = x86_ReadWord(emu, num << 2);
    }
}

void x86_Setup(struct X86EMU* emu, void* ram_ptr, uint32_t ram_size, uint32_t isa_io, uint32_t isa_ram)
{
    emu->x86.R_AX = 0x00;
    emu->x86.R_DX = 0x00;
    emu->x86.R_IP = 0x0000;
    emu->x86.R_CS = 0x0000;
    emu->x86.R_SS = 0x0000;
    emu->x86.R_SP = 0xfffe;
    emu->x86.R_DS = 0x0000;
    emu->x86.R_ES = 0x0000;

    emu->emu_inb = x86_inb;
    emu->emu_inw = x86_inw;
    emu->emu_inl = x86_inl;
    
    emu->emu_outb = x86_outb;
    emu->emu_outw = x86_outw;
    emu->emu_outl = x86_outl;

    emu->emu_rdb = x86_rdb;
    emu->emu_rdw = x86_rdw;
    emu->emu_rdl = x86_rdl;

    emu->emu_wrb = x86_wrb;
    emu->emu_wrw = x86_wrw;
    emu->emu_wrl = x86_wrl;

    emu->mem_base = (void *) ram_ptr;
    emu->mem_size = ram_size;

    emu->isa_iobase = isa_io;
    emu->isa_membase = isa_ram;

    memset(emu->mem_base, 0xf4, emu->mem_size);
    emu->_X86EMU_run = X86EMU_exec;

    for (int i = 0; i < 256; i++) {
        emu->_X86EMU_intrTab[i] = x86_int;
    }

    char *date = "01/01/99";
    for (int i = 0; date[i]; i++) {
        emu->emu_wrb(emu, 0xffff5 + i, date[i]);
    }
    emu->emu_wrb(emu, 0xffff7, '/');
    emu->emu_wrb(emu, 0xffffa, '/');
}

static struct X86EMU static_x86emu;
struct X86EMU* x86_Init(void* ram_ptr, uint32_t ram_size, uint32_t isa_io, uint32_t isa_ram)
{
    x86emu = &static_x86emu;
    x86_Setup(x86emu, ram_ptr, ram_size, isa_io, isa_ram);
    return x86emu;
}

#else

struct X86EMU* x86_Init(struct X86EMU* emu) {
    x86emu = emu;
    return x86emu;
}

#endif /* !defined(EXCLUDE_X86CORE) */


// --------------------------------------------------------------------------------------------------------


struct X86EMU* x86_Get() {
    return x86emu;
}

void x86_SetRegs(x86_regs_t* regs) {
    x86emu->x86.register_a.I32_reg.e_reg = regs->e.eax;
    x86emu->x86.register_b.I32_reg.e_reg = regs->e.ebx;
    x86emu->x86.register_c.I32_reg.e_reg = regs->e.ecx;
    x86emu->x86.register_d.I32_reg.e_reg = regs->e.edx;
    x86emu->x86.register_si.I32_reg.e_reg = regs->e.esi;
    x86emu->x86.register_di.I32_reg.e_reg = regs->e.edi;
}

void x86_GetRegs(x86_regs_t* regs) {
    regs->e.eax = x86emu->x86.register_a.I32_reg.e_reg;
    regs->e.ebx = x86emu->x86.register_b.I32_reg.e_reg;
    regs->e.ecx = x86emu->x86.register_c.I32_reg.e_reg;
    regs->e.edx = x86emu->x86.register_d.I32_reg.e_reg;
    regs->e.esi = x86emu->x86.register_si.I32_reg.e_reg;
    regs->e.edi = x86emu->x86.register_di.I32_reg.e_reg;
}

void x86_SetSRegs(x86_sregs_t* sregs) {
    x86emu->x86.register_cs = sregs->cs;
    x86emu->x86.register_ds = sregs->ds;
    x86emu->x86.register_es = sregs->es;
    x86emu->x86.register_fs = sregs->fs;
    x86emu->x86.register_gs = sregs->gs;
    x86emu->x86.register_ss = sregs->ss;
}

void x86_GetSRegs(x86_sregs_t* sregs) {
    sregs->cs = x86emu->x86.register_cs;
    sregs->ds = x86emu->x86.register_ds;
    sregs->es = x86emu->x86.register_es;
    sregs->fs = x86emu->x86.register_fs;
    sregs->gs = x86emu->x86.register_gs;
    sregs->ss = x86emu->x86.register_ss;
}

static inline void x86_Exec(struct X86EMU* emu) {
    emu->_X86EMU_run(emu);
}

void x86_Int(uint8_t num, x86_regs_t* regs) {
    x86_SetRegs(regs);
    x86emu->x86.register_ss = 0x0000;
    x86emu->x86.register_sp.I16_reg.x_reg = 0xfffe;
    x86_PushWord(x86emu, x86emu->x86.register_flags);
    x86_PushWord(x86emu, 0);
    x86_PushWord(x86emu, 0);
    x86emu->x86.register_flags &= ~0x0200UL;        // ie
    x86emu->x86.register_flags &= ~0x0100UL;        // trap
    x86emu->x86.register_cs = x86_ReadWord(x86emu, num * 4 + 2);
    x86emu->x86.register_ip.I16_reg.x_reg = x86_ReadWord(x86emu, num * 4 + 0);
    x86emu->x86.intr = 0;
    x86_Exec(x86emu);
}    

void x86_Call(uint16_t seg, uint16_t off) {
    x86emu->x86.register_ss = 0x0000;
    x86emu->x86.register_sp.I16_reg.x_reg = 0xfffe;
    x86_PushWord(x86emu, 0);
    x86_PushWord(x86emu, 0);
    x86emu->x86.register_cs = seg;
    x86emu->x86.register_ip.I16_reg.x_reg = off;
    x86_Exec(x86emu);
}

void x86_Run(uint16_t off, x86_regs_t* regs, x86_sregs_t* sregs) {
    x86_SetRegs(regs);
    x86_SetSRegs(sregs);
    x86emu->x86.register_ip.I16_reg.x_reg = off;
    x86_Exec(x86emu);
}



