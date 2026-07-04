

constant_float:		equ	0.4f			; defines a float
constant_fract:		equ	0.5				// defines fractional

val_1:				equ	constant_float + float( constant_fract )		// float(x) will convert fixedpoint number to float
val_2:				equ	fract(constant_float) +  constant_fract 		// fract(x) will convert float number to fixedpoint


			org	p:$0
			jmp	start
			
			org	p:$40
			
start:		move	#  val_1 , x0								; if value will be float or fixed point, assembler will try to store it as fixed point
			move	#  val_2 , y0
			move	#  int ( val_1 + 0.1f), y1					; to force conversion to integer you have to use int(x) function 
			move	#  int ( val_2 + 0.1 ), y1
			
			move	#  int ( val_1 + float( 0.1 ) ), y1			; float() is called to make it compatible with val_1 type
			move	#  int ( val_2 + fract( 0.1f ) ), y1		; similar to the above
			
			move	# 0.5 * 0.5 , a
			move	# int( 4 / 2 ) , a
			move	#  0.1 - 0.1  , a
			
			jmp	start