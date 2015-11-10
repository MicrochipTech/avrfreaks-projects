;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : EARTHINV_UFO_MOTHER.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_EARTHINV_UFO_MOTHER.INC
;|
;| Destiny Image Properties
;|     Size   :    12 Bytes
;|     Width  :    13 Pixels
;|     Height :     6 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                  
;               1  
;     0123456789012
;     |         |  
;   0-#############   -   0
;   1-####     ####   -   1
;   2-###  # #  ###   -   2
;   3-#           #   -   3
;   4-##    #    ##   -   4
;   5-#############   -   5
;     |         |  
;                  
;               1  
;     0123456789012

;( IMAGE DATA )

BMP_EARTHINV_UFO_MOTHER:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     13 ;Width Pixels
	.DW      6 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     12 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x0F,0x80,0x1A,0xC0,0x7F,0xF0,0x3D,0xE0,0x00,0x00

