;--------------------------------------------------------------------------------------------------
; AES - AT90CAN128 test routine for the Rijndael / Advanced Encryption Standard (AES) Toolkit
;
.include	"can128def.inc"
.include	"regdefs.inc"
.include	"boot.inc"
.include	"aes_setup.inc"

	; alternate names for the multiply result registers
.def multl   =r0		; multiplier result word (low)
.def multh   =r1		; multiplier result word (high)

.cseg

.org 0

	jmp bootload	; External, Power-on, Brownout, Watchdog and JTAG AVR Reset


.cseg

.org LARGEBOOTSTART

;
; BOOT LOADER
;
	jmp bootload	; External, Power-on, Brownout, Watchdog and JTAG AVR Reset

bootload:
	;
	; Globally disable all interrupts
	;
	cli

	;
	; Set stack pointer to end of SRAM
	;
	ldi XL, low(RAMEND)
	ldi XH, high(RAMEND)
	OutReg SPL, XL			; Low stack Pointer Register
	OutReg SPH, XH			; High stack Pointer Register

;--------------------------------------------------------------------------------------------------
;
;	This is just some simple test code to provide a framework example on how to use aes_gentbl,
;		aes_key_expand, aes_b_encrypt and aes_b_decrypt. The data in these examples are from the
;		FIPS PUB 197 AES specification. You can use this document to test these routines.
;
;	Sorry, I like to use AVR register defines as it helps me integrate different code modules. If you
;		do not like them then you can cut-n-paste edit them out.
;
;--------------------------------------------------------------------------------------------------
; TEST <==> TEST <==> TEST <==> TEST <==> TEST <==> TEST <==> TEST <==> TEST <==> TEST <==> TEST <==> TEST <==>
;--------------------------------------------------------------------------------------------------
;
; Check out the aes_setup.inc file first (it has all the assembly flags)
;	boot.inc has some AT90CAN128 FLASH size information
;
		; Generate the Inverse Finite Field Multiplication table
;
;
;
	call aes_gentbl
;
;
;
aes_test:		; cobbled together test code
	ldi	r16, byte3(aes_b_decrypt * 2)
	OutReg RAMPZ, r16				; FLASH Page Z Select Register (17th bit extension for the Z registers)
	clr r16
	rjmp eeeover
;
; the first s_box[Nb=4] (16 bytes) is from the Advanced Encryption Standard (AES) (FIPS PUB 197) document.
; the second s_box[[Nb=4]] (16 bytes) is from round 1 of a test in the Advanced Encryption Standard (AES) (FIPS PUB 197) document.
; do not forget to set Nb in aes_setup.inc to match the block size
s_btab:
	.db 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
	;.db 0x19, 0x3d, 0xe3, 0xbe, 0xa0, 0xf4, 0xe2, 0x2b, 0x9a, 0xc6, 0x8d, 0x2a, 0xe9, 0xf8, 0x48, 0x08
;
;##########################################
.if defined KEY_GEN
;
; the 128, 192 and 256 bit cipher keys below are from the Advanced Encryption Standard (AES) (FIPS PUB 197) document.
; comment out the two keys that are not used and uncomment the key that is used
; do not forget to set Nk in aes_setup.inc to match the key size
;
s_ktab:
		; 256 bit cipher key
	.db 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81		; 256 bit cipher key first half
	.db 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4		; 256 bit cipher key second half
		; 192 bit cipher key
	;.db 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b		; 192 bit cipher key first half
	;.db 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b		; 192 bit cipher key second half
		; 128 bit cipher key
	;.db 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c		; 128 bit cipher key
;
.endif ; KEY_GEN
;##########################################
eeeover:
		; copy the test s_box[i] values to SRAM
	clr r16						; initialize [i] counter
	ldi XL, low(s_box)			; SRAM s_box[Nb] address
	ldi XH, high(s_box)
	ldi ZL, low(s_btab * 2)		; FLASH s_box[Nb] sample data address
	ldi ZH, high(s_btab * 2)
;
aes_test_loop:
		elpm r17, Z+			; get a s_box[r16] FLASH test data byte
		st X+, r17				; save it in s_box[r16]
		inc r16
		cpi r16, nbX4
	brlo aes_test_loop
;
;##########################################
.if defined KEY_GEN
		; copy the test key values to SRAM
	clr r16
	ldi XL, low(k_ex)			; SRAM key[Nk] pointer
	ldi XH, high(k_ex)
	ldi ZL, low(s_ktab * 2)		; FLASH expanded key schedule pointer ((Nb * (Nr+1)) * 4), up to 480 bytes max.
	ldi ZH, high(s_ktab * 2)
aes_ktest_loop:
		elpm r17, Z+
		st X+, r17
		inc r16
		cpi r16, nkX4
	brlo aes_ktest_loop
;
	ldi ZL, low(k_ex)				; expanded key schedule SRAM pointer (low)
	ldi ZH, high(k_ex)				; expanded key schedule SRAM pointer (high)
	sts aes_kexp, ZL				; save the SRAM pointer in the SRAM aes_kexp variable location
	sts aes_kexp+1, ZH
;
;
;
	rcall aes_key_expand			; perform the key expansion
;
;
;
.endif ; KEY_GEN
;##########################################
;
;##########################################
.if defined ENCRYPT
;##########################################
.if defined KEY_GEN
	ldi ZL, low(k_ex)				; expanded key schedule FLASH pointer (low)
	ldi ZH, high(k_ex)				; expanded key schedule FLASH pointer (high)
.else
	ldi ZL, low(aes_key1 * 2)		; expanded key schedule FLASH pointer (low)
	ldi ZH, high(aes_key1 * 2)		; expanded key schedule FLASH pointer (high)
.endif ; KEY_GEN
;##########################################
										; ldi ZL, low(k_ex)				; optional expanded key schedule SRAM pointer (low)
										; ldi ZH, high(k_ex)				; optional expanded key schedule SRAM pointer (high)
	sts aes_kexp, ZL				; save the FLASH pointer in the SRAM aes_kexp variable location
	sts aes_kexp+1, ZH
;
	ldi ZL, low(s_box)				; SRAM s_box[Nb] pointer (low)
	ldi ZH, high(s_box)				; SRAM s_box[Nb] pointer (high)
	sts aes_sbox, ZL				; save the SRAM pointer in the SRAM aes_sbox variable location
	sts aes_sbox+1, ZH
;
;
;
	rcall aes_b_encrypt				; perform the encryption on a Nb sized block pointed to by the aes_sbox variable
;
;
;
.endif ; ENCRYPT
;##########################################
;
;##########################################
.if defined KEY_GEN
	ldi ZL, low(k_ex)				; expanded key schedule FLASH pointer (low)
	ldi ZH, high(k_ex)				; expanded key schedule FLASH pointer (high)
.else
	ldi ZL, low(aes_key1 * 2)		; expanded key schedule FLASH pointer (low)
	ldi ZH, high(aes_key1 * 2)		; expanded key schedule FLASH pointer (high)
.endif ; KEY_GEN
;##########################################
										; ldi ZL, low(k_ex)				; optional expanded key schedule SRAM pointer (low)
										; ldi ZH, high(k_ex)				; optional expanded key schedule SRAM pointer (high)
	sts aes_kexp, ZL				; save the FLASH pointer in the SRAM aes_kexp variable location
	sts aes_kexp+1, ZH
;
	ldi ZL, low(s_box)				; SRAM s_box[Nb] pointer (low)
	ldi ZH, high(s_box)				; SRAM s_box[Nb] pointer (high)
	sts aes_sbox, ZL				; save the SRAM pointer in the SRAM aes_sbox variable location
	sts aes_sbox+1, ZH
;
;
;
	rcall aes_b_decrypt				; perform the decryption on a Nb sized block pointed to by the aes_sbox variable
;
;
;
	ldi	r16, 0
	OutReg RAMPZ, r16				; FLASH Page Z Select Register (17th bit extension for the Z registers)
;
	jmp bootload					; The test is done <<<<<===== loop back to the start
;
;--------------------------------------------------------------------------------------------------
; AES routines
;
.include "aes.asm"
;
;
;##########################################
.if !defined KEY_GEN 	; not KEY_GEN
;
;-------------------------------------------------------------------------------------------------------------
;
; AES expanded key schedule (key block size is 32 bytes and text block = 16 bytes):
;	Rijndael and not AES allows text blocks of up to 32 bytes which can grow the key schedule up to 480 bytes.
;
; 256 bit cipher expanded key schedule from the Advanced Encryption Standard (AES) (FIPS PUB 197) document A.3 example.
; Generated by aes_key_expand, read from FLASH into a .hex file, copied here and formatted into .db statements by hand.
; The last line of 16 bytes is not used (the key schedule is only 240 bytes). In a real application you may want to offset
; the starting point of this table and fill in deceptive bytes at the beginning and end of the 240 byte key.
;
;
.org 0xFF00		; second to last page
;
aes_key1:
			; Nb = 4, Nk = 8 - 256 bit key AES example (used for testing)
	.db 0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81
	.db 0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
	.db 0x9B, 0xA3, 0x54, 0x11, 0x8E, 0x69, 0x25, 0xAF, 0xA5, 0x1A, 0x8B, 0x5F, 0x20, 0x67, 0xFC, 0xDE
	.db 0xA8, 0xB0, 0x9C, 0x1A, 0x93, 0xD1, 0x94, 0xCD, 0xBE, 0x49, 0x84, 0x6E, 0xB7, 0x5D, 0x5B, 0x9A
	.db 0xD5, 0x9A, 0xEC, 0xB8, 0x5B, 0xF3, 0xC9, 0x17, 0xFE, 0xE9, 0x42, 0x48, 0xDE, 0x8E, 0xBE, 0x96
	.db 0xB5, 0xA9, 0x32, 0x8A, 0x26, 0x78, 0xA6, 0x47, 0x98, 0x31, 0x22, 0x29, 0x2F, 0x6C, 0x79, 0xB3
	.db 0x81, 0x2C, 0x81, 0xAD, 0xDA, 0xDF, 0x48, 0xBA, 0x24, 0x36, 0x0A, 0xF2, 0xFA, 0xB8, 0xB4, 0x64
	.db 0x98, 0xC5, 0xBF, 0xC9, 0xBE, 0xBD, 0x19, 0x8E, 0x26, 0x8C, 0x3B, 0xA7, 0x09, 0xE0, 0x42, 0x14
	.db 0x68, 0x00, 0x7B, 0xAC, 0xB2, 0xDF, 0x33, 0x16, 0x96, 0xE9, 0x39, 0xE4, 0x6C, 0x51, 0x8D, 0x80
	.db 0xC8, 0x14, 0xE2, 0x04, 0x76, 0xA9, 0xFB, 0x8A, 0x50, 0x25, 0xC0, 0x2D, 0x59, 0xC5, 0x82, 0x39
	.db 0xDE, 0x13, 0x69, 0x67, 0x6C, 0xCC, 0x5A, 0x71, 0xFA, 0x25, 0x63, 0x95, 0x96, 0x74, 0xEE, 0x15
	.db 0x58, 0x86, 0xCA, 0x5D, 0x2E, 0x2F, 0x31, 0xD7, 0x7E, 0x0A, 0xF1, 0xFA, 0x27, 0xCF, 0x73, 0xC3
	.db 0x74, 0x9C, 0x47, 0xAB, 0x18, 0x50, 0x1D, 0xDA, 0xE2, 0x75, 0x7E, 0x4F, 0x74, 0x01, 0x90, 0x5A
	.db 0xCA, 0xFA, 0xAA, 0xE3, 0xE4, 0xD5, 0x9B, 0x34, 0x9A, 0xDF, 0x6A, 0xCE, 0xBD, 0x10, 0x19, 0x0D
	.db 0xFE, 0x48, 0x90, 0xD1, 0xE6, 0x18, 0x8D, 0x0B, 0x04, 0x6D, 0xF3, 0x44, 0x70, 0x6C, 0x63, 0x1E
;	.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
;
			; Nb = 4, Nk = 4 - 128 bit key AES example (used for testing)
;	.db 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
;	.db 0xA0, 0xFA, 0xFE, 0x17, 0x88, 0x54, 0x2C, 0xB1, 0x23, 0xA3, 0x39, 0x39, 0x2A, 0x6C, 0x76, 0x05
;	.db 0xF2, 0xC2, 0x95, 0xF2, 0x7A, 0x96, 0xB9, 0x43, 0x59, 0x35, 0x80, 0x7A, 0x73, 0x59, 0xF6, 0x7F
;	.db 0x3D, 0x80, 0x47, 0x7D, 0x47, 0x16, 0xFE, 0x3E, 0x1E, 0x23, 0x7E, 0x44, 0x6D, 0x7A, 0x88, 0x3B
;	.db 0xEF, 0x44, 0xA5, 0x41, 0xA8, 0x52, 0x5B, 0x7F, 0xB6, 0x71, 0x25, 0x3B, 0xDB, 0x0B, 0xAD, 0x00
;	.db 0xD4, 0xD1, 0xC6, 0xF8, 0x7C, 0x83, 0x9D, 0x87, 0xCA, 0xF2, 0xB8, 0xBC, 0x11, 0xF9, 0x15, 0xBC
;	.db 0x6D, 0x88, 0xA3, 0x7A, 0x11, 0x0B, 0x3E, 0xFD, 0xDB, 0xF9, 0x86, 0x41, 0xCA, 0x00, 0x93, 0xFD
;	.db 0x4E, 0x54, 0xF7, 0x0E, 0x5F, 0x5F, 0xC9, 0xF3, 0x84, 0xA6, 0x4F, 0xB2, 0x4E, 0xA6, 0xDC, 0x4F
;	.db 0xEA, 0xD2, 0x73, 0x21, 0xB5, 0x8D, 0xBA, 0xD2, 0x31, 0x2B, 0xF5, 0x60, 0x7F, 0x8D, 0x29, 0x2F
;	.db 0xAC, 0x77, 0x66, 0xF3, 0x19, 0xFA, 0xDC, 0x21, 0x28, 0xD1, 0x29, 0x41, 0x57, 0x5C, 0x00, 0x6E
;	.db 0xD0, 0x14, 0xF9, 0xA8, 0xC9, 0xEE, 0x25, 0x89, 0xE1, 0x3F, 0x0C, 0xC8, 0xB6, 0x63, 0x0C, 0xA6
;	.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
;	.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
;	.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
;	.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
;	.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
;
;
;##########################################
.else	; KEY_GEN
;##########################################
;
;
.org 0xFF00		; second to last page
;
aes_key1:
;
		; when a key is generated and written to flash, it must start on a FLASH page address boundary
		; after it is generated, you may move it where you want it in Boot loader FLASH without page boundary restrictions
		; be careful, a key schedule can be up to 480 bytes long (2 FLASH pages)
		;	however, this can only happen if you select a non-standard Nb block size (Nb = 4 is the AES standard)
	.if defined KEY_FLASH
		.if ((aes_key1 * 2) & 0xFF) != 0
			.error "aes_key1 must start on an even FLASH page address"
		.endif
	.endif
;
.endif 	; not KEY_GEN
;##########################################
;
