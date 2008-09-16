$regfile = "m16def.dat"
'Specify the on board crystal
$crystal = 8000000                                          '3579545
'Specify Baud Rate
'$baud = 9600
$baud = 9600

Config Lcdbus = 4
Config Lcd = 16 * 2
Config Lcdpin = Pin , Db4 = Portb.5 , Db5 = Portb.4 , Db6 = Portb.3 , Db7 = Portb.2 , E = Portb.6 , Rs = Portb.7


Const Range = 20
'Dim Convintens As Single
Dim Tolerancehigh As Integer , Tolerancelow As Integer
'Dim Conv As Integer
Dim Steer As Integer , Wheel As Integer , Steertarget As Integer
Dim Steerright As Integer
Steerright = 890
Dim Steerleft As Integer
Steerleft = 535
Dim Steercentre As Integer
Steercentre = 775
Dim Steermax As Integer
Steermax = 890
Dim Steermin As Integer
Steermin = 535
Steer = 0
Wheel = 0
Dim W As Word , Channel As Byte
Channel = 1
Dim N As String * 5
Dim Q As String * 1
Dim R As String * 1
Dim S As String * 1

Config Portc = Output
Fwf Alias Portc.0
Fwr Alias Portc.1
Fsr Alias Portc.2
Fsl Alias Portc.3

'config
Config Adc = Single , Prescaler = Auto
Start Adc

'Dim W As Word
Lcd "REAR RIGHT"
Wait 1

Do
W = Getadc(0)
Cls
Lcd W ; "               "

Input N
W = Getadc(0)

'Cls
'Lcd N
Q = Mid(n , 1 , 1)
'Lowerline
'Lcd Q
If Q = "1" Then
   R = Mid(n , 2 , 1)
   If R = "1" Then
      S = Mid(n , 3 , 1)
         If S = "1" Then
            Wheel = 1
         Elseif S = "0" Then
            Wheel = 0
         Elseif S = "2" Then
            Wheel = 2
         End If
   Elseif R = "2" Then
      Print Wheel
   End If
Elseif Q = "2" Then
   R = Mid(n , 2 , 1)
   If R = "1" Then
      S = Mid(n , 2 , 1)
         If S = "1" Then
            Steertarget = Steerright
         Elseif S = "0" Then
            Steertarget = Steercentre
         Elseif S = "2" Then
            Steertarget = Steerleft
         End If
   Elseif R = "2" Then
      Print W
   End If
End If


''''''''''''''''''''''''

Tolerancehigh = Steertarget                                 '+ Range
Tolerancelow = Steertarget                                  ' - Range
'If Steer = 0 Then
   If W > Tolerancehigh Then
      Fsr = 1
      Fsl = 0
   Elseif W < Tolerancelow Then
      Fsr = 0
      Fsl = 1
   End If
'End If

If Wheel = 1 Then
   Fwf = 1
   Fwr = 0
Elseif Wheel = 2 Then
   Fwf = 0
   Fwr = 1
Elseif Wheel = 0 Then
   Fwf = 0
   Fwr = 0
End If
''''''''''''''''''''''''

Loop