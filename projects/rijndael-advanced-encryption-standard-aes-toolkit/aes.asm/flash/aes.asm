;---------------------------------------------------------------------------------------------------------------------
;
; Rijndael / Advanced Encryption Standard (AES) Toolkit for AVR 8 bit microprocessors.
;
;	FLASH maximized implementation.
;
;	##############################
;	# Author: 	Michael Blenderman
;	# Version: 	1.1	(FLASH maximized implementation)
;	# Date: 	5/1/2006
;	#
;	# Source: 	AVR Assembler 2.1
;	# Processor: AVR AT90CAN128
;	#
;	# Routines: (see the source code comments for more information)
;	# 	aes_b_encrypt	- Rijndael/AES Block Encrypt
;	# 	aes_b_decrypt	- Rijndael/AES Block Decrypt
;	# 	aes_key_expand	- Rijndael/AES Block Key Schedule expansion
;	##############################
;
;	Version 1.1 changes:
;		1) The style and documentation for clearing some temporary registers in aes_b_encrypt and aes_b_decrypt was improved.
;		2) The aes_shift and aes_invshift routines were improved to reduce the code size and slightly improve the execution speed.
;			The SRAM shiftmp: variable was removed from the aes_setup.inc file.
;
;	Feel free to use this code however you see fit, including commercial use. If you modify it please add your name above
;		as another author (preserve my name as the original author) and identify your newer version of the program. I wrote
;		this program from scratch using the Rijndael and AES specifications. I ran the AVRstudio editor on a large screen.
;		So, I will apologize in advance to those with smaller size monitor screens.
;
;	See the parent directory "readme.txt" file for more specific information.
;
;	This implementation of the Rijndael/AES toolkit uses up to 512 bytes for the SubByte Table (aes_sub_xy) and Inverse SubByte
;		Table (aes_invsub_xy). It is optimized for fast operation. This implementation is mainly a tradeoff between FLASH and
;		SRAM memory usage.
;
;	Only 16, 24 and 32 byte blocks sizes and 128, 192 and 256 bit keys are supported.
;		See the aes_setup.inc file for more information. This program will only encrypt or decrypt a single block at one time.
;		Cipher Block Chaining must be handled first before this program is called. AES only allows Nb = 4 (16 byte s_box[]
;		blocks) while Rijndael allows Nb = 4, 6, 8 and other sizes.
;
;	Both encryption and decryption are supported. Encryption has a conditional assembly flag (ENCRYPT). In addition there is
;		a KEY_GEN assembly flag for the key schedule expansion and a KEY_FLASH assembly flag for writing the expanded key
;		schedule into Boot loader FLASH (see aes_setup.inc for more information). The FLASH expanded key schedule is the only one
;		the program uses. It will require some minor rewriting if you want to use SRAM for the expanded key schedule.
;
;	There are two 16 bit SRAM variables (aes_sbox, aes_kexp) that must be setup before calling aes_b_encrypt or aes_b_decrypt.
;		aes_sbox is a pointer to the s_box[nbX4] array in SRAM
;		aes_kexp is a pointer to the expanded_key[(Nb * (Nr+1)) * 4] array in Boot loader FLASH (up to 480 bytes storage)
;		Except when using aes_key_expand, then:
;			aes_kexp is a pointer to the soon to be expanded_key[(Nb * (Nr+1)) * 4] array in SRAM. The unexpanded original key is
;			copied to the beginning of SRAM pointed to be aes_kexp.
;		The aes_kexp is a pointer that can be placed in SRAM for encryption or decryption (see the comment in aes_addroundkey).
;			This will require aes_key_expand to always be run first one time to create the SRAM expanded key schedule.
;
;	This routine is only setup for a boot loader in an AVR with 128k or more memory.
;		A rather cheap, but easy out just sets the RAMPZ register based on the address of the main block encode/decode. The
;			RAMPZ setting can be moved up into higher level code as long is its set correctly when any of this code is used.
;		Since this is security software, it is assumed that it will all reside in Boot loader FLASH with the same RAMPZ setting.
;
;	An effort was made to make this program run in the same number of CPU cycles each time, no matter what the values of the data
;		being processed actually are. This was done because side channel attacks are the most successful types of attacks on AES
;		encryption. Please be aware that measures like this are only the beginning of securing your programs. Obviously, I am not
;		eager to trust SRAM over FLASH for security, so I use FLASH instead whenever possible. I also erase several SRAM buffers
;		immediately after they are of no further use. You will of course do whatever you want, I'm just letting you know there is
;		some SRAM erase code present.
;
;	Many of the comments in this code are based on published Rijndael and AES specifications. These are available on the Internet.
;
;-------------------------------------------------------------------------------------------------------------
.equ p_r12 = 0x0C	; general purpose register r12 pointer value
;-------------------------------------------------------------------------------------------------------------
;
.def zero    = r2	;              zero value constant
.def tmp     = r16	;              temporary variable storage
.def round   = r25	; <<=global=>> round counter (used throughout all modules)
					; SREG
;
;---------------------------------------------------------------------------------------------------------------------
;
; Rijndael Block Encrypt
;
;	The aes_sbox pointer must point at the plain text input block in SRAM (Nb size)
;	The aes_kexp pointer must point at an already expanded key schedule in Boot loader FLASH (((Nb * (Nr + 1)) * 4) size)
;
;	Each block is nbX4 bytes as set by Nb (see aes_setup.inc). Fill the memory pointed to through aes_sbox (s_box[Nb]) with
;		a Nb size block of "plain text" (it can actually be binary or whatever). After aes_b_encrypt runs successfully, this
;		memory block (s_box[Nb]) will contain the encrypted data.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
;	The SRAM variable aes_kexp must point at the FLASH k_ex[] location and must be stored before calling this routine
;
;##########################################
.if defined ENCRYPT
;
;
;
aes_b_encrypt:				; <<<<<<<<=========== Program entry point
;
;
;
		; set RAMPZ to match this routine address
	ldi	tmp, byte3(aes_b_encrypt * 2)
	OutReg RAMPZ, tmp				; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	clr zero
	mov round, zero			; reset round value to zero
;
	rcall aes_addroundkey	; round zero
;
aes_bround:
		inc round			; increment the round value
		cpi round, Nr		; is this the next to last round ?
	breq aes_bfin			; => this is the last round
;
		rcall aes_sub_bytes
		rcall aes_shift
		rcall aes_mix_col
		rcall aes_addroundkey
	rjmp aes_bround
;
aes_bfin:					; round Nr
	rcall aes_sub_bytes
	rcall aes_shift
	rcall aes_addroundkey
;
		; clear the temporary storage register values (leave no tracks)
		;	clears shiftrows, invshiftrows, mixcolumns, invmixcolumns  temporary storage values
	clr r8					; r_row0 and tmp_c7
	clr r9					; r_row1 and tmp_c6
	clr r10					; r_row2 and tmp_c5
	clr r11					; r_row3 and tmp_c4
		;	clears shiftrows, invshiftrows, mixcolumns, invmixcolumns and keygen temporary storage values
	clr r12					; i_row0, tmp_c3 and tmp0
	clr r13					; i_row1, tmp_c2 and tmp1
	clr r14					; i_row2, tmp_c1 and tmp2
	clr r15					; i_row3, tmp_c0 and tmp3
;
	OutReg RAMPZ, zero		; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	ret
;
.endif	;ENCRYPT
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Block Decrypt
;
;	The aes_sbox pointer must point at the encrypted input block in SRAM (Nb size)
;	The aes_kexp pointer must point at an already expanded key schedule in Boot loader FLASH (((Nb * (Nr + 1)) * 4) size)
;
;	Each block is nbX4 bytes as set by Nb (see aes_setup.inc). Fill the memory pointed to through aes_sbox (s_box[Nb]) with
;		a Nb size block of encrypted data. After aes_b_encrypt runs successfully, this
;		memory block (s_box[Nb]) will contain the "plain text" (it can actually be binary or whatever).
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
;	The SRAM variable aes_kexp must point at the FLASH k_ex[] location and must be stored before calling this routine
;
;
;
aes_b_decrypt:				; <<<<<<<<=========== Program entry point
;
;
;
		; set RAMPZ to match this routine address
	ldi	tmp, byte3(aes_b_decrypt * 2)
	OutReg RAMPZ, tmp				; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	ldi round, Nr			; initialize the total number of rounds in the round value
;
	rcall aes_addroundkey	; round Nr
;
aes_invbround:
		dec round			; decrement the round value
		cpi round, 0		; is this the next to last round ?
	breq aes_invbfin		; => this is the last round
;
		rcall aes_invshift
		rcall aes_invsub_bytes
		rcall aes_addroundkey
		rcall aes_invmix_col
	rjmp aes_invbround
;
aes_invbfin:				; round 0
	rcall aes_invshift
	rcall aes_invsub_bytes
	rcall aes_addroundkey
;
		; clear the temporary storage register values (leave no tracks)
		;	clears shiftrows, invshiftrows, mixcolumns, invmixcolumns  temporary storage values
	clr r8					; r_row0 and tmp_c7
	clr r9					; r_row1 and tmp_c6
	clr r10					; r_row2 and tmp_c5
	clr r11					; r_row3 and tmp_c4
		;	clears shiftrows, invshiftrows, mixcolumns, invmixcolumns and keygen temporary storage values
	clr r12					; i_row0, tmp_c3 and tmp0
	clr r13					; i_row1, tmp_c2 and tmp1
	clr r14					; i_row2, tmp_c1 and tmp2
	clr r15					; i_row3, tmp_c0 and tmp3
;
	OutReg RAMPZ, zero		; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	ret
;
;-------------------------------------------------------------------------------------------------------------
;
.undef zero			; r2               zero value constant
.undef tmp			; r16              temporary variable storage
;
.def tmp_c7  = r8	;              temporary intermediate storage for column 7 byte of s_box[row, 7]
.def tmp_c6  = r9	;              temporary intermediate storage for column 6 byte of s_box[row, 6]
.def tmp_c5  = r10	;              temporary intermediate storage for column 5 byte of s_box[row, 5]
.def tmp_c4  = r11	;              temporary intermediate storage for column 4 byte of s_box[row, 4]
.def tmp_c3  = r12	;              temporary intermediate storage for column 3 byte of s_box[row, 3]
.def tmp_c2  = r13	;              temporary intermediate storage for column 2 byte of s_box[row, 2]
.def tmp_c1  = r14	;              temporary intermediate storage for column 1 byte of s_box[row, 1]
.def tmp_c0  = r15	;              temporary intermediate storage for column 0 byte of s_box[row, 0]
					; YH:YL        s_box[] SRAM pointer
					; SREG
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael ShiftRows Transform
;
; 	Each element of a row is addressed by its column (s_box [row, column]).
;
;	When Nb == 4, the row 2 barrel shift algorithm is its own inverse
;	When Nb == 6 or Nb == 8, the row 3 barrel shift algorithm is its own inverse
;		In order to save program code, the encrypt aes_shift jumps to the equivalent routine in the decrypt aes_invshift
;
;	This program is assembled for either 16, 24 or 32 byte s_box[] blocks.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
;##########################################
.if defined ENCRYPT
;
aes_shift:
	lds YL, aes_sbox			; SRAM s_box [r, c] state table pointer
	lds YH, aes_sbox + 1
;
.if Nb == 4				; small 4 * 4 s_box [16]
;------------------------------------------------------------------
;
; row 1 - small Barrel shift 1 <<
	ldd tmp_c0, Y + (1 + (4 * 0))			; get - row 1 column 0
	ldd tmp_c1, Y + (1 + (4 * 1))			; get - row 1 column 1
	ldd tmp_c2, Y + (1 + (4 * 2))			; get - row 1 column 2
	ldd tmp_c3, Y + (1 + (4 * 3))			; get - row 1 column 3
;
	std Y + 		(1 + (4 * 0)), tmp_c1	; save c1 to - row 1 column 0
	std Y + 		(1 + (4 * 1)), tmp_c2	; save c2 to - row 1 column 1
	std Y + 		(1 + (4 * 2)), tmp_c3	; save c3 to - row 1 column 2
	std Y + 		(1 + (4 * 3)), tmp_c0	; save c0 to - row 1 column 3
;
; row 3 - small Barrel shift 3 <<
	ldd tmp_c0, Y + (3 + (4 * 0))			; get - row 3 column 0
	ldd tmp_c1, Y + (3 + (4 * 1))			; get - row 3 column 1
	ldd tmp_c2, Y + (3 + (4 * 2))			; get - row 3 column 2
	ldd tmp_c3, Y + (3 + (4 * 3))			; get - row 3 column 3
;
	std Y + 		(3 + (4 * 0)), tmp_c3	; save c3 to - row 3 column 0
	std Y + 		(3 + (4 * 1)), tmp_c0	; save c0 to - row 3 column 1
	std Y + 		(3 + (4 * 2)), tmp_c1	; save c1 to - row 3 column 2
	std Y + 		(3 + (4 * 3)), tmp_c2	; save c2 to - row 3 column 3
;
; row 2 - small Barrel shift 2 <<
;	ldd tmp_c0, Y + (2 + (4 * 0))			; get - row 2 column 0
;	ldd tmp_c1, Y + (2 + (4 * 1))			; get - row 2 column 1
;	ldd tmp_c2, Y + (2 + (4 * 2))			; get - row 2 column 2
;	ldd tmp_c3, Y + (2 + (4 * 3))			; get - row 2 column 3
;
;	std Y + 		(2 + (4 * 0)), tmp_c2	; save c2 to - row 2 column 0
;	std Y + 		(2 + (4 * 1)), tmp_c3	; save c3 to - row 2 column 1
;	std Y + 		(2 + (4 * 2)), tmp_c0	; save c0 to - row 2 column 2
;	std Y + 		(2 + (4 * 3)), tmp_c1	; save c1 to - row 2 column 3
	rjmp shift4_row2						; save memory by reusing aes_invshift code
;
.elif Nb == 6			; medium 4 * 6 s_box [24]
;------------------------------------------------------------------
;
; row 1 - med Barrel shift 1 <<
	ldd tmp_c0, Y + (1 + (4 * 0))			; get - row 1 column 0
	ldd tmp_c1, Y + (1 + (4 * 1))			; get - row 1 column 1
	ldd tmp_c2, Y + (1 + (4 * 2))			; get - row 1 column 2
	ldd tmp_c3, Y + (1 + (4 * 3))			; get - row 1 column 3
	ldd tmp_c4, Y + (1 + (4 * 4))			; get - row 1 column 4
	ldd tmp_c5, Y + (1 + (4 * 5))			; get - row 1 column 5
;
	std Y + 		(1 + (4 * 0)), tmp_c1	; save c1 to - row 1 column 0
	std Y + 		(1 + (4 * 1)), tmp_c2	; save c2 to - row 1 column 1
	std Y + 		(1 + (4 * 2)), tmp_c3	; save c3 to - row 1 column 2
	std Y + 		(1 + (4 * 3)), tmp_c4	; save c4 to - row 1 column 3
	std Y + 		(1 + (4 * 4)), tmp_c5	; save c5 to - row 1 column 4
	std Y + 		(1 + (4 * 5)), tmp_c0	; save c0 to - row 1 column 5
;
; row 2 - med Barrel shift 2 <<
	ldd tmp_c0, Y + (2 + (4 * 0))			; get - row 2 column 0
	ldd tmp_c1, Y + (2 + (4 * 1))			; get - row 2 column 1
	ldd tmp_c2, Y + (2 + (4 * 2))			; get - row 2 column 2
	ldd tmp_c3, Y + (2 + (4 * 3))			; get - row 2 column 3
	ldd tmp_c4, Y + (2 + (4 * 4))			; get - row 2 column 4
	ldd tmp_c5, Y + (2 + (4 * 5))			; get - row 2 column 5
;
	std Y + 		(2 + (4 * 0)), tmp_c2	; save c2 to - row 2 column 0
	std Y + 		(2 + (4 * 1)), tmp_c3	; save c3 to - row 2 column 1
	std Y + 		(2 + (4 * 2)), tmp_c4	; save c4 to - row 2 column 2
	std Y + 		(2 + (4 * 3)), tmp_c5	; save c5 to - row 2 column 3
	std Y + 		(2 + (4 * 4)), tmp_c0	; save c0 to - row 2 column 4
	std Y + 		(2 + (4 * 5)), tmp_c1	; save c1 to - row 2 column 5
;
; row 3 - med Barrel shift 3 <<
;	ldd tmp_c0, Y + (3 + (4 * 0))			; get - row 3 column 0
;	ldd tmp_c1, Y + (3 + (4 * 1))			; get - row 3 column 1
;	ldd tmp_c2, Y + (3 + (4 * 2))			; get - row 3 column 2
;	ldd tmp_c3, Y + (3 + (4 * 3))			; get - row 3 column 3
;	ldd tmp_c4, Y + (3 + (4 * 4))			; get - row 3 column 4
;	ldd tmp_c5, Y + (3 + (4 * 5))			; get - row 3 column 5
;
;	std Y + 		(3 + (4 * 0)), tmp_c3	; save c3 to - row 3 column 0
;	std Y + 		(3 + (4 * 1)), tmp_c4	; save c4 to - row 3 column 1
;	std Y + 		(3 + (4 * 2)), tmp_c5	; save c5 to - row 3 column 2
;	std Y + 		(3 + (4 * 3)), tmp_c0	; save c0 to - row 3 column 3
;	std Y + 		(3 + (4 * 4)), tmp_c1	; save c1 to - row 3 column 4
;	std Y + 		(3 + (4 * 5)), tmp_c2	; save c2 to - row 3 column 5
	rjmp shift6_row3						; save memory by reusing aes_invshift code
;
.elif Nb == 8			; large 4 * 8 s_box [32]
;------------------------------------------------------------------
;
; row 1 - large Barrel shift 1 <<
	ldd tmp_c0, Y + (1 + (4 * 0))			; get - row 1 column 0
	ldd tmp_c1, Y + (1 + (4 * 1))			; get - row 1 column 1
	ldd tmp_c2, Y + (1 + (4 * 2))			; get - row 1 column 2
	ldd tmp_c3, Y + (1 + (4 * 3))			; get - row 1 column 3
	ldd tmp_c4, Y + (1 + (4 * 4))			; get - row 1 column 4
	ldd tmp_c5, Y + (1 + (4 * 5))			; get - row 1 column 5
	ldd tmp_c6, Y + (1 + (4 * 6))			; get - row 1 column 6
	ldd tmp_c7, Y + (1 + (4 * 7))			; get - row 1 column 7
;
	std Y + 		(1 + (4 * 0)), tmp_c1	; save c1 to - row 1 column 0
	std Y + 		(1 + (4 * 1)), tmp_c2	; save c2 to - row 1 column 1
	std Y + 		(1 + (4 * 2)), tmp_c3	; save c3 to - row 1 column 2
	std Y + 		(1 + (4 * 3)), tmp_c4	; save c4 to - row 1 column 3
	std Y + 		(1 + (4 * 4)), tmp_c5	; save c5 to - row 1 column 4
	std Y + 		(1 + (4 * 5)), tmp_c6	; save c0 to - row 1 column 5
	std Y + 		(1 + (4 * 6)), tmp_c7	; save c0 to - row 1 column 6
	std Y + 		(1 + (4 * 7)), tmp_c0	; save c0 to - row 1 column 7
;
; row 2 - large Barrel shift 3 <<
	ldd tmp_c0, Y + (2 + (4 * 0))			; get - row 2 column 0
	ldd tmp_c1, Y + (2 + (4 * 1))			; get - row 2 column 1
	ldd tmp_c2, Y + (2 + (4 * 2))			; get - row 2 column 2
	ldd tmp_c3, Y + (2 + (4 * 3))			; get - row 2 column 3
	ldd tmp_c4, Y + (2 + (4 * 4))			; get - row 2 column 4
	ldd tmp_c5, Y + (2 + (4 * 5))			; get - row 2 column 5
	ldd tmp_c6, Y + (2 + (4 * 6))			; get - row 2 column 6
	ldd tmp_c7, Y + (2 + (4 * 7))			; get - row 2 column 7
;
	std Y + 		(2 + (4 * 0)), tmp_c3	; save c3 to - row 2 column 0
	std Y + 		(2 + (4 * 1)), tmp_c4	; save c4 to - row 2 column 1
	std Y + 		(2 + (4 * 2)), tmp_c5	; save c5 to - row 2 column 2
	std Y + 		(2 + (4 * 3)), tmp_c6	; save c6 to - row 2 column 3
	std Y + 		(2 + (4 * 4)), tmp_c7	; save c7 to - row 2 column 4
	std Y + 		(2 + (4 * 5)), tmp_c0	; save c0 to - row 2 column 5
	std Y + 		(2 + (4 * 6)), tmp_c1	; save c1 to - row 2 column 6
	std Y + 		(2 + (4 * 7)), tmp_c2	; save c2 to - row 2 column 7
;
; row 3 - large Barrel shift 4 <<
;	ldd tmp_c0, Y + (3 + (4 * 0))			; get - row 3 column 0
;	ldd tmp_c1, Y + (3 + (4 * 1))			; get - row 3 column 1
;	ldd tmp_c2, Y + (3 + (4 * 2))			; get - row 3 column 2
;	ldd tmp_c3, Y + (3 + (4 * 3))			; get - row 3 column 3
;	ldd tmp_c4, Y + (3 + (4 * 4))			; get - row 3 column 4
;	ldd tmp_c5, Y + (3 + (4 * 5))			; get - row 3 column 5
;	ldd tmp_c6, Y + (3 + (4 * 6))			; get - row 3 column 6
;	ldd tmp_c7, Y + (3 + (4 * 7))			; get - row 3 column 7
;
;	std Y + 		(3 + (4 * 0)), tmp_c4	; save c4 to - row 3 column 0
;	std Y + 		(3 + (4 * 1)), tmp_c5	; save c5 to - row 3 column 1
;	std Y + 		(3 + (4 * 2)), tmp_c6	; save c6 to - row 3 column 2
;	std Y + 		(3 + (4 * 3)), tmp_c7	; save c7 to - row 3 column 3
;	std Y + 		(3 + (4 * 4)), tmp_c0	; save c0 to - row 3 column 4
;	std Y + 		(3 + (4 * 5)), tmp_c1	; save c1 to - row 3 column 5
;	std Y + 		(3 + (4 * 6)), tmp_c2	; save c2 to - row 3 column 6
;	std Y + 		(3 + (4 * 7)), tmp_c3	; save c3 to - row 3 column 7
	rjmp shift8_row3						; save memory by reusing aes_invshift code
;
.else
	.error "Nb is not a valid, its value must be 4, 6 or 8"
.endif
;	ret										; the rjmp to aes_invshift now bypasses this ret instruction
.endif	;ENCRYPT
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Inverse ShiftRows Transform
;
; 	Each element of a row is addressed by its column (s_box [row, column]).
;
;	When Nb == 4, the row 2 barrel shift algorithm is its own inverse
;	When Nb == 6 or Nb == 8, the row 3 barrel shift algorithm is its own inverse
;		In order to save program code, the encrypt aes_shift jumps to the equivalent routine in the decrypt aes_invshift
;
;	This program is assembled for either 16, 24 or 32 byte s_box[] blocks.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
aes_invshift:
	lds YL, aes_sbox			; SRAM s_box [r, c] state table pointer
	lds YH, aes_sbox + 1
;
.if Nb == 4			; small 4 * 4 s_box [16]
;------------------------------------------------------------------
;
; row 1 - invsmall Barrel shift 1 >>
	ldd tmp_c0, Y + (1 + (4 * 0))			; get - row 1 column 0
	ldd tmp_c1, Y + (1 + (4 * 1))			; get - row 1 column 1
	ldd tmp_c2, Y + (1 + (4 * 2))			; get - row 1 column 2
	ldd tmp_c3, Y + (1 + (4 * 3))			; get - row 1 column 3
;
	std Y + 		(1 + (4 * 0)), tmp_c3	; save c3 to - row 1 column 0
	std Y + 		(1 + (4 * 1)), tmp_c0	; save c0 to - row 1 column 1
	std Y + 		(1 + (4 * 2)), tmp_c1	; save c1 to - row 1 column 2
	std Y + 		(1 + (4 * 3)), tmp_c2	; save c2 to - row 1 column 3
;
; row 3 - invsmall Barrel shift 3 >>
	ldd tmp_c0, Y + (3 + (4 * 0))			; get - row 3 column 0
	ldd tmp_c1, Y + (3 + (4 * 1))			; get - row 3 column 1
	ldd tmp_c2, Y + (3 + (4 * 2))			; get - row 3 column 2
	ldd tmp_c3, Y + (3 + (4 * 3))			; get - row 3 column 3
;
	std Y + 		(3 + (4 * 0)), tmp_c1	; save c1 to - row 3 column 0
	std Y + 		(3 + (4 * 1)), tmp_c2	; save c2 to - row 3 column 1
	std Y + 		(3 + (4 * 2)), tmp_c3	; save c3 to - row 3 column 2
	std Y + 		(3 + (4 * 3)), tmp_c0	; save c4 to - row 3 column 3
;
shift4_row2:								; aes_shift program memory saving entry point (uses the ret from this routine)
; row 2 - invsmall Barrel shift 2 >>
	ldd tmp_c0, Y + (2 + (4 * 0))			; get - row 2 column 0
	ldd tmp_c1, Y + (2 + (4 * 1))			; get - row 2 column 1
	ldd tmp_c2, Y + (2 + (4 * 2))			; get - row 2 column 2
	ldd tmp_c3, Y + (2 + (4 * 3))			; get - row 2 column 3
;
	std Y + 		(2 + (4 * 0)), tmp_c2	; save c2 to - row 2 column 0
	std Y + 		(2 + (4 * 1)), tmp_c3	; save c3 to - row 2 column 1
	std Y + 		(2 + (4 * 2)), tmp_c0	; save c0 to - row 2 column 2
	std Y + 		(2 + (4 * 3)), tmp_c1	; save c1 to - row 2 column 3
;
.elif Nb == 6			; medium 4 * 6 s_box [24]
;------------------------------------------------------------------
;
; row 1 - invmed Barrel shift 1 >>
	ldd tmp_c0, Y + (1 + (4 * 0))			; get - row 1 column 0
	ldd tmp_c1, Y + (1 + (4 * 1))			; get - row 1 column 1
	ldd tmp_c2, Y + (1 + (4 * 2))			; get - row 1 column 2
	ldd tmp_c3, Y + (1 + (4 * 3))			; get - row 1 column 3
	ldd tmp_c4, Y + (1 + (4 * 4))			; get - row 1 column 4
	ldd tmp_c5, Y + (1 + (4 * 5))			; get - row 1 column 5
;
	std Y + 		(1 + (4 * 0)), tmp_c5	; save c5 to - row 1 column 0
	std Y + 		(1 + (4 * 1)), tmp_c0	; save c0 to - row 1 column 1
	std Y + 		(1 + (4 * 2)), tmp_c1	; save c1 to - row 1 column 2
	std Y + 		(1 + (4 * 3)), tmp_c2	; save c2 to - row 1 column 3
	std Y + 		(1 + (4 * 4)), tmp_c3	; save c3 to - row 1 column 4
	std Y + 		(1 + (4 * 5)), tmp_c4	; save c4 to - row 1 column 5
;
;
; row 2 - invmed Barrel shift 2 >>
	ldd tmp_c0, Y + (2 + (4 * 0))			; get - row 2 column 0
	ldd tmp_c1, Y + (2 + (4 * 1))			; get - row 2 column 1
	ldd tmp_c2, Y + (2 + (4 * 2))			; get - row 2 column 2
	ldd tmp_c3, Y + (2 + (4 * 3))			; get - row 2 column 3
	ldd tmp_c4, Y + (2 + (4 * 4))			; get - row 2 column 4
	ldd tmp_c5, Y + (2 + (4 * 5))			; get - row 2 column 5
;
	std Y + 		(2 + (4 * 0)), tmp_c4	; save c4 to - row 2 column 0
	std Y + 		(2 + (4 * 1)), tmp_c5	; save c5 to - row 2 column 1
	std Y + 		(2 + (4 * 2)), tmp_c0	; save c0 to - row 2 column 2
	std Y + 		(2 + (4 * 3)), tmp_c1	; save c1 to - row 2 column 3
	std Y + 		(2 + (4 * 4)), tmp_c2	; save c2 to - row 2 column 4
	std Y + 		(2 + (4 * 5)), tmp_c3	; save c3 to - row 2 column 5
;
;
shift6_row3:								; aes_shift program memory saving entry point (uses the ret from this routine)
; row 3 - invmed Barrel shift 3 >>
	ldd tmp_c0, Y + (3 + (4 * 0))			; get - row 3 column 0
	ldd tmp_c1, Y + (3 + (4 * 1))			; get - row 3 column 1
	ldd tmp_c2, Y + (3 + (4 * 2))			; get - row 3 column 2
	ldd tmp_c3, Y + (3 + (4 * 3))			; get - row 3 column 3
	ldd tmp_c4, Y + (3 + (4 * 4))			; get - row 3 column 4
	ldd tmp_c5, Y + (3 + (4 * 5))			; get - row 3 column 5
;
	std Y + 		(3 + (4 * 0)), tmp_c3	; save c3 to - row 3 column 0
	std Y + 		(3 + (4 * 1)), tmp_c4	; save c4 to - row 3 column 1
	std Y + 		(3 + (4 * 2)), tmp_c5	; save c5 to - row 3 column 2
	std Y + 		(3 + (4 * 3)), tmp_c0	; save c0 to - row 3 column 3
	std Y + 		(3 + (4 * 4)), tmp_c1	; save c1 to - row 3 column 4
	std Y + 		(3 + (4 * 5)), tmp_c2	; save c2 to - row 3 column 5
;
.elif Nb == 8			; large 4 * 8 s_box [32]
;------------------------------------------------------------------
;
; row 1 - invlarge Barrel shift 1 >>
	ldd tmp_c0, Y + (1 + (4 * 0))			; get - row 1 column 0
	ldd tmp_c1, Y + (1 + (4 * 1))			; get - row 1 column 1
	ldd tmp_c2, Y + (1 + (4 * 2))			; get - row 1 column 2
	ldd tmp_c3, Y + (1 + (4 * 3))			; get - row 1 column 3
	ldd tmp_c4, Y + (1 + (4 * 4))			; get - row 1 column 4
	ldd tmp_c5, Y + (1 + (4 * 5))			; get - row 1 column 5
	ldd tmp_c6, Y + (1 + (4 * 6))			; get - row 1 column 6
	ldd tmp_c7, Y + (1 + (4 * 7))			; get - row 1 column 7
;
	std Y + 		(1 + (4 * 0)), tmp_c7	; save c7 to - row 1 column 0
	std Y + 		(1 + (4 * 1)), tmp_c0	; save c0 to - row 1 column 1
	std Y + 		(1 + (4 * 2)), tmp_c1	; save c1 to - row 1 column 2
	std Y + 		(1 + (4 * 3)), tmp_c2	; save c2 to - row 1 column 3
	std Y + 		(1 + (4 * 4)), tmp_c3	; save c3 to - row 1 column 4
	std Y + 		(1 + (4 * 5)), tmp_c4	; save c4 to - row 1 column 5
	std Y + 		(1 + (4 * 6)), tmp_c5	; save c5 to - row 1 column 6
	std Y + 		(1 + (4 * 7)), tmp_c6	; save c6 to - row 1 column 7
;
; row 2 - invlarge Barrel shift 3 >>
	ldd tmp_c0, Y + (2 + (4 * 0))			; get - row 2 column 0
	ldd tmp_c1, Y + (2 + (4 * 1))			; get - row 2 column 1
	ldd tmp_c2, Y + (2 + (4 * 2))			; get - row 2 column 2
	ldd tmp_c3, Y + (2 + (4 * 3))			; get - row 2 column 3
	ldd tmp_c4, Y + (2 + (4 * 4))			; get - row 2 column 4
	ldd tmp_c5, Y + (2 + (4 * 5))			; get - row 2 column 5
	ldd tmp_c6, Y + (2 + (4 * 6))			; get - row 2 column 6
	ldd tmp_c7, Y + (2 + (4 * 7))			; get - row 2 column 7
;
	std Y + 		(2 + (4 * 0)), tmp_c5	; save c5 to - row 2 column 0
	std Y + 		(2 + (4 * 1)), tmp_c6	; save c6 to - row 2 column 1
	std Y + 		(2 + (4 * 2)), tmp_c7	; save c7 to - row 2 column 2
	std Y + 		(2 + (4 * 3)), tmp_c0	; save c0 to - row 2 column 3
	std Y + 		(2 + (4 * 4)), tmp_c1	; save c1 to - row 2 column 4
	std Y + 		(2 + (4 * 5)), tmp_c2	; save c2 to - row 2 column 5
	std Y + 		(2 + (4 * 6)), tmp_c3	; save c3 to - row 2 column 6
	std Y + 		(2 + (4 * 7)), tmp_c4	; save c4 to - row 2 column 7
;
shift8_row3:								; aes_shift program memory saving entry point (uses the ret from this routine)
; row 3 - invlarge Barrel shift 4 >>
	ldd tmp_c0, Y + (3 + (4 * 0))			; get - row 3 column 0
	ldd tmp_c1, Y + (3 + (4 * 1))			; get - row 3 column 1
	ldd tmp_c2, Y + (3 + (4 * 2))			; get - row 3 column 2
	ldd tmp_c3, Y + (3 + (4 * 3))			; get - row 3 column 3
	ldd tmp_c4, Y + (3 + (4 * 4))			; get - row 3 column 4
	ldd tmp_c5, Y + (3 + (4 * 5))			; get - row 3 column 5
	ldd tmp_c6, Y + (3 + (4 * 6))			; get - row 3 column 6
	ldd tmp_c7, Y + (3 + (4 * 7))			; get - row 3 column 7
;
	std Y + 		(3 + (4 * 0)), tmp_c4	; save c4 to - row 3 column 0
	std Y + 		(3 + (4 * 1)), tmp_c5	; save c5 to - row 3 column 1
	std Y + 		(3 + (4 * 2)), tmp_c6	; save c6 to - row 3 column 2
	std Y + 		(3 + (4 * 3)), tmp_c7	; save c7 to - row 3 column 3
	std Y + 		(3 + (4 * 4)), tmp_c0	; save c0 to - row 3 column 4
	std Y + 		(3 + (4 * 5)), tmp_c1	; save c1 to - row 3 column 5
	std Y + 		(3 + (4 * 6)), tmp_c2	; save c2 to - row 3 column 6
	std Y + 		(3 + (4 * 7)), tmp_c3	; save c3 to - row 3 column 7
;
.else
	.error "Nb is not a valid, its value must be 4, 6 or 8"
.endif
	ret
;
;-------------------------------------------------------------------------------------------------------------
.undef tmp_c7		; r8               temporary intermediate storage for column 7 byte of s_box[row, 7]
.undef tmp_c6		; r9               temporary intermediate storage for column 6 byte of s_box[row, 6]
.undef tmp_c5		; r10              temporary intermediate storage for column 5 byte of s_box[row, 5]
.undef tmp_c4		; r11              temporary intermediate storage for column 4 byte of s_box[row, 4]
.undef tmp_c3		; r12              temporary intermediate storage for column 3 byte of s_box[row, 3]
.undef tmp_c2		; r13              temporary intermediate storage for column 2 byte of s_box[row, 2]
.undef tmp_c1		; r14              temporary intermediate storage for column 1 byte of s_box[row, 1]
.undef tmp_c0		; r15              temporary intermediate storage for column 0 byte of s_box[row, 0]
;
.def zero    = r2	;              zero value constant
.def tmp_v   = r20	;              temporary value 
.def state_cnt= r21	;              state table s_box counter
.def ZLt     = r22	;              temporary ZL storage
.def ZHt     = r23	;              temporary ZH storage
.def tmp     = r24	;              temporary variable storage
					; XH:XL        s_box[] SRAM pointer
					; ZH:ZL        FLASH table pointer (aes_sub_xy and aes_invsub_xy)
					; SREG
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael SubBytes Transformation:
;
;##########################################
.if defined ENCRYPT || defined KEY_GEN
;
aes_sub_xy:
	; Y   0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	.db 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76	; X 0
	.db 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0	; X 1
	.db 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15	; X 2
	.db 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75	; X 3
	.db 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84	; X 4
	.db 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf	; X 5
	.db 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8	; X 6
	.db 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2	; X 7
	.db 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73	; X 8
	.db 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb	; X 9
	.db 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79	; X A
	.db 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08	; X B
	.db 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a	; X C
	.db 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e	; X D
	.db 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf	; X E
	.db 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16	; X F
;
.endif	; ENCRYPT || KEY_GEN
;##########################################
;
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Inverse SubBytes Transformation:
;
aes_invsub_xy:
	; Y   0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	.db 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb	; X 0
	.db 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb	; X 1
	.db 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e	; X 2
	.db 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25	; X 3
	.db 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92	; X 4
	.db 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84	; X 5
	.db 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06	; X 6
	.db 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b	; X 7
	.db 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73	; X 8
	.db 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e	; X 9
	.db 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b	; X A
	.db 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4	; X B
	.db 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f	; X C
	.db 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef	; X D
	.db 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61	; X E
	.db 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d	; X F
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael SubBytes Transform
;
;	This is a non-linear byte substitution for each individual state transformation.
;		Each value in the S box has this calculation applied to it.
;
;	This code is reused by aes_subword (part of aes_key_expand) using aes_sub_loop as its entry point. Be careful
;		if you modify this code as it could impact aes_key_expand operations.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
;##########################################
.if defined ENCRYPT || defined KEY_GEN
;
aes_sub_bytes:
	lds XL, aes_sbox				; SRAM s_box [r, c] state table pointer
	lds XH, aes_sbox + 1
	ldi ZLt, low(aes_sub_xy * 2)	; initialize base pointer to the SRAM SubBytes Transformation table
	ldi ZHt, high(aes_sub_xy * 2)
;
	clr zero
	ldi state_cnt, nbX4			; Nb count
;
aes_sub_loop:
		ld tmp_v, X						; get the s_box element value
	;
		movw ZH:ZL, ZHt:ZLt				; get the base pointer to the SRAM SubBytes Transformation table
		add ZL, tmp_v					; use tmp_v as the offset into the aes_sub_xy table
		adc ZH, zero
		elpm tmp_v, Z						; retrieve the S Box table result element
	;
		st X+, tmp_v					; save the sub_byte value back into the s_box and increment the s_box index
	;
		dec state_cnt					; decrement the s_box state table index count until it reaches zero
	brne  aes_sub_loop			; => if not zero, there are still mote state Sboxes to process
;
	ret
;
.endif	; ENCRYPT || KEY_GEN
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Inverse SubBytes Transform
;
;	This is a non-linear byte substitution for each individual state transformation.
;		Each value in the S box has this calculation applied to it.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
aes_invsub_bytes:
	lds XL, aes_sbox				; SRAM s_box [r, c] state table pointer
	lds XH, aes_sbox + 1
	ldi ZLt, low(aes_invsub_xy * 2)	; initialize base pointer to the SRAM Inverse SubBytes Transformation table
	ldi ZHt, high(aes_invsub_xy * 2)
;
	clr zero
	ldi state_cnt, nbX4			; Nb count
;
aes_invsub_loop:
		ld tmp_v, X						; get the s_box element value
	;
		movw ZH:ZL, ZHt:ZLt				; get the base pointer to the SRAM Inverse SubBytes Transformation table
		add ZL, tmp_v					; use tmp_v as the offset into the aes_invsub_xy table
		adc ZH, zero
		elpm tmp_v, Z					; retrieve the S Box table result element
	;
		st X+, tmp_v					; save the sub_byte value back into the s_box and increment the s_box index
	;
		dec state_cnt					; decrement the s_box state table index count until it reaches zero
	brne  aes_invsub_loop		; => if not zero, there are still mote state Sboxes to process
;
	ret
;
;-------------------------------------------------------------------------------------------------------------
.undef zero			; r2               zero value constant
.undef tmp_v		; r20              temporary value 
.undef state_cnt	; r21              state table s_box counter
.undef ZLt			; r22              temporary ZL storage
.undef ZHt			; r23              temporary ZH storage
.undef tmp			; r24              temporary variable storage
;
.def r_row0  = r8	;              temporary result storage for 1st (row) byte of s_box[] 32 bit word
.def r_row1  = r9	;              temporary result storage for 2nd (row) byte of s_box[] 32 bit word
.def r_row2  = r10	;              temporary result storage for 3rd (row) byte of s_box[] 32 bit word
.def r_row3  = r11	;              temporary result storage for 4th (row) byte of s_box[] 32 bit word
.def i_row0  = r12	;              temporary intermediate storage for 1st (row) byte of s_box[] 32 bit word
.def i_row1  = r13	;              temporary intermediate storage for 2nd (row) byte of s_box[] 32 bit word
.def i_row2  = r14	;              temporary intermediate storage for 3rd (row) byte of s_box[] 32 bit word
.def i_row3  = r15	;              temporary intermediate storage for 4th (row) byte of s_box[] 32 bit word
.def polly   = r17	;              Finite Field Multiplication polynomial (its actually 0x11B except the 0x100 is truncated anyway)
.def col_cnt = r22	;              state table s_box column counter
					; YH:YL        s_box[] SRAM pointer
					; SREG
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael MixColumns Transformation
;
; 	Each element of a column is addressed by its row (s_box [row, column]). Initially a copy is made of each row element that makes
;		up a column (i_rowN). The r_rowN registers are used as an accumulator for the new result value. The i_rowN registers are
;		used for intermediate calculations. Each column is processed as a complete 4 byte (32 bit word) entity. Then Nb number of
;		columns are processed.
;
;	A column is always 4 bytes long (a 32 bit word) no matter how many columns there are in the s_box [row, column] buffer.
;
;	The arcane looking operations below were derived by analyzing the Finite field Multiplication (FFMult) on the {02} and {03}
;		defined data elements (from the Rijndael/AES documentation). By using the multiplication by repeated shifts method to achieve
;		the GF(256) FFMult with the defined data elements, it breaks down to a very small number of operations for the program
;		code to carry out. It is an optimized version of the same operations found in the Rijndael/AES documentation.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
;##########################################
.if defined ENCRYPT
;
aes_mix_col:
;
	ldi polly, 0x1B				; the truncated special eighth order irreducible polynomial 1{1B}
	lds YL, aes_sbox			; SRAM s_box [r, c] state table pointer
	lds YH, aes_sbox + 1
	ldi col_cnt, Nb				; Nb count
;
aes_mc_loop:

		ld i_row0, Y				; get a complete copy of all four row elements that make up a column (s_box[rows, Nb - col_cnt])
		ldd i_row1, Y + 1			;	do not increment the row pointer (YH:YL) yet
		ldd i_row2, Y + 2
		ldd i_row3, Y + 3
	;
		clr r_row0					; initialize all four temporary row result accumulators
		clr r_row1
		clr r_row2
		clr r_row3
	;
		eor r_row0, i_row1			; process row0 (row0 < 0)
		eor r_row0, i_row2
		eor r_row0, i_row3
	;
		eor r_row1, i_row0			; process row1 (row1 < 0)
		eor r_row1, i_row2
		eor r_row1, i_row3
	;
		eor r_row2, i_row0			; process row2 (row2 < 0)
		eor r_row2, i_row1
		eor r_row2, i_row3
	;
		eor r_row3, i_row0			; process row3 (row3 < 0)
		eor r_row3, i_row1
		eor r_row3, i_row2
	;
		rcall aes_ffmult			; perform a limited FFMult operation on each row element of the column (rows < 1 FFMult)
	;
		eor r_row0, i_row0			; process row0 (row0 < 1)
		eor r_row0, i_row1
	;
		eor r_row1, i_row1			; process row1 (row1 < 1)
		eor r_row1, i_row2
	;
		eor r_row2, i_row2			; process row2 (row2 < 1)
		eor r_row2, i_row3
	;
		eor r_row3, i_row0			; process row3 (row3 < 1)
		eor r_row3, i_row3
	;
		st Y+, r_row0				; save the all the row elements of the column back to the s_box[rows, Nb - col_cnt] table
		st Y+, r_row1				;	and increment the row pointer (YH:YL) through all 4 rows to the next column
		st Y+, r_row2
		st Y+, r_row3
	;
		dec col_cnt					; decrement the s_box state table 32 bit word column count until it reaches zero
	brne  aes_mc_loop			; => if not zero, there are still more state Sbox elements to process
;
	ret
;
.endif	; ENCRYPT
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Inverse MixColumns Transformation
;
; 	Each element of a column is addressed by its row (s_box [row, column]). Initially a copy is made of each row element that makes
;		up a column (i_rowN). The r_rowN registers are used as an accumulator for the new result value. The i_rowN registers are
;		used for intermediate calculations. Each column is processed as a complete 4 byte (32 bit word) entity. Then Nb number of
;		columns are processed.
;
;	A column is always 4 bytes long (a 32 bit word) no matter how many columns there are in the s_box [row, column] buffer.
;
;	The arcane looking operations below were derived by analyzing the Finite field Multiplication (FFMult) on the {0E}, {0B}, {0D},
;		and {09} defined data elements (from the Rijndael/AES documentation). By using the multiplication by repeated shifts method
;		to achieve the GF(256) FFMult with the defined data elements, it breaks down to a very small number of operations for the
;		program code to carry out. It is an optimized version of the same operations found in the Rijndael/AES documentation.
;
;	The SRAM variable aes_sbox must point at the SRAM s_box[] location and must be stored before calling this routine
;
aes_invmix_col:
;
	ldi polly, 0x1B				; the truncated special eighth order irreducible polynomial 1{1B}
	lds YL, aes_sbox			; SRAM s_box [r, c] state table pointer
	lds YH, aes_sbox + 1
	ldi col_cnt, Nb				; Nb count
;
aes_invmc_loop:

		ld i_row0, Y				; get a complete copy of all four row elements that make up a column (s_box[rows, Nb - col_cnt])
		ldd i_row1, Y + 1			;	do not increment the row pointer (YH:YL) yet
		ldd i_row2, Y + 2
		ldd i_row3, Y + 3
	;
		clr r_row0					; initialize all four temporary row result accumulators
		clr r_row1
		clr r_row2
		clr r_row3
	;
		eor r_row0, i_row1			; process row0 (row0 < 0)
		eor r_row0, i_row2
		eor r_row0, i_row3
	;
		eor r_row1, i_row0			; process row1 (row1 < 0)
		eor r_row1, i_row2
		eor r_row1, i_row3
	;
		eor r_row2, i_row0			; process row2 (row2 < 0)
		eor r_row2, i_row1
		eor r_row2, i_row3
	;
		eor r_row3, i_row0			; process row3 (row3 < 0)
		eor r_row3, i_row1
		eor r_row3, i_row2
	;
		rcall aes_ffmult			; perform a limited FFMult on each row element of the column (rows < 1 FFMult)
	;
		eor r_row0, i_row0			; process row0 (row0 < 1)
		eor r_row0, i_row1
	;
		eor r_row1, i_row1			; process row1 (row1 < 1)
		eor r_row1, i_row2
	;
		eor r_row2, i_row2			; process row2 (row2 < 1)
		eor r_row2, i_row3
	;
		eor r_row3, i_row0			; process row3 (row3 < 1)
		eor r_row3, i_row3
									; up to this point it has all been the same as aes_mix_col
	;
		rcall aes_ffmult			; perform a limited FFMult on each row element of the column (rows < 2 FFMult)
	;
		eor r_row0, i_row0			; process row0 (row0 < 2)
		eor r_row0, i_row2
	;
		eor r_row1, i_row1			; process row1 (row1 < 2)
		eor r_row1, i_row3
	;
		eor r_row2, i_row0			; process row2 (row2 < 2)
		eor r_row2, i_row2
	;
		eor r_row3, i_row1			; process row3 (row3 < 2)
		eor r_row3, i_row3
	;
		rcall aes_ffmult			; perform a limited FFMult on each row element of the column (rows < 3 FFMult)
	;
		eor i_row0, i_row1			; the last operation is an exclusive or of i_row0 ^ i_row1 ^ i_row2 ^ i_row3
		eor i_row0, i_row2			;	so all four values are all exclusive ored into i_row0 first
		eor i_row0, i_row3			;	{0E}, {0B}, {0D} and {09} all have a 1 as the value for bit 3
	;
		eor r_row0, i_row0			; process row0, row1, row2 and row3 (rows < 3)
		eor r_row1, i_row0
		eor r_row2, i_row0
		eor r_row3, i_row0
	;
		st Y+, r_row0				; save the all the row elements of the column back to the s_box[rows, Nb - col_cnt] table
		st Y+, r_row1				;	and increment the row pointer (YH:YL) through all 4 rows to the next column
		st Y+, r_row2
		st Y+, r_row3
	;
		dec col_cnt					; decrement the s_box state table 32 bit word count until it reaches zero
	brne  aes_invmc_loop		; => if not zero, there are still more state Sbox elements to process
;
	ret
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Finite field Multiplication (FFMult)
;
; 	This is a specialized version of part of a Finite field Multiplication in GF(256) just for the aes_mix_col and
;		aes_invmix_col temporary buffers.
;
;	The polly value is provided by the calling routine so that it does not have to be initialized again on each call to aes_ffmult.
;
aes_ffmult:
;
	lsl i_row0					; i_row0 * 2 in GF(256)
	brcc aes_ffm1				; if there is a carry then....
		eor i_row0, polly		;	add the truncated special eighth order irreducible polynomial 1{1B}
aes_ffm1:
;
	lsl i_row1					; i_row1 * 2 in GF(256)
	brcc aes_ffm2				; if there is a carry then....
		eor i_row1, polly		;	add the truncated special eighth order irreducible polynomial 1{1B}
aes_ffm2:
;
	lsl i_row2					; i_row2 * 2 in GF(256)
	brcc aes_ffm3				; if there is a carry then....
		eor i_row2, polly		;	add the truncated special eighth order irreducible polynomial 1{1B}
aes_ffm3:
;
	lsl i_row3					; i_row3 * 2 in GF(256)
	brcc aes_ffm4				; if there is a carry then....
		eor i_row3, polly		;	add the truncated special eighth order irreducible polynomial 1{1B}
aes_ffm4:
;
	ret
;
;-------------------------------------------------------------------------------------------------------------
.undef r_row0		; r8               temporary result storage for 1st (row) byte of s_box[] 32 bit word
.undef r_row1		; r9               temporary result storage for 2nd (row) byte of s_box[] 32 bit word
.undef r_row2		; r10              temporary result storage for 3rd (row) byte of s_box[] 32 bit word
.undef r_row3		; r11              temporary result storage for 4th (row) byte of s_box[] 32 bit word
.undef i_row0		; r12              temporary intermediate storage for 1st (row) byte of s_box[] 32 bit word
.undef i_row1		; r13              temporary intermediate storage for 2nd (row) byte of s_box[] 32 bit word
.undef i_row2		; r14              temporary intermediate storage for 3rd (row) byte of s_box[] 32 bit word
.undef i_row3		; r15              temporary intermediate storage for 4th (row) byte of s_box[] 32 bit word
.undef polly		; r17              Finite Field Multiplication polynomial (its actually 0x11B except the 0x100 is truncated)
.undef col_cnt		; r22              state table s_box column counter
;
.def scnt    = r16	;              s_box[scnt]
.def state   = r18	;              s_box[scnt] element value byte
.def rkey    = r19	;              key schedule [scnt * round]
					; XH:XL        s_box[] SRAM pointer
					; ZH:ZL        k_exp[] SRAM pointer
					; SREG
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael AddRoundKey() Transform
;
;	AddRoundKey() is its own inverse.
;
;	The "elpm rkey, Z+" can be replaced with "ld rkey, Z+" and the aes_kexp pointer moved to point at SRAM for the
;		expanded key schedules. If you decide to use it this way, the aes_key_expand routine must have the initial key
;		preloaded and have already been run before any block encryption/decryption code to create the SRAM expanded
;		key schedule. In this case it would make sense to store the original key in FLASH and copy it to SRAM at the aes_kexp
;		location before calling aes_key_expand.
;
;	The SRAM variable aes_sbox must point a the SRAM s_box[] location and must be stored before calling this routine
;
;	The SRAM variable aes_kexp must point at the FLASH k_ex[] location and must be stored before calling this routine
;
aes_addroundkey:
;
	lds ZL, aes_kexp			; SRAM expanded key schedule pointer => w[i]
	lds ZH, aes_kexp + 1
;
	ldi scnt, nbX4				; bytes per round
	mul round, scnt				; k_ex offset = round * bytes per round
	add ZL, multl				; add in the k_ex offset to the k_ex pointer
	adc ZH, multh
;
	lds XL, aes_sbox			; SRAM s_box [r, c] state table pointer
	lds XH, aes_sbox + 1
sbox_loop:
		ld state, X					; get the s_box[i] element
		elpm rkey, Z+				; get the round key byte from the key schedule (increment the key schedule pointer)
		eor state, rkey				; exclusive or the above values together
		st X+, state				; save the result back into the s_box[i] as the new element (increment the i pointer)
		dec scnt					; update the s_box[] element count
	brne sbox_loop				; loop until done
;
	clr rkey					; leave no tracks
	ret
;
;-------------------------------------------------------------------------------------------------------------
;
.undef scnt			; r16              s_box[scnt]
.undef state		; r18              s_box[scnt] element value
.undef rkey			; r19              key schedule [scnt * round]
;
.def zero    = r2	;              zero value constant
.def tmp0    = r12	;              w[i-1] temp value, first byte	tmp0 to tmp3 is a 32 bit word
.def tmp1    = r13	;              w[i-1] temp value, second byte	tmp0 to tmp3 is a 32 bit word
.def tmp2    = r14	;              w[i-1] temp value, third byte	tmp0 to tmp3 is a 32 bit word
.def tmp3    = r15	;              w[i-1] temp value, fourth byte	tmp0 to tmp3 is a 32 bit word
.def cntl    = r16	;              low(i = 0: i < (nbX4 * (Nr+1). i++) count
.def cnth    = r17	;              high(i = 0: i < (nbX4 * (Nr+1). i++) count
.def tmp     = r18	;              temporary storage location
.if defined KEY_GEN
.def Rcon    = r19	;              The round constant word array byte
.endif
.if Nk == 6
.def mod6    = r20	;              MOD 6 counter
.endif
.def state_cnt= r21	;              aes_subword - state table s_box counter
.def ZLt     = r22	;              aes_subword - temporary ZL storage
.def ZHt     = r23	;              aes_subword - temporary ZH storage
					; YH:YL        aes_kexp SRAM pointer
					; SREG
;
;-------------------------------------------------------------------------------------------------------------
;
; Rijndael Key Expansion (EXAMPLE ROUTINE ONLY, its complete, but its interface is limited)
;
;	If KEY_FLASH is set this routine will write to flash every time its run. This could be detrimental to the life of your FLASH
;		in the long term (another reason this is only an example program).
;
;	The term word used below means a 4 byte / 32 bit value.
;
;	The entire key schedule is expanded at one time. It can take from 176 bytes to 480 bytes to hold a key schedule (240 bytes
;		will hold an AES 256 bit key). A fixed expanded key can be placed in the program FLASH where it gets the security
;		benefits of the FLASH memory lock bits. Placing the key or expanded key in SRAM may not be as secure?
;
;	The key expansion can also be broken up and generated on the fly for each nkX4 bytes as required by the round (this algorithm
;		does not do this). Since SRAM (including the general purpose registers) would have to be used to hold nkX4 sized parts of
;		the expanded key for the round, this may not be as secure as placing the entire expanded key schedule in FLASH.
;
;	MOD 4 and MOD 8 are easy to code. However, the mod6 counter is used for the Nk = 6 MOD 6 version coding
;
;	k_ex is the expanded key schedule (up to 480 bytes or 3840 bits)
;	The original 128, 192 or 256 bit key of Nk length is copied to the beginning of k_ex in SRAM. Nk must match the size of the
;		key to work correctly.
;
; 	The k_ex table is treated as a linear address space with implied [r, c] values.
;
;	The SRAM variable aes_kexp must point at the SRAM k_ex[] location and must be stored before calling this routine
;
;	A label "aes_key1" must be created in FLASH and have at least 1 FLASH page of empty FLASH after it. Whenever Nb is set
;		higher than 4, the key schedule may exceed 1 FLASH page and go up to 480 bytes. After the expended key schedule is
;		written to FLASH at aes_key1, you can read the AVR FLASH memory and manually extract the key schedule. The extracted
;		key schedule can be formatted as .DB data defines.
;
;	Nb = 4 and Nk = 4 will generate the smallest expanded key schedule of ((Nb * (Nr+1)) * 4) or ((4 * (10+1)) * 4) = 176 bytes
;		for a 128 bit key with a 16 byte s_box[]. Nb and Nk together determine the number of rounds (Nr), which accounts for the
;		otherwise invisible effect of Nk in the previous equation.
;
;##########################################
.if defined KEY_GEN
;
;
;
;
aes_key_expand:				; <<<<<<<<=========== Program entry point
;
;
;
		; set RAMPZ to match this routine address
	ldi	tmp, byte3(aes_key_expand * 2)
	OutReg RAMPZ, tmp				; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	clr zero
;
	ldi Rcon, 0x01				; round constant initial value
;
.if Nk == 6
	clr mod6					; MOD 6 counter
.endif
;
		; skip the key which occupies the beginning of SRAM at aes_kexp
	lds YL, aes_kexp			; SRAM w [i-Nk] pointer (this pointer lags [i] by Nk words or nkX4 bytes)
	lds YH, aes_kexp + 1		;	so, (nkX4 + byte position) must be added to the Y register for a byte element of [i]
	ldi cntl, Nk				; i = Nk (Nk = 8 and Nr = 14 makes cntl maximum value = 120 words)
;
aes_keyex_loop:
;
;-------------> while (i < Nb * (Nr+1)) or (i < (number of blocks * (number of rounds + 1)))
	cpi cntl, (Nb * (Nr+1))			; compare to see if the entire key schedule has been expanded
	brsh aes_keyex_done				; branch is we are done expanding the key schedule

	;-------------> temp = w[i-1] or (temp = previous 32 bit word from k_ex)
		ldd tmp0, Y + (nkX4 - 4)	; nkX4 = w[i] word, and the minus 4 selects the LSB first byte of w[i-1]
		ldd tmp1, Y + (nkX4 - 3)
		ldd tmp2, Y + (nkX4 - 2)
		ldd tmp3, Y + (nkX4 - 1)	; nkX4 = w[i] word, and the minus 1 selects the MSB last byte of w[i-1]
									; for reference, the w[i] word individual bytes would be (nkX4+0), (nkX4+1), (nkX4+2), (nkX4+3)
	;
	;-------------> if (i mod Nk == 0)
	.if Nk == 6
		tst mod6			; if (i mod 6 = 0)
		brne aes_elseif		; => branch if (i mod 6 != 0)
	.else
		mov tmp, cntl		; if (i mod Nk = 0)
		andi tmp, (Nk - 1)	; MOD 4 or MOD 8 depending on the Nk value
		brne aes_elseif		; => branch if (i mod Nk != 0)
	.endif
	;-------------> temp = SubWord(RotWord(temp)) xor Rcon[i/Nk]
		;
		;-------------> RotWord temp
			mov tmp, tmp0
			mov tmp0, tmp1
			mov tmp1, tmp2 
			mov tmp2, tmp3 
			mov tmp3, tmp 
		;
		;-------------> SubWord temp
			rcall aes_subword
		;
		;-------------> temp xor Rcon[i/Nk] or (w[i] = FFmult(MSB w[i])) ---- "Rcon[i/Nk]"?? is from the AES documentation
			eor tmp0, Rcon
			lsl Rcon				; multiply Rcon using the Finite field method
			brcc aes_endif
				ldi tmp, 0x1B			; apply the multiplication polynomial if Rcon overflows
				eor Rcon, tmp
			rjmp aes_endif
	;
aes_elseif:
	;
		;-------------> else if ((Nk > 6) and (i mod Nk = 4))
		.if Nk == 8
			mov tmp, cntl
			andi tmp, (Nk - 1)			; tmp = count low MOD 8
			cpi tmp, 0x4				; test for tmp = 4 
			brne aes_endif				; => and skip aes_subword if tmp != 4
			;
				;-------------> SubWord temp
				rcall aes_subword
		.endif
	;
aes_endif:
	;-------------> w[i] = w[i-Nk] xor (tmp0, tmp1, tmp2, tmp3)
		ld tmp, Y+					; w[i-Nk]	- increment [i-Nk] byte pointer
		eor tmp, tmp0				; xor tmp0  - first byte
		std Y + (nkX4 - 1), tmp		; = w[i]    - first byte of [i] word (the -1 neutralizes the effect of the Y+ two instructions ago)
		;
		ld tmp, Y+					; w[i-Nk]	- increment [i-Nk] byte pointer
		eor tmp, tmp1				; xor tmp 1 - second byte
		std Y + (nkX4 - 1), tmp		; = w[i]    - second byte of [i] word (the -1 neutralizes the effect of the Y+ two instructions ago)
		;
		ld tmp, Y+					; w[i-Nk]	- increment [i-Nk] byte pointer
		eor tmp, tmp2				; xor tmp2  - third byte
		std Y + (nkX4 - 1), tmp		; = w[i]    - third byte of [i] word (the -1 neutralizes the effect of the Y+ two instructions ago)
		;
		ld tmp, Y+					; w[i-Nk]	- increment [i-Nk] byte pointer, this one also increments [i-Nk] pointer to the next word
		eor tmp, tmp3				; xor tmp3  - fourth byte
		std Y + (nkX4 - 1), tmp		; = w[i]    - fourth byte of [i] word (the -1 neutralizes the effect of the Y+ two instructions ago)
		;
		;-------------> i = i + 1
		inc cntl				; increment the 8 bit key word counter [i] (cntl maximum value = 120 words)
;
.if Nk == 6
	inc mod6					; MOD 6 counter for if (i mod Nk = 0) test, where Nk = 6
	cpi mod6, 0x06				; The MOD 6 counter must keep its value in the range of 0 to 5
	brne aes_keyex_loop
	clr mod6
.endif
;
	rjmp aes_keyex_loop			; loop until done
;
aes_keyex_done:
	; at this point the entire key schedule has been expanded in SRAM
;
;-------------------------------------------------------------------------------------------------------------
;
.undef Rcon			; r19              The round constant word array
;
;-------------------------------------------------------------------------------------------------------------
;
;##########################################
.if !defined KEY_FLASH			; not KEY_FLASH
;
	OutReg RAMPZ, zero		; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	ret
;
;##########################################
.else	; KEY_FLASH
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
;
.def spmfunct= r19	;              SPMCSR function selection
;
;-------------------------------------------------------------------------------------------------------------
;
		; write the key schedule to FLASH where aes_key1 is pointing (pick an empty Boot loader FLASH page)
		; at least one or maybe two FLASH pages will be written, make sure the is room at aes_key1
		; this routine only works with No Read-While-Write (NRWW) FLASH memory
	ldi ZL, low(aes_key1 * 2)	; Set the RAMPZ:ZH:ZL expanded FLASH key schedule address to aes_key1
	ldi ZH, byte2(aes_key1 * 2)
	ldi	tmp, byte3(aes_key1 * 2)
	OutReg RAMPZ, tmp			; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
;
	lds YL, aes_kexp			; SRAM expanded key schedule pointer => w[i]
	lds YH, aes_kexp + 1
;
	clr cntl					; zero the 16 bit counter value
	clr cnth
;
aes_flash_loop:
		ldi r20, PAGE_SZ_W
	;
			; exit this routine when the 16 bit counter value is equal to or higher than ((Nb * (Nr+1)) * 4)
		ldi tmp, high((Nb * (Nr+1)) * 4)
		cpi cntl, low((Nb * (Nr+1)) * 4)
		cpc cnth, tmp
		brsh aes_flash_done
	;
aes_onepage:
		;
			ld r0, Y+			; get the first byte from boot loader FLASH
			ld r1, Y+			; get the second byte from boot loader FLASH (both bytes make a 16 bit FLASH word)
		;
			ldi spmfunct, (1 << SPMEN)	; (see data sheet)
			rcall aes_exe_spm	; copy the 2 FLASH bytes to the built-in temporary write buffer with the spm instruction
		;
			adiw ZH:ZL, 2		; increment the Z pointer by 1 word (+2) after the spm instruction is done
		;
			ldi tmp, 2
			add cntl, tmp
			adc cnth, zero
			subi r20, 1
		brne aes_onepage		; always copy at least one full page (1 or 2 pages will be copied)
	;
		movw XH:XL, ZH:ZL				; save the current ZH:ZL pointer
		subi ZL, low(PAGE_SZ_B)			; reset the FLASH pointer back to the beginning of the buffer
		sbci ZH, high(PAGE_SZ_B)		; spm needs to have RAMPZ:ZH:ZL point at the start of the buffer
	;
			; erase the page pointed to by RAMPZ:ZH:ZL
		ldi spmfunct, (1 << PGERS) | (1 << SPMEN)	; (see data sheet)
		rcall aes_exe_spm				; do the page erase
	;
			; write the temporary buffer page to the FLASH page pointed to by RAMPZ:ZH:ZL
		ldi spmfunct, (1 << PGWRT) | (1 << SPMEN)	; (see data sheet)
		rcall aes_exe_spm				; copy the 2 FLASH bytes to the built-in temporary write buffer
	;
		movw ZH:ZL, XH:XL				; restore the saved ZH:ZL pointer
	;
	rjmp aes_flash_loop
;
aes_flash_done:
;
	OutReg RAMPZ, zero		; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	ret
;
;-------------------------------------------------------------------------------------------------------------
;
;
aes_exe_spm:
;
	; Wait until / make sure, the FLASH is not busy
flash_wait:
	InReg tmp, SPMCSR		; Store Program Memory Control and Status Register
	sbrc tmp, SPMEN			; SPMEN = Store Program Memory Enable (see data sheet)
		rjmp flash_wait
;
	; Wait until / make sure, the EEPROM is not busy (recommended before writing to the SPMCSR register)
eeprom_wait:
	sbic EECR, EEWE			; EEWE = EEPROM Write Enable Write Strobe (while EEWE is set, the EEPROM is busy)
		rjmp eeprom_wait
;
		;	SPMIE  = SPM Interrupt Enable (0 = interrupt disabled, 1 = interrupt enabled)
		;	RWWSRE = Read-While-Write Section Read Enable (see data sheet)
		;	BLBSET = Boot Lock Bit Set (see data sheet)
		;	PGWRT  = Page Write (see data sheet)
		;	PGERS  = Page Erase (see data sheet)
		;	SPMEN  = Store Program Memory Enable (see data sheet)
	OutReg SPMCSR, spmfunct
	spm
;
	ret
;
;-------------------------------------------------------------------------------------------------------------
;
.undef spmfunct		; r19              SPMCSR function selection
;
;-------------------------------------------------------------------------------------------------------------
;
.endif	; KEY_FLASH
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
; Rijndael SubWord Transform
;
;	This is a non-linear byte substitution for each individual byte in a 32 bit temp word.
;		This routine is used by the Key Schedule
;
;	This code uses a trick to reuse the program code in the aes_sub_bytes program. The trick is the XH:XL registers
;		are pointed at general purpose register r12 with the hex value p_r12. p_r12 happens to be the address of r12
;		and this is the only way to express it as a pointer in AVR assembler 2 that I have found. Each call below
;		to aes_sub_loop will return the Rijndael subbyte transform value from the temporary register pointed to by
;		XH:XL and saved in the temporary register pointed to by XH:XL (and XH:XL will be incremented).
;
aes_subword:
		; SubWord temp
;
;
	ldi XL, low(p_r12)				; initialize the X pointer to point at register r12
	ldi XH, high(p_r12)				;	it will be incremented in aes_sub_loop call according to the state_cnt count
	ldi ZLt, low(aes_sub_xy * 2)	; initialize base pointer to the SRAM SubBytes Transformation table
	ldi ZHt, high(aes_sub_xy * 2)
;
	ldi state_cnt, 0x4	; set the aes_sub_bytes state_cnt counter value to 4 so aes_sub_bytes will do all 4 tmp registers
	rcall aes_sub_loop	; tmp0 is pointed to by X = p_r12
;
;
	ret					; save the temp result element
;
.endif	; KEY_GEN
;##########################################
;
;-------------------------------------------------------------------------------------------------------------
;
.undef zero			; r2               zero value constant
.undef tmp0			; r12              w[i-1] temp value, first byte
.undef tmp1			; r13              w[i-1] temp value, second byte
.undef tmp2			; r14              w[i-1] temp value, third byte
.undef tmp3			; r15              w[i-1] temp value, fourth byte
.undef cntl			; r16              (i = 0: i < (nbX4 * (Nr+1). i++) (low)
.undef cnth			; r17              (i = 0: i < (nbX4 * (Nr+1). i++) (high)
.undef tmp			; r18              temporary value
.undef round		; r25 <<=global=>> round counter
.if Nk == 6
.undef mod6			; r20              MOD 6 counter
.endif
.undef state_cnt	; r21              aes_subword - state table s_box counter
.undef ZLt			; r22              aes_subword - temporary ZL storage
.undef ZHt			; r23              aes_subword - temporary ZH storage
;
