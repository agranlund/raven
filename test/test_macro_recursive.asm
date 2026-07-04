
	include test_macros.asm
	
	org p:$40
	
	recurse 1
	
	ifdef TEST_ERROR
		test_error
	endc
		
	ifndef DONT_ERROR
			msg "User generated error! ifndef test"
	endc
	
	
	whost x0
	
	