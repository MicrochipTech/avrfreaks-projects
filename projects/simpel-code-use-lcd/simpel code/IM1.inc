_22:
	.DB  0x31,0x30,0x30,0x2E,0x35,0x20,0x49,0x6D
	.DB  0x70,0x61,0x63,0x74,0x20,0x46,0x6D,0xA
	.DB  0x4D,0x6F,0x72,0x65,0x20,0x54,0x68,0x61
	.DB  0x6E,0x20,0x57,0x69,0x6E,0x65,0x72,0x0
___base_y_G2:
	.DW  __base_y_G2
	.DW  0x2
	.DB  0x80,0xC0
	.DEF __lcd_x=R4
	.DEF __lcd_y=R5
	.DEF __lcd_maxx=R6
_tbl10_G3:
	.DW  0x2710,0x3E8,0x64,0xA,0x1
_tbl16_G3:
	.DW  0x1000,0x100,0x10,0x1

__GLOBAL_INI_TBL:
	.DW  ___base_y_G2*2
	.DW  0
