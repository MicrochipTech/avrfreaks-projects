' Copyright (c) 2009 by ali hooshangian all rights reserved
' Compiler: BASCOM 1.11.9.0
'
'This program count a number start at  0000, and increase 2 step per second.  
'
'-------------------------------------------------------------------------
'  MSB.......................................LSB
'   a     b     c     d     e     f     g     Dp   (segments wiring)
'  2^7   2^6   2^5   2^4   2^3   2^2   2^1   2^0
'  PD7   PD6   PD5   PD4   PD3   PD2   PD1   PD0    PORT_EQU
'-------------------------------------------------------------------------
'For Example: &b11111101=&HFD show "-" (Minus) on 7segment
'             &b00000011=&H03 show "0" (Zero) on 7segment
'             &b00000010=&H03 show "0." (Zero With Dot) on 7segment
'-------------------------------------------------------------------------
'$sim
$regfile = "m8def.dat"
$crystal = 1000000

$hwstack = 128                                              ' default use 32 for the hardware stack
$swstack = 25                                               'default use 10 for the SW stack
$framesize = 100                                            'default use 40 for the frame space


Declare Sub Disp7seg(number As Word)

Dim Dcnt As Byte
Dim Wcnt As Word
Dim N(4) As Byte
N(1) = &HFF : N(2) = &HFF : N(3) = &HFF : N(4) = &HFF

'Config Adc = Single , Prescaler = Auto , Reference = Internal
'Start Adc

Dispdata Alias Portd
Dispdata = &HFF
Config Dispdata = Output

Dn1 Alias Portc.2                                           ' digit number 1 (x1)
Dn2 Alias Portc.3                                           ' digit number 2 (x10)
Dn3 Alias Portc.4                                           ' digit number 3 (x100)
Dn4 Alias Portc.5                                           ' digit number 4 (x1000)
Portc = &B00000000
Ddrc = &B00111100

Set Portb.1
Config Portb.1 = Output

Config Timer0 = Timer , Prescale = 8
On Timer0 T0isr
Enable Timer0

Enable Interrupts
Wcnt = 0
'*********************************************************************
Do
      'Wcnt = Getadc(0)
      'Wcnt = Wcnt * 5
      Incr Wcnt
      Disp7seg Wcnt
      Waitms 500
Loop
End

'**************  subroutines & interrupts service routines are here *********

Chrdata:
Data &H03 , &H9F , &H25 , &H0D , &H99 , &H49 , &H41 , &H1B , &H01 , &H09       'asc=48to57 (numbers)

T0isr:
   Portc = Portc And &B11000011                             'Turn off all segments
   Incr Dcnt : If Dcnt > 4 Then Dcnt = 1
   Select Case Dcnt
      Case 1
         Dispdata = N(1) : Set Dn1
      Case 2
         Dispdata = N(2) : Set Dn2
      Case 3
         Dispdata = N(3) : Set Dn3
      Case 4
         Dispdata = N(4) : Set Dn4
   End Select
Return

Sub Disp7seg(number As Word)
   Local D7sj As Byte
   Local Count As Byte
   Local Ss As String * 6
   Local S1 As String * 1
   Ss = Str(number)
   Ss = Format(ss , "    ")
   For D7sj = 1 To 4
      S1 = Mid(ss , D7sj , 1)
      Count = Val(s1)
      N(d7sj) = Lookup(count , Chrdata )
   Next D7sj
End Sub

