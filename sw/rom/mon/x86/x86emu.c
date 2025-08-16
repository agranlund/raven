
#define DEBUG_X86EMU                        0

#if (DEBUG_X86EMU == 0)
    #define DEBUG_X86CORE                   0
    #define DEBUG_X86ISA_IO                 0
    #define DEBUG_X86ISA_MEM                0
    #define DEBUG_X86PORTS                  0
    #define DEBUG_X86INTS                   0
    #define DEBUG_X86MEM                    0
#else
    #define DEBUG_X86CORE                   1
    #define DEBUG_X86ISA_IO                 1
    #define DEBUG_X86ISA_MEM                0
    #define DEBUG_X86PORTS                  1
    #define DEBUG_X86INTS                   1
    #define DEBUG_X86MEM                    1
#endif

#include "x86emu.h"

#define ENABLE_DIRECT_VGA_MEM_READ      1
#define ENABLE_DIRECT_VGA_MEM_WRITE     1
#define ENABLE_BLOCK_BIOS_WRITES        1
#define ENABLE_WRAP_1MB_ADDRESS_SPACE   0

#define X86_VGA_RAM         0x0A0000
#define X86_VGA_ROM         0x0C0000
#define X86_SYS_BIOS        0x0F0000


#define x86isa_swap16(x)    x86_swap16(x)
#define x86isa_swap32(x)    x86_swap32(x)
#define x86printf(...)      printf(__VA_ARGS__)



#if DEBUG_X86PORTS
uint32_t iodbg[256];
#define dbgport(...)  { \
    if (port < 0x100 && iodbg[port] == 0) { \
        iodbg[port]++; \
        x86printf(__VA_ARGS__); \
    } \
}
#else
#define dbgport(...)
#endif
#if (DEBUG_X86ISA_IO || DEBUG_X86ISA_MEM)
#define dbgisa(...)  x86dbg(__VA_ARGS__)
#else
#define dbgisa(...)
#endif
#if DEBUG_X86INTS
#define dbgint(...)  x86dbg(__VA_ARGS__)
#else
#define dbgint(...)
#endif
#if DEBUG_X86MEM
#define dbgmem(...)  x86dbg(__VA_ARGS__)
#else
#define dbgmem(...)
#endif


extern void X86EMU_exec(struct X86EMU *emu);
extern void X86EMU_exec_call(struct X86EMU *emu, uint16_t seg, uint16_t off);
extern void X86EMU_exec_intr(struct X86EMU *emu, uint8_t intr);
extern void X86EMU_halt_sys(struct X86EMU *);

static inline void x86_halt(struct X86EMU* emu) {
    x86dbg("\n"); longjmp(emu->exec_state, 1);
}

//--------------------------------------------------------------------------------------------------
// Real ISA access
//--------------------------------------------------------------------------------------------------


static inline uint8_t isa_rdb(uint32_t addr) {
    uint8_t be = *((volatile uint8_t*)(addr));
#if DEBUG_X86ISA_MEM
    if (!(addr & 0x01000000)) { dbgisa("isa_rdb_mem %08x %02x %02x\n", addr, be, be); }
#endif
#if DEBUG_X86ISA_IO
    if(  (addr & 0x01000000)) { dbgisa("isa_rdb_io  %08x %02x %02x\n", addr, be, be); }
#endif
    return be;
}

static inline uint32_t isa_rdw(uint32_t addr) {
    uint16_t le = *((volatile uint16_t*)addr); uint16_t be = x86isa_swap16(le);
#if DEBUG_X86ISA_MEM
    if (!(addr & 0x01000000)) { dbgisa("isa_rdw_mem %08x %04x %04x\n", addr, be, le); }
#endif
#if DEBUG_X86ISA_IO
    if(  (addr & 0x01000000)) { dbgisa("isa_rdw_io  %08x %04x %04x\n", addr, be, le); }
#endif
    return be;
}

static inline uint32_t isa_rdl(uint32_t addr) {
    uint32_t le = *((volatile uint32_t*)addr); uint32_t be = x86isa_swap32(le);
#if DEBUG_X86ISA_MEM
    if (!(addr & 0x01000000)) { dbgisa("isa_rdl_mem %08x %08x %08x\n", addr, be, le); }
#endif
#if DEBUG_X86ISA_IO
    if(  (addr & 0x01000000)) { dbgisa("isa_rdl_io  %08x %08x %08x\n", addr, be, le); }
#endif
    return be;
}

static inline void isa_wrb(uint32_t addr, uint8_t data)  {
#if DEBUG_X86ISA_MEM
    if (!(addr & 0x01000000)) { dbgisa("isa_wrb_mem %08x %02x %02x\n", addr, data, data); }
#endif
#if DEBUG_X86ISA_IO
    if(  (addr & 0x01000000)) { dbgisa("isa_wrb_io  %08x %02x %02x\n", addr, data, data); }
#endif
    *((volatile uint8_t*)(addr)) = data;
}

static inline void isa_wrw(uint32_t addr, uint16_t data) {
    uint16_t le = x86isa_swap16(data);
#if DEBUG_X86ISA_MEM
    if (!(addr & 0x01000000)) { dbgisa("isa_wrw_mem %08x %04x %04x\n", addr, data, le); }
#endif
#if DEBUG_X86ISA_IO
    if(  (addr & 0x01000000)) { dbgisa("isa_wrw_io  %08x %04x %04x\n", addr, data, le); }
#endif
    *((volatile uint16_t*)(addr)) = le;
}

static inline void isa_wrl(uint32_t addr, uint32_t data) {
    uint32_t le = x86isa_swap32(data);
#if DEBUG_X86ISA_MEM
    if (!(addr & 0x01000000)) { dbgisa("isa_wrw_mem %08x %08x %08x\n", addr, data, le); }
#endif
#if DEBUG_X86ISA_IO
    if(  (addr & 0x01000000)) { dbgisa("isa_wrw_io  %08x %08x %08x\n", addr, data, le); }
#endif
    *((volatile uint32_t*)(addr)) = le;
}

//--------------------------------------------------------------------------------------------------
// IO ports
//--------------------------------------------------------------------------------------------------
static uint32_t iobuf[256];
static uint8_t pit_bc = 0;

static bool x86_inp_emu(struct X86EMU* emu, uint16_t port, uint32_t* val) {
    if (port < 0x100) {
        switch (port) {
            case 0x42:  // pit data
                //iobuf[0x42] = (iobuf[0x42] - 0xff) & 0xffff;
                *val = (pit_bc++ & 1) ? ((iobuf[0x42] >> 8) & 0xff) : ((iobuf[0x42] >> 0) & 0xff);
                return true;

            case 0x61:  // commonly used as time delay
                // d4 changes state, indefinitely, every 15.085 microseconds
                uint8_t d4 = 0x10 & ~iobuf[port];
                iobuf[port] = (iobuf[port] & ~0x10) | d4;
                *val = iobuf[port];
                return true;

            default:
                *val = iobuf[port];
                return true;
        }
    }
    return false;
}

static bool x86_outp_emu(struct X86EMU* emu, uint16_t port, uint32_t val) {
    if (port < 0x100) {
        switch(port)
        {
            case 0x42:  // pit data
                if (pit_bc++ & 1) {
                    iobuf[0x42] = (iobuf[0x42] & 0x00ff) | ((val << 8) & 0xff00);
                } else {
                    iobuf[0x42] = (iobuf[0x42] & 0xff00) | ((val << 0) & 0x00ff);
                }
                return true;

            case 0x43:  // pit command
                pit_bc = 0;
                iobuf[port] = val;
                return true;

            case 0x80:  // post
                return false;

            default:
                iobuf[port] = val;
                return true;
        }
        return true;
    }  
    return false;
}

static uint8_t x86_inb(struct X86EMU *emu, uint16_t port) {
    uint32_t val;
    if (!x86_inp_emu(emu, port, &val)) {
        val = (uint8_t)isa_rdb(emu->isa_iobase + port);
    }
    dbgport("port_inb %04x : %02x\n", port, (uint8_t)val);
    return (uint8_t)val;
}

static uint16_t x86_inw(struct X86EMU *emu, uint16_t port) {
    uint32_t val;
    if (!x86_inp_emu(emu, port, &val)) {
        val = (uint16_t)isa_rdw(emu->isa_iobase + port);
    }
    dbgport("port_inw %04x : %04x\n", port, (uint16_t)val);
    return (uint16_t)val;
}

static uint32_t x86_inl(struct X86EMU *emu, uint16_t port) {
    uint32_t val;
    if (!x86_inp_emu(emu, port, &val)) {
        val = (uint32_t)isa_rdl(emu->isa_iobase + port);
    }
    dbgport("port_inl %04x : %08x\n", port, (uint32_t)val);
    return (uint32_t)val;
}

static void x86_outb(struct X86EMU *emu, uint16_t port, uint8_t  val)
{
    dbgport("port_outb %04x : %02x\n", port, val);
    if (!x86_outp_emu(emu, port, (uint32_t)val)) {
        isa_wrb(emu->isa_iobase + port, val);
    }
}

static void x86_outw(struct X86EMU *emu, uint16_t port, uint16_t val) {
    dbgport("port_outw %04x : %04x\n", port, val);
    if (!x86_outp_emu(emu, port, (uint32_t)val)) {
        isa_wrw(emu->isa_iobase + port, val);
    }
}

static void x86_outl(struct X86EMU *emu, uint16_t port, uint32_t val) {
    dbgport("port_outl %04x : %08x\n", port, val);
    if (!x86_outp_emu(emu, port, (uint32_t)val)) {
        isa_wrw(emu->isa_iobase + port, val);
    }
}




//--------------------------------------------------------------------------------------------------
// MEMORY ACCESS
//--------------------------------------------------------------------------------------------------

static uint8_t x86_rdb(struct X86EMU *emu, uint32_t addr) {
#if 0
    if (addr < 0x500) {
        x86dbg("x86_rdb %08x\n", addr);
    }
#endif
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86err("x86_rdb %08x\n", addr);
        x86_halt(emu);
        return 0;
    }
    else if (ENABLE_DIRECT_VGA_MEM_READ && ((addr >= 0xfffff) || (addr >= 0xa0000 && addr < 0xc0000))) {
        return isa_rdb(emu->isa_membase + addr);
    } else {
        return emu->mem_base[addr];
    }
}
static uint16_t x86_rdw(struct X86EMU *emu, uint32_t addr) {
#if 0
    if (addr < 0x500) {
        x86dbg("x86_rdw %08x\n", addr);
    }
#endif
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86err("x86_rdw %08x\n", addr);
        x86_halt(emu);
        return 0;
    } else if (ENABLE_DIRECT_VGA_MEM_READ && ((addr >= 0xfffff) || (addr >= 0xa0000 && addr < 0xc0000))) {
        if (addr > 0xfffff) {
            x86err("isa mem rdw %08x\n", addr);
            x86_halt(emu);
        }
        return isa_rdw(emu->isa_membase + addr);
    } else {
        return x86_swap16(*((uint16_t*)(emu->mem_base + addr)));
    }
}
static uint32_t x86_rdl(struct X86EMU *emu, uint32_t addr) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86err("x86_rdl %08x\n", addr);
        x86_halt(emu);
        return 0;
    } else if (ENABLE_DIRECT_VGA_MEM_READ && addr >= 0xa0000 && addr < 0xc0000) {
        return isa_rdl(emu->isa_membase + addr);
    } else {
        return x86_swap32(*((uint32_t*)(emu->mem_base + addr)));
    }
}
static void x86_wrb(struct X86EMU *emu, uint32_t addr, uint8_t val) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
#if 0
    if (addr < 0x500) {
        x86dbg("x86_wrb %08x\n", addr);
    }
#endif
    if (addr >= emu->mem_size) {
        x86err("x86_wrb %08x\n", addr);
        x86_halt(emu);
    } else if (ENABLE_DIRECT_VGA_MEM_WRITE && ((addr >= 0xfffff) || (addr >= 0xa0000 && addr < 0xc0000))) {
        isa_wrb(emu->isa_membase + addr, val);
    } else if (!ENABLE_BLOCK_BIOS_WRITES || addr < 0xa0000) {
        emu->mem_base[addr] = val;
    }
}
static void x86_wrw(struct X86EMU *emu, uint32_t addr, uint16_t val) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
#if 0
    if (addr < 0x500) {
        x86dbg("x86_wrw %08x\n", addr);
    }
#endif
    if (addr >= emu->mem_size) {
        x86err("x86_wrw %08x\n", addr);
        x86_halt(emu);
        return;
    } else if (ENABLE_DIRECT_VGA_MEM_WRITE && ((addr >= 0xfffff) || (addr >= 0xa0000 && addr < 0xc0000))) {
        isa_wrw(emu->isa_membase + addr, val);
    } else if (!ENABLE_BLOCK_BIOS_WRITES || addr < 0xa0000) {
        *((uint16_t*)(emu->mem_base + addr)) = x86_swap16(val);
    }
}
static void x86_wrl(struct X86EMU *emu, uint32_t addr, uint32_t val) {
    addr = ENABLE_WRAP_1MB_ADDRESS_SPACE ? (addr & 0xfffff) : addr;
    if (addr >= emu->mem_size) {
        x86err("x86_wrl %08x\n", addr);
        x86_halt(emu);
        return;
    } else if (ENABLE_DIRECT_VGA_MEM_WRITE && addr >= 0xa0000 && addr < 0xc0000) {
        isa_wrl(emu->isa_membase + addr, val);
    } else if (!ENABLE_BLOCK_BIOS_WRITES || addr < 0xa0000) {
        *((uint32_t*)(emu->mem_base + addr)) = x86_swap32(val);
    }
}

//--------------------------------------------------------------------------------------------------
// INT Vectors
//--------------------------------------------------------------------------------------------------

static void x86_int(struct X86EMU *emu, int num)
{
    uint32_t seg = x86_ReadWord(emu, (num << 2) + 2);
    uint32_t off = x86_ReadWord(emu, (num << 2) + 0);
    uint32_t vec = (seg == 0xffff) ? 0 : (off + (seg << 4));
    dbgint("int:%02x : %08x (%04x:%04x): ah_%02x, al_%02x, bl_%02x\n", num, vec, seg, off, emu->x86.R_AH, emu->x86.R_AL, emu->x86.R_BL);

    switch (num)
    {
        case 0x10:  // video interrupt
        case 0x42:  // video interrupt
        case 0x6d:  // vga internal interrupt
        {
            if ((vec == 0) && (emu->x86.R_AH == 0x12) && (emu->x86.R_BL == 0x32)) {
                if (emu->x86.R_AL == 0x00) {
                    // enable cpu access to video memory
                    x86_outb(emu, 0x3c2, x86_inb(emu, 0x3cc) | 0x02);
                } else if (emu->x86.R_AL == 1) {
                    // disable cpu access to video memory
                    x86_outb(emu, 0x3c2, x86_inb(emu, 0x3cc) & ~0x02);
                }
            }

            // vga write string
            if (emu->x86.R_AH == 0x13)
            {
                int num_chars = emu->x86.register_c.I16_reg.x_reg;
                int seg = emu->x86.register_es;
                int off = emu->x86.register_bp.I16_reg.x_reg;
                int str = (seg << 4) + off;
                for (int i = 0; i < num_chars; i++) {
                    x86printf("%c", * (char *)(emu->mem_base + str + i));
                }
            }

            if (vec == 0) {
                dbgint("uninitialised int vector\n");
            } else if (vec == 0xff065) {
                vec = 0;
            } /*else if (vec == 0xC56E2) { 
                vec = 0;
            }*/
        } break;

        case 0x15:  // memory
            /*dbgint("int15: AH:%02x AL:%02x DH=%02x DL=%02x\n", emu->x86.R_AH, emu->x86.R_AL, emu->x86.R_DH, emu->x86.R_DL);*/
            vec = 0;
            break;

        case 0x16: // keyboard services
            break;

        case 0x1a:  // rtc & pci
            vec = 0;
            break;

        case 0xe6:
            break;

        default:
            x86err("unhandled interrupt 0x%x\n", num);
            break;
    }

    if (vec)
    {
        //dbgint(": CS %04x IP %04x\n", emu->x86.R_CS, emu->x86.R_IP);
        //dbgint(": SS %04x SP %04x\n", emu->x86.R_SS, emu->x86.R_SP);
        uint32_t eflags;
        eflags = emu->x86.R_EFLG;
        x86_PushWord(emu, eflags);
        x86_PushWord(emu, emu->x86.R_CS);
        x86_PushWord(emu, emu->x86.R_IP);
        emu->x86.R_CS = x86_ReadWord(emu, (num << 2) + 2);
        emu->x86.R_IP = x86_ReadWord(emu, num << 2);
        //dbgint(": CS %04x IP %04x\n", emu->x86.R_CS, emu->x86.R_IP);
    }
}


//--------------------------------------------------------------------------------------------------
// Emulator
//--------------------------------------------------------------------------------------------------

static void x86emu_Run(struct X86EMU* emu) {
    x86dbg("x86_run: %04x %04x\n", emu->x86.R_CS, emu->x86.R_IP);
    emu->x86.R_SS = 0x0000;
    emu->x86.R_SP = 0xfffe;
    X86EMU_exec(emu);
}
static void x86emu_Call(struct X86EMU* emu, uint16_t seg, uint16_t off) {
    x86dbg("x86_call: %04x %04x\n", seg, off);
    emu->x86.R_SS = 0x0000;
    emu->x86.R_SP = 0xfffe;
    X86EMU_exec_call(emu, seg, off);
}

static void x86emu_Int(struct X86EMU* emu, uint8_t num) {
    x86dbg("x86_int: %02x\n", num);
    emu->x86.R_SS = 0x0000;
    emu->x86.R_SP = 0xfffe;
    X86EMU_exec_intr(emu, num);
}

void x86_Create(struct X86EMU* emu, void* ram_ptr, uint32_t ram_size, uint32_t isa_io, uint32_t isa_ram)
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

    emu->_X86EMU_run = x86emu_Run;
    emu->_X86EMU_call = x86emu_Call;
    emu->_X86EMU_int = x86emu_Int;


    memset(emu->mem_base, 0xf4, emu->mem_size); // fill everthing with hlt instructions
    memset(emu->mem_base, 0x00, 0x600);         // clear bios area
    emu->emu_wrw(emu, 0x410, 0x0010);           // equipment flag
    emu->emu_wrw(emu, 0x413, 0x0280);           // 640kb conventional memory

    for (int i = 0; i < 256; i++) {
        emu->_X86EMU_intrTab[i] = x86_int;
        iobuf[i] = 0;
#if DEBUG_X86PORTS
        iodbg[i] = 0;
#endif        
    }

    char *date = "01/01/99";
    for (int i = 0; date[i]; i++) {
        emu->emu_wrb(emu, 0xffff5 + i, date[i]);
    }
    emu->emu_wrb(emu, 0xffff7, '/');
    emu->emu_wrb(emu, 0xffffa, '/');
    emu->emu_wrb(emu, 0xffffe, 0xFC);   // model:    AT
    emu->emu_wrb(emu, 0xfffff, 0x00);   // submodel: ??

}

