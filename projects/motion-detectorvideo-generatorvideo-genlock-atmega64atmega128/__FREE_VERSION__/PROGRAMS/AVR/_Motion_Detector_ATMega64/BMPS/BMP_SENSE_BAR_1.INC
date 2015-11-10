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
;|     File : SENSE_BAR_1.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : SENSE_BAR_1.ASM
;|
;| Destiny Image Properties
;|     Size   :   324 Bytes
;|     Width  :    25 Pixels
;|     Height :    81 Pixels
;|     Width  :     4 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                              
;               1         2    
;     0123456789012345678901234
;     |         |         |    
;   0- #######################        -   0
;   1-#                       #       -   1
;   2-#                #####  #       -   2
;   3-#          #### #     # #       -   3
;   4-#               #     # #       -   4
;   5-#             # #     # #       -   5
;   6-#               #     # #       -   6
;   7-#             # #     # #       -   7
;   8-#               #     # #       -   8
;   9-#             # #     # #       -   9
;  10-#               #     # #       -  10
;  11-#             # #     # #       -  11
;  12-#               #     # #       -  12
;  13-#           ### #     # #       -  13
;  14-#               #     # #       -  14
;  15-#             # #     # #       -  15
;  16-#               #     # #       -  16
;  17-#             # #     # #       -  17
;  18-#               #     # #       -  18
;  19-#             # #     # #       -  19
;  20-#               #     # #       -  20
;  21-#             # #     # #       -  21
;  22-#               #     # #       -  22
;  23-#           ### #     # #       -  23
;  24-#               #     # #       -  24
;  25-#             # #     # #       -  25
;  26-#               #     # #       -  26
;  27-#             # #     # #       -  27
;  28-#               #     # #       -  28
;  29-#             # #     # #       -  29
;  30-#               #     # #       -  30
;  31-#             # #     # #       -  31
;  32-#               #     # #       -  32
;  33-#         ##### #     # #       -  33
;  34-#               #     # #       -  34
;  35-#             # #     # #       -  35
;  36-#               #     # #       -  36
;  37-#             # #     # #       -  37
;  38-#               #     # #       -  38
;  39-#             # #     # #       -  39
;  40-#               #     # #       -  40
;  41-#             # #     # #       -  41
;  42-#               #     # #       -  42
;  43-#           ### #     # #       -  43
;  44-#               #     # #       -  44
;  45-#             # #     # #       -  45
;  46-#               #     # #       -  46
;  47-#             # #     # #       -  47
;  48-#               #     # #       -  48
;  49-#             # #     # #       -  49
;  50-#               #     # #       -  50
;  51-#             # #     # #       -  51
;  52-#               #     # #       -  52
;  53-#           ### #     # #       -  53
;  54-#               #     # #       -  54
;  55-#             # #     # #       -  55
;  56-#               #     # #       -  56
;  57-#             # #     # #       -  57
;  58-#               #     # #       -  58
;  59-#             # #     # #       -  59
;  60-#               #     # #       -  60
;  61-#             # #     # #       -  61
;  62-#               #     # #       -  62
;  63-#          #### #     # #       -  63
;  64-#                #####  #       -  64
;  65-#                       #       -  65
;  66-#########################       -  66
;  67-#                       #       -  67
;  68-#  ###################  #       -  68
;  69-# #                   # #       -  69
;  70-# #  ###   ###   ###  # #       -  70
;  71-# # #   # #   # #   # # #       -  71
;  72-# # #  ## #  ## #  ## # #       -  72
;  73-# # # # # # # # # # # # #       -  73
;  74-# # ##  # ##  # ##  # # #       -  74
;  75-# # #   # #   # #   # # #       -  75
;  76-# #  ###   ###   ###  # #       -  76
;  77-# #                   # #       -  77
;  78-#  ###################  #       -  78
;  79-#                       #       -  79
;  80- #######################        -  80
;     |         |         |    
;                              
;               1         2    
;     0123456789012345678901234

;( IMAGE DATA )

BMP_SENSE_BAR_1:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     25 ;Width Pixels
	.DW     81 ;Height Pixels
	.DW      4 ;Width Bytes
	.DW    324 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0x80,0x00,0x00,0x80,0x7F,0xFF,0xFF,0x00,0x7F,0xFF,0x83,0x00,0x7F,0xE1,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xF1,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xF1,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xC1,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xF1,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xF1,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00
	.DB 0x7F,0xFF,0x7D,0x00,0x7F,0xFD,0x7D,0x00,0x7F,0xFF,0x7D,0x00,0x7F,0xE1,0x7D,0x00
	.DB 0x7F,0xFF,0x83,0x00,0x7F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0x00
	.DB 0x60,0x00,0x03,0x00,0x5F,0xFF,0xFD,0x00,0x58,0xE3,0x8D,0x00,0x57,0x5D,0x75,0x00
	.DB 0x56,0x59,0x65,0x00,0x55,0x55,0x55,0x00,0x53,0x4D,0x35,0x00,0x57,0x5D,0x75,0x00
	.DB 0x58,0xE3,0x8D,0x00,0x5F,0xFF,0xFD,0x00,0x60,0x00,0x03,0x00,0x7F,0xFF,0xFF,0x00
	.DB 0x80,0x00,0x00,0x80

