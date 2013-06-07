;----------------------------------------------------
; Project: hello.zdsp
; Main File: hello.asm
; Date: 21/05/2013 11:36:11
;
; Created with zDevStudio - Z80 Development Studio.
;
;----------------------------------------------------

;    CX80 - Homebrew 8bit computer
;    (c) 2013 Calogiuri Enzo Antonio

;    hello.zdsp - Simple test that show how, using system calls,
;                 clear the screen, place the cursor on the screen
;                 and print text.

;    Compiler used: zDevStudio ver 0.8
;    Compiled on Windows 7 Home Premium

;    To compile this source, start zDevStudio, open project hello.zdsp
;    and press F9

;    To run this example on CX80, put hello.bin on sdcard, power on CX80
;    and type: run hello.bin [return]

include         "../CX80.inc"

org             $0008
;The first 8 bytes of memory are reserved and used to control the system

;Clear the screen
ld              iy, CX80_SYSTEM_FNC_REG_ADDR    ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_CLEAR_SCREEN     ;load into memory the desired function number

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

;Move the cursor at position 2, 13
ld              iy, CX80_SYSTEM_FNC_REG_ADDR    ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_MOVE_CURSOR      ;load into memory the desired function number
ld              (iy + CX80_SYSTEM_REG1L_ADDR), 2        ;REG1L = new row position
ld              (iy + CX80_SYSTEM_REG1H_ADDR), 13       ;REG1H = new column position

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

; Print a string at cursor position
ld              iy, CX80_SYSTEM_FNC_REG_ADDR    ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_PRINT_STRING     ;load into memory the desired function number
ld              hl, Msg                         ; HL = address of string to print
ld              (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
ld              (iy + CX80_SYSTEM_REG1H_ADDR), h        ; REG1L = high address value

call            CX80_SYSTEM_CALL_FNC_ADDR       ;Asks the system to perform the desired function

; IMPORTANT!! Program MUST use this call to exit to CX80 OS
ld              iy, CX80_SYSTEM_FNC_REG_ADDR            ;iy point to a memory used for manage system calls
ld              (iy), CX80_FNC_STOP_USER_PROGRAM        ;load into memory the desired function number
ld              (iy + CX80_SYSTEM_REG1L_ADDR), 0        ;Exit with no errors (0 = no errors)

call            CX80_SYSTEM_CALL_FNC_ADDR
halt                                            ;never execute

Msg             db "Hello World!", $D, $A, 0    ;IMPORTANT! Text string MUST end with 0
