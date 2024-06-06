	.text
	.even
	.globl	__infinitydf

__infinitydf:			| +infinity as proposed by IEEE
	.long	0x7ff00000,0x00000000
