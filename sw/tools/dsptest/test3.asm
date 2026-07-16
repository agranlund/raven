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

	;  0 PM[7-0]    : 15 : divide by 16
	; 11 PSR   		:  1 : divide by eight disabled
	; 12 DC[4-0]	:  1 : 2 words per frame
	; 19 WL[2-0]    :  4 : 32bits, first 24bits valid
	movep	#$20180f,x:<<M_CRA0

	; 13 MOD		:  1 : network mode
	; 12 SYN		:  1 : synchronous mode
	; 11 CKP		:  1 : clock polarity, falling edge
	; 10 FSP		:  1 : frame polarity, falling edge
	;  9 FSR		:  1 : frame sync one bit early
	;  7 FSL[1-0]	:  0 : rx=word, tx=word
	;  6 SHFD		:  0 : MSB shift order
	;  5 SCKD		:  1 : DSP is clock master
	;  4 SCD2		:  1 : SC2 is output
	movep	#$003e30,x:<<M_CRB0
	;movep	#$003a30,x:<<M_CRB0
	;movep	#$003e70,x:<<M_CRB0
	;movep	#$003a70,x:<<M_CRB0

	; prime transmitter
    movep	#0,x:<<M_TX00
    movep	#0,x:<<M_TX00

	; PC0 = gpio, PC1 = pgio
	; PC2 = SC02, PC3 = SCK0
	; PC4 = SRD0, PC5 = STD0
	movep	#%00111100,x:<<M_PCRC
	movep	#%00101100,x:<<M_PRRC

	; enable transmitter and receiver
	bset	#M_SSTE0,x:<<M_CRB0
	bset	#M_SSRE,x:<<M_CRB0

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
	jmp		loop2


	; sawtooth -> DAC output
	clr		a
	clr		b
loop1:
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       		; output left sample 
	nop
	nop
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   b1,x:<<M_TX00       		; output right sample
    add     #$004000,a
	sub		#$004000,b
    jmp     loop1


	; ADC input -> DAC output
loop2:

	clr		a
	jclr	#M_RDF,x:<<M_SSISR0,*
	movep	x:<<M_RX0,a1
	move	a1,x0

    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   x0,x:<<M_TX00

	clr		a
	jclr	#M_RDF,x:<<M_SSISR0,*
	movep	x:<<M_RX0,a1
	move	a1,x1

	;movep	x0,x:<<M_HTX
	;movep	x1,x:<<M_HTX

    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   x1,x:<<M_TX00

    jmp     <loop2




; the setting i think should be the correct one
; static noise is louder or same level as actual sound
; when no sound is playing, static noise is somewhat intermittent
; between nothing and noise.

;  0 PM[7-0]    : 15 : divide by 16
; 11 PSR   		:  1 : divide by eight disabled
; 12 DC[4-0]	:  1 : 2 words per frame
; 19 WL[2-0]    :  4 : 32bits, first 24bits valid
movep	#$20180f,x:<<M_CRA0

; 13 MOD		:  1 : network mode
; 12 SYN		:  1 : synchronous mode
; 11 CKP		:  1 : clock polarity, falling edge
; 10 FSP		:  1 : frame polarity, falling edge
;  9 FSR		:  1 : frame sync one bit early
;  7 FSL[1-0]	:  0 : rx=word, tx=word
;  6 SHFD		:  0 : MSB shift order
;  5 SCKD		:  1 : DSP is clock master
;  4 SCD2		:  1 : SC2 is output
movep	#$003e30,x:<<M_CRB0

; x0. all bits are moving around, top 4 bits flip together on/off
; x1. all bits are moving around

; ----------------------------------------------

; actual sound is heard louder than the static noise
; when no sound is playing then static noise is
; regular rather than intermittent on/off

;  0 PM[7-0]    : 15 : divide by 16
; 11 PSR   		:  1 : divide by eight disabled
; 12 DC[4-0]	:  1 : 2 words per frame
; 19 WL[2-0]    :  4 : 32bits, first 24bits valid
movep	#$20180f,x:<<M_CRA0

; 13 MOD		:  1 : network mode
; 12 SYN		:  1 : synchronous mode
; 11 CKP		:  1 : clock polarity, falling edge
; 10 FSP		:  0 : frame polarity, rising edge
;  9 FSR		:  1 : frame sync one bit early
;  7 FSL[1-0]	:  0 : rx=word, tx=word
;  6 SHFD		:  0 : MSB shift order
;  5 SCKD		:  1 : DSP is clock master
;  4 SCD2		:  1 : SC2 is output
movep	#$003a30,x:<<M_CRB0

; x0. all bits are moving around
; x1. all bits are moving around, top 4 bits flip together on/off


; ----------------------------------------------

; changing clock polarity makes similar result, but with top bit always 0
; everything shifted right one bit, likely.
; "stuttering" noise
movep	#$003630,x:<<M_CRB0

; clock polarity and frame polarity inverted
; silence.
; play sound (extreme noise covering the sound)
; volume fades down
; when at zero, getting many/most bits toggling on/off at same time,
; producing a very high pitch tone
; triggering repeats the above
movep	#$003230,x:<<M_CRB0


; ----------------------------------------------

; and always, regardless of settings:
;  out #0
;  out x1
;  produce identical output on left+right

;  out x1
;  out #0
;  produce silence on left+right

exampleloop2:
	jclr	#M_RDF,x:<<M_SSISR0,*
	movep	x:<<M_RX0,x0
	jclr	#M_RDF,x:<<M_SSISR0,*
	movep	x:<<M_RX0,x1

	brclr	#M_HTDE,x:<<M_HSR,*
	;movep	x0,x:<<M_HTX
	movep	x1,x:<<M_HTX

    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   x0,x:<<M_TX00
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   x1,x:<<M_TX00

    jmp     <exampleloop2

; and the output channel issue is identical when doing this test:

	clr		a
exampleloop3:
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       		; output left sample 
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       		; output right sample
    add     #$004000,a
    jmp     exampleloop3



; WL = 101
;	movep	#$28180f,x:<<M_CRA0
;	movep	#$003a30,x:<<M_CRB0
;lowest byte of x0 and x1 always 0

; WL = 101
;	movep	#$28180f,x:<<M_CRA0
;	movep	#$003e30,x:<<M_CRB0
;lowest byte of x0 and x1 always 0

; much lower volume level.
; first though that was weird, because the upper byte should have given that result,
; not the lower byte.

; but when testing the sawtooth player. i got the same very low volume.
; which indicates WL101 has that effect on the volume on the output side.

