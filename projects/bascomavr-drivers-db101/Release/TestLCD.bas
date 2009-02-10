' file TestLCD.bas
' Sample program for testing LCD and Joystick driver for AVR DB101
'

$regfile = "M1281def.dat"
$include "declares.bas"


$crystal = 7380000
$hwstack = 60
$swstack = 60
$framesize = 60

Declare Function Menu_key() As Byte
Declare Sub Draw_menu(byval Curr_item As Byte)
Declare Sub Contrast_set()
Declare Sub Draw_hbar(byval X As Byte , Byval Y As Byte , Byval H As Byte , Byval L As Byte)
Declare Sub Clear_hbar(byval X As Byte , Byval Y As Byte , Byval H As Byte , Byval L As Byte)
Declare Sub Rgb_set()
Declare Sub Invert_screen()
Declare Sub Normal_screen()
Declare Sub Serial_set()
Declare Sub Beep_tone()

Dim A As Byte , Contrast As Byte
Dim Rgb As Word , Rgbt As Word
'Dim R As Byte , G As Byte , B As Byte
Dim C(3) As Byte
Dim Tone As Word
'$sim
'Speaker Alias Portb.7
Dim Menu(6) As String * 22
Dim Stric(3) As String * 1
Dim Mnu_key As Byte

Const Mnu_items = 6

Menu(1) = "Contrast"
Menu(2) = "RGB"
Menu(3) = "Invert Screen"
Menu(4) = "Normal Screen"
Menu(5) = "Serial Settings"
Menu(6) = "Beep Tone"

Config Com2 = 115200 , Synchrone = 0 , Parity = None , Stopbits = 1 , Databits = 8 , Clockpol = 0
Open "COM2:" For Binary As #1


Dim Char As Byte , X1 As Byte , Y1 As Byte , S As String * 8
Ddrd.4 = 1
Portd.4 = 0

Config Portb.4 = Input
Portb.4 = 1
Config Portc = Output
Portc = 0

On Pci0 Joy_isr
Pcmsk0 = &B00010000
Enable Interrupts
Enable Pci0

Print #1 , "Hello"

Call Lcd_init()

Call Lcd_clear

Call Lcd_print_at(32 , 3 , 1 , 0 , "HELLO")
For Tone = 200 To 500 Step 10
Sound Speaker , 100 , Tone
Next Tone
Readeeprom Tone , Etone
If Tone = 65535 Then Tone = 240

Do
If Key <> 0 Then
Key = 0
Mnu_key = Menu_key()
Select Case Mnu_key
Case 1 : Call Contrast_set
Case 2 : Call Rgb_set
Case 3 : Call Invert_screen
Case 4 : Call Normal_screen
Case 5 : Call Serial_set
Case 6 : Call Beep_tone

End Select
Call Lcd_clear
Call Lcd_print_at(32 , 3 , 1 , 0 , "HELLO")
End If
Loop

'=================================
Function Menu_key() As Byte
Local Curr_item As Byte
Curr_item = 1
Call Draw_menu(curr_item)

Do
If Key <> 0 Then
Select Case Key
Case K_down : Incr Curr_item
Case K_up : Decr Curr_item
Case K_enter : Exit Do
End Select
Key = 0
If Curr_item > Mnu_items Then Curr_item = 1
If Curr_item = 0 Then Curr_item = Mnu_items
Call Draw_menu(curr_item)
End If
Loop
Key = 0
Menu_key = Curr_item
End Function
'============================

Sub Draw_menu(byval Curr_item As Byte)
Local I As Byte , J As Byte , Inv As Byte
For I = 1 To Mnu_items
J = I                                                       '+ 2
Inv = 0
If I = Curr_item Then Inv = 1
Call Lcd_print_at(25 , J , 0 , Inv , Menu(i))
Next I
End Sub
'====================================

Sub Rgb_set()
Local Stri As String * 3 , Color As Byte , Tmp As Byte      ', Stric(3) As String * 1       ' C1 =R, C2=G, C3=B
Stric(1) = "R"
Stric(2) = "G"
Stric(3) = "B"
Call Lcd_clear

Call Lcd_print_at(5 , 0 , 0 , 0 , "=== COLOR ADJ ===")
    Readeeprom C(1) , Er
    Readeeprom C(2) , Eg
    Readeeprom C(3) , Eb
For Color = 1 To 3
Tmp = 2 * Color
Call Lcd_print_at(5 , Tmp , 0 , 0 , Stric(color))
Stri = Str(c(color))
Call Lcd_print_at(12 , Tmp , 0 , 0 , Stri)
Next Color

Color = 1
Do
If Key <> 0 Then
   Select Case Key
           Case K_up : Incr Color
           Case K_down : Decr Color
           Case K_right : C(color) = C(color) + 4
           Case K_left : C(color) = C(color) - 4
           Case K_enter : Exit Do
   End Select
Key = 0
   If Color = 4 Then Color = 1
   If Color = 0 Then Color = 3

   Stri = Str(color)
   Call Lcd_print_at(8 , 7 , 0 , 1 , "  ")
   Call Lcd_print_at(12 , 7 , 0 , 1 , Stric(color))

   Stri = Str(c(color))
   Tmp = 2 * Color
   Call Lcd_print_at(12 , Tmp , 0 , 0 , "   ")
   Call Lcd_print_at(12 , Tmp , 0 , 0 , Stri)
End If
   Call Set_rgb(c(1) , C(2) , C(3))
Loop
   Writeeeprom C(1) , Er
   Writeeeprom C(2) , Eg
   Writeeeprom C(3) , Eb
End Sub
'===========================================

Sub Contrast_set()
Local Cont As String * 3 , Mulcont As Byte
Call Lcd_clear()
Call Lcd_print_at(5 , 0 , 0 , 0 , "====CONTRAST SET====")

Do
If Key = 0 Then Goto Scip
Call Clear_hbar(20 , 16 , 6 , Mulcont)
If Key = K_up Then Incr Contrast
If Key = K_down Then Decr Contrast
If Key = K_enter Then Exit Do
If Contrast = 33 Then Contrast = 32
If Contrast = 0 Then Contrast = 1
Key = 0
Call Lcd_write_cmd(lcd_cmd_srv)
Call Lcd_write_cmd(contrast)
Mulcont = 3 * Contrast
Cont = Str(contrast)
Call Lcd_print_at(2 , 2 , 0 , 0 , "   ")
Call Lcd_print_at(2 , 2 , 0 , 0 , Cont)
Call Draw_hbar(20 , 16 , 6 , Mulcont)
Scip:
Loop
Key = 0
Writeeeprom Contrast , Econtrast
End Sub

'========================================================

Sub Beep_tone()
Local Ton As String * 5 , Multone As Byte
Call Lcd_clear()
Call Lcd_print_at(5 , 0 , 0 , 0 , "===Beep Tone Set===")
Ton = Str(tone)
Call Lcd_print_at(2 , 2 , 0 , 0 , Ton)
Do
If Key = 0 Then Goto Scip1
Call Clear_hbar(30 , 16 , 6 , Multone)
If Key = K_up Then Tone = Tone + 8
If Key = K_down Then Tone = Tone - 8
If Key = K_enter Then Exit Do
If Tone > 2047 Then Tone = 2047
If Tone < 128 Then Tone = 128
Key = 0
Multone = High(tone)                                        ' / 16
Ton = Str(tone)
Call Lcd_print_at(2 , 2 , 0 , 0 , "    ")
Call Lcd_print_at(2 , 2 , 0 , 0 , Ton)
Scip1:
Loop
Key = 0
Writeeeprom Tone , Etone
End Sub

'================================================
Sub Draw_hbar(byval X As Byte , Byval Y As Byte , Byval H As Byte , Byval L As Byte)
Local Hh As Byte , Xx As Byte , Lx As Byte , Hx As Byte
Lx = X + L
Hx = Y + H
Decr Lx

For Hh = Y To Hx
For Xx = X To Lx
Call Lcd_putpixel(xx , Hh)
Next Xx
Next Hh
End Sub

Sub Clear_hbar(byval X As Byte , Byval Y As Byte , Byval H As Byte , Byval L As Byte)
Local Hh As Byte , Xx As Byte , Lx As Byte , Hx As Byte
Lx = X + L
Hx = Y + H

For Hh = Y To Hx
For Xx = X To Lx
Call Lcd_clrpixel(xx , Hh)
Next Xx
Next Hh
End Sub


Sub Invert_screen()

Call Lcd_write_cmd(lcd_cmd_reverse_on)
End Sub

Sub Normal_screen()

Call Lcd_write_cmd(lcd_cmd_reverse_off)

End Sub

Sub Serial_set()
Call Lcd_clear()
'Close #1
Ucsr1a.1 = 0
Menu(1) = "9600"
Menu(2) = "19200"
Menu(3) = "38400"
Menu(4) = "57600"
Menu(5) = "115200"
Menu(6) = "          "
Mnu_key = Menu_key()
Select Case Mnu_key
Case 1 : Ubrr1 = 47
Case 2 : Ubrr1 = 23
Case 3 : Ubrr1 = 11
Case 4 : Ubrr1 = 7
Case 5 : Ubrr1 = 3


End Select

'Open "COM2:" For Binary As #1
Print #1 , "Hello serial"

Menu(1) = "Contrast"
Menu(2) = "RGB"
Menu(3) = "Invert Screen"
Menu(4) = "Normal Screen"
Menu(5) = "Serial Settings"
Menu(6) = "Beep Tone"
End Sub




End

$include "Joystick.bas"
$include "lcd.bas"