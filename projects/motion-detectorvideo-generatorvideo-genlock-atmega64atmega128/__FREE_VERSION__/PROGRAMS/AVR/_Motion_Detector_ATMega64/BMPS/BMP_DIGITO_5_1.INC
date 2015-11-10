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
;+--------------------------------------------------------------------------------------------------------
;| Source
;|     Path : D:\__NEPER_THE_MATH__\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : DIGITO_5_1.BMP
;|
;| Destiny
;|     Path : D:\__NEPER_THE_MATH__\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : BMP_DIGITO_5_1.INC
;|
;| Destiny Image Properties
;|     Size   :   128 Bytes
;|     Width  :    28 Pixels
;|     Height :    32 Pixels
;|     WidthB :     4 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                 
;               1         2       
;     0123456789012345678901234567
;     |         |         |       
;   0-############################    -   0
;   1-#####                   ####    -   1
;   2-##### ################# ####    -   2
;   3-##### ################# ####    -   3
;   4-##### ################# ####    -   4
;   5-##### ################# ####    -   5
;   6-####  ################# ####    -   6
;   7-#### #######            ####    -   7
;   8-#### ######  ###############    -   8
;   9-#### ###### ################    -   9
;  10-#### ######         ########    -  10
;  11-###  ##############   ######    -  11
;  12-### #################  #####    -  12
;  13-### ##################  ####    -  13
;  14-### ################### ####    -  14
;  15-### ######      #######  ###    -  15
;  16-###        ####  ####### ###    -  16
;  17-################ ####### ###    -  17
;  18-################ ####### ###    -  18
;  19-################ ####### ###    -  19
;  20-################ ####### ###    -  20
;  21-##         ##### ####### ###    -  21
;  22-## ####### ##### ####### ###    -  22
;  23-## #######  ###  ####### ###    -  23
;  24-##  #######     #######  ###    -  24
;  25-###  #################  ####    -  25
;  26-####  ###############  #####    -  26
;  27-#####  #############  ######    -  27
;  28-######   #########   #######    -  28
;  29-########           #########    -  29
;  30-############################    -  30
;  31-############################    -  31
;     |         |         |       
;                                 
;               1         2       
;     0123456789012345678901234567

;( IMAGE DATA )

BMP_DIGITO_5_1:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     28 ;Width Pixels
	.DW     32 ;Height Pixels
	.DW      4 ;Width Bytes
	.DW    128 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0x00,0x04,0x00,0x01,0x00,0x04,0x00,0x01,0x00
	.DB 0x04,0x00,0x01,0x00,0x04,0x00,0x01,0x00,0x0C,0x00,0x01,0x00,0x08,0x0F,0xFF,0x00
	.DB 0x08,0x18,0x00,0x00,0x08,0x10,0x00,0x00,0x08,0x1F,0xF0,0x00,0x18,0x00,0x1C,0x00
	.DB 0x10,0x00,0x06,0x00,0x10,0x00,0x03,0x00,0x10,0x00,0x01,0x00,0x10,0x3F,0x01,0x80
	.DB 0x1F,0xE1,0x80,0x80,0x00,0x00,0x80,0x80,0x00,0x00,0x80,0x80,0x00,0x00,0x80,0x80
	.DB 0x00,0x00,0x80,0x80,0x3F,0xE0,0x80,0x80,0x20,0x20,0x80,0x80,0x20,0x31,0x80,0x80
	.DB 0x30,0x1F,0x01,0x80,0x18,0x00,0x03,0x00,0x0C,0x00,0x06,0x00,0x06,0x00,0x0C,0x00
	.DB 0x03,0x80,0x38,0x00,0x00,0xFF,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

