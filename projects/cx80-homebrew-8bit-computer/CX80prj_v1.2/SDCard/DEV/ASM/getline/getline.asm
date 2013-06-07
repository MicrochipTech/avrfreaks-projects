;----------------------------------------------------
; Project: getline.zdsp
; Main File: getline.asm
; Date: 22/05/2013 10:55:30
;
; Created with zDevStudio - Z80 Development Studio.
;
;----------------------------------------------------

;    CX80 - Homebrew 8bit computer
;    (c) 2013 Calogiuri Enzo Antonio

;    getline.zdsp - Simple test that show how, using system calls,
;                   get a string typed by user.

;    Compiler used: zDevStudio ver 0.8
;    Compiled on Windows 7 Home Premium

;    To compile this source, start zDevStudio, open project getline.zdsp
;    and press F9

;    To run this example on CX80, put getline.bin on sdcard, power on CX80
;    and type: run getline.bin [return]

include         "../CX80.inc"

org             $0008
;The first 8 bytes of memory are reserved and used to control the system

ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_PRINT_STRING             ;load into memory the desired function number
ld              hl, Txt1                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              hl, Txt2                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              hl, Txt3                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              iy, CX80_SYSTEM_FNC_REG_ADDR
ld              (iy), CX80_FNC_GET_INPUT_STRING
ld              (iy + CX80_SYSTEM_REG1L_ADDR), 30       ; REG1L = len of inpute buffer (30)
ld              hl, InputBuffer                         ; HL = address of input buffer
ld              (iy + CX80_SYSTEM_REG2L_ADDR), l        ; REG2L = low address value
ld              (iy + CX80_SYSTEM_REG2H_ADDR), h        ; REG2L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              iy, CX80_SYSTEM_FNC_REG_ADDR
ld              (iy), CX80_FNC_PRINT_STRING
ld              hl, Txt4                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              hl, InputBuffer                         ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              hl, Txt5                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

; IMPORTANT!! Program MUST use this call to exit to CX80 OS
ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_STOP_USER_PROGRAM        ;load into memory the desired function number
ld              (iy + CX80_SYSTEM_REG1L_ADDR), 0        ;Exit with no errors (0 = no errors)

call            CX80_SYSTEM_CALL_FNC_ADDR
halt                                            ;never execute

InputBuffer     ds 30
Txt1            db "Example of getline system call", $D, $A, 0
Txt2            db "Type a string, confirm with Return", $D, $A, 0
Txt3            db "->", 0
Txt4            db $D, $A, "Typed:", 0
Txt5            db $D, $A, 0
