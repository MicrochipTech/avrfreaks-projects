;+------------------------------------------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : EARTHINV_UFO_BARRIER.BMP
;|
;| Destiny
;|     Path : C:\____NEPER\___PROJETOS\_MOTION_DETECTOR\VERSIONS\__FREE_VERSION__\PROGRAMS\AVR\_MOTION_DETECTOR_ATMEGA64\BMPS\
;|     File : BMP_EARTHINV_UFO_BARRIER.INC
;|
;| Destiny Image Properties
;|     Size   :    18 Bytes
;|     Width  :    15 Pixels
;|     Height :     9 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels (1 bit)
;+------------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                    
;               1    
;     012345678901234
;     |         |    
;   0-############### -   0
;   1-####       #### -   1
;   2-##           ## -   2
;   3-#             # -   3
;   4-#             # -   4
;   5-#             # -   5
;   6-#    #####    # -   6
;   7-#   #######   # -   7
;   8-############### -   8
;     |         |    
;                    
;               1    
;     012345678901234

;( IMAGE DATA )

BMP_EARTHINV_UFO_BARRIER:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     15 ;Width Pixels
	.DW      9 ;Height Pixels
	.DW      2 ;Width Bytes
	.DW     18 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x0F,0xE0,0x3F,0xF8,0x7F,0xFC,0x7F,0xFC,0x7F,0xFC,0x78,0x3C,0x70,0x1C
	.DB 0x00,0x00

