				INCLUDE	"gem.i"

				XDEF	aes_global
				XDEF	_GemParBlk
				
				BSS
				
_GemParBlk:		ds.w	VDI_CNTRLMAX    ; control
aes_global:		ds.w	AES_GLOBMAX     ; global
				ds.w	VDI_INTINMAX    ; intin
				ds.w	VDI_INTOUTMAX   ; intout
				ds.w	VDI_PTSOUTMAX   ; ptsout
				ds.l	AES_ADDRINMAX   ; addrin
				ds.l	AES_ADDROUTMAX  ; addrout
				ds.w	VDI_PTSINMAX    ; ptsin
				ds.w	AES_CTRLMAX     ; acontrl
				ds.w	AES_INTINMAX    ; aintin
				ds.w	AES_INTOUTMAX	; aintout
