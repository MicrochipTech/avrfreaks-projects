'-------------------------------------------------------

'File Name: tiplayCOMMON.BAS
'Function : <type> triplay cOmMoN
'Version  : 1.11.ODD
'Desc     : A tRiPlAy cOmMoN IC program
'-------------------------------------------------------
'specify from code which chip we use:
$regfile = "m16def.dat"
'Specify the on board crystal
$crystal = 8000000                                          '3579545
'Specify Baud Rate
'$baud = 9600
$baud = 9600
'$large
'$sim
'Config Serialin = Buffered , Size = 20
Config Lcdbus = 4
Config Lcd = 16 * 2
Config Lcdpin = Pin , Db4 = Portb.5 , Db5 = Portb.4 , Db6 = Portb.3 , Db7 = Portb.2 , E = Portb.6 , Rs = Portb.7

Config Adc = Single , Prescaler = Auto
Start Adc
'''''''''''''''''''''''''''''''''''''
''RS232
Dim S As String * 1
Dim Q As String * 1
Dim R As String * 1
Dim T As String * 5
Dim W As Word

''Read Info
Dim Wheel As Byte , Steer As Byte                           'values can be from 0-2
'0=centre/no motion
'1=right/forward
'2=left/reverse
'''''''''''''''''''''''''''''''''''''''
Do

Input T
R = Mid(t , 1 , 1)
If R = "5" Then                                             'Rear Right Wheel
   Q = Mid(t , 2 , 1)
   If Q = "1" Then                                          'Assign Value
      S = Mid(t , 3 , 1)
      If S = "1" Then
         Wheel = 1                                          'forward
      Elseif S = "2" Then
         Wheel = 2                                          'revers
      Elseif S = "0" Then
         Wheel = 0                                          'no motion
      End If
   Elseif Q = "2" Then                                      'Read Value
      Print Wheel                                           'give the status of wheel
   End If
Elseif R = "6" Then                                         'Rear Right Steer
   Q = Mid(t , 2 , 1)                                       'Assign Value
   If Q = "1" Then
      S = Mid(t , 3 , 1)
      If S = "1" Then
         Steer = 1                                          'Right
      Elseif S = "2" Then
         Steer = 2                                          'Left
      Elseif S = "0" Then
         Steer = 0                                          'Centre
      End If
   Elseif Q = "2" Then
      Print Steer
   End If
End If

Cls
Lcd S
Lowerline
W = Getadc(0)
Lcd W
Loop