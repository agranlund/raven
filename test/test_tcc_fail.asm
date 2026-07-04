; XFAIL

	org p:$40

; X is not a valid source register and should be rejected
	
	tlt	x,a						; yes, set new max
