'//////////////////////////////////////////////////////////
'///    ROBOT MASTER 011                                ///
'///    VERSION ROBOT   07/10/2003                      ///
'///    GLU                                             /// 
'//////////////////////////////////////////////////////////
' FASTAVR 3.1.7
' STUDIO  3.53
' PROGRAMMER 1.33 SERIAL
'            D:\AVR_SERIAL_ISP\APROGWIN\AVRPROG
' DIM Bit, Byte, Integer, Word, Long or Float
'
' 1482 lines 
' 3971 Words
' 140 SRAM Free
'
' BUG REPORT
' pas de connection entre AGND et DGND
' faire une masse puissance séparée pour la version suivante
' inversion PX
'
'
'
'-------------------- CARTE BX II -----------------------
' 8 Analog  IN                                                  PA
' 8 Digital IN                                                  PB sauf PB2
' 8 Power OUT ( 2 Motors with PWM )                             PC
' RS232 / INT0 / INT1 / SERVO / I2C / IRRX / IRTX               PD
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
'        PB0 IN    CODEUR 1  / LED GREEN   /   DIGITAL OUT
'        PB1 IN    CODEUR 2                /   DIGITAL OUT
'        PB2 IN    IR TX                   /   DIGITAL OUT
'        PB3 IN    RESERVE       LED????   /   DIGITAL OUT
'        PB4 IN   DIGITAL IN 1             OK
'        PB5 IN   DIGITAL IN 2  ( MOSI )   OK
'        PB6 IN   DIGITAL IN 3  ( MISO )   OK
'        PB7 IN   DIGITAL IN 4  ( SCK  )   OK
'
' Port C  I2C and Power Outputs
'        PC0     ( SCL )  ( EEPROM I2C )
'        PC1     ( SDA )  ( EEPROM I2C )
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
' Adresse 2   MS
'
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
' VAR0   Analog input 0
' VAR1   Analog input 1  
' VAR2   Analog input 2
' VAR3   Analog input 3
' VAR4   Analog input 4
' VAR5   Analog input 5
' VAR6   Analog input 6
' VAR7   Analog input 7
'
' VAR8   Digital input 0
' VAR9   Digital input 1
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
' VAR31  COUNTER0 SETPOINT
' VAR32  COUNTER1
' VAR33  COUNTER1 SETPOINT
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
' VAR41 RUN Interpreter ON/OFF

' VAR42 DIRECT COMMAND MOTOR0 ( 0: STOP, 1...128 FORWARD SETPOINT, 129..255 BACKWARD )
' VAR43 DIRECT COMMAND MOTOR1                       //
'
' VAR44 MS              SET MOTOR MODE
' VAR45 TEMPS
' VAR46 T0
' VAR47 T1
' VAR48 PCO
' VAR49 RESERVE
'
' VAR50 - > 128 Variables
'
$Device   = 8535			        ' used device 8535
$Stack    = 32						' stack depth 
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

' ------------------ DEFINITIONS ----------------------

' PA Analog IN

' PB Digtal IN sauf PB2

$Def CODEUR1 = PORTB.0         
$Def LED     = PORTB.0   ' ?????????????   

$Def CODEUR2 = PORTB.1           
$Def IRTX    = PORTB.2            
$Def RESERVE = PORTB.3 
'$Def LED     = PORTB.3   ' LED ???           
$Def D1      = PORTB.4           
$Def D2      = PORTB.5           
$Def D3      = PORTB.6             
$Def D4      = PORTB.7           

' PC Digital OUT
' PC0 PC1  - > I2C
$Def P4   = PORTC.2             ' POWER 1 
$Def P3   = PORTC.3             ' POWER 2
$Def P2   = PORTC.4             ' POWER 3
$Def P1   = PORTC.5             ' POWER 4

$Def PDIR0 = PORTC.6             ' DIR 0 Motor Direction  
$Def PDIR1 = PORTC.7             ' DIR 1

' PD general I/O

Dim MS As Byte  ' MOTOR MODE SET

Dim PT As Byte  ' for Interupt Timer
Dim PL As Byte  ' for LED

Dim SCAN As Byte
Dim ADR As Byte             ' Lan Address

Dim COUNT As Byte           ' PWM Counter
Dim COUNT1 As Byte          ' 

Dim CMD As Byte             ' RX COMMAND

Dim A As Byte               ' General VAR
Dim B As Byte				' A, B, P, T 
Dim P As Byte
Dim T As Byte

Dim A2 As Integer
Dim B2 As Integer
Dim P2 As Integer
Dim T2 As Integer

Dim ALE As Byte              ' RANDOM DATA

Dim RX(32) As Byte           ' RX Buffer
Dim PRX As Byte              ' RX POINTER RECEPTION
Dim PLE As Byte              ' RX POINTER LECTURE
Dim MF As Bit                ' RX Message Ready Flag

Dim CPT0 As Byte          ' Counter INT 0
Dim CPT1 As Byte          ' Counter INT 1
Dim CPT0R As Byte         ' Counter Report 0
Dim CPT1R As Byte         ' Counter Report 1

Dim SPEED0 As Byte        ' Motor Speed 1
Dim SPEED1 As Byte        '             2

Dim DIR0 As Byte
Dim DIR1 As Byte

Dim TEMPMS As Byte        ' Temps 1 MS
Dim TEMPS  As Byte        ' Temps  S

Dim T0 As Byte            ' Temps INT 0
Dim T1 As Byte            ' Temps INT 1

Dim WD0 As Word           ' Watchdog 1
Dim WD1 As Word           ' Watchdog 2  

Dim VERSION As Byte
Dim REVISION As Byte

Dim ERROR As Byte         ' error CODE
Dim STATUS As Byte        ' STATUS 0=STOP,1=RUN
 
'--------------- INTERPRETER --------------------------

Dim Buffer(16) As Byte		' Interpreter Buffer starts at &h60
Dim BP As Byte
Dim DB As Byte

Dim PGM(64) As Byte   

Dim PCO As Integer
Dim TOKEN As Byte
Dim TOKEN2 As Byte

Dim VAR(128) As Byte
Dim PVAR As Byte
Dim NVAR As Byte
Dim LABEL As Byte

Dim PILE(16) As Integer
Dim PP As Byte

Dim MATH1 As Integer
Dim MATH2 As Integer

Dim TEST As Byte

Dim VLOW As Byte
Dim VMEDIUM As Byte
Dim VFAST As Byte

Const C_ELSE = 1
Const C_ENDIF = 2
Const C_LABEL = 3

Const A_STX=2
Const A_ETX=3
Const A_ACK=2
Const A_NACK=3

Const M_WRITE = 1
Const M_READ =2
Const M_STOP=3
Const M_RUN=4
Const M_DEBUG=6
Const M_STEP=7
Const M_VAR=8

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

Declare Sub PUSH(PCO As Integer)
Declare Function PULL() As Integer
Declare Function COPY() As Integer
Declare Sub POP()
Declare Sub SEARCH(CODE As Byte)
Declare Sub SEARCHLABEL(LABEL As Byte)
Declare Sub SEARCHENDIF()

Declare Function READHEX() As Byte
Declare Function READ2HEX() As Integer

Declare Interrupt Ovf0()    ' Timer 0 Overflow
Declare Interrupt Urxc()    ' RX Char
Declare Interrupt Int0()    ' INT 0
Declare Interrupt Int1()    ' INT 1

Const CR    =13
Const LF    =17    ' ?? 
Const CACK  =41 
Const CNACK =42
Const STX   =65
Const ETX   =66

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

' INIT COMM BUFFER AND VAR
For P=0 To 30          ' RESET RX Buffer
RX(P)=0
Next 

PRX=0
CMD=0
MF=0

BREAK()              ' INITIALISATION
SPEED()

ADR    = ReadEE(1)      	' Card LAN ADDRESS
VAR(44)= ReadEE(2)          '  MS Motor Mode

VLOW   = ReadEE(3)
If VLOW=0 Then VLOW=150

VMEDIUM= ReadEE(4)
If VMEDIUM=0 Then VMEDIUM=200

VFAST  = ReadEE(5)
If VFAST=0 Then VFAST=255

'----------------------------------------------
VERSION=1   		' Version
REVISION=11         ' Revision
'-------------- INTERPRETER --------------------------------

PP=0
PCO=1
MATH1=0
MATH2=0

Print
Print "ROBOT GLU READY "
Print "ADRESSE "; ADR
Print "Version "; VERSION ; "." ; REVISION
Print

PL=0
'-----------------MAIN PROGRAM--------------------
Enable Interrupts
Wait 2
'-------------------------- MAIN LOOP --------------------
Do 
 SCAN=0
 Reset WatchDog
 STATUS=1
 
 ALE=Rnd()                 ' RANDOM
      	
  For P=0 To 7              ' ANALOG INPUTS  10 Bits
  VAR(P)=Adc8(P)
  Next
 
  For P=0 To 7
  A=PINB.P              ' DIGITAL INPUTS
  VAR(8+P)=Abs(A)
  Next

  VAR(39)=PIND.6     ' IR RX
    
  If MF=1 Then                 ' EXECUTE AVAILABLE MESSAGE
   COMMUNICATION()
  End If

 MS=VAR(44)

 If VAR(SECURITY)>0 Then
 DEAD()						 ' DETECT DEAD Moteurs bloques
 End If                      ' RESET VARS SPEED

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

' SERVO Timer Increment = 8 µS         125 -> 1000 µs 250 -> 2000 µS
For P=22 To 25
A=VAR(P)
Set D1
Timer2=0         ' ?????????????????????
ATTENTE:
If Timer2 < VAR(P) Then GoTo ATTENTE
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
VAR(47)=T1     ' PEROID INT1

ATTENTE2:
If SCAN<20 Then GoTo ATTENTE2    ' SCANTIME WAIT 20 mS pour les servos

Incr PL
If PL>20 Then
Toggle LED       ' Toggle every 200 ms
PL=0
End If

' ---Manque que la gestion de I2C---
 
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
	
End Interrupt
'-------------------------------------------------------
' INTERRUPT RX          S CMD (XX) (YY) CR
Interrupt Urxc(), Save All
If MF=0 Then                    ' PAS REENTRANT
	InputBin CMD                ' S CMD DATA CR
	RX(PRX)=CMD
	If PRX < 31 Then
	Incr PRX
	Else
	ERROR=100
	PRX=0
	End If
	
	Select Case CMD
		Case 83         		'  "S" START MESSAGE
			PRX=1
		Case 13         		' END MESSAGE
			If PRX > 2 Then
				MF = 1          ' MESSAGE READY TO BE TREAT 
			Else
				PRX=0
			End If
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
	If CPT0 > 254 Then
	 	CPT0=0
	 	Incr CPT0R
	End If
	T0=WD0
	WD0=0
End Interrupt

'-------------------------------------------------------
' INTERRUPT INT1
Interrupt Int1(), Save All
	Incr CPT1 
	If CPT1 > 254 Then
	 	CPT1=0
	 	Incr CPT1R
	End If 
	T1=WD1
	WD1=0
End Interrupt

'-----------------------------------------------------------------
Sub BREAK()
Local P As Byte

For P=22 To 25       ' RESET SERVO 1500 µS  
VAR(P)=200
Next

VAR(16)=0             ' RESET SPEED
VAR(17)=0

VAR(42)=0             ' RESET SPEED
VAR(43)=0

For P=18 To 21        ' RESET PX
VAR(P)=0
Next

End Sub

'----------------- FONCTION --------------------------------------
'   BCC ??
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
' RUN SCRIPT
'
' -------------- LOGO -----------------------------
Sub COMMUNICATION()
Local A As Byte
Local B As Byte

	CMD = RX(1) 
	PLE = 2
	Select Case CMD
	
		Case &H52                    'Asc("R") R RESET             SR(CR) OK
		 If PRX=3 Then  
			BREAK()
			Print "RESET"
			Else
			NOACK()
			End If
	
		Case &H41                   'ASC("A") A  READ VAR           SAXX(CR)   XX HEX OK
		 If PRX=5 Then   
			A=READHEX()
			Print "VAR ";A
			WRITEHEX(VAR(A))
		Else
			NOACK()
		End If

		Case &H42                   'ASC("B") B  READ PGM         SBXX(CR) OK
		 If PRX=5 Then   
			A=READHEX()
			Print "PGM ";A
			WRITEHEX(PGM(A))
			Else
			NOACK()
			End If

		Case &H43                   'ASC("C") C  READ EE     SCXX(CR) OK
		 If PRX=5 Then   
			A=READHEX()
			Print "EE ";A
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
           Print "VAR ";A;"=";B
			Else
			NOACK()
        End If
        Else
        NOACK()
		End If

		Case &H57                   'Asc("W")  WRITE PGM
		 If PRX=7 Then       
				A=READHEX()
				B=READHEX()
            If A<64 Then				       
           PGM(A)=B
           Print "PGM ";A;"=";B      
           Else
           NOACK()
           End If
			Else
			NOACK()
        End If

		Case &H58                'Asc("X")  WRITE EE
		 If PRX=7 Then
		  A=READHEX()
		  B=READHEX()
		  If A>1 And A<256 Then
		   If B<>ReadEE(A) Then
		    WriteEE(A,B)
		     Print "EE ";A;"=";B
		   End If
		    Else
		    NOACK()
		  End If
		 Else
		  NOACK()
		 End If
	
		Case &H4E                'Asc("N")  WRITE LAN ADDRESS
		 If PRX=5 Then
		  A=READHEX()
		  If A>0 And A<256 Then
		   If A<>ReadEE(1) Then
		    WriteEE(1,A)
		    Print "LAN ";A
		   End If
		    Else
		    NOACK()
		  End If
		 Else
		  NOACK()
		 End If

		Case &H54            'Asc("T")  T PRINT TEST     ST(CR) OK
	    If PRX=3 Then
		PTEST()
		Else
		NOACK()
		End If
		
		Case &H55           ' Asc("U") LOAD Script      15 Scripts de 32 Pas    
		If PRX=5 Then
		A=READHEX()
		For P=1 To 32
		PGM(P)=ReadEE(10+P*32)
		Next P
		Else
		NOACK()
		End If

       Case &h56             ' Asc("V") RUN PROGRAM
        If PRX=3 Then
        VAR(RUN)=1 
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
Local LA As Byte
Local LB As Byte
LA=X mod 256
LB=(X-LA)/256

WRITEHEX(LB)
WRITEHEX(LA)

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
Sub ACKNO()
PrintBin 40
PrintBin CACK
End Sub

'-----------------------------------------------------------------
Sub NOACK()
PrintBin 40
PrintBin CNACK
End Sub
'-----------------------------------------------------------------
Sub SPEED()

If MS=0 Then
 SPEED0=VAR(16)
 SPEED1=VAR(17)
 DIR0=VAR(36)
 DIR1=VAR(37)
Else
 If VAR(42)>128 Then
  DIR0=1
  SPEED0=(VAR(42)-128)*2
 Else
  DIR0=0
  SPEED0=VAR(42)*2
 End If

 If VAR(43)>128 Then
  DIR1=1
  SPEED1=(VAR(43)-128)*2
 Else
  DIR1=0
  SPEED1=VAR(43)*2
 End If

End If

If SPEED0=0 Then
WD0=0
End If

If SPEED1=0 Then
WD1=0
End If

If DIR0=1 Then
 Set PDIR0
 Pwm1A=SPEED0
Else
 Reset PDIR0
 Pwm1A=SPEED0
End If

If DIR1=1 Then
 Set PDIR1
 Pwm1B=SPEED1 
Else
 Reset PDIR1
 Pwm1B=SPEED1
End If

End Sub
'---------------------------------------------------------
Sub PTEST()
Local P As Byte

           Print "TEST "
           Print
		   For P=0 To 64
		   	Print P;
		   	Print "= ";
		   	Print VAR(P)
		   Next
		   Print
		   Print "TIME"
		   Print TEMPS
		   Print
End Sub
'---------------------------------------------------------------
Sub DEAD()
Local A As Word

A=VAR(40)*10        ' DETECT MOTOR DEAD FOR 1 AND 2  GENERAL ????
                    ' VAR(40) en 0.1 s 
If WD0 > A Then
VAR(16)=0
VAR(42)=0
ERROR=1
Print "WD0 "
End If

If WD1 > A Then
VAR(17)=0
VAR(43)=0
ERROR=2
Print "WD1 "
End If

End Sub
'-------------------------------------------------------------------
' READ 4 HEX TO MAKE AN INTEGER
Function READ2HEX() As Integer
Local LA As Byte
Local LB As Byte
LA=READHEX()
LB=READHEX()

Return (LA*256+LB)

End Function
'-------------------------------------------------------------------
' READ TWHO HEX TO MAKE A BYTE
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
' MULTITASKING
' PGM=1 A 4
' DIM PCO(4) and use PCO(PGM)
' DIM PILE(64) and use PP(4)
' PP(1) 1  -> 8
' PP(2) 9  -> 16
' PP(3) 17 -> 24
' PP(4) 25 -> 32
' DIM MATH1(4) and MATH2(4) and TEST(4) ou ne pas couper les calculs et les tests
'
' LABEL 101 debut du programme 1 finir par END
' LABEL 102                    2           END
' LABEL 103                    3           END
' LABEL 104                    4           END
'
' STATUS(4)    O:STOP 1:RUN    >1: ERREUR
' ERREUR(4)    0:NO ERREUR
' 
' START PGM
' STOP  PGM
' 
' VAR( ) sont des variables globales
'
'
'
Sub interpreter()
' interprete une instruction

VAR(44)=0  ' SET MOTOR MODE ADR
MS=0       ' SET MOTOR MODE
                                         
TOKEN=PGM(PCO)
Incr PCO

'If TOKEN < 31 Then
On token GoTo L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,L13,L14,L15,L16,L17,L18,L19,L20,L21,L22,L23,L24,L25,L26,L27,L28,L29,L30
'Else
TOKEN2=TOKEN-30
'On TOKEN2 GoTo L31,L32,L33,L34,L35,L36,L37,L38,L39,L40,L41,L42,L43,L44,L45,L46,L47,L48,L49,L50,L51,L52,L53,L54,L55,L56
'End If
             
back2:
Incr PCO             
back1:          
Incr PCO             
back:

End Sub

'----------------------------- INSTRUCTIONS -------------------------------------
L1:
' GOTO direct adresse 8 Bits 
PCO=PGM(PCO)
GoTo back

'L100:
' GOTO direct adresse 16 Bits 
'Incr PCO
'PCO=PGM(PCO)*256 + ReadEE(PCO+1)
'GoTo back

L2:
' GOTO LABEL
LABEL=PGM(PCO)
SEARCHLABEL(LABEL)
GoTo back

L3:
' GOSUB direct adresse 8 bits
PUSH(PCO+1)
PCO=PGM(PCO)
GoTo back

'L101:
' GOSUB direct adresse 16 bits
'Incr PCO
'PUSH(PCO) ' PCO = 16 bits
'PCO=PGM(PCO)*256+ReadEE(PCO+1)
'GoTo back

L4:
' GOSUB LABEL
PUSH(PCO+1)
LABEL=PGM(PCO)
SEARCHLABEL(LABEL)
GoTo back

L5:
' RETURN
PCO=PULL()
GoTo back

L6:
' END
VAR(RUN)=0
PCO=1                      
GoTo back

'--------------------- CALCULATION ---------------------------------
L7:
' READ CONSTANT 1 byte
MATH1=PGM(PCO)
GoTo back1

L8:
' READ CONSTANT2 2 bytes
MATH1=PGM(PCO)*256+PGM(PCO+1)
GoTo back2

L9:
' READ1 VAR
MATH1=VAR(PGM(PCO))
GoTo back1

'L10:
' READ INDIRECT VAR
'MATH1=ReadEE(VAR(PCO))
'GoTo back1

'L11:
' READ1 ARRAY VAR   BASE+VAR
'Incr PCO
'A=PGM(PCO)
'B=ReadEE(PCO+1)
'MATH1=A+VAR(B)
'GoTo back2

L10:
' EMPILE
MATH2=MATH1
'MATH1=0
GoTo back

L11:

L12:
' STORE VAR
VAR(PGM(PCO))=MATH1
GoTo back1

L13:
' ADD
MATH1=MATH1+MATH2
GoTo back

L14:
' SUBB
If MATH2<MATH1 Then
MATH1=MATH1-MATH2
Else
MATH1=0
End If
GoTo back1

L15:
' RND
MATH1=Rnd()
GoTo back1

L16:
' MUL
MATH1=MATH1*MATH2
GoTo back1

L17:
' DIV
If MATH2<>0 Then
MATH1=MATH1/MATH2
Else
MATH1=0
End If
GoTo back1

L18:
' TEST=
If MATH1=MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L19:
' TEST<>
If MATH1<>MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L20:
' TEST>
If MATH1>MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L21:
' TEST<
If MATH1<MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L22:
' TEST<=
If MATH1<=MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L23:
' TEST>=
If MATH1>=MATH2 Then
TEST=1
Else
TEST=0
End If
GoTo back

L24:

L25:
' TEST<>0
If MATH1<>0 Then
TEST=1
Else
TEST=0
End If
GoTo back

L26:
' TEST_OR
If MATH1=1 Or MATH2=1 Then
TEST=1
Else
TEST=0
End If
GoTo back

L27:
' TEST_AND
If MATH1=1 And MATH2=1 Then
TEST=1
Else
TEST=0
End If
GoTo back

L28:
' TEST=0
If MATH1=0 Then
TEST=1
Else
TEST=0
End If
GoTo back

L29:
' TEST=1
If MATH1=1 Then
TEST=1
Else
TEST=0
End If
GoTo back

L30:
' CLEAR DIRECT
VAR(PGM(PCO))=0
GoTo back1

L31:
' INC DIRECT
VAR(PGM(PCO))=VAR(PGM(PCO))+1
GoTo back1

L32:
' DEC DIRECT
VAR(PGM(PCO))=VAR(PGM(PCO))-1
GoTo back1

' PAS DE IF THEN ELSE IMBRIQUEES
L33:
' IF
If TEST=0 Then
SEARCHENDIF()  ' search ELSE ou ENDIF
'Incr PCO        ' on ne doit pas traiter le ELSE
Else
Incr PCO
End If
GoTo back

L34:
' ELSE
SEARCH(C_ENDIF)  ' si on voit le ELSE la condition est TRUE
GoTo back

L35:
' ENDIF
'TEST=0
'Incr PCO
GoTo back

L36:
'' FOR
'VAR(PGM(PCO))=MATH1
'Incr PCO
'PUSH(PCO)
'GoTo BACK

L37:
'' NEXT                ???? Attention VAR/CONTANTE 1 BYTE / CONSTANTE 2 BYTE
''VAR(PGM(PCO))=VAR(PGM(PCO))+ReadEE(PCO+1)
'If VAR(PGM(PCO)) < PGM(PCO+1) Then ' pas seulement var 
'PCO=COPY()
'Else
'POP() '     clear one stack position
'End If
'GoTo back

L38:
'' NEXT1                ????
''VAR(PGM(PCO))=VAR(PGM(PCO))+1
'If VAR(PGM(PCO)) < PGM(PCO+1) Then
'PCO=COPY()
'Else
'POP()
'End If
'GoTo back

L39:
' DO
PUSH(PCO)
GoTo back

L40:
' UNTIL
If TEST=0 Then
PCO=COPY()
Else
POP()
End If

L41:
' WHILE
If TEST=1 Then
PCO=COPY()
Else
POP()
End If

' IF TEST GOTO
' IF TEST GOSUB

'--------------------- ROBOTIQUE ----------------------

L42:
' WAIT UNTIL VAR/TIMER/COUNTER =0
If VAR(PCO)=0 Then
Incr PCO
Else
Decr PCO
End If
GoTo back

L43:
' FORWARD
VAR(36)=1
VAR(37)=1
GoTo back

L44:
' BACKWARD
VAR(36)=0
VAR(37)=0
GoTo back

L45:
' RIGHT
VAR(36)=1
VAR(37)=0
GoTo back

L46:
' LEFT
VAR(36)=0
VAR(37)=1
GoTo back

L47:
' STOP
VAR(16)=0
VAR(17)=0
VAR(36)=0
VAR(37)=0
GoTo back

L48:
' LOW SPEED
VAR(16)=VLOW
VAR(17)=VLOW
GoTo back

L49:
' MEDIUM SPEED
VAR(16)=VMEDIUM
VAR(17)=VMEDIUM
GoTo back

L50:
' HIGH SPEED
VAR(16)=VFAST
VAR(17)=VFAST
GoTo back

End
'//////////////////////////////////////////////////////////
Sub PUSH(PCO As Integer) 		
PILE(PP)=PCO
Incr PP                 
    
If PP>8 Then  
PP=8
'ERROR=22
End If                  

End Sub
'//////////////////////////////////////////////////////////
Function PULL() As Integer 
If PP>0 Then
Decr PP
Else
PP=0
'ERROR=23
End If
Return PILE(PP)        				
End Function    
'////////////////////////////////////////////////////////
Sub SEARCH(CODE As Byte)
Do
Incr PCO
Loop While PGM(PCO)<>CODE
Incr PCO
End Sub
'////////////////////////////////////////////////////////
Function COPY() As Byte
Return PILE(PP-1)
End Function
'////////////////////////////////////////
Sub POP()
Decr PP
End Sub
'////////////////////////////////////////
Sub SEARCHLABEL(LABEL As Byte)
PCO=1
Do
Do
Incr PCO
Loop While PGM(PCO)<>C_LABEL
Incr PCO
Loop While PGM(PCO)<>LABEL
Incr PCO
If PCO>512 Then ' ??????????????????
ERROR=1
End If
End Sub
'/////////////////////////////////////////////////////////
Sub SEARCHENDIF()   
'Local FIN As Byte
'Local Data As Byte
'FIN=0
'Do
'Incr PCO   
'Data=PGM(PCO)
'If Data=C_ELSE Or Data=C_ENDIF Then FIN=1
'Loop While FIN=0
'Incr PCO    
End Sub
' -------------------- SCRIPT
' AVANCE_TIMER
'
'1 STOP
'2 STORE PARAM -> TIMER1 
'3 FORWARD
'4 SMEDIUM
'5 WAIT_VAR TIMER1 (26)
'6 END
'7 NOP
'8 NOP
'
' AVANCE_STEP
'
'1 STOP
'2 STORE PARAM -> COUNTER0 
'3 FORWARD
'4 SMEDIUM
'5 WAIT_VAR COUNTER0 (30)
'6 END
'7 NOP
'8 NOP
'
' AVANCE_TIMER
'
'1 STOP
'2 STORE PARAM -> TIMER1 
'3 FORWARD
'4 SMEDIUM
'5 WAIT_VAR TIMER1 (26)
'6 END
'7 NOP
'8 NOP
