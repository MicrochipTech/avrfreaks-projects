
_7:
	.DB  0x80,0xC0
	.DEF __lcd_x=R4
	.DEF __lcd_y=R5
	.DEF __lcd_maxx=R6

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G2
	.DW  _7*2

	.DW  0
_0x11:
