;+--------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : OLHO_2.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : OLHO_2.ASM
;|
;| Destiny Image Properties
;|     Size   :    16 Bytes
;|     Width  :    16 Pixels
;|     Height :     8 Pixels
;|     Width  :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                     
;               1     
;     0123456789012345
;     |         |     
;   0-################-   0
;   1-################-   1
;   2-##          ####-   2
;   3-# ##############-   3
;   4-###          ###-   4
;   5-################-   5
;   6-################-   6
;   7-################-   7
;     |         |     
;                     
;               1     
;     0123456789012345

;( IMAGE DATA )

BMP_OLHO_2:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     16 ;Width Pixels
	.DW      8 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     16 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x3F,0xF0,0x40,0x00,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00

