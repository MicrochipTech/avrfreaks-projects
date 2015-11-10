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
;|     File : TIME_XY_NTSC.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : TIME_XY_NTSC.ASM
;|
;| Destiny Image Properties
;|     Size   :    80 Bytes
;|     Width  :   128 Pixels
;|     Height :     5 Pixels
;|     Width  :    16 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                                                                                         1                           
;               1         2         3         4         5         6         7         8         9         0         1         2       
;     01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567
;     |         |         |         |         |         |         |         |         |         |         |         |         |       
;   0-###################   #   #############################   #   ############################# ##   ###############################-   0
;   1-#### ########### #### # # ############## ########### #### # # ############# ########### ##  ## # ############# ################ -   1
;   2-#### ##########   ##  # # ############## ##########   #   # # ############# ##########   ## ## # ############# ################ -   2
;   3-#### ########### #### # # ############## ########### ## ### # ############# ########### ### ## # ############# ################ -   3
;   4-###################   #   #############################   #   ############################   #   ###############################-   4
;     |         |         |         |         |         |         |         |         |         |         |         |         |       
;                                                                                                         1                           
;               1         2         3         4         5         6         7         8         9         0         1         2       
;     01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567

;( IMAGE DATA )

BMP_TIME_XY_NTSC:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW    128 ;Width Pixels
	.DW      5 ;Height Pixels
	.DW     16 ;Width Bytes
	.DW     80 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x1D,0xC0,0x00,0x00,0x01,0xDC,0x00,0x00,0x00,0x13,0x80,0x00,0x00,0x00
	.DB 0x08,0x00,0x85,0x40,0x00,0x80,0x08,0x54,0x00,0x10,0x01,0x32,0x80,0x02,0x00,0x01
	.DB 0x08,0x01,0xCD,0x40,0x00,0x80,0x1D,0xD4,0x00,0x10,0x03,0x92,0x80,0x02,0x00,0x01
	.DB 0x08,0x00,0x85,0x40,0x00,0x80,0x09,0x14,0x00,0x10,0x01,0x12,0x80,0x02,0x00,0x01
	.DB 0x00,0x00,0x1D,0xC0,0x00,0x00,0x01,0xDC,0x00,0x00,0x00,0x3B,0x80,0x00,0x00,0x00

