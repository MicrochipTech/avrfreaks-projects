;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : EARTHINV_UFO_3.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_EARTHINV_UFO_3.INC
;|
;| Destiny Image Properties
;|     Size   :    18 Bytes
;|     Width  :     9 Pixels
;|     Height :     9 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;              
;              
;     012345678
;     |        
;   0-#########       -   0
;   1-#########       -   1
;   2-###   ###       -   2
;   3-##     ##       -   3
;   4-# ## ## #       -   4
;   5-#       #       -   5
;   6-# #   # #       -   6
;   7-## ### ##       -   7
;   8-#########       -   8
;     |        
;              
;              
;     012345678

;( IMAGE DATA )

BMP_EARTHINV_UFO_3:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW      9 ;Width Pixels
	.DW      9 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     18 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x1C,0x00,0x3E,0x00,0x49,0x00,0x7F,0x00,0x5D,0x00,0x22,0x00
	.DB 0x00,0x00

