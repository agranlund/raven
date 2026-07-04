

	org	l:$100
	
	dc 1
	
	dc 0x7fffff

	dc 0.1
	
	dc 0x7fffff << 16
	dc 0x7fffff << 16
	dc 0x7fffff << 24

	dc 0x7fffffffffff   ; 48 bit

	ifdef trigger_fail
		move	x0,a
	endc
	