;--------------------------------------------------------------------------------------------------
;
; User selected AES encryption configuration
;
;==========>>>> Encryption and key generation is optional <<<<==========
;
; Comment out the .equ line below to remove the AES "encryption" code
;
	.equ ENCRYPT=1
;
; Comment out the .equ line below to remove the AES "key schedule generation" code
;
	.equ KEY_GEN=1
;
; Comment out the .equ line below to remove the AES "key schedule copy to FLASH" code, starting at the aes_key1 label (depends on KEY_GEN)
;
	;.equ KEY_FLASH=1
;
		.if defined KEY_FLASH
			.if !defined KEY_GEN
				.error "set KEY_GEN in order to use KEY_FLASH"
			.endif
		.endif
;
;
; Comment out the .equ line below if the application code does not use or define any SRAM (see below in Data SRAM section).
;	Using this define requires the application and boot loader code to each have exclusive use of the entire SRAM while running.
;	See the note in Data SRAM below.
;
	;.equ AES_SRMAM_SHARED=1
;
;==========>>>> USER SELECTABLE VALUE <<<<==========
.equ Nb = 4			; number of 32 bit s_box blocks (4, 6 or 8 words -- 4 bytes / 32 bits per word)
					;	4 word block = 16 byte block, 6 word block = 24 byte block and 8 word block = 32 byte block
;
.if !(((Nb) == 4) || ((Nb) == 6) || ((Nb) == 8))
.error "Nb is not a valid, its value must be 4, 6 or 8"
.endif
;
;
;==========>>>> USER SELECTABLE VALUE <<<<==========
.equ Nk = 8			; number of 32 bit key blocks (4, 6 or 8 words -- 4 bytes / 32 bits per word)
					;	4 word key = 128 bits, 6 word key = 192 bits and 8 word key = 256 bits
;
;
;
;========================================>>>> END OF USER SELECTABLE VALUES <<<<========================================
;
;
;
.if !(((Nk) == 4) || ((Nk) == 6) || ((Nk) == 8))
.error "Nk is not a valid, its value must be 4, 6 or 8"
.endif
.if Nb > Nk
.message "Nk SHOULD BE AT LEAST AS LARGE AS Nb"
.endif
;
; All of the extraneous assembler error reporting is suppressed so the explicit error message will not get buried under the extraneous ones
;
	; set Nr = number of rounds, based on Nb and Nk values
.if Nb == 4
	.if Nk == 4
		.equ Nr = 10
	.elif Nk == 6
		.equ Nr = 12
	.elif Nk == 8
		.equ Nr = 14
	.else
		.equ Nr = 10		; default definition used to prevent extraneous assembler error reporting if Nk (above) is not correct
	.endif
.elif Nb == 6
	.if ((Nk == 4) || (Nk == 6))
		.equ Nr = 12
	.elif Nk == 8
		.equ Nr = 14
	.else
		.equ Nr = 12		; default definition used to prevent extraneous assembler error reporting if Nk (above) is not correct
	.endif
.elif Nb == 8
	.equ Nr = 14
.else
	.equ Nr = 10			; default definition used to prevent extraneous assembler error reporting if Nb (above) is not correct
.endif
;
;
;
.equ nbX4 = Nb * 4			; number of s_box bytes (32 bytes maximum)
.equ nkX4 = Nk * 4			; number of key bytes (32 bytes maximum, 128 bits, 192 bits or 256 bits)
.equ nksc = nbX4 * (Nr+1)	; number of key schedule bytes ( 120 maximum words, 480 maximum bytes, 3840 maximum bits)
;
;--------------------------------------------------------------------------------------------------
;
; Data SRAM
;
;	The Application code and Bootloader code usually each have exclusive use of the entire SRAM. The AVRasm 2.1 assembler will not allow
;	the SRAM memory allocation to be reused after its defined the first time unless the .overlap directive is used.
;
;	It appears that when the AES_SRMAM_SHARED=1 equate is used, the include for aes_setup.inc must be placed before any other SRAM is
;	declared for the application program
;
;--------------------------------------------------------------------------------------------------
;
; Bootloader SRAM memory use.
;
;	AES words are 32 bits or 4 bytes long.
;
;	The .overlap directive is used to reassign SRAM memory allocations. Without this directive any application program code that
;		used .dseg to assign SRAM will cause an assembly error with this boot loader program code.
;
;	This shared SRAM allocation assumes this boot loader program code has exclusive use of the entire SRAM. This is only possible
;		when the application program and boot loader program are separate entities that initialize and run by themselves.
;
;##########################################
.if defined AES_SRMAM_SHARED
;
.dseg
.org SRAM_START

.overlap						; allow boot loader SRAM allocation separate from already allocated application SRAM
;
	s_box:		.byte	nbX4		; State s_box[Nb] word storage
	;
	.if defined KEY_GEN
	k_ex:		.byte	nksc		; Cipher key (Nk bytes) which can expand up to 480 bytes long (3840 bits total)
	.endif	; KEY_GEN
	;
	aes_sbox:	.byte	2			; s_box pointer storage location
	;
	aes_kexp:	.byte	2			; expanded key schedule pointer storage location
	;
.nooverlap						; ensure the overlap directive is turned off

;##########################################
.else	; not AES_SRMAM_SHARED
;##########################################
;
.dseg
.org SRAM_START
;
s_box:		.byte	nbX4		; State s_box[Nb] word storage
;
.if defined KEY_GEN
k_ex:		.byte	nksc		; Cipher key (Nk bytes) which can expand up to 480 bytes long (3840 bits total)
.endif	; KEY_GEN
;
aes_sbox:	.byte	2			; s_box pointer storage location
;
aes_kexp:	.byte	2			; expanded key schedule pointer storage location
;
.endif	; AES_SRMAM_SHARED
;##########################################
;
