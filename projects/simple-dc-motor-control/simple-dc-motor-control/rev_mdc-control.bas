'--------------------------
'Pengatur Kecepatan Motor DC
'--------------------------
$regfile = "m16def.dat"
'Jika menggunakan ATMega8535 maka diganti dengan "m8535.dat"
$crystal = 12000000

'--------------------------SET_PENGATURAN PUTARAN

Config Timer1 = Pwm , Pwm = 8 , Compare A Pwm = Clear Up , Prescale = 8

'--------------------------SET_TOMBOL
Pwm1a = 200
'-----------
Ddrb.2 = 0
Portb.2 = 1
Ddrb.3 = 0
Portb.3 = 1

'--------------------------LCD
Config Lcdpin = Pin , Db4 = Portc.4 , Db5 = Portc.5 , Db6 = Portc.6 , Db7 = Portc.7 , E = Portc.2 , Rs = Portc.0

Config Lcd = 16 * 2
'--------------------------

'--------------------------
Dim Putar As Integer
'========================================================
'                      """"MULAI""""
'========================================================
Putar = 1
Do

Locate 1 , 1
Lcd "Motor DC Control"


'----------------------------
If Pinb.2 = 0 Then
Waitms 200
Putar = Putar + 1
End If

If Pinb.3 = 0 Then
Waitms 200
Putar = Putar - 1
End If

If Putar > 7 Then
Putar = 1
End If

If Putar < 1 Then
Putar = 7
End If
'---------------------------
If Putar = 7 Then
Pwm1a = 5
Locate 2 , 1
Lcd "MAX=260 RPM"
End If

If Putar = 6 Then
Pwm1a = 20
Locate 2 , 1
Lcd "V = 220 RPM"
End If

If Putar = 5 Then
Pwm1a = 35
Locate 2 , 1
Lcd "V = 180 RPM"
End If

If Putar = 4 Then
Pwm1a = 50
Locate 2 , 1
Lcd "V = 140 RPM"
End If

If Putar = 3 Then
Pwm1a = 70
Locate 2 , 1
Lcd "V = 120 RPM"
End If

If Putar = 2 Then
Pwm1a = 90
Locate 2 , 1
Lcd "V = 80 RPM"
End If

If Putar = 1 Then
Pwm1a = 110
Locate 2 , 1
Lcd "MIN=60 RPM"
End If
Waitms 300
Loop
'--------------------------- end