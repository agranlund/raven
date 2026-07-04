	
recurse:	macro
.lab:	dc \1
	
	if \1 + 1 < 0x20
		recurse \1 + 1
	else
		ifdef TEST_ERROR
			error "User generated error!"
		endc
	endc
		
	endm

	
	test_error:	macro
	
		msg "----------------------------------------------"
		
		if 1 = 1
			error "User generated error!"
		else
			error "User generated error! a wrong one"
		endc
		
		if 1 = 2
			error "Second user generated error! a wrong one"
		else
			error "Second user generated error!"
		endc

		msg "----------------------------------------------"
	
	endm
	
whost:	MACRO
        jclr    #1,X:<<$ffe9,*
        movep   \1,X:<<$ffeb
		ENDM