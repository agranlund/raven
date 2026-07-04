
		if 1 != 1
			error "failed!"	
		endc

		if 1 > 2
			error "failed!"
		endc

		if 2 < 1
			error "failed!"
		endc

		ifdef undefined
			error "failed!"
		endc

defined:	equ	10
ziobro		equ 0

		ifndef defined
			error "failed!"
		else
			if	defined + 1 != 11
				error "failed!"
			endc
		endc

		
		ifne defined
			msg "defined not equal 0"
		endc
		
		ifeq ziobro
			msg "ziobro equals zero"
		endc
		
		if	ziobro != 1
			msg "ziobro indeed equals zero"
		endc

		end
