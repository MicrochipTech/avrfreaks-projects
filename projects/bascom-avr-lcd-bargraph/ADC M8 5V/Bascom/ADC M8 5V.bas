' ***************************************************************************
' *
' * Title         : ADC M8.bas
' * Version       : 1.0
' * Last Updated :  08.04.2006
' * Target        : AT Mega8
' * Author        : www.avrprojects.net
' * Program code  : BASCOM AVR
' * Hardware req. :
' * Description   : how to use the ADC convertor, show the result on a LCD
' *                 display with percentage and bargraph
' ***************************************************************************
$regfile = "M8def.dat"                                      ' use the Mega8

$crystal = 10000000

Deflcdchar 0 , 32 , 32 , 31 , 31 , 31 , 31 , 32 , 32        ' replace ? with number (0-7)'
Deflcdchar 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1                ' replace ? with number (0-7)
Deflcdchar 2 , 16 , 16 , 16 , 16 , 16 , 16 , 16 , 16        ' replace ? with number (0-7)
Deflcdchar 3 , 16 , 16 , 31 , 31 , 31 , 31 , 16 , 16        ' replace ? with number (0-7)
Deflcdchar 4 , 1 , 1 , 31 , 31 , 31 , 31 , 1 , 1            ' replace ? with number (0-7)

Config Lcdpin = Pin , Db4 = Portd.3 , Db5 = Portd.2 , Db6 = Portd.1 , Db7 = Portd.0 , E = Portd.6 , Rs = Portd.7

Config Lcd = 20 * 4

Config Adc = Single , Prescaler = Auto , Reference = Avcc   'config ADC

Start Adc

Dim W As Word , Perc As Word , Channel As Byte , I As Byte , Cols As Byte

Cls
Cursor Off
Locate 3 , 1
Lcd "0"
Locate 3 , 5
Lcd "25"
Locate 3 , 10
Lcd "50"
Locate 3 , 15
Lcd "75"


Do
  W = Getadc(0)
  Perc = W / 10
  Locate 1 , 1
  Lcd "ADC(0):" ; W ; "   "
  Locate 1 , 13
  Lcd Perc ; "%  "

  Cols = Perc / 5

  Locate 2 , 1
  For I = 1 To Cols
   Select Case I
   Case 1 : Lcd Chr(3)
   Case 5 : Lcd Chr(4)
   Case 10 : Lcd Chr(4)
   Case 15 : Lcd Chr(4)
   Case 20 : Lcd Chr(4)
   Case Else : Lcd Chr(0)
  End Select
  Next I

  Cols = Cols + 1

  For I = Cols To 20
  Select Case I
   Case 1 : Lcd Chr(2)
   Case 5 : Lcd Chr(1)
   Case 10 : Lcd Chr(1)
   Case 15 : Lcd Chr(1)
   Case 20 : Lcd Chr(1)
   Case Else : Lcd Chr(32)
   End Select
   Next I

   Waitms 100
  Loop
End