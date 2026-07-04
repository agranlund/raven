COMPATIBLE:     EQU     0

                //INCLUDE macros.asm

; ====================================
; === current limits of DSP engine ===
; ====================================

                IFNE    COMPATIBLE
MAX_POINTS:     EQU     600
MAX_TRIANGLES:  EQU     800
                ELSE
MAX_POINTS:     EQU     750
MAX_TRIANGLES:  EQU     1150
                ENDC
				
				
				org p:$40
				
				move #MAX_POINTS, x0
				move #MAX_TRIANGLES, y0