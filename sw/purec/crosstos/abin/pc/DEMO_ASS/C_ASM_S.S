;       C_ASM.S
;       Demoprogramm zum Aufruf von Assemblerroutinen aus
;       C-Programmen und umgekehrt.

        EXPORT  SumMulFac   ; FÅr MAIN.C

        IMPORT  fakult      ; Aus MAIN.C

        IMPORT  printf      ; Aus der Bibliothek PCFLTLIB



        TEXT

SumMulFac:
        move.l  D3, -(A7)   ; Benutzte Register retten
        move.l  D4, -(A7)

        move.w  D0, D3      ; D3,D4 dienen als Variable
        move.w  D1, D4

        muls    D0, D1      ; Produkt berechnen

        move.L  D1, -(A7)   ; Ergebnis mit printf() ausgeben
        move.w  D4, -(A7)
        move.w  D3, -(A7)
        lea     multi, A0
        bsr     printf
        addq    #8, A7      ; Stackkorrektur nach Aufruf von printf()

        move.w  D3, D0
        move.w  D4, D1
        add     D0, D1      ; Summe berechnen

        move.w  D1, -(A7)   ; Ergebnis mit printf() ausgeben
        move.w  D4, -(A7)
        move.w  D3, -(A7)
        lea     summe, A0
        bsr     printf
        addq    #6, A7      ; Stackkorrektur nach Aufruf von printf()

        move.w  d3, d0
        beq     exit
        bsr     fakult      ; FakultÑt berechnen (in C_ASM.C)

exit:
        move.l  (A7)+, D4   ; Register wiederherstellen
        move.l  (A7)+, D3
        rts


        DATA
multi:
        dc.b    "Das Produkt aus %d und %d ist %ld", 10, 0
summe:
        dc.b    "Die Summe aus %d und %d ist %d", 10, 0
