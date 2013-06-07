;----------------------------------------------------
; Project: waitkey.zdsp
; Main File: waitkey.asm
; Date: 23/05/2013 10:02:49
;
; Created with zDevStudio - Z80 Development Studio.
;
;----------------------------------------------------

;    CX80 - Homebrew 8bit computer
;    (c) 2013 Calogiuri Enzo Antonio

;    waitkey.zdsp - Simple test that show how, using system calls,
;                   program waits a keypress and print it on screen.

;    Compiler used: zDevStudio ver 0.8
;    Compiled on Windows 7 Home Premium

;    To compile this source, start zDevStudio, open project waitkey.zdsp
;    and press F9

;    To run this example on CX80, put waitkey.bin on sdcard, power on CX80
;    and type: run waitkey.bin [return]

include         "../CX80.inc"

org             $0008
;The first 8 bytes of memory are reserved and used to control the system

ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_PRINT_STRING             ;load into memory the desired function number
ld              hl, Txt1                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_WAIT_FOR_KEYPRESSED      ;load into memory the desired function number

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              a, (iy + CX80_SYSTEM_REG1L_ADDR)        ;store in reg a the ascii value of key pressed
ld              (KeyPressed), a                         ; and put it into ram

ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_PRINT_STRING             ;load into memory the desired function number
ld              hl, Txt2                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_PRINT_CHAR               ;load into memory the desired function number
ld              a, (KeyPressed)                         ; load the ascii value of key pressed from ram
ld              (iy + CX80_SYSTEM_REG1L_ADDR), a        ;REG1L = Ascii value of the character to be printed

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_PRINT_STRING             ;load into memory the desired function number
ld              hl, Txt3                                ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

; IMPORTANT!! Program MUST use this call to exit to CX80 OS
ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_STOP_USER_PROGRAM        ;load into memory the desired function number
ld              (iy + CX80_SYSTEM_REG1L_ADDR), 0        ;Exit with no errors (0 = no errors)

call            CX80_SYSTEM_CALL_FNC_ADDR
halt                                            ;never execute

KeyPressed      db 1
Txt1            db "Press a key...", 0
Txt2            db $D, $A, "Key pressed = ", 0
Txt3            db $D, &A, 0
