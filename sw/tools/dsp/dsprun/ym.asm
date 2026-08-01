
;-----------------------------------------------------------------------
;
; raven default audio program
; many todos..
;	
;	ym2149
;	 tone controls
;	 crossfeed for headphones
;	 reverb	
;
;	pcm
;	 falcon dmasound emulation (feed from hostport)
;
;-----------------------------------------------------------------------

	include "dsp56303.inc"

	org	p:$0
	jmp	sys_start

	org	p:$30
	jsr	int_essi_rx
	jsr	int_essi_rx

	org l:$0
saveab2		ds 1
saveab1		ds 1
saveab0		ds 1
savex		ds 1
savey		ds 1
savex_rx	ds 1

	org l:
adc_in		dc 0
dac_out		dc 0
sam			dc 0

	org x:
ONE		equ	$7FFFFF
ZERO	equ	$000000
ffff	dc	$FFFFFF
one		dc	ONE
zero	dc 	ZERO
adc_s	dc	0


;-----------------------------------------------------------------------
; System startup and main loop
;-----------------------------------------------------------------------
	org	p:M_START
sys_start:
	jsr		filter_init

	bclr	#M_S0L0,x:<<M_IPRP		; ESSI0 IPL 1
	bset	#M_S0L1,x:<<M_IPRP
	bset	#M_SSRIE,x:<<M_CRB0		; ESSI0 rx int enable

	movec	#0,sr					; IPL0
	jset	#M_RFS,x:<<M_SSISR0,*	; sync to left sample
	jclr	#M_RFS,x:<<M_SSISR0,*

sys_main:
	jclr	#1,x:<adc_s,sys_main	; wait for sample ready
	jsr		filter_main				; run filter
	move	l:<sam,x				; copy sam -> dac_out
	move	x,l:<dac_out
	bclr	#1,x:<adc_s				; flag filter done
	jmp		<sys_main


sys_saveregs:
	move	a1,x:<saveab1
	move	a0,x:<saveab0
	move	a2,x:<saveab2
	move	b1,y:<saveab1
	move	b0,y:<saveab0
	move	b2,y:<saveab2
	move	x,l:<savex
	move	y,l:<savey
	rts

sys_restregs:
	move	x:<saveab1,a1
	move	x:<saveab0,a0
	move	x:<saveab2,a2
	move	y:<saveab1,b1
	move	y:<saveab0,b0
	move	y:<saveab2,b2
	move	l:<savex,x
	move	l:<savey,y
	rts

;-----------------------------------------------------------------------
; ESSI0 receive interrupt
;-----------------------------------------------------------------------
int_essi_rx:
	jset	#M_RFS,x:<<M_SSISR0,_int_essi_rx_right
	movep	x:<<M_RX0,x:adc_in				; adc -> adc_in (left)
	movep	x:dac_out,x:<<M_TX00			; dac <- dac_out (left)
	rti
_int_essi_rx_right:
	movep	x:<<M_RX0,y:adc_in				; adc -> adc_in (right)
	movep	y:dac_out,x:<<M_TX00			; dac <- dac_out (right)
	jset	#1,x:<adc_s,_int_essi_rx_done	; filter busy?
	move	x,l:<savex_rx					; copy adc_in -> sam
	move	l:<adc_in,x
	move	x,l:<sam
	move	l:<savex_rx,x
	bset	#1,x:<adc_s						; let filter run
_int_essi_rx_done:	
	rti


;
; r0, n=1, m=-1 (reverb, tone, crossfeed)
; r1, n=1, m=-1 (reverb)
; r2, n=1, m=-1
; r3, n=1, m=-1 (biquad)
; r4, n=1, m=-1 (reverb)
; r5, n=1, m=-1 (reverb)
; r6, n=1, m=-1
; r7, n=2, m=3 (biquad)
;


;-----------------------------------------------------------------------
; biquad (half coeffs)
;-----------------------------------------------------------------------
; r7 = state, m7=3 (modulo 4), n7=2
; r3 = coeffs b0,b1,b2,a1,a2  (divided by two)
; a1 = sample in/out
;-----------------------------------------------------------------------
biquad_div2:
	move	a1,x1                                 ; x1 = x(n)
	move	             x:(r3)+,x0  y:(r7)+,y0   ; b0,  y0=x(n-1)         r7->1
	mpy		x0,x1,a      x:(r3)+,x0  y:(r7),y1    ; b1,  y1=x(n-2)         r7 =1
	mac		x0,y0,a      x:(r3)+,x0  y0,y:(r7)+   ; b2,  x(n-2)=old x(n-1) r7->2
	mac		x0,y1,a      x:(r3)+,x0  y:(r7)+,y0   ; a1,  y0=y(n-1)         r7->3
	mac-	x0,y0,a      x:(r3)+,x0  y:(r7),y1    ; a2,  y1=y(n-2)         r7 =3
	mac-	x0,y1,a                  y0,y:(r7)+   ;      y(n-2)=old y(n-1) r7->0
	asl		a
	rnd		a
	move	x1,y:(r7)+n7                          ;      x(n-1)=x(n)    	r7->2
	move	a1,y:(r7)+n7                          ;      y(n-1)=y(n)		r7->0
	rts

;-----------------------------------------------------------------------
; biquad (quarter coeffs)
;-----------------------------------------------------------------------
; r7 = state, m7=3 (modulo 4), n7=2
; r3 = coeffs b0,b1,b2,a1,a2  (divided by four)
; a1 = sample in/out
;-----------------------------------------------------------------------
biquad_div4:
	move	a1,x1                                 ; x1 = x(n)
	move	             x:(r3)+,x0  y:(r7)+,y0   ; b0,  y0=x(n-1)         r7->1
	mpy		x0,x1,a      x:(r3)+,x0  y:(r7),y1    ; b1,  y1=x(n-2)         r7 =1
	mac		x0,y0,a      x:(r3)+,x0  y0,y:(r7)+   ; b2,  x(n-2)=old x(n-1) r7->2
	mac		x0,y1,a      x:(r3)+,x0  y:(r7)+,y0   ; a1,  y0=y(n-1)         r7->3
	mac-	x0,y0,a      x:(r3)+,x0  y:(r7),y1    ; a2,  y1=y(n-2)         r7 =3
	mac-	x0,y1,a                  y0,y:(r7)+   ;      y(n-2)=old y(n-1) r7->0
	asl		a
	asl		a
	rnd		a
	move	x1,y:(r7)+n7                          ;      x(n-1)=x(n)    	r7->2
	move	a1,y:(r7)+n7                          ;      y(n-1)=y(n)		r7->0
	rts


;-----------------------------------------------------------------------
; filters init
;-----------------------------------------------------------------------
filter_init:
	move	#1,n0
	movec	x:<ffff,m0
	move	#1,n1
	movec	x:<ffff,m1
	move	#1,n2
	movec	x:<ffff,m2
	move	#1,n3
	movec	x:<ffff,m3
	move	#1,n4
	movec	x:<ffff,m4
	move	#1,n5
	movec	x:<ffff,m5
	move	#1,n6
	movec	x:<ffff,m6
	move	#2,n7			; n7 stepsize for biquad state
	movec	#3,m7			; m7 modulo for biquad state

	jsr		fx_reverb_init
	rts


;-----------------------------------------------------------------------
; filters process
;-----------------------------------------------------------------------
filter_main:
	jsr		fx_tone
	jsr		fx_crossfeed
	jsr		fx_reverb
	rts



;-----------------------------------------------------------------------
;
; fx_tone
; bass and treble tone controls
;
;-----------------------------------------------------------------------
fx_tone_init:
	move	#0,x0					; clear state
	move	#tone_lostatel,r0
	rep		#16
	move	x0,y:(r0)+

	move	x:tone_bass,x0			; bass coeffs
	move    #tone_loco_table,r0
	move	x0,n0
	nop
	move	x:(r0+n0),y1
	move	y1,x:tone_loco

	move	x:tone_treb,x1			; treble coeffs
	move    #tone_hico_table,r0
	move	x1,n0
	nop
	move	x:(r0+n0),y1
	move	y1,x:tone_hico

	clr		a						; attenuation
	add		x0,a
	add		x1,a
	move	#tone_att_table,r0
	move	a,n0
	nop
	move	x:(r0+n0),x0
	move	x0,x:tone_atten

	move	#1,n0
	bset	#1,x:tone_flag			; flag as updated
	rts

fx_tone:
	move	x:tone_flag,y1			; need recalc?
	jsclr	#1,y1,fx_tone_init

	move	x:<sam,x0				; left
	move	x:tone_atten,y0			; attenuate
	mpy		x0,y0,a
	move	x:tone_loco,r3			; low shelf
	move	#tone_lostatel, r7
	jsr		biquad_div2
	move	x:tone_hico,r3			; high shelf
	move	#tone_histatel, r7
	jsr		biquad_div4
	move	a,x:<sam

	move	y:<sam,x0				; right
	move	x:tone_atten,y0			; attenuate
	mpy		x0,y0,a
	move	x:tone_loco,r3			; low shelf
	move	#tone_lostater, r7
	jsr		biquad_div2
	move	x:tone_hico,r3			; high shelf
	move	#tone_histater, r7
	jsr		biquad_div4
	move	a,y:<sam
	rts


;-----------------------------------------------------------------------
;
; fx_crossfeed
; crossfeed effect for headhpones
;
;-----------------------------------------------------------------------
fx_crossfeed:
	move	x:cf_gain,y0	; direct gain
	move	x:cf_gaini,y1	; crossfeed gain
	movec	x:cf_delay,m0	; m0 = delay modulo
	move	x:cf_ptr_r,r0	; r0 = delay buffer right

	move	x:<sam,x0		; x0 = current_left
	move	x:(r0),x1		; x1 = delayed_right
	mpy		x0,y0,a			; a  = (0.7 * current_left)
	mac		x1,y1,a			; a += (0.3 * delayed_right)

	move	x:cf_ptr_l,r0	; r0 = delay buffer left
	move	a,x:<sam		; write left

	move	x:(r0),x1		; x1 = delayed_left
	move	x0,x:(r0)+		; update left delay
	move	r0,x:cf_ptr_l	; update left delay ptr

	move	x:cf_ptr_r,r0	; r0 = delay buffer right
	move	y:<sam,x0		; x0 = current_right
	mpy		x0,y0,a			; a  = (0.7 * current_right)
	mac		x1,y1,a			; a += (0.3 * delayed_left)
	move	x0,x:(r0)+		; update right delay
	move	r0,x:cf_ptr_r	; update right delay ptr
	move	a,y:<sam		; write right
	movec	x:<ffff,m0
	rts


	org y:
	align 4
tone_lostatel: 	dc 0,0,0,0
tone_histatel: 	dc 0,0,0,0
tone_lostater: 	dc 0,0,0,0
tone_histater: 	dc 0,0,0,0
tone_dummy40:	dc ONE,ONE,ONE,ONE

	org x:
	align 32
cf_buffer_l	ds	32
cf_buffer_r ds	32
cf_enable	dc	1
cf_gain		dc	0.6
cf_gaini	dc  0.4
cf_delay	dc	21
cf_ptr_l	dc	cf_buffer_l
cf_ptr_r	dc	cf_buffer_r

tone_atten	dc	$7fffff
tone_bass	dc 	18	; 12
tone_treb	dc 	13	; 12
tone_flag	dc  0
tone_loco	dc  tone_loco_data+(5 * 12)
tone_hico	dc  tone_hico_data+(5 * 12)

tone_att_table:
    dc $7FFFFF     ; -24 dB
    dc $7FFFFF     ; -23 dB
    dc $7FFFFF     ; -22 dB
    dc $7FFFFF     ; -21 dB
    dc $7FFFFF     ; -20 dB
    dc $7FFFFF     ; -19 dB
    dc $7FFFFF     ; -18 dB
    dc $7FFFFF     ; -17 dB
    dc $7FFFFF     ; -16 dB
    dc $7FFFFF     ; -15 dB
    dc $7FFFFF     ; -14 dB
    dc $7FFFFF     ; -13 dB
    dc $7FFFFF     ; -12 dB
    dc $7FFFFF     ; -11 dB
    dc $7FFFFF     ; -10 dB
    dc $7FFFFF     ; -9 dB
    dc $7FFFFF     ; -8 dB
    dc $7FFFFF     ; -7 dB
    dc $7FFFFF     ; -6 dB
    dc $7FFFFF     ; -5 dB
    dc $7FFFFF     ; -4 dB
    dc $7FFFFF     ; -3 dB
    dc $7FFFFF     ; -2 dB
    dc $7FFFFF     ; -1 dB
    dc $7FFFFF     ;  0 dB
    dc $7FFFFF     ; +1 dB
    dc $7FFFFF     ; +2 dB
    dc $7FFFFF     ; +3 dB
    dc $7FFFFF     ; +4 dB
    dc $7FFFFF     ; +5 dB
    dc $7FFFFF     ; +6 dB
    dc 0.891251    ; +7 dB
    dc 0.794328    ; +8 dB
    dc 0.707946    ; +9 dB
    dc 0.630957    ; +10 dB
    dc 0.562341    ; +11 dB
    dc 0.501187    ; +12 dB
    dc 0.446684    ; +13 dB
    dc 0.398107    ; +14 dB
    dc 0.354813    ; +15 dB
    dc 0.316228    ; +16 dB
    dc 0.281838    ; +17 dB
    dc 0.251189    ; +18 dB
    dc 0.223872    ; +19 dB
    dc 0.199526    ; +20 dB
    dc 0.177828    ; +21 dB
    dc 0.158489    ; +22 dB
    dc 0.141254    ; +23 dB
    dc 0.125893    ; +24 dB


tone_loco_table:
	dc tone_loco_data+(5 * 0)
	dc tone_loco_data+(5 * 1)
	dc tone_loco_data+(5 * 2)
	dc tone_loco_data+(5 * 3)
	dc tone_loco_data+(5 * 4)
	dc tone_loco_data+(5 * 5)
	dc tone_loco_data+(5 * 6)
	dc tone_loco_data+(5 * 7)
	dc tone_loco_data+(5 * 8)
	dc tone_loco_data+(5 * 9)
	dc tone_loco_data+(5 * 10)
	dc tone_loco_data+(5 * 11)
	dc tone_loco_data+(5 * 12)
	dc tone_loco_data+(5 * 13)
	dc tone_loco_data+(5 * 14)
	dc tone_loco_data+(5 * 15)
	dc tone_loco_data+(5 * 16)
	dc tone_loco_data+(5 * 17)
	dc tone_loco_data+(5 * 18)
	dc tone_loco_data+(5 * 19)
	dc tone_loco_data+(5 * 20)
	dc tone_loco_data+(5 * 21)
	dc tone_loco_data+(5 * 22)
	dc tone_loco_data+(5 * 23)
	dc tone_loco_data+(5 * 24)

tone_hico_table:
	dc tone_hico_data+(5 * 0)
	dc tone_hico_data+(5 * 1)
	dc tone_hico_data+(5 * 2)
	dc tone_hico_data+(5 * 3)
	dc tone_hico_data+(5 * 4)
	dc tone_hico_data+(5 * 5)
	dc tone_hico_data+(5 * 6)
	dc tone_hico_data+(5 * 7)
	dc tone_hico_data+(5 * 8)
	dc tone_hico_data+(5 * 9)
	dc tone_hico_data+(5 * 10)
	dc tone_hico_data+(5 * 11)
	dc tone_hico_data+(5 * 12)
	dc tone_hico_data+(5 * 13)
	dc tone_hico_data+(5 * 14)
	dc tone_hico_data+(5 * 15)
	dc tone_hico_data+(5 * 16)
	dc tone_hico_data+(5 * 17)
	dc tone_hico_data+(5 * 18)
	dc tone_hico_data+(5 * 19)
	dc tone_hico_data+(5 * 20)
	dc tone_hico_data+(5 * 21)
	dc tone_hico_data+(5 * 22)
	dc tone_hico_data+(5 * 23)
	dc tone_hico_data+(5 * 24)

tone_loco_data:
    ; Format: b0, b1, b2, a1, a2 (divided by 2)
    ; Index 0 (-12 dB) to Index 12 (0 dB) to Index 24 (+12 dB)
    dc 0.490258, -0.961003, 0.470870, -0.960817, 0.461315  ; -12 dB
    dc 0.491089, -0.962069, 0.471112, -0.961900, 0.462371  ; -11 dB
    dc 0.491914, -0.963106, 0.471332, -0.962954, 0.463399  ; -10 dB
    dc 0.492735, -0.964115, 0.471529, -0.963979, 0.464400  ; -9 dB
    dc 0.493551, -0.965097, 0.471704, -0.964977, 0.465374  ; -8 dB
    dc 0.494364, -0.966052, 0.471856, -0.965948, 0.466323  ; -7 dB
    dc 0.495173, -0.966981, 0.471986, -0.966892, 0.467247  ; -6 dB
    dc 0.495980, -0.967885, 0.472093, -0.967811, 0.468147  ; -5 dB
    dc 0.496785, -0.968764, 0.472178, -0.968705, 0.469022  ; -4 dB
    dc 0.497589, -0.969618, 0.472241, -0.969575, 0.469874  ; -3 dB
    dc 0.498392, -0.970450, 0.472282, -0.970421, 0.470704  ; -2 dB
    dc 0.499196, -0.971258, 0.472301, -0.971244, 0.471511  ; -1 dB
    dc 0.500000, -0.972044, 0.472297, -0.972044, 0.472297  ;  0 dB
    dc 0.500805, -0.972808, 0.472270, -0.972823, 0.473061  ; +1 dB
    dc 0.501613, -0.973551, 0.472222, -0.973580, 0.473805  ; +2 dB
    dc 0.502423, -0.974272, 0.472151, -0.974316, 0.474529  ; +3 dB
    dc 0.503236, -0.974974, 0.472057, -0.975033, 0.475234  ; +4 dB
    dc 0.504053, -0.975656, 0.471941, -0.975730, 0.475920  ; +5 dB
    dc 0.504874, -0.976318, 0.471802, -0.976407, 0.476587  ; +6 dB
    dc 0.505701, -0.976961, 0.471640, -0.977066, 0.477236  ; +7 dB
    dc 0.506533, -0.977586, 0.471455, -0.977707, 0.477867  ; +8 dB
    dc 0.507372, -0.978193, 0.471247, -0.978330, 0.478482  ; +9 dB
    dc 0.508218, -0.978782, 0.471016, -0.978936, 0.479079  ; +10 dB
    dc 0.509072, -0.979353, 0.470760, -0.979526, 0.479661  ; +11 dB
    dc 0.509935, -0.979908, 0.470481, -0.980099, 0.480226  ; +12 dB

tone_hico_data:
    ; Format: b0, b1, b2, a1, a2 (divided by 4)
    ; Index 0 (-12 dB) to Index 12 (0 dB) to Index 24 (+12 dB)
    dc 0.091448, -0.059632, 0.001814, -0.287527, 0.071157  ; -12 dB
    dc 0.099416, -0.067129, 0.002899, -0.283011, 0.068196  ; -11 dB
    dc 0.108085, -0.075466, 0.004183, -0.278432, 0.065235  ; -10 dB
    dc 0.117517, -0.084729, 0.005697, -0.273790, 0.062275  ; -9 dB
    dc 0.127780, -0.095018, 0.007469, -0.269087, 0.059318  ; -8 dB
    dc 0.138948, -0.106435, 0.009534, -0.264321, 0.056368  ; -7 dB
    dc 0.151097, -0.119098, 0.011933, -0.259494, 0.053427  ; -6 dB
    dc 0.164315, -0.133134, 0.014708, -0.254607, 0.050496  ; -5 dB
    dc 0.178696, -0.148682, 0.017906, -0.249660, 0.047579  ; -4 dB
    dc 0.194339, -0.165896, 0.021583, -0.244653, 0.044679  ; -3 dB
    dc 0.211357, -0.184943, 0.025795, -0.239588, 0.041797  ; -2 dB
    dc 0.229867, -0.206005, 0.030610, -0.234465, 0.038936  ; -1 dB
    dc 0.250000, -0.229285, 0.036101, -0.229285, 0.036101  ;  0 dB
    dc 0.271896, -0.255001, 0.042347, -0.224049, 0.033291  ; +1 dB
    dc 0.295709, -0.283394, 0.049439, -0.218756, 0.030512  ; +2 dB
    dc 0.321603, -0.314725, 0.057475, -0.213411, 0.027764  ; +3 dB
    dc 0.349757, -0.349282, 0.066565, -0.208011, 0.025052  ; +4 dB
    dc 0.380367, -0.387376, 0.076828, -0.202559, 0.022378  ; +5 dB
    dc 0.413642, -0.429351, 0.088398, -0.197056, 0.019745  ; +6 dB
    dc 0.449810, -0.475577, 0.101419, -0.191502, 0.017155  ; +7 dB
    dc 0.489119, -0.526461, 0.116055, -0.185900, 0.014612  ; +8 dB
    dc 0.531836, -0.582446, 0.132479, -0.180250, 0.012118  ; +9 dB
    dc 0.578250, -0.644014, 0.150888, -0.174552, 0.009676  ; +10 dB
    dc 0.628675, -0.711688, 0.171493, -0.168810, 0.007289  ; +11 dB
    dc 0.683449, -0.786041, 0.194530, -0.163022, 0.004960  ; +12 dB





;-----------------------------------------------------------------------
; 	reverb delay buffers
	org	reverb_eram,x:$1000
;-----------------------------------------------------------------------
	align 4096
c1d 	ds	2203	
c1m		equ	2203-1
c1g1	equ	0.3046875
c1g2	equ	0.5812382

	align 512
a1d 	ds 263
a1m		equ	263-1
a1g1	equ	0.7000000
a1g2	equ	0.0000000

	align 4096
c2d		ds 2467
c2m		equ	2467-1
c2g1	equ	0.3281250
c2g2	equ	0.5616459

	align 4096
c3d 	ds 2689
c3m		equ	2689-1
c3g1	equ	0.3515625
c3g2	equ	0.5420536

	align 4096
c4d		ds 2999
c4m		equ	2999-1
c4g1	equ	0.3750000
c4g2	equ	0.5224613

	align 512
a2d 	ds 313
a2m		equ	313-1
a2g1	equ	0.8500000
a2g2	equ	0.0000000

	align 4096
c5d		ds 3169
c5m		equ	3169-1
c5g1	equ	0.3984375
c5g2	equ	0.5028690

	align 4096
c6d		ds 3433
c6m		equ	3433-1
c6g1	equ	0.4218750
c6g2	equ	0.4832767


;-----------------------------------------------------------------------
; 	reverb xram
	org	x:
;-----------------------------------------------------------------------
a1r			dc	a1d
a1md		dc	a1m
a1g1d		dc	a1g1
a2r			dc	a2d
a2md		dc	a2m
a2g1d		dc	a2g1

L_overall	dc	$7FFFFF
R_overall	dc	$7FFFFF

	align	8
signal_vector
Lin			dc	0.0
Rin			dc	0.0
Reverb_L	dc	0.0
Reverb_R	dc	0.0
Lout		dc	0
Rout		dc	0

	align 8
curaddr:	dc c1d,c2d,c3d,c4d,c5d,c6d

	align 8
lowstate:	dc 0,0,0,0,0,0

	align 8
comb_g_tab:	dc 0,0,0,0,0,0

; Diffusion / density → a1g1 (0.7) and a2g1 (0.75)
; The allpass coefficients.
; Higher = more smearing of each echo = denser, smoother, less "grainy" tail.
; These two differ deliberately (along with the 263/277 lengths) to decorrelate L from R.
; Push both toward 0.8 for a smoother tail, drop toward 0.5 for a more discrete/gritty one.

;lowpass_cutoff_start
; overall brightness.
; More negative → lower g1 → brighter, less damped.
; Toward zero and positive → darker.

;lowpass_cutoff_slope
;how much more damped each successive (longer) comb is than the last.
;Comb 1 is unaffected by slope; comb 6 gets 5× the slope added.

; comb_g is length / decay time. 
;0.643 1.0s
;0.745 1.5s
;0.802 2.0s
;0.838 2.5s
;0.863 3.0s
;0.896 4.0s

lowpass_cutoff_start
;	dc	 0.8681000	; was -0.3906250
;	dc	-0.0681000	; was -0.3906250
	dc	-0.1681000	; was -0.3906250
;	dc	-0.9000000	; was -0.3906250
;	dc	-0.3000000	; was -0.3906250

lowpass_cutoff_slope
;	dc	0.0051000	; was  0.0468750
;	dc	0.0151000	; was  0.0468750
	dc	0.0451000	; was  0.0468750
;	dc	0.0900000	; was  0.0468750

;lowpass_cutoff_start
;	dc	-0.3906250	; ctl value of 39
;lowpass_cutoff_slope
;	dc	0.0468750	; ctl value of 67
comb_g
;	dc	0.8325507974	; ctl value of 57
	dc	0.802


;0.0622 8.0s
;0.1245 4.0s
;0.1659 3.0s
;0.2489 2.0s
;0.3319 1.5s
;0.4979 1.0s
;0.6223 0.8s

rvb_decay
;	dc	0.2489
	dc	0.3319
;	dc	0.2489


;-----------------------------------------------------------------------
; 	reverb yram
	org	y:
;-----------------------------------------------------------------------
consts
	dc	c1m
	dc	c1g1
	dc	c1g2
	dc	c2m
	dc	c2g1
	dc	c2g2
	dc	c3m
	dc	c3g1
	dc	c3g2
	dc	c4m
	dc	c4g1
	dc	c4g2
	dc	c5m
	dc	c5g1
	dc	c5g2
	dc	c6m
	dc	c6g1
	dc	c6g2

gain_vectors
Reverb_Lin			dc	0.5
Reverb_Rin			dc	0.5
Reverb_Reverb_L		dc	0.0
Reverb_Reverb_R		dc	0.0
Lout_Lin			dc	1.0
Lout_Rin			dc	0.0
Lout_Reverb_L		dc	ONE
Lout_Reverb_R		dc	0.0
Rout_Lin			dc	0.0
Rout_Rin			dc	-1.0
Rout_Reverb_L		dc	0.0
Rout_Reverb_R		dc	-1.0

comb_w
	dc	0.5000000	; 2203
	dc	0.5599183	; 2467
	dc	0.6103041	; 2689
	dc	0.6806627	; 2999
	dc	0.7192465	; 3169
	dc	0.7791648	; 3433



;-----------------------------------------------------------------------
; 	reverb code
	org	p:
;-----------------------------------------------------------------------
fx_reverb:

	; copy inputs
	move	x:<sam,a
	move	a,x:Lin
	move	y:<sam,b
	move	b,x:Rin

	; compute outputs using matrix multiply
	; [reverb_in out_l out_r] = [gain_vector] * [signal_vector]
	movec	#3,m0
	move	#signal_vector,r0
	move	#gain_vectors,r4
	move	x:L_overall,y1
	clr		a			x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	macr	x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	clr		a			a,b
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	macr	x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	clr		a			a,x1
	mpy		x1,y1,a		x:R_overall,y1
	clr		a			a,x:<sam
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	mac		x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	macr	x0,y0,a		x:(r0)+,x0 		y:(r4)+,y0
	clr		a			a,x1
	mpy		x1,y1,a
	clr		a			a,y:<sam

	; r0	curaddr (m0 = whatever)
	; r1	lowstate vector (m1 = -1)
	; r4	ptr to modulus,g1,g2 consts (m4 = -1)
	; r5	curaddr ptr (m5 = -1)
	; b		comb out accum
	; x0	scaled comb in

	clr		b			b,x0	;get reverb_in
	move	#1.0/16,x1			;allow 4 bits of headroom
	mpyr	x0,x1,a				;and 4 bits of noise
	move	a,x0
	move	#lowstate,r1
	move	#consts,r4
	move	#curaddr,r5

	do		#6,_comb_loop
	move	x:(r5),r0						;r0=curaddr
	movec	y:(r4)+,m0						;m0=modulus
	move	x:(r1),x1 	y:(r4)+,y1			;x1=lowstate, y1=g1, wait for m0
	move	x:(r0)+,y0						;y0=delay out
	add		y0,b		y0,a				;a=delay out
	macr	x1,y1,a		y:(r4)+,y1			;a=out+g1*lowstate, y1=g2
	clr		a      		a,x:(r1)+ a,y0		;y0=new lowstate
	add		x0,a		r0,x:(r5)+			;a=in, save ptr
	macr	y0,y1,a							;a=in+g2*g1*lowstate
	move	a,x:-(r0)						;store delay in (takes 2 cyc)
_comb_loop

	; scale
	rnd		b			#$150000,x0		;scale by ~1/6
	move	b,y0
	mpyr	x0,y0,b
	move	b,y1		;save b for right chan

	; allpass L
	move	x:a1r,r0
	movec	x:a1md,m0
	move	x:a1g1d,x0
	move	x:(r0),x1
	macr	x0,x1,b		x1,a
	move	b,y0
	macr	-x0,y0,a 	b,x:(r0)+
	asl		a   		r0,x:a1r
	asl		a			;get rid of the headroom
	asl		a
	asl		a
	move	a,x:Reverb_L

	; allpass R
	tfr		y1,b		x:a2r,r0
	movec	x:a2md,m0
	neg		b			x:a2g1d,x0
	move	x:(r0),x1
	macr	x0,x1,b		x1,a
	move	b,y0
	macr	-x0,y0,a 	b,x:(r0)+
	asl		a			r0,x:a2r
	asl		a
	asl		a
	asl		a
	move	a,x:Reverb_R

	movec	x:<ffff,m0
	rts


fx_reverb_init:
	move	#c1d,r0
	movec	#c1m,m0
	jsr		_clearline
	move	#c2d,r0
	movec	#c2m,m0
	jsr		_clearline
	move	#c3d,r0
	movec	#c3m,m0
	jsr		_clearline
	move	#c4d,r0
	movec	#c4m,m0
	jsr		_clearline
	move	#c5d,r0
	movec	#c5m,m0
	jsr		_clearline
	move	#c6d,r0
	movec	#c6m,m0
	jsr		_clearline
	move	#a1d,r0
	movec	#a1m,m0
	jsr		_clearline
	move	#a2d,r0
	movec	#a2m,m0
	jsr		_clearline
	jsr		fx_reverb_recalc
	movec	x:<ffff,m0
	rts
_clearline:
	clr		a
	rep		#1024
	move	a,x:(r0)+
	rep		#1024
	move	a,x:(r0)+
	rep		#1024
	move	a,x:(r0)+
	rep		#1024
	move	a,x:(r0)+
	rts


fx_reverb_recalc:
	move	x:rvb_decay,x1
	move	#comb_w,r1		; weights (Y)
	move	#comb_g_tab,r4	; results (X)

	; generate comb table
	do		#6,_recalc1
	move	y:(r1)+,y0		; w_i
	mpyr	x1,y0,a			; v = q * w_i
	move	a,x0

	move	#0.0048091,y0
	move	#0.0277520,a
	macr	-x0,y0,a		; k3 - v*k4
	move	a,y0
	move	#0.1201132,a
	macr	-x0,y0,a		; k2 - v*(...)
	move	a,y0
	move	#0.3465736,a
	macr	-x0,y0,a		; k1 - v*(...)
	move	a,y0
	move	#0.5,a
	macr	-x0,y0,a		; h = 2^-v / 2

	move	a,x0
	mpy		x0,x0,a			; h^2
	asl		a
	asl		a				; (2h)^2 = g_i
	move	a,x:(r4)+
_recalc1

	; generate constants
	move	#consts,r1
	move	#comb_g_tab,r4
	move	x:lowpass_cutoff_start,b
	move	x:lowpass_cutoff_slope,y1
	move	x:comb_g,x1
	move	#0.5,x0

	do		#6,_recalc2
	move	x:(r4)+,x1
	clr		a			(r1)+		;skip over the modulus const
	add		x0,a		b,y0
	mac		x0,y0,a					;g1 = .5 * b + .5
	clr		a			a,y0		;y0 = g1
	add		x1,a		y0,y:(r1)+	;store g1
	macr	-x1,y0,a				;g2 = g(1-g1) = g - g * g1
	add		y1,b		a,y:(r1)+	;store g2
_recalc2
	rts
