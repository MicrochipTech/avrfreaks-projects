' file LCD.bas
' This is a very simple LCD driver for Atmel AVR DB101 board
' based on "Reflow Solder Controller" article published in
' December 2007 Elector magazine
'
' Vladimir Yershov: yershov@chloride.ru
'
Sub Lcd_write_cmd(byval Cmd As Byte)
     Ddra = 255
     Lcd_data = Cmd
     Lcd_a0 = 0                                             '' select command-data
     ''LCD_delay()
     Lcd_rd = 1                                             '' define write-access
     Lcd_wr = 0                                             '' give an Enable-strobe
     nop
     Lcd_wr = 1
     Ddra = 0
End Sub

Sub Lcd_write_dat(byval Dat As Byte)
    Ddra = 255
    P0 = Dat
    Lcd_a0 = 1                                              '' select display-data
    ''LCD_delay()
    Lcd_rd = 1                                              '' define write-access

    Lcd_wr = 0                                              '' give an Enable-strobe
    nop
    Lcd_wr = 1
    Ddra = 0
End Sub

Function Lcd_read_dat() As Byte

    Local Rslt As Byte
    Ddra = 0
    Lcd_data = &HFF
    Lcd_a0 = 1                                              '' Select command/status register

    Lcd_wr = 1                                              '' define read-access


    Lcd_rd = 0
    nop
'    Rslt = P0                           '' read data
    Rslt = Pina
 '   Lcd_RD = 0                           '' disable the LCD
    Lcd_rd = 1
    Lcd_read_dat = Rslt
End Function

Function Lcd_read_stat() As Byte

    Local Rslt As Byte
    Ddra = 0
    Lcd_data = &HFF
    Lcd_a0 = 0                                              '' Select command/status register
    ''LCD_delay()
    Lcd_wr = 1                                              '' define read-access
    ''LCD_delay()
 '   Lcd_RD = 1                           '' enable the LCD
    Lcd_rd = 0
    nop
 '   Rslt = P0                           '' read data
    Rslt = Pina
 '   Lcd_RD = 0                           '' disable the LCD
    Lcd_rd = 1
    Lcd_read_stat = Rslt
End Function

Sub Lcd_clear()

  Local Pa As Byte , Col As Byte

  For Pa = 0 To Lcd_rows
    Call Lcd_set_pos(pa , 0 )                               ''
    For Col = 0 To Lcd_cols
       Call Lcd_write_dat(0)
       '' Print #1 , "pa=" ; Pa;
       '' Print #1 , " col=" ; Col : A = Waitkey(#1)
    Next Col
  Next Pa
End Sub

Sub Lcd_clear_bottom()

  Local Pa As Byte , Col As Byte
  For Pa = 3 To Lcd_rows
    Call Lcd_set_pos(pa , 0 )
    For Col = 0 To Lcd_cols
        Call Lcd_write_dat(&H00 )
        Next Col
  Next Pa
End Sub

Sub Lcd_set_pos(byval Row As Byte , Byval Col As Byte )
  Local Tmp As Byte , Tmp1 As Byte
  Tmp = Lcd_cmd_spage Or Row
  Call Lcd_write_cmd(tmp )
'  LCD_write_cmd ( LCD_CMD_COL | (( col >> 4 ) & &H0F ))
  Tmp1 = Col
  Shift Tmp1 , Right , 4
  Tmp1 = Tmp1 And &H0F
  Tmp = Lcd_cmd_col Or Tmp1
  Call Lcd_write_cmd(tmp)
'  LCD_write_cmd ( &H00 | (col & &H0F) )
  Tmp = Col And &H0F
  Tmp = Tmp Or &H00
  Call Lcd_write_cmd(tmp)
End Sub

Sub Lcd_init()
    Local Count As Word , Tmp As Byte , R As Byte , G As Byte , B As Byte
    Mcucr.4 = 0
    Ddra = 0
    Porta = 255
    Pina = 255
    Ddrc = &B11111011
    Ddrg = &B00000111
    Portg.0 = 0
    Ddre.5 = 1
    Porte.5 = 1

    Lcd_rd = 1                                              '' Enable is inactive
    Lcd_wr = 1
    Lcd_a0 = 1
    Lcd_reset = 0                                           '' Reset LCD

    Waitms 100
    Lcd_reset = 1                                           '' end of hard reset LCD

    Call Lcd_write_cmd(lcd_cmd_reset)                       '' Software reset

    Tmp = Lcd_read_stat() And Lcd_stat_resetb
    While Tmp <> 0                                          '' Wait for reset to complete
    Tmp = Lcd_read_stat() And Lcd_stat_resetb
    Wend

    Call Lcd_write_cmd(lcd_cmd_adc)                         '' Set ADC "0" flip horizontal

    Call Lcd_write_cmd(lcd_cmd_shl)                         '' Set SHL "0" flip vertical

    Tmp = Lcd_cmd_bias Or 1
    Call Lcd_write_cmd(tmp)                                 '' Set BIAS '1'

    '' Enable Voltage converter
    Tmp = Lcd_cmd_powerc Or Lcd_powerc_vc
    Call Lcd_write_cmd(tmp )                                '' Control power circuit
    Waitms 100

    '' Enable Voltage Regulator
    '  LCD_write_cmd ( LCD_CMD_POWERC | LCD_POWERC_VC | LCD_POWERC_VR )   '' Control power circuit
    Tmp = Lcd_cmd_powerc Or Lcd_powerc_vc
    Tmp = Tmp Or Lcd_powerc_vr
    Call Lcd_write_cmd(tmp)                                 '' Control power circuit
    Waitms 100

    '' Enable Voltage Follower
    '   LCD_write_cmd ( LCD_CMD_POWERC | LCD_POWERC_VC | LCD_POWERC_VR | LCD_POWERC_VF )    '' Control power circuit
    Tmp = Lcd_cmd_powerc Or Lcd_powerc_vc
    Tmp = Tmp Or Lcd_powerc_vr
    Tmp = Tmp Or Lcd_powerc_vf
    Call Lcd_write_cmd(tmp)
    Waitms 100

    '' Regulator Resistor select
    Tmp = Lcd_cmd_res Or 6                                  '' Select regulator resistor
    Call Lcd_write_cmd(tmp)

    '' Set voltage reference
    Readeeprom Tmp , Econtrast
    If Tmp > 32 Then
    Tmp = 10
    Writeeeprom Tmp , Econtrast
    End If
    Call Lcd_write_cmd(lcd_cmd_srv )                        '' Set reference voltage mode
    Call Lcd_write_cmd(tmp)                                 '' Set voltage reference register = Contrast

    Tmp = Lcd_cmd_on Or 1
    Call Lcd_write_cmd(tmp)                                 '' Display on

    Tmp = Lcd_cmd_sir Or 1
    Call Lcd_write_cmd(tmp)                                 '' Set indicator mode

    Tmp = &H00 Or 3
    Call Lcd_write_cmd(tmp )                                '' Set indicator register "Always on"

    Call Lcd_write_cmd(lcd_cmd_eon)                         '' Set display normal (instead of all on)

    Call Lcd_write_cmd(lcd_cmd_reverse_off)                 '' Set normal display (instead of inverse)

    Call Lcd_write_cmd(lcd_cmd_idl )                        ''

    Call Lcd_write_cmd(lcd_cmd_smr)                         '' Set Modify-Read. This prevents the column adress to
                                                            '' keep on incrementing the column adress
    Call Lcd_set_pos(0 , 0)

    Call Lcd_clear()

'===== Set display backlight =====
    Readeeprom R , Er
    Readeeprom G , Eg
    Readeeprom B , Eb
    If R = 255 Then Call Set_rgb(136 , 96 , 80)
    Call Set_rgb(r , G , B)
    Readeeprom Tone , Etone
    If Tone = 65535 Then Tone = 240
End Sub

Sub Lcd_putchar(byval Character As Byte)

  Local Index As Integer , Tmp As Byte , Dbyte As Byte
  Character = Character - 32
  If Character > 95 Then Character = 0
  Index = Character * 5
  For Tmp = 1 To 5
  Dbyte = Lookup(index , Font)
  Call Lcd_write_dat(dbyte)
  Incr Index
  Next Index
  Call Lcd_write_dat(0)
End Sub

Sub Lcd_putchar_inv(byval Character As Byte)

  Local Index As Integer , Tmp As Byte , Dbyte As Byte
  Character = Character - 32
  If Character > 95 Then Character = 0
  Index = Character * 5
  For Tmp = 1 To 5
  Dbyte = Lookup(index , Font)
  Dbyte = Not Dbyte
  Call Lcd_write_dat(dbyte)
  Incr Index
  Next Index
  Call Lcd_write_dat(255)
End Sub

Sub Lcd_putchar_bold(byval Character As Byte)

  Local Index As Integer , Tmp As Byte , Tmp1 As Byte , Prev_bitmap As Byte , Dbyte As Byte
  Prev_bitmap = 0
  Character = Character - 32
  If Character > 95 Then Character = 0
 '' for (index=(character*5);index<((character*5)+5);index++)
  Index = Character * 5
        For Tmp = 1 To 5
        Tmp1 = Lookup(index , Font)
        Tmp1 = Tmp1 Or Prev_bitmap
        Call Lcd_write_dat(tmp1)
        Prev_bitmap = Lookup(index , Font)
        Incr Index
        Next Tmp
  Call Lcd_write_dat(prev_bitmap)
  Call Lcd_write_dat(0)
End Sub

Sub Lcd_putchar_bold_inv(byval Character As Byte)

  Local Index As Integer , Tmp As Byte , Tmp1 As Byte , Prev_bitmap As Byte , Dbyte As Byte
  Prev_bitmap = 0
  Character = Character - 32
  If Character > 95 Then Character = 0
 '' for (index=(character*5);index<((character*5)+5);index++)
  Index = Character * 5
        For Tmp = 1 To 5
        Tmp1 = Lookup(index , Font)
        Tmp1 = Tmp1 Or Prev_bitmap
        Tmp1 = Not Tmp1
        Call Lcd_write_dat(tmp1)
        Prev_bitmap = Lookup(index , Font)
        Incr Index
        Next Tmp
        Prev_bitmap = Not Prev_bitmap
  Call Lcd_write_dat(prev_bitmap)
  Call Lcd_write_dat(255)
End Sub


Sub Lcd_print_at(byval X As Byte , Byval Y As Byte , Byval Bold As Byte , Byval Inv As Byte , Byval Char As String)
'Sub Lcd_print_at(byval X As Byte , Byval Y As Byte , Byval Bold As Byte , Char As String)
    Local Tmp As Byte , Tmp1 As String * 1 , Tmp2 As Byte , Lengh As Byte       ', Inv As Byte
    Call Lcd_set_pos(y , X)
    Lengh = Len(char)
   For Tmp = 1 To Lengh
     Tmp1 = Mid(char , Tmp , 1)
     Tmp2 = Tmp1
     If Bold = 1 Then
        If Inv = 1 Then
        Call Lcd_putchar_bold_inv(tmp2)
        Else
        Call Lcd_putchar_bold(tmp2)
        End If
     Else
        If Inv = 1 Then
        Call Lcd_putchar_inv(tmp2 )
        Else
        Call Lcd_putchar(tmp2 )
        End If
     End If
     Next Tmp
End Sub

Sub Lcd_putpixel(byval X As Byte , Byval Y As Byte)

    Local Display_dat As Byte , Tmp As Byte , One As Byte
    One = 1
    Tmp = Y / 8
    Call Lcd_set_pos(tmp , X)
    Display_dat = Lcd_read_dat()                            '' First, a dummy read dummy read. See datasheet S6B1713
    Display_dat = Lcd_read_dat()
    Call Lcd_set_pos(tmp , X)
    Tmp = Y Mod 8
    Shift One , Left , Tmp
    Tmp = One Or Display_dat
    Call Lcd_write_dat(tmp)
End Sub

Sub Lcd_clrpixel(byval X As Byte , Byval Y As Byte)

    Local Display_dat As Byte , Tmp As Byte , One As Byte
    One = 0
    Tmp = Y / 8
    Call Lcd_set_pos(tmp , X)
    Display_dat = Lcd_read_dat()                            '' First, a dummy read dummy read. See datasheet S6B1713
    Display_dat = Lcd_read_dat()
    Call Lcd_set_pos(tmp , X)
    Tmp = Y Mod 8
    Shift One , Left , Tmp
    Tmp = One And Display_dat
    Call Lcd_write_dat(tmp)
End Sub

Sub Display_splash(byval Xst As Byte , Byval Yst As Byte)   ' 0<=Xst<=127 horis pos, 0<=Yst<=7 муке pos
  Local Xs As Byte , Ys As Byte , Tmps As Byte , Xx1 As Byte , Yy1 As Byte , Dbyte As Byte

  For Xs = 0 To 23
    For Ys = 0 To 2
       Tmps = Xs * 3
       Tmps = Tmps + Ys
       Xx1 = Xs + Xst
       Yy1 = Ys + Yst
       Call Lcd_set_pos(yy1 , Xx1)
       Dbyte = Lookup(tmps , Logo )
       Call Lcd_write_dat(dbyte)
    Next Ys
  Next Xs
End Sub

Sub Lcd_shift_left(byval Yst As Byte , Byval Yend As Byte)  '0=<Yst<=7, 1<=Yend<=7

  Local X1s As Byte , X2s As Byte , Yss As Byte , Dats As Byte , Tmpss As Byte , Tmp1s As Byte       ', Strin(16) As Byte
   Call Lcd_write_cmd(lcd_cmd_rmr)                          '' ReSet Modify-Read.
    For Yss = Yst To Yend
       For X1s = 0 To 127 Step 16
          Tmpss = X1s + 1
          Call Lcd_set_pos(yss , Tmpss)
          Dats = Lcd_read_dat()                             '' dummy read. See datasheet S6B1713
         For X2s = 1 To 16
          Strin(x2s) = Lcd_read_dat()
         Next X2s
          Call Lcd_set_pos(yss , X1s)
         For X2s = 1 To 16
          Tmp1s = X1s + X2s
          If Tmp1s < 128 Then
          Call Lcd_write_dat(strin(x2s))
          Else
          Call Lcd_write_dat(0)
          End If
         Next X2s
      Next X1s
     Next Yss
     Call Lcd_write_cmd(lcd_cmd_smr)                        '' Set Modify-Read. This prevents the column adress to
End Sub

Sub Set_rgb(byval R As Byte , Byval G As Byte , Byval B As Byte)
    Pwm3a = R
    Pwm3b = G
    Pwm3c = B

End Sub

End

Logo:
Data &HFF , &HFF , &HFF,
Data &HFF , &HFF , &HFF,
Data &HFF , &HFF , &HFF,
Data &HFF , &HFF , &HFF,
Data &HFF , &HC0 , &HFF,
Data &H3F , &H00 , &HFE,
Data &H1F , &H00 , &HFC,
Data &H0F , &H00 , &HF8,
Data &H87 , &H71 , &HF8,
Data &HC7 , &HF1 , &HF8,
Data &HC3 , &HF1 , &HF1,
Data &HE3 , &HF1 , &HF1,
Data &HE3 , &HF1 , &HF1,
Data &HE3 , &HF1 , &HF1,
Data &HC7 , &HF1 , &HF1,
Data &H87 , &HF1 , &HF0,
Data &H07 , &H30 , &HF8,
Data &H0F , &H30 , &HF8,
Data &H1F , &H30 , &HFC,
Data &HFF , &H30 , &HFE,
Data &HFF , &HFF , &HFF,
Data &HFF , &HFF , &HFF,
Data &HFF , &HFF , &HFF,
Data &HFF , &HFF , &HFF

Font:
                                    ''dec ascii
Data &H00 , &H00 , &H00 , &H00 , &H00 ,                     ''32 <Space>
Data &H00 , &H00 , &H4F , &H00 , &H00,                      ''33 !
Data &H00 , &H07 , &H00 , &H07 , &H00 , ''34 "
Data &H14 , &H7F , &H14 , &H7F , &H14,                      ''35 #
Data &H24 , &H2A , &H7F , &H2A , &H12,                      ''36 $
Data &H23 , &H13 , &H08 , &H64 , &H62,                      ''37 %
Data &H36 , &H49 , &H55 , &H22 , &H50,                      ''38 &
Data &H00 , &H05 , &H03 , &H00 , &H00,                      ''39 '
Data &H1C , &H22 , &H41 , &H00 , &H00,                      ''40 (
Data &H00 , &H00 , &H41 , &H22 , &H1C,                      ''41 )
Data &H14 , &H08 , &H3E , &H08 , &H14,                      ''42 *
Data &H08 , &H08 , &H3E , &H08 , &H08,                      ''43 +
Data &H00 , &H50 , &H30 , &H00 , &H00,                      ''44 ,
Data &H08 , &H08 , &H08 , &H08 , &H08,                      ''45 -
Data &H00 , &H60 , &H60 , &H00 , &H00,                      ''46 .
Data &H20 , &H10 , &H08 , &H04 , &H02,                      ''47 /
Data &H3E , &H51 , &H49 , &H45 , &H3E,                      ''48 0
Data &H00 , &H42 , &H7F , &H40 , &H00,                      ''49 1
Data &H42 , &H61 , &H51 , &H49 , &H46,                      ''50 2
Data &H21 , &H41 , &H45 , &H4B , &H31,                      ''51 3
Data &H18 , &H14 , &H12 , &H7F , &H10,                      ''52 4
Data &H27 , &H45 , &H45 , &H45 , &H39,                      ''53 5
Data &H3C , &H4A , &H49 , &H49 , &H30,                      ''54 6
Data &H01 , &H71 , &H09 , &H05 , &H03,                      ''55 7
Data &H36 , &H49 , &H49 , &H49 , &H36,                      ''56 8
Data &H06 , &H49 , &H49 , &H49 , &H3E,                      ''57 9
Data &H00 , &H36 , &H36 , &H00 , &H00,                      ''58 :
Data &H00 , &H56 , &H36 , &H00 , &H00,                      ''59 ;
Data &H08 , &H14 , &H22 , &H41 , &H00,                      ''60 <
Data &H14 , &H14 , &H14 , &H14 , &H14,                      ''61 =
Data &H00 , &H41 , &H22 , &H14 , &H08,                      ''62 >
Data &H02 , &H01 , &H51 , &H09 , &H06,                      ''63 ?
Data &H32 , &H49 , &H79 , &H41 , &H3E,                      ''64 @
Data &H7E , &H11 , &H11 , &H11 , &H7E,                      ''65 A
Data &H7F , &H49 , &H49 , &H49 , &H36,                      ''66 B
Data &H3E , &H41 , &H41 , &H41 , &H22,                      ''67 C
Data &H7F , &H41 , &H41 , &H22 , &H1C,                      ''68 D
Data &H7F , &H49 , &H49 , &H49 , &H41,                      ''69 E
Data &H7F , &H09 , &H09 , &H09 , &H01,                      ''70 F
Data &H3E , &H41 , &H49 , &H49 , &H7A,                      ''71 G
Data &H7F , &H08 , &H08 , &H08 , &H7F,                      ''72 H
Data &H00 , &H41 , &H7F , &H41 , &H00,                      ''73 I
Data &H20 , &H40 , &H41 , &H3F , &H01,                      ''74 J
Data &H7F , &H08 , &H14 , &H22 , &H41,                      ''75 K
Data &H7F , &H40 , &H40 , &H40 , &H40,                      ''76 L
Data &H7F , &H02 , &H0C , &H02 , &H7F,                      ''77 M
Data &H7F , &H04 , &H08 , &H10 , &H7F,                      ''78 N
Data &H3E , &H41 , &H41 , &H41 , &H3E,                      ''79 O
Data &H7F , &H09 , &H09 , &H09 , &H06,                      ''80 P
Data &H3E , &H41 , &H51 , &H21 , &H5E,                      ''81 Q
Data &H7F , &H09 , &H19 , &H29 , &H46,                      ''82 R
Data &H46 , &H49 , &H49 , &H49 , &H31,                      ''83 S
Data &H01 , &H01 , &H7F , &H01 , &H01,                      ''84 T
Data &H3F , &H40 , &H40 , &H40 , &H3F,                      ''85 U
Data &H1F , &H20 , &H40 , &H20 , &H1F,                      ''86 V
Data &H3F , &H40 , &H38 , &H40 , &H3F,                      ''87 W
Data &H63 , &H14 , &H08 , &H14 , &H63,                      ''88 X
Data &H07 , &H08 , &H70 , &H08 , &H07,                      ''89 Y
Data &H61 , &H51 , &H49 , &H45 , &H43,                      ''90 Z
Data &H7F , &H41 , &H41 , &H00 , &H00,                      ''91 [
Data &H02 , &H04 , &H08 , &H10 , &H20,                      ''92 (BackSlash)
Data &H00 , &H00 , &H41 , &H41 , &H7F,                      ''93 ]
Data &H04 , &H02 , &H01 , &H02 , &H04,                      ''94 ^
''Data &H40,&H40,&H40,&H40,&H40, ''95 _
Data &H00 , &H02 , &H05 , &H02 , &H00,                      ''95  Celsius-sign
Data &H00 , &H01 , &H02 , &H04 , &H00,                      ''96 `
Data &H20 , &H54 , &H54 , &H54 , &H78,                      ''97 a
Data &H7F , &H48 , &H44 , &H44 , &H38,                      ''98 b
Data &H38 , &H44 , &H44 , &H44 , &H20,                      ''99 c
Data &H38 , &H44 , &H44 , &H48 , &H7F,                      ''100 d
Data &H38 , &H54 , &H54 , &H54 , &H18,                      ''101 e
Data &H08 , &H7E , &H09 , &H01 , &H02,                      ''102 f
Data &H0C , &H52 , &H52 , &H52 , &H3E,                      ''103 g
Data &H7F , &H08 , &H04 , &H04 , &H78,                      ''104 h
Data &H00 , &H44 , &H7D , &H40 , &H00,                      ''105 i
Data &H00 , &H20 , &H40 , &H44 , &H3D,                      ''106 j
Data &H7F , &H10 , &H28 , &H44 , &H00,                      ''107 k
Data &H00 , &H41 , &H7F , &H40 , &H00,                      ''108 l
Data &H7C , &H04 , &H18 , &H04 , &H78,                      ''109 m
Data &H7C , &H08 , &H04 , &H04 , &H78,                      ''110 n
Data &H38 , &H44 , &H44 , &H44 , &H38,                      ''111 o
Data &H7C , &H14 , &H14 , &H14 , &H08,                      ''112 p
Data &H08 , &H14 , &H14 , &H18 , &H7C,                      ''113 q
Data &H7C , &H08 , &H04 , &H04 , &H08,                      ''114 r
Data &H48 , &H54 , &H54 , &H54 , &H20,                      ''115 s
Data &H04 , &H3F , &H44 , &H40 , &H20,                      ''116 t
Data &H3C , &H40 , &H40 , &H20 , &H7C,                      ''117 u
Data &H1C , &H20 , &H40 , &H20 , &H1C,                      ''118 v
Data &H3C , &H40 , &H30 , &H40 , &H3C,                      ''119 w
Data &H44 , &H28 , &H10 , &H28 , &H44,                      ''120 x
Data &H0C , &H50 , &H50 , &H50 , &H3C,                      ''121 y
Data &H44 , &H64 , &H54 , &H4C , &H44,                      ''122 z
Data &H08 , &H36 , &H41 , &H00 , &H00,                      ''123 {
Data &H00 , &H00 , &H7F , &H00 , &H00,                      ''124 |
Data &H00 , &H00 , &H41 , &H36 , &H08,                      ''125 }
Data &H00 , &H08 , &H04 , &H08 , &H04                       ''126 ~