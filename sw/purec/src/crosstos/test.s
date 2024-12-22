	TEXT

start:

	nop
	nop

end:
	TRAP #1

loop:
	jmp loop

	DATA
info:
	dc.l end
	dc.l end
	dc.l end
	dc.l end

	
