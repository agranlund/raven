
/* LINTLIBRARY */

bcopy(b1, b2, len)
char *b1, *b2;
int len;
{
#ifdef	lint
	/* This is wrong */
	while (--len >= 0)
		*b2++ = *b1++;
#else
	asm {
		move.l	b1(A6), A0
		move.l 	b2(A6), A1
		move.w	len(A6), D1
		ext.l	D1

		cmpa.l  A0, A1      ; if (b1 < b2) then reverse
		bhi     reverse    

		move.w	D1, D0		; \
		and.w	#0x3, D0	;  > if len is not div. by 4 try for words
		bne.s	onlyw		; /

		or.l	b1(A6), D0	; Check addresses as well
		or.l	b2(A6), D0	;

		and.w	#0x1, D0	; if something is odd
		bne.s	bybyte		;     move by bytes
		bra.s	bylong		; else move by longs

onlyw:	or.l	b1(A6), D0	; Addresses...
		or.l	b2(A6), D0	;

		and.w	#0x1, D0	; If nothing is odd
		beq.s	byword		;     move by words

bybyte:
		bra		beloop
bloop:	move.b	(A0)+,(A1)+
beloop:	dbf		D1, bloop
		bra.s	exit

byword:	asr.w	#1, D1
		bra.s	weloop
wloop:	move.w	(A0)+,(A1)+
weloop:	dbf		D1, wloop
		bra.s	exit

bylong:	asr.w	#2, D1
		bra.s	leloop
lloop:	move.l	(A0)+,(A1)+
leloop:	dbf		D1, lloop
		bra.s	exit

reverse:
		adda.l	D1, A0		; set up to move
		adda.l	D1, A1		; in reverse

		move.w	D1, D0		; \
		and.w	#0x3, D0	;  > if len is not div. by 4 try for words
		bne.s	ronlyw		; /

		or.l	b1(A6), D0	; Check addresses as well
		or.l	b2(A6), D0	;

		and.w	#0x1, D0	; if something is odd
		bne.s	rbybyte		;     move by bytes
		bra.s	rbylong		; else move by longs

ronlyw:	or.l	b1(A6), D0	; Addresses...
		or.l	b2(A6), D0	;

		and.w	#0x1, D0	; If nothing is odd
		beq.s	rbyword		;     move by words

rbybyte:
		bra		rbeloop
rbloop:	move.b	-(A0),-(A1)
rbeloop:dbf		D1, rbloop
		bra.s	exit

rbyword:asr.w	#1, D1
		bra.s	rweloop
rwloop:	move.w	-(A0),-(A1)
rweloop:dbf		D1, rwloop
		bra.s	exit

rbylong:asr.w	#2, D1
		bra.s	rleloop
rlloop:	move.l	-(A0),-(A1)
rleloop:dbf		D1, rlloop

exit:
	}
#endif

}
