	org p:0
	jmp <start

	org	p:$40
	
start:

	;; first flavor of local labels

	do	#4,.loop
		jsr	<do_some_work
.loop:

	
	do	#4,.loop1
		jsr	<do_some_work
.loop1:

	jmp	test_2

do_some_work:	
			nop
			nop
			rts
		
		;; second flavor of local labels
test_2:

		do	#4,_loop
		jsr	<do_some_work
_loop:

	
	do	#4,_loop1
		jsr	<do_some_work
_loop1:


.done:
		movec   #<6-1,m4
		jclr    #0,x0,<.done //(error in local label evaluation?)
		move    #'CSR',x0
		movec   #<0,omr
	
	
	