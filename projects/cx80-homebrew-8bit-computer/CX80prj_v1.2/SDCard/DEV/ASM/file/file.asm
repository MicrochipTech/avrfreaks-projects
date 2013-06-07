;----------------------------------------------------
; Project: file.zdsp
; Main File: file.asm
; Date: 23/05/2013 11:13:34
;
; Created with zDevStudio - Z80 Development Studio.
;
;----------------------------------------------------

;    CX80 - Homebrew 8bit computer
;    (c) 2013 Calogiuri Enzo Antonio

;    file.zdsp - Simple test that show how, using system calls,
;                create a text file and write into it.

;    Compiler used: zDevStudio ver 0.8
;    Compiled on Windows 7 Home Premium

;    To compile this source, start zDevStudio, open project file.zdsp
;    and press F9

;    To run this example on CX80, put file.bin on sdcard, power on CX80
;    and type: run file.bin [return]

include         "../CX80.inc"

org             $0008
;The first 8 bytes of memory are reserved and used to control the system

                ;iy point to a memory used for manage system calls
                ld      iy, CX80_SYSTEM_FNC_REG_ADDR

                ;load into HL StartMessage Address
                ld      hl, StartMessage

                ;print message on screen
                call    WriteMsg

                ;load into HL FileName Address
                ld      hl, FileName

                ;Try to create the file
                call    CreateFile

                ;Test if file creation is OK
                ;if L > 2, error!
                ld      a, l
                sub     $3

                jr      NC, _CreateError
                ld      a, l
                ;Save the file ID into memory
                ld      (FileID), a

                ;Try to write a string into file
                call    WriteBuffer

                ; if A = 0, all ok!
                ld      a, l
                or      a
                jr      Z, _Write_OK

                ; Error on write, put message on screen
                ld      hl, Error1

                call    WriteMsg

                jr      _FileClose

_Write_OK:
                ; Write ok, print message on screen
                ld      hl, MsgWriteOk

                call    WriteMsg

_FileClose:
                ; Close the opened file
                call    CloseFile

                jr      _ExitProgram


_CreateError:
                ; Error on file creation, print message on screen
                ld      hl, Error2

                call    WriteMsg

_ExitProgram:
                ; IMPORTANT!! Program MUST use this call to exit to CX80 OS
                ld              (iy), CX80_FNC_STOP_USER_PROGRAM        ;load into memory the desired function number
                ld              (iy + CX80_SYSTEM_REG1L_ADDR), 0        ;Exit with no errors (0 = no errors)

                ;Asks the system to perform the desired function
                call            CX80_SYSTEM_CALL_FNC_ADDR
                halt

WriteMsg:
                ;This routine print a message on screen
                ;HL must contain the addess of string to print
                ld      (iy), CX80_FNC_PRINT_STRING             ;load into memory the desired function number
                ld      (iy + CX80_SYSTEM_REG1L_ADDR), l        ; REG1L = low address value
                ld      (iy + CX80_SYSTEM_REG1H_ADDR), h

                ;Asks the system to perform the desired function
                call    CX80_SYSTEM_CALL_FNC_ADDR

                ret

CreateFile:
                ;This routine try to create a file
                ;HL must contain the addess of filename to print
                ld      (iy), CX80_FNC_OPEN_CREATE_FILE         ;load into memory the desired function number
                ld      (iy + CX80_SYSTEM_REG1L_ADDR), FA_WRITE Or FA_CREATE_ALWAYS
                ld      (iy + CX80_SYSTEM_REG2L_ADDR), l
                ld      (iy + CX80_SYSTEM_REG2H_ADDR), h

                ;Asks the system to perform the desired function
                call    CX80_SYSTEM_CALL_FNC_ADDR

                ;REG1H contain the return value
                ;store it into reg L
                ld      l, (iy + CX80_SYSTEM_REG1H_ADDR)

                ret

WriteBuffer:
                ;This routine try to write into file
                ld      (iy), CX80_FNC_WRITE_FILE_BUFFER        ;load into memory the desired function number
                ld      a, (FileID)
                ld      (iy + CX80_SYSTEM_REG1L_ADDR), a        ;REG1L = fileID

                ;HL point to text to write
                ld      hl, TextToWrite

                ;REG2 = HL
                ld      (iy + CX80_SYSTEM_REG2L_ADDR), l
                ld      (iy + CX80_SYSTEM_REG2H_ADDR), h

                ;HL contain the size of buffer to write
                ld      hl, 16

                ;REG3 = HL
                ld      (iy + CX80_SYSTEM_REG3L_ADDR), l
                ld      (iy + CX80_SYSTEM_REG3H_ADDR), h

                ;Asks the system to perform the desired function
                call    CX80_SYSTEM_CALL_FNC_ADDR

                ;REG1H contain the return value
                ;store it into reg L
                ld      l, (iy + CX80_SYSTEM_REG1H_ADDR)

                ret

CloseFile:
                ;This routine close the file
                ld      (iy), CX80_FNC_CLOSE_FILE               ;load into memory the desired function number
                ld      a, (FileID)
                ld      (iy + CX80_SYSTEM_REG1L_ADDR), a        ;REG1L = fileID

                call    CX80_SYSTEM_CALL_FNC_ADDR

                ret

FileID          db 1
FileName        db "ftest.txt", 0
StartMessage    db "Create ftest.txt ...", $A, $D, 0
TextToWrite     db "CX80 FILE TEST", &A, $D, 0
MsgWriteOk      db "Write ok", &A, $D, 0
Error1          db "Error while write on file!", &A, $D, 0
Error2          db "Error on file creation!", &A, $D, 0
