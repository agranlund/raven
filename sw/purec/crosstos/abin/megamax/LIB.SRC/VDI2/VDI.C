extern vdi();
extern unsigned contrl[];
extern long _save[12];
extern int	_saveSP;

struct {
	long pblock, iioff, pioff, iooff, pooff;
} pblock;

i_ptr(addr)
long addr;
{
	contrl[7] = addr >> 16;
	contrl[8] = addr & 0xffff;
}

i_ptr2(addr)
long addr;
{
	contrl[9] = addr >> 16;
	contrl[10] = addr & 0xffff;
}

m_lptr2(addr)
long *addr;
{
	*addr = ((long)contrl[9] << 16) | contrl[10];
}

#ifndef LINT
#ifdef PCREL
asm {
	vdi:
		move.l	A4, A0
		adda.w	_saveSP(A4), A0
		addq.w	#4, _saveSP(A4)
		move.l	A1, _save(A0)
		move.l	A2, _save+16(A0)

		pea		contrl(A4)
		move.l	(A7)+, pblock.pblock(A4)
		pea		pblock(A4)
		move.l	(A7)+, D1
		move.l	#115, D0 
		trap	#2

		move.l	A4, A0
		subq.w	#4, _saveSP(A4)
		adda.w	_saveSP(A4), A0
		move.l	_save(A0), A1
		move.l	_save+16(A0), A2
		rts
}
#else
asm {
	vdi:
		lea		_save, A0
		adda.w	_saveSP, A0
		addq.w	#4, _saveSP
		move.l	A1, (A0)
		move.l	A2, 16(A0)

		pea		contrl
		move.l	(A7),32(A0)				;For LaserDB
		move.l	(A7)+, pblock.pblock
		pea		pblock
		move.l	(A7)+, D1
		move.l	#115, D0 
		trap	#2

		subq.w	#4, _saveSP
		lea		_save, A0
		adda.w	_saveSP, A0
		move.l	(A0), A1
		move.l	16(A0), A2
		rts
}
#endif
#endif
