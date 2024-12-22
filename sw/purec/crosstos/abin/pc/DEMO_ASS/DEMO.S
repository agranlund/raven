;       Demoprogramm fÅr Pure Assembler

                import _StkSize                 ; von PLINK definiert !

start:          lea.l   start-256, a1           ; Start der Basepage
                move.l  12(a1), d1              ; Grîûe des Textsegments
                add.l   20(a1), d1              ; Grîûe des Datensegments
                add.l   28(a1), d1              ; Grîûe des BSS-Segments
                add.l   #256,   d1              ; Grîûe der Basepage

                move.l  d1,  -(sp)              ; Benîtigter Speicher
                move.l  a1,  -(sp)              ; Startadresse
                clr     -(sp)                   ; dummy
                move    #74, -(sp)              ; Mshrink
                trap    #1                      ; Gemdos

                add.l   a1, d1                  ; new stack
                and.b   #$fc, d1                ; gerade Adresse
                move.l  d1, sp                  ; Stack liegt im BSS

                ; Bildschirm lîschen:
                move.l  #clearscreen, -(sp)
                move    #9, -(sp)               ; Cconws
                trap    #1                      ; Gemdos
                addq.l  #6, sp                  ; Stackkorektur

                ; Cconws("Hello world\n\r")
                move.l  #hello, -(sp)
                move    #9, -(sp)
                trap    #1
                addq.l  #6, sp

                ; Cconws("Stacksize: $");
                move.l  #stack, -(sp)
                move    #9, -(sp)
                trap    #1
                addq.l  #6, sp

                ; Cconws(ltohex(_StkSize))
                move.l  #_StkSize, -(sp)
                jsr     ltohex(pc)
                move.l  a0, (sp)
                move    #9, -(sp)
                trap    #1
                addq.l  #6, sp


                ; Cconws("\n\rWir wollen wÅrfeln...")
                move.l  #wuerfeln, -(sp)
                move    #9, -(sp)
                trap    #1
                addq.l  #6, sp

main_loop:      ; Cconws("\n\rDie gewÅrfelte Zahl lautet: ");
                move.l  #ergebnis, -(sp)
                move    #9, -(sp)
                trap    #1
                addq.l  #6, sp

                ; Random()
                move    #17, -(sp)              ; Random
                trap    #14                     ; xbios
                addq.l  #2, sp

                and.l   #$FFFF, d0              ; damit kein öberlauf
                divu    #6, d0                  ; Zufallszahl durch 6
                swap    d0                      ; modulo 6
                add     #$31, d0                ; eins drauf ist Zufallsz.

                ; Cconout(d0)
                move    d0, -(sp)
                move    #2, -(sp)               ; Cconout
                trap    #1
                addq.l  #4, sp

small_loop:     ; Cconws("\n\rnochmal (j/n)?");
                move.l  #nochmal, -(sp)
                move    #9, -(sp)
                trap    #1
                addq.l  #6, sp

                ; Cconin()
                move    #1, -(sp)
                trap    #1
                addq.l  #2, sp

                cmp.b   #'j', d0
                beq     main_loop
                cmp.b   #'J', d0
                beq     main_loop

                cmp.b   #'n', d0
                beq     exit
                cmp.b   #'N', d0
                bne     small_loop

exit:           ; Pterm0()
                move    #0, -(sp)
                trap    #1


; Diese Funktion wandelt einen long Parameter vom Stack in Hex-ascii
; um und benutzt dabei den Buffer. RÅckgabewert ist die Adresse des
; Buffers in a0

ltohex:         move.l  a1, -(sp)               ; save a1
                move.l  d1, -(sp)               ; save d1
                move.l  d2, -(sp)               ; save d2

                move.l  16(sp), d0              ; hole parameter
                move.l  #buffer, a0
                move.l  #ziffern, a1

                move    #8, d2
hex_loop:       move.b  d0, d1
                and.b   #$0f, d1
                ext     d1
                move.b  (a1, d1.w), (a0, d2)
                asr.l   #4, d0
                dbeq    d2, hex_loop


                move.l  (sp)+, d2
                move.l  (sp)+, d1
                move.l  (sp)+, a1
                rts


                data

clearscreen:    dc.b    27, 'E', 0
hello:          dc.b    "Hello world", $d, $a, 0
stack:          dc.b    "Stacksize: $", 0
wuerfeln:       dc.b    $d, $a, "Wir wollen wÅrfeln...", 0
ergebnis:       dc.b    $d, $a, "Die gewÅrfelte Zahl lautet: ", 0
nochmal:        dc.b    $d, $a, "Nochmal (j/n)?", 0
ziffern:        dc.b    "0123456789ABCDEF"
buffer:         dc.b    "00000000", 0
