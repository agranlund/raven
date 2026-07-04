		ORG	L:$0009

x_l:		ds	1
x_r:		ds	1


	org p:$40
	
	tgt	y0,b		r1,r2
	
	sub	b,a		x:(r3+n3),x1
	sub	b,a

	move	l:<x_r,b
	
	
	tfr	y0,b
	tfr	y0,b		#1<<(24-12-1),x1
	
	and	x1,b
	and	x1,b		#1<<(16-1),x0			; b: $00:vv:$00
	
	move	#'dom',x0
	
	ifdef dsp56301
		add	x0,b
	endc
	
	movep	y:$0,X:<<$ffeb
	movep	#$1234,X:<<$ffeb
	movep	X:<<$ffeb,y:$0
	
	move			x:(r0)+,x0	y:(r4)+,y0
	move			x:(r0)-,x0	y:(r4)-,y0
		
	
	bset	#2,x:<<$ffe8					; host command enable

	
	mac	x0,y1,b	
	mac	x0,y0,a	
	mac	x1,y1,a
	mac	y0,y1,a

	
	movec	sp,ssh						

	movec	m0,x0
	movec	sr,x1						
	movec	omr,y0						
	movec	sp,y1						
	movec	ssh,a						
	movec	ssl,b						

	movec	m1,r2
	movec	sr,m5						
	movec	omr,n2						
	movec	sp,sr						
	movec	ssh,omr						
	movec	ssl,sp						
	movec	ssh,ssl						
	movec	ssl,la						
	movec	lc,la						
	movec	la,lc						

	
	tlt	x0,a						; yes, set new max
	
	
	add	x,b		x:(r4),a	b,y1
	add	y,a		x:(r4),a	b,y1

	add	x0,a		x:(r4),a	b,y1
	add	x1,b		x:(r4),a	b,y1
	add	y0,b		x:(r4),a	b,y1
	add	y1,a		x:(r4),a	b,y1

	
	move			b,x:(r1)-	y:(r4)-,b

	lua	(r5)-n5,r4					; n4: offset
	lua	(r5)+n5,n4					; n4: offset
	lua	(r1)-,n4					; n4: offset
	lua	(r2)+,r1					; n4: offset
		
	add	x0,b		x:(r0)+,x0	a,y:(r5)+
	add	x0,b		x:(r0)+,x0	b,y:(r5)+
	add	x0,b		x:(r0)+,x0	y0,y:(r5)+
	add	x0,b		x:(r0)+,x0	y1,y:(r5)+
	
	
	jclr	#8,X:<<0xfffff2,*
	jclr	#8,X:<2,*
		
short_loop:
			jmp	<short_loop
			
short_loop2:
			jmp	<short_loop2
;---------------------------------------------------------------------------

	move				x0,x:(r0)+	y:(r4)+,y1
	move				x0,x:(r0)+	y1,y:(r4)+
	move				x:(r0)+,x0	y1,y:(r4)+
	move				x:(r0)+,x0	y:(r4)+,y1
	
	move				L:(r0)+n0,a10
	move				L:(r0)+n0,ba
	move				ab,l:(r0)+n0	
	move				ba,l:(r0)+n0	
	
	move				b,x1		y1,y:(r0)+
	move				a,x0 		#10,y1
	move				b,x1		y:(r0)+,y1
	
	move				b,y:(r0)+
	move				y:(r0)+,a
	move				x:(r0)+,a	a,y1
	mpyr	x1,y0,a		x:(r0)+,x0	b,y0
	mpyr	x1,y0,a		x0,x:(r0)+	b,y0
	mpyr	x1,y0,a		#10,x0 		b,y0
	
	mpyr	x1,y0,a		b,x:(r0)+
	mpyr	x1,y0,a		x:(r0)+,a
	mpyr	x1,y0,a		x0,a
	mpyr	x1,y0,a		(r3)+n3
	
	move	#<$12,b	
	move	#$12345,b

	tfr	x0,a	x:(r3)+n3,x0 y:(r7)+n7,y0
	tfr	a,b	x:(r3)+n3,x0 y:(r7)+n7,y0
	
	teq	x0,a	r0,r1
	tne	b,a		r7,r3
	
	teq	x0,a
	tne	b,a
	
	rep		x0
		mpyr	x1,y0,a		x:(r3)+n3,x0 y:(r7)+n7,y0	
	
	rep		#10
		mpyr	x1,y0,a		x:(r3)+n3,x0 y:(r7)+n7,y0
	
	rep		x:(r0)
		mpyr	x1,y0,a		x:(r3)+n3,x0 y:(r7)+n7,y0
	
	norm	r0,a
	
	movep	y:<$fffff0,x0
	movep	y1,x:<$fffff0
	
	movep	#$123234,y:<$fffff0
	movep	#$123234,x:<$fffff0
	
	movep	p:$40,y:<$fffff0

	movep	y:<$fffff0,p:(r4)+
	
	movep	y:(r7)+,y:<<$fffff0
	movep	y:<$fffff0,y:(r7)+
	
	movem	x0,p:$10
	movem	p:$3,a2
	
	movec	#<$10,m1
	movec	#$1234,lc
	
	movec	x0,m0
	movec	lc,a1
	
	movec	x:$2,m0
	movec	lc,x:$2
	
	move	x0,b
	move	x:(r3)+n3,x0 y:(r7)+n7,y0
	move	x:(r2)+n2,b
	
	mpyr	x1,y0,a		x:(r3)+n3,x0 y:(r7)+n7,y0
	mpyr	-x0,y1,b	x:(r3)+n3,x0 y:(r7)+n7,y0

	
	mpy		x1,y0,a		b,x1
	mpy		-x0,y1,b	y0,x0
	
	macr		x1,y0,a		b,x1
	macr	-x0,y1,b	y0,x0
	
	mac		x1,y0,a		b,x1
	mac		-x0,y1,b	y0,x0
	
	lua		(r1)+n1,r1
	lua		(r1)+,n7
		
	jsr		(r0)
	jsr 	do_loop4
	jseq	do_loop4
	jsne	(r1+n1)
	
	jmp		(r0)
	jmp 	do_loop4
	jeq		do_loop4
	jne		(r1+n1)
	
	jclr	#8,r0,do_loop4
	jclr	#8,n0,do_loop4
	jset	#2,m7,do_loop4

	jclr	#8,b0,do_loop4
	jclr	#8,b1,do_loop4
	jset	#2,b2,do_loop4
	
	jclr	#8,X:<<0xffffc2,do_loop4
	jclr	#8,X:<<4,do_loop4
	jset	#1,Y:<<0xffffe0,do_loop4
		
	illegal
	
	ifdef dsp56301
		insert x0,a,b
		insert #34,b,a
		extract x0,a,b
		extractu x1,b,a
	endc
	
	ifdef dsp56301
	do	forever,loop5
	nop
loop5:
	endc
	
	do	b1,do_loop4
	enddo
do_loop4:
	
	do	x0,do_loop3
	enddo
do_loop3:
	
	do	#8,do_loop2
	enddo
do_loop2:
	
	do	x:<$30,do_loop1
	enddo
do_loop1:

	div x0,a
	div y1,b

	ifdef dsp56301
		inc	a
		dec	a
		debug
		cmpu	y0,b		
		cmpu	b,a
	endc

	cmpm	y0,b		x0,a
	cmpm	x0,a
	
	ifdef dsp56301
		clb	a,b
	endc
	
	not a 	x0,b
	asl a 	x0,b	
	asr a 	x0,b
	and x0,a	x:(r0),a0
	
	adc x,a 	y:(r1)+,b	
	sbc x,a 	a,x:(r0)+
	
	add x0,a	x:(r0),a0

	ifdef dsp56301
		add #$aabbcc,a		;56301 extension
		add #>63,a		;56301 extension
	endc
	
	eor x0,a	x:(r0),a0
	
	ifdef dsp56301
		eor #$aabbcc,a		;56301 extension
		eor #>63,a		;56301 extension
	endc
	
	or x0,a	x:(r0),a0
	
	ifdef dsp56301
		or #$aabbcc,a		;56301 extension
		or #>63,a			;56301 extension
	endc
	
	addl	a,b
	addr	b,a
	subl	a,b
	subr	b,a
	
	asl		a
	asl		b
	
	ifdef dsp56301
		asl		#5,b,a
		asl		x0,b,a
	endc
	
loopz:	asr		a
	asr		b	
	ifdef dsp56301
		asr		#5,a,b	
		asr		y0,b,a	
	endc
	
	bchg	#1,a
	bclr	#10,x1
	bset	#22,y0	
	bchg	#1,x:$2000
	bchg	#1,x:$FFFFC4
	bchg	#1,x:$FFFF81
	bchg	#1,x:<20

	jset	#1,x:<$30,loopz
	jclr	#1,y:(r4),loopz
	
	ifdef dsp56301
		jset	#22,x0,loopz
		jclr	#5,b2,loopz	
		brkeq	
		bseq	loopz
		bsr	loopz
		bsr	>loopz	
	endc
	
	jeq	loopz

	
	dc	0,1,2,3,4,5,6,7,8,10
	
	
	
	set loopz,1
