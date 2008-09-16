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

Config Portc = Output

'Configute the timer to use the clock divided by 1024

Config Timer0 = Timer , Prescale = 8
'Define the ISR handler
On Ovf0 Tim0_isr
'you may also use TIMER0 for OVF0, it is the same
Enable Timer0                                               ' enable the timer interrupt
Enable Interrupts                                           'allow interrupts to occur


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

''Assembly Info
Wheelforward Alias Portc.1
Wheelreverse Alias Portc.2
Steeringright Alias Portc.3
Steeringleft Alias Portc.4
'''''''''''''''''''''''''''''''''''''''
Cls
Lcd "Rear Right"
''''''''''''''''''''''''
'Fwf = 1
'Portc.0 = 1
Do

Input T Noecho
W = Getadc(0)
R = Mid(t , 1 , 1)
If R = "7" Then                                             'Rear Right Wheel
   Q = Mid(t , 2 , 1)
   If Q = "1" Then                                          'Assign Value
      S = Mid(t , 3 , 1)
      If S = "1" Then
         Wheel = 1                                          'forward
         Wheelforward = 1
         Wheelreverse = 0
      Elseif S = "2" Then
         Wheel = 2                                          'revers
         Wheelforward = 0
         Wheelreverse = 1
      Elseif S = "0" Then
         Wheel = 0                                          'no motion
         Wheelforward = 0
         Wheelreverse = 0
      End If
   Elseif Q = "2" Then                                      'Read Value
      Print Wheel                                           'give the status of wheel
   End If
Elseif R = "8" Then                                         'Rear Right Steer
   Q = Mid(t , 2 , 1)                                       'Assign Value
   If Q = "1" Then
      S = Mid(t , 3 , 1)
      If S = "1" Then
         Steer = 1                                          'Right
         Steeringright = 1
         Steeringleft = 0
      Elseif S = "2" Then
         Steer = 2                                          'Left
         Steeringright = 0
         Steeringleft = 1
      Elseif S = "0" Then
         Steer = 0                                          'Centre
         Steeringright = 0
         Steeringleft = 0
      End If
   Elseif Q = "2" Then
      Print W
   End If
End If

W = Getadc(0)
If Steer = 0 Then                                           'we need to check the direction only if the steering needs to come to the centre
If W < 630 Then
   Steeringright = 0                                        'right decrease
   Steeringleft = 1                                         'left increases value of w
Elseif W > 640 Then
   Steeringright = 1                                        'Right decreases
   Steeringleft = 0                                         'left increases value of w
End If
End If
'''''''''''''''''''''''''''''''''
'Cls
'Lcd S
'Lowerline
'Lcd W
Loop

Tim0_isr:
W = Getadc(0)
If Steer = 0 Then
   If W > 630 And W < 640 Then
      Steeringright = 0
      Steeringleft = 0
   End If
Elseif Steer = 1 Then
   If W < 455 Then
      Steeringright = 0
      Steeringleft = 0
   End If
Elseif Steer = 2 Then
   If W > 980 Then
      Steeringright = 0
      Steeringleft = 0
   End If
End If

Return
