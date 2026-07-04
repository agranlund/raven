;*****************************************************************************
;
; EQUATES for 56303 I/O registers and ports
;
; Last update: June 11 1995
;
;*****************************************************************************
;page132,55,0,0,0
;optmex
;ioequ ident 1,0


;------------------------------------------------------------------------
;
; A.2 Equates for I/O Port Programming
;
;------------------------------------------------------------------------

; Register Addresses
;------------------------------------------------------------------------
M_HDR       EQU $FFFFC9     ; Host port GPIO data Register
M_HDDR      EQU $FFFFC8     ; Host port GPIO direction Register
M_PCRC      EQU $FFFFBF     ; Port C Control Register
M_PRRC      EQU $FFFFBE     ; Port C Direction Register
M_PDRC      EQU $FFFFBD     ; Port C GPIO Data Register
M_PCRD      EQU $FFFFAF     ; Port D Control register
M_PRRD      EQU $FFFFAE     ; Port D Direction Data Register
M_PDRD      EQU $FFFFAD     ; Port D GPIO Data Register
M_PCRE      EQU $FFFF9F     ; Port E Control register
M_PRRE      EQU $FFFF9E     ; Port E Direction Register
M_PDRE      EQU $FFFF9D     ; Port E Data Register
M_OGDB      EQU $FFFFFC     ; OnCE GDB Register

;------------------------------------------------------------------------
;
; A.3 Host Interface (HI08) Equates
;
;------------------------------------------------------------------------

; Register Addresses

M_HCR       EQU $FFFFC2     ; Host Control Register
M_HSR       EQU $FFFFC3     ; Host Status Register
M_HPCR      EQU $FFFFC4     ; Host Polarity Control Register
M_HBAR      EQU $FFFFC5     ; Host Base Address Register
M_HRX       EQU $FFFFC6     ; Host Receive Register
M_HTX       EQU $FFFFC7     ; Host Transmit Register

; HCR bits definition

M_HRIE      EQU $0          ; Host Receive interrupts Enable
M_HTIE      EQU $1          ; Host Transmit Interrupt Enable
M_HCIE      EQU $2          ; Host Command Interrupt Enable
M_HF2       EQU $3          ; Host Flag 2
M_HF3       EQU $4          ; Host Flag 3

; HSR bits definition

M_HRDF      EQU $0          ; Host Receive Data Full
M_HTDE      EQU $1          ; Host Receive Data Empty
M_HCP       EQU $2          ; Host Command Pending
M_HF0       EQU $3          ; Host Flag 0
M_HF1       EQU $4          ; Host Flag 1

; HPCR bits definition

M_HGEN      EQU $0          ; Host Port GPIO Enable
M_HA8EN     EQU $1          ; Host Address 8 Enable
M_HA9EN     EQU $2          ; Host Address 9 Enable
M_HCSEN     EQU $3          ; Host Chip Select Enable
M_HREN      EQU $4          ; Host Request Enable
M_HAEN      EQU $5          ; Host Acknowledge Enable
M_HEN       EQU $6          ; Host Enable
M_HOD       EQU $8          ; Host Request Open Drain mode
M_HDSP      EQU $9          ; Host Data Strobe Polarity
M_HASP      EQU $A          ; Host Address Strobe Polarity
M_HMUX      EQU $B          ; Host Multiplexed bus select

M_HD_HS     EQU $C          ; Host Double/Single Strobe select
M_HCSP      EQU $D          ; Host Chip Select Polarity
M_HRP       EQU $E          ; Host Request Polarity
M_HAP       EQU $F          ; Host Acknowledge Polarity

;------------------------------------------------------------------------
;
; A.4 Serial Communications Interface (SCI) Equates
;
;------------------------------------------------------------------------

; Register Addresses

M_STXH      EQU $FFFF97     ; SCI Transmit Data Register (high)
M_STXM      EQU $FFFF96     ; SCI Transmit Data Register (middle)
M_STXL      EQU $FFFF95     ; SCI Transmit Data Register (low)
M_SRXH      EQU $FFFF9A     ; SCI Receive Data Register (high)
M_SRXM      EQU $FFFF99     ; SCI Receive Data Register (middle)
M_SRXL      EQU $FFFF98     ; SCI Receive Data Register (low)
M_STXA      EQU $FFFF94     ; SCI Transmit Address Register
M_SCR       EQU $FFFF9C     ; SCI Control Register
M_SSR       EQU $FFFF93     ; SCI Status Register
M_SCCR      EQU $FFFF9B     ; SCI Clock Control Register

; SCI Control Register Bit Flags

M_WDS       EQU $7          ; Word Select Mask (WDS0-WDS3)
M_WDS0      EQU 0           ; Word Select 0
M_WDS1      EQU 1           ; Word Select 1
M_WDS2      EQU 2           ; Word Select 2
M_SSFTD     EQU 3           ; SCI Shift Direction
M_SBK       EQU 4           ; Send Break
M_WAKE      EQU 5           ; Wakeup Mode Select
M_RWU       EQU 6           ; Receiver Wakeup Enable
M_WOMS      EQU 7           ; Wired-OR Mode Select
M_SCRE      EQU 8           ; SCI Receiver Enable
M_SCTE      EQU 9           ; SCI Transmitter Enable
M_ILIE      EQU 10          ; Idle Line Interrupt Enable
M_SCRIE     EQU 11          ; SCI Receive Interrupt Enable
M_SCTIE     EQU 12          ; SCI Transmit Interrupt Enable
M_TMIE      EQU 13          ; Timer Interrupt Enable
M_TIR       EQU 14          ; Timer Interrupt Rate
M_SCKP      EQU 15          ; SCI Clock Polarity
M_REIE      EQU 16          ; SCI Error Interrupt Enable (REIE)

; SCI Status Register Bit Flags

M_TRNE      EQU 0           ; Transmitter Empty
M_TDRE      EQU 1           ; Transmit Data Register Empty
M_RDRF      EQU 2           ; Receive Data Register Full
M_IDLE      EQU 3           ; Idle Line Flag

M_OR        EQU 4           ; Overrun Error Flag
M_PE        EQU 5           ; Parity Error
M_FE        EQU 6           ; Framing Error Flag
M_R8        EQU 7           ; Received Bit 8 (R8) Address

; SCI Clock Control Register

M_CD        EQU $FFF        ; Clock Divider Mask (CD0-CD11)
M_COD       EQU 12          ; Clock Out Divider
M_SCP       EQU 13          ; Clock Prescaler
M_RCM       EQU 14          ; Receive Clock Mode Source Bit
M_TCM       EQU 15          ; Transmit Clock Source Bit

;------------------------------------------------------------------------
;
; A.5 Enhanced Synchronous Serial Interface (ESSI) Equates
;
;------------------------------------------------------------------------

; Register Addresses Of SSI0

M_TX00      EQU $FFFFBC     ; SSI0 Transmit Data Register 0
M_TX01      EQU $FFFFBB     ; SSIO Transmit Data Register 1
M_TX02      EQU $FFFFBA     ; SSIO Transmit Data Register 2
M_TSR0      EQU $FFFFB9     ; SSI0 Time Slot Register
M_RX0       EQU $FFFFB8     ; SSI0 Receive Data Register
M_SSISR0    EQU $FFFFB7     ; SSI0 Status Register
M_CRB0      EQU $FFFFB6     ; SSI0 Control Register B
M_CRA0      EQU $FFFFB5     ; SSI0 Control Register A
M_TSMA0     EQU $FFFFB4     ; SSI0 Transmit Slot Mask Register A
M_TSMB0     EQU $FFFFB3     ; SSI0 Transmit Slot Mask Register B
M_RSMA0     EQU $FFFFB2     ; SSI0 Receive Slot Mask Register A
M_RSMB0     EQU $FFFFB1     ; SSI0 Receive Slot Mask Register B

; Register Addresses Of SSI1

M_TX10      EQU $FFFFAC     ; SSI1 Transmit Data Register 0
M_TX11      EQU $FFFFAB     ; SSI1 Transmit Data Register 1
M_TX12      EQU $FFFFAA     ; SSI1 Transmit Data Register 2
M_TSR1      EQU $FFFFA9     ; SSI1 Time Slot Register
M_RX1       EQU $FFFFA8     ; SSI1 Receive Data Register
M_SSISR1    EQU $FFFFA7     ; SSI1 Status Register
M_CRB1      EQU $FFFFA6     ; SSI1 Control Register B
M_CRA1      EQU $FFFFA5     ; SSI1 Control Register A
M_TSMA1     EQU $FFFFA4     ; SSI1 Transmit Slot Mask Register A
M_TSMB1     EQU $FFFFA3     ; SSI1 Transmit Slot Mask Register B
M_RSMA1     EQU $FFFFA2     ; SSI1 Receive Slot Mask Register A
M_RSMB1     EQU $FFFFA1     ; SSI1 Receive Slot Mask Register B

; SSI Control Register A Bit Flags

M_PM        EQU $FF         ; Prescale Modulus Select Mask (PM0-PM7)
M_PSR       EQU 11          ; Prescaler Range 
M_DC        EQU $1F000      ; Frame Rate Divider Control Mask (DC0-DC7)
M_ALC       EQU 18          ; Alignment Control (ALC)
M_WL        EQU $380000     ; Word Length Control Mask (WL0-WL7)
M_SSC1      EQU 22          ; Select SC1 as TR #0 drive enable (SSC1)

; SSI Control Register B Bit Flags

M_OF        EQU $3          ; Serial Output Flag Mask
M_OF0       EQU 0           ; Serial Output Flag 0
M_OF1       EQU 1           ; Serial Output Flag 1
M_SCD       EQU $1C         ; Serial Control Direction Mask
M_SCD0      EQU 2           ; Serial Control 0 Direction
M_SCD1      EQU 3           ; Serial Control 1 Direction
M_SCD2      EQU 4           ; Serial Control 2 Direction
M_SCKD      EQU 5           ; Clock Source Direction
M_SHFD      EQU 6           ; Shift Direction
M_FSL       EQU $180        ; Frame Sync Length Mask (FSL0-FSL1)
M_FSL0      EQU 7           ; Frame Sync Length 0
M_FSL1      EQU 8           ; Frame Sync Length 1
M_FSR       EQU 9           ; Frame Sync Relative Timing
M_FSP       EQU 10          ; Frame Sync Polarity
M_CKP       EQU 11          ; Clock Polarity
M_SYN       EQU 12          ; Sync/Async Control
M_MOD       EQU 13          ; SSI Mode Select
M_SSTE      EQU $1C000      ; SSI Transmit enable Mask
M_SSTE2     EQU 14          ; SSI Transmit #2 Enable
M_SSTE1     EQU 15          ; SSI Transmit #1 Enable
M_SSTE0     EQU 16          ; SSI Transmit #0 Enable
M_SSRE      EQU 17          ; SSI Receive Enable
M_SSTIE     EQU 18          ; SSI Transmit Interrupt Enable
M_SSRIE     EQU 19          ; SSI Receive Interrupt Enable
M_STLIE     EQU 20          ; SSI Transmit Last Slot Interrupt Enable
M_SRLIE     EQU 21          ; SSI Receive Last Slot Interrupt Enable
M_STEIE     EQU 22          ; SSI Transmit Error Interrupt Enable
M_SREIE     EQU 23          ; SSI Receive Error Interrupt Enable

; SSI Status Register Bit Flags

M_IF        EQU $3          ; Serial Input Flag Mask
M_IF0       EQU 0           ; Serial Input Flag 0
M_IF1       EQU 1           ; Serial Input Flag 1
M_TFS       EQU 2           ; Transmit Frame Sync Flag
M_RFS       EQU 3           ; Receive Frame Sync Flag
M_TUE       EQU 4           ; Transmitter Underrun Error FLag
M_ROE       EQU 5           ; Receiver Overrun Error Flag
M_TDE       EQU 6           ; Transmit Data Register Empty
M_RDF       EQU 7           ; Receive Data Register Full

; SSI Transmit Slot Mask Register A

M_SSTSA     EQU $FFFF       ; SSI Transmit Slot Bits Mask A (TS0-TS15)

; SSI Transmit Slot Mask Register B

M_SSTSB     EQU $FFFF       ; SSI Transmit Slot Bits Mask B (TS16-TS31)

; SSI Receive Slot Mask Register A

M_SSRSA     EQU $FFFF       ; SSI Receive Slot Bits Mask A (RS0-RS15)

; SSI Receive Slot Mask Register B

M_SSRSB     EQU $FFFF       ; SSI Receive Slot Bits Mask B (RS16-RS31)

;------------------------------------------------------------------------
;
; A.6 Exception Processing Equates
;
;------------------------------------------------------------------------

; Register Addresses
M_IPRC      EQU $FFFFFF     ; Interrupt Priority Register Core
M_IPRP      EQU $FFFFFE     ; Interrupt Priority Register Peripheral

; Interrupt Priority Register Core (IPRC)
M_IAL       EQU $7          ; IRQA Mode Mask
M_IAL0      EQU 0           ; IRQA Mode Interrupt Priority Level (low)
M_IAL1      EQU 1           ; IRQA Mode Interrupt Priority Level (high)
M_IAL2      EQU 2           ; IRQA Mode Trigger Mode
M_IBL       EQU $38         ; IRQB Mode Mask
M_IBL0      EQU 3           ; IRQB Mode Interrupt Priority Level (low)
M_IBL1      EQU 4           ; IRQB Mode Interrupt Priority Level (high)
M_IBL2      EQU 5           ; IRQB Mode Trigger Mode
M_ICL       EQU $1C0        ; IRQC Mode Mask
M_ICL0      EQU 6           ; IRQC Mode Interrupt Priority Level (low)
M_ICL1      EQU 7           ; IRQC Mode Interrupt Priority Level (high)
M_ICL2      EQU 8           ; IRQC Mode Trigger Mode
M_IDL       EQU $E00        ; IRQD Mode Mask
M_IDL0      EQU 9           ; IRQD Mode Interrupt Priority Level (low)
M_IDL1      EQU 10          ; IRQD Mode Interrupt Priority Level (high)
M_IDL2      EQU 11          ; IRQD Mode Trigger Mode
M_D0L       EQU $3000       ; DMA0 Interrupt priority Level Mask
M_D0L0      EQU 12          ; DMA0 Interrupt Priority Level (low)
M_D0L1      EQU 13          ; DMA0 Interrupt Priority Level (high)
M_D1L       EQU $C000       ; DMA1 Interrupt Priority Level Mask
M_D1L0      EQU 14          ; DMA1 Interrupt Priority Level (low)
M_D1L1      EQU 15          ; DMA1 Interrupt Priority Level (high)
M_D2L       EQU $30000      ; DMA2 Interrupt priority Level Mask
M_D2L0      EQU 16          ; DMA2 Interrupt Priority Level (low)
M_D2L1      EQU 17          ; DMA2 Interrupt Priority Level (high)
M_D3L       EQU $C0000      ; DMA3 Interrupt Priority Level Mask
M_D3L0      EQU 18          ; DMA3 Interrupt Priority Level (low)
M_D3L1      EQU 19          ; DMA3 Interrupt Priority Level (high)
M_D4L       EQU $300000     ; DMA4 Interrupt priority Level Mask
M_D4L0      EQU 20          ; DMA4 Interrupt Priority Level (low)
M_D4L1      EQU 21          ; DMA4 Interrupt Priority Level (high)
M_D5L       EQU $C00000     ; DMA5 Interrupt priority Level Mask
M_D5L0      EQU 22          ; DMA5 Interrupt Priority Level (low)
M_D5L1      EQU 23          ; DMA5 Interrupt Priority Level (high)

; Interrupt Priority Register Peripheral (IPRP)

M_HPL       EQU $3          ; Host Interrupt Priority Level Mask
M_HPL0      EQU 0           ; Host Interrupt Priority Level (low)
M_HPL1      EQU 1           ; Host Interrupt Priority Level (high)
M_S0L       EQU $C          ; SSI0 Interrupt Priority Level Mask
M_S0L0      EQU 2           ; SSI0 Interrupt Priority Level (low)
M_S0L1      EQU 3           ; SSI0 Interrupt Priority Level (high)
M_S1L       EQU $30         ; SSI1 Interrupt Priority Level Mask
M_S1L0      EQU 4           ; SSI1 Interrupt Priority Level (low)
M_S1L1      EQU 5           ; SSI1 Interrupt Priority Level (high)
M_SCL       EQU $C0         ; SCI Interrupt Priority Level Mask
M_SCL0      EQU 6           ; SCI Interrupt Priority Level (low)
M_SCL1      EQU 7           ; SCI Interrupt Priority Level (high)
M_T0L       EQU $300        ; TIMER Interrupt Priority Level Mask
M_T0L0      EQU 8           ; TIMER Interrupt Priority Level (low)
M_T0L1      EQU 9           ; TIMER Interrupt Priority Level (high)

;---------------------------------------------------------------
;
; A.7 Timer Module Equates
;
;---------------------------------------------------------------

; Register Addresses Of TIMER0

M_TCSR0     EQU $FFFF8F     ; TIMER0 Control/Status Register
M_TLR0      EQU $FFFF8E     ; TIMER0 Load Reg
M_TCPR0     EQU $FFFF8D     ; TIMER0 Compare Register
M_TCR0      EQU $FFFF8C     ; TIMER0 Count Register

; Register Addresses Of TIMER1

M_TCSR1     EQU $FFFF8B     ; TIMER1 Control/Status Register
M_TLR1      EQU $FFFF8A     ; TIMER1 Load Reg
M_TCPR1     EQU $FFFF89     ; TIMER1 Compare Register
M_TCR1      EQU $FFFF88     ; TIMER1 Count Register

; Register Addresses Of TIMER2

M_TCSR2     EQU $FFFF87     ; TIMER2 Control/Status Register
M_TLR2      EQU $FFFF86     ; TIMER2 Load Reg
M_TCPR2     EQU $FFFF85     ; TIMER2 Compare Register
M_TCR2      EQU $FFFF84     ; TIMER2 Count Register
M_TPLR      EQU $FFFF83     ; TIMER Prescaler Load Register
M_TPCR      EQU $FFFF82     ; TIMER Prescaler Count Register

; Timer Control/Status Register Bit Flags

M_TE        EQU 0           ; Timer Enable
M_TOIE      EQU 1           ; Timer Overflow Interrupt Enable
M_TCIE      EQU 2           ; Timer Compare Interrupt Enable
M_TC        EQU $F0         ; Timer Control Mask TC(3:0)
M_INV       EQU 8           ; Inverter Bit
M_TRM       EQU 9           ; Timer Restart Mode
M_DIR       EQU 11          ; Direction Bit
M_DI        EQU 12          ; Data Input
M_DO        EQU 13          ; Data Output
M_PCE       EQU 15          ; Prescaled Clock Enable
M_TOF       EQU 20          ; Timer Overflow Flag
M_TCF       EQU 21          ; Timer Compare Flag

; Timer Prescaler Register Bit Flags

M_PS        EQU $600000     ; Prescaler Source Mask
M_PS0       EQU 21
M_PS1       EQU 22

; Timer Control Bits

M_TC0       EQU 4           ; Timer Control 0
M_TC1       EQU 5           ; Timer Control 1
M_TC2       EQU 6           ; Timer Control 2
M_TC3       EQU 7           ; Timer Control 3


;------------------------------------------------------------------------
;
; A.8 Direct Memory Access (DMA) Equates
;
;------------------------------------------------------------------------

; Register Addresses Of DMA

M_DSTR      EQU $FFFFF4     ; DMA Status Register
M_DOR0      EQU $FFFFF3     ; DMA Offset Register 0
M_DOR1      EQU $FFFFF2     ; DMA Offset Register 1
M_DOR2      EQU $FFFFF1     ; DMA Offset Register 2
M_DOR3      EQU $FFFFF0     ; DMA Offset Register 3

; Register Addresses Of DMA0

M_DSR0      EQU $FFFFEF     ; DMA0 Source Address Register
M_DDR0      EQU $FFFFEE     ; DMA0 Destination Address Register 
M_DCO0      EQU $FFFFED     ; DMA0 Counter
M_DCR0      EQU $FFFFEC     ; DMA0 Control Register

; Register Addresses Of DMA1

M_DSR1      EQU $FFFFEB     ; DMA1 Source Address Register
M_DDR1      EQU $FFFFEA     ; DMA1 Destination Address Register
M_DCO1      EQU $FFFFE9     ; DMA1 Counter
M_DCR1      EQU $FFFFE8     ; DMA1 Control Register

; Register Addresses Of DMA2

M_DSR2      EQU $FFFFE7     ; DMA2 Source Address Register
M_DDR2      EQU $FFFFE6     ; DMA2 Destination Address Register
M_DCO2      EQU $FFFFE5     ; DMA2 Counter
M_DCR2      EQU $FFFFE4     ; DMA2 Control Register

; Register Addresses Of DMA4

M_DSR3      EQU $FFFFE3     ; DMA3 Source Address Register
M_DDR3      EQU $FFFFE2     ; DMA3 Destination Address Register
M_DCO3      EQU $FFFFE1     ; DMA3 Counter
M_DCR3      EQU $FFFFE0     ; DMA3 Control Register

; Register Addresses Of DMA4

M_DSR4      EQU $FFFFDF     ; DMA4 Source Address Register
M_DDR4      EQU $FFFFDE     ; DMA4 Destination Address Register
M_DCO4      EQU $FFFFDD     ; DMA4 Counter
M_DCR4      EQU $FFFFDC     ; DMA4 Control Register

; Register Addresses Of DMA5

M_DSR5      EQU $FFFFDB     ; DMA5 Source Address Register
M_DDR5      EQU $FFFFDA     ; DMA5 Destination Address Register
M_DCO5      EQU $FFFFD9     ; DMA5 Counter
M_DCR5      EQU $FFFFD8     ; DMA5 Control Register

; DMA Control Register

M_DSS       EQU $3          ; DMA Source Space Mask
M_DSS0      EQU 0           ; DMA Source Memory space 0
M_DSS1      EQU 1           ; DMA Source Memory space 1
M_DDS       EQU $C          ; DMA Destination Space Mask
M_DDS0      EQU 2           ; DMA Destination Memory Space 0
M_DDS1      EQU 3           ; DMA Destination Memory Space 1
M_DAM       EQU $3f0        ; DMA Address Mode Mask
M_DAM0      EQU 4           ; DMA Address Mode 0
M_DAM1      EQU 5           ; DMA Address Mode 1
M_DAM2      EQU 6           ; DMA Address Mode 2
M_DAM3      EQU 7           ; DMA Address Mode 3
M_DAM4      EQU 8           ; DMA Address Mode 4
M_DAM5      EQU 9           ; DMA Address Mode 5
M_D3D       EQU 10          ; DMA Three Dimensional Mode
M_DRS       EQU $F800       ; DMA Request Source Mask (DRS0-DRS4)
M_DCON      EQU 16          ; DMA Continuous Mode
M_DPR       EQU $60000      ; DMA Channel Priority
M_DPR0      EQU 17          ; DMA Channel Priority Level (low)
M_DPR1      EQU 18          ; DMA Channel Priority Level (high)
M_DTM       EQU $380000     ; DMA Transfer Mode Mask
M_DTM0      EQU 19          ; DMA Transfer Mode 0
M_DTM1      EQU 20          ; DMA Transfer Mode 1
M_DTM2      EQU 21          ; DMA Transfer Mode 2
M_DIE       EQU 22          ; DMA Interrupt Enable bit
M_DE        EQU 23          ; DMA Channel Enable bit

; DMA Status Register

M_DTD       EQU $3F         ;Channel Transfer Done Status MASK
M_DTD0      EQU 0           ; DMA Channel Transfer Done Status 0
M_DTD1      EQU 1           ; DMA Channel Transfer Done Status 1
M_DTD2      EQU 2           ; DMA Channel Transfer Done Status 2
M_DTD3      EQU 3           ; DMA Channel Transfer Done Status 3
M_DTD4      EQU 4           ; DMA Channel Transfer Done Status 4
M_DTD5      EQU 5           ; DMA Channel Transfer Done Status 5
M_DACT      EQU 8           ; DMA Active State
M_DCH       EQU $E00        ; DMA Active Channel Mask
M_DCH0      EQU 9           ; DMA Active Channel 0
M_DCH1      EQU 10          ; DMA Active Channel 1
M_DCH2      EQU 11          ; DMA Active Channel 2


;---------------------------------------------------------------
;
; A.9 Phase Locked Loop (PLL) equates
;
;---------------------------------------------------------------

; Register Addresses Of PLL

M_PCTL      EQU $FFFFFD     ; PLL Control Register

; PLL Control Register

M_MF        EQU $FFF        ; Multiplication Factor Bits Mask (MF0-MF11)
M_DF        EQU $7000       ; Division Factor Bits Mask (DF0-DF2)
M_XTLR      EQU 15          ; XTAL Range select bit
M_XTLD      EQU 16          ; XTAL Disable Bit
M_PSTP      EQU 17          ; STOP Processing State Bit
M_PEN       EQU 18          ; PLL Enable Bit
M_PCOD      EQU 19          ; PLL Clock Output Disable Bit
M_PD        EQU $F00000     ; PreDivider Factor Bits Mask (PD0-PD3)


;---------------------------------------------------------------
;
; A.10 Bus Interface Unit (BIU) Equates
;
;---------------------------------------------------------------

; Register Addresses Of BIU

M_BCR       EQU $FFFFFB     ; Bus Control Register
M_DCR       EQU $FFFFFA     ; DRAM Control Register
M_AAR0      EQU $FFFFF9     ; Address Attribute Register 0
M_AAR1      EQU $FFFFF8     ; Address Attribute Register 1
M_AAR2      EQU $FFFFF7     ; Address Attribute Register 2
M_AAR3      EQU $FFFFF6     ; Address Attribute Register 3
M_IDR       EQU $FFFFF5     ; ID Register

; Bus Control Register

M_BA0W      EQU $1F         ; Area 0 Wait Control Mask (BA0W0-BA0W4)
M_BA1W      EQU $3E0        ; Area 1 Wait Control Mask (BA1W0-BA14)
M_BA2W      EQU $1C00       ; Area 2 Wait Control Mask (BA2W0-BA2W2)
M_BA3W      EQU $E000       ; Area 3 Wait Control Mask (BA3W0-BA3W3)
M_BDFW EQU $1F0000; Default Area Wait Control Mask (BDFW0-BDFW4)
M_BBS       EQU 21          ; Bus State
M_BLH       EQU 22          ; Bus Lock Hold
M_BRH       EQU 23          ; Bus Request Hold

; DRAM Control Register

M_BCW       EQU $3          ; In Page Wait States Bits Mask (BCW0-BCW1)
M_BRW EQU $C; Out Of Page Wait States Bits Mask (BRW0-BRW1)
M_BPS       EQU $300        ; DRAM Page Size Bits Mask (BPS0-BPS1)
M_BPLE      EQU 11          ; Page Logic Enable
M_BME       EQU 12          ; Mastership Enable
M_BRE       EQU 13          ; Refresh Enable
M_BSTR      EQU 14          ; Software Triggered Refresh
M_BRF       EQU $7F8000     ; Refresh Rate Bits Mask (BRF0-BRF7)
M_BRP       EQU 23          ; Refresh prescaler

; Address Attribute Registers

M_BAT       EQU $3          ; External Access Type and Pin Definition Bits

;Mask BAT(1:0)

M_BAAP      EQU 2           ; Address Attribute Pin Polarity
M_BPEN      EQU 3           ; Program Space Enable
M_BXEN      EQU 4           ; X Data Space Enable
M_BYEN      EQU 5           ; Y Data Space Enable
M_BAM       EQU 6           ; Address Muxing
M_BPAC      EQU 7           ; Packing Enable
M_BNC       EQU $F00        ; Number of Address Bits to Compare Mask
M_BAC       EQU $FFF000     ; Address to Compare Bits Mask BAC(11:0)

; control and status bits in SR

M_CP        EQU $c00000     ; mask for CORE-DMA priority bits in SR
M_CA        EQU 0           ; Carry
M_V         EQU 1           ; Overflow
M_Z         EQU 2           ; Zero
M_N         EQU 3           ; Negative
M_U         EQU 4           ; Unnormalized
M_E         EQU 5           ; Extension
M_L         EQU 6           ; Limit
M_S         EQU 7           ; Scaling Bit
M_I0        EQU 8           ; Interrupt Mask Bit 0
M_I1        EQU 9           ; Interrupt Mask Bit 1
M_S0        EQU 10          ; Scaling Mode Bit 0
M_S1        EQU 11          ; Scaling Mode Bit 1
M_SC        EQU 13          ; Sixteen_Bit Compatibility
M_DM        EQU 14          ; Double Precision Multiply
M_LF        EQU 15          ; DO-Loop Flag
M_FV        EQU 16          ; DO-Forever Flag
M_SA        EQU 17          ; Sixteen-Bit Arithmetic
M_CE        EQU 19          ; Instruction Cache Enable
M_SM        EQU 20          ; Arithmetic Saturation
M_RM        EQU 21          ; Rounding Mode
M_CP0       EQU 22          ; bit 0 of priority bits in SR
M_CP1       EQU 23          ; bit 1 of priority bits in SR

; control and status bits in OMR

M_CDP       EQU $300        ; mask for CORE-DMA priority bits in OMR
M_MA        EQU 0           ; Operating Mode A
M_MB        EQU 1           ; Operating Mode B
M_MC        EQU 2           ; Operating Mode C
M_MD        EQU 3           ; Operating Mode D
M_EBD       EQU 4           ; External Bus Disable bit in OMR
M_SD        EQU 6           ; Stop Delay
M_MS        EQU 7           ; Memory Switch bit in OMR
M_CDP0      EQU 8           ; bit 0 of priority bits in OMR
M_CDP1      EQU 9           ; bit 1 of priority bits in OMR
M_BEN       EQU 10          ; Burst Enable
M_TAS       EQU 11          ; TA Synchronize Select
M_BRT       EQU 12          ; Bus Release Timing
M_ATE       EQU 15          ; Address Tracing Enable bit in OMR.
M_XYS       EQU 16          ; Stack Extension space select bit in OMR.
M_EUN       EQU 17          ; Extended stack UNderflow flag in OMR.
M_EOV       EQU 18          ; Extended stack OVerflow flag in OMR.
M_WRP       EQU 19          ; Extended WRaP flag in OMR.
M_SEN       EQU 20          ; Stack Extension Enable bit in OMR.

;---------------------------------------------------------------
;
; A.11 Interrupt Equates
;
;---------------------------------------------------------------

;page132,55,0,0,0
;optmex
;intequ ident 1,0
;if @DEF(I_VEC)
;leave user definition as is.
;else
I_VEC       EQU $0
;endif

;---------------------------------------------------------------
; Non-Maskable interrupts
;---------------------------------------------------------------
I_RESET     EQU I_VEC+$00   ; Hardware RESET
I_STACK     EQU I_VEC+$02   ; Stack Error
I_ILL       EQU I_VEC+$04   ; Illegal Instruction
I_DBG       EQU I_VEC+$06   ; Debug Request
I_TRAP      EQU I_VEC+$08   ; Trap
I_NMI       EQU I_VEC+$0A   ; Non Maskable Interrupt
;---------------------------------------------------------------
; Interrupt Request Pins
;---------------------------------------------------------------
I_IRQA      EQU I_VEC+$10   ; IRQA
I_IRQB      EQU I_VEC+$12   ; IRQB
I_IRQC      EQU I_VEC+$14   ; IRQC
I_IRQD      EQU I_VEC+$16   ; IRQD
;---------------------------------------------------------------
; DMA Interrupts
;---------------------------------------------------------------
I_DMA0      EQU I_VEC+$18   ; DMA Channel 0
I_DMA1      EQU I_VEC+$1A   ; DMA Channel 1
I_DMA2      EQU I_VEC+$1C   ; DMA Channel 2
I_DMA3      EQU I_VEC+$1E   ; DMA Channel 3
I_DMA4      EQU I_VEC+$20   ; DMA Channel 4
I_DMA5      EQU I_VEC+$22   ; DMA Channel 5

;---------------------------------------------------------------
; Timer Interrupts
;---------------------------------------------------------------
I_TIM0C     EQU I_VEC+$24   ; TIMER 0 compare
I_TIM0OF    EQU I_VEC+$26   ; TIMER 0 overflow
I_TIM1C     EQU I_VEC+$28   ; TIMER 1 compare
I_TIM1OF    EQU I_VEC+$2A   ; TIMER 1 overflow
I_TIM2C     EQU I_VEC+$2C   ; TIMER 2 compare
I_TIM2OF    EQU I_VEC+$2E   ; TIMER 2 overflow
;---------------------------------------------------------------
; ESSI Interrupts
;---------------------------------------------------------------
I_SI0RD     EQU I_VEC+$30   ; ESSI0 Receive Data
I_SI0RDE    EQU I_VEC+$32   ; ESSI0 Receive Data With Exception Status
I_SI0RLS    EQU I_VEC+$34   ; ESSI0 Receive last slot
I_SI0TD     EQU I_VEC+$36   ; ESSI0 Transmit data
I_SI0TDE    EQU I_VEC+$38   ; ESSI0 Transmit Data With Exception Status
I_SI0TLS    EQU I_VEC+$3A   ; ESSI0 Transmit last slot
I_SI1RD     EQU I_VEC+$40   ; ESSI1 Receive Data
I_SI1RDE    EQU I_VEC+$42   ; ESSI1 Receive Data With Exception Status
I_SI1RLS    EQU I_VEC+$44   ; ESSI1 Receive last slot
I_SI1TD     EQU I_VEC+$46   ; ESSI1 Transmit data
I_SI1TDE    EQU I_VEC+$48   ; ESSI1 Transmit Data With Exception Status
I_SI1TLS    EQU I_VEC+$4A   ; ESSI1 Transmit last slot
;---------------------------------------------------------------
; SCI Interrupts
;---------------------------------------------------------------
I_SCIRD     EQU I_VEC+$50   ; SCI Receive Data
I_SCIRDE    EQU I_VEC+$52   ; SCI Receive Data With Exception Status
I_SCITD     EQU I_VEC+$54   ; SCI Transmit Data
I_SCIIL     EQU I_VEC+$56   ; SCI Idle Line
I_SCITM     EQU I_VEC+$58   ; SCI Timer
;---------------------------------------------------------------
; HOST Interrupts
;---------------------------------------------------------------
I_HRDF      EQU I_VEC+$60   ; Host Receive Data Full
I_HTDE      EQU I_VEC+$62   ; Host Transmit Data Empty
I_HC        EQU I_VEC+$64   ; Default Host Command
;---------------------------------------------------------------
; INTERRUPT ENDING ADDRESS
;---------------------------------------------------------------
I_INTEND    EQU I_VEC+$FF   ; last address of interrupt vector space
