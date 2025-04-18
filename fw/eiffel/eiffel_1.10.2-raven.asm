; ********************************************************************************
; Filename: Eiffel.asm
; Date: July 18, 2001
; Update: November 27, 2004
; 
; Authors: Favard Laurent, Didier Mequignon
; 
; This program is free software; You can redistribute it and/or modify
; it under the terms of the gnu general public license as published by
; The Free Software Foundation; Either version 2 of the license, or
; (at your option) Any later version.
; 
; This program is distributed in the hope that it will be used,
; But within Any Warranty; Without Even the implied warranty of
; Merchantability or fitness for a particular purposes.  See The
; GNU General Public License for More Details.
; 
; You should have received a copy of the gnu general public license
; Along with this program; If not, Write to the Free Software
; Foundation, Inc., 59 temple place, suite 330, boston, ma 02111-1307 USA
; 
; ********************************************************************************
; Hardware Description:
; 
; Processor: 16F876
; Crystal: 4MHz
; WDT: Disable
; RS-232C: Max 232a Converter, TXD/RXD WIRED
; Power supply: via RJ-11 Cable/TXD, RXD
; Soft revision: 1.10
; Revision Board: C
; 
; ********************************************************************************
; Software note:
; 
; - Compatible with the pilot realizes by the OXO company.
; 
; - DEFAUT compatible with the Atari mouse protocol. The peak
; transforms the mouse mouse/2 frames into normal mouse atari,
; Whatever the branched mouse.
; 
; - Addition of additional scan-codes for Atari for the news
; PS/2 keys and new mouse functions.
; 
; See the /docs/xtensionatari.txt file
; 
; ********************************************************************************
; 
; This program makes it possible to translate the scan-codes of the game 3 (or 2
; From version 1.0.5) from a PS/2 keyboard to scan-codes
; Atari IKBD and to manage a standard or smart PS/2 mouse
; Roulette 3 buttons and double roulette 5 buttons. The pimples
; 4 and 5 are assigned to scan-codes.
; 
; The 2 original designed joysticks in the Atari IKBD can also
; Be connected (from version 1.0.4):
; The joystick 0 is branch on port A of the peak:
; RA1: High
; RA2: Bas
; RA3: Gauche
; RA4: Right
; RA5: shot
; The joystick 1 is branch on port C of the peak:
; RC0: High
; RC1: Bas
; RC2: left
; RC3: right
; RC4: shot
; 
; The control of a fan is also possible (from the version
; 1.0.4):
; RC5: Continuous engine control in all or nothing via a mosfet.
; RA0: Analog Entrance Temperature An0, a CTN 10kohms at 25
; deg is connected between an0 and vss (0V) with a reminder
; VDD (+4.3V) of 10kohms. Let Van0 = (4.3 * RCTN)/(10th+RCTN).
; Val_an0 = ADC value on year0 or 0 for 0V and 255 for 4.3V or:
; Val_an0 = (256 * RCTN)/(10th+RCTN)
; 
; The software allows the connection of the card to an RS-232C serie port
; Or directly to an Atari ST computer (RJ-12 or HE-14).
; 
; If the program is compile with LCD (since version 1.0.9), the
; Serial_debug is deleted and an LCD display takes its place in RB4/5
; HD44780 compatible by 2 bits RB4 and RB5 via a 74LS174. In that
; Case The Debug Jumpers who follow are unusable.
; 
; The two jumpers allow:
; 
; Jumper4, allowing to activate or not:
; 
; 5V on Portb4: Atari mode (Translation of keyboard/mouse codes)
; 0V on Portb4: direct mode (no translation, PS/2 direct)
; 
; Jumper 5, allowing to activate or not:
; 
; 5V on Portb5: normal mode (connection to an atari)
; 0V on Portb5: Debug mode (RS-232C connection)
; 
; normal debug
; 
; The Serie 9600 BPS transmission frequency 7812.5 BPS.
; The transmission of the Binary Ascii keyboard bytes.
; The transmission of binary ascii bytes.
; The emission of the text channel 'debug' no
; ********************************************************************************
; Notes:
; Crystal 4 MHz, BPS Rate for Atari = 7812.5
; BPS Rate for RS232 = 9600
; 
; Serial frame: 1 start bit, 8 bits data, 1 stop stop
; 
; JUMPER4 = PIN 4 Port B (Disable State at 5v with Pull-up)
; JUMPER5 = PIN 5 Port B (Disable State at 5v with Pull-up)
; LED GREEN = PIN 6 Port B (Enable State at 0V)
; LED Yellow = PIN 7 Port B (Enable State at 0V)
; ********************************************************************************
; See the Technik.txt and Evolution.txt file
; ********************************************************************************


; ********************************************************************************
; 1.10.2-RAVEN
; - Comments translated to english by google translate
; - Default to scancode set 2 instead of rarely used set 3
; - can be built with ansi layout as default
; - can be built to communicate with high baud rate
; ********************************************************************************


		PROCESSOR p16f876a
		RADIX dec
		
#include <p16f876a.inc>					; Processor Specific Variable Definitions
#include <eiffel.inc>					; macros

		LIST      P=16F876A			; List Directive to Define Processor

		__CONFIG _CP_OFF & _DEBUG_OFF & _WRT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_OFF & _PWRTE_ON & _WDT_OFF & _XT_OSC

; ----- Flags Compilation -------------------------------------------------------------------------------------

SERIAL_DEBUG                 EQU     0; If 0, Portb4 & Portb5 Enlevates, Atari mode only
                                      ; Leave 0 for Lyndon cards
SCROOL_LOCK_ERR              EQU     0; If 1, use to display errors Breakdown mouse PS/2
                                      ; On the SCROLL LOCK LED on the keyboard
NON_BLOQUANT                 EQU     1; If 1, PS/2 non-blocking routines (time-out)
PS2_ALTERNE                  EQU     1; If 1, alternate management keyboard and PS/2 mouse
LCD                          EQU     0; If 1, LCD display In place of the JYYSTICK 0 compatible HD44780
LCD_DEBUG                    EQU     0; If 1, LCD display uses to see the codes sent to Atari
INTERRUPTS                   EQU     1; If 1, Timer 2 Gere by interruptions => Need a residant 1.10


IF SERIAL_DEBUG
BAUDRATE                     EQU     BAUD_9600
ELIFNDEF BAUDRATE
BAUDRATE                     EQU     BAUD_7812_5
ENDIF


; ----- Variables --------------------------------------------------------------------------------------------

; page 0 (usable from 0x20)
Status_Boot                  EQU    0x20; Return to 0 to reset
Info_Boot                    EQU    0x21; Put to 0xff if the Demarre program on page 2 in flash
Val_AN0                      EQU    0x22; Reading CTN on An0
BUTTONS                      EQU    0x23; mouse buttons
OLD_BUTTONS                  EQU    0x24; Former state of mouse buttons
OLD_BUTTONS_ABS              EQU    0x25; Old state of mouse buttons in absolute mode
JOY0                         EQU    0x26; Joystick 0 reading 0
JOYB                         EQU    0x26; = Joy0, just to test eventually with Joy1
JOY1                         EQU    0x27; Reading Joystick 1
BUTTON_ACTION                EQU    0x28; IKBD button mode
Counter_MState_Temperature   EQU    0x29; Temperature reading machine (CPU load reduction)
RCTN                         EQU    0x2A; CTN / 100 resistance value
Idx_Temperature              EQU    0x2B; Index Reading Table Temperature by interpolation
Counter_LOAD                 EQU    0x2C; COMPTER CONTRES REPUSE by LOAD command in main loop
Counter_Debug_Lcd            EQU    0x2E

DEB_INDEX_EM                 EQU    0x38; Data index to send circular buffer Liaison Serie
FIN_INDEX_EM                 EQU    0x39; end index given to send circular buffer
PTRL_LOAD                    EQU    0x3A; Strong weight address LOAD
PTRH_LOAD                    EQU    0x3B; Low weight command LOAD
TEMP5                        EQU    0x3C
TEMP6                        EQU    0x3D
Flags4                       EQU    0x3E  
Flags5                       EQU    0x3F
HEADER_IKBD                  EQU    0x40; IKBD HOST IKBD
KEY_MOUSE_WHEEL              EQU    0x41; Movement scan-code (Wheel & Wheel+)
KEY2_MOUSE_WHEEL             EQU    0x42; Additional byte: scan-code button 4 or 5 (Wheel+)
CLIC_MOUSE_WHEEL             EQU    0x43; Supplementary byte: Central button scan
Value_0                      EQU    0x44; 1 byte reception trame mouse ps/2
Value_X                      EQU    0x45; 2nd byte Reception Motor mouse PS/2
Value_Y                      EQU    0x46; 3rd byte reception trame mouse ps/2
Value_Z                      EQU    0x47; 4th Event byte Reception Motor mouse PS/2
Counter_Mouse                EQU    0x48; COMPTER OCTET Reception Mot mouse PS/2
Temperature                  EQU    0x49; CTN temperature reading on year0
Rate_Joy                     EQU    0x4A; time monitoring joystick ikbd
Counter_5MS                  EQU    0x4B; clock clock and time base 5 ms
Counter_10MS_Joy_Monitor     EQU    0x4C; DELAIRS SEND MONITORING JYYSTICKS
Counter3                     EQU    0x4D; Boucles counter
Counter_10MS_Joy             EQU    0x4E; Keycode Joystick 0 (not 100 ms)
Counter_100MS_Joy            EQU    0x4F; 100 ms meter Keycode Joystick 0 mode
RX_JOY                       EQU    0x50; KEYCODE JOYSTICK 0 IKBD
RY_JOY                       EQU    0x51; Time Ry Mode Keycode Joystick 0 IKBD
TX_JOY                       EQU    0x52; time tx mode keycode joystick 0 ikbd
TY_JOY                       EQU    0x53; Ty mode KEYCODE JOYSTICK 0 IKBD
VX_JOY                       EQU    0x54; time vx fashion keycode joystick 0 ikbd
VY_JOY                       EQU    0x55; Time Vy Mode Keycode Joystick 0 IKBD
Status_Joy                   EQU    0x56; FLAGS JYYSTICK 0 mode Keycode
OLD_JOY                      EQU    0x57; Former state joystick buttons 0 fashion keycode
START_RX_JOY                 EQU    0x58; Tempo RX MODE start -up value KEYCODE JOYSTICK 0
START_RY_JOY                 EQU    0x59; Tempo RY MODE start -up value KEYCODE JOYSTICK 0
START_TX_JOY                 EQU    0x5A; Tempo TX MODE start -up value KEYCODE JOYSTICK 0
START_TY_JOY                 EQU    0x5B; DEMARrage Value Ty Mode Keycode JYYSTICK 0
X_SCALE                      EQU    0x5C; ABSOLLY MOUSED FASTER FASTER
Y_SCALE                      EQU    0x5D; A absolute mouse factor factor
X_POSH                       EQU    0x5E; ABSOLUE SOUS POSITION SOULD STRAY
X_POSL                       EQU    0x5F; ABSOLUE SOUS POSEMENT SOUS Low weight
Y_POSH                       EQU    0x60; ABSOLUE MOUSE POSIT
Y_POSL                       EQU    0x61; ABSOLUE MOUSE POST MOUSE Low weight
X_POSH_SCALED                EQU    0x62; absolute mouse x position with strong weight factor
X_POSL_SCALED                EQU    0x63; absolute mouse x position with low weight factor factor
Y_POSH_SCALED                EQU    0x64; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
Y_POSL_SCALED                EQU    0x65; Absolute mouse position with low weight factor factor
X_MAX_POSH                   EQU    0x66; Absolute x position maximum mouse strong weight
X_MAX_POSL                   EQU    0x67; Absolute x position maximum mouse low weight
Y_MAX_POSH                   EQU    0x68; Absolute position maximum mouse strong weight
Y_MAX_POSL                   EQU    0x69; Absolute position maximum mice low weight
X_MOV                        EQU    0x6A; Relative DEPLACEMENT MOUSE IN X
Y_MOV                        EQU    0x6B; Deplacement Relative mouse in Y
X_INC_KEY                    EQU    0x6C; Keycode mouse in x mode
Y_INC_KEY                    EQU    0x6D; Keycode mouse in x mode
DELTA_X                      EQU    0x6E; Deltax mode Keycode mouse ikbd
DELTA_Y                      EQU    0x6F; Deltay mode Keycode mouse ikbd

; municipalities to all pages
TEMP3                        EQU    0x70
TEMP4                        EQU    0x71
Counter                      EQU    0x72; Boucles counter
Value                        EQU    0x73; OCTET received
TEMP1                        EQU    0x74
Counter2                     EQU    0x75; Boucles counter
PARITY                       EQU    0x76; PARE Scripture/PS/2 reading and flashing
TEMP2                        EQU    0x77
Flags                        EQU    0x78
Flags2                       EQU    0x79
Flags3                       EQU    0x7A
Counter_5MS_Inter            EQU    0x7B; inter, assignment to keep from Eiffel 1.10
Save_STATUS                  EQU    0x7C; inter, assignment to keep from Eiffel 1.10
Save_W                       EQU    0x7D; inter, assignment to keep from Eiffel 1.10
BUFFER_FLASH                 EQU    0x7E; Buffer 2 bytes

; Page 2 in zone not awarded 0 to reset by start_flash if <0x120
YEAR                         EQU    0x110; year, keep this order for the 6 clock variables
MONTH                        EQU    0x111; month
DAY                          EQU    0x112; day
HRS                          EQU    0x113; hours
MIN                          EQU    0x114; minutes
SEC                          EQU    0x115; seconds
YEAR_BCD                     EQU    0x116; year, keep this order for the 6 BCD variables
MONTH_BCD                    EQU    0x117; month
DAY_BCD                      EQU    0x118; day
HRS_BCD                      EQU    0x119; hours
MIN_BCD                      EQU    0x11A; minutes
SEC_BCD                      EQU    0x11B; seconds

USER_LCD                     EQU    0x120; Message 8 CHARACTERS LCD

; Page 3 in zone not awarded 0 to reset by start_flash if <0x1a0
TAMPON_EM                    EQU    0x190; Circular Buffer Serie connection (size_tampon_em in eiffel.inc)

; ----- Program ---------------------------------------------------------

		ORG	0x000
		
		clrf Status_Boot; Discount at 0 when switching or reset hard
Reset_Prog
		bsf PCLATH,3; Page 1 (0x800 - 0xfff)
		bcf PCLATH,4
		goto Start_Flash; jump to the launch program of page 0 or 2

Inter
		btfss INTCON,PEIE; Peripheral interrupt enable is 1 during the interruption
		                 ; So we jump the goto
; -----------------------------------------------------------------
; Startup, initialization (jump here after the boot!)
; Passage requires for interruptions from Eiffel 1.10
; -----------------------------------------------------------------
Startup
			goto Startup2		
		movwf Save_W; backup of w
		swapf STATUS,W
		clrf STATUS; page 0
		movwf Save_STATUS; backup of status
		bcf PIR1,TMR2IF; Acquitta Timer 2
		incf Counter_5MS_Inter,F
		swapf Save_STATUS,W; restitution of W and status
		movwf STATUS
		swapf Save_W,F
		swapf Save_W,W
		retfie
	
Startup2
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Init
		bcf PCLATH,3; page 0 or 2
		goto Init_Flags

; -----------------------------------------------------------------
; Welcome characteristics! (Test on the CT60)
; -----------------------------------------------------------------

IF SERIAL_DEBUG
WelcomeText
		addwf PCL,F
		DT "Eiffel 1.10"
		IF INTERRUPTS
		DT "i"
		ENDIF
		DT" 11/2004"
		DT 0
ENDIF


; -------------------------------------------------------------------
; Main custom -waiting loop:
; We look at if a keyboard is arrived, then mouse
; -------------------------------------------------------------------

Main
		; ------------------------------------------------------------------------------
		; Waiting loop on clocks: Polling on KCLC and MCLK
		; Then blocking opposes and call for the treatment of element
		IF INTERRUPTS
		bsf INTCON,GIE; Authorizes interruptions
		ENDIF
		IF PS2_ALTERNE
		movf Counter_5MS,W
		andlw 3 	
		btfss STATUS,Z; Alternate management every 5 ms keyboard and 15 ms PS/2 mouse
			goto EnableMouse
		ENABLEKEYB
		goto Main_Loop
EnableMouse
		ENABLEMOUSE
		ELSE
		ENABLEPS2; Authorizes keyboard and mouse transfers at the same time
		ENDIF
Main_Loop
			IF PS2_ALTERNE	
			movf Counter_5MS,W
			andlw 3 	
			btfss STATUS,Z; Alternate management every 5 ms keyboard and 15 ms PS/2 mouse
				goto TstMouse; mouse management
			btfsc PORTB,KCLOCK; Clk's descending front
				goto TstAtariSend
			ELSE
			btfsc PORTB,KCLOCK; Clk's descending front
				goto TstMouse; The keyboard does not manifest itself, we go to the mouse
			ENDIF
			IF INTERRUPTS
			bcf INTCON,GIE; prohibited interruptions
			ENDIF
			DISABLEMOUSE; Block mouse
			call KPSGet2; => Value, we recover the keyboard byte
			goto doKeyboard; We call the complete keyboard treatment
TstMouse
			btfsc PORTB,MCLOCK; Clk's descending front
				goto TstAtariSend; The mouse does not manifest itself, we go into control control Atari
			IF INTERRUPTS
			bcf INTCON,GIE; prohibited interruptions
			ENDIF
			DISABLEKEYB; keyboard block
			call MPSGet2; => Value, we recover the byte mouse
			goto doMouse; We call the complete mouse treatment
TstAtariSend
			btfss Flags2,DATATOSEND
				goto TstAtariReceive; Nothing to be sent by the Serie Liaison
	 		btfss PIR1,TXIF; check that buffer is empty
				goto TstAtariReceive; full emission register
			DISABLEPS2; Block keyboard and mouse
			movf DEB_INDEX_EM,W; Circular buffer test
			subwf FIN_INDEX_EM,W
			btfss STATUS,Z
				goto SendData; End_index_em <> deb_index_em
			bcf Flags2,DATATOSEND
			goto Main
SendData
			bsf STATUS,IRP; page 3
			incf DEB_INDEX_EM,W; increment circular buffer data sent
			movwf FSR; Index pointer
			movlw TAILLE_TAMPON_EM
			subwf FSR,W
			btfsc STATUS,C
				clrf FSR; circular buffer
			movf FSR,W
			movwf DEB_INDEX_EM
			movlw LOW TAMPON_EM
			addwf FSR,F; Index pointer
			movf INDF,W; Reading in the stamp
			movwf TXREG; Transmit byte
			goto Main
TstAtariReceive
			btfss PIR1,RCIF; Control control Atari arrival
				goto TstTimer; Reception asynchronous connection
			DISABLEPS2; Block keyboard and mouse
			btfss RCSTA,OERR
				goto FramingErrorTest
			bcf RCSTA,CREN; Overrun Error
			bsf RCSTA,CREN; Enable reception
			goto Main
FramingErrorTest
			btfss RCSTA,FERR 
				goto ReceiveOK
			movf RCREG,W; Acquitta Framing Error error
			goto Main
ReceiveOK
			movf RCREG,W; Get Received Data Into W
			movwf Value; => Value, we test the Atari keyboard commands
			btfss Flags3,RE_TIMER_IKBD
				goto doAtariCommand; Treat the Atari Clavier Collection command
			call Receive_Bytes_Load				
			goto Main	
TstTimer
		IF !INTERRUPTS
			btfss PIR1,TMR2IF
                ELSE
                	movf Counter_5MS_Inter,W
                	subwf Counter_5MS,W
			btfsc STATUS,Z
		ENDIF
			goto Main_Loop
				
; -----------------------------------------------------------------
; Timer management 2 to 5 ms
; -----------------------------------------------------------------

doTimer
		DISABLEPS2; Block keyboard and mouse
		IF !INTERRUPTS
		bcf PIR1,TMR2IF; Acquitta Timer 2
		incf Counter_5MS,F
		movlw 200
		subwf Counter_5MS,W		
		btfss STATUS,Z
			goto NotIncClock
		clrf Counter_5MS
		ELSE
		bcf INTCON,GIE; prohibited interruptions
		movlw 200
		subwf Counter_5MS_Inter,W; Counter_5ms_inter - 200
		btfss STATUS,C
			goto NotIncClockInt; Counter_5ms_inter <200
		movwf Counter_5MS_Inter
		movwf Counter_5MS			
		bsf INTCON,GIE; Authorizes interruptions
		ENDIF
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Inc_Clock; clock incorrect every second
		IF LCD
		IF !LCD_DEBUG
		call Time_Lcd
		call Temperature_Lcd
		ENDIF
		ENDIF
		bcf PCLATH,3; page 0 or 2
		movlw 1
		movwf Counter_MState_Temperature; launches the reading of the temperature
		btfss Flags2,RESET_KEYB
			goto NotResetKeyb
		bcf Flags2,RESET_KEYB; Flag Request Reset Keyboard given to 0
		call KbBAT; Preferable to cmdresetkey with certain keyboards
		movlw 0xF0; Return Code Reset keyboard
		call SerialTransmit_Host
                goto NotIncClock
                IF INTERRUPTS    
NotIncClockInt
		movf Counter_5MS_Inter,W
		movwf Counter_5MS		
		bsf INTCON,GIE; Authorizes interruptions
                goto NotIncClock
		ENDIF
NotResetKeyb		
		btfss Flags2,RESET_MOUSE
			goto NotIncClock
		call cmdResetMouse
NotIncClock
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Read_Temperature; Temperature reading every second via counter_mstate_temperature
		bcf PCLATH,3; page 0 or 2
		btfsc Counter_5MS,0
			goto Main

; -----------------------------------------------------------------
; LCD management every 10 ms
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG
		btfss Flags4,LCD_ENABLE; LCD Timer Management inhibits
			goto Joysticks_10MS
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Message_User_Lcd
		bcf PCLATH,3; page 0 or 2
		ENDIF
		ENDIF      

; -----------------------------------------------------------------
; Management under Timer 2 every 10 ms of the joysticks
; -----------------------------------------------------------------

Joysticks_10MS		
		btfss Flags,JOY_ENABLE; Management by Timer Joysticks
			goto Main; No joystick management
		btfsc Flags2,JOY_MONITOR
			goto Joy_Monitoring; Joystick monitoring
		btfsc Flags2,JOY_EVENT
			goto Not_100MS; Joysticks events
		btfss Flags2,JOY_KEYS; Shipping items
			goto Main; Nothing to manage under the timer at the joysticks
		decfsz Counter_10MS_Joy,F
			goto Not_100MS
		movlw 10
		movwf Counter_10MS_Joy
		incf Counter_100MS_Joy,F
		call SendAtariJoysticks_Fleches; KEYCODE MODE OF THE JOYSTICK 0
Not_100MS
		comf PORTA,W; management events joysticks or Keycode Action on Fire
		movwf TEMP1
		rrf TEMP1,W
		andlw 0x1F; 000FDGBH (fire, right, left, stocking, high)
		movwf TEMP1
		subwf JOY0,W; Joystick 0 reading 0
		btfsc STATUS,Z
			goto Not_Joy0_Change; No change joystick 0
		movf TEMP1,W		
		movwf JOY0; Joystick 0 reading 0
		movlw HEADER_JOYSTICK0; Header Joystick 0
		goto SerialTransmit_Joy
Not_Joy0_Change
		comf PORTC,W
		andlw 0x1F; 000FDGBH (fire, right, left, stocking, high)
		movwf TEMP1
		subwf JOY1,W; Reading Joystick 1
		btfsc STATUS,Z
			goto Main; No change joystick 1
		movf TEMP1,W	
		movwf JOY1; Reading Joystick 1
		movlw HEADER_JOYSTICK1; Header Joystick 1
SerialTransmit_Joy
		movwf HEADER_IKBD
		call Leds_Eiffel_On
		call SendAtariJoysticks
		goto End_Read
		
; -----------------------------------------------------------------
; Management under Timer 2 every 10 ms Sending Translands Joystick continuously
; -----------------------------------------------------------------

Joy_Monitoring
		decfsz Counter_10MS_Joy_Monitor,F
			goto Main
		movf Rate_Joy,W
		movwf Counter_10MS_Joy_Monitor
		call Leds_Eiffel_On
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Read_Joysticks
		bcf PCLATH,3; page 0 or 2
		clrf TEMP1
		btfsc JOY0,4; Joystick 0 button
			bsf TEMP1,1
		btfsc JOY1,4; Joytick 1 button
			bsf TEMP1,0
		movf TEMP1,W; 00000000xy with Y for Fire Joystick 1 and X for Fire Joystick 0
		call SerialTransmit_Host
		swapf JOY0,W; Joystick 0 reading 0
		andlw 0xF0; 4 bits of strong weight
		movwf TEMP1
		movf JOY1,W; Reading Joystick 1
		andlw 0x0F; DGBHDGBH (right, left, bottom, top)
		iorwf TEMP1,W; nnnnmmmm with M for the joystick 1 and n for the joystick 0
		call SerialTransmit_Host
		goto End_Read
		
; -----------------------------------------------------------------
; Atari collection keyboard command
; -----------------------------------------------------------------

doAtariCommand
		bsf Flags,IKBD_ENABLE; Authorized IKBD transfers
		bcf Flags2,JOY_MONITOR; Monitoring Joysticks Desactive
		; ------------------------------------------------------------------------------
		; Temperature
		movf Value,W
		sublw IKBD_GETTEMP; Temperature reading
		btfss STATUS,Z
			goto Not_Get_Temperature
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_GETTEMP
		call SerialTransmit_Host
		movf Temperature,W; temperature
		call SerialTransmit_Host
		movf Val_AN0,W; Reading CTN on An0
		call SerialTransmit_Host
		clrw
		btfsc PORTC,MOTORON
			movlw 1
		call SerialTransmit_Host
		movlw Tab_Temperature-EEProm+IDX_LEVELHTEMP
		call Lecture_EEProm
		call SerialTransmit_Host
		movlw Tab_Temperature-EEProm+IDX_LEVELLTEMP
		call Lecture_EEProm
		call SerialTransmit_Host
		movf RCTN,W; CTN / 100 resistance value
		call SerialTransmit_Host		
		goto Main		
Not_Get_Temperature
		movf Value,W
		sublw IKBD_PROGTEMP; Temperature threshold programming
		btfss STATUS,Z
			goto Not_Prog_Temperature
		clrf Val_AN0; Reading CTN on AN0 => Update the measurement and control of the fan
		call SerialReceive; Temperature programming code
		sublw IDX_TAB_CTN+24; subtract the Max offset
		btfss STATUS,C; If carry = 1 no problem we go
			goto Not_Prog_EEProm; ignore code> idx_tab_ctn+24
		movf Value,W
		addlw Tab_Temperature-EEProm; To point the beginning of the temperature bytes in the table
		goto Prog_EEProm
Not_Prog_Temperature
		; ------------------------------------------------------------------------------
		; Programming mouse and keyboard
		movf Value,W
		sublw IKBD_PROGMS; Mouse programming
		btfss STATUS,Z
			goto Not_Prog_Mouse
		; Indicate the ignition programming mode of the three LEDs
		movlw LEDS_ON
		call cmdLedOnOff
		call SerialReceive; Mouse Programming Code
		sublw IDX_WHREPEAT; subtract the Max offset
		btfss STATUS,C; If carry = 1 no problem we go
			goto Not_Prog_EEProm; ignore code> idx_whrepeat
		movf Value,W
		addlw Tab_Mouse-EEProm; To point the beginning of the bytes mouse in the table
		goto Prog_EEProm
Not_Prog_Mouse
		movf Value,W
		sublw IKBD_PROGKB; Keyboard programming
		btfss STATUS,Z
			goto Not_ProgKB
		; Indicate the ignition programming mode of the three LEDs
		movlw LEDS_ON
		call cmdLedOnOff
		call SerialReceive; keyboard programming code
		comf Value,W		
		btfsc STATUS,Z
			goto ChangeSet; 0xff
		movf Value,W		
		sublw MAX_VALUE_LOOKUP; subtract the Max offset
		btfss STATUS,C; If carry = 1 no problem we go
			goto Not_Prog_EEProm; ignore code> max_value_lookup
		movf Value,W
		addlw Tab_Scan_Codes-EEProm; To point the start of the keyboard bytes in the table
		goto Prog_EEProm
ChangeSet
		movlw Tab_Config-EEProm
Prog_EEProm
		movwf Counter
		call SerialReceive
		WREEPROM Counter,Value
		goto Main
Not_ProgKB
		; ------------------------------------------------------------------------------
		; IKBD mouse operating mode
		movf Value,W
		sublw IKBD_SET_MOUSE_BUTTON_ACTION; Mouse buttons
		btfss STATUS,Z
			goto Not_Set_Mouse_Button_Action
		call SerialReceive
		movwf BUTTON_ACTION
		goto Main
Not_Set_Mouse_Button_Action
		movf Value,W
		sublw IKBD_REL_MOUSE_POS_REPORT; Relative fashion mouse
		btfss STATUS,Z
			goto Not_Rel_Mouse
		bcf Flags2,MOUSE_KEYS
		bcf Flags,MOUSE_ABS
		goto Mouse_Enable
Not_Rel_Mouse
		movf Value,W
		sublw IKBD_ABS_MOUSE_POSITIONING; Absolute fashion mouse
		btfss STATUS,Z
			goto Not_Abs_Mouse
		call SerialReceive; Xmsb, x maximum
		movwf X_MAX_POSH; Absolute x position maximum mouse strong weight
		call SerialReceive; Xlsb
		movwf X_MAX_POSL; Absolute X position maximum mouse low weight
		call SerialReceive; YMSB, Y Maximum
		movwf Y_MAX_POSH; ABSOLUE Position Maximum Mouse Strong Weight
		call SerialReceive; Ylsb
		movwf Y_MAX_POSL; ABSOLUE MOUSE MOUNDUE Position Low weight
		call Init_X_Y_Abs
		bcf Flags2,MOUSE_KEYS
		bsf Flags,MOUSE_ABS
		goto Mouse_Enable
Not_Abs_Mouse
		movf Value,W
		sublw IKBD_SET_MOUSE_KEYCODE_CODE; Mouse flucing touches mode
		btfss STATUS,Z
			goto Not_Mouse_KeyCode
		call SerialReceive; deltax
		call Change_0_To_1
		movwf DELTA_X; Deltax mode Keycode mouse ikbd
		call SerialReceive; deltay
		call Change_0_To_1
		movwf DELTA_Y; Deltay mode Keycode mouse ikbd
		clrf X_INC_KEY; Keycode mouse in x mode
		clrf Y_INC_KEY; Interect in Yeycode Mouse mode
		bsf Flags2,MOUSE_KEYS
		goto Mouse_Enable
Not_Mouse_KeyCode
		movf Value,W
		sublw IKBD_SET_MOUSE_THRESHOLD; unparalleled
		btfsc STATUS,Z
			goto Receive2Bytes; X & y Threshold
		movf Value,W
		sublw IKBD_SET_MOUSE_SCALE; Absolute Mouse Mouse Factor
		btfss STATUS,Z
			goto Not_Mouse_Scale
		call SerialReceive; X
		call Change_0_To_1
		movwf X_SCALE
		call SerialReceive; Y
		call Change_0_To_1
		movwf Y_SCALE
		goto Conv_Not_Scaled
Not_Mouse_Scale
		movf Value,W
		sublw IKBD_INTERROGATE_MOUSE_POS; Request absolute position mouse
		btfss STATUS,Z
			goto Not_Mouse_Pos
		btfss Flags,MOUSE_ABS
			goto Main; <> Absolute fashion
		btfsc Flags2,JOY_MONITOR
			goto Main
		movlw HEADER_ABSOLUTE_MOUSE; Header mouse absolute fashion
		movwf HEADER_IKBD
		bcf PORTB,LEDYELLOW; Led mouse lighter
		call SendAtariMouse
		bsf PORTB,LEDYELLOW; Eteint Led mouse
		goto Main
Not_Mouse_Pos
		movf Value,W
		sublw IKBD_LOAD_MOUSE_POS; Initialization absolute mouse coords
		btfss STATUS,Z
			goto Not_Load_Mouse_Pos
		call SerialReceive; 0
		call SerialReceive; Xmsb, x coordinates
		movwf X_POSH_SCALED; absolute mouse x position with strong weight factor
		call SerialReceive; Xlsb
		movwf X_POSL_SCALED; absolute mouse x position with low weight factor factor
		call SerialReceive; Ymsb, y coordinates
		movwf Y_POSH_SCALED; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
		call SerialReceive; Ylsb
		movwf Y_POSL_SCALED; Absolute mouse position with low weight factor factor
Conv_Not_Scaled
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Conv_Inv_Scale_X
		call Conv_Inv_Scale_Y
		bcf PCLATH,3; page 0 or 2
		goto Main
Not_Load_Mouse_Pos
		movf Value,W
		sublw IKBD_SET_Y0_AT_BOTTOM; 0 mouse below
		btfss STATUS,Z
			goto Not_Y0_At_Bottom
		bsf Flags,SIGN_Y
		goto Main
Not_Y0_At_Bottom
		movf Value,W
		sublw IKBD_SET_Y0_AT_TOP; 0 mouse at the top (out of default)
		btfss STATUS,Z
			goto Not_Y0_At_Top
		bcf Flags,SIGN_Y
		goto Main
Not_Y0_At_Top
		movf Value,W
		sublw IKDB_RESUME; Transfer authorization
		btfss STATUS,Z
			goto Not_Resume
		goto Main
Not_Resume
		movf Value,W
		sublw IKDB_DISABLE_MOUSE; Inhibit the mouse
		btfss STATUS,Z
			goto Not_Disable_Mouse
		bcf Flags,MOUSE_ENABLE
		goto Main
Not_Disable_Mouse
		movf Value,W
		sublw IKDB_PAUSE_OUTPUT; Transfers stop
		btfss STATUS,Z
			goto Not_Pause
		bcf Flags,IKBD_ENABLE
		goto Main
Not_Pause
		; ------------------------------------------------------------------------------
		; IKBD Joysticks operating mode
		movf Value,W
		sublw IKBD_SET_JOY_EVNT_REPORT; Joystick Automatic Transfers
		btfss STATUS,Z
			goto Not_Joy_On
		bcf Flags2,JOY_KEYS
		bsf Flags2,JOY_EVENT
		bsf Flags,JOY_ENABLE
		goto Main
Not_Joy_On
		movf Value,W
		sublw IKBD_SET_JOY_INTERROG_MODE; Automatic joystick transfers stop
		btfss STATUS,Z
			goto Not_Joy_Off
		bcf Flags2,JOY_KEYS
		bcf Flags2,JOY_EVENT
		bsf Flags,JOY_ENABLE
		goto Main
Not_Joy_Off
		movf Value,W
		sublw IKBD_JOY_INTERROG; Question Joysticks
		btfss STATUS,Z
			goto Not_Joy_Interrog
		btfss Flags,JOY_ENABLE
			goto Main; inhibited joysticks
		btfsc Flags2,JOY_KEYS
			goto Main; Keycode Joystick 0 mode
		btfsc Flags2,JOY_MONITOR
			goto Main; monitoring joysticks mode
		comf PORTA,W
		movwf JOY0
		rrf JOY0,W
		andlw 0x1F
		movwf JOY0; 000FDGBH (Fire, right, left, stocking, top) JYYSTICK 0
		comf PORTC,W
		andlw 0x1F
		movwf JOY1; 000FDGBH (Fire, right, left, stocking, top) Joystick 1
		movlw HEADER_JOYSTICKS; Header Joysticks
		goto SerialTransmit_Joy
Not_Joy_Interrog
		movf Value,W
		sublw IKBD_SET_JOY_MONITOR
		btfss STATUS,Z
			goto Not_Joy_Monitor; continuous joystick reading
		call SerialReceive; missed
		call Change_0_To_1
		movwf Rate_Joy
		movwf Counter_10MS_Joy_Monitor
		bsf Flags2,JOY_MONITOR
		bsf Flags,JOY_ENABLE
		goto Main
Not_Joy_Monitor
		movf Value,W
		sublw IKBD_SET_FIRE_BUTTON_MONITOR
		btfss STATUS,Z
			goto Not_Fire_Button
		goto Main
Not_Fire_Button
		movf Value,W
		sublw IKBD_SET_JOY_KEYCODE_MODE; Joystick 0 hits
		btfss STATUS,Z
			goto Not_Joy_KeyCode
		call SerialReceive; Rx
		movwf RX_JOY
		call SerialReceive; Ry
		movwf RY_JOY
		call SerialReceive; Tx
		movwf TX_JOY
		call SerialReceive; Ty
		movwf TY_JOY
		call SerialReceive; Vx
		movwf VX_JOY
		call SerialReceive; Vy
		movwf VY_JOY
		clrf Status_Joy
		movlw 0xFF
		movwf OLD_JOY
		bsf Flags2,JOY_KEYS
		bcf Flags2,JOY_EVENT
		bsf Flags,JOY_ENABLE
		goto Main
Not_Joy_KeyCode
		movf Value,W
		sublw IKDB_DISABLE_JOYSTICKS; Inhibit joysticks
		btfss STATUS,Z
			goto Not_Disable_Joysticks
		bcf Flags,JOY_ENABLE
		goto Main
Not_Disable_Joysticks
		movf Value,W
		sublw IKBD_TIME_OF_DAY_CLOCK_SET; Initializes the clock
		btfss STATUS,Z
			goto Not_Time_Set
		; ------------------------------------------------------------------------------
		; IKBD clock programming
		movlw LOW YEAR_BCD
		movwf FSR
		movlw 6
		movwf Counter
Loop_Get_Time
			call SerialReceive; YY, MM, DD, HH, MM, SS
			bsf STATUS,IRP; Page 2
			movwf INDF
			incf FSR,F
			decfsz Counter,F
		goto Loop_Get_Time
		clrf Counter_5MS
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Conv_Inv_Bcd_Time
		bcf PCLATH,3; page 0 or 2
		goto Main
Not_Time_Set
		movf Value,W
		sublw IKBD_INTERROG_TIME_OF_DAY; clock
		btfss STATUS,Z
			goto Not_Interrog_Time
		; ------------------------------------------------------------------------------
		; IKBD clock reading
		call Leds_Eiffel_On
		call SendAtariClock
		goto End_Read
Not_Interrog_Time
		movf Value,W
		sublw IKBD_MEMORY_LOAD; MEMORY loading
		btfss STATUS,Z
			goto Not_Memory_Load
		; ------------------------------------------------------------------------------
		; Memory loading possible of
		; $ 00A0 A $ 00EF, and $ 0120 A $ 016F
		; If the basic address of the order and the number of byte are 0
		; => Flash programming on page 2 ($ 1000 A $ 1FFF) Motorola format
		call SerialReceive
		movwf PTRH_LOAD; Adrmsb
		call SerialReceive
		movwf PTRL_LOAD; Adrlsb
		call SerialReceive
		movwf Counter_LOAD; Num
		iorwf PTRL_LOAD,W; Adrlsb
		iorwf PTRH_LOAD,W; Adrmsb
		btfsc STATUS,Z
			goto Prog_Flash; Address $ 0000 size $ 00 => Flash programming
		bsf Flags3,RE_TIMER_IKBD; Flag reception ikbd in main loop
		goto Main
Prog_Flash
		; Indicate the ignition programming mode of the three LEDs
	 	movlw LEDS_ON
		call cmdLedOnOff
		call cmdDisableKey
		call cmdDisableMouse
		clrf STATUS; Correct Bug Init_Page Ram before Eiffel 1.10 (IRP = 0)
		bsf PCLATH,3; Page 1 (0x800 - 0xfff) Flashing program
		bcf PCLATH,4
		goto Ecriture_Flash
Not_Memory_Load
		movf Value,W
		sublw IKBD_MEMORY_READ; Memory reading
		btfss STATUS,Z
			goto Not_Memory_Read
		; ------------------------------------------------------------------------------
		; Memory reading possible everywhere
		; from $ 0000 to $ 01FF (PIC and RAM registers)
		; from $ 2100 to $ 20ff (EEPROM)
		; $ 8000 to $ ffff (to see the whole flash from $ 0000 to $ 1FFF
		; byte by byte in Motorola format)
		call SerialReceive
		movwf TEMP1; Adrmsb
		bcf STATUS,IRP; 0-1 pages
		btfsc Value,0; Adrmsb
			bsf STATUS,IRP; 2-3 pages
		call SerialReceive
		movwf Counter2
		movwf FSR; Adrlsb
		call Leds_Eiffel_On
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_MEMORY_LOAD; memory
		call SerialTransmit_Host
		btfsc TEMP1,7
			goto Dump_Flash; Flash reading if address> = 0x8000
		movlw 6; 6 bytes
		movwf Counter
		movf TEMP1,W
		sublw 0x21
		btfsc STATUS,Z
			goto Dump_EEProm; Reading Eeprom if address 0x21xx
Loop_Dump_Ram
			movf INDF,W
			call SerialTransmit_Host; data
			incf FSR,F
			btfsc STATUS,Z
				bsf STATUS,IRP; 2-3 pages
			decfsz Counter,F
		goto Loop_Dump_Ram	
		goto End_Read
Dump_EEProm
			movf Counter2,W
			call Lecture_EEProm
			call SerialTransmit_Host; data
			incf Counter2,F
			decfsz Counter,F
		goto Dump_EEProm	
		goto End_Read
Dump_Flash
		bcf TEMP1,7; Flash address strong weight
		movlw 3; 3 words (6 bytes)
		movwf Counter	
Loop_Dump_Flash
			READ_FLASH TEMP1,Counter2,BUFFER_FLASH; Reading 2 bytes
			movf BUFFER_FLASH+1,W
			call SerialTransmit_Host; Data Weight Low
			movf BUFFER_FLASH,W
			call SerialTransmit_Host; Data strong weight
			incf Counter2,F
			btfsc STATUS,Z
				incf TEMP1,F
			decfsz Counter,F
		goto Loop_Dump_Flash
End_Read
		call Leds_Eiffel_Off
		goto Main
Not_Memory_Read
		movf Value,W
		sublw IKBD_CONTROLLER_EXECUTE; unparalleled
		btfss STATUS,Z
			goto Not_Execute
Receive2Bytes
		call SerialReceive; Adrmsb
		; Adrlsb
Not_Prog_EEProm
		call SerialReceive
		goto Main
Not_Execute
		; ------------------------------------------------------------------------------
		; LCD display
		IF LCD
		IF !LCD_DEBUG
		movf Value,W
		sublw IKBD_LCD; Shipping or data to the LCD
		btfss STATUS,Z
			goto Not_Lcd
		call SerialReceive
                btfss STATUS,Z
                	goto Test_Data_Lcd; <> 0
		call SerialReceive
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call SendINS
		bcf PCLATH,3; page 0 or 2
		bcf Flags4,LCD_ENABLE; Inhibitory LCD Timer Management
		goto Main
Test_Data_Lcd
		movwf Counter_LOAD; number of bytes
		comf Counter_LOAD,W		
		btfss STATUS,Z
			goto Data_Lcd; <> 0xff
		bsf Flags4,LCD_ENABLE; Authorizes Timer Management LCD
		goto Not_Prog_EEProm; last byte
Data_Lcd
		bsf Flags4,RE_LCD_IKBD; Flag reception LCD
		bsf Flags3,RE_TIMER_IKBD; Flag reception ikbd in main loop
		goto Main		
Not_Lcd
		ENDIF
		ENDIF
		movf Value,W
		sublw IKBD_RESET; reset
		btfss STATUS,Z
			goto Not_Reset
		call SerialReceive
		sublw 1; Reset code
		btfss STATUS,Z
			goto Main
		; ------------------------------------------------------------------------------
		; Reset command => Eiffel Reset
		; - The CAPS indicator is canceled in the state word.
		; - We stop the events and PS/2 keyboard.
		; - We send the Reset command to the keyboard and to the PS/2 mouse.
		; - We authorize IKBD, mouse and joystick transfers.
		clrf Flags
		clrf Flags2
		clrf Flags3
		clrf Flags4
		clrf Flags5
		clrf Status_Joy
		clrf DEB_INDEX_EM; Data index to send circular buffer Liaison Serie
		clrf FIN_INDEX_EM; end index given to send circular buffer
		bcf PORTC,MOTORON; Fan stop
		call Leds_Eiffel_On
		IF LCD
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Init_Lcd
		bcf PCLATH,3; page 0 or 2
		ENDIF
		btfss Status_Boot,POWERUP_MOUSE
			goto NotResetMouse
		call cmdDisableMouse
		bsf Flags2,RESET_MOUSE; Valid reset mouse in Timer Treatment
NotResetMouse
		btfss Status_Boot,POWERUP_KEYB
			goto Init_Flags
		movlw LEDS_ON
		call cmdLedOnOff
		bsf Flags2,RESET_KEYB; valid reset keyboard in timer treatment
Init_Flags
		call Init_X_Y_Abs; Absolute position mouse
		movlw Tab_Config-EEProm
		call Lecture_EEProm
		sublw 2; game 2 keyboard request
		btfsc STATUS,Z
			bsf Flags3,KEYB_SET_2
		IF LCD
		IF !LCD_DEBUG
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Init_Message_User_Lcd
		bcf PCLATH,3; page 0 or 2
		bsf Flags4,LCD_ENABLE; Authorizes Timer Management LCD
		ENDIF
		ENDIF
		bsf Flags2,JOY_EVENT
		bsf Flags,JOY_ENABLE
		bsf Flags,IKBD_ENABLE
Mouse_Enable
		bsf Flags,MOUSE_ENABLE
		goto Main
Not_Reset
		; ------------------------------------------------------------------------------
		; Status commands
		movf Value,W
		sublw IKBD_STATUS_MOUSE_BUT_ACTION; status
		btfss STATUS,Z
			goto Not_Mouse_Action
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_SET_MOUSE_BUTTON_ACTION
		call SerialTransmit_Host
		movf BUTTON_ACTION,W
		call SerialTransmit_Host
		movlw 5
		goto Send_Null_Bytes
Not_Mouse_Action
		movf Value,W
		sublw IKBD_STATUS_MOUSE_MODE_R
		btfsc STATUS,Z
			goto Mouse_Mode
		movf Value,W
		sublw IKBD_STATUS_MOUSE_MODE_A
		btfsc STATUS,Z
			goto Mouse_Mode
		movf Value,W
		sublw IKBD_STATUS_MOUSE_MODE_K
		btfss STATUS,Z
			goto Not_Mouse_Mode
Mouse_Mode
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		btfsc Flags2,MOUSE_KEYS
			goto Mouse_Mode_Keys
		btfsc Flags,MOUSE_ABS
			goto Mouse_Mode_Abs
		movlw IKBD_REL_MOUSE_POS_REPORT
		goto Send_End_Status_6_Null
Mouse_Mode_Abs
	 	movlw IKBD_ABS_MOUSE_POSITIONING
		call SerialTransmit_Host
		movf X_MAX_POSH,W; Absolute x position maximum mouse strong weight
		call SerialTransmit_Host; Xmsb, x maximum
		movf X_MAX_POSL,W; Absolute x position maximum mouse low weight
		call SerialTransmit_Host; Xlsb
		movf Y_MAX_POSH,W; Absolute position maximum mouse strong weight
		call SerialTransmit_Host; YMSB, Y Maximum
		movf Y_MAX_POSL,W; Absolute position maximum mice low weight
		call SerialTransmit_Host; Ylsb
		movlw 2
		goto Send_Null_Bytes		
Mouse_Mode_Keys
		movlw IKBD_SET_MOUSE_KEYCODE_CODE
		call SerialTransmit_Host
		movf DELTA_X,W; Deltax mode Keycode mouse ikbd
		call SerialTransmit_Host
		movf DELTA_Y,W; Deltay mode Keycode mouse ikbd
		goto Send_End_Status_4_Null
Not_Mouse_Mode
		movf Value,W
		sublw IKBD_STATUS_MOUSE_THRESHOLD
		btfss STATUS,Z
			goto Not_Threshold
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_SET_MOUSE_THRESHOLD
		call SerialTransmit_Host
		movlw 1
		call SerialTransmit_Host
		movlw 1
		goto Send_End_Status_4_Null
Not_Threshold
		movf Value,W
		sublw IKBD_STATUS_MOUSE_SCALE
		btfss STATUS,Z
			goto Not_Scale
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_SET_MOUSE_SCALE
		call SerialTransmit_Host
		movf X_SCALE,W
		call SerialTransmit_Host
		movf Y_SCALE,W
Send_End_Status_4_Null
		call SerialTransmit_Host
		movlw 4
		goto Send_Null_Bytes
Not_Scale
		movf Value,W
		sublw IKBD_STATUS_MOUSE_Y0_AT_B
		btfsc STATUS,Z
			goto Mouse_Vert
		movf Value,W
		sublw IKBD_STATUS_MOUSE_Y0_AT_T
		btfss STATUS,Z
			goto Not_Mouse_Vert
Mouse_Vert
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_SET_Y0_AT_BOTTOM
		btfss Flags,SIGN_Y
			movlw IKBD_SET_Y0_AT_TOP
		goto Send_End_Status_6_Null
Not_Mouse_Vert
		movf Value,W
		sublw IKDB_STATUS_DISABLE_MOUSE
		btfss STATUS,Z
			goto Not_Mouse_Enable
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		clrw; Enabled
		btfss Flags,MOUSE_ENABLE
			movlw IKDB_DISABLE_MOUSE
		goto Send_End_Status_6_Null
Not_Mouse_Enable
		movf Value,W
		sublw IKBD_STATUS_JOY_MODE_E
		btfsc STATUS,Z
			goto Joy_Mode
		movf Value,W
		sublw IKBD_STATUS_JOY_MODE_I
		btfsc STATUS,Z
			goto Joy_Mode
		movf Value,W
		sublw IKBD_STATUS_JOY_MODE_K
		btfss STATUS,Z
			goto Not_Joy_Mode
Joy_Mode	
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		btfsc Flags2,JOY_KEYS
			goto Joy_Mode_Keys
		movlw IKBD_SET_JOY_EVNT_REPORT
		btfss Flags2,JOY_EVENT
			movlw IKBD_SET_JOY_INTERROG_MODE
		goto Send_End_Status_6_Null		
Joy_Mode_Keys	
		movlw IKBD_SET_JOY_KEYCODE_MODE
		call SerialTransmit_Host
		movf RX_JOY,W
		call SerialTransmit_Host; Rx
		movf RY_JOY,W
		call SerialTransmit_Host; Ry
		movf TX_JOY,W
		call SerialTransmit_Host; Tx
		movf TY_JOY,W
		call SerialTransmit_Host; Ty
		movf VX_JOY,W
		call SerialTransmit_Host; Vx
		movf VY_JOY,W
		call SerialTransmit_Host; Vy
		goto Main
Not_Joy_Mode
		movf Value,W
		sublw IKDB_STATUS_DISABLE_JOY
		btfss STATUS,Z
			goto Main
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		clrw; Enabled
		btfss Flags,JOY_ENABLE
			movlw IKDB_DISABLE_JOYSTICKS
Send_End_Status_6_Null
		call SerialTransmit_Host
		movlw 6
Send_Null_Bytes
		call TransmitNullBytes; end status
		goto Main

; -----------------------------------------------------------------
; Keyboard errors
; -----------------------------------------------------------------

Error_Keyboard
		call Remove_Key_Forced
		call cmdResetKey
		goto Main

Error_Parity_Keyboard
		call cmdResendKey
		goto Main

; -----------------------------------------------------------------
; Data from the keyboard
; -----------------------------------------------------------------

doKeyboard
		; Prohibit all emissions from peripherals PS/2 (Mouse and Keyboard)
		DISABLEKEYB
		btfss PARITY,7
			goto Error_Parity_Keyboard; Break error
		; Is this the byte of the Bat Tipboard order?
		movf Value,W
		sublw CMD_BAT; Self-test passed (Keyboard Controller Init)
		btfsc STATUS,Z; Wuts $ aa?
			goto OnKbBAT
		movf Value,W
		sublw BAT_ERROR; Keyboard error
		btfsc STATUS,Z
			goto Error_Keyboard
		movf Value,W
		sublw ACK_ERROR; Keyboard error
		btfsc STATUS,Z
			goto Error_Keyboard

		; No, other byte, we continue according to the mode
		; PS/2 Direct or Atari Tranlation

		IF SERIAL_DEBUG
		btfsc PORTB,JUMPER4; If Jumper4 (+5V): Atari mode
			goto ModeKeyAtari
; -----------------------------------------------------------------
; PC mode: no translation, direct sending of scan-codes in text
; ASCII (BCD) or binaries directly

		bcf PORTB,LEDGREEN; LED keyboard lighter
		movf Value,W; keyboard code
		call SerialTransmit_Host
		bsf PORTB,LEDGREEN; Eteint LED Keyboard
		goto Main
		ENDIF
		
; -----------------------------------------------------------------
; Atari mode: PC translate -> Atari Scancodes

ModeKeyAtari
		btfss Flags3,KEYB_SET_2
			goto Set3
		btfsc Flags3,PAUSE_SET_2
			goto TraiterPause
		movf Value,W
		sublw ESCAPE                    ; does w contain $ e0?
		btfss STATUS,Z                  ; Yes, pass
			goto TestPauseSet2          ; SPECIAL CASE PAUSE 2
		bsf Flags3,NEXT_CODE            ; Next Code Process
		IF LCD
		IF !LCD_DEBUG
		btfss Flags4,LCD_ENABLE         ; LCD Enabled?
			goto Main
		movlw 0x8E                      ; line 1, column 15
		bsf PCLATH,3                    ; Page 1 gold 3 (0x800 - 0xfff)
		call Send_Value_Lcd
		bcf PCLATH,3                    ; page 0 2
		ENDIF
		ENDIF
                goto Main
TestPauseSet2
		movf Value,W
		sublw ESCAPE1                   ; does w contain $ e1?
		btfss STATUS,Z                  ; Yes, pass
			goto Set3
		bsf Flags3,NEXT_CODES           ; Next Codes Process
		bsf Flags3,PAUSE_SET_2
		goto Main
TraiterPause
		btfsc Flags3,NEXT_CODES
			goto TraiterPause2
		movf Value,W
		sublw CMD_RELEASED              ; Dues w contain $ f0?
		btfsc STATUS,Z                  ; Yes, pass
			goto BreakCode
		bcf Flags3,NEXT_CODES
		bcf Flags2,BREAK_CODE           ; Key Pressed by Default
		bsf Flags3,NEXT_CODE            ; Next Code Process
                goto Main
TraiterPause2
		movf Value,W
		sublw CMD_RELEASED              ; Dues w contain $ f0?
		btfsc STATUS,Z                  ; Yes, pass
			goto BreakCode
		movf Value,W
		sublw 0x77                      ; does w contain $ 77?
        btfss STATUS,Z                  ; Yes, pass
               	goto Not_SendKey
		btfsc Flags2,JOY_MONITOR
			goto Not_SendPause
		bcf PORTB,LEDGREEN              ; Keyboard led on
            	movlw DEF_PAUSE         ; Key Pressed
		btfsc Flags2,BREAK_CODE
            iorlw 0x80                  ; Released (Breakcode bit7 = 1)
		call SerialTransmit_Host
		bsf PORTB,LEDGREEN              ; Keyb LED OFF
Not_SendPause
		bcf Flags3,PAUSE_SET_2
		goto Not_SendKey
Set3
		movf Value,W
		sublw CMD_RELEASED              ; Dues w contain $ f0?
		btfss STATUS,Z                  ; Yes, pass
			goto Not_BreakCode
BreakCode
		bsf Flags2,BREAK_CODE           ; Process Break Code (Key Release)
            goto Main
Not_BreakCode
		btfsc Flags2,JOY_MONITOR
			goto Not_SendKey
		IF LCD
		IF !LCD_DEBUG
		btfss Flags4,LCD_ENABLE         ; LCD Enabled?
			goto ByPass_Lcd
		movlw 0xCE                      ; line 2, column 15
		bsf PCLATH,3                    ; Page 1 gold 3 (0x800 - 0xfff)
		call Send_Value_Lcd
		bcf PCLATH,3                    ; page 0 2
ByPass_Lcd
		ENDIF
		ENDIF
		bcf PORTB,LEDGREEN              ; Keyboard led on
		call SendAtariKeyboard
		bsf PORTB,LEDGREEN              ; Keyb LED OFF
Not_SendKey
		bcf Flags2,BREAK_CODE           ; Key Pressed by Default
		bcf Flags3,NEXT_CODE            ; FLAG 2nd Code after 0X0
		goto Main

; -----------------------------------------------------------------
; Receive beats Keyboard Command
; -----------------------------------------------------------------

OnKbBAT
		call KbBAT
		bsf Status_Boot,POWERUP_KEYB
		goto Main

KbBAT
; After Reset.
; Delay: 500 ms, spleen; 10.9 CPS, set 2, Typematic/Make/Break
		call cmdScanSet
		btfss Flags3,KEYB_SET_2
			call cmdMakeBreak
; BTFSC flags3, keyb_set_2
; Call cmdtyperate
		movlw CAPS_OFF      ; Enable Num-Lock, Disable Caps-Lock
		call cmdLedOnOff
		call cmdEnableKey
		bsf PORTB,LEDGREEN  ; Disable green led to show onbat
		return

; -----------------------------------------------------------------
; Mouse Errors
; -----------------------------------------------------------------

Error_Mouse
		call cmdResetMouse
		goto Main
		
Error_Parity_Mouse
		IF SCROOL_LOCK_ERR
		call UpdateLedOnOff
		ENDIF
		call cmdResendMouse
		clrf Counter_Mouse; Resend Whole Frame
		goto Main

; -----------------------------------------------------------------
; Date arrived from the mouse
; -----------------------------------------------------------------
		
doMouse
		; Prohibit all emissions from peripherals PS/2 (Mouse and Keyboard)
		DISABLEMOUSE
		btfss PARITY,7
			goto Error_Parity_Mouse
		movf Counter_Mouse,W
		btfss STATUS,Z
			goto Not_BatMouse; <> 1st byte
		; Is this the byte of the Bat Souris command?
		movf Value,W
		sublw CMD_BAT; Self-test passed
		btfsc STATUS,Z; Wuts $ aa?
			goto OnMsBAT
		movf Value,W
		sublw BAT_ERROR; Mouse error
		btfsc STATUS,Z
			goto Error_Mouse
		movf Value,W
		sublw ACK_ERROR; Mouse error
		btfsc STATUS,Z
			goto Error_Mouse
Not_BatMouse
		IF SERIAL_DEBUG
		btfsc PORTB,JUMPER4; If Jumper4 (+5V): Atari mode
			goto ModeMsAtari
; -----------------------------------------------------------------
; PC mouse mode: Directly sends codes in ASCII (BCD) text codes or
; binary

		bcf PORTB,LEDYELLOW; Led mouse lighter
		movf Value,W
		call SerialTransmit_Host
		bsf PORTB,LEDYELLOW; Eteint Led mouse
		goto Main
		ENDIF

; -----------------------------------------------------------------
; Atari mouse mode: we convert the byte packets into a Atari frame

ModeMsAtari
		incf Counter_Mouse,F; counter to treat each byte received via the main loop (hand)
		movf Counter_Mouse,W
		sublw 1
		btfsc STATUS,Z
			goto First_Byte; 1st byte
		movf Counter_Mouse,W
		sublw 2
		btfsc STATUS,Z
			goto Second_Byte; 2nd byte (x)
		movf Counter_Mouse,W
		sublw 3
		btfsc STATUS,Z
			goto Third_Byte; 3rd byte (y)
		goto Fourth_Byte; 4th Event byte (Z)
First_Byte		
		movf Value,W
		movwf Value_0; Value Value_0 test
		btfsc Value_0,7
			goto Pb_Trame_Mouse; Y Overflow
		btfsc Value_0,6
			goto Pb_Trame_Mouse; X overflow
		btfsc Value_0,3
			goto Main
		; bit 3 must always be 1
Pb_Trame_Mouse
		clrf Counter_Mouse; Drag in reading the frame
		goto Main
Second_Byte
		movf Value,W
		movwf Value_X; Value x Movement
		goto Main
Third_Byte		
		movf Value,W
		movwf Value_Y; Value Y Movement
		btfss Flags,MSWHEEL
			goto Normal_Mouse
		clrf Value_Z
		goto Main
Fourth_Byte
		movf Value,W
		movwf Value_Z; Value Z Mouvement (-8 to 7)
Normal_Mouse
		clrf Counter_Mouse
		clrf BUTTONS; mouse buttons
		btfsc Value_0,PC_LEFT; Test if left button smile at 1
			bsf BUTTONS,AT_LEFT; If 1 put the one atri to 1
		btfsc Value_0,PC_RIGHT; Test if right mouse at 1
			bsf BUTTONS,AT_RIGHT; If 1 Put the one at 1
		movf BUTTONS,W
		iorlw HEADER_RELATIVE_MOUSE; Header mouse relative mode (0xf8 - 0xfb)
		movwf HEADER_IKBD		

		; ----------------------------------------------------------------------------
		; Instead of simulating right and left support, we generate a scan-code
                ; we write 0 so as not to transmit then
		clrf CLIC_MOUSE_WHEEL; button 3 key
		clrf KEY_MOUSE_WHEEL; button roulette key 3
		clrf KEY2_MOUSE_WHEEL; 4 & 5 button button
		btfss Value_0,PC_MIDDLE; Test if central button mouse at 1
			goto Compteurs_Mouse
		movlw Tab_Mouse-EEProm+IDX_BUTTON3
		call Lecture_EEProm
		movwf CLIC_MOUSE_WHEEL
		; ----------------------------------------------------------------------------
Compteurs_Mouse
		; ----------------------------------------------------------------------------
		; Treat the X counter
		bcf Value_X,AT_SIGN; Bring the value back to 7 bits
		btfsc Value_0,PC_SIGNX; Test the PC sign bit for x
			bsf Value_X,AT_SIGN; is raised so we raise the Atari sign
		movf Value_X,W
		movwf X_MOV; Write the result in the X Atari counter
		btfss Flags,MOUSE_ABS
			goto Not_Max_X; <> Absolute fashion
		clrf TEMP2; positive
		btfsc Value_X,7
			comf TEMP2,F; negative
		addwf X_POSL,F; ABSOLUE SOUS POSEMENT SOUS Low weight
		movf TEMP2,W
		btfsc STATUS,C
			incfsz TEMP2,W
		addwf X_POSH,F; X_pos = x_pos + x_mov
		btfsc X_POSH,7; Absolute mouse position x
			goto Dep_Min_X; If x_pos <0 -> x_pos = 0
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Conv_Scale_X
		bcf PCLATH,3; page 0 or 2
		movf X_MAX_POSH,W; Absolute x position maximum mouse strong weight
		subwf X_POSH_SCALED,W; absolute mouse x position with strong weight factor
		btfsc STATUS,Z
			goto XCompl
		btfss STATUS,C
			goto Not_Max_X; X_pos <x_max_pos
		goto Dep_Max_X; X_pos> x_max_pos
XCompl		movf X_MAX_POSL,W; Absolute x position maximum mouse low weight
		subwf X_POSL_SCALED,W; absolute mouse x position with low weight factor factor
		btfsc STATUS,Z
			goto Dep_Max_X; X_pos = x_max_pos
		btfss STATUS,C
			goto Not_Max_X; X_pos <x_max_pos
Dep_Max_X
		movf X_MAX_POSL,W
		movwf X_POSL_SCALED
		movf X_MAX_POSH,W
		movwf X_POSH_SCALED; If x_pos> = x_max_pos -> x_pos = x_max_pos
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Conv_Inv_Scale_X
		bcf PCLATH,3; page 0 or 2
		goto Not_Max_X
Dep_Min_X
		call Init_X_Abs
Not_Max_X
		; ----------------------------------------------------------------------------
		; Treat the meter y
		bcf Value_Y,AT_SIGN; Bring the value back to 7 bits
		btfsc Value_0,PC_SIGNY; Test the PC sign bit for y
			bsf Value_Y,AT_SIGN; is raised so we raise the Atari sign
                ; The PS/2 movement and Atari is opposite by default
		btfsc Flags,SIGN_Y
			goto Not_Inv_Y; Reversion of meaning y ikbd
		; Completely has two to reverse the Y movement: we only treat +127/-128
		; Ex: $ ff %11111111 D-1 => $ 01 %00000001 D+1
		comf Value_Y,F
		incf Value_Y,F
Not_Inv_Y
		movf Value_Y,W
		movwf Y_MOV; Write the result in the Y -Etari counter
		btfss Flags,MOUSE_ABS
			goto Not_Max_Y; <> Absolute fashion
		clrf TEMP2; positive
		btfsc Value_Y,7
			comf TEMP2,F; negative
		addwf Y_POSL,F; ABSOLUE MOUSE POST MOUSE Low weight
		movf TEMP2,W
		btfsc STATUS,C
			incfsz TEMP2,W
		addwf Y_POSH,F; Y_pos = y_pos + y_mov
		btfsc Y_POSH,7; Absolute mouse position
			goto Dep_Min_Y; If y_pos <0 -> y_pos = 0
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Conv_Scale_Y
		bcf PCLATH,3; page 0 or 2
		movf Y_MAX_POSH,W; Absolute position maximum mouse strong weight
		subwf Y_POSH_SCALED,W; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
		btfsc STATUS,Z
			goto YCompl
		btfss STATUS,C
			goto Not_Max_Y; Y_pos <y_max_pos
		goto Dep_Max_Y; Y_pos> y_max_pos
YCompl
		movf Y_MAX_POSL,W; Absolute position maximum mice low weight
		subwf Y_POSL_SCALED,W; Absolute mouse position with low weight factor factor
		btfsc STATUS,Z
			goto Dep_Max_Y; X_pos = y_max_pos
		btfss STATUS,C
			goto Not_Max_Y; Y_pos <y_max_pos
Dep_Max_Y
		movf Y_MAX_POSL,W
		movwf Y_POSL_SCALED
		movf Y_MAX_POSH,W
		movwf Y_POSH_SCALED; If y_pos> = y_max_pos -> y_pos = y_max_pos
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Conv_Inv_Scale_Y
		bcf PCLATH,3; page 0 or 2
		goto Not_Max_Y
Dep_Min_Y
		call Init_Y_Abs
Not_Max_Y
		; -------------------------------------------------------------------------------------------------------
		; We receive the Z counter, if it's a roulette mouse
		btfss Flags,MSWHEEL
			goto TransmitHost; If bit mouse with roulette in zero we now transmit
		; Case of the Intelimouse, treat the Z counter
		btfsc Value_Z,PC_SIGNZ; Is this a negative value
			goto ZNegatif
		; Testing whether the value on the 4bits [3-0] is equal to 0 or not
		movf Value_Z,W; Load w with the value
		andlw 0x0F; Hide and treat the Z counter on 4 bits
		btfsc STATUS,Z
			goto ZZero
		; ----------------------------------------------------------------------------
		; Positive value: vertical wheel downwards
		; or horizontal wheel to the right
		btfss Value_Z,PC_BITLSBZ; If the bit 0 is 1 we therefore 0x1 => vertical wheel
			goto ZGauche; go to a horizontal wheel
		movlw Tab_Mouse-EEProm+IDX_WHEELDN
		call Lecture_EEProm
		movwf KEY_MOUSE_WHEEL; positive value therefore at the bottom any
		goto ZZero
ZGauche
		movlw Tab_Mouse-EEProm+IDX_WHEELLT
		call Lecture_EEProm
		movwf KEY_MOUSE_WHEEL; positive value therefore on the left all
		goto ZZero
ZNegatif
		; ----------------------------------------------------------------------------
		; Negative value: vertical wheel upwards
		; or horizontal wheel to the left
		btfss Value_Z,PC_BITLSBZ; If the bit 0 is 1 we therefore 0xf => vertical wheel
			goto ZDroit; go to a horizontal wheel
		movlw Tab_Mouse-EEProm+IDX_WHEELUP
		call Lecture_EEProm
		movwf KEY_MOUSE_WHEEL; Negative value -1 therefore above all
		goto ZZero
ZDroit
		movlw Tab_Mouse-EEProm+IDX_WHEELRT
		call Lecture_EEProm
		movwf KEY_MOUSE_WHEEL; Negative value -2 So on the right all
ZZero
		; ----------------------------------------------------------------------------
		; If the intelimouse and 5 buttons test the state
		btfss Flags,MSWHEELPLUS; If bit Wheelplus to Zero we transmit now, he has no
			goto TransmitHost; buttons 4 & 5 and 2nd dial
		btfss Value_Z,PC_BUTTON4; Active 4 button?
			goto Prochain_Bouton; Test the next button (5)
		movlw Tab_Mouse-EEProm+IDX_BUTTON4
		call Lecture_EEProm
		movwf KEY2_MOUSE_WHEEL; Scan-Code combines button 4
		goto TransmitHost; We transmit now
Prochain_Bouton
		btfss Value_Z,PC_BUTTON5; ; Active 5 button?
			goto TransmitHost; We transmit now
		movlw Tab_Mouse-EEProm+IDX_BUTTON5
		call Lecture_EEProm
		movwf KEY2_MOUSE_WHEEL; Scan-Code combines button 5
TransmitHost
		; ----------------------------------------------------------------------------
		; Finally send the mouse package to host
		btfss Flags,MOUSE_ENABLE
			goto Main; No authorization from Atari
		btfsc Flags2,JOY_MONITOR
			goto Main; Monitoring Joysticks in progress requests by Atari
		bcf PORTB,LEDYELLOW; Led mouse lighter
		call SendAtariMouse; relative
		; ----------------------------------------------------------------------------
		; IKBD Button Action mode test
		btfss BUTTON_ACTION,0; Struck button => sending absolute position
			goto Not_Action_0
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS,W; Former state of mouse buttons
		andlw 1		
		btfsc STATUS,Z
			goto Not_Change_A
		btfsc BUTTONS,0
			goto Env_Action; right button
Not_Change_A
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS,W; Former state of mouse buttons
		andlw 2		
		btfsc STATUS,Z
			goto Not_Action_0
		btfsc BUTTONS,1
			goto Env_Action; Left button drives
Not_Action_0
		btfss BUTTON_ACTION,1; Refshor button => sending absolute position
			goto Not_Action_1	
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS,W; Former state of mouse buttons
		andlw 1		
		btfsc STATUS,Z
			goto Not_Change_B
		btfss BUTTONS,0
			goto Env_Action; Right drop -down button
Not_Change_B
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS,W; Former state of mouse buttons
		andlw 2		
		btfsc STATUS,Z
			goto Not_Action_1
		btfsc BUTTONS,1; Left button releases
			goto Not_Action_1
Env_Action
		movf BUTTONS,W; mouse buttons
		movwf OLD_BUTTONS_ABS; Old state of mouse buttons in absolute mode
		comf OLD_BUTTONS_ABS,F; force the sending of state changes
		movlw HEADER_ABSOLUTE_MOUSE; Header mouse absolute fashion
		movwf HEADER_IKBD
		call SendAtariMouse
Not_Action_1
		bsf PORTB,LEDYELLOW; Eteint Led mouse
		movf BUTTONS,W; mouse buttons
		movwf OLD_BUTTONS; Former state of mouse buttons
		goto Main

; -----------------------------------------------------------------
; Reception of the Bat Souris command
; -----------------------------------------------------------------

OnMsBAT
		call TryWheel; Detect if the mouse is with roulette
		btfsc Flags,MSWHEEL; If it's a roulette mouse
			call TryWheelPlus; Detect if is even more
		call cmdEnableMous
		bsf PORTB,LEDYELLOW; Turn off the yellow LED to show onbat
		bsf Status_Boot,POWERUP_MOUSE
		goto Main
		
; ====================================================================================================================================
; Procedures and functions
; ====================================================================================================================================

; -----------------------------------------------------------------
; Forcing 1 if 0
; -----------------------------------------------------------------
		
Change_0_To_1
		iorlw 0
		btfsc STATUS,Z
			movlw 1
		return
		
; -----------------------------------------------------------------
; Discount to 0 variables
; -----------------------------------------------------------------
		
Init_X_Y_Abs
		call Init_X_Abs

Init_Y_Abs
		clrf Y_POSL
		clrf Y_POSH
		clrf Y_POSL_SCALED; Absolute mouse position with low weight factor factor
		clrf Y_POSH_SCALED; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
		return
		
Init_X_Abs	
		clrf X_POSL
		clrf X_POSH
		clrf X_POSL_SCALED; absolute mouse x position with low weight factor factor
		clrf X_POSH_SCALED; absolute mouse x position with strong weight factor
		return

; -----------------------------------------------------------------
; Byte transmission procedure About the clock
; -----------------------------------------------------------------
		
SendAtariClock
		movlw HEADER_TIME_OF_DAY; Header Clock
		call SerialTransmit_Host
		movlw LOW YEAR_BCD
		movwf FSR; point on the 1st element sent that is to say the year
		movlw 6; 6 bytes
		movwf Counter
Loop_Env_Time
			bsf STATUS,IRP; Page 2
			movf INDF,W
			call SerialTransmit_Host; YY, MM, DD, HH, MM, SS
			incf FSR,F
			decfsz Counter,F
		goto Loop_Env_Time; following element
		return

; -----------------------------------------------------------------
; Byte transmission procedure About (or) joystick (s)
; -----------------------------------------------------------------
		
SendAtariJoysticks
; -----------------------------------------------------------------
; Begin Fake Mouse Data (Right Mouse Button)-Added by MQ (2023-10-07)
		btfss Flags,MOUSE_ENABLE
			goto Not_Fake_Mouse_Data; No authorization from Atari
		movlw 0xF9; Joy1 Fire Pressed
		btfss JOY1,4
			movlw 0xF8; Joy1 Fire Released
		call SerialTransmit_Host
		movlw 0
		call SerialTransmit_Host
		movlw 0
		call SerialTransmit_Host
Not_Fake_Mouse_Data
; End Fake Mouse Data
; -----------------------------------------------------------------

		movf HEADER_IKBD,W
		sublw HEADER_JOYSTICKS; Header Joysticks
		btfss STATUS,Z
			goto Not_Joy01
		; ----------------------------------------------------------------------------
		; Transmit the Atari package (3 bytes) of the joysticks
		movf HEADER_IKBD,W
		call SerialTransmit_Host
	 	movf JOY0,W; Joystick 0 reading 0
		call SerialTransmit_Host_Joy
	 	movf JOY1,W; Reading Joystick 1
		goto SerialTransmit_Host_Joy
Not_Joy01
		movf HEADER_IKBD,W
		sublw HEADER_JOYSTICK0; Header Joystick 0
		btfss STATUS,Z
			goto Not_Joy0
		btfss Flags2,JOY_KEYS; Shipping items
			goto Not_ButAct0
		; ----------------------------------------------------------------------------
		; Transmit Button Action Joystick 0
		movlw 0x74; Joystick button 0 drives
		btfss JOY0,4
			movlw 0xF4; JYYSTICK button 0 RAPCH
		goto SerialTransmit_Host
Not_ButAct0
		; ----------------------------------------------------------------------------
		; Transmit the standard Atari (2 bytes) package of the JYYSTICK 0
		movf HEADER_IKBD,W
		call SerialTransmit_Host
		movf JOY0,W; Joystick 0 reading 0
		goto SerialTransmit_Host_Joy
Not_Joy0
		btfss Flags2,JOY_KEYS; Shipping items
			goto Not_ButAct1
		; ----------------------------------------------------------------------------
		; Transmit Button Action Joystick 1
		movlw 0x75; Joystick button 1 Punish
		btfss JOY1,4
			movlw 0xF5; Joystick button 1 release
		goto SerialTransmit_Host
Not_ButAct1
		; ----------------------------------------------------------------------------
		; Transmit the standard Atari (2 bytes) package of the joystick 1
		movf HEADER_IKBD,W
		call SerialTransmit_Host
		movf JOY1,W; Reading Joystick 1
SerialTransmit_Host_Joy
		; DEPLACTION OF THE JOYSTICK BIT FIRE (4 -> 7)
		movwf TEMP1
		btfsc TEMP1,4
			bsf TEMP1,7; Fire button
		bcf TEMP1,4
		movf TEMP1,W
		goto SerialTransmit_Host
		
; -----------------------------------------------------------------
; Bytes transmission procedure About Emulation
; Keyboard arrows via the joystick 0, ie the IKBD mode
; Keycodes joystick
; - Initial time RX/RY Decleche when supporting the direction
; scan-code of the keyboard is then sent a 1st time following
; The direction chosen on the JYYSTICK 0
; - Repetition time tx/ty (x times next vx/vy)
; - Total time VX/VY causing the stop of sending the scan-code of the
; keyboard arrow (ditto if relating to the chosen direction of the
; JOYSTICK 0)
; -----------------------------------------------------------------

SendAtariJoysticks_Fleches
		btfsc Status_Joy,RY_H
			goto Not_H
		movf JOYB,W; JYYSTICK 0
		xorwf OLD_JOY,W
		andlw 1		
		btfsc STATUS,Z
			goto Not_H; No change of state
		btfss JOYB,0
			goto Not_H
		bsf Status_Joy,RY_H; high button
		bcf Status_Joy,TY_H
		movf Counter_100MS_Joy,W
		movwf START_RY_JOY
		movf RY_JOY,W
		btfsc STATUS,Z
			goto Not_H; Ry time
		call SerialTransmit_Haut; 1st sending
Not_H
		btfsc Status_Joy,RY_B
			goto Not_B
		movf JOYB,W
		xorwf OLD_JOY,W
		andlw 2		
		btfsc STATUS,Z
			goto Not_B; No change of state
		btfss JOYB,1
			goto Not_B
		bsf Status_Joy,RY_B; Bas button drives
		bcf Status_Joy,TY_B
		movf Counter_100MS_Joy,W
		movwf START_RY_JOY
		movf RY_JOY,W
		btfsc STATUS,Z
			goto Not_B; Ry time
		call SerialTransmit_Bas; 1st sending
Not_B
		btfsc Status_Joy,RX_G
			goto Not_G
		movf JOYB,W
		xorwf OLD_JOY,W
		andlw 4		
		btfsc STATUS,Z
			goto Not_G; No change of state
		btfss JOYB,2
			goto Not_G
		bsf Status_Joy,RX_G; Left button drives
		bcf Status_Joy,TX_G
		movf Counter_100MS_Joy,W
		movwf START_RX_JOY
		movf RX_JOY,W
		btfsc STATUS,Z
			goto Not_G; time rx
		call SerialTransmit_Gauche; 1st sending
Not_G
		btfsc Status_Joy,RX_D
			goto Not_D
		movf JOYB,W
		xorwf OLD_JOY,W
		andlw 8		
		btfsc STATUS,Z
			goto Not_D; No change of state
		btfss JOYB,3
			goto Not_D
		bsf Status_Joy,RX_D; right button
		bcf Status_Joy,TX_D
		movf Counter_100MS_Joy,W
		movwf START_RX_JOY
		movf RX_JOY,W
		btfsc STATUS,Z
			goto Not_D; time rx
		call SerialTransmit_Droite; 1st sending
Not_D
		btfss Status_Joy,RY_H
			goto Not_H2
		btfss JOYB,0
			goto End_H; High release button
		movf START_RY_JOY,W
		subwf Counter_100MS_Joy,W
		subwf VY_JOY,W; VY - Time Lécoule
		btfsc STATUS,C
			goto Not_H3
End_H
		bcf Status_Joy,RY_H; Total end
		bcf Status_Joy,TY_H
		goto Not_H2
Not_H3
		btfsc Status_Joy,TY_H
			goto Not_H4; Repetition time
		movf START_RY_JOY,W
		subwf Counter_100MS_Joy,W
		subwf RY_JOY,W; Ry - Time Ecoule
		btfsc STATUS,C
			goto Not_H2; <> End 1st time
		goto Delay_TY_H
Not_H4
		movf START_TY_JOY,W
		subwf Counter_100MS_Joy,W
		subwf TY_JOY,W; Ty - Time Live
		btfsc STATUS,C
			goto Not_H2
Delay_TY_H
		bsf Status_Joy,RY_H
		bsf Status_Joy,TY_H
		movf Counter_100MS_Joy,W
		movwf START_TY_JOY
		call SerialTransmit_Haut; repetition
Not_H2
		btfss Status_Joy,RY_B
			goto Not_B2
		btfss JOYB,1
			goto End_B; Low drop button
		movf START_RY_JOY,W
		subwf Counter_100MS_Joy,W
		subwf VY_JOY,W; VY - Time Lécoule
		btfsc STATUS,C
			goto Not_B3
End_B
		bcf Status_Joy,RY_B; Total end
		bcf Status_Joy,TY_B
		goto Not_B2
Not_B3
		btfsc Status_Joy,TY_B
			goto Not_B4; Repetition time
		movf START_RY_JOY,W
		subwf Counter_100MS_Joy,W
		subwf RY_JOY,W; Ry - Time Ecoule
		btfsc STATUS,C
			goto Not_B2; <> End 1st time
		goto Delay_TY_B
Not_B4
		movf START_TY_JOY,W
		subwf Counter_100MS_Joy,W
		subwf TY_JOY,W; Ty - Time Live
		btfsc STATUS,C
			goto Not_B2
Delay_TY_B
		bsf Status_Joy,RY_B
		bsf Status_Joy,TY_B
		movf Counter_100MS_Joy,W
		movwf START_TY_JOY
		call SerialTransmit_Bas; repetition
Not_B2
		btfss Status_Joy,RX_G
			goto Not_G2
		btfss JOYB,2
			goto End_G; Left button releases
		movf START_RX_JOY,W
		subwf Counter_100MS_Joy,W
		subwf VX_JOY,W; VX - ECOULE TIME
		btfsc STATUS,C
			goto Not_G3
End_G
		bcf Status_Joy,RX_G; Total end
		bcf Status_Joy,TX_G
		goto Not_G2
Not_G3
		btfsc Status_Joy,TX_G
			goto Not_G4; Repetition time
		movf START_RX_JOY,W
		subwf Counter_100MS_Joy,W
		subwf RX_JOY,W; RX - ECOULE TIME
		btfsc STATUS,C
			goto Not_G2; <> End 1st time
		goto Delay_TX_G
Not_G4
		movf START_TX_JOY,W
		subwf Counter_100MS_Joy,W
		subwf TX_JOY,W; TX - ECOULE TIME
		btfsc STATUS,C
			goto Not_G2
Delay_TX_G
		bsf Status_Joy,RX_G
		bsf Status_Joy,TX_G
		movf Counter_100MS_Joy,W
		movwf START_TX_JOY
		call SerialTransmit_Gauche; repetition
Not_G2
		btfss Status_Joy,RX_D
			goto Not_D2
		btfss JOYB,3
			goto End_D; Right drop -down button
		movf START_RX_JOY,W
		subwf Counter_100MS_Joy,W
		subwf VX_JOY,W; VX - ECOULE TIME
		btfsc STATUS,C
			goto Not_D3
End_D
		bcf Status_Joy,RX_D; Total end
		bcf Status_Joy,TX_D
		goto Not_D2
Not_D3
		btfsc Status_Joy,TX_D
			goto Not_D4; Repetition time
		movf START_RX_JOY,W
		subwf Counter_100MS_Joy,W
		subwf RX_JOY,W; RX - ECOULE TIME
		btfsc STATUS,C
			goto Not_D2; <> End 1st time
		goto Delay_TX_D
Not_D4
		movf START_TX_JOY,W
		subwf Counter_100MS_Joy,W
		subwf TX_JOY,W; TX - ECOULE TIME
		btfsc STATUS,C
			goto Not_D2
Delay_TX_D
		bsf Status_Joy,RX_D
		bsf Status_Joy,TX_D
		movf Counter_100MS_Joy,W
		movwf START_TX_JOY
		call SerialTransmit_Droite; repetition
Not_D2
		movf JOYB,W
		movwf OLD_JOY	
		return

; -----------------------------------------------------------------
; Bytes transmission procedure About the mouse. We send
; The standard Atari mouse package, out of 3 bytes and then if the bytes are no
; null, we send two supplements which contain scan-codes for
; The 4 & 5 plums and buttons.
; 
; ENTRES: HEADER, X_MOV, Y_MOV in IKBD MOUND
; Header, buttons, x_posh, x_posl, y_posh, y_posl absolutely
; Outings: w Destruit
; -----------------------------------------------------------------

SendAtariMouse
		movf HEADER_IKBD,W
		sublw HEADER_ABSOLUTE_MOUSE; Header mouse absolute fashion
		btfsc STATUS,Z
			goto SendAtariMouse_Abs; Absolute fashion
		btfsc Flags,MOUSE_ABS
			goto Not_trame_Rel; Mouse in absolute mode
		btfsc Flags2,MOUSE_KEYS; Shipping items
			goto Not_trame_Rel
		; ----------------------------------------------------------------------------
		; Transmit the standard Atari mouse package (3 bytes) Relative mode
		movf HEADER_IKBD,W
		call SerialTransmit_Host; Send the byte order to the host
		movf X_MOV,W; Relative DEPLACEMENT MOUSE IN X
		call SerialTransmit_Host; Send the counter X to the host
		movf Y_MOV,W; Deplacement Relative mouse in Y
		call SerialTransmit_Host; Send the byte counter y to the host
		; ----------------------------------------------------------------------------
Not_trame_Rel                      
		; ----------------------------------------------------------------------------
		; Ikbd Mouse Keycodes fashion test
		btfss Flags2,MOUSE_KEYS; Shipping items
			goto Not_Keys_Mouse
		movf X_MOV,W; Relative DEPLACEMENT MOUSE IN X
		addwf X_INC_KEY,F; Keycode mouse in x mode
		btfsc X_INC_KEY,7
			goto Loop_Moins_H
Loop_Plus_H
			movf DELTA_X,W; Deltax mode Keycode mouse ikbd
			subwf X_INC_KEY,W
			btfsc STATUS,C
				goto Not_Key_Mouse_H
			movwf X_INC_KEY
			call SerialTransmit_Droite
		goto Loop_Plus_H	
Loop_Moins_H
			movf DELTA_X,W; Deltax mode Keycode mouse ikbd
			addwf X_INC_KEY,W
			btfsc STATUS,C
				goto Not_Key_Mouse_H
			movwf X_INC_KEY
			call SerialTransmit_Gauche
		goto Loop_Moins_H	
Not_Key_Mouse_H
		movf Y_MOV,W; Deplacement Relative mouse in Y
		addwf Y_INC_KEY,F; Interect in Yeycode Mouse mode
		btfsc Y_INC_KEY,7
			goto Loop_Moins_V
Loop_Plus_V
			movf DELTA_Y,W; Deltay mode Keycode mouse ikbd
			subwf Y_INC_KEY,W
			btfsc STATUS,C
				goto Not_Keys_Mouse
			movwf Y_INC_KEY
			btfss Flags,SIGN_Y	
				call SerialTransmit_Bas
			btfsc Flags,SIGN_Y	
				call SerialTransmit_Haut
		goto Loop_Plus_V	
Loop_Moins_V
			movf DELTA_Y,W; Deltay mode Keycode mouse ikbd
			addwf Y_INC_KEY,W
			btfsc STATUS,C
				goto Not_Keys_Mouse
			movwf Y_INC_KEY
			btfss Flags,SIGN_Y	
				call SerialTransmit_Haut
			btfsc Flags,SIGN_Y	
				call SerialTransmit_Bas
		goto Loop_Moins_V
Not_Keys_Mouse
		; ----------------------------------------------------------------------------
		; IKBD Button Action mode test
		btfss BUTTON_ACTION,2; Sending buttons
			goto Not_Clic_Mouse
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS,W; Former state of mouse buttons
		andlw 1		
		btfsc STATUS,Z
			goto Not_Change_C
		movlw 0x75; right button
		btfss BUTTONS,0
			movlw 0xF5; Right drop -down button
		call SerialTransmit_Host
Not_Change_C
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS,W; Former state of mouse buttons
		andlw 2		
		btfsc STATUS,Z
			goto Not_Clic_Mouse
		movlw 0x74; Left button drives
		btfss BUTTONS,1
			movlw 0xF4; Left button releases
		call SerialTransmit_Host
Not_Clic_Mouse
		; ----------------------------------------------------------------------------
		; Transmit the scan code for the central button (if it is non-zero)
		movf CLIC_MOUSE_WHEEL,W; Test the evidence to transmit
		btfsc STATUS,Z
			goto Roulettes
		; Effective transmission: sending the scan-code for the central button
		call SerialTransmit_KeyUpDown; send the wheel
Roulettes
		; ----------------------------------------------------------------------------
		; Transmit the scan code of knobs (if it is not zero)
		movlw Tab_Mouse-EEProm+IDX_WHREPEAT
		call Lecture_EEProm; Number Repetitions Mouse Wheel Touche
		movwf Counter
Repeat
			movf KEY_MOUSE_WHEEL,W ; Test the evidence to transmit
			btfsc STATUS,Z
				goto Buttons45
			; Effective transmission: sending the scan-code for the wheel
			call SerialTransmit_KeyUpDown; send the wheel
			decfsz Counter,F; repeat the show
		goto Repeat
Buttons45
		; ----------------------------------------------------------------------------
		; Transmit the 4 & 5 buttons code (if it is non-zero)
		movf KEY2_MOUSE_WHEEL,W; ; Test the evidence to transmit
		btfsc STATUS,Z
			return
		goto SerialTransmit_KeyUpDown; Send the button 4 & 5
		; ----------------------------------------------------------------------------
SendAtariMouse_Abs
		; ----------------------------------------------------------------------------
		; Transmit the Atari mouse package (5 bytes) in absolute mode
		movf HEADER_IKBD,W
		call SerialTransmit_Host
		clrf TEMP1
		movf BUTTONS,W; mouse buttons
		; Right button change test test
		xorwf OLD_BUTTONS_ABS,W; Old state of mouse buttons in absolute mode
		andlw 1		
		btfsc STATUS,Z
			goto Not_Change_0
		btfsc BUTTONS,0
			bsf TEMP1,0; right button
		btfss BUTTONS,0
			bsf TEMP1,1; Right drop -down button
Not_Change_0
		; Test change of state of the left button
		movf BUTTONS,W; mouse buttons
		xorwf OLD_BUTTONS_ABS,W; Old state of mouse buttons in absolute mode
		andlw 2		
		btfsc STATUS,Z
			goto Not_Change_1
		btfsc BUTTONS,1
			bsf TEMP1,2; Left button drives
		btfss BUTTONS,1
			bsf TEMP1,3; Left button releases
Not_Change_1
		; Sending the Ikbd Mouse Mouse Fashion
		movf BUTTONS,W; mouse buttons
		movwf OLD_BUTTONS_ABS; Old state of mouse buttons in absolute mode
		movf TEMP1,W
		call SerialTransmit_Host; Buttercase changes: B3/2: Left releases/pushing, B1/0: Droit releases/Think
		movf X_POSH_SCALED,W; absolute mouse x position with strong weight factor
		call SerialTransmit_Host; Xsmb
		movf X_POSL_SCALED,W; absolute mouse x position with low weight factor factor
		call SerialTransmit_Host; Xlsb
		movf Y_POSH_SCALED,W; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
		call SerialTransmit_Host; Ymsb
		movf Y_POSL_SCALED,W; Absolute mouse position with low weight factor factor
		goto SerialTransmit_Host; Ylsb
	
; -----------------------------------------------------------------
; Detection of a simple intelimouse mouse. We try the detection and we
; Position the MSWHEEL bit has such a roulette.
; 
; METHOD: Emit Sample Sample Rate 200
; Emit Sample Sample Rate 100
; Emit Sample Sample Rate 80
; 
; Emitting Get Device ID
; If returns 0x03 => mouse extended to roulette
; Otherwise return 0x00 => PS/2 Standard mouse
; -----------------------------------------------------------------

TryWheel
		; Send the parametering sequence to detect the type
		movlw 200
		call cmdSetSmpRate
		movlw 100
		call cmdSetSmpRate
		movlw 80
		call cmdSetSmpRate; Enter the Scrolling Wheel mode
		; Send the order for identification
		movlw GET_DEVICE_ID
		call MPSCmd
		; Wait now the mouse identification answer
		call MPSGet; Wait and Get Next byte
		; Analyze the return byte: 0x00 = PS/ 2 Standard/ 0x03 Intelimouse 3 buttons
		bcf Flags,MSWHEEL
		movf Value,W
		sublw MS_WHEEL; Subtract the Intelimouse code 3 buttons
		btfsc STATUS,Z
			bsf Flags,MSWHEEL; MS Scrolling Mouse
		; Just for Test: If BCD display Show the ID code of the mouse
		IF SERIAL_DEBUG
		movf Value,W
		btfss PORTB,JUMPER5; If Debug Mode
			call SerialTransmit_Host
		ENDIF
		movlw 100; Return the initial sampling rate
		goto cmdSetSmpRate; Leaving the mode

; -----------------------------------------------------------------
; Detection of an intentional intelimouse mouse, that is to say 5 buttons and
; Possibly with the second roulette. We try the detection and we position
; The Bit Mswheelplus has such a roulette.
; 
; METHOD: Emit Sample Sample Rate 200
; Emit Sample Sample 200
; Emit Sample Sample Rate 80
; 
; Emitting Get Device ID
; If returns 0x04 => mouse extended to roulette
; Otherwise return 0x00 => PS/2 Standard mouse
; -----------------------------------------------------------------

TryWheelPlus
		; Send the parametering sequence to detect the type
		movlw 200
		call cmdSetSmpRate
		movlw 200
		call cmdSetSmpRate
		movlw 80
		call cmdSetSmpRate; Enter Scrolling Wheel + 5 buttons
		; Send the order for identification
		movlw GET_DEVICE_ID
		call MPSCmd
		; Wait now the mouse identification answer
		call MPSGet; Wait and Get Next byte
		; Analyze the return byte: 0x00 = PS/ 2 Standard/ 0x04 Extended to Roulette
		bcf Flags,MSWHEELPLUS
		movf Value,W
		sublw MS_WHEELPLUS; Subtract the Intelimouse code 5 buttons
		btfsc STATUS,Z
			bsf Flags,MSWHEELPLUS; MS Intellimouse
		; Just for Test: If BCD display Show the ID code of the mouse
		IF SERIAL_DEBUG
		movf Value,W
		btfss PORTB,JUMPER5; If Debug Mode
			call SerialTransmit_Host
		ENDIF
		movlw 100; Return the initial sampling rate
		goto cmdSetSmpRate; Leaving the mode

; -----------------------------------------------------------------
; Translation of Touches at to Atari. This procedure deals with
; Scan-codes of game 2 or 3 of the PS/2 keyboard.
; 
; Make Code: <octy button>
; BREAK CODE: <$ f0> <octute key> for game 3
; 
; Entrance: Value = byte Keyboard recuis, and flag break_code to 1 for breakcode
; Output: w = destroy
; 
; Global: Value which contains the byte
; -----------------------------------------------------------------

SendAtariKeyboard
		; Controller so as not to take the table size
		; The Bit Carry for a Subb positions the bit to Zero
		; If the subtraction gives a remainder therefore a disparaging
		movf Value,W			
		sublw MAX_VALUE_LOOKUP; subtract the Max offset
		btfss STATUS,C; If carry = 1 no problem we go
			return; ignore code> max_value_lookup
		bcf Flags4,FORCE_ALT_SHIFT; No alt / shift forcing to the central unit
		btfss Flags3,KEYB_SET_2
			goto GetSet3
		; Treatment Play 2
		btfsc Flags3,NEXT_CODE
			goto NextCode
		movf Value,W; recharge W with the value
		sublw 0x11; Left Alt Code Game 2, Special treatment
		btfss STATUS,Z
			goto NoAltSet2
		bcf Flags3,ALT_PS2; Releasing Alt
		btfss Flags2,BREAK_CODE
			bsf Flags3,ALT_PS2
		movlw DEF_ALTGR; Atari
                goto SaveValue
NoAltSet2
		movlw HIGH Get_Set2_ScanCode_1
	        movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movf Value,W; recharge W with the value
		call Get_Set2_ScanCode_1
		bcf PCLATH,3; page 0 or 2
		goto TestIdxEeprom
NextCode
		movlw HIGH Get_Set2_ScanCode_2
	        movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movf Value,W; recharge W with the value
		call Get_Set2_ScanCode_2
		bcf PCLATH,3; page 0 or 2
TestIdxEeprom		
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Search_Code_Set3
		bcf PCLATH,3; page 0 or 2
		iorlw 0
		btfsc STATUS,Z
			goto TestCode; scan-code eeprom not found
GetSet3		; Treatment Game 3
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Test_Shift_Alt_AltGr
		bcf PCLATH,3; page 0 or 2
		btfsc Flags3,ALT_PS2
			goto No_Modifier; Alt pushes, only uses the EPROM table
		movlw HIGH Get_Modifier
		movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movf Value,W
		call Get_Modifier
		bcf PCLATH,3; page 0 or 2
		movwf Counter
		btfss Counter,7
			clrf Counter; No alt / shift forcing to the central unit
		btfsc Counter,7
			bsf Flags4,FORCE_ALT_SHIFT	
                btfss Flags5,ALTGR_PS2_BREAK
                	goto NoAltGr
		movlw HIGH Get_Scan_Codes_AltGr
		movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movf Value,W
		call Get_Scan_Codes_AltGr
		bcf PCLATH,3; page 0 or 2
		iorlw 0
		btfsc STATUS,Z
			goto NoAltGr
		swapf Counter,F; bit 1: alt, bit 0: shift stats for the altgr table
		bcf Counter,6
		btfsc Counter,2
			bsf Counter,6; Recopy forcing Bit 6 Ctrl Event
		goto SaveValue; Code with altgr
NoAltGr
                btfss Flags5,SHIFT_PS2_BREAK
                	goto No_Modifier
		movlw HIGH Get_Scan_Codes_Shift
		movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movf Value,W
		call Get_Scan_Codes_Shift
		bcf PCLATH,3; page 0 or 2
		iorlw 0
		btfsc STATUS,Z
			goto No_Modifier
		rrf Counter,F			
		rrf Counter,F; bit 1: alt, bit 0: shift stats for the shift table
		bcf Counter,6
		btfsc Counter,4
			bsf Counter,6; Recopy forcing Bit 6 Ctrl Event
		goto SaveValue; Code with shift
No_Modifier
		movf Value,W
GetValueEEProm
		call Lecture_EEProm; Scan-Code Eeprom Game 3 Side modified
SaveValue	; Common Part Game 2 & 3
		movwf Value; Save the result in Value
TestCode
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Test_Shift_Alt_Ctrl_Host
		bcf PCLATH,3; page 0 or 2
		btfsc Flags2,BREAK_CODE
			goto NoForceHostCtrlAltShift; releasing
		btfss Flags4,FORCE_ALT_SHIFT
			goto TestKeyForced; No forcing after the modification table
		bsf Flags4,KEY_FORCED
		btfss Counter,6; Ctrl
			goto NoForceHostCtrl
		movlw 0x1D; Ctrl
		btfss Flags4,CTRL_HOST
			call SerialTransmit_Host
NoForceHostCtrl
		btfsc Counter,0; Shift
			goto ForceShiftOn
		movlw 0xAA; Releasing Left Shift
		btfsc Flags4,LEFT_SHIFT_HOST
			call SerialTransmit_Host
		movlw 0xB6; Releasing Right Shift
		btfsc Flags4,RIGHT_SHIFT_HOST
			call SerialTransmit_Host
		goto TestAltHost
ForceShiftOn
		movlw 0x2A; Left Shift
		btfss Flags4,LEFT_SHIFT_HOST
			call SerialTransmit_Host
		movf Value,W
		sublw 0x60; <
		btfsc STATUS,Z
			goto TestAltHost
		movlw 0x36; Right Shift
		btfss Flags4,RIGHT_SHIFT_HOST
			call SerialTransmit_Host
TestAltHost
		btfsc Counter,1; Alt
			goto ForceAltOn
		movlw 0xB8; Releasing Alt
		btfsc Flags4,ALT_HOST
			call SerialTransmit_Host
		goto NoForceHostCtrlAltShift	
ForceAltOn
		movlw 0x38; Alt
		btfss Flags4,ALT_HOST
			call SerialTransmit_Host
		goto NoForceHostCtrlAltShift
TestKeyForced	
		call Remove_Key_Forced; abnormal case of a CTRL, shift or alt still force
NoForceHostCtrlAltShift
		movf Value,W; Test if translation a gives 0
		btfsc STATUS,Z; Z = 1 if W contains 0
			return; no assignment
		btfss Value,7
			goto NoSpecialCodeWithStatus; Code <0x80
; > = 0x80 => sent via 0xf6 0x05 0x00 0x00 0x00 0x00 0x00 0xxx-0x80
; or 0xf6 0x05 0x00 0x00 0x00 0x00 0x00 0xxx if Relax
		bcf Value,7
		movf Value,W
		sublw MAX_VALUE_ATARI; subtract the Max offset
		btfss STATUS,C; If carry = 1 no problem we go
			return; ignore code> max_value_atari
		movlw HEADER_STATUS; State Header
		call SerialTransmit_Host
		movlw IKBD_PROGKB; Eiffel Code
		call SerialTransmit_Host
		movlw 5
		call TransmitNullBytes
NoSpecialCodeWithStatus
		movf Value,W
		sublw MAX_VALUE_ATARI; subtract the Max offset
		btfss STATUS,C; If carry = 1 no problem we go
			return; ignore code> max_value_atari
		movf Value,W
		btfsc Flags2,BREAK_CODE
			iorlw 0x80; Relaxation (breakcode bit7 = 1)
		movwf TEMP3
                ; Detect if caps lock press to light/turn off the LED
		sublw KEY_CAPSLOCK; Lock caps
		btfsc STATUS,Z
			call CapsLock
		movf TEMP3,W
		call SerialTransmit_Host; Atari-Code
		btfss Flags2,BREAK_CODE
			return; penalty
		; Relax (Break Code)
		btfss Flags4,FORCE_ALT_SHIFT
			return; No forcing after the modification table
		bcf Flags4,KEY_FORCED
		btfss Counter,6; Ctrl
			goto NoForceHostCtrl2
		movlw 0x9D; Relaxation Ctrl
		btfss Flags4,CTRL_HOST
			call SerialTransmit_Host
NoForceHostCtrl2
		btfsc Counter,0; Shift
			goto RestoreShift
		movlw 0x2A; Left Shift
		btfsc Flags4,LEFT_SHIFT_HOST
			call SerialTransmit_Host
		movlw 0x36; Right Shift
		btfsc Flags4,RIGHT_SHIFT_HOST
			call SerialTransmit_Host
		goto TestAltHost2
RestoreShift
		movlw 0xAA; Releasing Left Shift
		btfss Flags4,LEFT_SHIFT_HOST
			call SerialTransmit_Host
		movlw 0xB6; Releasing Right Shift
		btfss Flags4,RIGHT_SHIFT_HOST
			call SerialTransmit_Host
TestAltHost2
		btfsc Counter,1; Alt
			goto RestoreAlt
		movlw 0x38; Alt
		btfsc Flags4,ALT_HOST
			call SerialTransmit_Host
		return	
RestoreAlt
		movlw 0xB8; Releasing Alt
		btfss Flags4,ALT_HOST
			call SerialTransmit_Host
		return

; -----------------------------------------------------------------
; Abnormal case of a CTRL, shift or alt still force
; -----------------------------------------------------------------

Remove_Key_Forced
		btfss Flags4,KEY_FORCED
			return
		btfsc Flags4,CTRL_HOST
			goto UnforceHostCtrlAltShift
		btfsc Flags4,LEFT_SHIFT_HOST
			goto UnforceHostCtrlAltShift
		btfsc Flags4,RIGHT_SHIFT_HOST
			goto UnforceHostCtrlAltShift
		btfss Flags4,ALT_HOST
			return
UnforceHostCtrlAltShift
		bcf Flags4,KEY_FORCED
		movlw 0x9D; Relaxation Ctrl
		call SerialTransmit_Host
		movlw 0xAA; Releasing Left Shift
		call SerialTransmit_Host
		movlw 0xB6; Releasing Right Shift
		call SerialTransmit_Host
		movlw 0xB8; Releasing Alt
		goto SerialTransmit_Host

; -----------------------------------------------------------------
; Reception Usart byte by byte on a LOAD command
; -----------------------------------------------------------------
		
Receive_Bytes_Load		
		IF LCD
		IF !LCD_DEBUG		
		btfsc Flags4,RE_LCD_IKBD; End reception LCD
			goto Receive_Load_Lcd
		ENDIF
		ENDIF
		; Recuctance in progress of bytes via the LOAD command
		btfsc PTRH_LOAD,0
			goto Not_Page_0_1; 2-3 pages
		bcf STATUS,IRP; 0-1 pages
		btfss PTRL_LOAD,7
			goto Not_Load; Ignore page 0 (used for variables)
		goto Load_Page
Not_Page_0_1
		bsf STATUS,IRP; 2-3 pages
		btfsc PTRL_LOAD,7
			goto Not_Load; ignore page 3 (used for the emission stamp)
Load_Page
		movf PTRL_LOAD,W
		andlw 0x60
		btfsc STATUS,Z
			goto Not_Load; ignore address <0x20 (registers)
		movf PTRL_LOAD,W
		movwf FSR	
		andlw 0x70
		sublw 0x70
		btfsc STATUS,Z
			goto Not_Load; ignore address> = 0x70 (variables common to 4 pages)
		movf Value,W
		movwf INDF
Not_Load        
		incf PTRL_LOAD,F
		btfsc STATUS,Z
			incf PTRH_LOAD,F
		IF LCD
		IF !LCD_DEBUG
		goto Next_Byte_Load
Receive_Load_Lcd
		movf Value,W
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call SendCHAR
		bcf PCLATH,3; page 0 or 2
Next_Byte_Load
		ENDIF
		ENDIF
		decfsz Counter_LOAD,F
			return
		bcf Flags4,RE_LCD_IKBD; End reception LCD
		bcf Flags3,RE_TIMER_IKBD; End reception ikbd in the main loop
		return

; -----------------------------------------------------------------
; WAIT FOR BYTE TO BE Received in Usart and Return with byte in Value
; -----------------------------------------------------------------

SerialReceive
		btfss PIR1,RCIF ; Check if data received
			goto SerialReceive; Wait Until New Data
		btfss RCSTA,OERR
			goto TestFramingError
		bcf RCSTA,CREN; Overrun Error
		bsf RCSTA,CREN; Enable reception
		goto SerialReceive
TestFramingError
		btfss RCSTA,FERR 
			goto GetByteReceived
		movf RCREG,W; Acquitta Framing Error error
                goto SerialReceive
GetByteReceived
		movf RCREG,W; Get Received Data Into W
		movwf Value; Put w Into Value (Result)
		return

; -----------------------------------------------------------------
; Transmit byte in w Register from Usart
; 
; Entrance: w = binary given
; Exit: nothing
; 
; Global: temp2, temp3, temp4, temp5, temp6
; Deb_index_em, end_index_em, stamp_em used
; -----------------------------------------------------------------
		
SerialTransmit_Host
		IF SERIAL_DEBUG
		btfsc PORTB,JUMPER5
			goto Mode_Binaire; If Jumper5 Disable (5V) Normal
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call SendHexa; IF MODE DEBUG
		bcf PCLATH,3; page 0 or 2
		return
Mode_Binaire
		ENDIF
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call SerialTransmit; Send the byte order to the host
		bcf PCLATH,3; page 0 or 2
		return
		
; -----------------------------------------------------------------
; Transmit null bytes from Usart
; 
; Entrance: w = number of void bytes to send
; Exit: nothing
; 
; Global: Counter
; -----------------------------------------------------------------

TransmitNullBytes
		movwf Counter	
Loop_SerialTransmit_Status
			clrw
			call SerialTransmit_Host
			decfsz Counter,F
		goto Loop_SerialTransmit_Status
		return		

; -----------------------------------------------------------------
; Send of flowers
; -----------------------------------------------------------------

SerialTransmit_Haut
		movlw 0x48; High breeze button
		goto SerialTransmit_KeyUpDown
		
SerialTransmit_Bas
		movlw 0x50; Low boast button
		goto SerialTransmit_KeyUpDown
		
SerialTransmit_Gauche
		movlw 0x4B; Left Fleche button
		goto SerialTransmit_KeyUpDown
		
SerialTransmit_Droite
		movlw 0x4D; Straight Fleche button

; -----------------------------------------------------------------
; Sending the code to a key and its just releasing after
; 
; Entrance: w = binary given
; Exit: nothing
; 
; Global: Value uses
; -----------------------------------------------------------------

SerialTransmit_KeyUpDown
		movwf Value
		call SerialTransmit_Host; key
		movf Value,W
		iorlw 0x80; releasing
		goto SerialTransmit_Host

; -----------------------------------------------------------------
; Change of state LED Caps Lock
; 
; Entrance: w = binary given
; Exit: nothing
; 
; Global: Flags, flags3
; -----------------------------------------------------------------
		
CapsLock
		IF SCROOL_LOCK_ERR
		bsf PARITY,7
		ENDIF
		btfss Flags,CAPS_LOCK; Testing the Capslock bit is active
			goto CapsToOn; we execute the goto if bit = 0
		; Capslock was already started: we desactive his LED and cancels his bit
		bcf Flags,CAPS_LOCK
		goto UpdateLedOnOff
CapsToOn
		; Capslock was not started: we activate your LED and activate your bit
		bsf Flags,CAPS_LOCK

UpdateLedOnOff
		movlw CAPS_OFF
		btfsc Flags,CAPS_LOCK
			movlw CAPS_ON
		IF SCROOL_LOCK_ERR
		btfss PARITY,7  	
			iorlw LED_SCROLL
		ENDIF

; -----------------------------------------------------------------
; Light keyboard leds at
; 
; Entrance: Var LED contains the bits field
; Output: w = destroy
; 
; Global: Var LED
; -----------------------------------------------------------------

cmdLedOnOff
		IF !SCROOL_LOCK_ERR
		btfsc Flags3,KEYB_SET_2
			iorlw LED_SCROLL; game no 2
		ENDIF
		movwf TEMP2
		movlw SET_RESET_LEDS; send the ignition
		call KPSCmd
		movf TEMP2,W; Light LEDs chosen
		goto KPSCmd

; -----------------------------------------------------------------
; Activate the Make/Break mode of CC scan.
; This mode must be selected because the delays and the repetition of the keys
; are gears by the Atari Xbios.
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdMakeBreak
		movlw SET_ALL_KEYS_MAKE_BREAK; Send the Make/Break order
		goto KPSCmd

; -----------------------------------------------------------------
; Send the Set Typematic Life command on the keyboard, to fix the
; repetition speed
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

; cmdtyperate
; movlw set_typematic_rate_delay
; Call KPSCMD
; MOVLW 0x20; DEALS 250 ms 30 characteristics per second
; Goto KPSCMD
 
; -----------------------------------------------------------------
; Passage in scan-code game 2 or 3 keyboard
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdScanSet
		movlw SET_SCAN_CODE_SET; Send the selection order
		call KPSCmd
		movlw SCANCODESET3
		btfsc Flags3,KEYB_SET_2
			movlw SCANCODESET2
		goto KPSCmd

; -----------------------------------------------------------------
; Sending the Reset command to the keyboard
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdResetKey
		bcf Flags2,RESET_KEYB; Flag Request Reset Keyboard given to 0
		movlw RESET
		goto KPSCmd

; -----------------------------------------------------------------
; Sending the order Resend to the keyboard
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdResendKey
		movlw RESEND
		goto KPS2cmd; no acknoledge
		
; -----------------------------------------------------------------
; Sending the ENable command to the keyboard
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdEnableKey
		movlw ENABLE; send the order
		goto KPSCmd

; -----------------------------------------------------------------
; Sending the disabled command to the keyboard
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdDisableKey
		movlw DEFAULT_DISABLE
		goto KPSCmd

; -----------------------------------------------------------------
; Sending the reset command to the mouse
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdResetMouse
		bcf Flags2,RESET_MOUSE; flag request reset mouse given to 0
		movlw RESET
		goto MPSCmd

; -----------------------------------------------------------------
; Sending the command gives the mouse
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdResendMouse
		movlw RESEND
		goto MPS2cmd; No acknowledge

; -----------------------------------------------------------------
; Sending the Enable to the Mouse Order
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdEnableMous
		movlw ENABLE_DATA_REPORTING
		goto MPSCmd

; -----------------------------------------------------------------
; Sending the order disabled to the mouse
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdDisableMouse
		movlw DISABLE_DATA_REPORTING
		goto MPSCmd

; -----------------------------------------------------------------
; Emission of the Set Sample Rate on the mouse
; 
; Entrance: nothing
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

cmdSetSmpRate
		movwf TEMP2
		movlw SET_SAMPLE_RATE
		call MPSCmd
		movf TEMP2,W
		goto MPSCmd
		
; -----------------------------------------------------------------
; Emitting an order on the keyboard
; 
; Entrance: w
; Output: w = destroy
; 
; Global: none
; -----------------------------------------------------------------

KPSCmd
		call KPS2cmd; => ATENTE VALUE ACKNOWLEDGE
		
; -----------------------------------------------------------------
; Keyboard: reading routine of an byte from Port Din5 and Minidin6
; 
; Entrance: nothing
; Output: w = 1 if byte received otherwise 0
; Var value = byte received
; -----------------------------------------------------------------

KPSGet
		; Test the clock, if low condition, the keyboard communicates
		IF NON_BLOQUANT	
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Wait_Kclock
		bcf PCLATH,3; page 0 or 2
		movf Counter,W
		btfsc STATUS,Z
			return; time-out
		ELSE
		WAIT_KCLOCK_L; waiting front descendant of clk
		ENDIF	
KPSGet2
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call _KPSGet2
		bcf PCLATH,3; page 0 or 2
		return

; -----------------------------------------------------------------
; This routine sends a byte in w to a ps/2 mouse or keyboard. Temp1, temp2,
; and parity are general purpose registers. Clock and data are assigned to
; Port bits, and "delay" is a self-explainatory macro. Data and Clock Are
; Held High by Setting Their I/O Pin To Input and Allowing An External Pullup
; resistor to sweater the line high. The Lines are Busht Low by Setting The
; I/O PIN to OUTPUT AND WRITING A "0" to the PIN.
; -----------------------------------------------------------------

KPS2cmd
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call _KPS2cmd
		bcf PCLATH,3; page 0 or 2
		return

; -----------------------------------------------------------------
; Emitting an order on the mouse
; 
; Entrance: w
; Output: w = destroy
; 
; Global: none
; -------------------------------------------------------------------

MPSCmd
		call MPS2cmd; => ATENTE VALUE ACKNOWLEDGE
		
; -----------------------------------------------------------------
; Routine reading an byte on the mouse port. She is waiting for a package
; be available.
; 
; Entrance: nothing
; Output: w = 1 if byte received otherwise 0
; Var value = byte received
; -----------------------------------------------------------------

MPSGet
		; Test the clock, if low state, the mouse communicates
		IF NON_BLOQUANT
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Wait_Mclock
		bcf PCLATH,3; page 0 or 2
		movf Counter,W
		btfsc STATUS,Z
			return; time-out
		ELSE
		WAIT_MCLOCK_L; waiting front descendant of clk
		ENDIF
MPSGet2
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call _MPSGet2
		bcf PCLATH,3; page 0 or 2
		return
		
; -----------------------------------------------------------------
; This routine sends a byte in w to a ps/2 mouse
; -----------------------------------------------------------------

MPS2cmd
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call _MPS2cmd
		bcf PCLATH,3; page 0 or 2
		return

; -----------------------------------------------------------------
; Some routine for reducing the code size
; -----------------------------------------------------------------

Lecture_EEProm
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Lecture_EEProm2
		bcf PCLATH,3; page 0 or 2
		return
		
Leds_Eiffel_On
		bcf PORTB,LEDGREEN; turn on the two LEDs
		bcf PORTB,LEDYELLOW
		return

Leds_Eiffel_Off
		bsf PORTB,LEDGREEN; turn off the two LEDs
		bsf PORTB,LEDYELLOW
		return

; -----------------------------------------------------------------

		ORG   0x8C8; Zone from 0x800 to 0x8c7 for the flashing program

; ====================================================================================================================================
; Procedures and functions
; -----------------------------------------------------------------
; Zone on page 1 or 3 for functions
; 
; Program on page 0, call:
; BSF PCLATH, 3; Page 1 or 3 (0x800 - 0xfff)
; Call function
; BCF PCLATH, 3; page 0 or 2
; or with table with PCL:
; Movlw High Function
; Movwf PCLATH
; BTFSC Info_Boot, 7
; BSF PCLATH, 4; Page 2-3 (0x1000 - 0x1fff)
; Call function
; BCF PCLATH, 3; page 0 or 2
; If one function calls for another function in the same page a call is
; Sufficient unless it is a function with PCL table:
; Movlw High Function
; Movwf PCLATH
; BTFSC Info_Boot, 7
; BSF PCLATH, 4; Page 2-3 (0x1000 - 0x1fff)
; Call function
; ====================================================================================================================================

; -----------------------------------------------------------------
; Counter3 division: Counter2 / W (temp3)
; Result in Counter3: Counter2
; Stay in Value
; -----------------------------------------------------------------
	
Div_1608
		movwf TEMP3	
		UDIV1608L Counter3,Counter2,TEMP3,Value
		return	

; -----------------------------------------------------------------
; Reading the two joysticks in Joy0 and Joy1
; -----------------------------------------------------------------

Read_Joysticks
		comf PORTA,W
		movwf JOY0
		rrf JOY0,W
		andlw 0x1F
		movwf JOY0; Joystick 0 reading 0
		comf PORTC,W
		andlw 0x1F
		movwf JOY1; Reading Joystick 1
		return

; -----------------------------------------------------------------
; Delais 4W+4 cycles (with call, Return, and Movlw) (0 = 256)
; 
; Entrance: W = Delais in Us
; Exit: nothing
; 
; -----------------------------------------------------------------

Delay_Routine
			addlw -1; Precise delays used in i/o
			btfss STATUS,Z
				goto Delay_Routine
		return
		
; -----------------------------------------------------------------

		ORG   0x900

; -----------------------------------------------------------------
; Implementation of a correspondence table to give the number of
; day in the month, W contains the month and return the number of days
; 
; Entrance: w = month
; Outing: w = number of days of the month
; 
; Global: none
; -----------------------------------------------------------------

Days_In_Month
		addwf PCL,F
		nop
		retlw 31; January
		retlw 28; FEBRUARY
		retlw 31; March
		retlw 30; april
		retlw 31; may
		retlw 30; June
		retlw 31; July
		retlw 31; august
		retlw 30; september
		retlw 31; october
		retlw 30; november
		retlw 31; December
		retlw 31
		retlw 31
		retlw 31
		
; -----------------------------------------------------------------------------------
; Return the scan code for play 2
; 
; Entrance: w = scan code jeut 2 to convert if 0x0 => 2nd code
; Outing: w = scan code Atari
; 
; Global: none
; -----------------------------------------------------------------------------------

Get_Set2_ScanCode_1
		addwf PCL,F
		retlw 0x00; Offset + 0x00 Never use: "Error or Buffer Overflow"
		retlw 0x43; Offset + 0x01 F9
		retlw 0x00; Offset + 0x02 never used
		retlw 0x3F; Offset + 0x03 F5
		retlw 0x3D; Offset + 0x04 F3
		retlw 0x3B; Offset + 0x05 F1
		retlw 0x3C; Offset + 0x06 F2
		retlw DEF_F12; Offset + 0x07 F12 <= undo Atari (FR)
		retlw 0x00; Offset + 0x08 never used
		retlw 0x44; Offset + 0x09 F10
		retlw 0x42; Offset + 0x0a F8
		retlw 0x40; Offset + 0x0b F6
		retlw 0x3E; Offset + 0x0c F4
		retlw 0x0F; Offset + 0x0d Tabulation
		retlw DEF_RUSSE; Offset + 0x0e <2> key (`) (next 1)
		retlw 0x00; Offset + 0x0f never used
		retlw 0x00; Offset + 0x10 never used
		retlw DEF_ALTGR; Offset + 0x11 LEFT ALT (Atari En has only one)
		retlw 0x2A; Offset + 0x12 Left Shift
		retlw 0x00; Offset + 0x13 never used
		retlw 0x1D; Offset + 0x14 Left Ctrl (Atari in one)
		retlw 0x10; Offset + 0x15 A (Q)
		retlw 0x02; Offset + 0x16 1
		retlw 0x00; Offset + 0x17 never used
		retlw 0x00; Offset + 0x18 never used
		retlw 0x00; Offset + 0x19 never used
		retlw 0x2C; Offset + 0x1a W (Z)
		retlw 0x1F; Offset + 0x1b s
		retlw 0x1E; Offset + 0x1c Q (A)
		retlw 0x11; Offset + 0x1d Z (W)
		retlw 0x03; Offset + 0x1e 2
		retlw 0x00; Offset + 0x1f never used
		retlw 0x00; Offset + 0x20 never used
		retlw 0x2E; Offset + 0x21 C
		retlw 0x2D; Offset + 0x22 x
		retlw 0x20; Offset + 0x23 D
		retlw 0x12; Offset + 0x24 e
		retlw 0x05; Offset + 0x25 4
		retlw 0x04; Offset + 0x26 3
		retlw 0x00; Offset + 0x27 never used
		retlw 0x00; Offset + 0x28 never used
		retlw 0x39; Offset + 0x29 Space Bar
		retlw 0x2F; Offset + 0x2a V
		retlw 0x21; Offset + 0x2b F
		retlw 0x14; Offset + 0x2c t
		retlw 0x13; Offset + 0x2d R
		retlw 0x06; Offset + 0x2e 5
		retlw 0x00; Offset + 0x2f never used
		retlw 0x00; Offset + 0x30 never used
		retlw 0x31; Offset + 0x31 n
		retlw 0x30; Offset + 0x32 B
		retlw 0x23; Offset + 0x33 h
		retlw 0x22; Offset + 0x34 g
		retlw 0x15; Offset + 0x35 y
		retlw 0x07; Offset + 0x36 6
		retlw 0x00; Offset + 0x37 never used
		retlw 0x00; Offset + 0x38 never used
		retlw 0x00; Offset + 0x39 never used
		retlw 0x32; Offset + 0x3a <,> (m)
		retlw 0x24; Offset + 0x3b J
		retlw 0x16; Offset + 0x3c U
		retlw 0x08; Offset + 0x3d 7
		retlw 0x09; Offset + 0x3e 8
		retlw 0x00; Offset + 0x3f never used
		retlw 0x00; Offset + 0x40 never used
		retlw 0x33; Offset + 0x41 <;> (,)
		retlw 0x25; Offset + 0x42 k
		retlw 0x17; Offset + 0x43 I
		retlw 0x18; Offset + 0x44 o
		retlw 0x0B; Offset + 0x45 0 (Zero figure)
		retlw 0x0A; Offset + 0x46 9
		retlw 0x00; Offset + 0x47 never used
		retlw 0x00; Offset + 0x48 never used
		retlw 0x34; Offset + 0x49 <:> (.)
		retlw 0x35; Offset + 0x4a <!> (/)
		retlw 0x26; Offset + 0x4b L
		retlw 0x27; Offset + 0x4c m (;)
		retlw 0x19; Offset + 0x4d P
		retlw 0x0C; Offset + 0x4e <)> (-)
		retlw 0x00; Offset + 0x4f never used
		retlw 0x00; Offset + 0x50 never used
		retlw 0x00; Offset + 0x51 never used
		retlw 0x28; Offset + 0x52 <�> (')
		retlw 0x2B; Offset + 0x53 <*> (\) Key on Compaq
		retlw 0x1A; Offset + 0x54 <^> ([)
		retlw 0x0D; Offset + 0x55 <=> (=)
		retlw 0x00; Offset + 0x56 never used
		retlw 0x00; Offset + 0x57 never used
		retlw 0x3A; Offset + 0x58 caps
		retlw 0x36; Offset + 0x59 Right Shift
		retlw 0x1C; Offset + 0x5a Return
		retlw 0x1B; Offset + 0x5b <$> (])
		retlw 0x00; Offset + 0x5c never used
		retlw 0x2B; Offset + 0x5d <*> (\) Soft Key key
		retlw 0x00; Offset + 0x5e never used
		retlw 0x00; Offset + 0x5f never used
		retlw 0x00; Offset + 0x60 never used
		retlw 0x60; Offset + 0x61> <
		retlw 0x00; Offset + 0x62 never used
		retlw 0x00; Offset + 0x63 never used
		retlw 0x00; Offset + 0x64 never used
		retlw 0x00; Offset + 0x65 never used
		retlw 0x0E; Offset + 0x66 backspace
		retlw 0x00; Offset + 0x67 never used
		retlw 0x00; Offset + 0x68 never used
		retlw 0x6D; Offset + 0x69 kp 1
		retlw 0x00; Offset + 0x6a never used
		retlw 0x6A; Offset + 0x6b KP 4
		retlw 0x67; Offset + 0x6c KP 7
		retlw 0x00; Offset + 0x6d never used
		retlw 0x00; Offset + 0x6e never used
		retlw 0x00; Offset + 0x6f never used
		retlw 0x70; Offset + 0x70 kp 0 (Zero)
		retlw 0x71; Offset + 0x71 kp.
		retlw 0x6E; Offset + 0x72 kp 2
		retlw 0x6B; Offset + 0x73 kp 5
		retlw 0x6C; Offset + 0x74 kp 6
		retlw 0x68; Offset + 0x75 kp 8
		retlw 0x01; Offset + 0x76 ESC
		retlw DEF_VERRNUM; Offset + 0x77 Verr Num (Unused on Atari Before)
		retlw 0x62; Offset + 0x78 F11 <= Help Atari (FR)
		retlw 0x4E; Offset + 0x79 kp +
		retlw 0x6F; Offset + 0x7a KP 3
		retlw 0x4A; Offset + 0x7b KP -
		retlw 0x66; Offset + 0x7c KP *
		retlw 0x69; Offset + 0x7d KP 9
		retlw DEF_SCROLL; Offset + 0x7e Scroll
		retlw 0x00; Offset + 0x7f never used
		retlw 0x00; Offset + 0x80 never used
		retlw 0x00; Offset + 0x81 never used
		retlw 0x00; Offset + 0x82 never used
		retlw 0x41; Offset + 0x83 F7
		retlw DEF_PRINTSCREEN; Offset + 0x84 print Screen + Alt
		retlw 0x00; Offset + 0x85 never used
		retlw 0x00; Offset + 0x86 never used
		retlw 0x00; Offset + 0x87 never used
		retlw 0x00; Offset + 0x88 never used
		retlw 0x00; Offset + 0x89 never used
		retlw 0x00; Offset + 0x8a never used
		retlw 0x00; Offset + 0x8b never used
		retlw 0x00; Offset + 0x8c never used
		retlw 0x00; Offset + 0x8d never used
		retlw 0x00; Offset + 0x8e never used
		retlw 0x00; Offset + 0x8f never used
		
; -----------------------------------------------------------------------------------
; Registers and variable initialization
; -----------------------------------------------------------------------------------

Init
		bsf STATUS,RP0; page 1
		bcf STATUS,RP1
		movlw 0x0E
		movwf ADCON1; Digital inputs except year0
		movlw 0xFF
		movwf TRISA; 6 inputs
		movlw 0xDF; Motoron out
		movwf TRISC; 7 entrees
		IF LCD
		movlw 0x05; RB4 & RB5 used for LCD
		ELSE	
		movlw KM_DISABLE; Ledyellow, Ledgreen, McLock, Kclock out
		ENDIF
		movwf TRISB
                IF MHZ_8
                movlw 250
                ELSE
		movlw 125
                ENDIF
		movwf PR2; Timer 2 to 5 ms (1 MHz /4/125/10)
		bcf STATUS,RP0; page 0
		movlw 0xC5; RC OSC, Go, Adon, launches conversion A/D on an0
		movwf ADCON0
		clrf PORTA
		; Set Latchs B to 0 and set on Leds
		clrf PORTB; Allow to Disable Devices Communications
		movlw 0x49; Timer 2 Predivator by 4 and divider by 10
		movwf T2CON
		clrf TMR2
		bsf T2CON,TMR2ON; Lance Timer 2
		; SETUP RS-232 PORT AT 9600 OR 7812.
		IF SERIAL_DEBUG
		movlw BAUDRATE; Set Baud Rate 9600 For 4MHz Clock (Fosc/16*(x+1))
		btfsc PORTB,JUMPER5; If Jumper5 Enable (0V) Skip
		ENDIF
		movlw BAUDRATE; ELSE SET BAUD RATE 7812.5 For 4MHZ CLOCK
		bsf STATUS,RP0; page 1
		movwf SPBRG
		IF INTERRUPTS
		bsf PIE1,TMR2IE; Timer 2 interrupt enable
		ENDIF
		bsf TXSTA,BRGH; Baud Rate High Speed ​​Option, asynchronous high speed
		bsf TXSTA,TXEN; Enable transmission
		bcf STATUS,RP0; page 0
		bsf RCSTA,CREN; Enable reception
		bsf RCSTA,SPEN; Enable Serial Port (RC6/7)
		clrf Flags
		clrf Flags2
		clrf Flags3
		clrf Flags4
		clrf Flags5
		call Read_Joysticks
		clrf Val_AN0; Reading CTN on An0
		clrf Temperature; CTN temperature reading on year0
		clrf X_MAX_POSH; Absolute x position maximum mouse strong weight
		clrf X_MAX_POSL; Absolute x position maximum mouse low weight
		clrf Y_MAX_POSH; Absolute position maximum mouse strong weight
		clrf Y_MAX_POSL; Absolute position maximum mice low weight
		movlw 1
		movwf X_SCALE; ABSOLLY MOUSED FASTER FASTER
		movwf Y_SCALE; A absolute mouse factor factor
		clrf DELTA_X; Deltax mode Keycode mouse ikbd
		clrf DELTA_Y; Deltay mode Keycode mouse ikbd
		clrf BUTTONS; mouse buttons
		clrf OLD_BUTTONS; Former state of mouse buttons
		clrf OLD_BUTTONS_ABS; Old state of mouse buttons absolute fashion
		clrf BUTTON_ACTION; IKBD button mode
		movlw 10
		movwf Counter_10MS_Joy; Keycode Joystick 0 (not 100 ms)
		clrf Counter_100MS_Joy; 100 ms meter Keycode Joystick 0 mode
		clrf Counter_Mouse; COMPTER OCTET Reception Mot mouse PS/2
		clrf Counter_MState_Temperature; Temperature reading machine (CPU load reduction)
		clrf DEB_INDEX_EM; Data index to send circular buffer Liaison Serie
		clrf FIN_INDEX_EM; end index given to send circular buffer
		IF SERIAL_DEBUG
		btfss PORTB,JUMPER5; If Jumper5 Disable (5V) No message
			call SerialWelcome; Else Send this String
		ENDIF
		; The PS/2 keyboard and mouse was inhibited during the flashing
		; The reset is therefore sending a keyboard reset and the mouse.
		; In the event of cold start (switching on), the control commands
		; naturally from the keyboard and the PS/2 mouse.
		btfsc Status_Boot,POWERUP_LCD
			goto Clock_Ok		
		bsf STATUS,RP1; Page 2
		clrf SEC; Raz clock at stress
		clrf SEC_BCD
		clrf MIN
		clrf MIN_BCD
		clrf HRS
		clrf HRS_BCD
		movlw 1
		movwf DAY
		movwf DAY_BCD
		movwf MONTH
		movwf MONTH_BCD		
		clrf YEAR
		clrf YEAR_BCD
		bcf STATUS,RP1; page 0
		IF LCD
		call Init_Lcd
		ENDIF
		bsf Status_Boot,POWERUP_LCD
Clock_Ok
		IF LCD
		IF LCD_DEBUG
		clrf Counter_Debug_Lcd
		ENDIF
		ENDIF
		btfsc Status_Boot,POWERUP_KEYB; put in 1 reception beats keyboard
			bsf Flags2,RESET_KEYB; valid reset keyboard in timer treatment
		btfsc Status_Boot,POWERUP_MOUSE; put in 1 of the Bat mouse reception
			bsf Flags2,RESET_MOUSE; Valid reset mouse in Timer Treatment
		bcf PIR1,TMR2IF; Acquitta Timer 2
		IF INTERRUPTS
		bsf INTCON,PEIE; Peripheral Interrupt Enable
		bsf INTCON,GIE; Global interrupt enable, authorizes interruptions
		ENDIF
		return

; -----------------------------------------------------------------
; 1/10 delages of ms
; 
; Entrance: w = delates in 1/10 of ms
; Exit: nothing
; 
; Global: Counter
; -----------------------------------------------------------------
	
		IF LCD
Delay_Routine_Ms

		movwf Counter
Loop_Delay_Routine_Ms
			Delay 100; US
			decfsz Counter,F
				goto Loop_Delay_Routine_Ms
                return
                ENDIF
		
; -----------------------------------------------------------------
		
		ORG   0xA00

; -----------------------------------------------------------------------------------
; Return the scan code for play 2
; 
; Entrance: w = scan code game 2 to convert if 0xe0 => 2nd code
; Outing: w = scan code Atari
; 
; Global: none
; -----------------------------------------------------------------------------------
		
Get_Set2_ScanCode_2
		addwf PCL,F
		retlw 0x00; Offset + 0x00 never used
		retlw 0x00; Offset + 0x01 never used
		retlw 0x00; Offset + 0x02 never used
		retlw 0x00; Offset + 0x03 never used
		retlw 0x00; Offset + 0x04 never used
		retlw 0x00; Offset + 0x05 never used
		retlw 0x00; Offset + 0x06 never used
		retlw 0x00; Offset + 0x07 never used
		retlw 0x00; Offset + 0x08 never used
		retlw 0x00; Offset + 0x09 never used
		retlw 0x00; Offset + 0x0a never used
		retlw 0x00; Offset + 0x0b never used
		retlw 0x00; Offset + 0x0c never used
		retlw 0x00; Offset + 0x0d never used
		retlw 0x00; Offset + 0x0e never used
		retlw 0x00; Offset + 0x0f never used
		retlw DEF_WWW_SEARCH; Offset + 0x10 www Search
		retlw DEF_ALTGR; Offset + 0x11 Right Alt Gr (Atari En has only one)
		retlw 0x00; Offset + 0x12 never used
		retlw 0x00; Offset + 0x13 never used
		retlw 0x1D; Offset + 0x14 right ctrl (Atari in only one)
		retlw DEF_PREVIOUS_TRACK; Offset + 0x15 PREVIOUS TRACK
		retlw 0x00; Offset + 0x16 never used
		retlw 0x00; Offset + 0x17 never used
		retlw DEF_WWW_FAVORITES; Offset + 0x18 www favorites
		retlw 0x00; Offset + 0x19 never used
		retlw 0x00; Offset + 0x1a never used
		retlw 0x00; Offset + 0x1b never used
		retlw 0x00; Offset + 0x1c never used
		retlw 0x00; Offset + 0x1d never used
		retlw 0x00; Offset + 0x1e never used
		retlw DEF_WINLEFT; Offset + 0x1f Left Win
		retlw DEF_WWW_REFRESH; Offset + 0x20 www Refresh
		retlw DEF_VOLUME_DOWN; Offset + 0x21 Down volume
		retlw 0x00; Offset + 0x22 never used
		retlw DEF_MUTE; Offset + 0x23 Mute
		retlw 0x00; Offset + 0x24 never used
		retlw 0x00; Offset + 0x25 never used
		retlw 0x00; Offset + 0x26 never used
		retlw DEF_WINRIGHT; Offset + 0x27 Right Win
		retlw DEF_WWW_STOP; Offset + 0x28 www stop
		retlw 0x00; Offset + 0x29 never used
		retlw 0x00; Offset + 0x2a never used
		retlw DEF_CACULATOR; Offset + 0x2b calculator
		retlw 0x00; Offset + 0x2c never used
		retlw 0x00; Offset + 0x2d never used
		retlw 0x00; Offset + 0x2e never used
		retlw DEF_WINAPP; Offset + 0x2f Popup Win
		retlw DEF_WWW_FORWARD; Offset + 0x30 www Forward
		retlw 0x00; Offset + 0x31 never used
		retlw DEF_VOLUME_UP; Offset + 0x32 volume up
		retlw 0x00; Offset + 0x33 never used
		retlw DEF_PLAY_PAUSE; Offset + 0x34 play/break
		retlw 0x00; Offset + 0x35 never used
		retlw 0x00; Offset + 0x36 never used
		retlw DEF_POWER; Offset + 0x37 Power
		retlw DEF_WWW_BACK; Offset + 0x38 www back
		retlw 0x00; Offset + 0x39 never used
		retlw DEF_WWW_HOME; Offset + 0x3a www Home
		retlw DEF_STOP; Offset + 0x3b stop
		retlw 0x00; Offset + 0x3c never used
		retlw 0x00; Offset + 0x3d never used
		retlw 0x00; Offset + 0x3e never used
		retlw DEF_SLEEP; Offset + 0x3f Sleep
		retlw DEF_MY_COMPUTER; Offset + 0x40 My Computer
		retlw 0x00; Offset + 0x41 never used
		retlw 0x00; Offset + 0x42 never used
		retlw 0x00; Offset + 0x43 never used
		retlw 0x00; Offset + 0x44 never used
		retlw 0x00; Offset + 0x45 never used
		retlw 0x00; Offset + 0x46 never used
		retlw 0x00; Offset + 0x47 never used
		retlw DEF_EMAIL; Offset + 0x48 E-mail
		retlw 0x00; Offset + 0x49 never used
		retlw 0x65; Offset + 0x4a kp /
		retlw 0x00; Offset + 0x4b never used
		retlw 0x00; Offset + 0x4c never used
		retlw DEF_NEXT_TRACK; Offset + 0x4d Next Track
		retlw 0x00; Offset + 0x4e never used
		retlw 0x00; Offset + 0x4f never used
		retlw DEF_MEDIA_SELECT; Offset + 0x50 Media Select
		retlw 0x00; Offset + 0x51 never used
		retlw 0x00; Offset + 0x52 never used
		retlw 0x00; Offset + 0x53 never used
		retlw 0x00; Offset + 0x54 never used
		retlw 0x00; Offset + 0x55 never used
		retlw 0x00; Offset + 0x56 never used
		retlw 0x00; Offset + 0x57 never used
		retlw 0x00; Offset + 0x58 never used
		retlw 0x00; Offset + 0x59 never used
		retlw 0x72; Offset + 0x5a KP Enter
		retlw 0x00; Offset + 0x5b never used
		retlw 0x00; Offset + 0x5c never used
		retlw 0x00; Offset + 0x5d never used
		retlw DEF_WAKE; Offset + 0x5e Wake
		retlw 0x00; Offset + 0x5f never used
		retlw 0x00; Offset + 0x60 never used
		retlw 0x00; Offset + 0x61 never used
		retlw 0x00; Offset + 0x62 never used
		retlw 0x00; Offset + 0x63 never used
		retlw 0x00; Offset + 0x64 never used
		retlw 0x00; Offset + 0x65 never used
		retlw 0x00; Offset + 0x66 never used
		retlw 0x00; Offset + 0x67 never used
		retlw 0x00; Offset + 0x68 never used
		retlw 0x55; Offset + 0x69 End
		retlw 0x00; Offset + 0x6a never used
		retlw 0x4B; Offset + 0x6b Left Arrow
		retlw 0x47; Offset + 0x6c Clear Home
		retlw 0x00; Offset + 0x6d never used
		retlw 0x00; Offset + 0x6e never used
		retlw 0x00; Offset + 0x6f never used
		retlw 0x52; Offset + 0x70 Insert
		retlw 0x53; Offset + 0x71 Delete
		retlw 0x50; Offset + 0x72 Down Arrow
		retlw 0x00; Offset + 0x73 never used
		retlw 0x4D; Offset + 0x74 Right Arrow
		retlw 0x48; Offset + 0x75 Up Arrow
		retlw 0x00; Offset + 0x76 never used
		retlw 0x00; Offset + 0x77 never used
		retlw 0x00; Offset + 0x78 never used
		retlw 0x00; Offset + 0x79 never used
		retlw DEF_PAGEDN; Offset + 0x7a Page Down (Unused on Atari Before)
		retlw 0x00; Offset + 0x7b never used
		retlw DEF_PRINTSCREEN; Offset + 0x7c Print Screen
		retlw DEF_PAGEUP; Offset + 0x7d page up (Unused on Atari Before)
		retlw DEF_PAUSE; Offset + 0x7e Pause + Ctrl
		retlw 0x00; Offset + 0x7f never used
		retlw 0x00; Offset + 0x80 never used
		retlw 0x00; Offset + 0x81 never used
		retlw 0x00; Offset + 0x82 never used
		retlw 0x00; Offset + 0x83 never used
		retlw 0x00; Offset + 0x84 never used
		retlw 0x00; Offset + 0x85 never used
		retlw 0x00; Offset + 0x86 never used
		retlw 0x00; Offset + 0x87 never used
		retlw 0x00; Offset + 0x88 never used
		retlw 0x00; Offset + 0x89 never used
		retlw 0x00; Offset + 0x8a never used
		retlw 0x00; Offset + 0x8b never used
		retlw 0x00; Offset + 0x8c never used
		retlw 0x00; Offset + 0x8d never used
		retlw 0x00; Offset + 0x8e never used
		retlw 0x00; Offset + 0x8f never used

; -----------------------------------------------------------------
; CTN temperature reading on year0
; -----------------------------------------------------------------

Read_Temperature
		movf Counter_MState_Temperature,W; Temperature reading machine (CPU load reduction)
		addlw 255
		btfsc STATUS,Z
			goto Calcul_RCTN
		addlw 255
		btfsc STATUS,Z
			goto Find_Table_Temperature
		addlw 255
		btfss STATUS,Z
			return
Calcul_Temperature
		; Step 3
		movlw Tab_CTN-EEProm+1
		call Lecture_EEProm3
		movwf TEMP4; tab_ctn+1 = temperature (n)
		movlw Tab_CTN-EEProm-2
		call Lecture_EEProm3
		movwf TEMP1; tab_ctn-2 = rctn (n-1)
		movlw Tab_CTN-EEProm-1
		call Lecture_EEProm3
		movwf TEMP5; tab_ctn-1 = temperature (n-1)
		; Temperature = temp5-(RCTN-Temp1) * (temp5-temp4)
		; -------------------------
		; Temp3-temp1
		movf TEMP1,W
		subwf RCTN,W; RCTN - Temp1
		movwf Counter3
		movf TEMP4,W
		subwf TEMP5,W; Temp5 - temp4
		call Mul_0816; Counter3: Counter2 = (RCTN-Temp1) * (temp5-temp4)
		movf TEMP1,W
		subwf TEMP3,W; Temp3 - temp1
		call Div_1608; Counter3: Counter2 = (RCTN-Temp1) * (temp5-temp4) / (temp3-temp1)
		movf Counter2,W
		subwf TEMP5,W; Temp5 - Counter2
Cmd_Motor
		movwf Temperature
		movlw Tab_Temperature-EEProm+IDX_LEVELHTEMP
		call Lecture_EEProm2
		subwf Temperature,W; Temperature - Idx_levelhtemp
		btfsc STATUS,C
			bsf PORTC,MOTORON; Temperature> = Idx_levelhtemp -> Fan walk
		movlw Tab_Temperature-EEProm+IDX_LEVELLTEMP
		call Lecture_EEProm2
		subwf Temperature,W; Temperature - Idx_levelltemp
		btfss STATUS,C
			bcf PORTC,MOTORON; Temperature <Idx_leveltemp -> Fan stop
New_Temperature
		clrf Counter_MState_Temperature
		bsf ADCON0,GO_DONE; Relaunch conversion
		return
Find_Table_Temperature
		; Step 2
		clrf Idx_Temperature
		movlw 12; Number of table values
		movwf Counter3
Loop_Find_Rctn
			movlw Tab_CTN-EEProm
			call Lecture_EEProm3
			movwf TEMP3; tab_ctn = rctn (n)
			subwf RCTN,W; CTN / 100 resistance value
			btfss STATUS,C
				goto Exit_Loop_Rctn; tab_ctn> rctn
			incf Idx_Temperature,F
			incf Idx_Temperature,F
			decfsz Counter3,F
		goto Loop_Find_Rctn
		decf Idx_Temperature,F
		decf Idx_Temperature,F
End_Tab_Rctn
		movlw Tab_CTN-EEProm+1; Temperature (N)
		call Lecture_EEProm3; Extreme value of the table
		goto Cmd_Motor
Exit_Loop_Rctn
		movf Idx_Temperature,W
		btfsc STATUS,Z
			goto End_Tab_Rctn
		movf Counter3,W
		sublw 1
		btfsc STATUS,Z
			goto End_Tab_Rctn
		goto Next_MState_Temperature
Calcul_RCTN
		; Step 1
		movf ADRESH,W; 8 bits strong weight
		subwf Val_AN0,W; Reading CTN on An0
		btfsc STATUS,Z
			goto New_Temperature; No change => recovery the measurement
		movf ADRESH,W; 8 bits strong weight
		movwf Val_AN0; Reading CTN on An0
		movwf Counter3
		movlw 100
		call Mul_0816; Counter3: Counter2 = Val_an0 * 100
		comf Val_AN0,W; 255 - Val_an0
		call Div_1608; Y_pos = (val_an0 * 100) / (255 - Val_an0)
		movf Counter2,W; low value CTN in ohms / 100
		movwf RCTN; CTN / 100 resistance value
Next_MState_Temperature
		incf Counter_MState_Temperature,F
		return

Lecture_EEProm3
		addwf Idx_Temperature,W
		goto Lecture_EEProm2

Mul_0816
		movwf Value
		UMUL0808L Counter3,Counter2,Value
		return

; -----------------------------------------------------------------

		ORG   0xB00
		
; -----------------------------------------------------------------
; Links of the EEPROM SCAN-CODES EEPROM zone
; of the game 3 with the game 2 in flash
; -----------------------------------------------------------------

Get_List_Idx_Set3
		addwf PCL,F
		nop
		retlw 0x07; F1
		retlw IDX_RUSSE
		retlw 0x0F; F2
		retlw 0x11; Left Ctrl (Atari in has only one)
		retlw 0x12; Left Shift
		retlw 0x13; > <
		retlw 0x15; A (Q)
		retlw 0x16; 1
		retlw 0x17; F3
		retlw 0x1E; 2
		retlw 0x1F; F4
		retlw 0x24; E
		retlw 0x25; 4
		retlw 0x26; 3
		retlw 0x27; F5
		retlw 0x2E; 5
		retlw 0x2F; F6
		retlw 0x36; 6
		retlw 0x37; F7
		retlw IDX_ALTGR
		retlw 0x3A; <,> (m)
		retlw 0x3D; 7
		retlw 0x3E; 8
		retlw 0x3F; F8
		retlw 0x41; <;> (,)
		retlw 0x45; 0 (Zero)
		retlw 0x46; 9
		retlw 0x47; F9
		retlw 0x49; <:> (.)
		retlw 0x4A; <!> (/)
		retlw 0x4C; M (;)
		retlw 0x4E; <)> (-)
		retlw 0x4F; F10
		retlw 0x52; <�> (')
		retlw 0x53; <*> (\) COMPAQ
		retlw 0x54; <^> ([)
		retlw 0x55; <=> (=)
		retlw IDX_F11
		retlw IDX_PRNTSCREEN
		retlw 0x59; Right Shift
		retlw 0x5B; <$> (])
		retlw IDX_F12
		retlw IDX_SCROLL
		retlw IDX_PAUSE
		retlw IDX_END
		retlw IDX_PAGEDN
		retlw IDX_PAGEUP
		retlw IDX_VERRN
		retlw IDX_SLEEP
		retlw IDX_POWER
		retlw IDX_WAKE
		retlw IDX_WLEFT
		retlw IDX_WRIGHT
		retlw IDX_WAPP
End_Get_List_Idx_Set3

Get_List_Def_Set3
		addwf PCL,F
		nop; Demarre a 1
		retlw 0x3B; F1
		retlw DEF_RUSSE
		retlw 0x3C; F2
		retlw 0x1D; Left Ctrl (Atari in has only one)
		retlw 0x2A; Left Shift
		retlw 0x60; > <
		retlw 0x10; A (Q)
		retlw 0x02; 1
		retlw 0x3D; F3
		retlw 0x03; 2
		retlw 0x3E; F4
		retlw 0x12; E
		retlw 0x05; 4
		retlw 0x04; 3
		retlw 0x3F; F5
		retlw 0x06; 5
		retlw 0x40; F6
		retlw 0x07; 6
		retlw 0x41; F7
		retlw DEF_ALTGR
		retlw 0x32; <,> (m)
		retlw 0x08; 7
		retlw 0x09; 8
		retlw 0x42; F8
		retlw 0x33; <;> (,)
		retlw 0x0B; 0 (Zero)
		retlw 0x0A; 9
		retlw 0x43; F9
		retlw 0x34; <:> (.)
		retlw 0x35; <!> (/)
		retlw 0x27; M (;)
		retlw 0x0C; <)> (-)
		retlw 0x44; F10
		retlw 0x28; <�> (')
		retlw 0x2B; <*> (\) COMPAQ
		retlw 0x1A; <^> ([)
		retlw 0x0D; <=> (=)
		retlw DEF_F11
		retlw DEF_PRINTSCREEN
		retlw 0x36; Right Shift
		retlw 0x1B; <$> (])
		retlw DEF_F12
		retlw DEF_SCROLL
		retlw DEF_PAUSE
		retlw DEF_END
		retlw DEF_PAGEDN
		retlw DEF_PAGEUP
		retlw DEF_VERRNUM
		retlw DEF_SLEEP
		retlw DEF_POWER
		retlw DEF_WAKE
		retlw DEF_WINLEFT
		retlw DEF_WINRIGHT
		retlw DEF_WINAPP
End_Get_List_Def_Set3

; -----------------------------------------------------------------
; Looking for a code for game 3 from a code of game 2
; 
; Entrance: w = scan code game 2
; Exit: Value, w = scan code game 3
; 
; Global: Value, Counter
; -----------------------------------------------------------------
 
Search_Code_Set3
		movwf Value
		movlw HIGH Get_List_Def_Set3
	        movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movlw End_Get_List_Def_Set3-Get_List_Def_Set3-2
		movwf Counter; Veriff if the Atari code is not in the modifiable list in Eeprom
LoopIdxEeeprom
			movf Counter,W
			call Get_List_Def_Set3
                        subwf Value,W
                        btfsc STATUS,Z
                        	goto FoundValueEeprom
			decfsz Counter,F	
		goto LoopIdxEeeprom
		clrw; End of table => EEPROM scan-code not found
		return
FoundValueEeprom
		movlw HIGH Get_List_Idx_Set3
	        movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		movf Counter,W; index game 3
		call Get_List_Idx_Set3
		movwf Value; Save the result in Value
		return

; -----------------------------------------------------------------
; Transmit byte in w Register from Usart
; 
; Entrance: w = binary given
; Exit: nothing
; 
; Global: temp4, temp5, temp6, deb_index_em, end_index_em, stamp_em used
; -----------------------------------------------------------------
		
SerialTransmit
		btfss Flags,IKBD_ENABLE
			return
		movwf TEMP4
		IF LCD
		IF LCD_DEBUG
		bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
		call Send_Debug_Hexa_Lcd
		bcf PCLATH,3; page 0 or 2
		ENDIF
		ENDIF
		movf FIN_INDEX_EM,W 
		btfss PIR1,TXIF; check that buffer is empty
			goto TxReg_Full
		subwf DEB_INDEX_EM,W
		btfss STATUS,Z
			goto TxReg_Full; End_index_em <> deb_index_em => character still present in the circular buffer
		movf TEMP4,W
		movwf TXREG; Transmit byte
		bcf Flags2,DATATOSEND; flag nothing to send in the main hand loop
		return
TxReg_Full
		movf STATUS,W
		movwf TEMP6; Save IRP
		movf FSR,W
		movwf TEMP5; Save FSR
		bsf STATUS,IRP; page 3
		incf FIN_INDEX_EM,W; increment index circular buffer given to send
		movwf FSR; Index pointer
		movlw TAILLE_TAMPON_EM
		subwf FSR,W
		btfsc STATUS,C
			clrf FSR; circular buffer
		movf DEB_INDEX_EM,W
		subwf FSR,W
		btfsc STATUS,Z
			goto End_SerialTransmit; buffer
		movf FSR,W
		movwf FIN_INDEX_EM
		movlw LOW TAMPON_EM
		addwf FSR,F; Index pointer
		movf TEMP4,W
		movwf INDF; Writing in the stamp
		bsf Flags2,DATATOSEND; Flag given to send in the main hand loop
End_SerialTransmit
		movf TEMP5,W
		movwf FSR; RESTUIT FSR
		movf TEMP6,W
		movwf STATUS; RESTUTURE IRP
		return	
		
; -----------------------------------------------------------------
; Clock
; 
; Dry, min, hrs, day, month, year are in an area on page 2
; not reset to reset.
; The function does not take into account the bissextile years.
; -----------------------------------------------------------------

Inc_Clock
		bsf STATUS,RP1; Page 2
		incf SEC,F; next second
		movf SEC,W
		call Conv_Bcd_Value
		movwf SEC_BCD
		movlw 60
		subwf SEC,W
		btfss STATUS,C
			goto End_Inc_Clock
		clrf SEC
		clrf SEC_BCD
		incf MIN,F; next minute
		movf MIN,W
		call Conv_Bcd_Value
		movwf MIN_BCD
		movlw 60
		subwf MIN,W
		btfss STATUS,C
			goto End_Inc_Clock
		clrf MIN
		clrf MIN_BCD
		incf HRS,F; next time
		movf HRS,W
		call Conv_Bcd_Value
		movwf HRS_BCD
		movlw 24
		subwf HRS,W
		btfss STATUS,C
			goto End_Inc_Clock
		clrf HRS
		clrf HRS_BCD
		incf DAY,F; following day
		movf DAY,W
		call Conv_Bcd_Value
		movwf DAY_BCD
		bcf STATUS,RP1; page 0
		movlw HIGH Days_In_Month
	        movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
		bsf STATUS,RP1; Page 2
		movf MONTH,W
		andlw 0x0F
		call Days_In_Month; number of days in the month
		subwf DAY,W
		btfss STATUS,C
			goto End_Inc_Clock
		movlw 1
		movwf DAY
		movwf DAY_BCD
		incf MONTH,F; following
		movf MONTH,W
		call Conv_Bcd_Value
		movwf MONTH_BCD
		movlw 13
		subwf MONTH,W
		btfss STATUS,C
			goto End_Inc_Clock
		movlw 1
		movwf MONTH
		movwf MONTH_BCD
		incf YEAR,F; next year
		movf YEAR,W
		call Conv_Bcd_Value
		movwf YEAR_BCD
		movlw 100
		subwf YEAR,W
		btfss STATUS,C
			goto End_Inc_Clock
		clrf YEAR
		clrf YEAR_BCD
End_Inc_Clock
		bcf STATUS,RP1; page 0
		return

; -----------------------------------------------------------------------------------
; Conversion of value to BCD
; 
; Entrance: w
; Outing: W
; 
; Global: temp1, temp2 are used
; -----------------------------------------------------------------------------------
		
Conv_Bcd_Value
		movwf TEMP1; Low weight BCD
		clrf TEMP2; BCD strong weight
Loop_Bcd
			movlw 10
			subwf TEMP1,W
			btfss STATUS,C
				goto Exit_Bcd
			movwf TEMP1; Low weight BCD
			incf TEMP2,F; BCD strong weight
		goto Loop_Bcd
Exit_Bcd
		swapf TEMP2,W
		andlw 0xF0; 4 bits of strong weight
		iorwf TEMP1,W
		return

; -----------------------------------------------------------------------------------
; BCD conversion to clock binary
; Year_bcd, month_bcd, day_bcd, hrs_bcd, min_bcd, and dry_bcd on page 2
; => Year, Month, Day, HRS, Min, and dry on page 2
; 
; Global: temp1, Counter are used
; -----------------------------------------------------------------------------------

Conv_Inv_Bcd_Time
		movlw LOW YEAR
		movwf FSR
		bsf STATUS,IRP; Page 2
		movlw 6
		movwf Counter
Loop_Bcd_Time
			movlw YEAR_BCD-YEAR
			addwf FSR,F
			movf INDF,W; BCD value
			movwf TEMP1
			movlw YEAR_BCD-YEAR
			subwf FSR,F
			clrw
			btfsc TEMP1,0
				addlw 1
			btfsc TEMP1,1
				addlw 2
			btfsc TEMP1,2
				addlw 4
			btfsc TEMP1,3
				addlw 8
			btfsc TEMP1,4
				addlw 10
			btfsc TEMP1,5
				addlw 20
			btfsc TEMP1,6
				addlw 40
			btfsc TEMP1,7
				addlw 80
			movwf INDF; Decimal value
			incf FSR,F
			decfsz Counter,F	
		goto Loop_Bcd_Time
		return

; -----------------------------------------------------------------
; Reverse conversion scale mouse in x absolute mode
; X_pos = x_pos_scaled * x_scale
; -----------------------------------------------------------------

Conv_Inv_Scale_X
		clrf X_POSL; X_pos = x_pos_scaled * x_scale
		clrf X_POSH
		movf X_SCALE,W
		movwf Counter
Loop_Inv_Scale_X
			movf X_POSL_SCALED,W; absolute mouse x position with low weight factor factor
			addwf X_POSL,F
			movf X_POSH_SCALED,W; absolute mouse x position with strong weight factor
			btfsc STATUS,C
				incfsz X_POSH_SCALED,W
			addwf X_POSH,F
			decfsz Counter,F
		goto Loop_Inv_Scale_X
		return	

; -----------------------------------------------------------------
; Reverse conversion scale mouse in y absolute mode
; Y_pos = y_pos_scaled * y_scale
; -----------------------------------------------------------------

Conv_Inv_Scale_Y
		clrf Y_POSL; Y_pos = y_pos_scaled * y_scale
		clrf Y_POSH
		movf Y_SCALE,W
		movwf Counter
Loop_Inv_Scale_Y
			movf Y_POSL_SCALED,W; Absolute mouse position with low weight factor factor
			addwf Y_POSL,F
			movf Y_POSH_SCALED,W; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
			btfsc STATUS,C
				incfsz Y_POSH_SCALED,W
			addwf Y_POSH,F
			decfsz Counter,F
		goto Loop_Inv_Scale_Y
		return		

; -----------------------------------------------------------------
; Conversion echelle mouse in x absolute mode
; X_pos = x_pos_scaled / x_scale
; -----------------------------------------------------------------
		
Conv_Scale_X
		movf X_POSL,W
		movwf Counter2
		movf X_POSH,W
		movwf Counter3
		movf X_SCALE,W
		call Div_1608; X_pos = x_pos_scaled / x_scale
		movf Counter2,W
		movwf X_POSL_SCALED; absolute mouse x position with low weight factor factor
		movf Counter3,W
		movwf X_POSH_SCALED; absolute mouse x position with strong weight factor
		return
		
; -----------------------------------------------------------------
; Conversion peeler mouse in y absolute mode
; Y_pos = y_pos_scaled / y_scale
; -----------------------------------------------------------------

Conv_Scale_Y
		movf Y_POSL,W
		movwf Counter2
		movf Y_POSH,W
		movwf Counter3
		movf Y_SCALE,W
		call Div_1608; Y_pos = y_pos_scaled / y_scale
		movf Counter2,W
		movwf Y_POSL_SCALED; Absolute mouse position with low weight factor factor
		movf Counter3,W
		movwf Y_POSH_SCALED; ABSOLUE MOUSE POSEMENT WITH FORCE FORTOR
		return

; -----------------------------------------------------------------
; Keyboard: reading routine of an byte from Port Din5 and Minidin6
; 
; Entrance: nothing
; Output: w = 1 if byte received otherwise 0
; Var value = byte received
; -----------------------------------------------------------------

_KPSGet2
		clrf PARITY; Used for Parity Calc
		call KPSGetBit; Get Start bit (for Nothing)
		clrf Value; Clear Value
		movlw 8; 8 bits
		movwf Counter; set counter to 8 bits to read
KGetLoop
			rrf Value,F; Rotate to right to get a shift
			bcf Value,7; Force MSB to Zero to Disable Carry Stated
			call KPSGetBit; Get a bit from Keyboard
			iorwf Value,F; Logical or with previous value
			xorwf PARITY,F; parish
			decfsz Counter,F; check if we should get another one
		goto KGetLoop
		call KPSGetBit; Get Parity Bit
		xorwf PARITY,F
		call KPSGetBit; Get Stop Bit
		iorlw 0
		btfsc STATUS,Z
			clrf PARITY; stop bit = 0 => error
		btfss PARITY,7
			clrf Value; error
		return
		
; -----------------------------------------------------------------
; This routine sends a byte in w to a ps/2 mouse or keyboard.  Temp1, temp2,
; and parity are general purpose registers.  Clock and data are assigned to
; Port bits, and "delay" is a self-explainatory macro.  Data and Clock Are
; Held High by Setting Their I/O Pin To Input and Allowing An External Pullup
; resistor to sweater the line high.  The Lines are Busht Low by Setting The
; I/O PIN to OUTPUT AND WRITING A "0" to the PIN.
; -----------------------------------------------------------------

_KPS2cmd
		movwf TEMP1; To-Be-Be-by Byte Store
		IF !NON_BLOQUANT
		movlw 8; 8 bits
		movwf Counter; Initialize to Counter
		ENDIF
		clrf PARITY; Used for Parity Calc
		bsf STATUS,RP0; page 1
		bcf TRISB,KCLOCK; output
		bcf STATUS,RP0; page 0
		bcf PORTB,KCLOCK; Inhibit communication
		Delay 100; For at Least 100 Microseconds
		bsf STATUS,RP0; page 1
		bcf TRISB,KDATA; output
		bcf STATUS,RP0; page 0
		bcf PORTB,KDATA; Valid the Scripture, Pull Data Low
		Delay 5
		bsf STATUS,RP0; page 1
		bsf TRISB,KCLOCK; In Entree, Release Clock
		bcf STATUS,RP0; page 0
		IF NON_BLOQUANT
		call Wait_Kclock; Use Counter
		movf Counter,W
		btfsc STATUS,Z
			return; time-out
		movlw 8; 8 bits
		movwf Counter; Initialize to Counter
		ENDIF
KPS2cmdLoop
			movf TEMP1,W
			xorwf PARITY,F; parish
			call KPS2cmdBit; Output 8 Data Bits
			rrf TEMP1,F
			decfsz Counter,F
		goto KPS2cmdLoop
		comf PARITY,W
		call KPS2cmdBit; Output Parity Bit
		movlw 1
		call KPS2cmdBit; output stop bit (1)
		WAIT_KCLOCK_L; waiting front descendant of clk
		WAIT_KCLOCK_H; waiting front of clk (ACK)
		return

; -----------------------------------------------------------------
; Routine reading an byte on the mouse port. She is waiting for a package
; be available.
; 
; Entrance: nothing
; Output: w = 1 if byte received otherwise 0
; Var value = byte received
; -----------------------------------------------------------------

_MPSGet2
		clrf PARITY; Used for Parity Calc
		call MPSGetBit; Get Start bit (for Nothing)
		clrf Value; Clear Value
		movlw 8; 8 bits
		movwf Counter; set counter to 8 bits to read
MGetLoop
			rrf Value,F; Rotate to right to get a shift
			bcf Value,7; Force MSB to Zero to Disable Carry Stated
			call MPSGetBit; Get a bit from Mouse
			iorwf Value,F; Logical or with previous value
			xorwf PARITY,F; parish
			decfsz Counter,F; check if we should get another one
		goto MGetLoop
		call MPSGetBit; Get Parity Bit
		xorwf PARITY,F
		call MPSGetBit; Get Stop Bit
		iorlw 0
		btfsc STATUS,Z
			clrf PARITY; stop bit = 0 => error
		btfss PARITY,7
			clrf Value; error
		return

; -----------------------------------------------------------------
; This routine sends a byte in w to a ps/2 mouse
; -----------------------------------------------------------------
		
_MPS2cmd
		movwf TEMP1; To-Be-Be-by Byte Store
		IF !NON_BLOQUANT
		movlw 8; 8 bits
		movwf Counter; Initialize to Counter
		ENDIF
		clrf PARITY; Used for Parity Calc
		bsf STATUS,RP0; page 1
		bcf TRISB,MCLOCK; output
		bcf STATUS,RP0; page 0
		bcf PORTB,MCLOCK; Inhibit communication
		Delay 100; For at Least 100 Microseconds
		bsf STATUS,RP0; page 1
		bcf TRISB,MDATA; output
		bcf STATUS,RP0; page 0
		bcf PORTB,MDATA; Valid the Scripture, Pull Data Low
		Delay 5
		bsf STATUS,RP0; page 1
		bsf TRISB,MCLOCK; In Entree, Release Clock
		bcf STATUS,RP0; page 0
		IF NON_BLOQUANT
		call Wait_Mclock; Use Counter
		movf Counter,W
		btfsc STATUS,Z
			return; time-out
		movlw 8; 8 bits
		movwf Counter; Initialize to Counter
		ENDIF
MPS2cmdLoop
			movf TEMP1,W
			xorwf PARITY,F; Parish
			call MPS2cmdBit; Output 8 Data Bits
			rrf TEMP1,F
			decfsz Counter,F
		goto MPS2cmdLoop
		comf PARITY,W
		call MPS2cmdBit; Output Parity Bit
		movlw 1
		call MPS2cmdBit; output stop bit (1)
		WAIT_MCLOCK_L; waiting front descendant of clk
		WAIT_MCLOCK_H; waiting front of clk (ACK)
		return

; -----------------------------------------------------------------
; Init 8 -character user message for the LCD display
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter2
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG		
Init_Message_User_Lcd
		call Init_Message_User_Ptr
Loop_Init_Message_Lcd
			movlw ' '
			bsf STATUS,IRP; Page 2
			movwf INDF
			incf FSR,F
			decfsz Counter2,F
		goto Loop_Init_Message_Lcd
		return
		ENDIF
		ENDIF
		
; -----------------------------------------------------------------
; Sending a 8 -character user message to the LCD display
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter, Counter2, Temp1, Temp2
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG		
Message_User_Lcd
		movlw 0xC6; line 2, column 7
		call SendINS
		call Init_Message_User_Ptr
Loop_Message_Lcd
			bsf STATUS,IRP; Page 2
			movf INDF,W
			call SendCHAR
			incf FSR,F
			decfsz Counter2,F
		goto Loop_Message_Lcd
		return
		ENDIF
		ENDIF

; -----------------------------------------------------------------------------------
; Correspondence to convert the content of the register into hexa
; W and send it to the Serie house or the LCD display
; 
; Entrance: w = quartet to convert (LSB)
; Outing: w = ASCII code
; 
; Global: temp1
; -----------------------------------------------------------------------------------
		
Conv_Hexa
		movwf TEMP1
		movlw 10
		subwf TEMP1,W
		btfsc STATUS,C
			addlw 7; A A F
		addlw '0'+10
		return
		
; -----------------------------------------------------------------
; Reading Eeprom
; -----------------------------------------------------------------

Lecture_EEProm2
		RDEEPROM
		return
		
; -----------------------------------------------------------------
	
		ORG   0xD00

; -----------------------------------------------------------------
; Edit Ctrl, Alt, Shift Table User
; -----------------------------------------------------------------
		
Get_Modifier
		addwf PCL,F
		FILL 0x3400,MAX_VALUE_LOOKUP+1; Retlw 0x00

; -----------------------------------------------------------------
; PS2 Keyboard Suboutine
; -----------------------------------------------------------------

KPSGetBit
		clrw
		WAIT_KCLOCK_L; waiting front descendant of clk
		btfsc PORTB,KDATA
			movlw BIT_UN
		WAIT_KCLOCK_H; expectation front of clk
		return

KPS2cmdBit
		WAIT_KCLOCK_L; waiting front descendant of clk
		bsf STATUS,RP0; page 1
		andlw 1
		btfss STATUS,Z ; Set/reset data line
			bsf TRISB,KDATA; in entry
		btfsc STATUS,Z
			bcf TRISB,KDATA; output
		bcf STATUS,RP0; page 0
		WAIT_KCLOCK_H; expectation front of clk
		return

		IF NON_BLOQUANT
Wait_Kclock
		movlw 16
		movwf Counter; time-out
		clrf Counter2
		clrf Counter3
WCK
					btfss PORTB,KCLOCK; waiting front descendant of clk
						return
					decfsz Counter3,F
				goto WCK
				decfsz Counter2,F
			goto WCK
			decfsz Counter,F
		goto WCK
		return
		ENDIF

; -----------------------------------------------------------------
; PS2 Mouse Suboutines
; -----------------------------------------------------------------

MPSGetBit
		clrw
		WAIT_MCLOCK_L; waiting front descendant of clk
		btfsc PORTB,MDATA
			movlw BIT_UN
		WAIT_MCLOCK_H; expectation front of clk
		return

MPS2cmdBit
		WAIT_MCLOCK_L; waiting front descendant of clk
		bsf STATUS,RP0; page 1
		andlw 1
		btfss STATUS,Z; Set/reset data line
			bsf TRISB,MDATA; in entry
		btfsc STATUS,Z
			bcf TRISB,MDATA; output
		bcf STATUS,RP0; page 0
		WAIT_MCLOCK_H; expectation front of clk
		return

		IF NON_BLOQUANT
Wait_Mclock
		movlw 16
		movwf Counter; time-out
		clrf Counter2
		clrf Counter3
WCM
					btfss PORTB,MCLOCK; waiting front descendant of clk
						return
					decfsz Counter3,F
				goto WCM
				decfsz Counter2,F
			goto WCM
			decfsz Counter,F
		goto WCM
		return
		ENDIF

; -----------------------------------------------------------------------------------
; Send the content of W in Hexa on the Serie Liaison
; That is to say: w = 0x41 => Emission of the ascii '4' then ascii '1'
; 
; Entrance: w = value
; Exit: nothing
; 
; Global: temp3
; -----------------------------------------------------------------------------------
		IF SERIAL_DEBUG
SendHexa
		movwf TEMP3; save the value to be treated
                ; Just to have 0x printed on screen
		movlw '$'
		call SerialTransmit; send the byte to the host
		swapf TEMP3,W
		andlw 0x0F; 4 bits of strong weight
		call Conv_Hexa; look for the ASCII code
		call SerialTransmit
		movf TEMP3,W
		andlw 0x0F; 4 low weight bits
		call Conv_Hexa; look for the ASCII code
		goto SerialTransmit
		ENDIF
		
; -----------------------------------------------------------------------------------
; Send the content of W in Hexa to the LCD display
; That is to say: w = 0x41 => Emission of the ascii '4' then ascii '1'
; 
; Entrance: w = value
; Exit: nothing
; 
; Global: Counter, temp1, temp2, temp3
; -----------------------------------------------------------------------------------
		IF LCD
Send_Hexa_Lcd
		movwf TEMP3; save the value to be treated
		swapf TEMP3,W
		andlw 0x0F; 4 bits of strong weight
		call Conv_Hexa; look for the ASCII code
		call SendCHAR
		movf TEMP3,W
		andlw 0x0F; 4 low weight bits
		call Conv_Hexa; look for the ASCII code
		goto SendCHAR
		ENDIF
		
; -----------------------------------------------------------------
; Variable Init for 8 -character user message on the LCD display
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter2
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG		
Init_Message_User_Ptr
		movlw LOW USER_LCD
		movwf FSR
		movlw 8
		movwf Counter2
		return
		ENDIF
		ENDIF
		
; -----------------------------------------------------------------
		
		ORG   0xE00

; -----------------------------------------------------------------
; Shift Table User
; -----------------------------------------------------------------
		
Get_Scan_Codes_Shift
		addwf PCL,F
		FILL 0x3400,MAX_VALUE_LOOKUP+1; Retlw 0x00

; -----------------------------------------------------------------
; Test change of state shifts, alt & altgr of set 3
; 
; Entrance: Value = binary given
; Exit: nothing
; 
; Global: flags2, flags3
; -----------------------------------------------------------------

Test_Shift_Alt_AltGr
		movf Value,W
		sublw 0x59; Right shift code set 3
		btfsc STATUS,Z
			goto Test_Shift
		movf Value,W
		sublw 0x12; LEFT SHIFT CODE SET 3
		btfss STATUS,Z
			goto No_Shift
Test_Shift
		bcf Flags3,SHIFT_PS2; Releasing Shift Droit
		btfsc Flags2,BREAK_CODE
			return
		bsf Flags3,SHIFT_PS2
		bsf Flags5,SHIFT_PS2_BREAK
		return
No_Shift         
		movf Value,W
		sublw 0x19; Alt code set 3
		btfss STATUS,Z
			goto No_Alt
		bcf Flags3,ALT_PS2; Releasing Alt
		btfss Flags2,BREAK_CODE
			bsf Flags3,ALT_PS2
		return
No_Alt
		movf Value,W
		sublw 0x39; Alt gr code set 3
		btfss STATUS,Z
			goto No_AltGr
		bcf Flags3,ALTGR_PS2; Releasing Altgr
		btfsc Flags2,BREAK_CODE
			return
		bsf Flags3,ALTGR_PS2
		bsf Flags5,ALTGR_PS2_BREAK
		return
No_AltGr
		btfsc Flags2,BREAK_CODE
			return; releasing
		btfss Flags3,ALTGR_PS2 		
			bcf Flags5,ALTGR_PS2_BREAK; If the altgr key is released
		btfss Flags3,SHIFT_PS2 		
			bcf Flags5,SHIFT_PS2_BREAK; If the Shift key is released
		return

; -----------------------------------------------------------------
; Test change of state shift, alt and ctrl keys sent to central unit
; 
; Entrance: Value = binary given
; Exit: nothing
; 
; Global: flags2, flags4
; -----------------------------------------------------------------

Test_Shift_Alt_Ctrl_Host
		movf Value,W
		sublw 0x1D; Ctrl
		btfss STATUS,Z
			goto No_CtrlHost
		bcf Flags4,CTRL_HOST; Relaxation Ctrl
		btfss Flags2,BREAK_CODE
			bsf Flags4,CTRL_HOST
		return
No_CtrlHost
		movf Value,W
		sublw 0x36; Right Shift
		btfss STATUS,Z
			goto No_RightShiftHost
		bcf Flags4,RIGHT_SHIFT_HOST; Releasing Shift Droit
		btfss Flags2,BREAK_CODE
			bsf Flags4,RIGHT_SHIFT_HOST
		return
No_RightShiftHost	
		movf Value,W
		sublw 0x2A; Left Shift
		btfss STATUS,Z
			goto No_LeftShiftHost
		bcf Flags4,LEFT_SHIFT_HOST; Releasing left shift
		btfss Flags2,BREAK_CODE
			bsf Flags4,LEFT_SHIFT_HOST
		return
No_LeftShiftHost         
		movf Value,W
		sublw 0x38; Alt
		btfss STATUS,Z
			return
		bcf Flags4,ALT_HOST; Releasing Alt
		btfss Flags2,BREAK_CODE
			bsf Flags4,ALT_HOST
		return
		
; -----------------------------------------------------------------
; Sending the welcome channel on the RS232
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter, temp4, temp5, temp6, deb_index_em, end_index_em, stamp_em
; -----------------------------------------------------------------

		IF SERIAL_DEBUG
SerialWelcome
		clrf Counter
		movlw HIGH WelcomeText
		movwf PCLATH
		btfsc Info_Boot,7
			bsf PCLATH,4; Page 2-3 (0x1000 - 0x1fff)
Loop_SerialWelcome
			movf Counter,W
			call WelcomeText
			bsf PCLATH,3; Page 1 or 3 (0x800 - 0xfff)
			iorlw 0
			btfsc STATUS,Z
				return         
			call SerialTransmit
			incf Counter,F
		goto Loop_SerialWelcome
		ENDIF
		
; -----------------------------------------------------------------
; Shipping the time on the LCD display
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter, Counter2, temp1, temp2, temp3
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG
Time_Lcd
		btfss Flags4,LCD_ENABLE; LCD Timer Management inhibits
			return
		movlw 0x80; line 1, column 1
		call SendINS
		movlw LOW MONTH_BCD
		movwf FSR
		movlw 4
		movwf Counter2
		call Send_Indf_Hexa_Lcd
		movlw '/'
		goto Separator_Time
Loop_Time_Lcd
			call Send_Indf_Hexa_Lcd
			movf Counter2,W
			sublw 2
			movlw ' '
			btfss STATUS,Z
				goto Separator_Time
			bsf STATUS,RP1; Page 2
			btfss SEC_BCD,0
				goto Separator_Time
			movlw ':'
Separator_Time
			bcf STATUS,RP1; page 0
			call SendCHAR; separator
			incf FSR,F
			decfsz Counter2,F	
		goto Loop_Time_Lcd
Send_Indf_Hexa_Lcd
		bsf STATUS,IRP; Page 2
		movf INDF,W; BCD value
		goto Send_Hexa_Lcd
		ENDIF
		ENDIF	

; -----------------------------------------------------------------
; Shipping from Value in Hexa to the LCD display
; 
; Entrance: w = LCD position + 0x80
; Exit: nothing
; 
; Global: Counter, Counter2, temp1, temp2, temp3
; -----------------------------------------------------------------
		IF LCD
		IF !LCD_DEBUG		
Send_Value_Lcd
		btfsc Flags2,BREAK_CODE
			goto Clear_Hex_Lcd
		call SendINS
		movf Value,W
		goto Send_Hexa_Lcd
Clear_Hex_Lcd
		movlw 0x8E; line 1, column 15
		call Spaces_Lcd
		movlw 0xCE; line 2, column 15
		goto Spaces_Lcd
		ENDIF
		ENDIF

; -----------------------------------------------------------------------------------
; Sending value in w in hexa on the entire LCD display
; 
; Entrance: w = value
; Exit: nothing
; 
; Global: Counter2, temp2, temp3, temp4, temp5, temp6
; -----------------------------------------------------------------

		IF LCD
		IF LCD_DEBUG
Send_Debug_Hexa_Lcd
		movwf TEMP4; save value
		movf Counter,W
		movwf TEMP5
		movf TEMP1,W
		movwf TEMP6
		call Cursor_Debug_Lcd
		movf TEMP4,W
		call Send_Hexa_Lcd
		incf Counter_Debug_Lcd,F
		movlw 15
		andwf Counter_Debug_Lcd,F
		call Cursor_Debug_Lcd
		movlw ' '
		call SendCHAR
		movlw ' '
		call SendCHAR
		movf TEMP5,W
		movwf Counter 
		movf TEMP6,W
		movwf TEMP1
		return
		
Cursor_Debug_Lcd
                movf Counter_Debug_Lcd,W
		addwf Counter_Debug_Lcd,W
		btfsc Counter_Debug_Lcd,3
			goto Debug_Ligne2_Lcd; => 2nd line
		addlw 0x80; line 1, column 1
		goto Debug_Ligne_Lcd
Debug_Ligne2_Lcd
		andlw 15
		addlw 0xC0; line 2, column 1
Debug_Ligne_Lcd
		goto SendINS
		ENDIF
		ENDIF

; -----------------------------------------------------------------
				
		ORG   0xF00
		
; -----------------------------------------------------------------
; Altgr User Table
; -----------------------------------------------------------------
		
Get_Scan_Codes_AltGr
		addwf PCL,F
		FILL 0x3400,MAX_VALUE_LOOKUP+1; Retlw 0x00
	
; -----------------------------------------------------------------
; Sending the temperature on the LCD display
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter, temp1, temp2, temp3, temp4
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG
Temperature_Lcd
		btfss Flags4,LCD_ENABLE; LCD Timer Management inhibits
			return
		movlw 0xC0; line 2, column 1
		call SendINS
		movf Temperature,W
		movwf TEMP4
		movlw '0'
		movwf TEMP3
Lcd_Dec10
			movlw 10
			subwf TEMP4,W
			btfss STATUS,C
				goto Lcd_Dec1
			movwf TEMP4
			incf TEMP3,F
		goto Lcd_Dec10
Lcd_Dec1
		movf TEMP3,W
		call SendCHAR
		movlw '0'
		addwf TEMP4,W
		call SendCHAR
		movlw 0xDF
		call SendCHAR
		movlw 'C'
		call SendCHAR
		movlw ' '
		call SendCHAR
		movlw '-'
		btfsc PORTC,MOTORON
			movlw '+'
		goto SendCHAR
		ENDIF
		ENDIF

; -----------------------------------------------------------------
; Sending 2 spaces on the LCD display
; 
; Entrance: w = LCD position + 0x80
; Exit: nothing
; 
; Global: Counter, temp1, temp2
; -----------------------------------------------------------------

		IF LCD
		IF !LCD_DEBUG		
Spaces_Lcd
		call SendINS
		movlw ' '
		call SendCHAR
		movlw ' '
		goto SendCHAR
		ENDIF
		ENDIF
		
; -----------------------------------------------------------------------------------
; Initialization of the LCD display
; 
; Entrance: nothing
; Exit: nothing
; 
; Global: Counter, temp1, temp2
; -----------------------------------------------------------------------------------

		IF LCD
Init_Lcd
		Delay_Ms 200; wait 20 ms before reset LCD display
		bcf STATUS,C; erases Carry (instruction out)
		movlw 0x03; Reset command
		call NybbleOut; Send Nybble
		Delay_Ms 50; wait 5 ms before sending the future
		EStrobe
		Delay 160; wait 160 US
		EStrobe
		Delay 160; wait 160 US
		bcf STATUS,C              
		movlw 0x02; 4 -bit mode
		call NybbleOut              
		Delay 160
		movlw 0x28; 2 display lines
		call SendINS
		movlw 0x08; Cut the display
		call SendINS
		movlw 0x01; erase the display
		call SendINS
		Delay_Ms 50; 4.1 ms maxi
		movlw 0x06; Cursor Deplacement Authorization
		call SendINS
		movlw 0x0C; LCD Back on
		goto SendINS
		
; -----------------------------------------------------------------------------------
; Sending a character to the LCD
; 
; Entrance: w = character
; Exit: nothing
; 
; Global: Counter, temp1, temp2
; -----------------------------------------------------------------------------------

SendCHAR
		movwf TEMP1; Save the value
		swapf TEMP1,W; Sending 4 high weight bits
		bsf STATUS,C; RS = 1
		call NybbleOut
		movf TEMP1,W; Sending 4 low weight bits
		bsf STATUS,C
		goto NybbleOut

; -----------------------------------------------------------------------------------
; Sending an instruction to the LCD
; 
; Entrance: w = code
; Exit: nothing
; 
; Global: Counter, temp1, temp2
; -----------------------------------------------------------------------------------

SendINS
		movwf TEMP1; save the value
		swapf TEMP1,W; Sending 4 high weight bits
		bcf STATUS,C; RS = 0
		call NybbleOut
		movf TEMP1,W; Sending 4 low weight bits
		bcf STATUS,C
		
; -----------------------------------------------------------------------------------
; Sending 4 bits to the LCD
; use of a 74LLS174 external dumping register or
; 1Q = D4, 2Q = D5, 3Q = D6, 4Q = D7, 5Q = RS, 6Q = E
; 
; Entrance: w = value, c = rs
; Exit: nothing
; 
; Global: Counter, temp2
; -----------------------------------------------------------------------------------

NybbleOut
		movwf TEMP2 ; Save 4 bits to send
		swapf TEMP2 ; Place the 4 bits on the left
		movlw 6	; erases the decaling register (74LS714)
		movwf Counter
NOLoop1
			ClockStrobe
			decfsz Counter
		goto NOLoop1
		bsf PORTB,LCD_DATA; Position Gate e Bit A 1
		ClockStrobe
		bcf PORTB,LCD_DATA; Position RS Bit A 0
		btfsc STATUS,C
			bsf PORTB,LCD_DATA; RS A 1
		ClockStrobe
		movlw 4; Sending the Data bits
		movwf Counter
NOLoop2
			rlf TEMP2; 4 -bit decay to send
			bcf PORTB,LCD_DATA; erase the bit data
			btfsc STATUS,C
				bsf PORTB,LCD_DATA; Bit data has 1
			ClockStrobe
			decfsz Counter
		goto NOLoop2
		EStrobe; Strobe LCD Data
		return
		
		ENDIF
		
; -----------------------------------------------------------------

		ORG   0xFFF
Banks_2_3_OK
		DW    0x3FFF; Banks 2 & 3 Invalides
		
		ORG   0x800

; -----------------------------------------------------------------
; Boot program on page 0 or 2 and download in flash
; -----------------------------------------------------------------

Start_Flash
		goto Start_Flash_2
		
Ecriture_Flash	
		IF LCD
		movlw 0x01; erase the display
		call SendINS
		ENDIF
		movlw LOW Banks_2_3_OK
		movwf Counter2; Flash Address Weight
		movlw HIGH Banks_2_3_OK
		movwf Counter; Flash address strong weight
		movlw 0xFF
		movwf BUFFER_FLASH
		movlw 0x3F
		movwf BUFFER_FLASH+1
		movlw 'W'; validation
		call Write_Flash; Writing 2 byte => bad program (0x3fff) in 0x0fff
		clrf Value; checkout
		clrf Counter2; Flash Address Weight
		clrf Counter; Flash address strong weight
		bsf Counter,4; 2-3 pages (0x1000 - 0x1fff)
Loop_Flash_1
				btfss PIR1,RCIF
					goto Loop_Flash_1
				movf RCREG,W
				andlw 0x3F
				movwf BUFFER_FLASH+1; strong weight
				addwf Value,F
Loop_Flash_2
				btfss PIR1,RCIF
					goto Loop_Flash_2
				movf RCREG,W
				movwf BUFFER_FLASH; low weight
				addwf Value,F
				movlw 'W'; validation
				call Write_Flash; Writing 2 bytes
				btfss Counter2,2
					bcf PORTB,LEDGREEN; turn on the two LEDs
				btfss Counter2,2
					bcf PORTB,LEDYELLOW
				btfsc Counter2,2
					bsf PORTB,LEDGREEN; turn off the two LEDs
				btfsc Counter2,2
					bsf PORTB,LEDYELLOW
				IF LCD	
				movf Counter,W
				movwf TEMP4; save Counter
				movlw 0x80; line 1, column 1
				call SendINS
				movf TEMP4,W; Flash address strong weight
				call Send_Hexa_Lcd
				movf Counter2,W; Flash Address Weight
				call Send_Hexa_Lcd
				movf TEMP4,W
				movwf Counter
				ENDIF
				incf Counter2,F
				btfss STATUS,Z
			goto Loop_Flash_1
			incf Counter,F
			btfss Counter,5
		goto Loop_Flash_1
Loop_Flash_3
		btfss PIR1,RCIF
			goto Loop_Flash_3
		movf RCREG,W; Reading Checksum
		subwf Value,W; Checksum bytes in prevenue of the Serie Port
		btfss STATUS,Z
			goto Fin_Prog_Flag; bad checksum
		clrf PARITY; checkout
		clrf Counter2; Flash Address Weight
		clrf Counter; Flash address strong weight
		bsf Counter,4; 2-3 pages (0x1000 - 0x1fff)
Loop_Flash_4                                               
				call Read_Flash; Reading 2 bytes
				movf BUFFER_FLASH+1,W; strong weight
				addwf PARITY,F
				movf BUFFER_FLASH,W; low weight
				addwf PARITY,F
				incf Counter2,F
				btfss STATUS,Z
			goto Loop_Flash_4
			incf Counter,F
			btfss Counter,5
		goto Loop_Flash_4
		movf PARITY,W; Reading Checksum bytes Flash
		subwf Value,W
		btfss STATUS,Z
			goto Fin_Prog_Flag; bad checksum
		movlw LOW Banks_2_3_OK
		movwf Counter2; Flash Address Weight
		movlw HIGH Banks_2_3_OK
		movwf Counter; Flash address strong weight
		clrf BUFFER_FLASH
		clrf BUFFER_FLASH+1
		movlw 'W'; validation
		call Write_Flash; Writing 2 bytes => OK program (0) in 0x0fff
		IF LCD
		movlw ' '
		call SendCHAR
		movlw 'O'
		call SendCHAR
		movlw 'K'
		call SendCHAR
		ENDIF
Fin_Prog_Flag
		clrf PCLATH
		clrf STATUS
		goto Reset_Prog

Read_Flash
		READ_FLASH Counter,Counter2,BUFFER_FLASH
		return
		
Write_Flash
		sublw 'W'
		btfss STATUS,Z
			return
		WRITE_FLASH Counter,Counter2,BUFFER_FLASH
                return

Start_Flash_2
		clrf INTCON; prohibited interruptions
		bsf STATUS,IRP; 2-3 pages
		movlw 0xA0; page 3
		movwf FSR
Init_Page_3
			clrf INDF; Ram initialization
			incf FSR,F
			btfsc FSR,7
		goto Init_Page_3
		movlw 0x20; Page 2
		movwf FSR
Init_Page_2
			clrf INDF
			incf FSR,F
			btfss FSR,7
		goto Init_Page_2
		bcf STATUS,IRP; 0-1 pages
		movlw 0xA0; page 1
		movwf FSR
Init_Page_1
			clrf INDF
			incf FSR,F
			btfsc FSR,7
		goto Init_Page_1
		movlw Status_Boot+1; page 0
		movwf FSR
Init_Page_0
			clrf INDF
			incf FSR,F
			btfss FSR,7
		goto Init_Page_0
		bsf STATUS,RP0; page 1
		bcf STATUS,RP1
		movlw 0xFF
		movwf TRISC; 8 inputs
		bcf STATUS,RP0; page 0
		clrf PCLATH
		clrf Info_Boot
		btfss PORTC,4; Fire Joystick 1
			goto Startup; Force Launch Program on page 0
		movlw LOW Banks_2_3_OK
		movwf Counter2; Flash Address Weight
		movlw HIGH Banks_2_3_OK
		movwf Counter; Flash address strong weight
		bsf PCLATH,3; Page 1 (0x800 - 0xfff)
		call Read_Flash; Reading 2 bytes in 0x0fff
		bcf PCLATH,3; page 0
		movf BUFFER_FLASH,W
		iorwf BUFFER_FLASH+1,W
		btfss STATUS,Z
			goto Startup; Lance program on page 0 if program on page 2 Invalide (0x1000 - 0x17ff)
		movlw 0xFF
		movwf Info_Boot
		bsf PCLATH,4; Page 2 (0x1000 - 0x17ff)
		goto Startup; Lance program on page 2 valid (0x1000 - 0x17ff)

; -----------------------------------------------------------------

		ORG   0x2100
EEProm

; -----------------------------------------------------------------
; Scan-coded mouse storage area
; These codes are the defect values ​​to be returned
; -----------------------------------------------------------------
Tab_Scan_Codes
		DE 0x00; Offset + 0x00 Never use: "Error or Buffer Overflow"
		DE 0x00; Offset + 0x01 never used
		DE 0x00; Offset + 0x02 never used
		DE 0x00; Offset + 0x03 never used
		DE 0x00; Offset + 0x04 never used
		DE 0x00; Offset + 0x05 never used
		DE 0x00; Offset + 0x06 never used
		DE 0x3B; Offset + 0x07 F1
		DE 0x01; Offset + 0x08 ESC
		DE 0x00; Offset + 0x09 never used
		DE 0x00; Offset + 0x0a never used
		DE 0x00; Offset + 0x0b never used
		DE 0x00; Offset + 0x0c never used
		DE 0x0F; Offset + 0x0d Tabulation
		DE DEF_RUSSE; Offset + 0x0e <2> (`) (next 1)
		DE 0x3C; Offset + 0x0f F2
		DE 0x00; Offset + 0x10 never used
		DE 0x1D; Offset + 0x11 Left Ctrl (Atari in only one)
		DE 0x2A; Offset + 0x12 Left Shift
		DE 0x60; Offset + 0x13> <
		DE 0x3A; Offset + 0x14 Caps
		DE 0x10; Offset + 0x15 A (Q)
		DE 0x02; Offset + 0x16 1
		DE 0x3D; Offset + 0x17 F3
		DE 0x00; Offset + 0x18 never used
		DE DEF_ALTGR; Offset + 0x19 LEFT ALT (Atari En has only one)
		DE 0x2C; Offset + 0x1a W (Z)
		DE 0x1F; Offset + 0x1b s
		DE 0x1E; Offset + 0x1c Q (A)
		DE 0x11; Offset + 0x1d Z (W)
		DE 0x03; Offset + 0x1e 2
		DE 0x3E; Offset + 0x1f F4
		DE 0x00; Offset + 0x20 never used
		DE 0x2E; Offset + 0x21 C
		DE 0x2D; Offset + 0x22 x
		DE 0x20; Offset + 0x23 D
		DE 0x12; Offset + 0x24 e
		DE 0x05; Offset + 0x25 4
		DE 0x04; Offset + 0x26 3
		DE 0x3F; Offset + 0x27 F5
		DE 0x00; Offset + 0x28 never used
		DE 0x39; Offset + 0x29 Space Bar
		DE 0x2F; Offset + 0x2a V
		DE 0x21; Offset + 0x2b F
		DE 0x14; Offset + 0x2c t
		DE 0x13; Offset + 0x2d R
		DE 0x06; Offset + 0x2e 5
		DE 0x40; Offset + 0x2f F6
		DE 0x00; Offset + 0x30 never used
		DE 0x31; Offset + 0x31 n
		DE 0x30; Offset + 0x32 B
		DE 0x23; Offset + 0x33 h
		DE 0x22; Offset + 0x34 g
		DE 0x15; Offset + 0x35 y
		DE 0x07; Offset + 0x36 6
		DE 0x41; Offset + 0x37 F7
		DE 0x00; Offset + 0x38 never used
		DE DEF_ALTGR; Offset + 0x39 Right Alt Gr (Atari En has only one)
		DE 0x32; Offset + 0x3a <,> (m)
		DE 0x24; Offset + 0x3b J
		DE 0x16; Offset + 0x3c U
		DE 0x08; Offset + 0x3d 7
		DE 0x09; Offset + 0x3e 8
		DE 0x42; Offset + 0x3f F8
		DE 0x00; Offset + 0x40 never used
		DE 0x33; Offset + 0x41 <;> (,)
		DE 0x25; Offset + 0x42 k
		DE 0x17; Offset + 0x43 I
		DE 0x18; Offset + 0x44 o (Letter O)
		DE 0x0B; Offset + 0x45 0 (Zero figure)
		DE 0x0A; Offset + 0x46 9
		DE 0x43; Offset + 0x47 F9
		DE 0x00; Offset + 0x48 never used
		DE 0x34; Offset + 0x49 <:> (.)
		DE 0x35; Offset + 0x4a <!> (/)
		DE 0x26; Offset + 0x4b L
		DE 0x27; Offset + 0x4c m (;)
		DE 0x19; Offset + 0x4d P
		DE 0x0C; Offset + 0x4e <)> (-)
		DE 0x44; Offset + 0x4f F10
		DE 0x00; Offset + 0x50 never used
		DE 0x00; Offset + 0x51 never used
		DE 0x28; Offset + 0x52 <�> (')
		DE 0x2B; Offset + 0x53 <*> (\) Key on Compaq
		DE 0x1A; Offset + 0x54 <^> ([)
		DE 0x0D; Offset + 0x55 <=> (=)
		DE 0x62; Offset + 0x56 F11 <= Help Atari (FR)
		DE DEF_PRINTSCREEN; Offset + 0x57 Print Screen
		DE 0x1D; Offset + 0x58 right ctrl (Atari in only one)
		DE 0x36; Offset + 0x59 Right Shift
		DE 0x1C; Offset + 0x5a Return
		DE 0x1B; Offset + 0x5b <$> (])
		DE 0x2B; Offset + 0x5c <*> (\) Soft Key key
		DE 0x00; Offset + 0x5d never used
		DE DEF_F12; Offset + 0x5e F12 <= Undo Atari (FR)
		DE DEF_SCROLL; Offset + 0x5f Scroll
		DE 0x50; Offset + 0x60 Down Arrow
		DE 0x4B; Offset + 0x61 Left Arrow
		DE DEF_PAUSE; Offset + 0x62 break
		DE 0x48; Offset + 0x63 Up Arrow
		DE 0x53; Offset + 0x64 Delete
		DE 0x55; Offset + 0x65 END
		DE 0x0E; Offset + 0x66 backspace
		DE 0x52; Offset + 0x67 insert
		DE 0x00; Offset + 0x68 never used
		DE 0x6D; Offset + 0x69 kp 1
		DE 0x4D; Offset + 0x6a Right Arrow
		DE 0x6A; Offset + 0x6b KP 4)
		DE 0x67; Offset + 0x6c KP 7
		DE DEF_PAGEDN; Offset + 0x6d Page Down (Unused on Atari Before)
		DE 0x47; Offset + 0x6e Clear Home
		DE DEF_PAGEUP; Offset + 0x6f Page Up (Unused on Atari Before)
		DE 0x70; Offset + 0x70 kp 0 (Zero)
		DE 0x71; Offset + 0x71 kp.
		DE 0x6E; Offset + 0x72 kp 2
		DE 0x6B; Offset + 0x73 kp 5
		DE 0x6C; Offset + 0x74 kp 6
		DE 0x68; Offset + 0x75 kp 8)
		DE DEF_VERRNUM; Offset + 0x76 Verr Num (Unused on Atari Before)
		DE 0x65; Offset + 0x77 kp /
		DE 0x00; Offset + 0x78 never used
		DE 0x72; Offset + 0x79 kp enter
		DE 0x6F; Offset + 0x7a KP 3
		DE 0x00; Offset + 0x7b never used
		DE 0x4E; Offset + 0x7c KP +
		DE 0x69; Offset + 0x7d KP 9
		DE 0x66; Offset + 0x7e KP *
		DE DEF_SLEEP; Offset + 0x7f Sleep Eiffel 1.0.5
		DE DEF_POWER; Offset + 0x80 never used Eiffel 1.0.8
		DE DEF_WAKE; Offset + 0x81 never used Eiffel 1.0.8
		DE 0x00; Offset + 0x82 never used
		DE 0x00; Offset + 0x83 never used
		DE 0x4A; Offset + 0x84 kp -
		DE 0x00; Offset + 0x85 never used
		DE 0x00; Offset + 0x86 never used
		DE 0x00; Offset + 0x87 never used
		DE 0x00; Offset + 0x88 never used
		DE 0x00; Offset + 0x89 never used
		DE 0x00; Offset + 0x8a never used
		DE DEF_WINLEFT; Offset + 0x8b Left Win
		DE DEF_WINRIGHT; Offset + 0x8c Right Win
		DE DEF_WINAPP; Offset + 0x8d Popup Win
		DE 0x00; Offset + 0x8e never used
		DE 0x00; Offset + 0x8f never used
; -----------------------------------------------------------------
Tab_Mouse
		DE DEF_WHEELUP; Offset + 0x90 AT_WHEELUP EIFFEL 1.0.0
		DE DEF_WHEELDN; Offset + 0x91 at_Wheeldown Eiffel 1.0.1 (V1.0.0 returned 0x60)
		DE DEF_WHEELLT; Offset + 0x92 at_Wheelleft Eiffel 1.0.3 Only
		DE DEF_WHEELRT; Offset + 0x93 at_Wheelright Eiffel 1.0.3 Only
		DE DEF_BUTTON3; Offset + 0x94 at_Button3 Eiffel 1.0.3 Scan-Code button 3 central
		DE DEF_BUTTON4; Offset + 0x95 at_Button4 Eiffel 1.0.3 Scan-Code button 4
		DE DEF_BUTTON5; Offset + 0x96 at_Button5 Eiffel 1.0.3 Scan-Code button 5
		DE DEF_WHREPEAT; Offset + 0x97 Adr_Wheelrepeat Eiffel 1.0.3 Repetition number

		DE "Eiffel 3"
		DE " LCD    "
; -----------------------------------------------------------------
Tab_Temperature
		DE 40; Maxi threshold for Eiffel 1.0.4 temperature
		DE 35; Mini eiffel temperature threshold 1.0.4
Tab_CTN
		DE 27; 2700 ohms Eiffel 1.0.6
		DE 64; 63.6 DEG C
		DE 33; 3300 ohms
		DE 57; 57.5 DEG C
		DE 39; 3900 ohms
		DE 52; 52.3 DEG C
		DE 47; 4700 ohms
		DE 47; 46.7 DEG C
		DE 56; 5600 ohms
		DE 41; 41.5 DEG C
		DE 68; 6800 ohms
		DE 36; 35.7 DEG C
		DE 82; 8200 ohms
		DE 30; 30.5 DEG C
		DE 100; 10,000 ohms
		DE 25; 25.1 DEG C
		DE 120; 12,000 ohms
		DE 20; 20.1 DEG C
		DE 150; 15,000 ohms
		DE 15; 14.7 DEG C
		DE 180; 18000 ohms
		DE 11; 10.6 DEG C
		DE 220; 22000 ohms
		DE 6; 5.6 DEG C
; -----------------------------------------------------------------
Tab_Config
		DE 2; Game 2 or 3 Eiffel keyboard 1.0.5
; -----------------------------------------------------------------
		END
