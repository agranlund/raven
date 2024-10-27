				INCLUDE	"gem.i"

				GLOBL	lbox_ascroll_to
				GLOBL	lbox_scroll_to
				GLOBL	lbox_bscroll_to
				GLOBL	lbox_cnt_items
				GLOBL	lbox_create
				GLOBL	lbox_delete
				GLOBL	lbox_do
				GLOBL	lbox_free_items
				GLOBL	lbox_free_list
				GLOBL	lbox_get_afirst
				GLOBL	lbox_get_first
				GLOBL	lbox_get_avis
				GLOBL	lbox_get_visible
				GLOBL	lbox_get_bentries
				GLOBL	lbox_get_bfirst
				GLOBL	lbox_get_bvis
				GLOBL	lbox_get_idx
				GLOBL	lbox_get_item
				GLOBL	lbox_get_items
				GLOBL	lbox_get_slct_idx
				GLOBL	lbox_get_slct_item
				GLOBL	lbox_get_tree
				GLOBL	lbox_get_udata
				GLOBL	lbox_set_asldr
				GLOBL	lbox_set_slider
				GLOBL	lbox_set_bentries
				GLOBL	lbox_set_bsldr
				GLOBL	lbox_set_items
				GLOBL	lbox_update


				MODULE	lbox_ascroll_to
				
lbox_scroll_to:
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.w	#4,_GemParBlk+aintin
				move.w	d0,_GemParBlk+aintin+2
				move.l  #$af020003,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_bscroll_to
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.w	#7,_GemParBlk+aintin
				move.w	d0,_GemParBlk+aintin+2
				move.l  #$af020003,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_cnt_items
				
				move.l	a0,_GemParBlk+addrin
				move.w	#0,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_create
				
				move.l	a0,_GemParBlk+addrin        ; tree
				lea		_GemParBlk+addrin+4,a0
				move.l	a1,(a0)+                    ; slct
				move.l	4(a7),(a0)+                 ; set
				move.l	8(a7),(a0)+                 ; items
				move.l	12(a7),(a0)+                ; ctrl_objs
				move.l	16(a7),(a0)+                ; objs
				move.l	22(a7),(a0)+                ; user_data
				move.l	26(a7),(a0)+                ; dialog
				lea		_GemParBlk+aintin,a0
				move.w	d0,(a0)+                    ; visible_a  
				move.w	d1,(a0)+                    ; first_a
				move.w	d2,(a0)+                    ; flags
				move.w	20(a7),(a0)+                ; pause_a
				move.w	30(a7),(a0)+                ; visible_b
				move.w	32(a7),(a0)+                ; first_b
				move.w	34(a7),(a0)+                ; entries_b
				move.w	36(a7),(a0)+                ; pause_b
				clr.l	_GemParBlk+addrout
				move.l  #$aa080008,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	lbox_delete
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$ad000101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_do
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$ac010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_free_items
				
				move.l	a0,_GemParBlk+addrin
				move.w	#2,_GemParBlk+aintin
				move.l  #$af010001,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_free_list
				
				move.l	a0,_GemParBlk+addrin
				move.w	#3,_GemParBlk+aintin
				move.l  #$af010001,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_afirst
				
lbox_get_first:
				move.l	a0,_GemParBlk+addrin
				move.w	#4,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_avis
				
lbox_get_visible:
				move.l	a0,_GemParBlk+addrin
				move.w	#2,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_bentries
				
				move.l	a0,_GemParBlk+addrin
				move.w	#11,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_bfirst
				
				move.l	a0,_GemParBlk+addrin
				move.w	#12,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_bvis
				
				move.l	a0,_GemParBlk+addrin
				move.w	#10,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_idx
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#9,_GemParBlk+aintin
				move.l  #$ae010102,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_item
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin+2
				move.w	#7,_GemParBlk+aintin
				move.l  #$ae020001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	lbox_get_items
				
				move.l	a0,_GemParBlk+addrin
				move.w	#6,_GemParBlk+aintin
				move.l  #$ae010001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	lbox_get_slct_idx
				
				move.l	a0,_GemParBlk+addrin
				move.w	#5,_GemParBlk+aintin
				move.l  #$ae010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_get_slct_item
				
				move.l	a0,_GemParBlk+addrin
				move.w	#8,_GemParBlk+aintin
				move.l  #$ae010001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	lbox_get_tree
				
				move.l	a0,_GemParBlk+addrin
				move.w	#1,_GemParBlk+aintin
				move.l  #$ae010001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	lbox_get_udata
				
				move.l	a0,_GemParBlk+addrin
				move.w	#3,_GemParBlk+aintin
				move.l  #$ae010001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	lbox_set_asldr
				
lbox_set_slider:
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	d0,_GemParBlk+aintin+2
				move.w	#0,_GemParBlk+aintin
				move.l  #$af020002,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_set_bentries
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin+2
				move.w	#6,_GemParBlk+aintin
				move.l  #$af020001,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_set_bsldr
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	d0,_GemParBlk+aintin+2
				move.w	#5,_GemParBlk+aintin
				move.l  #$af020002,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_set_items
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#1,_GemParBlk+aintin
				move.l  #$af010002,d1
				bra		_aes
				
				ENDMOD


				MODULE	lbox_update
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l  #$ab000002,d1
				bra		_aes
				
				ENDMOD


