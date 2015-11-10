;+--------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : LEVEL_BAR.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : LEVEL_BAR.ASM
;|
;| Destiny Image Properties
;|     Size   :    63 Bytes
;|     Width  :    51 Pixels
;|     Height :     9 Pixels
;|     Width  :     7 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                                        
;               1         2         3         4         5
;     012345678901234567890123456789012345678901234567890
;     |         |         |         |         |         |
;   0-#                                                 #     -   0
;   1- #################################################      -   1
;   2- ######################## ########################      -   2
;   3- #### ######### ######### ######### ######### ####      -   3
;   4- #### #### #### #### #### #### #### #### #### ####      -   4
;   5- #### ######### ######### ######### ######### ####      -   5
;   6- ######################## ########################      -   6
;   7- #################################################      -   7
;   8-#                                                 #     -   8
;     |         |         |         |         |         |
;                                                        
;               1         2         3         4         5
;     012345678901234567890123456789012345678901234567890

;( IMAGE DATA )

BMP_LEVEL_BAR:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     51 ;Width Pixels
	.DW      9 ;Height Pixels
	.DW      7 ;Width Bytes
	.DW     63 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x20,0x80,0x00
	.DB 0x00,0x40,0x00,0x00,0x20,0x84,0x01,0x00,0x40,0x10,0x04,0x20,0x84,0x21,0x08,0x42
	.DB 0x10,0x84,0x20,0x84,0x01,0x00,0x40,0x10,0x04,0x20,0x80,0x00,0x00,0x40,0x00,0x00
	.DB 0x20,0x80,0x00,0x00,0x00,0x00,0x00,0x20,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0x00

