;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : PROJECTILE.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_PROJECTILE.INC
;|
;| Destiny Image Properties
;|     Size   :     5 Bytes
;|     Width  :     1 Pixels
;|     Height :     5 Pixels
;|     WidthB :     1 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;      
;      
;     0
;     |
;   0-        -   0
;   1-        -   1
;   2-        -   2
;   3-#       -   3
;   4-#       -   4
;     |
;      
;      
;     0

;( IMAGE DATA )

BMP_PROJECTILE:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW      1 ;Width Pixels
	.DW      5 ;Height Pixels
	.DW      1 ;Width Bytes
	.DW      5 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x80,0x80,0x80,0x00,0x00,0x00

