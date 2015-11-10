;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : EARTHINV_UFO_CANNON.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_EARTHINV_UFO_CANNON.INC
;|
;| Destiny Image Properties
;|     Size   :    14 Bytes
;|     Width  :     9 Pixels
;|     Height :     7 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;              
;              
;     012345678
;     |        
;   0-#########       -   0
;   1-#### ####       -   1
;   2-#### ####       -   2
;   3-###   ###       -   3
;   4-#       #       -   4
;   5-#       #       -   5
;   6-#########       -   6
;     |        
;              
;              
;     012345678

;( IMAGE DATA )

BMP_EARTHINV_UFO_CANNON:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW      9 ;Width Pixels
	.DW      7 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     14 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x08,0x00,0x08,0x00,0x1C,0x00,0x7F,0x00,0x7F,0x00,0x00,0x00

