	.DEF _oldADCpin=R4
	.DEF _ADCpin=R6
	.DEF _ADCtemp=R8
	.DEF _xyzsampledflag=R10
	.DEF _avgOrgflag=R11
	.DEF _result=R12
_tbl10_G2:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G2:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0
_124:
	.DB  0x80,0xC0
	.DEF __lcd_x=R13
	.DEF __lcd_y=R14
_0:
	.DB  0xD,0xA,0x4F,0x72,0x69,0x67,0x69,0x6E
	.DB  0x20,0x5B,0x25,0x64,0x2C,0x25,0x64,0x2C
	.DB  0x25,0x64,0x5D,0x0,0x9,0x53,0x70,0x65
	.DB  0x65,0x64,0x20,0x5B,0x25,0x64,0x2C,0x25
	.DB  0x64,0x2C,0x25,0x64,0x5D,0x0,0xD,0xA
	.DB  0x5B,0x25,0x64,0x2C,0x25,0x64,0x2C,0x25
	.DB  0x64,0x5D,0x0,0x4D,0x4D,0x41,0x37,0x32
	.DB  0x36,0x30,0x20,0x54,0x45,0x53,0x54,0x0
	.DB  0xA,0x49,0x6E,0x69,0x74,0x69,0x61,0x6C
	.DB  0x69,0x7A,0x65,0x64,0xA,0x0,0x46,0x52
	.DB  0x4F,0x4E,0x54,0x0,0x4C,0x45,0x46,0x54
	.DB  0x20,0x0,0x42,0x41,0x43,0x4B,0x20,0x0
	.DB  0x52,0x49,0x47,0x48,0x54,0x0,0x53,0x54
	.DB  0x4F,0x50,0x20,0x0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G4
	.DW  _124*2

	.DW  0
