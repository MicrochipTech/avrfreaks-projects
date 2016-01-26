/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date 07/02/2016
*@brief <b>(Include)</b> File with BitMap of Earth Invaders Game UFO S1
*@copyright GNU Public License.
******************************************************************************************
*//*
;+------------------------------------------------------------------------------------------------------------------------
;| Generate C,C++ File for Atmel AVR Studio 4..7
;|
;| Source
;|     Path : C:\____NEPER\__FERRAMENTAS\__AVR__\__CPP_LIBRAY_2015\_DISPLAY\_T6963C\_T6963C_Drive_V141\_Resources\
;|     File : EARTHINV_UFO_S_1.bmp
;|
;| Destiny
;|     Path : C:\____NEPER\__FERRAMENTAS\__AVR__\__CPP_LIBRAY_2015\_DISPLAY\_T6963C\_T6963C_Drive_V141\_Resources\
;|     File : BMP_EARTHINV_UFO_S_1.INC
;|
;| Destiny Image Properties
;|     Size   :    24 Bytes
;|     Width  :    12 Pixels
;|     Height :    12 Pixels
;|     WidthB :     2 Bytes
;|     Colors :     2 Levels(1) Bit
;|
;| Define a below typedef in C,C++ code to accept BitMap resource
;|
;|	typedef const uint8_t BitMap;
;|
;+------------------------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                 
;               1 
;     012345678901
;     |         | 
;0000-################-   0
;0001-################-   1
;0002-####    ########-   2
;0003-###      #######-   3
;0004-##        ######-   4
;0005-##  ## ## ######-   5
;0006-##  ## ## ######-   6
;0007-#          #####-   7
;0008-#          #####-   8
;0009-### #### #######-   9
;0010-### #### #######-  10
;0011-####  ###  #####-  11
;     |         | 
;                 
;               1 
;     012345678901
*/

BitMap BMP_EARTHINV_UFO_S_1[] PROGMEM =
{
	//+------------+
	//| BMP Header |
	//+------------+
	0x42,0x4D,	// BitMap Header ASCII(B)+ASCII(M)
	0x01,0x00,	//     1 Bits Deep
	0x0C,0x00,	//    12 Width in Pixels
	0x0C,0x00,	//    12 Height in Pixels
	0x02,0x00,	//     2 Width in Bytes
	0x18,0x00,	//    24 Size in Bytes
	//+------------+
	//|  BMP Data  |
	//+------------+
	0x00,0x0F,0x00,0x0F,0x0F,0x0F,0x1F,0x8F,0x3F,0xCF,0x32,0x4F,0x32,0x4F,0x7F,0xEF,	//7
	0x7F,0xEF,0x10,0x8F,0x10,0x8F,0x0C,0x6F
};
