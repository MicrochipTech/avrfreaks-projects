
	.DEF _ComaCounter=R5
	.DEF _SCount=R4
_32:
	.DB  0x80,0xC0
	.DEF __lcd_x=R7
	.DEF __lcd_y=R6
	.DEF __lcd_maxx=R9
_0:
	.DB  0x52,0x61,0x6D,0x69,0x6E,0x2E,0x20,0x0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G2
	.DW  _32*2

	.DW  0
_0x2A:
