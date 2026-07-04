
	org	x:9
	align 8
	dc 1

	org	x:30
	dc 1
	dsm 3
	dc 2
	
	org	y:$100

data:	ds	10

data2:	ds	0
								; creating a string in data
	dc	"text test"				; this is a bit pointless on dsp...


	
	org p:$40
	
	move # int("dup") , x0		; this the way to define literal constant
	
	
	
;;	org	l:$200
;;	dc	0.1f
	