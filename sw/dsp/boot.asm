;--------------------------------------------------------------
;
; raven default dsp56303 bootcode
;
;--------------------------------------------------------------
	include "dsp56303.inc"
	org p:$0

;--------------------------------------------------------------
;
; vectors
;
;--------------------------------------------------------------
	jmp setup		; $00 reset
	ds 2			; $02 stack error
	ds 2			; $04 illegal instruction
	ds 2			; $06 debug request
	ds 2			; $08 trap
	ds 2			; $0a nmi
	ds 2			; $0c
	ds 2			; $0e
	ds 2			; $10 irqa
	ds 2			; $12 irqb
	ds 2			; $14 irqc
	ds 2			; $16 irqd
	ds 2			; $18 dma0
	ds 2			; $1a dma1
	ds 2			; $1c dma2
	ds 2			; $1e dma3
	ds 2			; $20 dma4
	ds 2			; $22 dma5
	ds 2			; $24 timer0 compare
	ds 2			; $26 timer0 overflow
	ds 2			; $28 timer1 compare
	ds 2			; $2a timer1 overflow
	ds 2			; $2c timer1 compare
	ds 2			; $2e timer1 overflow
	ds 2			; $30 essi0 rx data
	ds 2			; $32 essi0 rx data with exception status
	ds 2			; $34 essi0 rx last slot
	ds 2			; $36 essi0 tx data
	ds 2			; $38 essi0 tx data with exception status
	ds 2			; $3a essi0 tx last slot
	ds 2			; $3c
	ds 2			; $3e
	ds 2			; $40 essi1 rx data
	ds 2			; $42 essi1 rx data with exception status
	ds 2			; $44 essi1 rx last slot
	ds 2			; $46 essi1 tx data
	ds 2			; $48 essi1 tx data with exception status
	ds 2			; $4a essi1 tx last slot
	ds 2			; $4c
	ds 2			; $4e
	ds 2			; $50 sci rx data
	ds 2			; $52 sci rx data with exception status
	ds 2			; $54 sci tx data
	ds 2			; $56 sci idle line
	ds 2			; $58 sci timer
	ds 2			; $5a
	ds 2			; $5c
	ds 2			; $5e
	ds 2			; $60 host rx data full
	ds 2			; $62 host tx data empty
	ds 2			; $64 default host command
	ds 2			; $66
	ds 2			; $68
	ds 2			; $6a
	ds 2			; $6c
	ds 2			; $6e
	ds 2			; $70
	ds 2			; $72
	ds 2			; $74
	ds 2			; $76
	ds 2			; $78
	ds 2			; $7a
	ds 2			; $7c
	ds 2			; $7e

;--------------------------------------------------------------
;
; resident host vector functions
;
;--------------------------------------------------------------
;	ds ($80-*)
p56recv:
	bset	#M_HF2,x:<<M_HCR		; set HF2 to indicate ready
	jclr	#M_HRDF,x:<<M_HSR,*		; wait for header:space
	clr		a
	movep	x:<<M_HRX,a1
	move	#>3,x1
	cmp		x1,a
	jeq		<_done
	jclr	#M_HRDF,x:<<M_HSR,*		; wait for header:offset
	movep	x:<<M_HRX,r0
	jclr	#M_HRDF,x:<<M_HSR,*		; wait for header:length
	movep	x:<<M_HRX,r1
	move	#>2,x1
	cmp		x1,a
	jeq		<_ywrite
	move	#>1,x1
	cmp		x1,a
	jeq		<_xwrite
_pwrite:
	do		r1,_1
	jclr	#M_HRDF,x:<<M_HSR,*
	nop
	movep	x:<<M_HRX,p:(r0)+
_1:	jmp		<p56recv
_xwrite
	do		r1,_2
	jclr	#M_HRDF,x:<<M_HSR,*
	nop
	movep	x:<<M_HRX,x:(r0)+
_2:	jmp		<p56recv
_ywrite
	do		r1,_3
	jclr	#M_HRDF,x:<<M_HSR,*
	nop
	movep	x:<<M_HRX,y:(r0)+
_3:	jmp		<p56recv
_done:
	bclr	#M_HF2,x:<<M_HCR		; clear HF2 to indicate done
	jmp		<$0000
p56recv_end:


;--------------------------------------------------------------
;
; setup, overwritten by loaded programs
;
;--------------------------------------------------------------
	ds	(M_START-*)

setup:

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
	movep	#$012421,x:<<M_BCR		; 1 waitstate (12ns sram @ 90mhz / 112mhz)
	;movep	#$024842,x:<<M_BCR		; 2 waitstate (15ns sram @ 90mhz / 112mhz)
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
	bset	#M_HCIE,x:<<M_HCR		; HI08 command interrupts enabled
	bset	#M_I1,sr				; IPL2
	bclr	#M_I0,sr

	;--------------------------------------------------------------
	; p56 load
	;--------------------------------------------------------------
	jmp		p56recv

setup_end:
