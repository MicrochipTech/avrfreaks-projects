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
;|     File : CLOCK_SET_1.BMP
;|
;| Destiny
;|     Path : C:\NEPER_THE_MATH\PROJETOS\MOTION_DETECTOR\VERSIONS\_FREE_VERSION_\PROGRAMS\AVR\BMPS\
;|     File : CLOCK_SET_1.ASM
;|
;| Destiny Image Properties
;|     Size   :   255 Bytes
;|     Width  :    39 Pixels
;|     Height :    51 Pixels
;|     Width  :     5 Bytes
;|     Colors :     2 Levels (1 bit)
;+--------------------------------------------------------------------------------------------

;( IMAGE CONTENTS )

;                                            
;               1         2         3        
;     012345678901234567890123456789012345678
;     |         |         |         |        
;   0-  ###################################   -   0
;   1- #                                   #  -   1
;   2-#                #  ###               # -   2
;   3-#               ##    #               # -   3
;   4-#      #   #     #  ###    #          # -   4
;   5-#     ##  ##     #  #     ##          # -   5
;   6-#      #   #    ### ###    #          # -   6
;   7-#      #   #               #          # -   7
;   8-#     ### ###      #      ###         # -   8
;   9-#                                     # -   9
;  10-#  #  ###                      ###    # -  10
;  11-# ##  # #                        #    # -  11
;  12-#  #  # #                      ###    # -  12
;  13-#  #  # #                      #      # -  13
;  14-# ### ###                      ###    # -  14
;  15-#                                     # -  15
;  16-#                                     # -  16
;  17-#  ###                           ###  # -  17
;  18-#  # #            ###              #  # -  18
;  19-#  ### #          ###          #  ##  # -  19
;  20-#    #            ###              #  # -  20
;  21-#    #                           ###  # -  21
;  22-#                                     # -  22
;  23-#                                     # -  23
;  24-#    ###                       # #    # -  24
;  25-#    # #                       # #    # -  25
;  26-#    ###                       ###    # -  26
;  27-#    # #                         #    # -  27
;  28-#    ###                         #    # -  28
;  29-#                                     # -  29
;  30-#         ###      #      ###         # -  30
;  31-#           #             #           # -  31
;  32-#           #     #       ###         # -  32
;  33-#           #     #         #         # -  33
;  34-#           #     ###     ###         # -  34
;  35-#                 # #                 # -  35
;  36-#                 ###                 # -  36
;  37-#                                     # -  37
;  38-#     ###########################     # -  38
;  39-#    #                           #    # -  39
;  40-#    #                           #    # -  40
;  41-#    #                           #    # -  41
;  42-#    #             #             #    # -  42
;  43- #   #                           #   #  -  43
;  44-  ####             #             ####   -  44
;  45-     #                           #      -  45
;  46-     #                           #      -  46
;  47-     #                           #      -  47
;  48-     #                           #      -  48
;  49-     #                           #      -  49
;  50-      ###########################       -  50
;     |         |         |         |        
;                                            
;               1         2         3        
;     012345678901234567890123456789012345678

;( IMAGE DATA )

BMP_CLOCK_SET_1:

	;+------------+
	;| BMP Header |
	;+------------+
	.DW      1 ;Bits Color 1 bit
	.DW     39 ;Width Pixels
	.DW     51 ;Height Pixels
	.DW      5 ;Width Bytes
	.DW    255 ;Size Bytes
	;+------------+
	;|  BMP Data  |
	;+------------+
	.DB 0xC0,0x00,0x00,0x00,0x06,0xBF,0xFF,0xFF,0xFF,0xFA,0x7F,0xFF,0xB1,0xFF,0xFC,0x7F
	.DB 0xFF,0x3D,0xFF,0xFC,0x7E,0xEF,0xB1,0xEF,0xFC,0x7C,0xCF,0xB7,0xCF,0xFC,0x7E,0xEF
	.DB 0x11,0xEF,0xFC,0x7E,0xEF,0xFF,0xEF,0xFC,0x7C,0x47,0xEF,0xC7,0xFC,0x7F,0xFF,0xFF
	.DB 0xFF,0xFC,0x6C,0x7F,0xFF,0xFE,0x3C,0x4D,0x7F,0xFF,0xFF,0xBC,0x6D,0x7F,0xFF,0xFE
	.DB 0x3C,0x6D,0x7F,0xFF,0xFE,0xFC,0x44,0x7F,0xFF,0xFE,0x3C,0x7F,0xFF,0xFF,0xFF,0xFC
	.DB 0x7F,0xFF,0xFF,0xFF,0xFC,0x63,0xFF,0xFF,0xFF,0x8C,0x6B,0xFF,0xC7,0xFF,0xEC,0x62
	.DB 0xFF,0xC7,0xFE,0xCC,0x7B,0xFF,0xC7,0xFF,0xEC,0x7B,0xFF,0xFF,0xFF,0x8C,0x7F,0xFF
	.DB 0xFF,0xFF,0xFC,0x7F,0xFF,0xFF,0xFF,0xFC,0x78,0xFF,0xFF,0xFE,0xBC,0x7A,0xFF,0xFF
	.DB 0xFE,0xBC,0x78,0xFF,0xFF,0xFE,0x3C,0x7A,0xFF,0xFF,0xFF,0xBC,0x78,0xFF,0xFF,0xFF
	.DB 0xBC,0x7F,0xFF,0xFF,0xFF,0xFC,0x7F,0xC7,0xEF,0xC7,0xFC,0x7F,0xF7,0xFF,0xDF,0xFC
	.DB 0x7F,0xF7,0xDF,0xC7,0xFC,0x7F,0xF7,0xDF,0xF7,0xFC,0x7F,0xF7,0xC7,0xC7,0xFC,0x7F
	.DB 0xFF,0xD7,0xFF,0xFC,0x7F,0xFF,0xC7,0xFF,0xFC,0x7F,0xFF,0xFF,0xFF,0xFC,0x7C,0x00
	.DB 0x00,0x00,0x7C,0x7B,0xFF,0xFF,0xFF,0xBC,0x7B,0xFF,0xFF,0xFF,0xBC,0x7B,0xFF,0xFF
	.DB 0xFF,0xBC,0x7B,0xFF,0xEF,0xFF,0xBC,0xBB,0xFF,0xFF,0xFF,0xBA,0xC3,0xFF,0xEF,0xFF
	.DB 0x86,0xFB,0xFF,0xFF,0xFF,0xBE,0xFB,0xFF,0xFF,0xFF,0xBE,0xFB,0xFF,0xFF,0xFF,0xBE
	.DB 0xFB,0xFF,0xFF,0xFF,0xBE,0xFB,0xFF,0xFF,0xFF,0xBE,0xFC,0x00,0x00,0x00,0x7E,0x00

