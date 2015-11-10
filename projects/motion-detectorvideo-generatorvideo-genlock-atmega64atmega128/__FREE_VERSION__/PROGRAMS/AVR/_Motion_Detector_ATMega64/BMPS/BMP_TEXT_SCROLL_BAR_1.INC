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
;|     File : TEXT_SCROLL_BAR_1.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : TEXT_SCROLL_BAR_1.ASM
;|
;| Destiny Image Properties
;|     Size   :    52 Bytes
;|     Width  :     8 Pixels
;|     Height :    52 Pixels
;|     Width  :     1 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;             
;             
;     01234567
;     |       
;   0-#       -   0
;   1-#   #   -   1
;   2-#  # #  -   2
;   3-# #   # -   3
;   4-# ##### -   4
;   5-#       -   5
;   6-########-   6
;   7-#       -   7
;   8-#       -   8
;   9-#       -   9
;  10-#       -  10
;  11-#       -  11
;  12-#       -  12
;  13-#       -  13
;  14-#       -  14
;  15-#       -  15
;  16-#       -  16
;  17-#       -  17
;  18-#       -  18
;  19-#       -  19
;  20-#       -  20
;  21-#       -  21
;  22-#       -  22
;  23-#       -  23
;  24-#       -  24
;  25-#       -  25
;  26-#       -  26
;  27-#       -  27
;  28-#       -  28
;  29-#       -  29
;  30-#       -  30
;  31-#       -  31
;  32-#       -  32
;  33-#       -  33
;  34-#       -  34
;  35-#       -  35
;  36-#       -  36
;  37-#       -  37
;  38-#       -  38
;  39-#       -  39
;  40-#       -  40
;  41-#       -  41
;  42-#       -  42
;  43-#       -  43
;  44-#       -  44
;  45-########-  45
;  46-#       -  46
;  47-# ##### -  47
;  48-# #   # -  48
;  49-#  # #  -  49
;  50-#   #   -  50
;  51-#       -  51
;     |       
;             
;             
;     01234567

;( IMAGE DATA )

BMP_TEXT_SCROLL_BAR_1:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW      8 ;Width Pixels
	.DW     52 ;Height Pixels
	.DW      1 ;Width Bytes
	.DW     52 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x7F,0x77,0x6B,0x5D,0x41,0x7F,0x00,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F
	.DB 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F
	.DB 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x00,0x7F,0x41
	.DB 0x5D,0x6B,0x77,0x7F

