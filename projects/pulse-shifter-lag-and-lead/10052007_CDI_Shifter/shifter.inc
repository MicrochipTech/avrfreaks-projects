
	.DEF _waktu1=R4
	.DEF _waktu2=R7
	.DEF _waktu3=R6
	.DEF _waktu4=R8
	.DEF _waktu5=R10
	.DEF _keadaan_pin_up=R13
	.DEF _keadaan_pin_down=R12
_data_acuan:
	.DB  0x0,0x0,0x9B,0x2,0xDE,0x0,0x85,0x0
	.DB  0x5F,0x0,0x4A,0x0,0x3D,0x0,0x33,0x0
	.DB  0x2C,0x0,0x27,0x0,0x23,0x0,0x20,0x0
	.DB  0x1D,0x0,0x1B,0x0,0x19,0x0,0x17,0x0
	.DB  0x16,0x0,0x14,0x0,0x13,0x0,0x12,0x0
	.DB  0x11,0x0,0x10,0x0,0x10,0x0,0xF,0x0
	.DB  0xE,0x0
_tbl10_G3:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G3:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0
_378:
	.DB  0x80,0xC0
_387:
	.DW  0x00
_0:
	.DB  0x74,0x64,0x20,0x20,0x20,0x20,0x20,0x20
	.DB  0x20,0x20,0x52,0x50,0x4D,0x0,0xA,0x25
	.DB  0x32,0x69,0x20,0x20,0x20,0x20,0x20,0x20
	.DB  0x20,0x20,0x25,0x35,0x69,0x0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G4
	.DW  _378*2

	.DW  0x02
	.DW  _p_S72
	.DW  _387*2

	.DW  0
_0x197:
