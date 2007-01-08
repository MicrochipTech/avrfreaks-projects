;-----------------------------------------------------------------------------
; Program:   CCITT_CRC_16 
; Version:   0.1
; Date:      4/1/2007 
; Author:    Holmes
; Purpose:   To calculate the CRC-16 checksum of a data block.
;            This will support XMODEM data format.
; Target:    AVR Mega32  
; Proviso:   No optimisation has been carried out.
;            This program has been translated for AVR assembler
;            from Ashley Roll's "C" program.
; Testing:   The calculated CRC for the test data string "123456789" gives "29B1",
;            the same as achieved by Roll's program.
;            This has also been checked against the on-line CRC calculator at
;            http://www.lammertbies.nl/comm/info/crc-calculation.html
; Reference: http://www.digitalnemesis.com/info/codesamples/embeddedcrc16/ex4bit.c
; Credits:   Digital Nemesis (Ashley Roll)
; Timing:    582us for a 1024 bit data block (20MHz processor).
;-----------------------------------------------------------------------------
.include "m32def.inc"

.equ Data_Block_Size    = 9 ;128 ;size in bytes

.def CRC16_High         = r24
.def CRC16_Low          = r23
.def Nibble             = r25 ;only the 4 LSB's are used
.def Data_Byte          = r22
.def RH_Nibble_Mask     = r21
.def Byte_Counter       = r20
.def Working_Nibble     = r19
.def LH_Nibble_Mask     = r18

.dseg

LookupHigh:  .byte 16
LookupLow:   .byte 16
DataBlock:   .byte 128

.cseg
.org 0x0000

   rjmp RESET

RESET:
   ldi r16, low(RAMEND)       ;initialise stack pointer
   out SPL, r16
   ldi r16, high(RAMEND)
   out SPH, r16

INITIALISE:
   rcall CRC16_Initialise
MAIN:
   ldi r27, high(DataBlock)
   ldi r26, low(DataBlock)
   ldi r16, 49 ; 1
   st X+, r16
   ldi r16, 50 ; 2
   st X+, r16
   ldi r16, 51 ; 3
   st X+, r16
   ldi r16, 52 ; 4
   st X+, r16
   ldi r16, 53 ; 5
   st X+, r16
   ldi r16, 54 ; 6
   st X+, r16
   ldi r16, 55 ; 7
   st X+, r16
   ldi r16, 56 ; 8
   st X+, r16
   ldi r16, 57 ; 9
   st X, r16
   rcall Compute_CRC16
 
;----------------
CRC16_Initialise:
;----------------
; Inputs:         Nil
; Outputs:        Nil
; Registers used: r16 -     general purpose
;                 r21 -     Nibble Mask
;                 r26:r27 - 16-bit addressing (X-reg)
;----------------
   ldi CRC16_High, 0xFF       ;initialise the CRC registers
   ldi CRC16_Low, 0xFF        ;IAW CCITT specifications 

   ldi RH_Nibble_Mask, 0b00001111;build the nibble masks
   ldi LH_Nibble_Mask, 0b11110000 

;                             ;initialise the High and Low tables
   ldi r26, low(LookupHigh)   
   ldi r27, high(LookupHigh)

   ldi r16, 0x00              ;1st element of LookupHigh table
   st X+, r16

   ldi r16, 0x10              ;2nd element of LookupHigh table
   st X+, r16

   ldi r16, 0x20              ;3rd element of LookupHigh table
   st X+, r16

   ldi r16, 0x30              ;4th element of LookupHigh table
   st X+, r16

   ldi r16, 0x40              ;5th element of LookupHigh table
   st X+, r16

   ldi r16, 0x50              ;6th element of LookupHigh table
   st X+, r16

   ldi r16, 0x60              ;7th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0x70              ;8th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0x81              ;9th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0x91              ;10th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0xA1              ;11th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0xB1              ;12th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0xC1              ;13th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0xD1              ;14th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0xE1              ;15th element of LookupHigh table
   st X+, r16
   
   ldi r16, 0xF1              ;16th element of LookupHigh table
   st X, r16 

   ldi r26, low(LookupLow)
   ldi r27, high(LookupLow)

   ldi r16, 0x00              ;1st element of LookupLow table
   st X+, r16

   ldi r16, 0x21              ;2nd element of LookupLow table
   st X+, r16
   
   ldi r16, 0x42              ;3rd element of LookupLow table
   st X+, r16 

   ldi r16, 0x63              ;4th element of LookupLow table
   st X+, r16

   ldi r16, 0x84              ;5th element of LookupLow table
   st X+, r16

   ldi r16, 0xA5              ;6th element of LookupLow table
   st X+, r16

   ldi r16, 0xC6              ;7th element of LookupLow table
   st X+, r16

   ldi r16, 0xE7              ;8th element of LookupLow table
   st X+, r16

   ldi r16, 0x08              ;9th element of LookupLow table
   st X+, r16

   ldi r16, 0x29              ;10th element of LookupLow table
   st X+, r16
   
   ldi r16, 0x4A              ;11th element of LookupLow table
   st X+, r16
   
   ldi r16, 0x6B              ;12th element of LookupLow table
   st X+, r16
   
   ldi r16, 0x8C              ;13th element of LookupLow table
   st X+, r16
   
   ldi r16, 0xAD              ;14th element of LookupLow table
   st X+, r16
   
   ldi r16, 0xCE              ;15th element of LookupLow table
   st X+, r16
   
   ldi r16, 0xEF              ;16th element of LookupLow table
   st X, r16 
   ret

;------------
Compute_CRC16:
;------------
; Purpose:        To pass each nibble of DataBlock to CRC generator
;                 in turn. Each such 4-bits is RJ in Nibble.
;                 The CRC generated by CRC16_Update4Bits can be found in
;                 CRC16_Low and CRC16_High. 
; Inputs:         Nil
; Outputs:        Nil
; Registers used: r20 -     Byte_Counter
;                 r21 -     RH_Nibble_Mask
;                 r22 -     DataByte
;                 r25 -     Nibble 
;                 r28:r29 - 16-bit addressing (Y-reg)
; Calls:          CRC16_Update4Bits
; Timing:         without computation:  802.25us on 4MHz simulator
;                 with computation:    2914.25us on 4MHz simulator
;                 extrapolates to:      582.00us on 20MHz machine
; Testing:        Test Vector: 123456789 (character string)
;                 CRC:         0x29B1    
;----------------
   clr Byte_Counter
   ldi r28, low(DataBlock)
   ldi r29, high(DataBlock)
CC_Next_Byte:
   ld Data_Byte, Y+              ;get next byte of Data table
CC_Left_Nibble:
   mov Nibble, Data_Byte
   swap Nibble   
   and Nibble, RH_Nibble_Mask    ;now we have the LH-nibble RJ and cleaned
   rcall CRC16_Update4Bits
CC_Right_Nibble:
   mov Nibble, Data_Byte
   and Nibble, RH_Nibble_Mask    ;now we have the RH-nibble RJ and cleaned
   rcall CRC16_Update4Bits
CC_End_of_Byte:
   inc Byte_Counter
   cpi Byte_Counter, Data_Block_Size
   brlo CC_Next_Byte
   ret

;----------------
CRC16_Update4Bits:
;----------------
; Purpose:        To compute the CRC for a nibble and update the
;                 CRC registers.
; Inputs:         r25 -     Nibble (RJ) leading 0's
; Outputs:        r23 -     CRC16_Low - updated by CRC generator on each call
;                 r24 -     CRC16_High                ditto
; Registers used: r16 -     general purpose
;                 r17 -          ditto
;                 r18 -     LH_Nibble_Mask
;                 r19 -     Working_Nibble; relates to "t" in Roll's program
;                 r21 -     RH_Nibble_Mask
;                 r26:r27 - 16-bit addressing
; Calls:          Nil
; Timing:         10us on 4MHz simulator
;----------------
   mov r16, CRC16_High           ;isolate the MSB of CRC16_High register
   swap r16
   and r16, RH_Nibble_Mask       ;clear LH nibble
   mov Working_Nibble, r16

   eor Working_Nibble, Nibble    ;XOR in the data nibble

                                 ;shift the CRC registers LEFT 4 bits 
   mov r16, CRC16_High
   swap r16
   and r16, LH_Nibble_Mask       ;high nibble is done, low nibble cleared 
   mov r17, CRC16_Low
   swap r17
   and r17, RH_Nibble_Mask       
   or r17, r16                   
   mov CRC16_High, r17           ;completing the High register
   
   mov r16, CRC16_Low 
   swap r16
   and r16, LH_Nibble_Mask
   mov CRC16_Low, r16            ;completing the Low register

                                 ;do the table lookups and XOR the
								 ;result into the CRC registers
   clr r16
   ldi r26, low(LookupHigh)      ;get the 16-bit address of LookupHigh
   ldi r27, High(LookupHigh)
   add r26, Working_Nibble       ;add the "t" value from Roll's program
   adc r27, r16                  ;now have address of element of LookupHigh
   ld r16, X                     ;now have the table value
   eor CRC16_High, r16           ;completes the XOR of High register

   clr r16
   ldi r26, low(LookupLow)       ;get the 16-bit address of LookupLow
   ldi r27, High(LookupLow)
   add r26, Working_Nibble       ;add the "t" value from Roll's program
   adc r27, r16                  ;now have address of element of LookupLow
   ld r16, X                     ;now have the table value
   eor CRC16_Low, r16            ;completes the XOR of Low register
   ret


   
