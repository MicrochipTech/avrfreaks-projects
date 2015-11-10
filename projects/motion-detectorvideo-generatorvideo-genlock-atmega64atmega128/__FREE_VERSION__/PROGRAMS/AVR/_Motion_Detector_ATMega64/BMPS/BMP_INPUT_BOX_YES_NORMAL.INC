;+--------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : INPUT_BOX_YES_NORMAL.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\DSQUARE\PROGRAMS\AVR\BMPS\
;|     File : INPUT_BOX_YES_NORMAL.ASM
;|
;| Destiny Image Properties
;|     Size   :     5 Bytes
;|     Width  :     7 Pixels
;|     Height :     5 Pixels
;|     Width  :     1 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;            
;            
;     0123456
;     |      
;   0-### ### -   0
;   1-##   ## -   1
;   2-#     # -   2
;   3-### ### -   3
;   4-### ### -   4
;     |      
;            
;            
;     0123456

;( IMAGE DATA )

BMP_INPUT_BOX_YES_NORMAL:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW      7 ;Width Pixels
	.DW      5 ;Height Pixels
	.DW      1 ;Width Bytes
	.DW      5 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x10,0x38,0x7C,0x10,0x10,0x00

