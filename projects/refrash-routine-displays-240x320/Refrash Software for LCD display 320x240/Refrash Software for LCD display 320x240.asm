.Equ Frecventa                  = 16000000
.Equ Inceput_Stiva              = 4096+256-1

.Equ RezolutionX                = 240
.Equ RezolutionY                = 320
.Equ Delay                      = 0

.Equ DisplayClk                 = 0
.Equ DisplayLP                  = 1
.Equ DisplaySPL                 = 2
.Equ DisplayCLS                 = 3
.Equ DisplayPS                  = 4
.Equ DisplaySPS                 = 5
.Equ DisplayVCOM                = 6
.Equ DisplayMOD                 = 7

.Equ Rosu     = 0
.Equ Galben   = 1
.Equ Albastru = 2
;********************************************************************************************************************************************************
.NOLIST
.INCLUDE "m64def.inc"
.LIST

.org 0
    Rjmp Reset
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
.Org INT_VECTORS_SIZE
Reset:   
	Cli
    Ldi R16,Low(Inceput_stiva)
	Out spl,R16
	Ldi R16,High(Inceput_stiva)
	Out sph,R16

	Ser R17
	Sts DDRF,R17
	Clr R17
	Sbr R17,1<<DisplayPS
	Sbr R17,1<<DisplaySPS
	Sbr R17,1<<DisplayVCOM
	Sts PORTF,R17
	Ldi R16,4
	Rcall Slep
	Sbr R17,1<<DisplayMOD
	Sts PORTF,R17
	Ser R16
	Out DDRA,R16
	Cbi PORTA,Rosu
	Cbi PORTA,Galben
	Sbi PORTA,Albastru
	Clr Xh
	Clr Xl
	Clr Yh
	Clr Yl
	Clr R18
	Clr R20

	Ldi R16,4
	Rcall Slep
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************

Idle:
    Cbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplayClk
	Sts PORTF,R17
	Rcall RisingEdgeDisplay
	;Rcall FallingEdgeDisplay
	Rjmp Idle
;********************************************************************************************************************************************************
NuEsteCompareX1:
    Ret
RisingEdgeDisplay:
    Ser R16
	Out DDRA,R16
	Inc R18
	Out PORTA,Yl

	Adiw Xl:Xh,1
	Cpi Xh,High(RezolutionX+Delay+1)
    Brne NuEsteCompareX1
	Cpi Xl,Low(RezolutionX+Delay+1)
	Brne NuEsteCompareX1
	Cbr R17,1<<DisplayCLS;CLS->>Low
	Sbr R17,1<<DisplayPS;PS->>High
	Sts PORTF,R17
;**************************
	Cbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplayLP;LP->>High
	Sts PORTF,R17
;**************************
	Cbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplayClk
	Sts PORTF,R17
	Cbr R17,1<<DisplayLP;Lp->>Low
	Sts PORTF,R17
;**************************
	Cbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplaySPL;SPL->>High
	Sbr R17,1<<DisplayCLS;CLS->>High
	Cbr R17,1<<DisplayPS;PS->>Low
    Sts PORTF,R17
;**************************
    Cbr R17,1<<DisplayClk
	Sts PORTF,R17
	Sbr R17,1<<DisplayClk
	Sts PORTF,R17
	Cbr R17,1<<DisplaySPL;SPL->>Low
	
	Sts PORTF,R17
	Clr Xh
	Clr Xl
;**************************
	Adiw Yl:Yh,1
	Cpi Yh,High(RezolutionY-1)
    Brne NuEsteCompareYSPSLow
	Cpi Yl,Low(RezolutionY-1)
	Brne NuEsteCompareYSPSLow
    Cbr R17,1<<DisplaySPS;SPS->>Low
	Sts PORTF,R17
	Ret
NuEsteCompareYSPSLow:
    Cpi Yh,High(RezolutionY+0)
    Brne NuEsteCompareYSPSHigh
	Cpi Yl,Low(RezolutionY+0)
	Brne NuEsteCompareYSPSHigh
    Sbr R17,1<<DisplaySPS;SPS->>High
	Sts PORTF,R17

	Clr Yh
	Clr Yl
NuEsteCompareYSPSHigh:
;**************************
NuEsteCompareXSplLow:
;**************************
	Ret
;********************************************************************************************************************************************************
FallingEdgeDisplay:
    Ret

;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
;********************************************************************************************************************************************************
Slep:
    Push R16
	Push Xl
	Push Xh
Sleep3_:
	Ldi Xl,Low(65535)
	Ldi Xh,High(65535)
Sleep2s_:
    Wdr
	Nop 
	Sbiw Xl:Xh,1
	Cpi Xl,0
	Brne Sleep2s_
	Cpi Xh,0
	Brne Sleep2s_
	Dec R16
	Brne Sleep3_
	Pop Xh
	Pop Xl
	Pop R16
	Ret
