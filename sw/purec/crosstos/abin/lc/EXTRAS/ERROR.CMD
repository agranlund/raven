;	ERROR.CMD:	Parse Lattice C compilation error files
;			using MicroEMACS 3.10
;
;	(C)opyright 1987 by Daniel M. Lawrence
;	(C)opyright 1992 HiSoft
;
;	This is an example script file for use with MicroEMACS to
;	demonstrate how the -E and $EDITOR environment variables may be
;	used to provide a semi-integrated environment from a command
;	line mode of operation. To make MicroEMACS 3.10 automatically
;	read this file on a compiler error, add the environment variable
;	entries:
;
;		EDITOR=d:\bin\local\me.prg	; or whatever
;		LC_OPT=-E=-e
;

;	Run standard startup file

	execute-file "emacs.rc"

;	Load up the needed macros
	set $discmd "FALSE"
	write-message "[Setting UP]"

;	cycle to the next error
39	store-macro
	run closelast
!force	1 next-line
	run getnext
!endm
	bind-to-key execute-macro-39	FN1

;	cycle to the previous error
38	store-macro
	run closelast
!force	1 previous-line
	run getnext
!endm
	bind-to-key execute-macro-38	FN2

;	get the next error
store-procedure	getnext

	;check for end of errors
	2 next-window
	!if &seq $line ""
		3 next-window
		write-message "[No more errors]"
		!return
	!endif

	set $discmd "FALSE"

	;grab the file name
	set-mark
	search-forward " "
	backward-character
	kill-region
	yank
	set %efile $kill

	;and line number
	forward-character
	set-mark
	search-forward ":"
	backward-character
	kill-region
	yank
	set %eline $kill

	;and column number
	forward-character
	set-mark
	search-forward " "
	backward-character
	kill-region
	yank
	set %ecol &sub $kill 1

	;mark the line
	beginning-of-line
	insert-string "["
	end-of-line
	insert-string "]"
	beginning-of-line
	unmark-buffer

;	save the last file if different
	3 next-window
	!if &not &seq $lastfile ""
		!if &not &seq %lastfile %efile
			save-file
			select-buffer "[temp]"
			delete-buffer $lastfile
		!endif
	!endif
	find-file %efile
	%eline goto-line
	beginning-of-line
	%ecol forward-character
	update-screen
	write-message &cat &cat &cat &cat "[Error found in " %efile " at line " %eline "]"
	set $discmd "TRUE"
!endm

;	close the last error
store-procedure	closelast

	;if at the end.... go away
	2 next-window
	!if &seq $line ""
		!return
	!endif

	;unmark the last error
	beginning-of-line
	delete-next-character
	end-of-line
	delete-previous-character
	beginning-of-line

	;and save the file name
	set %lastfile %efile
!endm

;	Abort!
40	store-macro
	4 exit-emacs
!endm
	bind-to-key execute-macro-40	FN5

;	save current file

	bind-to-key save-file		FN9

;	recompile!!!!

	bind-to-key	quick-exit	FN0


;	set the help window

	1 split-current-window
	select-buffer "[Lattice C-Errors]"
	insert-string "Lattice C                               Compilation Error Scanning~n"
	insert-string "        F1      Go to next error        F9      Save current source file~n"
	insert-string "        F2      Go to last error        F10     Re-compile~n"
	insert-string "                        F5      Abort compiler"
	beginning-of-file
	4 resize-window
	add-mode "red"
	unmark-buffer

;	set up the error line window

	next-window
	1 split-current-window
	4 resize-window
	add-mode "black"
*getcc
	!if &not &seq &right $cbufname 4 ".err"
		next-buffer
		!goto getcc
	!endif

;	set up the current source window

	next-window
	next-buffer

;	initialize

	set %lastfile ""
	run getnext
	set $gflags 0

	set $discmd "TRUE"

;	wait for a command
