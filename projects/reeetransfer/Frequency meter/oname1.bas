$regfile = "M8def.dat"
$crystal = 8000000
$sim
Config Lcdpin = Pin , Db4 = Pinc.5 , Db5 = Pinc.4 , _
        , Db6 = Pind.1 , Db7 = Pind.0 , Rs = Pind.6 , E = Pind.7
Config Lcd = 16 * 2
Cursor Off
Config Portb.1 = Output
Config Portd.2 = Input
Config Portd.3 = Input
Config Portd.5 = Input                                      'counter
Ddrd.5 = 0
Portd.5 = 1                                                 'pull up
Config Timer1 = Counter , Edge = Rising
Config Timer2 = Timer , Async = On , Prescale = 128
Config Int0 = Rising
Config Int1 = Rising
Enable Interrupts
Enable Timer2
Enable Timer1
Enable Int0
Enable Int1
On Int0 Raise
On Int1 Fall
On Ovf1 Pulsecount
On Ovf2 Lable
Dim A As Long , I As Long , B As Byte , M As Integer , U As Single
B = 0
Dim Hum As Single
Hum = 65
Dim Eh As Single
Eh = Hum - U
Start Timer2
Cls
Lcd " Allah"
Wait 1

Do
If Eh > 0.2 Then
Portb.1 = 1
End If
If Eh < -0.2 Then
Portb.1 = 0
End If
Loop
End

Lable:
Toggle Portb.1
Stop Timer2
M = -7587.2
A = B * 65536
A = A + Counter1
A = A - 110
U = A - 7351
U = U * M
U = U / 100000
Cls
Locate 1 , 1
Lcd " H: " ; U ; "% "
Locate 2 , 1
Lcd " S: " ; Hum
Eh = Hum - U
B = 0
I = 0
Counter1 = 0
Start Timer2
Return

Pulsecount:
Incr B : Counter1 = 0
Return

Raise:
Hum = Hum + 0.1
Cls
Locate 1 , 1
Lcd " H: " ; U ; "% "
Locate 2 , 1
Lcd " S: " ; Hum
Eh = Hum - U
Return

Fall:
Hum = Hum - 0.1
Cls
Locate 1 , 1
Lcd " H: " ; U ; "% "
Locate 2 , 1
Lcd " S: " ; Hum
Eh = Hum - U
Return