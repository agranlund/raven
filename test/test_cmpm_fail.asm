; XFAIL

	org p:$40
	
; X is not a valid source register and should be rejected

	cmpm	x,a
