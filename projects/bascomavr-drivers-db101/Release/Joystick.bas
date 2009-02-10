
' file joystick.bas
' Joystick driver for AVR DB101 board
'
' Vladimir Yershov: yershov@chloride.ru
'
Joy_isr:
'If Portb.4 = 0 Then
Key = Scan_joy()
'End If
Pcifr.0 = 1
Return

Function Scan_joy()
Local Joykey As Byte

Waitms 1
If Pinb.4 = 1 Then Exit Function

Config Portb.4 = Output : Portb.4 = 0
Config Portc = Input : Portc = Portc Or &B11111000

Sound Speaker , 30 , Tone
Joykey = Pinc
Joykey = Not Joykey
Joykey = Joykey And &B11111000
Shift Joykey , Right , 3

Config Portb.4 = Input : Portb.4 = 1
Config Portc = Output : Portc = Portc And &B00000111

Scan_joy = Joykey

End Function

End