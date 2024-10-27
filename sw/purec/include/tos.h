#if !defined( __TOS_H__ )
#define __TOS_H__


#define LINE _CCONLINE
#define DISKINFO _DISKINFO
#define DOSTIME _DOSTIME
#define DTA _DTA
#define d_reserved dta_buf
#define d_attrib   dta_attribute
#define d_time     dta_time
#define d_date     dta_date
#define d_length   dta_size
#define d_fname    dta_name
#define BASPAG     BASEPAGE
/* #define _BasPag    _base */
#define BPB        _BPB
#define IOREC      _IOREC
#define KBDVBASE   _KBDVECS
#define kb_midivec  midivec
#define kb_vkbderr  vkbderr
#define kb_vmiderr  vmiderr
#define kb_statvec  statvec
#define kb_mousevec mousevec
#define kb_clockvec clockvec
#define kb_joyvec   joyvec
#define kb_midisys  midisys
#define kb_kbdsys   ikbdsys
#define MOUSE       _PARAM
#define x_scale     xparam
#define y_scale     yparam
#define x_max       xmax
#define y_max       ymax
#define x_start     xstart
#define y_start     ystart
#define PBDEF       _PBDEF
#define KEYTAB      _KEYTAB
#define capslock	caps
#define MD          _MD
#define m_link      md_next
#define m_start     md_start
#define m_length    md_length
#define m_own       md_owner
#define MPB         _MPB
#define mp_mfl      mp_free
#define mp_mal      mp_used
#define MAPTAB      _MAPTAB
#define BCONMAP     _BCONMAP
#define SYSHDR      OSHEADER
#define os_start    reseth
#define os_base     os_beg
#define os_membot   os_end
#define os_gendat   os_date
#define os_palmode  os_conf
#define os_gendatg  os_dosdate
#define _root       p_root
#define kbshift     pkbshift
#define _run        p_run


#ifndef _COMPILER_H
#include <compiler.h>
#endif
#ifndef _MINT_OSTRUCT_H
#include <mint/ostruct.h>
#endif
#include <mint/osbind.h>
#include <mintbind.h>
#include <mint/basepage.h>

#define BASPAG BASEPAGE

typedef struct
{
		unsigned short	p_magic;	/* konstant 0x601a */
		unsigned long	p_tlen;		/* size of text segment */
		unsigned long	p_dlen;		/* size of data segment */
		unsigned long	p_blen;		/* size of bss segment */
		unsigned long	p_slen;		/* size of symbol table */
		unsigned long	p_reserved1;/* reserved */
		unsigned long	p_reserved2;/* reserved */
		unsigned short	p_reserved3;/* reserved */
} PRGHDR;

#endif /* __TOS_H__ */
