	include "dsp56303.inc"

	org p:$0
	jmp start1

	org	p:$100
	; write external y/p ram
start1:
	move	#$550800,a	; 62k external p/y ram
	move	#$800,r0
	do		#62,_l1
	do		#1024,_l0
	move	a,y:(r0)+
	add		#1,a
_l0:nop
_l1:nop
	nop

	; write external x ram
start2:
	move	#$aa0800,a	; 61k external x ram
	move	#$800,r0
	do		#61,_l1
	do		#1024,_l0
	move	a,x:(r0)+
	add		#1,a
_l0:nop
_l1:nop
	nop

	; read external y/p ram
start3:
	move	#$800,r0
	do		#62,_l1
	do		#1024,_l0
	move	y:(r0)+,a
	brclr	#M_HTDE,x:<<M_HSR,*
	movep	a1,x:<<M_HTX
	nop
_l0:nop
_l1:nop
	nop

	; read external x ram
start4:
	move	#$800,r0
	do		#61,_l1
	do		#1024,_l0
	move	x:(r0)+,a
	brclr	#M_HTDE,x:<<M_HSR,*
	movep	a1,x:<<M_HTX
	nop
_l0:nop
_l1:nop
	nop

	jmp		*
