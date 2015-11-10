;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : EXPLOSION.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_EXPLOSION.INC
;|
;| Destiny Image Properties
;|     Size   :    20 Bytes
;|     Width  :    10 Pixels
;|     Height :    10 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;               
;               
;     0123456789
;     |         
;   0-##########      -   0
;   1-##########      -   1
;   2-# # #### #      -   2
;   3-### # ####      -   3
;   4-# #####  #      -   4
;   5-##########      -   5
;   6-## # # ###      -   6
;   7-# ## ## ##      -   7
;   8-####### ##      -   8
;   9-##########      -   9
;     |         
;               
;               
;     0123456789

;( IMAGE DATA )

BMP_EXPLOSION:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     10 ;Width Pixels
	.DW     10 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     20 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x50,0x80,0x14,0x00,0x41,0x80,0x00,0x00,0x2A,0x00,0x49,0x00
	.DB 0x01,0x00,0x00,0x00

