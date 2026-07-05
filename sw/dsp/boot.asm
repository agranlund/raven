include "dsp56303.inc"
;--------------------------------------------------------------
;
; raven default dsp56303 bootcode
;
;--------------------------------------------------------------

	org p:$0

;--------------------------------------------------------------
; vectors
;--------------------------------------------------------------
	jmp start		; $00 reset
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
	ds 2			; $2c timer2 compare
	ds 2			; $2e timer2 overflow

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

	ds ($100-$80)


;--------------------------------------------------------------
; setup
;--------------------------------------------------------------
start:

	;--------------------------------------------------------------
	; clock
	;--------------------------------------------------------------
	movep	#$040003,x:<<M_PCTL		; PLL x4 for 90.3168 Mhz
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
    movep	#%00000000,x:<<M_PCRC

	; prescaler modulus 15
	; bypass fixed prescaler
	; divider 2
	; 32bit word length (24bit valid data)
    movep   #(15)|(1<<M_PSR)|(1<<12)|(4<<19),x:<<M_CRA0

	; M_MOD:  network mode
	; M_SYN:  synchronous mode
	; M_FSP:  frame sync timing for i2s
	; M_CKP:  clock out on falling edge, latch on rising edge
	; M_SCKD: dsp internal clock is source
	; M_SCD2: SC2 is output
    movep   #(1<<M_MOD)|(1<<M_SYN)|(1<<M_FSR)|(1<<M_CKP)|(1<<M_SCKD)|(1<<M_SCD2),x:<<M_CRB0

	; init portc
	; PC0 = gpio
	; PC1 = pgio
	; PC2 = SC02
	; PC3 = SCK0
	; PC4 = SRD0
	; PC5 = STD0
	movep	#%00111100,x:<<M_PCRC

	; prime tx
    movep	#0,x:<<M_TX00
    movep   #0,x:<<M_TX00

	; enable
    bset    #M_SSTE0,x:<<M_CRB0  ; enable transmitter
    bset    #M_SSRE,x:<<M_CRB0   ; enable receiver

	;--------------------------------------------------------------
	; essi1
	;--------------------------------------------------------------

	;--------------------------------------------------------------
	; external ram
	;--------------------------------------------------------------
	movep	#$000039,x:<<M_AAR0		; (sram /CE) X,Y,P
	movep	#$000015,x:<<M_AAR1		; (sram A15) X
	movep	#$000001,x:<<M_AAR2		; unused
	movep	#$000001,x:<<M_AAR3		; unused
	;movep	#$012421,x:<<M_BCR		; 1 waitstate (12ns sram @ 90mhz)
	movep	#$024842,x:<<M_BCR		; 2 waitstate (15ns sram @ 90mhz or 112mhz)
	;movep	#$036c63,x:<<M_BCR		; 3 waitstate
	bset	#M_APD,omr				; all AAR active

	;--------------------------------------------------------------
	; internal ram (p:3,x:2,y:2 - cache enabled)
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
; *** temp test code ***
;--------------------------------------------------------------

	bset	#3,x:M_HCR				; signal HF2
    move    #0,a                	; sawtooth counter
testloop:
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output left sample 
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output right sample
    add     #$008000,a
	bchg	#4,x:M_HCR				; toggle HF3
    jmp     testloop


;--------------------------------------------------------------
; P56 loader
;--------------------------------------------------------------
