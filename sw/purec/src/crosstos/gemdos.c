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
#include "gemdos.h"
#include "tinyalloc.h"
#include "vt52.h"

#include <time.h>
#include <sys/time.h>

#if defined(WIN32)
#include <conio.h>
#else

#include <unistd.h>
#include <termios.h>
#include <assert.h>

#include "paths.h"

int getch(void)
{
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);

      if(c == 0x0a)
      {
        c = 0x0d;
      }

      return(c);
}
#endif

static file_t handles[256];

uint8_t ta_base[0x1000*16];

static uint8_t* rambase = NULL;

static int32_t writer_file(int16_t handle, int32_t count, void *buf)
{
    return fwrite(buf, 1, count, handles[handle].fd);
}


static int32_t writer_null(int16_t handle, int32_t count, void *buf)
{
    return count;
}

static int32_t writer_term(int16_t handle, int32_t count, void *buf)
{
    int32_t tmp = count;
    uint8_t* c  = buf;

    while(tmp--)
    {
        vt52_out(*c++, &handles[handle]);
    }

    return count;
}


static int32_t reader_file(int16_t handle, int32_t count, void *buf)
{
    return fread(buf, 1, count, handles[handle].fd);
}

static int32_t reader_null(int16_t handle, int32_t count, void *buf)
{
    return 0;
}

static int32_t reader_term(int16_t handle, int32_t count, void *buf)
{
    return fread(buf, 1, count, handles[handle].fd);
}

int32_t Fopen(char* fname, int16_t mode)
{
    int32_t retval = -1;
    int i;

 //   printf("Fopen %s...\n", fname);

    for(i = 0; i < sizeof(handles) / sizeof(handles[0]); i++)
    {
        if(!handles[i].fd)
        {
            char* f = path_open(fname, true);

            switch(mode & 3)
            {
                case 0: handles[i].fd = fopen(f, "rb"); break;
                case 1: handles[i].fd = fopen(f, "rb+"); break;
                case 2: handles[i].fd = fopen(f, "rb+"); break;
                default:
                {

                }
                    break;
            }

            if(handles[i].fd)
            {
                handles[i].fname = f;

                retval = i;
            }
            else
            {
                path_close(f);
            }
            
            break;
        }
    }

    return retval;
}

int16_t Fclose(int16_t handle)
{
    if(handle < 16)
    {
        if(handles[handle].term.state == not_term)
        {
            /* Protect against double Fclose() call
               (encountered in Lattice C invocations) */
            if (handles[handle].fd)
            {
                fclose(handles[handle].fd);
            }

            if(handles[handle].fname)
            {
     //           printf("Fclose(%d) (%s)\r\n", handle, handles[handle].fname);
                path_close(handles[handle].fname);
            }
 
            handles[handle].fd    = NULL;
            handles[handle].fname = NULL;
        }

        return GEMDOS_E_OK; /* E_OK */
    }

    return -1; /* error */
}

int16_t Fforce ( int16_t stdh, int16_t nonstdh )
{
 //   printf("Fforce() not implemented\r\n", stdh, nonstdh);
    return GEMDOS_E_OK;
}

int16_t Fcreate (char* fname, int16_t attr)
{
    int32_t retval = GEMDOS_E_EACCDN;

    char* f = path_open(fname, false);

  //  printf("Fcreate(\"%s\", %04x)\r\n", fname, attr);

    if(f)
    {
        FILE* fd = fopen(f,"wb+");

        if(fd)
        {
            fclose(fd);

            /* Steve: I think we need to pass the original name to Fopen()
               since that re-resolves the path name, using path_open()
               again.
               Passing "frame" would transform the already-transformed
               name, which can lead to malformed paths.
            */
            retval = Fopen(fname, 2);
        }

        path_close(f);
    }

    return retval;
}

int16_t Fattrib (char* fname, int16_t wflag, int16_t attr )
{
    char* f = path_open(fname, true);

    //  printf("Fattrib(\"%s\", %04x, %04x)\r\n", fname, wflag, attr);

    if(f)
    {

        path_close(f);
    }

    return GEMDOS_EFILNF;
}

int32_t Frename(char *fname, char *new_fname)
{
    int32_t retval = GEMDOS_E_OK;

    char* f1 = path_open(fname, true);
    char* f2 = path_open(new_fname, true);

  //  printf("Frename (%s, %s)\r\n", fname, new_fname);

    if(f1 && f2)
    {
        if(rename(f1, f2))
        {
            retval = GEMDOS_EFILNF;
        }

        path_close(f1);
        path_close(f2);

    }

    return retval; 
}

int16_t Fdelete (char *fname )
{
    int32_t retval = GEMDOS_E_OK;
 //   printf("Fdelete (%s)\r\n", fname);

    char* f = path_open(fname, true);

    if(f)
    {
        if(remove(f))
        {
            retval = GEMDOS_EFILNF;
        }

        path_close(f);
    }

    return retval; 
}

int32_t Fread(int16_t handle, int32_t count, void *buf)
{
    if(handle >= 0)
    {
        if(handles[handle].fd)
        {
            return handles[handle].reader(handle, count, buf);
        }
    }

    return -1;
}

int32_t Fwrite(int16_t handle, int32_t count, void *buf)
{
    if(handle >= 0)
    {
        if(handles[handle].fd)
        {
            return handles[handle].writer(handle, count, buf);
        }
    }

    return -1;
}

int32_t Fseek ( int32_t offset, int16_t handle, int16_t seekmode )
{
    int32_t r = -1;

    switch(seekmode)
    {
        case 0: r = fseek (handles[handle].fd, offset, SEEK_SET);  break;
        case 1: r = fseek (handles[handle].fd, offset, SEEK_CUR);  break;
        case 2: r = fseek (handles[handle].fd, offset, SEEK_END);  break;
        default:
        {

        }
            break;
    }

    if(!r)
    {
        r =ftell(handles[handle].fd);
    }

 //   printf("Fseek(%d, %d, %d) = %d\n", handle, offset, seekmode, r);

    return r;
}

uint32_t Mshrink(uint32_t block, uint32_t bytes)
{
  return 0;
}

void Mfree(uint32_t block)
{
    ta_free((void*)((intptr_t)block));
}

uint32_t Malloc(int32_t bytes)
{
    uint32_t retval = 0;

    if(bytes == -1)
    {
        retval = ta_biggest_block();
    }
    else
    {
        retval = (uint32_t)((intptr_t)ta_alloc(bytes));
    }

  //  printf("Malloc(%08x) = 0x%08x\r\n", bytes, retval);

    return retval;
}

uint32_t Mxalloc(int32_t bytes, int16_t mode)
{
    (void)mode;
    return Malloc(bytes);
}

uint32_t Pexec(int16_t mode, char* name, char* cmd, char* env)
{
    printf("Pexec (%d, %s, %s, %s) (not supported, ignored)\n", mode, name, cmd, env);

    return 0;
}


uint32_t gemdos_dispatch(uint16_t opcode, uint32_t pd)
{
    uint32_t retval = opcode;

    // printf("gemdos %02x\n", opcode);

    if (opcode >= 0x100) /* MiNT calls: not supported, but ignored */
        return retval;

    switch(opcode)
    {
        case 0x0000: /* Pterm0() */
        {
            exit(0);
        }
            break;

        case 0x0001: /* Cconin() */
        {
            retval = getchar(); /* ASCII in bits 0...7 */
        }
            break;

        case 0x0002: /* Cconout() */
        {
            int16_t  c  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = (vt52_out(c, &handles[1]) == EOF) ? -1 : GEMDOS_E_OK;
        }
            break;

        case 0x0004: /* int32_t Cauxout ( int16_t c );*/
        {
            int16_t  c  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = (vt52_out(c, &handles[2]) == EOF) ? -1 : GEMDOS_E_OK;
        }
            break;

        case 0x0006: /* int32_t Crawio ( int16_t w ); */
        {
            uint16_t w = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            if(w == 0x00ff) /* Read character from standard input */
            {
                /* ignore */
                retval = 0;
            }
            else
            {
                retval = (vt52_out(w, &handles[1]) == EOF) ? -1 : GEMDOS_E_OK;
            }

        }
            break;

        case 0x0007: /* int32_t Crawcin ( void ); */
        {
            do
            {
                retval = getch(); /* ASCII in bits 0...7 */
            } while(!retval);
        }
            break;

        case 0x0008: /* Cnecin() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x0009: /* Cconws() */
        {
            char* buf = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

            retval = GEMDOS_E_OK;

            while(*buf)
            {
                if(vt52_out(*buf++, &handles[1]) == EOF)
                {
                    break;
                    retval = -1;
                }
            }
        }
            break;

        case 0x000a: /* Cconrs() */
        {
            char* buf = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

          //  getline(buf,128,stdin);

            sprintf(buf, "%s", "abc");

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x000b: /* Cconis() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x0019: /* Dgetdrv() */
        {
            retval = 2; /* allways C: */
        }
            break;

        case 0x001A: /* void Fsetdta ( DTA *buf ); */
        {
            uint32_t dta = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            WRITE_LONG(rambase, pd + OFF_P_DTA, dta);
        }
            break;

        case 0x0020: /* int32_t Super ( void *stack ); */
        {
           uint32_t stack = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

           if(stack == 1)
           {
                retval = 0; /* user mode */
           }
           else
           {
                retval = 0x1234567;
           }
        }
            break;

        case 0x002a: /* Tgetdate() */
        {
            retval = 0; /* bogus */
        }
            break;

        case 0x002c: /* Tgettime() */
        {
            retval = 0; /* bogus */
        }
            break;

        case 0x002f: /* DTA *Fgetdta ( void ); */
        {
            retval = READ_LONG(rambase, pd + OFF_P_DTA);
        }
            break;

        case 0x0030: /* Sversion() */
        {
            retval = 30 << 8; /* GEMDOS 0.30 */
        }
            break;

        case 0x3c: /* Fcreate() */
        {
            char*    fname = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  attr  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            retval = Fcreate(fname, attr);
        }
            break;

        case 0x3d: /* Fopen() */
        {
            char*    fname = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  mode  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            retval = Fopen(fname, mode);
        }
            break;

        case 0x003e:  /* Fclose() */
        {
            int16_t  handle  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = Fclose(handle);
        }
            break;

        case 0x003f: /* Fread() */
        {
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
            char*    buf    = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            retval = Fread(handle, count, buf);
        }
            break;

        case 0x0040: /* Fwrite() */
        {
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
            char*    buf    = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            retval = Fwrite(handle, count, buf);
        }
            break;

        case 0x0043: /* int16_t Fattrib ( const int8_t *filename, int16_t wflag, int16_t attrib );*/ 
        {
            char*    fname  = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  wflag  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  attrib = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            retval = Fattrib(fname, wflag, attrib);
        }
            break;

        case 0x0041: /* int16_t Fdelete ( const int8_t *fname ) */
        {
            char*    fname  = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

            retval = Fdelete(fname);
        }
            break;

        case 0x0042: /* Fseek() */
        {
            int32_t  offset = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  mode   = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            retval = Fseek(offset, handle, mode);
        }
            break;

        case 0x0044: /* Mxalloc() */
        {
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  mode   = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            retval = Mxalloc(count, mode); 
        }
            break;

        case 0x0045: /* Fdup() */
        {
            printf("Ignoring Fdup() \n");
        }
            break;

        case 0x0046: /* int16_t Fforce ( int16_t stdh, int16_t nonstdh ); */
        {
            int16_t  stdh    = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  nonstdh = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);

            retval = Fforce(stdh,nonstdh);
        }
            break;

        case 0x0047: /* int16_t Dgetpath ( int8_t *path, int16_t driveno ); */
        {
            char*    buf    = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  drive  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            (void)drive;

            strcpy(buf, ".");

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x0048: /* Malloc() */
        {
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = Malloc(count); 
        }
            break;

        case 0x0049: /* Mfree() */
        {
            uint32_t  block  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            Mfree(block);

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x004a: /* Mshrink() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x004b: /* int32_t Pexec ( uint16_t mode, ... ) */
        {
            int16_t mode = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            char*   name = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4)];
            char*   cmd  = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];
            char*   env  = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 12)];

            retval = Pexec(mode, name, cmd, env);
        }
            break;

        case 0x004c: /* Pterm() */
        {
            int16_t status = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            exit(status);
        }   
            break;

        case 0x004e: /* int32_t Fsfirst ( const int8_t *filename, int16_t attr ) */
        {
            char*   fname = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t attr  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            char*   f = path_open(fname, true);

            if(f)
            {
                path_close(f);
            }

            printf("Fsfirst(\"%s\", 0x%04x) (%s)\r\n", fname, attr,f);

            retval = GEMDOS_EFILNF;
        }
            break;

        case 0x004f: /* int16_t Fsnext ( void ) */
        {
            printf("Fsnext()\r\n");

            retval = GEMDOS_EFILNF;
        }
            break;

        case 0x0056: /* int32_t Frename ( const int8_t *oldname, const int8_t *newname ) */
        {
            uint16_t reserved  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            char*    fname     = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4)];
            char*    new_fname = (char*)&rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            (void)reserved;

            retval = Frename(fname, new_fname);
        }
            break;

        case 0x0057: /* void Fdatime ( DOSTIME *timeptr, int16_t handle, int16_t wflag ); */
        {
            uint32_t tptr   = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  wflag  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            (void)handle;

            if(wflag)
            {
                /* Set values */
            }
            else
            {
                /* Obtain values */
                WRITE_WORD(rambase, tptr + 0, 0x0000); /* time */
                WRITE_WORD(rambase, tptr + 2, 0x0000); /* date */
            }
        }
            break;

        default:
        {
            printf("unknown GEMDOS call (0x%04x)\n", opcode);
            exit(0);
        }
            break;
    }

    return retval;
}


void gemdos_init(uint8_t* ram, uint32_t ramsize)
{
    int i;

    rambase = ram;

    ta_init();

    i = 0;
    handles[i].fd     = stdin;
    handles[i].fname  = "STDIN:";
    handles[i].term.state = normal;
    handles[i].reader = reader_term;
    handles[i].writer = writer_null;

    i++;
    handles[i].fd    = stdout;
    handles[i].fname = "STDOUT:";
    handles[i].term.state = normal;
    handles[i].reader = reader_null;
    handles[i].writer = writer_term;

    i++;
    handles[i].fd    = stderr;
    handles[i].fname = "STDAUX:";
    handles[i].term.state = normal;
    handles[i].reader = reader_null;
    handles[i].writer = writer_term;

    i++;
    handles[i].fd    = stderr;
    handles[i].fname = "STDPRN:";
    handles[i].term.state = normal;
    handles[i].reader = reader_null;
    handles[i].writer = writer_term;

    for(; i < sizeof(handles) / sizeof(handles[0]); i++)
    {
        handles[i].fd    = 0;
        handles[i].fname = NULL;
        handles[i].term.state = not_term;
        handles[i].reader = reader_file;
        handles[i].writer = writer_file;  
    }

}
