
_tbl10_G2:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G2:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0
_193:
	.DB  0x80,0xC0
	.DEF __lcd_x=R5
	.DEF __lcd_y=R4
	.DEF __lcd_maxx=R7
_202:
	.DW  0x00
_0:
	.DB  0x72,0x65,0x63,0x65,0x69,0x76,0x65,0x20
	.DB  0x3A,0x20,0x0,0x74,0x72,0x61,0x6E,0x73
	.DB  0x6D,0x69,0x74,0x20,0x3A,0x20,0x33,0x38
	.DB  0x0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G4
	.DW  _193*2

	.DW  0x02
	.DW  _p_S55
	.DW  _202*2

	.DW  0
_0xD1: