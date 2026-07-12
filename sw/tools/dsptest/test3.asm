	include "dsp56303.inc"

	org p:$0
	jmp start		; $00 reset

	ds	(M_START-*)
start:

	;--------------------------------------------------------------
	; clock
	;--------------------------------------------------------------
;	movep	#$040001,x:<<M_PCTL		; PLL x2 for  45.1584 Mhz
;	movep	#$040002,x:<<M_PCTL		; PLL x3 for  67.7376 Mhz
	movep	#$040003,x:<<M_PCTL		; PLL x4 for  90.3168 Mhz
;	movep	#$040004,x:<<M_PCTL		; PLL x5 for 112.8960 Mhz
	rep		#$fff					; wait for it to settle
	nop
	rep		#$fff					; wait for it to settle
	nop
	rep		#$fff					; wait for it to settle
	nop
	rep		#$fff					; wait for it to settle
	nop

	;--------------------------------------------------------------
	; sci
	;--------------------------------------------------------------
	bset	#1,x:<<M_PCRE			; enable PORTE:TXD
	movep	#$00000b,x:<<M_SCCR		; near enough 115200 baud
	movep	#$000202,x:<<M_SCR		; TX enable, async 8N1

	;--------------------------------------------------------------
	; essi0
	;	i2s, 32bit slots (24bit data), 44.1khz, DAC+ADC
	;--------------------------------------------------------------

	; force all essi pins to gpio as per user manual
    movep	#0,x:<<M_PCRC

	; prescaler modulus 15
	; bypass fixed prescaler
	; divider 2
	; 32bit word length (24bit valid data)
    movep   #(15)|(1<<M_PSR)|(1<<12)|(4<<19),x:<<M_CRA0

	; M_MOD:  network mode
	; M_SYN:  synchronous mode
	; M_FSR:  realtive frame sync timing
	; M_FSP:  frame sync polarity
	; M_CKP:  clock out on falling edge, latch on rising edge
	; M_SCKD: dsp internal clock is source
	; M_SCD2: SC2 is output
    movep   #(0<<M_SHFD)|(1<<M_MOD)|(1<<M_SYN)|(1<<M_FSR)|(1<<M_FSP)|(1<<M_CKP)|(1<<M_SCKD)|(1<<M_SCD2),x:<<M_CRB0

	; PC0 = gpio, PC1 = pgio
	; PC2 = SC02, PC3 = SCK0
	; PC4 = SRD0, PC5 = STD0
	movep	#%00111100,x:<<M_PCRC

	; prime transmitter
    movep	#0,x:<<M_TX00
    movep	#0,x:<<M_TX00

	; enable
	clr		a
	movep	x:<<M_CRB0,a1
	or		#(1<<M_SSTE0)|(1<<M_SSRE),a
	movep	a1,x:<<M_CRB0


	;--------------------------------------------------------------
	; essi1
	;--------------------------------------------------------------

	;--------------------------------------------------------------
	; external ram
	;--------------------------------------------------------------
	movep	#$000039,x:<<M_AAR0		; (sram /CE) X,Y,P
	movep	#$000015,x:<<M_AAR1		; (sram A16) X
	movep	#$000001,x:<<M_AAR2		; unused
	movep	#$000001,x:<<M_AAR3		; unused
	;movep	#$012421,x:<<M_BCR		; 1 waitstate (12ns sram @ 90mhz)
	movep	#$024842,x:<<M_BCR		; 2 waitstate (15ns sram @ 90mhz or 112mhz)
	;movep	#$036c63,x:<<M_BCR		; 3 waitstate
	bset	#M_APD,omr				; all AAR active

	;--------------------------------------------------------------
	; internal ram (c:1, p:3, x:2, y:2)
	;--------------------------------------------------------------
	bclr	#M_CE,sr				; disable instruction cache
	bclr	#M_MS,omr				; p:4, x:2, y:2
	nop
	nop
	nop
	nop
	nop
	nop
	bset	#M_CE,sr				; enable instruction cache

	;--------------------------------------------------------------
	; host commands
	;--------------------------------------------------------------
	movep	#>3,x:<<M_IPRP			; HI08 IPL2
	bset	#M_HCIE,x:M_HCR			; HI08 command interrupts enabled
	bset	#M_I1,sr				; IPL2
	bclr	#M_I0,sr
	bset	#M_HF2,x:M_HCR			; set HF2 to indicate ready

test_start:
;	jmp		loop2


	; sawtooth -> DAC output
	clr		a
loop1:
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       		; output left sample 
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       		; output right sample
    add     #$004000,a
    jmp     loop1


	; ADC input -> DAC output
loop2:

	do		#2,_revb
	jclr	#M_RDF,x:<<M_SSISR0,*
	clr		a
	movep	x:<<M_RX0,a1
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00
_revb:

    jmp     <loop2
