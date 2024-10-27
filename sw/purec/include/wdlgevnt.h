#ifndef __WDLGEVNT_H__
#define __WDLGEVNT_H__

#ifndef __fixed_defined
#define __fixed_defined 1
typedef _LONG fixed;
#endif

/* Mouse rectangle for EVNT_multi() */
#if !defined(__MOBLK) && !defined(__TOS)
#define __MOBLK
typedef struct
{
	_WORD	m_out;
	_WORD	m_x;
	_WORD	m_y;
	_WORD	m_w;
	_WORD	m_h;
} MOBLK;
#endif

#ifndef __EVNT
#define __EVNT
typedef struct
{
    _WORD    mwhich;
    _WORD    mx;
    _WORD    my;
    _WORD    mbutton;
    _WORD    kstate;
    _WORD    key;
    _WORD    mclicks;
    _WORD    reserved[9];
    _WORD    msg[16];
} EVNT;
#endif

/*	Maus-Position/Status und Tastatur-Status (evnt_button, evnt_multi)	*/
#ifndef __EVNTDATA
#define __EVNTDATA
typedef struct
{
	_WORD x;
	_WORD y;
	_WORD bstate;
	_WORD kstate;
} EVNTDATA;
#endif

_WORD graf_mkstate_event(EVNTDATA *data);

void EVNT_multi(_WORD evtypes, _WORD nclicks, _WORD bmask, _WORD bstate, const MOBLK *m1, const MOBLK *m2, unsigned long ms, EVNT *event);

#endif /* __WDLGEVNT_H__ */
