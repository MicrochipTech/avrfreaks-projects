;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|João D´Artagnan A. Oliveira Programmer and Author; 				  |
;|										  |
;| All rights reserved.								  |
;|										  |
;|   Redistribution and use in source and binary forms, with or without		  |
;|  modification, are permitted provided that the following conditions are met:	  |
;|										  |
;|   * Redistributions of source code must retain the above copyright		  |
;|     notice, this list of conditions and the following disclaimer.		  |
;|										  |
;|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
;|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
;|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
;|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     |
;|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  |
;|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	  |
;|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  |
;|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  |
;|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  |
;|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
;|   POSSIBILITY OF SUCH DAMAGE.						  |
;|										  |
;|   Brasília, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------------------------------------
;| Source
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : UFO_1_2.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : UFO_1_2.ASM
;|
;| Destiny Image Properties
;|     Size   :   153 Bytes
;|     Width  :    66 Pixels
;|     Height :    17 Pixels
;|     Width  :     9 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                                                       
;               1         2         3         4         5         6     
;     012345678901234567890123456789012345678901234567890123456789012345
;     |         |         |         |         |         |         |     
;   0-##################################################################      -   0
;   1-#######################                   ########################      -   1
;   2-#####################  ###################  ######################      -   2
;   3-#################### ####################### #####################      -   3
;   4-################### ###   #####   #####   ### ####################      -   4
;   5-################## ### ### ### ### ### ### ### ###################      -   5
;   6-################# #### ### ### ### ### ### #### ##################      -   6
;   7-######           ##### ### ### ### ### ### #####           #######      -   7
;   8-##### #################   #####   #####   ################# ######      -   8
;   9-#### ####################################################### #####      -   9
;  10-### ######################################################### ####      -  10
;  11-###  ######           ######           ######           ##### ####      -  11
;  12-###  ######           ######           ######           ##### ####      -  12
;  13-###  ######           ######           ######           ##### ####      -  13
;  14-### ######################################################### ####      -  14
;  15-### ######################################################### ####      -  15
;  16-###                                                           ####      -  16
;     |         |         |         |         |         |         |     
;                                                                       
;               1         2         3         4         5         6     
;     012345678901234567890123456789012345678901234567890123456789012345

;( IMAGE DATA )

BMP_UFO_1_2:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     66 ;Width Pixels
	.DW     17 ;Height Pixels
	.DW      9 ;Width Bytes
	.DW    153 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0xC0,0x00
	.DB 0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00
	.DB 0x08,0x00,0x00,0x00,0x00,0x00,0x11,0xC1,0xC1,0xC4,0x00,0x00,0x00,0x00,0x00,0x22
	.DB 0x22,0x22,0x22,0x00,0x00,0x00,0x00,0x00,0x42,0x22,0x22,0x21,0x00,0x00,0x00,0x03
	.DB 0xFF,0x82,0x22,0x22,0x20,0xFF,0xE0,0x00,0x04,0x00,0x01,0xC1,0xC1,0xC0,0x00,0x10
	.DB 0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x10,0x00,0x00,0x00,0x00,0x00
	.DB 0x00,0x04,0x00,0x18,0x1F,0xFC,0x0F,0xFE,0x07,0xFF,0x04,0x00,0x18,0x1F,0xFC,0x0F
	.DB 0xFE,0x07,0xFF,0x04,0x00,0x18,0x1F,0xFC,0x0F,0xFE,0x07,0xFF,0x04,0x00,0x10,0x00
	.DB 0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00
	.DB 0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x00,0x00

