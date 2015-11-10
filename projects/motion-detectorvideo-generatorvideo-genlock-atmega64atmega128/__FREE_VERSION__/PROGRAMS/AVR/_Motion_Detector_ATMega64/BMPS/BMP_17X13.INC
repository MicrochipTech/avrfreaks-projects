;+--------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : 17X13.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : 17X13.ASM
;|
;| Destiny Image Properties
;|     Size   :    39 Bytes
;|     Width  :    17 Pixels
;|     Height :    13 Pixels
;|     Width  :     3 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                      
;               1      
;     01234567890123456
;     |         |      
;   0-                       -   0
;   1-####                   -   1
;   2-   ########            -   2
;   3-          ######       -   3
;   4-               #       -   4
;   5-                #      -   5
;   6-      ######    #      -   6
;   7- #   ##     #   #      -   7
;   8- #   #      ### #      -   8
;   9- #   ##       ###      -   9
;  10- #    #                -  10
;  11- ##   #                -  11
;  12-   ####                -  12
;     |         |      
;                      
;               1      
;     01234567890123456

;( IMAGE DATA )

BMP_17X13:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     17 ;Width Pixels
	.DW     13 ;Height Pixels
	.DW      3 ;Width Bytes
	.DW     39 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0xFF,0xFF,0x80,0x0F,0xFF,0x80,0xE0,0x1F,0x80,0xFF,0xC0,0x80,0xFF,0xFE,0x80,0xFF
	.DB 0xFF,0x00,0xFC,0x0F,0x00,0xB9,0xF7,0x00,0xBB,0xF1,0x00,0xB9,0xFC,0x00,0xBD,0xFF
	.DB 0x80,0x9D,0xFF,0x80,0xE1,0xFF,0x80,0x00

