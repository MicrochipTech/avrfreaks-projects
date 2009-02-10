' file declares.bas
Declare Sub Lcd_init
Declare Sub Lcd_clear
Declare Sub Lcd_clear_bottom
Declare Sub Lcd_putpixel(byval X As Byte , Byval Y As Byte )
Declare Sub Lcd_clrpixel(byval X As Byte , Byval Y As Byte)
Declare Sub Lcd_shift_left __reentrant
Declare Sub Lcd_write_dat(byval Dat As Byte )
Declare Sub Lcd_set_pos(byval Row As Byte , Byval Col As Byte )
''Declare sub  LCD_drawline (unsigned char x1, unsigned char y1,unsigned char x2, unsigned char y2)
Declare Sub Lcd_write_cmd(byval Cmd As Byte)
Declare Function Lcd_read_dat() As Byte
Declare Function Lcd_read_stat() As Byte
Declare Sub Lcd_putchar(byval Character As Byte)
Declare Sub Lcd_putchar_bold(byval Character As Byte)
Declare Sub Lcd_shift_left(byval Yst As Byte , Byval Yend As Byte)
Declare Sub Display_splash(byval Xst As Byte , Byval Yst As Byte)
Declare Sub Lcd_print_at(byval X As Byte , Byval Y As Byte , Byval Bold As Byte , Byval Inv As Byte , Byval Char As String)
Declare Sub Lcd_putchar_inv(byval Character As Byte)
Declare Sub Lcd_putchar_bold_inv(byval Character As Byte)
Declare Sub Set_rgb(byval R As Byte , Byval G As Byte , Byval B As Byte)
Declare Function Scan_joy() As Byte
'' **** LCD 128x64 dots  Displaytech 64128G
Const Lcd_rows = 7                                          '' 8 bit row x 8 row = 64 dots
Const Lcd_cols = 127                                        '' 1 bit colum x 128 = 128 Dots

Const Lcd_cmd_col = &H10
Const Lcd_cmd_res = &H20
Const Lcd_cmd_powerc = &H28
Const Lcd_cmd_idl = &H40                                    'Initial display line
Const Lcd_cmd_srv = &H81
Const Lcd_cmd_reset = &HE2
Const Lcd_cmd_adc = &HA0
Const Lcd_cmd_bias = &HA2
Const Lcd_cmd_eon = &HA4
Const Lcd_cmd_reverse_on = &HA7
Const Lcd_cmd_reverse_off = &HA6
Const Lcd_cmd_sir = &HAC
Const Lcd_cmd_on = &HAE
Const Lcd_cmd_spage = &HB0
Const Lcd_cmd_shl = &HC0
Const Lcd_cmd_smr = &HE0                                    'Set modify-read
Const Lcd_cmd_rmr = &HEE                                    'Reset modify-read

Const Lcd_stat_busy = &H80
Const Lcd_stat_adc = &H40
Const Lcd_stat_on = &H20
Const Lcd_stat_resetb = &H10

Const Lcd_powerc_vc = &H04
Const Lcd_powerc_vr = &H02
Const Lcd_powerc_vf = &H01

Const Lcd_pixel_off = 0
Const Lcd_pixel_on = 1
Const Lcd_pixel_inv = 2

Const K_right = 1
Const K_down = 2
Const K_up = 4
Const K_left = 8
Const K_enter = 16


' EEPROM MAP
Const Econtrast = 0
Const Er = Econtrast + 1
Const Eg = Er + 1
Const Eb = Eg + 1
Const Etone = Eb + 1
'Joystick
Dim Key As Byte
'Dim Tone As Word
Speaker Alias Portb.7

Dim Strin(16) As Byte
'Dim R          as byte
'dim G          as byte
'dim B          as byte

Lcd_cs Alias Portg.2                                        ' (1)
Lcd_reset Alias Portc.1                                     ' (2)
Lcd_rd Alias Portg.1                                        ' LCD_RD(5) or E
Lcd_wr Alias Portg.0                                        ' LCD_WR(4)
Lcd_a0 Alias Portc.0                                        ' (3) A0
Lcd_data Alias Porta
P0 Alias Porta
Jport Alias Portc

Config Portb.4 = Input
Config Timer3 = Pwm , Prescale = 1 , Compare A Pwm = Clear Down , Compare B Pwm = Clear Down , Compare C Pwm = Clear Down
'On Pci0 Joy_isr