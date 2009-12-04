$regfile = "attiny2313.DAT"
$hwstack = 40
$swstack = 20
$framesize = 24
$crystal = 20000000
Config Timer0 = Timer , Prescale = 8
On Timer0 Frame : Enable Interrupts
Config Timer1 = Timer , Prescale = 1 , Compare A = Toggle , Clear Timer = 1
Timer1 = 0 : Compare1a = 10000 : Stop Timer1 : Config Portd = Output
Dim Linecount As Word , Count As Byte , Sndon As Bit , X As Bit
Const Red = 9
Const Green = 10
Const Blue = 12
Const Yellow = 11
Const Magenta = 13
Const Lblue = 14
Const White = 15
Const Black = 8
Ddrb = &B00001100 : Portb = &B00000011
Selector Alias Pinb.0 : Soundon Alias Pinb.1 : Led Alias Portb.2 : Video Alias Portd
Linecount = 1 : Count = 0 : Sndon = 0 : Enable Timer0
'--------------------------------------------------------------------------------
Bars:
rcall start_pattern
Do
rcall bardraw
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Smtp:
rcall start_pattern
Do
If Linecount <= 220 Then
rcall bardraw
Else
Video = Red
End If
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Smtp1:
rcall start_pattern
Do
If Linecount <= 220 Then
rcall bardraw
Else
Video = Green
End If
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Smtp2:
rcall start_pattern
Do
If Linecount <= 220 Then
rcall bardraw
Else
Video = Blue
End If
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Smtp3:
rcall start_pattern
Do
If Linecount >= 175 Then
If Linecount < 255 Then
rcall delay_time
Video = Red
rcall delay16
Video = Green
rcall delay16
Video = Blue
rcall delay16
Video = Black
Else
Video = Black
End If
Elseif Linecount <= 160 Then
rcall bardraw
Else
rcall delay_time
Video = Blue
rcall delay_time
Video = Black
rcall delay_time
Video = Magenta
rcall delay_time
Video = Black
rcall delay_time
Video = Lblue
rcall delay_time
Video = Black
rcall delay_time
Video = White
rcall delay_time
Video = Black
End If
Ext:
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Random:
rcall start_pattern
Do
If Linecount <= 90 Then
rcall bardraw
Elseif Linecount >= 234 Then
rcall bardraw
Elseif Linecount >= 192 Then
Video = Black
Elseif Linecount >= 132 Then
If Linecount <> 162 Then
ldi R17, $06
Random0:
dec R17
brne random0
rcall line_draw
Else
Video = White
End If
End If
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Raster:
rcall start_pattern
Do
Video = White
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Raster1:
rcall start_pattern
Do
Video = Red
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Raster2:
rcall start_pattern
Do
Video = Green
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Raster3:
rcall start_pattern
Do
Video = Blue
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Window:
rcall start_pattern
Do
If Linecount >= 250 Then
Video = Black
Elseif Linecount >= 75 Then
ldi R17, $46
Window0:
dec R17
brne window0
nop
nop
Video = White
ldi R17, $fd
Window1:
dec R17
brne window1
nop
Video = Black
End If
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Crosshatch:
rcall start_pattern
Do
If Linecount = 40 Or Linecount = 100 Or Linecount = 160 Or Linecount = 220 Or Linecount = 280 Then
Video = White
Else
rcall line_draw
End If
rcall switch_control
If X = 1 Then Exit Do
Idle
Loop
'--------------------------------------------------------------------------------
Dots:
rcall start_pattern
Do
ldi R17, $06
Loop4:
dec R17
brne loop4
If Linecount = 35 Or Linecount = 65 Or Linecount = 95 Or Linecount = 125 Or Linecount = 155 Or Linecount = 185 Or Linecount = 215 Or Linecount = 245 Or Linecount = 275 Then
Do
Incr Count
Video = White
Nop
Video = Black
ldi R17, $1a
Dot:
dec R17
brne dot
nop
nop
nop
Loop Until Count = 9
Video = White
Nop
Video = Black : Count = 0
End If
rcall switch_control
If X = 1 Then Goto Bars
Idle
Loop
'--------------------------------------------------------------------------------
Line_draw:
ldi R17, $06
Del3:
dec R17
brne del3
rcall crosshatch0
Video = White
Nop
Video = Black
rcall crosshatch0
Video = White
Nop
Video = Black
rcall crosshatch0
Video = White
Nop
Video = Black
rcall crosshatch0
Video = White
Nop
Video = Black
rcall crosshatch0
Video = White
Nop
Video = Black
Return
'--------------------------------------------------------------------------------
Bardraw:
rcall delay_time
Video = White
rcall delay_time
Video = Yellow
rcall delay_time
Video = Lblue
rcall delay_time
Video = Green
rcall delay_time
Video = Magenta
rcall delay_time
Video = Red
rcall delay_time
Video = Blue
rcall delay_time
Video = Black
Return
'--------------------------------------------------------------------------------
Delay16:
ldi R17, $4d
Wgloop0:
dec R17
brne wgloop0
nop
nop
ret
'--------------------------------------------------------------------------------
Delay_time:
ldi R17, $25
Bars1:
dec R17
brne bars1
ret
'--------------------------------------------------------------------------------
Crosshatch0:
ldi R17, $35
Crosshatch1:
dec R17
brne crosshatch1
nop
ret
'--------------------------------------------------------------------------------
Switch_control:
If Linecount < 6 Then
Debounce Selector , 0 , Next_pattern
Debounce Soundon , 0 , Make_sound
Goto Exit_sound
Next_pattern:
X = 1 : Goto Exit_sound
Make_sound:
If Sndon = 1 Then
Sndon = 0 : Led = 0 : Stop Timer1 : Goto Exit_sound
Else
Sndon = 1 : Led = 1 : Start Timer1
End If
End If
Exit_sound:
Return
'--------------------------------------------------------------------------------
Start_pattern:
X = 0
Return
'--------------------------------------------------------------------------------
Frame:
Timer0 = &H60
Video = 0
Vert:
Incr Linecount
ldi R17, $21
Frame_delay:
dec R17
brne frame_delay
nop
If Linecount > 307 And Linecount <= 312 Then
Goto Vert
Elseif Linecount = 313 Then
Linecount = 1
End If
Video = Black
Return