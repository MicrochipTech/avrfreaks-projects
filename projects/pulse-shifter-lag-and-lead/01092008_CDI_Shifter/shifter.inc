
	.DEF _waktu1=R4
	.DEF _waktu2=R7
	.DEF _waktu3=R6
	.DEF _waktu4=R8
	.DEF _waktu5=R10
	.DEF _keadaan_pin_up=R13
	.DEF _keadaan_pin_down=R12
_data_acuan_us:
	.DB  0x0,0x0,0x1,0x0,0x3,0x0,0x5,0x0
	.DB  0xA,0x0,0x19,0x0,0x32,0x0,0x64,0x0
	.DB  0x7D,0x0,0xFA,0x0
_tbl10_G3:
	.DB  0x10,0x27,0xE8,0x3,0x64,0x0,0xA,0x0
	.DB  0x1,0x0
_tbl16_G3:
	.DB  0x0,0x10,0x0,0x1,0x10,0x0,0x1,0x0
_318:
	.DB  0x80,0xC0
_327:
	.DW  0x00
_0:
	.DB  0x74,0x64,0x20,0x20,0x64,0x20,0x20,0x20
	.DB  0x20,0x20,0x52,0x50,0x4D,0x0,0xA,0x25
	.DB  0x32,0x69,0x20,0x25,0x33,0x69,0x20,0x20
	.DB  0x20,0x25,0x35,0x69,0x0

__GLOBAL_INI_TBL:
	.DW  0x02
	.DW  __base_y_G4
	.DW  _318*2

	.DW  0x02
	.DW  _p_S74
	.DW  _327*2

	.DW  0
_0x15F:
