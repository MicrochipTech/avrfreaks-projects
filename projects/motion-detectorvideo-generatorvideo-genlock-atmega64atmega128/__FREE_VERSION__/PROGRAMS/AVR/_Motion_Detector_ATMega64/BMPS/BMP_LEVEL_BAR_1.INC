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
;|     File : LEVEL_BAR_1.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : LEVEL_BAR_1.ASM
;|
;| Destiny Image Properties
;|     Size   :    63 Bytes
;|     Width  :    51 Pixels
;|     Height :     9 Pixels
;|     Width  :     7 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                                        
;               1         2         3         4         5
;     012345678901234567890123456789012345678901234567890
;     |         |         |         |         |         |
;   0-#                                                 #     -   0
;   1- #################################################      -   1
;   2- ######################## ########################      -   2
;   3- #### ######### ######### ######### ######### ####      -   3
;   4- #### #### #### #### #### #### #### #### #### ####      -   4
;   5- #### ######### ######### ######### ######### ####      -   5
;   6- ######################## ########################      -   6
;   7- #################################################      -   7
;   8-#                                                 #     -   8
;     |         |         |         |         |         |
;                                                        
;               1         2         3         4         5
;     012345678901234567890123456789012345678901234567890

;( IMAGE DATA )

BMP_LEVEL_BAR_1:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     51 ;Width Pixels
	.DW      9 ;Height Pixels
	.DW      7 ;Width Bytes
	.DW     63 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x20,0x80,0x00
	.DB 0x00,0x40,0x00,0x00,0x20,0x84,0x01,0x00,0x40,0x10,0x04,0x20,0x84,0x21,0x08,0x42
	.DB 0x10,0x84,0x20,0x84,0x01,0x00,0x40,0x10,0x04,0x20,0x80,0x00,0x00,0x40,0x00,0x00
	.DB 0x20,0x80,0x00,0x00,0x00,0x00,0x00,0x20,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0,0x00

