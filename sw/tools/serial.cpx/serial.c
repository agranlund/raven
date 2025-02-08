/*********************************************************************/
/*                                                                   */
/*     Konfiguration der Seriellen Schnittstellen                    */
/*                                                                   */
/*                                                                   */
/*      CPX-Version 0.90                   vom 17. August 1997       */
/*                                                                   */
/*      zu kompilieren mit Pure C ohne String - Merging !!!          */
/*                                                                   */
/*********************************************************************/
/*                                                                   */
/* 2025, version 0.91                                                */
/*                                                                   */
/* - Fixed Rsconf UCR read bug (PureC)                               */
/* - Fixed port selection bug                                        */
/* - Support EmuTOS extended iorec                                   */
/* - Support Raven high-speed serial port                            */
/*                                                                   */
/*********************************************************************/
#ifdef __GNUC__
#include <gem.h>
#include <mintbind.h>
#else
#include <aes.h>
#include <vdi.h>
#endif
#undef BYTE
#define BYTE char
#ifndef WORD
# define WORD short
#endif
#ifndef LONG
# define LONG long
#endif
#ifndef UBYTE
# define UBYTE unsigned char
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "serial.h"
#include "transprt.h"

#ifdef __GNUC__
# define MAPTAB _MAPTAB
# define BCONMAP _BCONMAP
#endif


#define  NUM_ENTRIES  16

typedef short int16;

typedef struct
{
	char name[13];
	const char *gemdos;
	int16 bios_no;
	int16 flags;
	int16 bits;
	int16 parity;
	int16 stops;
	int16 flow;
	int16 dtr;
	int16 send;
	int16 recve;
	long speed;
	MAPTAB port_driver;
} SER_PORT;

/* EmuTOS extended iorec */
typedef struct {
    _IOREC in;
    _IOREC out;
    UBYTE baudrate;
    UBYTE flowctrl;
    UBYTE ucr;
    UBYTE datamask;
    UBYTE wr5;
} EXT_IOREC;


/*
 *  Saved port variables. Must be first variables !!!
 */
SER_PORT array[NUM_ENTRIES] = {
	{ "", NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } },
};

static int num_ports = 0;
static int which = 0;


#include "serial.rsh"

#include "cpxdata.h"


#ifndef FALSE
#define  FALSE       0
#define  TRUE        1
#endif

#define  NOWORK      ((void *) -1L)
#define  MESSAGE     -1
#define  CROS_CHK    (OS_CROSSED | OS_CHECKED)

#define  F_SERLAN     0x01
#define  F_RSVF       0x02
#define  F_STANDARD   0x04
#define  F_USELAN     0x08
#define  F_DTR        0x100
#define  F_IN_USE     0x800

#define  TF_STOPBITS    0x0003
#define  TF_1STOP       0x0001
#define  TF_15STOP      0x0002
#define  TF_2STOP       0x0003
#define  TF_CHARBITS    0x000c
#define  TF_8BIT        0x0000
#define  TF_7BIT        0x0004
#define  TF_6BIT        0x0008
#define  TF_5BIT        0x000c
#define  TF_FLOW        0x3000
#define  TF_TANDEM      0x1000
#define  TF_RTSCTS      0x2000
#define  TF_PARITY      0xc000
#define  TF_EVEN        0x4000
#define  TF_ODD         0x8000

#ifndef TIOCM_DTR
#define  TIOCM_DTR      0x0002
#define  TIOCM_RTS      0x0004
#define  TIOCM_CTS      0x0008
#endif

int errno;

#define MCH_ST          0
#define MCH_STE         1
#define MCH_TT          2
#define MCH_FALCON      3
#define MCH_RAVEN       7


static XCPB *parameters;
static OBJECT *box;

static SER_PORT backup[NUM_ENTRIES];
static _WORD vdi_handle;
static _WORD mode_flag;
static _WORD last_bios;
static _WORD glbl_par;
static _WORD alerts;
static _WORD const mode_box[] = { PROT_BOX, DRVR_BOX };

static _WORD act_speed;
static _WORD num_speed;
static long rsvf;
static long mint;
static long machine;
static long emutos;

/*
"With the RSVE hardware, the Baud-rates 115200/57600/38400 occupy the positions of 150/134/110 in any
case, so there is no point in reassigning them there. The reassignments are transparent for programs"
*/

static long const baudrate_mfp[] = { 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50 };
static long const baudrate_rvn[] = { 19200, 9600, 4800, 3600, 2400, 1000000, 750000, 500000, 921600, 460800, 230400, 115200, 57600, 38400, 28800, 31250 };

static char *port[NUM_ENTRIES];
static char portbuf[384];
static char *speed[32];
static char speedbuf[320];
static char ex_bios[168];
static const char *const mode[] = { "  Parameter ", "  Driver    " };
static const char *const bits[] = { "    8   ", "    7   ", "    6   ", "    5   " };
static const char *const parity[] = { "  None  ", "  Even  ", "  Odd   " };
static const char *const stops[] = { "    2   ", "    1.5 ", "    1   " };
static const char *const flow[] = { "  None     ", "  Xon/Xoff ", "  RTS/CTS  ", "  Both     " };


static long get_baudrate(SER_PORT* port, int idx) {
    if (((machine >> 16) == MCH_RAVEN) && (port->bios_no == 7))
        return baudrate_rvn[idx];
    return baudrate_mfp[idx];
}

static long get_baudidx(SER_PORT* port, long baud) {
    long best_diff = 0x7fffffffL;
    long baud_idx = 0;
    int count = 0;
    for (count = 0; count < 16; count++)
    {
        long b = get_baudrate(port, count);
        long diff = (b > baud) ? (b - baud) : (baud - b);
        if (diff < best_diff) {
            best_diff = diff;
            baud_idx = count;
        }
    }
    return baud_idx;
}

static void parse_tree(OBJECT *tree, _WORD object, _WORD mode)
{
	int work;

	if (mode == -1)
	{
		if (tree[object].ob_flags & OF_EDITABLE)
			tree[object].ob_type |= 0x100;
	} else
	{
		if (tree[object].ob_type & 0x100)
		{
			if (mode == 1)
				tree[object].ob_flags |= OF_EDITABLE;
			else
				tree[object].ob_flags &= ~OF_EDITABLE;
		}
	}

	if ((work = tree[object].ob_head) == -1)
		return;

	do
	{
		parse_tree(tree, work, mode);
		work = tree[work].ob_next;
	} while (work != object);
}


#if defined(__PUREC__)
static void *push_a2(void *, short) 0x2f0a;
static long pop_a2(long) 0x245f;
static long addq2_a7(long) 0x544f;
static long addq4_a7(long) 0x584f;
static void *push_d0(void *) 0x3f00;
static long jsr_a0(void *fun) 0x4e90;
#define callout(func, dev) pop_a2(addq2_a7(jsr_a0(push_d0(push_a2(func, dev)))))
#endif


static void __CDECL flush(_MAPTAB *entry, short dev)
{
	if (entry->Bconin && entry->Bconstat)
	{
#ifdef __GNUC__
		register void *callout __asm__("a0");
		register short ret __asm__("d0");
		
		for (;;)
		{
			callout = entry->Bconstat;
			__asm__ volatile(
				" move%.w %[dev],-(%%sp)\n"
				" jsr (%[callout])\n"
				" addq.w #2,%%sp\n"
			: "=d"(ret) /* outputs */
			: [dev]"g"(dev), [callout]"a"(callout) /* inputs  */
			: "d1", "d2", "a1", "a2", "cc" AND_MEMORY); /* clobbered regs */

			if (ret == 0)
				break;

			callout = entry->Bconin;
			__asm__ volatile(
				" move%.w %[dev],-(%%sp)\n"
				" jsr (%[callout])\n"
				" addq.w #2,%%sp\n"
			: "=d"(ret) /* outputs */
			: [dev]"g"(dev), [callout]"a"(callout) /* inputs  */
			: "d1", "d2", "a1", "a2", "cc" AND_MEMORY); /* clobbered regs */
		}
#endif
#ifdef __PUREC__
		/*
		 * Both calls are allowed to (and actual do) clobber d2 and a2.
		 * Pure-C does not use D2 in this function, and the cdecl
		 * modifier ensures that it is preserved when the function returns.
		 * A2 would be used however to load the function pointer,
		 * so we must preserve it.
		 */
		while ((short)callout(entry->Bconstat, dev))
		{
			callout(entry->Bconin, dev);
		}
#endif
	}
}


static long do_flush(void)
{
	SER_PORT *port;
	int handle;
	char file[32];
	long value;

	port = &array[which];
	strcpy(file, "U:\\DEV\\");
	strcat(file, port->gemdos);

	if ((handle = (int) Fopen(file, S_DENYNONE | FO_RW)) >= 0)
	{
		value = 3;
		Fcntl(handle, port->flags & F_RSVF ? value : (long) &value, TIOCFLUSH);
		Fclose(handle);
	} else
	{
		flush(&port->port_driver, port->bios_no);
	}

	return 0;
}


#ifdef __GNUC__
struct rsconf_params {
	short speed;
	short flowctrl;
	short ucr;
	short rsr;
	short tsr;
	short scr;
};
static unsigned long __CDECL my_Rsconf(struct rsconf_params params)
{
	return Rsconf(params.speed, params.flowctrl, params.ucr, params.rsr, params.tsr, params.scr);
}
static unsigned long __CDECL do_Rsconf(_MAPTAB *ent, short spd, short flw, short u, short r, short t, short s)
{
	register unsigned long retvalue __asm__("d0");
	register void *callout __asm__("a0") = ent->Rsconf;
	__asm__ volatile(
		" mov%.w	%[s],-(%%sp)\n"
		" mov%.w	%[t],-(%%sp)\n"
		" mov%.w	%[r],-(%%sp)\n"
		" mov%.w	%[u],-(%%sp)\n"
		" mov%.w	%[flw],-(%%sp)\n"
		" mov%.w	%[spd],-(%%sp)\n"
		" jsr (%[callout])\n"
		" lea	12(%%sp),%%sp"
	: "=r"(retvalue) /* outputs */
	: [callout]"a"(callout), [spd]"g"(spd), [flw]"g"(flw), [u]"r"(u), [r]"g"(r), [t]"g"(t), [s]"g"(s) /* inputs  */
	: __CLOBBER_RETURN("d0") "d1", "d2", "a1", "a2", "cc" /* clobbered regs */
	  AND_MEMORY
	);
	return retvalue;
}
#else
static unsigned long __CDECL my_Rsconf(short speed, short flowctrl, short ucr, short rsr, short tsr, short scr)
{
	return Rsconf(speed, flowctrl, ucr, rsr, tsr, scr);
}
static unsigned long __CDECL do_Rsconf(_MAPTAB *ent, short spd, short flw, short u, short r, short t, short s)
{
	return ent->Rsconf(spd, flw, u, r, t, s);
}
#endif


static long do_break(void)
{
	SER_PORT *port;
	int handle;
	int ok_flag = FALSE;
	char file[32];
	long value;

	port = &array[which];
	strcpy(file, "U:\\DEV\\");
	strcat(file, port->gemdos);

	if ((port->flags & F_RSVF) == 0 && (mint == 0 || (port->flags & F_STANDARD) == 0))
		handle = -1;
	else
		handle = (int) Fopen(file, S_DENYNONE | FO_RW);

	if (handle >= 0)
	{
		if ((short)Fcntl(handle, 0L, glbl_par ? TIOCSBRK : TIOCCBRK) == 0)
			ok_flag = TRUE;
		Fclose(handle);
	}

	if (ok_flag == FALSE)
	{
		if (port->port_driver.iorec)
		{
			value = do_Rsconf(&port->port_driver, -1, -1, -1, -1, -1, -1);
			if (glbl_par)
				value = ((value >> 8) & 0xff) | 8L;
			else
				value = ((value >> 8) & 0xff) & ~8L;
			do_Rsconf(&port->port_driver, -1, -1, -1, -1, (short)value, -1);
		}
	}

	return 0;
}


static void get_rsc_data(void)
{
	SER_PORT *port = &array[which];

	port->send = atoi(box[SEND_BUF].ob_spec.tedinfo->te_ptext);
	port->recve = atoi(box[RCVE_BUF].ob_spec.tedinfo->te_ptext);

	if (port->flags & F_SERLAN)
	{
		if (box[USE_LAN].ob_state & OS_SELECTED)
			port->flags |= F_USELAN;
		else
			port->flags &= ~F_USELAN;
	}

	port->dtr = (box[DTR].ob_state & OS_SELECTED) ? TIOCM_DTR : 0;
}


static void set_popup_string(const char *original, _WORD index, int draw_flag, int enable)
{
	char temp[32];
	const char *walk;
	_WORD bt_x, bt_y;
	int len;
	
	if (enable)
	{
		walk = original;
		while (*walk == ' ')
			walk++;
		strncpy(temp, walk, 31);
		temp[31] = '\0';
		for (len = (int)strlen(temp) - 1; len >= 0 && temp[len] == ' ';)
			len--;
		temp[++len] = '\0';
		box[index].ob_state &= ~OS_DISABLED;
		original = temp;
	} else
	{
		original = "Unknown";
		box[index].ob_state |= OS_DISABLED;
	}
	strcpy(box[index].ob_spec.free_string, original);

	if (draw_flag)
	{
		objc_offset(box, index, &bt_x, &bt_y);
		objc_draw(box, index, 1, bt_x, bt_y, box[index].ob_width, box[index].ob_height);
	}
}


static void set_rsc_data(void)
{
	SER_PORT *port;
	int handle;
	int index;
	char file[32];
	long spd;
	long last;
    long baud_sort[16];

	if (((port = &array[which])->flags & F_IN_USE) != 0)
		return;
	strcpy(file, "U:\\DEV\\");
	strcat(file, port->gemdos);

	if ((port->flags & F_RSVF) == 0)
		box[RSVF].ob_flags |= OF_HIDETREE;
	else
		box[RSVF].ob_flags &= ~OF_HIDETREE;

	set_popup_string(flow[port->flow >> 12], FLOW, FALSE, port->flow != -1);

	set_popup_string(stops[3 - port->stops], STOPS, FALSE, port->stops != -1);

	index = (port->parity & TF_EVEN) ? 1 : ((port->parity & TF_ODD) ? 2 : 0);
	set_popup_string(parity[index], PARITY, FALSE, port->parity != -1);

	set_popup_string(bits[port->bits >> 2], BITS, FALSE, port->bits != -1);

	sprintf(box[SEND_BUF].ob_spec.tedinfo->te_ptext, "%d", port->send);
	sprintf(box[RCVE_BUF].ob_spec.tedinfo->te_ptext, "%d", port->recve);

	if ((port->flags & F_SERLAN) == 0)
	{
		box[LAN_BOX].ob_flags |= OF_HIDETREE;
	} else
	{
		box[LAN_BOX].ob_flags &= ~OF_HIDETREE;
		box[USE_LAN].ob_state &= ~OS_SELECTED;
		box[USE_LAN].ob_state |= (port->flags & F_USELAN) ? OS_SELECTED : OS_NORMAL;
	}

	if ((port->flags & F_DTR) != 0)
	{
		if (port->dtr)
			box[DTR].ob_state = OS_SELECTED;
		else
			box[DTR].ob_state = OS_NORMAL;
	} else
	{
		box[DTR].ob_state = OS_DISABLED;
	}

	if ((port->flags & F_RSVF) == 0 && (mint == 0 || (port->flags & F_STANDARD) == 0))
		handle = -1;
	else
		handle = (int) Fopen(file, S_DENYNONE | FO_RW);

	box[FLUSH].ob_state = (handle < 0 && port->port_driver.iorec == NULL) ? OS_DISABLED : OS_NORMAL;

	index = 0;
	spd = 100000000L;

	if (port->speed != -1)
	{
		if (handle >= 0)
		{
			do
			{
				last = --spd;
				if ((short)Fcntl(handle, (long) &spd, TIOCIBAUD) == -32)
					break;
				speed[index] = &speedbuf[10 * index];
				sprintf(speed[index++], " %7ld ", spd);
			} while (last >= spd);
		}

		if (index == 0)
		{
            int i,j;
            long sorted[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            for (i=0; i<16; i++) {
                for (j=0; j<16; j++) {
                    long b = get_baudrate(port, j);
                    if ((b > sorted[i]) && ((i == 0) || (b < sorted[i-1]))) {
                        sorted[i] = b;
                    }
                }
            }

			while (index < 16)
			{
				sprintf(speed[index] = &speedbuf[10 * index], " %7ld ", sorted[index]);
				index++;
			}
			index++;
		}
		num_speed = index - 1;

		for (index = 0; index < num_speed; index++)
			if (atol(speed[index]) < port->speed)
				break;
		if (index > 0)
			--index;
	}

	act_speed = index;
	sprintf(file, "%ld", port->speed);
	set_popup_string(file, BAUDRATE, FALSE, port->speed != -1);

	if (handle >= 0)
		Fclose(handle);
}


static long get_port_data(void)
{
	SER_PORT *port;
	_WORD act_no;
	_WORD index;
	int handle;
	int ok_flag;
	_WORD flags;
	_WORD rate;
	long spd;
	long mapping[6];
	char file[32];

	strcpy(file, "U:\\DEV\\");

	if (Bconmap(0) != 0L)
	{
		act_no = -1;
	} else
	{
		act_no = (_WORD)Bconmap(-1);
		which = 10000;
	}

	for (index = 0, port = &array[0]; index < num_ports; index++, port++)
	{
		strcpy(&file[7], port->gemdos);
		if ((port->flags & F_RSVF) == 0 && (mint == 0 || (port->flags & F_STANDARD) == 0))
		{
			handle = -1;
		} else
		{
			if ((handle = (int) Fopen(file, S_DENYNONE | FO_RW)) < 0)
			{
				port->flags |= F_IN_USE;
				continue;
			} else
			{


				port->flags &= ~F_IN_USE;
			}
		}
		spd = -1;
		if (handle >= 0)
			Fcntl(handle, (long) &spd, TIOCIBAUD);
		if (spd == -1)
		{
			if (port->port_driver.iorec)
			{
				rate = (int) do_Rsconf(&port->port_driver, -2, -1, -1, -1, -1, -1);
                /*printf("get rate = %d\n", rate);*/
				if (0 <= rate && rate <= 15)
					spd = get_baudrate(port, rate);
			}
		}
		port->speed = spd;

		ok_flag = FALSE;

		if (handle >= 0)
		{
			if ((short)Fcntl(handle, (long) &flags, TIOCGFLAGS) == 0)
			{
				ok_flag = TRUE;
				port->flow = flags & TF_FLOW;
				port->parity = flags & TF_PARITY;
				port->stops = flags & TF_STOPBITS;
				port->bits = flags & TF_CHARBITS;
			}
		}
		if (!ok_flag)
		{
			if (port->port_driver.iorec)
			{
				flags = (_WORD) (do_Rsconf(&port->port_driver, -1, -1, -1, -1, -1, -1) >> 24);
				port->parity = (flags & 4) ? (flags & 2 ? TF_EVEN : TF_ODD) : 0x0000;
				port->stops = (flags >> 3) & TF_STOPBITS;
				port->bits = (flags >> 3) & TF_CHARBITS;
                port->flow = -1;
                if (emutos)
                {
                    port->flow = (((EXT_IOREC*)(port->port_driver.iorec))->flowctrl << 12) & TF_FLOW;
                }
			} else
			{
				port->parity = port->stops = port->bits = port->flow = -1;
			}
		}

		ok_flag = FALSE;

		if (handle >= 0)
		{
			if ((short)Fcntl(handle, (long) mapping, TIOCCTLMAP) == 0)
			{
				ok_flag = TRUE;
				port->flags |= (mapping[0] & TIOCM_DTR) ? F_DTR : 0;
			}
		}
		if (!ok_flag)
		{
			switch ((int)(machine >> 16))
			{
			case MCH_ST:
				if (port->bios_no == 6)
					port->flags |= F_DTR;
				break;
			case MCH_STE:
				if (port->bios_no == 6)
					port->flags |= F_DTR;
				if ((machine & 0xffffL) == 16)
				{
					if (port->bios_no == 7 || port->bios_no == 8)
						port->flags |= F_DTR;
				}
				break;
			case MCH_TT:
				if (port->bios_no == 6 || port->bios_no == 7 || port->bios_no == 9)
					port->flags |= F_DTR;
				break;
			case MCH_FALCON:
				if (port->bios_no == 7 || port->bios_no == 8)
					port->flags |= F_DTR;
				break;
			case MCH_RAVEN:
				if (port->bios_no == 6 || port->bios_no == 7)
					port->flags |= F_DTR;
				break;
			}
		}

		ok_flag = FALSE;

		if (port->flags & F_DTR)
		{
			spd = TIOCM_DTR;
			if (handle >= 0)
			{
				if ((short)Fcntl(handle, (long) &spd, TIOCCTLGET) == 0)
				{
					ok_flag = TRUE;
					spd &= TIOCM_DTR;
				}
			}
			if (ok_flag == FALSE)
			{
				/* Grab DTR via HW access */
				ok_flag = TRUE;
			}
			port->dtr = spd;
		}

		ok_flag = FALSE;
		mapping[0] = mapping[1] = mapping[2] = mapping[3] = -1L;

		if (handle >= 0)
		{
			if ((short)Fcntl(handle, (long) mapping, TIOCBUFFER) == 0)
				ok_flag = TRUE;
		}
		if (ok_flag == FALSE)
		{
			if (port->port_driver.iorec)
			{
				mapping[0] = (port->port_driver.iorec)->ibufsiz;
				mapping[3] = (port->port_driver.iorec + 1)->ibufsiz;
			}
		}
		port->recve = mapping[0];
		port->send = mapping[3];

		if (port->bios_no == act_no)
			which = index;

		if (handle >= 0)
			Fclose(handle);
	}

	if (Bconmap(0) == 0L)
	{
		if (which >= num_ports)
			which = 0;
	}

	return 0;
}


static void set_port(SER_PORT *port, SER_PORT *back, int alert)
{
	int handle;
	int count;
	_WORD flags;
	short flow;
	short ucr;
	int ok_flag;
	long longs[4];
	char file[32];
	char error[128];
	static char const tmplt[] = "[1][ |  Cannot set %s for  | |  port \"%s\" !][ Hmm ]";

	strcpy(file, "U:\\DEV\\");
	strcpy(&file[7], port->gemdos);

	if ((port->flags & F_IN_USE) != 0)
		return;

	if ((port->flags & F_RSVF) == 0 && (mint == 0 || (port->flags & F_STANDARD) == 0))
	{
		handle = -1;
	} else
	{
		if ((handle = (int) Fopen(file, S_DENYNONE | FO_RW)) < 0)
		{
			sprintf(error, "[1][ |  Cannot open port  | |  \"%s\" !][ Hmm ]", port->name);
			if (alert)
				form_alert(1, error);
			return;
		}
	}

	if (port->speed != back->speed)
	{
		ok_flag = FALSE;
		if (handle != -1)
		{
			longs[0] = longs[1] = port->speed;
			if ((short)Fcntl(handle, (long) &longs[0], TIOCIBAUD) == 0 &&
				(short)Fcntl(handle, (long) &longs[1], TIOCOBAUD) == 0)
				ok_flag = TRUE;
		}
		if (!ok_flag)
		{
			if (port->port_driver.iorec)
			{
                long best_diff = 0x7fffffffL;
                long baud_idx = 0;
				for (count = 0; count < 16; count++)
                {
                    long b = get_baudrate(port, count);
                    long diff = (b > port->speed) ? (b - port->speed) : (port->speed - b);
                    if (diff < best_diff) {
                        best_diff = diff;
                        baud_idx = count;
                    }
                }
                /*printf("set baud: %ld, %ld\n", port->speed, baud_idx);*/
				do_Rsconf(&port->port_driver, baud_idx, -1, -1, -1, -1, -1);
				ok_flag = TRUE;
			}
		}
		if (!ok_flag)
		{
			sprintf(error, tmplt, "baudrate", port->name);
			if (alert)
				form_alert(1, error);
		}
	}

	if (handle != -1)
		Fcntl(handle, (long) &flags, TIOCGFLAGS);
	
	ucr = 0;
	if (port->port_driver.iorec)
	{
		flow = -1;
		ucr = (do_Rsconf(&port->port_driver, -1, -1, -1, -1, -1, -1) >> 24);
	}

	if (port->bits != back->bits)
	{
		flags = (flags & ~TF_CHARBITS) | port->bits;
		ucr = (ucr & 0x9f) | (port->bits << 3);
	}
	if (port->parity != back->parity)
	{
		flags = (flags & ~TF_PARITY) | port->parity;
		ucr = (ucr & 0xfb) | (port->parity ? 4 : 0);
		ucr = (ucr & 0xfd) | ((port->parity & TF_EVEN) ? 2 : 0);
	}
	if (port->stops != back->stops)
	{
		flags = (flags & ~TF_STOPBITS) | port->stops;
		ucr = (ucr & 0xe7) | (port->stops << 3);
	}
	if (port->flow != back->flow)
	{
		flags = (flags & ~TF_FLOW) | port->flow;
		flow = port->flow >> 12;
	}

	if (handle != -1)
	{
		if ((short)Fcntl(handle, (long) &flags, TIOCSFLAGS) < 0)
		{
			sprintf(error, "[1][  Can\'t set protocol due to|  illegal combination of|  Bits/Parity"
					"/Stopbits/FlowCTRL   |  for \"%s\" !][ Hmm ]", port->name);
			if (alert)
				form_alert(1, error);
		}
	} else
	{
		if (port->port_driver.iorec)
			do_Rsconf(&port->port_driver, -1, flow, ucr, -1, -1, -1);
	}

	if (port->flags & F_DTR && port->dtr != back->dtr)
	{
		ok_flag = FALSE;
		if (handle != -1)
		{
			longs[0] = TIOCM_DTR;
			longs[1] = port->dtr;
			if ((short)Fcntl(handle, (long) longs, TIOCCTLSET) == 0)
				ok_flag = TRUE;
		}
		if (!ok_flag)
		{
			/* Set DTR via HW access */
			ok_flag = TRUE;
		}
		if (!ok_flag)
		{
			sprintf(error, tmplt, "DTR", port->name);
			if (alert)
				form_alert(1, error);
		}
	}

	ok_flag = 0;

	if (port->recve == back->recve)
	{
		ok_flag |= 1;
	} else
	{
		if (handle != -1)
		{
			longs[1] = longs[2] = longs[3] = -1L;
			longs[0] = port->recve;
			if ((short)Fcntl(handle, (long) longs, TIOCBUFFER) == 0)
				if (longs[0] != -1L)
					ok_flag |= 1;
		}
	}

	if (port->send == back->send)
	{
		ok_flag |= 2;
	} else
	{
		if (handle != -1)
		{
			longs[0] = longs[1] = longs[2] = -1L;
			longs[3] = port->send;
			if ((short)Fcntl(handle, (long) longs, TIOCBUFFER) == 0)
				if (longs[3] != -1L)
					ok_flag |= 2;
		}
	}

	if (ok_flag != 3)
	{
		sprintf(error, tmplt, "buffers", port->name);
		if (alert)
			form_alert(1, error);
	}

	if (handle >= 0)
		Fclose(handle);
}


static long set_port_data(void)
{
	SER_PORT *port;
	SER_PORT *back;
	SER_PORT serlan;
	SER_PORT *old;
	_WORD index;
	int handle;
	char file[32];

	old = NULL;

	for (index = 0, port = array, back = backup; index < num_ports; index++, port++, back++)
	{
		if (port->flags & F_SERLAN)
		{
			old = back;
			memcpy(&serlan, port, sizeof(SER_PORT));
		}
		set_port(port, back, alerts);
	}

	if (old)
	{
		if (strcmp(serlan.gemdos, "LAN") == 0)
			serlan.gemdos = "SERIAL2";
		else
			serlan.gemdos = "LAN";
		set_port(&serlan, back, FALSE);

		strcpy(file, "U:\\DEV\\");
		strcpy(&file[7], (serlan.flags & F_USELAN) ? "LAN" : "SERIAL2");

		if ((serlan.flags & F_RSVF) == 0 && mint == 0)
			handle = -1;
		else
			handle = (int) Fopen(file, S_DENYNONE | FO_RW);

		if (handle < 0)
			(serlan.flags & F_USELAN) ? Offgibit(0x7f) : Ongibit(0x80);
		else
			Fclose(handle);
	}

	if (Bconmap(0) == 0L)
		Bconmap(array[which].bios_no);

	return 0;
}


static void xscript(const char *gemdos, const char *generic, int bios, int flag, int force)
{
	char ***walk;

	for (walk = (char ***) port; *walk; walk++)
	{
		if (*walk != NOWORK)
		{
			if (strcmp(**walk, gemdos) == 0)
			{
				num_ports++;
				strcpy(array[num_ports].name, generic);
				array[num_ports].gemdos = **walk;
				array[num_ports].bios_no = *((unsigned char *) * walk + 6);
				array[num_ports].flags = (*((unsigned char *) * walk + 4) & 0xf0) | F_RSVF | F_STANDARD;
				if (flag)
					array[num_ports].flags |= F_SERLAN;
				array[num_ports].port_driver.iorec = NULL;
				array[num_ports].port_driver.Bconstat = NULL;
				*walk = NOWORK;
				return;
			}
		}
	}

	if (force)
	{
		strcpy(array[++num_ports].name, generic);
		array[num_ports].gemdos = gemdos;
		array[num_ports].bios_no = bios;
		array[num_ports].flags = (flag ? F_SERLAN : 0x00) | F_STANDARD;
		array[num_ports].port_driver.iorec = NULL;
	}
}


static void xerase(const char *gemdos)
{
	char ***walk;

	for (walk = (char ***) port; *walk; walk++)
	{
		if (*walk != NOWORK)
			if (strcmp(**walk, gemdos) == 0)
				*walk = NOWORK;
	}
}


/* no need to do any tricks for GCC here since the parameter is not used */
static short __CDECL my_Bconstat(short dummy)
{
	(void)dummy;
	return Bconstat(1);
}


/* no need to do any tricks for GCC here since the parameter is not used */
static long __CDECL my_Bconin(short dummy)
{
	(void)dummy;
	return Bconin(1);
}


static void bconmap_init(void)
{
	BCONMAP *bcon;
	MAPTAB *map_walk;
	int count;
	int16 bios;
	int16 flag;
	int port;
	char *gemdos;

	if (Bconmap(0) != 0)
	{
		for (port = 0; port < num_ports; port++)
		{
			if (array[port].bios_no == 6)
			{
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
				array[port].port_driver.Rsconf = (unsigned long __CDECL (*)(short, short, short, short, short, short))my_Rsconf;
#else
				array[port].port_driver.Rsconf = my_Rsconf;
#endif
				array[port].port_driver.iorec = Iorec(0);
				array[port].port_driver.Bconstat = my_Bconstat;
				array[port].port_driver.Bconin = my_Bconin;
			}
		}
		return;
	}

	map_walk = (bcon = (BCONMAP *) Bconmap(-2))->maptab;
	bios = 6;
	gemdos = &ex_bios[0];

	for (count = bcon->maptabsize; count > 0; --count)
	{
		flag = FALSE;
		for (port = 0; port < num_ports; port++)
		{
			if (array[port].bios_no == bios)
			{
				flag = TRUE;
				memcpy(&array[port].port_driver, map_walk, sizeof(MAPTAB));
			}
		}
		if (bios > last_bios && flag == FALSE)
		{
			array[num_ports].bios_no = bios;
			array[num_ports].flags = 0;
			sprintf(array[num_ports].name, "Bios %d", bios);
			sprintf(gemdos, "BIOS%d", bios);
			array[num_ports].gemdos = gemdos;
			gemdos += strlen(gemdos) + 1;
			memcpy(&array[num_ports++].port_driver, map_walk, sizeof(MAPTAB));
		}
		map_walk++;
		bios++;
	}
}


static void RSVF_init(void)
{
	unsigned char *walk;
	char ***work;
	int index;

	for (walk = (unsigned char *) rsvf, index = 0; *(long *) walk != 0 && index < NUM_ENTRIES; )
	{
		if ((walk[4] & 0x80) != 0)
		{
			port[index++] = (char *) walk;
			walk += 8;
		} else
		{
			walk = *(unsigned char **) walk;
		}
	}
	
	port[index] = NULL;
	num_ports = -1;

	switch ((int)(machine >> 16))
	{
	case MCH_ST:
		xscript("MODEM1", "Modem 1", 6, FALSE, TRUE);
		last_bios = 6;
		break;
	case MCH_STE:
		xscript("MODEM1", "Modem 1", 6, FALSE, TRUE);
		last_bios = 6;
		if ((machine & 0xffffL) == 16)
		{
			xscript("MODEM2", "Modem 2", 7, FALSE, TRUE);
			xscript("SERIAL2", "Ser.2/LAN", 8, TRUE, TRUE);
			last_bios = 8;
			xerase("LAN");
		}
		break;
	case MCH_TT:
		xscript("MODEM1", "Modem 1", 6, FALSE, TRUE);
		xscript("MODEM2", "Modem 2", 7, FALSE, TRUE);
		xscript("SERIAL1", "Serial 1", 8, FALSE, TRUE);
		xscript("SERIAL2", "Ser.2/LAN", 9, TRUE, TRUE);
		last_bios = 9;
		xerase("LAN");
		break;
	case MCH_FALCON:
		xscript("MODEM1", "Modem 1", 6, FALSE, FALSE);
		xscript("MODEM2", "Modem 2", 7, FALSE, TRUE);
		xscript("LAN", "LAN", 8, FALSE, TRUE);
		last_bios = 8;
		xerase("SERIAL2");
		break;
    case MCH_RAVEN:
		xscript("MODEM1", "Modem 1", 6, FALSE, TRUE);
		xscript("MODEM2", "Modem 2", 7, FALSE, TRUE);
        array[num_ports-1].flags &= ~F_STANDARD;
		last_bios = 7;
        break;
	default:
		xscript("MODEM1", "Modem 1", 6, FALSE, TRUE);
		last_bios = 6;
		break;
	}
	xscript("MIDI", "Midi", 0, FALSE, FALSE);

	for (work = (char ***) port; *work; work++)
	{
		if (*work != NOWORK)
		{
			strcpy(array[++num_ports].name, **work);
			array[num_ports].gemdos = **work;
			array[num_ports].bios_no = *((unsigned char *) * work + 6);
			array[num_ports].flags = (*((unsigned char *) * work + 4) & 0xf0) | F_RSVF;
			array[num_ports].port_driver.iorec = NULL;
			array[num_ports].port_driver.Bconstat = NULL;
		}
	}

	num_ports++;
	bconmap_init();
}


static void do_entry(char *gemdos, char *generic, int16 bios, int16 flag)
{
	strcpy(array[num_ports].name, generic);
	array[num_ports].gemdos = gemdos;
	array[num_ports].bios_no = bios;
	array[num_ports].flags = (flag ? F_SERLAN : 0x00) | F_STANDARD;
	array[num_ports].port_driver.iorec = NULL;
	array[num_ports].port_driver.Bconstat = NULL;

	num_ports++;
}


static void TOS_init(void)
{
	num_ports = 0;

	switch ((int)(machine >> 16))
	{
	case MCH_ST:
		do_entry("MODEM1", "Modem 1", 6, FALSE);
		last_bios = 6;
		break;
	case MCH_STE:
		do_entry("MODEM1", "Modem 1", 6, FALSE);
		last_bios = 6;
		if ((machine & 0xffffL) == 16)
		{
			do_entry("MODEM2", "Modem 2", 7, FALSE);
			do_entry("SERIAL2", "Ser.2/LAN", 8, TRUE);
			last_bios = 8;
		}
		break;
	case MCH_TT:
		do_entry("MODEM1", "Modem 1", 6, FALSE);
		do_entry("MODEM2", "Modem 2", 7, FALSE);
		do_entry("SERIAL1", "Serial 1", 8, FALSE);
		do_entry("SERIAL2", "Ser.2/LAN", 9, TRUE);
		last_bios = 9;
		break;
	case MCH_FALCON:
		do_entry("MODEM1", "Modem 1", 6, FALSE);
		do_entry("MODEM2", "Modem 2", 7, FALSE);
		do_entry("LAN", "LAN", 8, FALSE);
		last_bios = 8;
		break;
	case MCH_RAVEN:
		do_entry("MODEM1", "Modem 1", 6, FALSE);
		do_entry("MODEM2", "Modem 2", 7, FALSE);
        array[num_ports-1].flags &= ~F_STANDARD;
		last_bios = 7;
		break;
	default:
		do_entry("MODEM1", "Modem 1", 6, FALSE);
		last_bios = 6;
		break;
	}

	bconmap_init();
}


static void set_default_data(void)
{
	SER_PORT *port;
	SER_PORT *back;
	SER_PORT bald;
	int saved_count;
    int saved_which;
	_WORD index;
	int count;
	const char *name;

    int backup_num_ports = num_ports;
    int backup_which = which;

	memcpy(backup, array, NUM_ENTRIES * sizeof(SER_PORT));
	saved_count = num_ports;
    saved_which = which;

	if (rsvf)
		RSVF_init();
	else
		TOS_init();

	Supexec(get_port_data);

	for (index = 0, port = &array[0]; index < num_ports; index++, port++)
	{
		name = port->name;
        if (port->bios_no == saved_which)
            which = port->bios_no;

		for (count = 0, back = &backup[0]; count < saved_count; count++, back++)
		{
			if (strcmp(name, back->name) == 0)
			{
				port->bits = back->bits;
				port->parity = back->parity;
				port->stops = back->stops;
				port->flow = back->flow;
				port->dtr = back->dtr;
				port->send = back->send;
				port->recve = back->recve;
				port->speed = back->speed;
				port->flags = (port->flags & ~F_USELAN) | (back->flags & F_USELAN);
				count = saved_count;
			}
		}
	}

	bald.speed = bald.bits = bald.parity = bald.stops = bald.flow = -1;
	bald.dtr = bald.send = bald.recve = -1;

	for (index = 0, back = &backup[0]; index < num_ports; index++, back++)
		memcpy(back, &bald, sizeof(SER_PORT));

	alerts = FALSE;

    which = saved_which;
    if (saved_which >= 0 && saved_which < saved_count)
        which = saved_which;

	Supexec(set_port_data);
}

static long read_emutos_version(void)
{
    if (mint == 0)
    {
        unsigned long tosbase = *(unsigned long*)0x4f2;
        tosbase = *((unsigned long*)(tosbase + 8));
        if (*((ULONG*)(tosbase + 0x2c)) == 0x45544F53)  /* 'ETOS' */
            return 1L;
    }
    return 0L;
}

static void read_cookies(void)
{
    long dummy;
	if (parameters->getcookie(0x5F4D4348L, &machine) == 0) /* '_MCH' */
		machine = 0;
	if (parameters->getcookie(0x52535646L, &rsvf) == 0) /* 'RSVF' */
		rsvf = 0L;
#if 0
	if (parameters->getcookie(0x4D694E54L, &mint) == 0) /* 'MiNT' */
#endif    
		mint = 0L;
	if (parameters->getcookie(0x5241564EL, &dummy) != 0) /* 'RAVN' */
		machine = ((long)MCH_RAVEN) << 16;

    emutos = Supexec(read_emutos_version);
}


static void do_port_popup(void)
{
	int index;
	int length = 0;
	int len;

	for (index = 0; index < num_ports; index++)
	{
		if (length < (len = (int)strlen(array[index].name)))
			length = len;
	}

	for (index = 0; index < num_ports; index++)
	{
		port[index] = &portbuf[16 * index];
		strcpy(port[index], "  ");
		strcat(port[index], array[index].name);
		strcat(port[index], "              ");
		port[index][length + 3] = '\0';
	}
}


static _WORD __CDECL my_button_handler(PARMBLK *parameter)
{
	_WORD clip[4];
	_WORD pxy[4];
	_WORD pos_x, pos_y;
	_WORD radius;

	clip[0] = parameter->pb_xc;
	clip[2] = clip[0] + parameter->pb_wc - 1;
	clip[1] = parameter->pb_yc;
	clip[3] = clip[1] + parameter->pb_hc - 1;
	vs_clip(vdi_handle, 1, clip);

	radius = (parameter->pb_w + parameter->pb_h) / 6;
	pos_x = parameter->pb_x + parameter->pb_w / 2;
	pos_y = parameter->pb_y + parameter->pb_h / 2;

	vsf_interior(vdi_handle, FIS_HOLLOW);

	if (parameter->pb_tree[parameter->pb_obj].ob_flags & OF_RBUTTON)
	{
		v_circle(vdi_handle, pos_x, pos_y, radius);

		if (parameter->pb_currstate & OS_SELECTED)
		{
			vsf_interior(vdi_handle, FIS_SOLID);
			v_circle(vdi_handle, pos_x, pos_y, radius / 2);
		}
	} else
	{
		pxy[0] = pos_x - radius;
		pxy[2] = pos_x + radius;
		pxy[1] = pos_y - radius;
		pxy[3] = pos_y + radius;
		v_bar(vdi_handle, pxy);

		if (parameter->pb_currstate & OS_SELECTED)
		{
			pxy[0] += 2;
			pxy[1] += 2;
			pxy[2] -= 2;
			pxy[3] -= 2;
			v_pline(vdi_handle, 2, pxy);
			radius = pxy[1];
			pxy[1] = pxy[3];
			pxy[3] = radius;
			v_pline(vdi_handle, 2, pxy);
		}
	}

	vs_clip(vdi_handle, 0, clip);

	return parameter->pb_currstate & ~OS_SELECTED;
}

static USERBLK my_user_block = { my_button_handler, 0 };


static _WORD __CDECL cpx_call(GRECT *wind)
{
	GRECT rect;
	SER_PORT *act;
	_WORD button;
	int count;
	_WORD choice;
	_WORD event;
	_WORD dummy;
	int abort_flg = FALSE;
	_WORD msg_buff[8];
	_WORD work_in[11];
	_WORD work_out[57];

	graf_mouse(ARROW, NULL);

	for (count = 0; count < 10; count++)
		work_in[count] = 1;
	work_in[10] = 2;

	vdi_handle = parameters->handle;
	v_opnvwk(work_in, &vdi_handle, work_out);

	box[ROOT].ob_x = wind->g_x;
	box[ROOT].ob_y = wind->g_y;
	objc_draw(box, ROOT, MAX_DEPTH, wind->g_x, wind->g_y, wind->g_w, wind->g_h);

	act = &array[which];

	alerts = TRUE;

	do
	{
		button = (act->flags & F_RSVF) || (mint && (act->flags & F_STANDARD));
		button = (act->flags & F_IN_USE) == 0 ? ((mode_flag && button) ? SEND_BUF : 0) : 0;
		button = (*parameters->Xform_do) (box, button, msg_buff);
		if (button > 0)
		{
			objc_offset(box, button &= 0x7fff, &rect.g_x, &rect.g_y);
			rect.g_w = box[button].ob_width;
			rect.g_h = box[button].ob_height;
		}
		switch (button)
		{
		case SAVE:
			choice = FALSE;
			for (count = 0; count < num_ports; count++)
				if ((array[count].flags & F_IN_USE) != 0)
					choice = TRUE;
#if 0
			choice = form_alert(1, (choice) ?
								"[2][  Some ports are in use,|  their settings would be  |  lost ! Save anyway ?][Save| Cancel ]" :
								"[2][ |  Save port settings ?][Save| Cancel ]");
#else
            choice = 1;
#endif                                
			if (choice == 1)
			{
				if ((act->flags & F_IN_USE) == 0)
					get_rsc_data();
				Supexec(set_port_data);
				/* WTF: buffer for non-volatile data is 64 bytes only */
				count = NUM_ENTRIES * (int)sizeof(SER_PORT) + 2 * (int)sizeof(int);
				(*parameters->CPX_Save) (array, count);
			}
			break;
		case SET:
			if ((act->flags & F_IN_USE) == 0)
				get_rsc_data();
			Supexec(set_port_data);
		case CANCEL:
			abort_flg = TRUE;
			break;
		case MESSAGE:
			if (msg_buff[0] == WM_CLOSED)
			{
				if ((act->flags & F_IN_USE) == 0)
					get_rsc_data();
				Supexec(set_port_data);
				abort_flg = TRUE;
			}
			if (msg_buff[0] == AC_CLOSE)
				abort_flg = TRUE;
			break;
		case PORT:
			choice = (*parameters->Popup) ((const char *const *)port, num_ports, which, IBM, &rect, wind);
			if (choice >= 0 && choice != which)
			{
				if ((act->flags & F_IN_USE) == 0)
					get_rsc_data();
				set_popup_string(port[which = choice], PORT, TRUE, TRUE);
				set_rsc_data();
				act = &array[which];
				if ((act->flags & F_IN_USE) == 0)
				{
					box[mode_box[mode_flag]].ob_flags &= ~OF_HIDETREE;
					count = (act->flags & F_RSVF) || (mint && (act->flags & F_STANDARD));
					parse_tree(box, choice = mode_box[mode_flag], count);
					box[FAILED].ob_flags |= OF_HIDETREE;
				} else
				{
					box[mode_box[mode_flag]].ob_flags |= OF_HIDETREE;
					parse_tree(box, mode_box[mode_flag], 0);
					box[choice = FAILED].ob_flags &= ~OF_HIDETREE;
				}
				objc_draw(box, choice, MAX_DEPTH, wind->g_x, wind->g_y, wind->g_w, wind->g_h);
			}
			break;
		case MODE:
			choice = (*parameters->Popup) (mode, 2, mode_flag, IBM, &rect, wind);
			if (choice >= 0 && choice != mode_flag)
			{
				if ((act->flags & F_IN_USE) == 0)
				{
					box[mode_box[mode_flag]].ob_flags |= OF_HIDETREE;
					parse_tree(box, mode_box[mode_flag], 0);
				}
				set_popup_string(mode[mode_flag = choice], MODE, TRUE, TRUE);
				if ((act->flags & F_IN_USE) == 0)
				{
					box[choice = mode_box[mode_flag]].ob_flags &= ~OF_HIDETREE;
					count = (act->flags & F_RSVF) || (mint && (act->flags & F_STANDARD));
					parse_tree(box, choice, count);
					objc_draw(box, choice, MAX_DEPTH, wind->g_x, wind->g_y, wind->g_w, wind->g_h);
				}
			}
			break;
		case BAUDRATE:
			choice = (*parameters->Popup) ((const char *const *)speed, num_speed, act_speed, IBM, &rect, wind);
			if (choice >= 0)
			{
				set_popup_string(speed[act_speed = choice], BAUDRATE, TRUE, TRUE);
				act->speed = atol(speed[act_speed]);
			}
			break;
		case BITS:
			choice = (*parameters->Popup) (bits, 4, act->bits >> 2, IBM, &rect, wind);
			if (choice >= 0)
				set_popup_string(bits[choice], BITS, TRUE, TRUE), act->bits = choice << 2;
			break;
		case PARITY:
			count = (act->parity & TF_EVEN) ? 1 : ((act->parity & TF_ODD) ? 2 : 0);
			choice = (*parameters->Popup) (parity, 3, count, IBM, &rect, wind);
			if (choice >= 0)
			{
				set_popup_string(parity[choice], PARITY, TRUE, TRUE);
				act->parity = choice ? (choice == 1 ? TF_EVEN : TF_ODD) : 0x0000;
			}
			break;
		case STOPS:
			choice = (*parameters->Popup) (stops, 3, 3 - act->stops, IBM, &rect, wind);
			if (choice >= 0)
				set_popup_string(stops[choice], STOPS, TRUE, TRUE), act->stops = 3 - choice;
			break;
		case FLOW:
			choice = (*parameters->Popup) (flow, 4, act->flow >> 12, IBM, &rect, wind);
			if (choice >= 0)
				set_popup_string(flow[choice], FLOW, TRUE, TRUE), act->flow = choice << 12;
			break;
		case FLUSH:
			event = evnt_multi(MU_BUTTON | MU_M1,
				1, 1, 0,
				1, rect.g_x, rect.g_y, rect.g_w, rect.g_h,
				0, 0, 0, 0, 0,
				NULL,
				0,
				&dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
			if ((event & MU_M1) == 0)
			{
				if (form_alert(1, "[2][ |  Flush Buffers ?   ][Flush| Cancel ]") == 1)
					Supexec(do_flush);
			}
			break;
		case BREAK:
			glbl_par = TRUE;
			Supexec(do_break);
			evnt_multi(MU_BUTTON | MU_M1,
				1, 1, 0,
				1, rect.g_x, rect.g_y, rect.g_w, rect.g_h,
				0, 0, 0, 0, 0,
				NULL,
				0,
				&dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
			glbl_par = FALSE;
			Supexec(do_break);
			break;
		}
		if (button > 0)
		{
			box[button].ob_state &= ~OS_SELECTED;
			objc_draw(box, button, 3, wind->g_x, wind->g_y, wind->g_w, wind->g_h);
		}
	} while (!abort_flg);

	v_clsvwk(vdi_handle);

	return FALSE;
}


static CPXINFO fkts = {
	cpx_call, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};


CPXINFO *__CDECL cpx_init(XCPB *para)
{
	int count;

	parameters = para;
	read_cookies();

	if (parameters->booting)
	{
		set_default_data();
		return (CPXINFO *)1;
	}

	if (!parameters->SkipRshFix)
	{
		(*parameters->rsh_fix) (NUM_OBS, NUM_FRSTR, NUM_FRIMG, NUM_TREE,
								rs_object, rs_tedinfo, rs_strings, rs_iconblk,
								rs_bitblk, rs_frstr, rs_frimg, rs_trindex, rs_imdope);
		box = rs_object;

		for (count = 0; count < NUM_OBS; count++)
		{
			if (box[count].ob_type & 0x7f00)
				if ((box[count].ob_state & CROS_CHK) == CROS_CHK)
				{
					box[count].ob_type = G_USERDEF;
					box[count].ob_spec.userblk = &my_user_block;
					box[count].ob_state &= ~CROS_CHK;
				}
		}
		parse_tree(box, ROOT, -1);

		if (rsvf)
			RSVF_init();
		else
			TOS_init();

		do_port_popup();
		Supexec(get_port_data);
		memcpy(backup, array, NUM_ENTRIES * sizeof(SER_PORT));

		if (which >= num_ports)
			which = 0;
		set_popup_string(port[which], PORT, FALSE, TRUE);
		set_popup_string(mode[mode_flag = 0], MODE, FALSE, TRUE);

		if ((array[which].flags & F_IN_USE) != 0)
		{
			parse_tree(box, PROT_BOX, 0);
			box[PROT_BOX].ob_flags |= OF_HIDETREE;
		} else
		{
			box[FAILED].ob_flags |= OF_HIDETREE;
		}
		
		parse_tree(box, DRVR_BOX, 0);
		box[DRVR_BOX].ob_flags |= OF_HIDETREE;
		set_rsc_data();
	}

	return &fkts;
}
