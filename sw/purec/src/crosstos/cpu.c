/*
 * CPU abstraction layer
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "xbios.h"
#include "bios.h"
#include "gemdos.h"

#include <time.h>

static uint8_t ram[1024*1024*16];
static uint32_t pd;
#if defined(_MSC_VER) || defined(WIN32)
#include <windows.h>
LARGE_INTEGER performancecounter_frequency;
#endif

uint32_t cpu_init(int argc, char **argv, char **envp)
{
    pd = 0x00008000; /* System basepage */

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68030);
    m68k_pulse_reset();

    xbios_init(ram, sizeof(ram));
    bios_init(ram, sizeof(ram));
    gemdos_init(ram, sizeof(ram));

#if defined(_MSC_VER) || defined(WIN32)
    QueryPerformanceFrequency(&performancecounter_frequency);
#endif

    uint32_t p_lowtpa  = pd;                    /* Start address of the TPA            */
    uint32_t p_hitpa   = p_lowtpa + 256;        /* First byte after the end of the TPA */
    uint32_t p_tbase   = 0;                     /* Start address of the program code   */
    uint32_t p_tlen    = 0;                     /* Length of the program code          */
    uint32_t p_dbase   = 0;                     /* Start address of the DATA segment   */
    uint32_t p_dlen    = 0;                     /* Length of the DATA section          */
    uint32_t p_bbase   = 0;                     /* Start address of the BSS segment    */
    uint32_t p_blen    = 0;                     /* Length of the BSS section           */
    uint32_t p_dta     = p_lowtpa + 128;        /* Pointer to the default DTA          */
                                                /* Warning: Points first to the        */
                                                /* command line !                      */
    uint32_t p_parent  = 0;                     /* Pointer to the basepage of the      */
                                                /* calling processes                   */
    uint32_t p_resrvd0 = 0;                     /* Reserved                            */
    uint32_t p_env     = p_hitpa;               /* Address of the environment string   */

    strcpy((char*)&ram[p_hitpa], "PATH="); p_hitpa += 6;
    strcpy((char*)&ram[p_hitpa], "."); p_hitpa += 2;
    ram[p_hitpa++] = 0;
    ram[p_hitpa++] = 0;

    memset(&ram[pd + OFF_P_CMDLIN], 0, 128);

    WRITE_LONG(ram, pd + OFF_P_LOWTPA,  p_lowtpa);
    WRITE_LONG(ram, pd + OFF_P_HITPA,   p_hitpa);
    WRITE_LONG(ram, pd + OFF_P_TBASE,   p_tbase);
    WRITE_LONG(ram, pd + OFF_P_TLEN,    p_tlen);
    WRITE_LONG(ram, pd + OFF_P_DBASE,   p_dbase);
    WRITE_LONG(ram, pd + OFF_P_DLEN,    p_dlen);
    WRITE_LONG(ram, pd + OFF_P_BBASE,   p_bbase);
    WRITE_LONG(ram, pd + OFF_P_BLEN,    p_blen);
    WRITE_LONG(ram, pd + OFF_P_DTA,     p_dta);
    WRITE_LONG(ram, pd + OFF_P_PARENT,  p_parent);
    WRITE_LONG(ram, pd + OFF_P_RESRVD0, p_resrvd0);
    WRITE_LONG(ram, pd + OFF_P_ENV,     p_env);

    return pd;
}

bool cpu_load(uint8_t* bin, uint32_t tpasize, const char * p_cmdlin, uint32_t parent_pd)
{
    pd  = Malloc(tpasize);

    if(!pd)
    {
        return false;
    }

    uint16_t ph_branch      = READ_WORD(bin,  0);   /* Branch to start of the program  */
                                                    /* (must be 0x601a!)               */
    uint32_t ph_tlen        = READ_LONG(bin,  2);   /* Length of the TEXT segment      */
    uint32_t ph_dlen        = READ_LONG(bin,  6);   /* Length of the DATA segment      */
    uint32_t ph_blen        = READ_LONG(bin, 10);   /* Length of the BSS segment       */
    uint32_t ph_slen        = READ_LONG(bin, 14);   /* Length of the symbol table      */
    uint32_t ph_res1        = READ_LONG(bin, 18);   /* Reserved, should be 0;          */
                                                    /* Required by PureC               */
    uint32_t ph_prgflags    = READ_LONG(bin, 22);   /* Program flags                   */
    uint16_t ph_absflag     = READ_WORD(bin, 26);   /* 0 = Relocation info present     */

    (void)ph_prgflags;
    (void)ph_res1;

 /*   printf("ph_branch    = %08x\n", ph_branch);
    printf("ph_tlen      = %08x\n", ph_tlen);
    printf("ph_dlen      = %08x\n", ph_dlen);
    printf("ph_blen      = %08x\n", ph_blen);
    printf("ph_slen      = %08x\n", ph_slen);
    printf("ph_res1      = %08x\n", ph_res1);
    printf("ph_prgflags  = %08x\n", ph_prgflags);
    printf("ph_absflag   = %08x\n", ph_absflag); */
 
  
    if(ph_branch != 0x601a)
    {
        return false;
    }

    uint32_t p_lowtpa  = pd;                    /* Start address of the TPA            */
    uint32_t p_hitpa   = p_lowtpa + tpasize;    /* First byte after the end of the TPA */
    uint32_t p_tbase   = p_lowtpa + 256 + 2048;        /* Start address of the program code   */
    uint32_t p_tlen    = ph_tlen;               /* Length of the program code          */
    uint32_t p_dbase   = p_tbase + p_tlen;      /* Start address of the DATA segment   */
    uint32_t p_dlen    = ph_dlen;               /* Length of the DATA section          */
    uint32_t p_bbase   = p_dbase + p_dlen;      /* Start address of the BSS segment    */
    uint32_t p_blen    = ph_blen;               /* Length of the BSS section           */
    uint32_t p_dta     = p_lowtpa + 128;        /* Pointer to the default DTA          */
                                                /* Warning: Points first to the        */
                                                /* command line !                      */
    uint32_t p_parent  = parent_pd;             /* Pointer to the basepage of the      */
                                                /* calling processes                   */
    uint32_t p_resrvd0 = 0;                     /* Reserved                            */

    uint32_t p_env_p   = READ_LONG(ram, parent_pd + OFF_P_ENV); /* Address of the environment string   */
    uint32_t p_env     = p_lowtpa + 256; 

    WRITE_LONG(ram, pd + OFF_P_LOWTPA,  p_lowtpa);
    WRITE_LONG(ram, pd + OFF_P_HITPA,   p_hitpa);
    WRITE_LONG(ram, pd + OFF_P_TBASE,   p_tbase);
    WRITE_LONG(ram, pd + OFF_P_TLEN,    p_tlen);
    WRITE_LONG(ram, pd + OFF_P_DBASE,   p_dbase);
    WRITE_LONG(ram, pd + OFF_P_DLEN,    p_dlen);
    WRITE_LONG(ram, pd + OFF_P_BBASE,   p_bbase);
    WRITE_LONG(ram, pd + OFF_P_BLEN,    p_blen);
    WRITE_LONG(ram, pd + OFF_P_DTA,     p_dta);
    WRITE_LONG(ram, pd + OFF_P_PARENT,  p_parent);
    WRITE_LONG(ram, pd + OFF_P_RESRVD0, p_resrvd0);
    WRITE_LONG(ram, pd + OFF_P_ENV,     p_env);

    memset(&ram[pd + OFF_P_RESRVD1 ], 0, 80);
    memset(&ram[pd + OFF_P_CMDLIN], 0, 128);
    ram[pd + OFF_P_CMDLIN+0] = 127;

    char* envp = (char*)&ram[p_env_p];
    char* env = (char*)&ram[p_env];
    while (1)
    {
        *env++ = *envp++;
        if ((*(envp-1) == 0) && (*(envp+0) == 0))
        {
            break;
        }
    }

    if (p_cmdlin)
    {
        strcpy(env, "ARGV="); env += 6;
        strcpy(env, "binary.prg"); env += 11;   // todo: real application name
        const char* pin = p_cmdlin;
        while (*pin)
        {
            char c = *pin++;
            *env++ = (c == ' ') ? 0 : c;
        }

        int l_cmdlin = strlen(p_cmdlin);
        if (l_cmdlin < 126)
        {
            ram[pd + OFF_P_CMDLIN] = l_cmdlin;
            strcpy((char*)&ram[pd + OFF_P_CMDLIN + 1], p_cmdlin);
        }
    }
    *env++ = 0;


#if 0
    env = (char*)&ram[p_env];
    while (*env)
    {
        char* s = env;
        int   l = env ? strlen(env) : 0;
        if (s) {
            printf("env = [%s]\n", env);
        }
        env += (l + 1);
    }
#endif
    



    memcpy(&ram[p_tbase], bin + 28, p_tlen + p_dlen);

 /*   printf("p_lowtpa = %08x\n", p_lowtpa);
    printf("p_hitpa  = %08x\n", p_hitpa);
    printf("p_tbase  = %08x\n", p_tbase);
    printf("p_tlen   = %08x\n", p_tlen);
    printf("p_dbase  = %08x\n", p_dbase);
    printf("p_dlen   = %08x\n", p_dlen);
    printf("p_bbase  = %08x\n", p_bbase);
    printf("p_blen   = %08x\n", p_blen);
    printf("p_dta    = %08x\n", p_dta);
    printf("p_parent = %08x\n", p_parent);
    printf("p_resrvd0= %08x\n", p_resrvd0);
    printf("p_env    = %08x\n", p_env);*/
  
    if(!ph_absflag)
    {
        uint32_t fix = p_tbase;
        uint8_t* rel = bin + 28 + ph_tlen + ph_dlen + ph_slen;

        uint32_t off = READ_LONG(rel, 0);

        if(off)
        {
            fix += off;

        //    printf("Relocating... (%08x = %08x + %08x)\r\n", fix, READ_LONG(ram, fix), p_tbase);
            WRITE_LONG(ram, fix, READ_LONG(ram, fix) + p_tbase);
 
            rel += 4;

            while(*rel)
            {
                if(*rel == 1)
                {
                    fix += 254;
                }
                else
                {

                    fix += *rel;

              //      printf("Relocating... (%08x = %08x + %08x)\r\n", fix, READ_LONG(ram, fix), p_tbase);
                    WRITE_LONG(ram, fix, READ_LONG(ram, fix) + p_tbase);
                }

                rel++;
            }
        }

    }

    /* Launch binary */
    m68k_set_reg(M68K_REG_SP, p_hitpa - 8);
    WRITE_LONG(ram, p_hitpa - 4, pd);
    m68k_set_reg(M68K_REG_PC, p_tbase);
    m68k_set_reg(M68K_REG_A0, 0); /* launch as non-accessory */

    return true;
}


unsigned int cpu_read_byte(unsigned int address)
{
    if(address > sizeof(ram))
    {
        return 0;
    }

    return READ_BYTE(ram, address);
}

unsigned int cpu_read_word(unsigned int address)
{
    if(address > sizeof(ram))
    {
        return 0;
    }

    return READ_WORD(ram, address);
}

unsigned int cpu_read_long(unsigned int address)
{
    if(address > sizeof(ram))
    {
        return 0;
    }

    switch(address)
    {
        case 0x4ba:
        {
#if defined(_MSC_VER) || defined(WIN32)
            LARGE_INTEGER t;
            QueryPerformanceCounter(&t);
            uint32_t ticks200 = (uint32_t)(((double)t.QuadPart / (double)performancecounter_frequency.QuadPart)*200.0);
            WRITE_LONG(ram, address, ticks200);
#else
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

            uint32_t ticks200 = (ts.tv_nsec / (1000000000/200)) + ts.tv_sec * 200;

            WRITE_LONG(ram, address, ticks200);
#endif
        }
            break;

        default:
        {

        }
            break;
    }

    return READ_LONG(ram, address);
}

unsigned int cpu_read_word_dasm(unsigned int address)
{
    if(address > sizeof(ram))
    {
        return 0;
    }

    return READ_WORD(ram, address);
}

unsigned int cpu_read_long_dasm(unsigned int address)
{
    if(address > sizeof(ram))
    {
        return 0;
    }

    return READ_LONG(ram, address);
}

void cpu_write_byte(unsigned int address, unsigned int value)
{
    if(address > sizeof(ram))
    {
        return;
    }

    WRITE_BYTE(ram, address, value);
}

void cpu_write_word(unsigned int address, unsigned int value)
{
    if(address > sizeof(ram))
    {
        return;
    }

    WRITE_WORD(ram, address, value);
}

void cpu_write_long(unsigned int address, unsigned int value)
{
    if(address > sizeof(ram))
    {
        return;
    }

    WRITE_LONG(ram, address, value);
}

char* m68ki_disassemble_quick(unsigned int pc, unsigned int cpu_type);
void cpu_callback_instruction(void)
{
}

void cpu_callback_pc_changed(uint32_t pc)
{
    
}

void cpu_callback_trap(uint32_t vector)
{
    switch(vector)
    {
        case 0x10:  /* LINEA */
        {
            printf("IGNORING LineaA\n");
        }
            break;

        case 0x21:  /* GEMDOS */
        {
            uint16_t opcode = READ_WORD(ram, m68k_get_reg(NULL, M68K_REG_SP));

            m68k_set_reg(M68K_REG_D0, gemdos_dispatch(opcode, pd));
        }
            break;

        case 0x22:
        {
            printf("Ignoring AES/VDI call\n");
        }
            break;

        case 0x2d: /* BIOS */
        {
            uint16_t opcode = READ_WORD(ram, m68k_get_reg(NULL, M68K_REG_SP));

            m68k_set_reg(M68K_REG_D0, bios_dispatch(opcode,pd));
        }
          break;


        case 0x2e:  /* XBIOS */
        {
            uint16_t opcode = READ_WORD(ram, m68k_get_reg(NULL, M68K_REG_SP));

            m68k_set_reg(M68K_REG_D0, xbios_dispatch(opcode,pd));
        }
          break;

        default:
        {
            printf("trap not implemented (vec %08x)\n", vector);
            exit(0);
        }
            break;
    }
}
