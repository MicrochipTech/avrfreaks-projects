;* Reset SH1106 (Vdd1 2.4V - 3.5V)

reset_SH1106:
 cbi PORTB, RST ;resetting SH1106
 ldi temp, 0x1b
 wait_pulse:
 dec temp ;wait at least 5 us (during)
 brne wait_pulse
 sbi PORTB, RST
 nop
 nop
 nop
 nop
 nop ;wait at least 1 us (ending)
 nop
 nop
 nop
 nop
 nop
 ret

  

  

  

  

  

  

  

;* Initialize SH1106: mapping, scan, display RAM

init_sh1106:
 ldi data, 0xae ;display off
 rcall write_command
 ldi data, 0xd5 ;set display clock divide ratio
 rcall write_command
 ldi data, 0x50 ;(POR=0x50)
 rcall write_command
 ldi data, 0xa8 ;set multiplex ratio
 rcall write_command
 ldi data, 0x3f ;(POR=0x3f)
 rcall write_command
 ldi data, 0xd3 ;set display offset
 rcall write_command
 ldi data, 0x00 ;(POR=0x00)
 rcall write_command
 ldi data, 0x40 ;set display start line
 rcall write_command
 ldi data, 0xb0 ;set page (POR=0)
 rcall write_command
 ldi data, 0x02 ;lower column address (+2)
 rcall write_command
 ldi data, 0x10 ;higher column address
 rcall write_command
 ldi data, 0xa1 ;set segment re-map (1=reverse)
 rcall write_command
 ldi data, 0xc8 ;set COM output scan direction
 rcall write_command
 ldi data, 0xda ;set common pads hw configuration
 rcall write_command
 ldi data, 0x12 ;alternate (POR=0x12)
 rcall write_command
 ldi data, 0x81 ;set contrast control mode
 rcall write_command
 ldi data, 0x80 ;(POR=0x80)
 rcall write_command
 ldi data, 0xd9 ;set pre-charge period mode
 rcall write_command
 ldi data, 0x02 ;(POR=0x02)
 rcall write_command
 ldi data, 0xdb ;set VCOMH de-select level
 rcall write_command
 ldi data, 0x35 ;(POR=0x35)
 rcall write_command
 ldi data, 0xa4 ;output follows RAM content
 rcall write_command
 ldi data, 0xa6 ;set normal display
 rcall write_command
 ret

Next, wait at least 100 msec, clear display RAM and turn display on (0xaf)