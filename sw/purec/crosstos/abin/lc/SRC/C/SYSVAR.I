**
** sysvar.i - Atari ST system variables
**
** $Log: sysvar.i,v $
** Revision 1.2  1991/08/05  10:23:32  AGK
** Updated for new TT/STE system variables.
**
** Revision 1.1  1991/08/05  10:21:26  AGK
** Initial revision
**
** Copyright (c) 1989 HiSoft
**
		offset	$400

etv_timer	ds.l	1	$400
etv_critic	ds.l	1	$404
etv_term	ds.l	1	$408
etv_xtra	ds.l	5	$40c
memvalid	ds.l	1	$420
memcntlr	ds.b	1	$424
		ds.b	1
resvalid	ds.l	1	$426
resvector	ds.l	1	$42a
phystop		ds.l	1	$42e
_membot		ds.l	1	$432
_memtop		ds.l	1	$436
memval2		ds.l	1	$43a
flock		ds.w	1	$43e
seekrate	ds.w	1	$440
_timr_ms	ds.w	1	$442
_fverify	ds.w	1	$444
_bootdev	ds.w	1	$446
palmode		ds.w	1	$448
defshiftmd	ds.b	1	$44a
		ds.b	1
sshiftmd	ds.w	1	$44c
_v_bas_ad	ds.l	1	$44e
vblsem		ds.w	1	$452
nvbls		ds.w	1	$454
_vblqueue	ds.l	1	$456
colorptr	ds.l	1	$45a
screenpt	ds.l	1	$45e
_vbclock	ds.l	1	$462
_frclock	ds.l	1	$466
hdv_init	ds.l	1	$46a
swv_vec		ds.l	1	$46e
hdv_bpb		ds.l	1	$472
hdv_rw		ds.l	1	$476
hdv_boot	ds.l	1	$47a
hdv_mediach	ds.l	1	$47e
_cmdload	ds.w	1	$482
conterm		ds.b	1	$484
		ds.b	1
trp14ret	ds.l	1	$486
criticret	ds.l	1	$48a
themd		ds.l	4	$48e
_md		ds.l	1	$49e
savptr		ds.l	1	$4a2
_nflops		ds.w	1	$4a6
con_state	ds.l	1	$4a8
save_row	ds.w	1	$4ac
sav_context	ds.l	1	$4ae
_bufl		ds.l	2	$4b2
_hz_200		ds.l	1	$4ba
the_env		ds.b	4	$4be
_drvbits	ds.l	1	$4c2
_dskbufp	ds.l	1	$4c6
_autopath	ds.l	1	$4ca
_vbl_list	ds.l	8	$4ce
_prt_cnt	ds.w	1	$4ee
_prtabt		ds.w	1	$4f0
_sysbase	ds.l	1	$4f2
_shell_p	ds.l	1	$4f6
end_os		ds.l	1	$4fa
exec_os		ds.l	1	$4fe
scr_dump	ds.l	1	$502
prv_lsto	ds.l	1	$506
prv_lst		ds.l	1	$50a
prv_auxo	ds.l	1	$50e
prv_aux		ds.l	1	$512
pun_ptr		ds.l	1	$516
memval3		ds.l	1	$51a
xconstat	ds.l	8	$51e
xconin		ds.l	8	$53e
xcostat		ds.l	8	$55e
xconout		ds.l	8	$57e
_longframe	ds.w	1	$59e
_p_cookies	ds.l	1	$5a0
fmemtop		ds.l	1	$5a4
fmemvalid	ds.l	1	$5a8
bell_hook	ds.l	1	$5ac
kcl_hook	ds.l	1	$5b0
