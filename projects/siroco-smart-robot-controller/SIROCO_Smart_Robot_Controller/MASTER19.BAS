'//////////////////////////////////////////////////////////
'///    ROBOT MASTER 019                                ///
'///    VERSION ROBOT  B  12/10/2003                      ///
'///    GLU                                             /// 
'//////////////////////////////////////////////////////////
' FASTAVR 3.1.7
' STUDIO  3.53
' PROGRAMMER 1.33 SERIAL
'            D:\AVR_SERIAL_ISP\APROGWIN\AVRPROG
' DIM Bit, Byte, Integer, Word, Long or Float
'
' 1561 lines 
' 4037 Words
' 164 SRAM Free
'
' -- HARDWARE BUG REPORT --
' Pas de connection entre AGND et DGND
' Faire une masse puissance PGND séparée
' Inversion des sorties PX
' Mettre la LED sur PB3
' Relier AGND CPU à AGND board         !!!!
' Relier la LED à PBX                  !!!!
' Utilisser un boitier PDIP
' Ne pas utilisser servo YO
'
' MASTER 11 suppresion des deux modes de vitesses
' MASTER 12 introduction PID et simplification de l'interpreter
' MASTER 13 simplification de l'interpreter
' MASTER 14 simplification de l'interpreter
' MASTER 15 B ON PASSE EN MODE BETA
' MASTER 16 B optimisation de l'interpreter
' MASTER 17 B SCRIPT LOAD OK
' MASTER 18 B 99 % OK
'
'-------------------- CARTE BX II -----------------------
' RS232
' 8 Analog  IN                      PA
' 8 Digital IN                      PB sauf PB2 et PB3
' 4 Power OUT                       PC
' 2 Motors with DIR and PWM         PC + PWM1A et PWM1B ( PD )
' Int0 et Int1                      PD
' 8 SERVO                           PD
' I2C                               PC
' IR RX and TX  
'
'
' Port A  Analog Inputs
'        PA0 Analog      V ALIM 
'        PA1 Analog 
'        PA2 Analog 
'        PA3 Analog 
'        PA4 Analog 
'        PA5 Analog 
'        PA6 Analog 
'        PA7 Analog 
'
' Port B  General Inputs  // sauf PB2
'        PB0 IN    CODEUR 1  / BUMPER0     /   DIGITAL OUT
'        PB1 IN    CODEUR 2  / BUMPER1     /   DIGITAL OUT
'        PB2 IN    IR TX                   /   DIGITAL OUT
'        PB3 IN    LED                     /   DIGITAL OUT
'        PB4 IN    DIGITAL IN 1             OK
'        PB5 IN    DIGITAL IN 2  ( MOSI )   OK
'        PB6 IN    DIGITAL IN 3  ( MISO )   OK
'        PB7 IN    DIGITAL IN 4  ( SCK  )   OK
'
' Port C  I2C and Power Outputs
'        PC0     SCL   I2C
'        PC1     SDA   I2C
'        PC2     POWER 4        OK
'        PC3     POWER 3        OK
'        PC4     POWER 2        OK
'        PC5     POWER 1        OK
'        PC6     DIR A          OK
'        PC7     DIR B          OK
'
' Port D Special I/O
'        PD0    RS 232 RX       OK
'        PD1    RS 232 TX       OK
'        PD2    INT 0
'        PD3    INT 1
'        PD4    OC1B PWM B      OK
'        PD5    OC1A PWM A      OK   
'        PD6    IR RX
'        PD7    SERVO OUT
'
' EEPROM
' Adresse 0   RESERVED
' Adresse 1   LAN ADDRESS
' Adresse 2   
' Adresse 3   VLOW
' Adresse 4   VMEDIUM
' Adresse 5   VFAST
' Adresse > 32 -> PGM
'
' TIMER 0 Timer 1 mS with Interrupt
' TIMER 1 PWM A and B
' TIMER 2 Timer 8 µS for SERVO 
' 
' -------------- INTERPRETER --------------------------------------------
'
' VALUE = VAR or CONSTANT
'
' HOME
' FORWARD VALUE
' BACKWARD VALUE
' RIGHT VALUE
' LEFT  VALUE
' STOP
' WAIT_TRUE  VAR    I/O, Speed, Timer, Counter or Variable = 0
' WAIT_FALSE VAR
' GOTO LABEL
' GOSUB LABEL
' RETURN
' LET VAR VALUE
' SET VAR             =1
' RESET VAR           =0
' CLEAR VAR
' INC VAR
' DEC VAR
' TEST_TRUE_GOTO   VAR LABEL
' TEST_FALSE_GOTO  VAR LABEL
'
' BEEP VALUE
' RUN
' END
'
'
' Un counter/ digital inputs
'
'
' VAR0   Analog input 0  / ALIM
' VAR1   Analog input 1  
' VAR2   Analog input 2
' VAR3   Analog input 3
' VAR4   Analog input 4
' VAR5   Analog input 5
' VAR6   Analog input 6
' VAR7   Analog input 7
'
' VAR8   Digital input 0  / BUMPER0
' VAR9   Digital input 1  / BUMPER1
' VAR10  Digital Input 2
' VAR11  Digital input 3
' VAR12  Digital input 4
' VAR13  Digital input 5
' VAR14  Digital input 6
' VAR15  Digital input 7
'
' VAR16  Moteur 0 SPEED
' VAR17  Moteur 1 SPEED
'
' VAR18  P1
' VAR19  P2
' VAR20  P3
' VAR21  P4
'
' VAR22  SERVO1     Valeur * 8 µS    125 -> 1000 µS   250 -> 2000 µS
' VAR23  SERVO2
' VAR24  SERVO3
' VAR25  SERVO4
'
' VAR26  TIMER1 
' VAR27  TIMER2
' VAR28  TIMER3
' VAR29  TIMER4
'
' VAR30  COUNTER0
' VAR31  RESERVE
' VAR32  COUNTER1
' VAR33  RESERVE
'
' VAR34  STATUS  
' VAR35  ERROR
'
' VAR36  DIR0
' VAR37  DIR1
'
' VAR38  IRTX
' VAR39  IRRX

' VAR40 WATCHDOG ( 0=OFF, X Value X 100 mS )
' VAR41 RUN (Interpreter) ON/OFF
' VAR42 EBUMPER SECURITY ( ON / OFF )
' VAR43 RESERVE
' VAR44 EPID
'
' VAR45 TEMPS
' VAR46 T0
' VAR47 T1
' VAR48 PCO
' VAR49 RESERVE
'
' VAR50 - > 64 Variables
'
$Device   = 8535			        ' used device 8535
$Stack    = 40						' stack depth 
$Clock    = 8                   	' used crystal 8.0 Mhz     

$Timer0   = Timer,Prescale = 64		' Timer  1 mS Interrupt

$Timer1   = Timer,Prescale = 8              ' PWM A and B      
$Timer1   = PWM, 8, PwmA=Normal,PwmB=Normal ' 8 Bits

$Timer2   = Timer,Prescale = 64             ' Timer 8 µS SERVO
 
$Baud     = 19200              		' 19200 8 BIT NO PARITY 1 STOP
$WatchDog = 2048                    ' environ 2 secondes
$Source   = On			     		' BASIC source in ASM

$Def SDA  = PORTC.1        ' I2C SDA MASTER
$Def SCL  = PORTC.0        ' I2C SCL MASTER

$Sound    = PORTC.5        ' SOUND

' ------------------ DEFINITIONS ----------------------

' PA Analog IN

' PB Digtal IN sauf PB2

$Def CODEUR1 = PORTB.0         
$Def LED1    = PORTB.0   ' ?????????????   

$Def CODEUR2 = PORTB.1           
$Def IRTX    = PORTB.2            
$Def RESERVE = PORTB.3 
$Def LED     = PORTB.3             
$Def D1      = PORTB.4           
$Def D2      = PORTB.5           
$Def D3      = PORTB.6             
$Def D4      = PORTB.7           

' PC Digital OUT
' PC0 PC1  - > I2C
$Def P4   = PORTC.2             ' POWER 1 
$Def P3   = PORTC.3             ' POWER 2
$Def P2   = PORTC.4             ' POWER 3
$Def P1   = PORTC.5             ' POWER 4   / SOUND

$Def PDIR0 = PORTC.6             ' DIR 0 Motor Direction  
$Def PDIR1 = PORTC.7             ' DIR 1

' PD general I/O

Dim PT As Byte              ' for Interupt Timer
Dim PL As Byte              ' for LED

Dim SCAN As Byte            ' for SERVO
Dim ADR As Byte             ' Lan Address

Dim COUNT As Byte           ' PWM Counter
Dim COUNT1 As Byte          ' 

Dim CMD As Byte             ' RX COMMAND

Dim A As Byte               ' General VAR
Dim B As Byte				' A, B, P, T 
Dim C As Byte
Dim D As Byte
Dim E As Byte
Dim F As Byte
Dim P As Byte
Dim T As Byte

Dim A2 As Integer
Dim B2 As Integer
Dim C2 As Integer
Dim D2 As Integer
Dim E2 As Integer
Dim F2 As Integer
Dim P2 As Integer
Dim T2 As Integer

'Dim ALE As Byte              ' RANDOM DATA

Dim RX(32) As Byte           ' RX Buffer
Dim PRX As Byte              ' RX POINTER RECEPTION
Dim PLE As Byte              ' RX POINTER LECTURE
Dim MF As Bit                ' RX Message Ready Flag

Dim CPT0 As Byte          ' Counter INT 0
Dim CPT1 As Byte          ' Counter INT 1
'Dim CPT0R As Byte         ' Counter Report 0
'Dim CPT1R As Byte         ' Counter Report 1

Dim SPEED0 As Byte        ' Motor Speed 1
Dim SPEED1 As Byte        '             2

Dim DIR0 As Byte
Dim DIR1 As Byte

Dim WD As Byte            ' Watchdog SET VALUE
 
Dim TEMPMS As Byte        ' Temps 1 MS    ???
Dim TEMPS  As Byte        ' Temps  S      ???

Dim T0 As Byte            ' Temps INT 0
Dim T1 As Byte            ' Temps INT 1

Dim WD0 As Word           ' Watchdog 1
Dim WD1 As Word           ' Watchdog 2  

Dim VERSION As Byte
Dim REVISION As Byte

Dim ERROR As Byte         ' error CODE
Dim STATUS As Byte        ' STATUS 0=STOP,1=RUN
 
'--------------- INTERPRETER --------------------------
Dim PGM(64) As Byte   

Dim PCO As Byte
Dim TOKEN As Byte
Dim TOKEN2 As Byte

Dim VAR(128) As Byte

Dim MATH1 As Integer
Dim MATH2 As Integer

Dim TEST As Integer

Dim VLOW As Byte
Dim VMEDIUM As Byte
Dim VFAST As Byte

' --------- ROBOTIC PARAM ------------------------------------
Dim PARAM As Integer        ' Parametre
Dim C_TIMER As Integer      ' SETPOINT Timer 
Dim C_COUNTER0 As Integer   ' SETPOINT Counter0
Dim C_COUNTER1 As Integer   ' SETPOINT Counter1

Dim MODE As Byte            ' MODE de DEPLACEMENT 
Dim PIDL As Byte            ' PID LOOP
Dim EPID As Byte            ' ENABLE PID

Dim EBUMPER As Byte         ' ENABLE BUMPER SECURITY
Dim BUMPER0 As Byte         ' BUMPER0 STATE
Dim BUMPER1 As Byte         ' BUMPER1 STATE

Dim OBUMPER0 As Byte         ' OLD BUMPER0 STATE
Dim OBUMPER1 As Byte         ' OLD BUMPER1 STATE

Dim RADDRESS As Byte        ' RETURN ADDRESS

'Const C_ELSE = 1
'Const C_ENDIF = 2
'Const C_LABEL = 3

'Const M_WRITE = 1
'Const M_READ =2
'Const M_STOP=3
'Const M_RUN=4
'Const M_DEBUG=6
'Const M_STEP=7
'Const M_VAR=8

' ------- INTERPRETER -------------
' PUSH VALUE
Const PCONSTANT    =6 ' -> MATH1
Const PCONSTANT2   =7 '
Const PVAR         =8 '
Const PPARAM       =99'
' DIRECTION
Const GFORWARD     =22
Const GBACKWARD    =23
Const GRIGHT       =24 
Const GLEFT        =25
' SPEED
Const GSTOP        =26
Const LOW_SPEED    =27
Const MEDIUM_SPEED =28
Const HIGH_SPEED   =29
Const SET_SPEED    =30 '<- MATH1 
' SET = PARAM
Const SET_TIMERP   =31
Const SET_COUNTER0P=32
Const SET_COUNTER1P=33
Const SET_COUNTERSP=99
' PULL VALUE
Const LET_TIMER    =99 'MATH1     IF 0 -> RND
Const LET_COUNTERS =99 'MATH1     IF 0 -> RND
Const LET_VAR      =99 'MATH1     ( MATH1 )
' CLEAR
Const CLR_TIMER    =39
Const CLR_COUNTERS =99
Const CLR_VAR	   =99 'XX -> 0	

Const SET_VAR      =99 'XX -> 1 
Const INC_VAR      =99 'XX +1
Const DEC_VAR      =99 'XX -1

Const WAIT_TIMER   =34
Const WAIT_COUNTERS=99
Const WAIT_VAR     =99  ' INPUT
Const WAIT_MODE    =99

Const FOREVER      =38
Const GEND         =5 

Const GSOUND       =40 'XX 

Const GGOTO        =1
Const GGOSUB       =3
Const IFGOTO       =2 ' IF TEST THEN GOTO
Const IFGOSUB      =0 ' IF TEST THEN GOSUB
Const GRETURN      =4

'------- VAR ---------------
Const SECURITY =40
Const RUN      =41

'---------------FUNCTIONS and SUBROUTINE---------------
Declare Sub BREAK() 
Declare Sub COMMUNICATION() 
Declare Sub WRITEHEX( X As Byte )
Declare Sub WRITE2HEX( X As Integer)
Declare Sub ACKNO()
Declare Sub NOACK()
Declare Sub SPEED()
Declare Sub PTEST()
Declare Sub DEAD()
Declare Sub INTERPRETER()

Declare Function READHEX() As Byte
Declare Function READ2HEX() As Integer

Declare Sub DOWNLOAD(NUM As Byte)
Declare Sub UPLOAD(NUM As Byte)
Declare Sub PID()
Declare Sub EXECUTE(NUM As Byte)
Declare Sub PARAMETRE()

Declare Interrupt Ovf0()    ' Timer 0 Overflow
Declare Interrupt Urxc()    ' RX Char
Declare Interrupt Int0()    ' INT 0
Declare Interrupt Int1()    ' INT 1

Const SPACE =32  ' " "
Const CR    =13  ' CR
Const LF    =10  ' LF
Const CACK  =62  ' > 
Const CNACK =63  ' ?
Const STX   =83  ' S
Const ETX   =13  ' CR

' SET PORTS A,B,C,D DIR
DDRA  =&h00        			' ALL Analog Input Port
PORTA =&h00                 ' NO PULL UP

DDRB.0=1                    ' CODEUR 1   /  LED GREEN
DDRB.1=0                    ' CODEUR 2
DDRB.2=1                    ' IR TX
DDRB.3=1                    ' RESERVE    /  LED GREEN
DDRB.4=0                    ' D1 I/O SS   Input Port
DDRB.5=0                    ' D2 I/O MOSI Input Port
DDRB.6=0                    ' D3 I/O MISO Input Port
DDRB.7=0                    ' D4 I/O SCK  Input Port

PORTB.0 = 1                 ' PULL UP          
PORTB.1 = 1                
PORTB.2 = 0
PORTB.3 = 0
PORTB.4 = 1
PORTB.5 = 1
PORTB.6 = 1
PORTB.7 = 1

DDRC= 252                   ' 255-1-2=252 I2C and Motors Port

DDRD.2 = 0                  ' INT 0
DDRD.3 = 0                  ' INT 1
DDRD.4 = 1                  ' PWM B
DDRD.5 = 1                  ' PWM A
DDRD.6 = 0                  ' IRRX
DDRD.7 = 1                  ' SERVO

PORTD.2 = 1                 ' INT 0 PULL UP
PORTD.3 = 1                 ' INT 1 PULL UP
PORTD.6 = 1                 ' IRRX  PULL UP   

Int0 Falling
Int1 Falling

Timer0    = &h83             ' Timer 1 mS
Timer1    = &h00             ' Timer PWM 
Timer2    = &h00             ' Timer SERVO

Enable  Ovf0 
Enable  Urxc
Enable  Int0
Enable  Int1
Disable Interrupts

Stop  WatchDog         ' ??
Start Timer0
Start Timer1
Start Timer2
Start Adc

'---------------MAIN INITIALISATION ---------------------
Randomize(123)

ERROR  =0
STATUS =0

VAR(SECURITY)=0
VAR(RUN)=0

COUNT  = 0               ' Timer 0 PWM Count
COUNT1 = 0               ' Timer 0     Count

For P=0 To 30          ' RESET RX Buffer  ??
RX(P)=0
Next 

PRX=0
CMD=0
MF=0

BREAK()              ' INITIALISATION
SPEED()              ' SET SPEED and DIR

ADR    = ReadEE(1)      	' Card LAN ADDRESS

VLOW   = ReadEE(3)
If VLOW=0 Then VLOW=100

VMEDIUM= ReadEE(4)
If VMEDIUM=0 Then VMEDIUM=150

VFAST  = ReadEE(5)
If VFAST=0 Then VFAST=255

'----------------------------------------------
VERSION=1   		' Version
REVISION=19         ' Revision
'-------------- INTERPRETER --------------------------------

PCO=1
MATH1=0
MATH2=0
ERROR=0

Print "ROBOT GLU READY "
Print "ADRESSE "; ADR
Print "Version B "; VERSION ; "." ; REVISION
Print

PL=0
'-----------------MAIN PROGRAM--------------------
Enable Interrupts
Wait 1
'-------------------------- MAIN LOOP --------------------
Do 
 SCAN=0
 Reset WatchDog
 STATUS=1
      	
  For P=0 To 7              ' ANALOG INPUTS  10 Bits
  VAR(P)=Adc8(P)
  Next
 
  For P=0 To 7
  A=PINB.P              ' DIGITAL INPUTS
  VAR(8+P)=Abs(A)
  Next

 EBUMPER=VAR(42)         ' ENABLE BUMPERS
 OBUMPER0=BUMPER0        ' OLD VALUE BUMPER
 OBUMPER1=BUMPER1
 BUMPER0=VAR(8)          ' NEW VALUE BUMPER
 BUMPER1=VAR(9)

  VAR(39)=PIND.6     ' IR RX
    
  If MF=1 Then                 ' EXECUTE AVAILABLE MESSAGE
   COMMUNICATION()
  End If

 SPEED0=VAR(16)
 SPEED1=VAR(17)
 DIR0=VAR(36)
 DIR1=VAR(37)
 WD=VAR(40)

 If VAR(SECURITY)>0 Then
 DEAD()						 ' DETECT DEAD Moteurs bloques
 End If                      ' RESET VARS SPEED

 If EBUMPER=1 Then    'ENABLE BUMPER SECURITY
            
  If BUMPER0 > OBUMPER0 Then ' DETECTION FLANC MONTANT
   SPEED0=0
   SPEED1=0
  End If
  
  If BUMPER1 > OBUMPER1 Then
   SPEED0=0
   SPEED1=0
  End If
  
 End If
  
 SPEED()                     ' SET MOTORS OUTPUTS

 If VAR(18)=1 Then
 Set P1
 Else
 Reset P1
 End If

 If VAR(19)=1 Then
 Set P2
 Else
 Reset P2
 End If

 If VAR(20)=1 Then
 Set P3
 Else
 Reset P3
 End If

 If VAR(21)=1 Then
 Set P4
 Else
 Reset P4
 End If

' SERVO Timer Increment = 8 µS         125 -> 1000 µs, 250 -> 2000 µS
For P=22 To 25
 A=VAR(P)
 Set D1
 Timer2=0         ' ?????????????????????
 ATTENTE:
 B=Timer2
 If B < A Then GoTo ATTENTE
 Reset D1
 WaitUs 5
Next P

If VAR(38)=1 Then         '  IRTX
Set IRTX
Else
Reset IRTX
End If

If VAR(RUN)=1 Then         ' 41
INTERPRETER()
Else
PCO=1
End If

VAR(48)=PCO    ' Program Pointer

VAR(30)=CPT0   ' Counter INT 0
VAR(32)=CPT1   ' Counter INT 1

VAR(34)=STATUS
VAR(35)=ERROR

VAR(45)=TEMPS  ' Timer mS

VAR(46)=T0     ' PERIOD INT0
VAR(47)=T1     ' PERIOD INT1

ATTENTE2:
If SCAN<20 Then GoTo ATTENTE2    ' SCANTIME WAIT 20 mS pour les servos

Incr PL
If PL > 20 Then
 If MODE=0 Then
  Toggle LED       ' Toggle every 400 ms
  Toggle LED1      ' only for test '''''''''''''
 Else
  Reset LED
  Reset LED1
 End If 
 PL=0
End If


' ---Manque que la gestion de I2C---
'
'
'
'----------- GESTION DES MODES D'AVANCE ------------------
' MODE=0  -> NOTHING
' MODE=1  -> TEST TIMER
' MODE=2  -> TEST COUNTER
' MODE=99 -> END 
'
If MODE=1 And C_TIMER=0 Then
  VAR(16)=0
  VAR(17)=0
  MODE=99
End If

If MODE=2 Then
 If C_COUNTER0=0 Then VAR(16)=0
 If C_COUNTER1=0 Then VAR(17)=0
 If C_COUNTER0=0 And C_COUNTER1=0 Then MODE=99
End If 

' ---- PID --------
EPID=VAR(44)

If MODE=2 And EPID=1 Then
 Incr PIDL

  If PIDL>50 Then           ' 1000 mS SCANNING PID
   PIDL=0
   PID()
  End If
End If

If MODE=99 Then
MODE=0
End If
 
Loop 

End
'------------------------------------------------------
' INTERRUPT TIMER 0 Timer every 1 mS 
Interrupt Ovf0(), Save All      ' ALL ??????????????????
Timer0 = &h83                   ' RELOAD 1 mS
	Incr COUNT                
	If COUNT>99 Then
	    COUNT=0
	 	Incr TEMPS             ' every 100 MS
	 	Incr WD0    
	 	Incr WD1
	 	For PT=26 To 29        ' GESTION TIMERS  100 mS
			If VAR(PT)>0 Then
			Decr VAR(PT)
			End If
		Next
	End If
	
Incr SCAN

If C_TIMER>0 Then
Decr C_TIMER
End If
	
End Interrupt
'-------------------------------------------------------
' INTERRUPT RX          S CMD (XX) (YY) CR
Interrupt Urxc(), Save All
If MF=0 Then                    ' PAS REENTRANT
	InputBin CMD                ' STX CMD DATA ETX
	If CMD>96 And CMD<123 Then  ' CONVERT TO UPPERCASE
	CMD=CMD-32
	End If
	RX(PRX)=CMD
	If PRX < 31 Then
	Incr PRX
	Else
	ERROR=100
	PRX=0
	End If
	
	Select Case CMD
		Case STX         		'  "S" START MESSAGE
			PRX=1
		
		Case ETX         		' END MESSAGE  "CR"
			If PRX > 2 Then
				MF = 1          ' MESSAGE READY TO BE EXECUTED 
			Else
				PRX=0
			End If
		
		Case SPACE              ' DO NOT CARE
		    Decr PRX	
		    
		Case Else
			If PRX=1 Then
				PRX=0
			End If
	End Select
End If
End Interrupt

'-------------------------------------------------------
' INTERRUPT INT0
Interrupt Int0(), Save All
	Incr CPT0
'	If CPT0 > 254 Then
'	 	CPT0=0
'	 	Incr CPT0R
'	End If
	T0=WD0
	WD0=0

If C_COUNTER0 > 0 Then
Decr C_Counter0
End If
	
End Interrupt

'-------------------------------------------------------
' INTERRUPT INT1
Interrupt Int1(), Save All
	Incr CPT1 
'	If CPT1 > 254 Then
'	 	CPT1=0
'	 	Incr CPT1R
'	End If 
	T1=WD1
	WD1=0
If C_COUNTER1>0 Then
Decr C_Counter1
End If
	
End Interrupt

'-----------------------------------------------------------------
Sub BREAK()
Local P As Byte

For P=22 To 25       ' RESET SERVO 1500 µS  
VAR(P)=200
Next

VAR(16)=0             ' RESET SPEED
VAR(17)=0

For P=18 To 21        ' RESET PX
VAR(P)=0
Next

End Sub

'----------------- FONCTION --------------------------------------
'   Checksum ??
'
' VAR READ  ( integer )
' VAR WRITE
'
' PGM READ  ( byte )
' PGM WRITE
'
' EE  READ  ( byte )
' EE  WRITE
'
' SET LAN ADDRESS
' TEST
' RESET
'
' LOAD SCRIPT
' SAVE SCRIPT

' RUN SCRIPT
'
' EXECUTE COMMAND
' EXECUTE COMMAND LIST
'
' -------------- LOGO -----------------------------
Sub COMMUNICATION()
Local A As Byte
Local B As Byte

	CMD = RX(1) 
	PLE = 2                    ' READ POINTER
	Select Case CMD
	
		Case &H52                    'Asc("R") R RESET             SR(CR) OK
		 If PRX=3 Then  
			BREAK()
'			Print "RESET"
			ACKNO()
			Else
			NOACK()
			End If
	
		Case &H41                   'ASC("A") A  READ VAR           SAXX(CR)   XX HEX OK
		 If PRX=5 Then   
			A=READHEX()
'			Print "VAR ";Hex(A);"=";
			WRITEHEX(VAR(A))
		Else
			NOACK()
		End If

		Case &H42                   'ASC("B") B  READ PGM         SBXX(CR) OK
		 If PRX=5 Then   
			A=READHEX()
'			Print "PGM ";Hex(A);"="
			WRITEHEX(PGM(A))
			Else
			NOACK()
			End If

		Case &H43                   'ASC("C") C  READ EE     SCXX(CR) OK
		 If PRX=5 Then   
			A=READHEX()
'			Print "EE ";Hex(A);"=";
			WRITEHEX(ReadEE(A))
			Else
			NOACK()
			End If

	
		Case &H56                   'Asc("V")  WRITE VAR    SVXXYY(CR) OK
		 If PRX=7 Then       
				A=READHEX()
				B=READHEX()
           If A<128 Then       
           VAR(A)=B
 '          Print "VAR ";Hex(A);"=";Hex(B)
			Else
			NOACK()
        End If
        Else
        NOACK()
		End If

		Case &H57                   'Asc("W")  SWXXYY(CR)  WRITE PGM  OK
		 If PRX=7 Then       
				A=READHEX()
				B=READHEX()
            If A<64 Then				       
           PGM(A)=B
'           Print "PGM ";Hex(A);"=";Hex(B)      
           Else
           NOACK()
           End If
			Else
			NOACK()
        End If

		Case &H58                'Asc("X")  SCXXYY(CR)  WRITE EE --PAGE0-- OK
		 If PRX=7 Then
		  A=READHEX()
		  B=READHEX()
		   If B<>ReadEE(A) Then
		    WriteEE(A,B)
'		     Print "EE ";Hex(A);"=";Hex(B)
       
		   End If
		   ACKNO()
		 Else
		  NOACK()
		 End If
	
		Case &H4C                'Asc("L")  SLXX(CR)  WRITE LAN ADDRESS     OK
		 If PRX=5 Then
		  A=READHEX()
		   If A<>ReadEE(1) Then
		    WriteEE(1,A)
'		    Print "LAN=";Hex(A)
           ACKNO()
		   End If
		 Else
		  NOACK()
		 End If

		Case &H54            'Asc("T")    PRINT TEST     ST(CR) OK
	    If PRX=3 Then
		PTEST()
		Else
		NOACK()
		End If
		
		Case &H4D           ' Asc("M") XXYY ZZ Execute une Commande ZZ and SET Param XXYY    OK
		If PRX=9 Then
        PARAMETRE()
		PGM(1)=READHEX()
'		Print "P=";Hex(C);" W=";PARAM 
		EXECUTE(1)
		Else
		NOACK()
		End If

       Case &h4E             ' Asc("N") RUN PROGRAM     SN(CR)     OK
        If PRX=3 Then
        ERROR=0
        VAR(RUN)=1 
'       Print "RUN"
        Else
        NOACK()
        End If
 
      Case &h4F             ' ASC("O") SET MODE OK ?????????????? 
      If PRX=5 Then
      C_TIMER=PARAM
      C_COUNTER0=PARAM
      C_COUNTER1=PARAM
      MODE=READHEX()
      ACKNO()
      Else
      NOACK()
      End If

        Case &H50           ' Asc("P") XX... YYZZ Execute 8 Command XX and SET Param YYZZ    OK    
		If PRX=23 Then
        PARAMETRE()
		For P=1 To 8
		PGM(P)=READHEX()
		Next
'		Print "P=";Hex(P);" W=";PARAM 
		EXECUTE(8)
		Else
		NOACK()
		End If
  
 
	End Select

CMD=0
PRX=0
MF=0                  ' ENABLE RX Doit etre en dernier
End Sub 
'-----------------------------------------------------------------
' WRITE2HEX
' WRITE AN INTEGER IN 4 HEX CODE
Sub WRITE2HEX( X As Integer )
'Local LA As Byte
'Local LB As Byte
'LA=X mod 256
'LB=(X-LA)/256

'WRITEHEX(LB)
'WRITEHEX(LA)

End Sub
'
'-------------------------------------------------------------------
' WRITE A BYTE IN 2 HEX CODE  OK
'
Sub WRITEHEX( X As Byte )   ' TESTED OK 
Local LA As Byte
Local LB As Byte
 
	LA = X mod 16    		' LOW CAR
	LB = (X-LA)/16    		' HIGH CAR
	If LA < 10 Then
		LA = LA + 48   		' this is zero offset
	Else
		LA = LA + 55    	' A here already have offset 10
	End If			    	' 55=65-10
	If LB < 10 Then
		LB = LB + 48
	Else
		LB = LB + 55
	End If
	PrintBin LB ; LA;
End Sub
'-----------------------------------------------------------------
Sub ACKNO()   ' OK
PrintBin CACK ; CACK
End Sub

'-----------------------------------------------------------------
Sub NOACK()   ' KO
PrintBin CNACK ; CNACK
End Sub
'-----------------------------------------------------------------
Sub SPEED()

'SPEED0=VAR(16)
'SPEED1=VAR(17)
'DIR0=VAR(36)
'DIR1=VAR(37)

If SPEED0=0 Then        ' STOP RESET WACHDOG
WD0=0
End If

If SPEED1=0 Then
WD1=0
End If

If DIR0=1 Then            ' SET DIR I/O
 Set PDIR0
Else
 Reset PDIR0
End If
Pwm1A=SPEED0

If DIR1=1 Then
 Set PDIR1
Else
 Reset PDIR1
End If
Pwm1B=SPEED1

End Sub
'---------------------------------------------------------
Sub PTEST()
Local P As Byte

           Print
		   For P=0 To 64
		   If P=8 Then Print
		   If P=16 Then Print
		   	Print P;
		   	Print "= ";
		   	Print VAR(P)
		   Next
		   Print
		   Print "TIME ";TEMPS
		   Print
End Sub
'---------------------------------------------------------------
Sub DEAD()
Local A As Integer

If WD0 > WD Then     ' WATCHDOG
SPEED0=0
SPEED1=0
ERROR=1
MODE=99
Print "WD0 "
End If

If WD1 > WD Then
SPEED1=0
SPEED0=0
ERROR=2
MODE=99
Print "WD1 "
End If

End Sub
'-------------------------------------------------------------------
' READ 4 HEX TO MAKE AN INTEGER
Function READ2HEX() As Integer
'Local LA As Byte
'Local LB As Byte
'LA=READHEX()
'LB=READHEX()

'return (LA*256+LB)

End Function
'-------------------------------------------------------------------
' READ TWO HEX TO MAKE A BYTE
'
Function READHEX() As Byte  ' TESTED OK
Local LA As Byte
Local LB As Byte

	LA = RX(PLE)
	If LA > 57 Then
	 	LA = LA - 55	' A here already have offset 10
	Else				' it is ASCII all the time
		LA = LA - 48
	End If

	Incr PLE

	LB = RX(PLE)
	If LB > 57 Then
		LB = LB - 55
	Else
		LB = LB - 48
	End If
	Incr PLE

	Return ( LA * 16 + LB )
End Function
'---------------------------------------------------------
'             INTERPRETER
'---------------------------------------------------------
Sub INTERPRETER()
' Interprete une instruction
' Manipule les variables VAR() comme INPUTS / VARIABLES / SORTIES 
                                         
TOKEN=PGM(PCO)
Print "TOKEN=";Hex(TOKEN)

Incr PCO

If TOKEN < 30 Then                   ' L0 ?????
On token GoTo L0,L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,L13,L14,L15,L16,L17,L18,L19,L20,L21,L22,L23,L24,L25,L26,L27,L28,L29
Else
TOKEN2=TOKEN-30
On TOKEN2 GoTo L30,L31,L32,L33,L34,L35,L36,L37,L38,L39,L40
End If
            
back2:
Incr PCO             
back1:          
Incr PCO             
back:

End Sub

'----------------------------- INSTRUCTIONS -------------------------------------
L0:
' NOP           DO NOTHING
GoTo back

L80:
' IF THEN GOSUB
If TEST=1 Then
RADDRESS=PCO+1
PCO=PGM(PCO)
End If
GoTo back1

L1:
' GOTO direct adresse 8 Bits            OK 
PCO=PGM(PCO)
GoTo back1

L2:                                
' IF THEN GOTO                    OK
If TEST=1 Then
PCO=PGM(PCO)
End If
GoTo back1

L3:
' GOSUB direct adresse 8 bits             OK
RADDRESS=PCO+1
PCO=PGM(PCO)
GoTo back1

L4:
' RETURN                          OK
PCO=RADDRESS
GoTo back

L5:
' END                           OK
VAR(RUN)=0
PCO=1
GoTo back

'--------------------- CALCULATION ---------------------------------
L6:
' READ CONSTANT 1 byte                    OK
MATH1=PGM(PCO)
GoTo back1

L7:
' READ CONSTANT2 2 bytes                  OK
MATH1=PGM(PCO+1)*256+PGM(PCO)
GoTo back2

L8:
' READ1 VAR                           OK
MATH1=VAR(PGM(PCO))
GoTo back1

L9:
' EMPILE                               OK
MATH2=MATH1
'MATH1=0
GoTo back

L10:
' STORE VAR                            OK
VAR(PGM(PCO))=MATH1
GoTo back1

L11:
' ADD                                OK
MATH1=MATH1+MATH2
GoTo back

L12:
' TEST=
If MATH1=MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L13:
' TEST<>
If MATH1<>MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L14:
' TEST_OR
If MATH1=1 Or MATH2=1 Then
TEST=1
Else
TEST=0
End If
GoTo back

L15:
' TEST_AND
If MATH1=1 And MATH2=1 Then
TEST=1
Else
TEST=0
End If
GoTo back

L16:
' TEST=0
If MATH1=0 Then
TEST=1
Else
TEST=0
End If
GoTo back

L17:
' TEST=1
If MATH1=1 Then
TEST=1
Else
TEST=0
End If
GoTo back

L18:
' TEST<>0
If MATH1<>0 Then
TEST=1
Else
TEST=0
End If
GoTo back

L19:
' CLEAR DIRECT
VAR(PGM(PCO))=0
GoTo back1

L20:
' INC DIRECT
VAR(PGM(PCO))=VAR(PGM(PCO))+1
GoTo back1

L21:
' DEC DIRECT
VAR(PGM(PCO))=VAR(PGM(PCO))-1
GoTo back1

'--------------------- ROBOTIQUE ----------------------------------

L22:
' FORWARD      OK
VAR(36)=1
VAR(37)=1
GoTo back

L23:
' BACKWARD     OK
VAR(36)=0
VAR(37)=0
GoTo back

L24:
' RIGHT          OK
VAR(36)=1
VAR(37)=0
GoTo back

L25:
' LEFT          OK
VAR(36)=0
VAR(37)=1
GoTo back

L26:
' STOP             OK
VAR(16)=0
VAR(17)=0
GoTo back

L27:
' LOW SPEED          OK
VAR(16)=VLOW
VAR(17)=VLOW
GoTo back

L28:
' MEDIUM SPEED        OK
VAR(16)=VMEDIUM
VAR(17)=VMEDIUM
GoTo back

L29:
' HIGH SPEED          OK
VAR(16)=VFAST
VAR(17)=VFAST
GoTo back

L30:
' SET SPEED
VAR(16)=MATH1
VAR(17)=MATH1
GoTo back

L31:
' SET TIMER1 and MODE with PARAM  INTEGER    OK   
C_TIMER=PARAM
MODE=1
GoTo back

L32:
' SET COUNTER0 and MODE with PARAM  INTEGER  OK  
C_COUNTER0=PARAM
MODE=2
GoTo back

L33:
' SET COUNTER1 with PARAM  INTEGER  OK
C_COUNTER1=PARAM
MODE=2
GoTo back

L60:
' SET COUNTER0/1 with PARAM  INTEGER  OK
C_COUNTER0=PARAM
C_COUNTER1=PARAM
MODE=2
GoTo back

L34:
' WAIT TIMER                 OK
If C_TIMER>0 Then
Decr PCO
End If
GoTo back

L35:
' WAIT COUNTER0                OK
If C_COUNTER0 > 0 Then
Decr PCO
End If
GoTo back

L36:
' WAIT COUNTER1               OK
If C_COUNTER0 > 0 Then
Decr PCO
End If
GoTo back

L37:
' WAIT MODE                 OK
If MODE <> 99 Then     ' Attention 99 -> 0
Decr PCO
End If
GoTo back

L38:
' FOREVER                  OK
Decr PCO
GoTo back       

L39:
' SET TIMER XX SECONDE  0-> RND        OK
If PGM(PCO)=0 Then
C_TIMER=2000+20*Rnd()
Else
C_TIMER=PGM(PCO)*100
End If
GoTo back

L40:
' SOUND XX       25,100      1 Khz 1 S      OK
A=PGM(PCO)
Sound A,A*4
GoTo back

L41:
' SET MODE 1
MODE=1
GoTo back

L42:
' SET MODE 2
MODE=2
GoTo back

'///////////////////////////////////////////////////////////////
Sub DOWNLOAD(NUM As Byte)  ' Move Script from EEPROM to RAM, 31 SCRIPTS of 15 STEP
Local P As Byte
Local BASE As Word
BASE=16+NUM*16

For P=1 To 16
PGM(P)=ReadEE(BASE+P)
Next P

End Sub
'//////////////////////////////////////////////////////////////
Sub UPLOAD(NUM As Byte)
Local P As Byte
Local BASE As Word
BASE=16+NUM*16

For P=1 To 16
WriteEE(BASE+P,PGM(P))
Next

End Sub
'////////////////////////////////////////////////////////////////////:
Sub PID()      ' P mais pas I risque oscillation

If ( C_COUNTER1 - C_COUNTER0 ) > 2 Then
   If SPEED0 < 255 Then                     ' VAR(16)
   Incr SPEED0
   Else
   Decr SPEED1
   End If
End If

'If ( C_COUNTER0 - C_COUNTER1 ) > 2 Then
'   If SPEED1 < 255 Then                     ' VAR(17)
'   Incr SPEED1
'   Else
'   Decr SPEED0
'   End If
'End If

End Sub
'////////////////////////////////////////////////
Sub EXECUTE(NUM As Byte) ' 1 or 8 Command        OK
PGM(NUM+1)=GEND
ERROR=0
PCO=1
VAR(RUN)=1
End Sub
'/////////////////////////////////////////////////////
Sub PARAMETRE()  ' READ 2 BYTE and SET PARAM       OK
Local A As Byte
Local B As Byte
A=READHEX()
B=READHEX()
PARAM=A*256
PARAM=PARAM+B 
End Sub

'-------------- SCRIPT ----------------------------
' SP 1000 1F 16 1B 00 00 00 00 00 (CR)
'    PARAM ( for TIMER or COUNTERS )
'         1F SET TIMER , 20 SET COUNTER0, 21 SET COUNTER1, XX SET BOTH
'            16 FORWARD, 17 BACKWARD, 18 RIGHT, 19 LEFT
'				SPEED 1A STOP, 1B LOW, 1C MEDIUM, 1D FAST, 1E SET XX
'
'
	