#define _SFR_ASM_COMPAT 1
#include <avr/io.h>

.global MBC_mode_MBC1_16M8k
.global MBC_mode_MBC1_4M32k
.global MBC_mode_MBC2
.global MBC_mode_MBC3
.global MBC_mode_MBC5

/*
  This code revision is meant to be faster to execute by using a masked value from PIND
  to jump directly to code. The upper bits of PIND show the address to be used and if a
  read is being called for, which is the necessary info to set a jump table.

  Each part of the table is aligned to a section in program memory, which wastes space,
  but improves performance. All MBC modes are located within 2KB of program memory,
  which means RJMP can be used without issue through all MBC modes (even though there is
  no reason that you should jump from one MBC mode to another, except for MBC_16M and
  MBC_4M (which is allowed to change on the fly).

  Because we're using IJMP, we need to set two bytes to jmp to in r30:31. r30 will be the
  masked PINA value, and the lowest 3 bits of r31 will set which area in program 
  corresponds to the selected MBC mode. Note we can't use r31 = 0, as this would require
  our jmp to go as low as 0x0000, which is obviously in the interrupt table! Accordingly,
  r31 must start as 0x01 at minimum, which should jump to 0x0100 at lowest (256 words in).
  Note as each jmp is based off the upper nibble of r30, you can only have 16 instructions 
  for each part of the JMP table. I think that's very reasonable for what we have to do.

  The hardest part of this will be setting up the memory areas and making sure we're
  calling the correct positions.

  PA7 will be used as an internal flag for RAM_ENABLE - PA5 is GB_/CS which will be used
  concurrently!
*/

/*
  r31 values for defining which jump table will be used:
  0x3F location of the base MBC code to be within RJMP distance
  0x40 MBC1_16M8k
  0x41 MBC2_2M32k
  0x42 MBC5
*/
#define r31_MBC1_16M  0x40
#define r31_MBC1_4M   0x41
#define r31_MBC5      0x42
#define r31_MBC3      0x43
#define r31_MBC5_DSM  0x44  // Double speed mode jump table :)

#define MA14_18_21_REGISTER   r22 ; Do not use this register for anything else!
#define MBC3_RAM_REGISTER     r23 ; As the comment suggests, used for RAM stuff in MBC3 <- change this to r28 for RTC abilities...

.section .MBC_BASE,"ax",@progbits ;0x3F00
; **********************************************************************************
MBC_mode_MBC1_16M8k:
  ; Set up the external interrupt on PD2 for the interrupt
  ldi   r16, 0x03; set rising edge mask for EICRA
  sts   EICRA, r16; Set EICRA
  ldi   r16, 0x01
  out   _SFR_IO_ADDR(EIMSK), r16; set EIMSK to enable the rising edge INT0 interrupt
  clr   r0; Make r0 0x01 for use clearing interrupt flags later - r1 is always 0
  inc   r0; r0 = 0x01
  clr   r1; Make sure this is zero, and leave it at zero!

  ldi   r22, 0x01; MA14-18 must start as 0x01 to meet the MBC1 read spec (can't be bank 0)
  ldi   ZH, r31_MBC1_16M; Set high byte for MBC mode for IJMP call to 1 (can't be 0)
  
MBC1_16M_CLK:; Wait for a CLK pulse
  sbis  _SFR_IO_ADDR(EIFR), INT0; See if the interrupt flag is set in EIFR, if so, continue, else loop
  rjmp  MBC1_16M_CLK
  
  ; These NOPs are required because ADDR15 is set AFTER all of the other address bits. Min 5 cycles after the rjmp above
  out   _SFR_IO_ADDR(EIFR), r0; EIFR, clear the interrupt flag
  nop 
  nop
  nop
  nop

  in    ZL, _SFR_IO_ADDR(PIND)
  andi  ZL, 0xF0
  ijmp

; **************************************
MBC_mode_MBC1_4M32k:
  ; Set up the external interrupt on PD2 for the interrupt
  ldi   r16, 0x03; set rising edge mask for EICRA
  sts   EICRA, r16; Set EICRA
  ldi   r16, 0x01
  out   _SFR_IO_ADDR(EIMSK), r16; set EIMSK to enable the rising edge INT0 interrupt
  clr   r0; Make r0 0x01 for use clearing interrupt flags later - r1 is always 0
  inc   r0; r0 = 0x01
  clr   r1; Make sure this is zero, and leave it at zero!

  ldi   r22, 0x01; MA14-18 must start as 0x01 to meet the MBC1 read spec (can't be bank 0)
  ldi   ZH, r31_MBC1_4M; Set high byte for MBC mode for IJMP call to 1 (can't be 0)
  
MBC1_4M_CLK:; Wait for a CLK pulse
  sbis  _SFR_IO_ADDR(EIFR), INT0; See if the interrupt flag is set in EIFR, if so, continue, else loop
  rjmp  MBC1_4M_CLK
  
  ; These NOPs are required because ADDR15 is set AFTER all of the other address bits. Min 5 cycles after the rjmp above
  out   _SFR_IO_ADDR(EIFR), r0; EIFR, clear the interrupt flag
  nop
  nop
  nop
  nop

  in    ZL, _SFR_IO_ADDR(PIND)
  andi  ZL, 0xF0
  ijmp

; **********************************************************************************
MBC_mode_MBC5:
  ; Set up the external interrupt on PD2 for the interrupt
  ldi   r16, 0x03; set rising edge mask for EICRA
  sts   EICRA, r16; Set EICRA
  ldi   r16, 0x01
  out   _SFR_IO_ADDR(EIMSK), r16; set EIMSK to enable the rising edge INT0 interrupt
  clr   r0; Make r0 0x01 for use clearing interrupt flags later - r1 is always 0
  inc   r0; r0 = 0x01
  clr   r1; Make sure this is zero, and leave it at zero!

  ldi   r22, 0x01; Other code looks like it preloads this to bank 1, even though 0x00 is a valid bank for MBC5
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  ldi   r31, r31_MBC5; Set high byte for MBC mode for IJMP call to 1 (can't be 0)

MBC5_CLK:; Wait for a CLK pulse
  sbis  _SFR_IO_ADDR(EIFR), INT0; See if the interrupt flag is set in EIFR, if so, continue, else loop
  rjmp  MBC5_CLK
  
  ; These NOPs are required because ADDR15 is set AFTER all of the other address bits. Min 5 cycles after the rjmp above
  out   _SFR_IO_ADDR(EIFR), r0; EIFR, clear the interrupt flag
  in    ZL, _SFR_IO_ADDR(PIND)
  andi  ZL, 0xF0
  in    r25, _SFR_IO_ADDR(PIND) ; Take this in to check CLK during READ cycles to go into double speed mode...
  ijmp
 

MBC5_DSM_CLK:; Wait for a CLK pulse
  sbis  _SFR_IO_ADDR(EIFR), INT0; See if the interrupt flag is set in EIFR, if so, continue, else loop
  rjmp  MBC5_DSM_CLK
  out   _SFR_IO_ADDR(EIFR), r0; EIFR, clear the interrupt flag
  
  ; ADDR15 is set AFTER all of the other address bits. Guess 3 cycles in DSM - will have to test
  in    ZL, _SFR_IO_ADDR(PIND)
  andi  ZL, 0xF0 ; Would be nice to do away with this. But CLK is always going to be hi, UART lines are probably pulled up too as will WE. It may be better to invert these lines in HW instead...
  ijmp

; **********************************************************************************
MBC_mode_MBC3:
  ; Set up the external interrupt on PD2 for the interrupt
  ldi   r16, 0x03; set rising edge mask for EICRA
  sts   EICRA, r16; Set EICRA
  ldi   r16, 0x01
  out   _SFR_IO_ADDR(EIMSK), r16; set EIMSK to enable the rising edge INT0 interrupt
  clr   r0; Make r0 0x01 for use clearing interrupt flags later - r1 is always 0
  inc   r0; r0 = 0x01
  clr   r1; Make sure this is zero, and leave it at zero!

  ldi   r22, 0x01; MA14-18 must start as 0x01 to meet the MBC1 read spec (can't be bank 0)
  ldi   ZH, r31_MBC3; Set high byte for MBC mode for IJMP call to 1 (can't be 0)

; Okay RAM and RTC for MBC3. This has been painful because of the overlap. 3rd attempt, I will ignore RTC.
; To do RAM, I will use the hardware solution, as for the other MBC modes, as when I tried to use 
; a software flag, Pokemon Red broke and the sprites on the start screen were corrupted! I will use
; a flag in a RAM register to manage RTC if I ever get there. (1 is enabled, 0 disabled. This way I can use it for 
; normal RAM access too. I'll still keep the register value in r28 just in case. You'll see how it works in the 
; jump table.)

  ;r23 bit 7 will be used for RAM enable in MBC3 because of the need to disable RAM when reading/writing RTC registers
  ; Hmmm... might need to set up some timers too to act as a fake RTC
  ;r23 low nibble will be the RAM/RTC bank number. Need to save this for later when using INTRAMEN. 
  ldi   r28, 0x00 ; Sets bank 0 by default. The C setup code already pulled SRAM_CS hi
  // Because I don't use RAM in MBC modes (generally), I will use RAM for RTC values so I can use an indirect load instruction (from Y register)
  clr   r29; CLR Y hi byte
  
MBC3_CLK:; Wait for a CLK pulse
  sbis  _SFR_IO_ADDR(EIFR), INT0; See if the interrupt flag is set in EIFR, if so, continue, else loop
  rjmp  MBC3_CLK

/*  ; These NOPs are required because ADDR15 is set AFTER all of the other address bits. Min 5 cycles after the rjmp above
  nop
  nop
  nop ; For my fake RTC, put an LD instruction here and replace the NOPs (ldd r24, Y)
  nop*/
  ; For this revision, data is (mostly) good to read after OUT. Use the 5 NOP cycles 
  out   _SFR_IO_ADDR(EIFR), r0; EIFR, clear the interrupt flag - do this after disabling the RAM chip, because bus contention might ensue otherwise
  in    ZL, _SFR_IO_ADDR(PIND)
  andi  ZL, 0xF0
  ijmp
  

  

; **********************************************************************************
MBC_mode_MBC2:

; **********************************************************************************
; ************* MBC1 16M8k MODE JUMP TABLE *****************************************
; **********************************************************************************
; ************************* READ JUMP TABLE (Read is low)
.section .MBC1_16M_0000,"ax",@progbits ;0x4000 ROM bank 0
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC1_16M_CLK; Finished setting up the lines for a read :)

.section .MBC1_16M_0010,"ax",@progbits ;0x4020 ROM bank 0
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC1_16M_CLK; Finished setting up the lines for a read :);

.section .MBC1_16M_0100,"ax",@progbits ;0x4040 ROM bank XX
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC1_16M_CLK; Finished setting up the lines for a read :)

.section .MBC1_16M_0110,"ax",@progbits ;0x4060 ROM bank XX
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC1_16M_CLK; Finished setting up the lines for a read :)

.section .MBC1_16M_1000,"ax",@progbits ;0x4080 Reserved. Do nothing
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_1010,"ax",@progbits ;0x40A0 RAM read
;  in    r16, _SFR_IO_ADDR(PINA)
;  andi  r16, 0xC0; RAM_/EN & GB_/CS flag pin check <- consider changing this to a register
;  brbc  1, MBC1_16M_RAM_DIS; branch without enabling RAM if zero flag is not set in SREG
;  cbi   _SFR_IO_ADDR(PORTA), PA2;SRAM_CS_PIN - Pull SRAM_CS low
;MBC1_16M_RAM_DIS:
  ; don't think I need to do anything with access controlled in hardware (OR gate)
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_1100,"ax",@progbits ;0x40C0 Reserved. Do nothing
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_1110,"ax",@progbits ;0x40E0 Reserved. Do nothing
  rjmp  MBC1_16M_CLK;

; ************************* WRITE JUMP TABLE (Read is hi)
.section .MBC1_16M_0001,"ax",@progbits ;0x4010 Ram enable/disable
  nop; This many NOPS may be needed to as data isn't good until WR is asserted
  nop;
  in    r17, _SFR_IO_ADDR(PINB)
  andi  r17, 0x1F ; Mask D0-4. I don't want to add this, but we'll see if MBC data is corrupted otherwise
  cpi   r17, 0x0A; 0x0A is an enable command
  brne  MBC1_16M_RAM_FLG_DIS
  cbi   _SFR_IO_ADDR(PORTA), PA7;
  rjmp  MBC1_16M_CLK
MBC1_16M_RAM_FLG_DIS:
  sbi   _SFR_IO_ADDR(PORTA), PA7
  rjmp  MBC1_16M_CLK

.section .MBC1_16M_0011,"ax",@progbits ;0x4030 ROM bank numbers
  andi  r22, 0xE0;REG_MA14_18, MA19_21_MASK ; Mask only hi bits to ditch the old low bits
  nop; Need a NOP here for DATA to be valid... without this, this is 13-16 cycles in, so it should be safe...
  in    r17, _SFR_IO_ADDR(PINB);GB_DAT_REG, GB_DAT_R ;Read in PINB data for use in each write mode
  andi  r17, 0x1F ; Mask D0-4. I don't want to add this, but we'll see if MBC data is corrupted otherwise
  tst   r17
  brne  MBC1_16M_NZERO
  inc   r17
MBC1_16M_NZERO:
  or    r22, r17;REG_MA14_18, GB_DAT_REG
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the MA mask value. Probably don't need to OUT this here for a WRITE command
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_0101,"ax",@progbits ;0x4050 ROM bank number high bits
  andi  r22, 0x1F ;REG_MA14_21, 0x1F; CLR the older data
  nop; This many NOPS may be needed to as data isn't good until WR is asserted
  sbic  _SFR_IO_ADDR(PINB), 0
  sbr   r22, 5;REG_MA14_21, 5; Set bit 5, MA19
  sbic  _SFR_IO_ADDR(PINB), 1
  sbr   r22, 6;REG_MA14_21, 6; Set bit 6, MA20
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER ;R_MA14_21, REG_MA14_21
  rjmp MBC1_16M_CLK;

.section .MBC1_16M_0111,"ax",@progbits ;0x4070 ROM/RAM mode select
  ; Figure out if we need a mode change
  nop; This many NOPS may be needed to as data isn't good until WR is asserted
  nop;
  sbis  _SFR_IO_ADDR(PINB), 0 ; Skip if Data bit 0 is set - we need a mode change
  rjmp  MBC1_16M_CLK; Stay in our own lane

/*  sbrc  r22, 5; lo hi bit (decipher that!) ; I took this part out because TCAGBD said this is zeroed. Should I save it in case this mode comes back?
  sbi   _SFR_IO_ADDR(PORTA), 0
  sbrc  r22, 6; hi hi bit
  sbi   _SFR_IO_ADDR(PORTA), 1*/

  andi  r22, 0x1F;REG_MA14_21, remove the high bits as we're mode switching
  out   _SFR_IO_ADDR(PORTC), r22; Update the MA address bits before we switch modes
  rjmp  MBC1_4M_CLK ; Mode switch jump!

.section .MBC1_16M_1001,"ax",@progbits ;0x4090 Reserved. Do nothing
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_1011,"ax",@progbits ;0x40B0 RAM write
;  in    r16, _SFR_IO_ADDR(PINA)
;  andi  r16, 0xC0; RAM_/EN & GB_/CS flag pin check <- consider changing this to a register
;  brbc  1, MBC1_16M_RAM_DIS2; branch without enabling RAM if zero flag is not set in SREG
;  cbi   _SFR_IO_ADDR(PORTA), PA2;SRAM_CS_PIN - Pull SRAM_CS low
;MBC1_16M_RAM_DIS2:
; May not need anything here with SRAM being controlled by the OR gate (and INTERNAL_RAM pin)
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_1101,"ax",@progbits ;0x40D0 Reserved. Do nothing
  rjmp  MBC1_16M_CLK;

.section .MBC1_16M_1111,"ax",@progbits ;0x40F0 Reserved. Do nothing
  rjmp  MBC1_16M_CLK;

; **********************************************************************************
; ************* MBC1 2M32k MODE JUMP TABLE *****************************************
; **********************************************************************************
; ************************* READ JUMP TABLE (Read is low)
.section .MBC1_4M_0000,"ax",@progbits ;0x4100 ROM bank 0
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_0010,"ax",@progbits ;0x4120 ROM bank 0
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_0100,"ax",@progbits ;0x4140 ROM bank XX
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_0110,"ax",@progbits ;0x4160 ROM bank XX
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1000,"ax",@progbits ;0x4180 Reserved. Do nothing
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1010,"ax",@progbits ;0x41A0 RAM read
  ; Do nothing, this is handled in hardware (OR gate)
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1100,"ax",@progbits ;0x41C0 Reserved. Do nothing
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1110,"ax",@progbits ;0x41E0 Reserved. Do nothing
  rjmp  MBC1_4M_CLK;

; ************************* WRITE JUMP TABLE (Read is hi)
.section .MBC1_4M_0001,"ax",@progbits ;0x4110 Ram enable
  nop; This many NOPS may be needed to as data isn't good until WR is asserted
  nop;
  in    r17, _SFR_IO_ADDR(PINB)
  andi  r17, 0x1F ; Mask D0-4. I don't want to add this, but we'll see if MBC data is corrupted otherwise
  cpi   r17, 0x0A; 0x0A is an enable command
  brne  MBC1_4M_RAM_FLG_DIS
  cbi   _SFR_IO_ADDR(PORTA), PA7;
  rjmp  MBC1_4M_CLK
MBC1_4M_RAM_FLG_DIS:
  sbi   _SFR_IO_ADDR(PORTA), PA7
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_0011,"ax",@progbits ;0x4130 ROM bank numbers
  andi  r22, 0xE0;REG_MA14_18, MA19_21_MASK ; Mask only hi bits to ditch the old low bits
  nop; Need a NOP here for DATA to be valid... without this, this is 13-16 cycles in, so it should be safe...
  in    r17, _SFR_IO_ADDR(PINB);GB_DAT_REG, GB_DAT_R ;Read in PINB data for use in each write mode
  andi  r17, 0x1F ; Mask D0-4. I don't want to add this, but we'll see if MBC data is corrupted otherwise
  tst   r17
  brne  MBC1_4M_NZERO
  inc   r17
MBC1_4M_NZERO:
  or    r22, r17;REG_MA14_18, GB_DAT_REG
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the MA mask value. Probably don't need to OUT this here for a WRITE command
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_0101,"ax",@progbits ;0x4150 RAM bank number
  in    r18, _SFR_IO_ADDR(PORTA) ;Prep for changing the RAM bank number
  andi  r18, 0xFC ; CLR the old bits
  in    r17, _SFR_IO_ADDR(PINB);Read in PINB data
  andi  r17, 0x03; Mask the RAM bank number (in case of rubbish in)
  or    r18, r17 ; Put it together and whaddaya got?
  out   _SFR_IO_ADDR(PORTA), r18 
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_0111,"ax",@progbits ;0x4170 ROM/RAM mode select 
;*** Something I hadn't considered here, what if these bits should be translated across ROM and RAM when switching? ***
; This might be the way the chip operates, rather than discarding them when clearing before a mode switch. Should I even
; clear the old bits? Guess time will tell what I should do here
  in    r18, _SFR_IO_ADDR(PORTA); This many NOPS may be needed to as data isn't good until WR is asserted
  andi  r18, 0xFC; In preparation for mode switch, get rid of the RAM bank bits
  sbic  _SFR_IO_ADDR(PINB), 0 ; Skip if Data bit 0 is set - we need a mode change
  rjmp  MBC1_4M_CLK; Stay in our own lane

/*  sbic  _SFR_IO_ADDR(PORTA), 0 ; As for 16M mode, I took this out because TCAGBD said this goes back to 0. Should I save it in case this mode comes back?
  sbr   r22, 5;REG_MA14_21, 5; Set bit 5, MA19
  sbic  _SFR_IO_ADDR(PORTA), 1
  sbr   r22, 6;REG_MA14_21, 6; Set bit 6, MA20
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER ;R_MA14_21, REG_MA14_21*/

  out   _SFR_IO_ADDR(PORTA), r18; Update the RAM bank bits before we switch modes
  rjmp  MBC1_16M_CLK ; Mode switch jump!

.section .MBC1_4M_1001,"ax",@progbits ;0x4190 Reserved. Do nothing
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1011,"ax",@progbits ;0x41B0 RAM write
  ; Do nothing, this is handled in hardware (OR gate)
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1101,"ax",@progbits ;0x41D0 Reserved. Do nothing
  rjmp  MBC1_4M_CLK;

.section .MBC1_4M_1111,"ax",@progbits ;0x41F0 Reserved. Do nothing
  rjmp  MBC1_4M_CLK;

; **********************************************************************************
; ************* MBC5 MODE JUMP TABLE *********************************************** 8 cycles to JMP table
; **********************************************************************************
; ************************* READ JUMP TABLE (Read is low)
.section .MBC5_0000,"ax",@progbits ;0x4200
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  sbrs  r25, 2; Check CLK pin that was read pre-jmp. If it's low, then we're in DSM
  ldi   ZH, r31_MBC5_DSM
  rjmp  MBC5_CLK;

.section .MBC5_0010,"ax",@progbits ;0x4220 ROM bank 0
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  sbrs  r25, 2; Check CLK pin that was read pre-jmp. If it's low, then we're in DSM
  ldi   ZH, r31_MBC5_DSM
  rjmp  MBC5_CLK;

.section .MBC5_0100,"ax",@progbits ;0x4240 ROM bank XX
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  sbrs  r25, 2; Check CLK pin that was read pre-jmp. If it's low, then we're in DSM
  ldi   ZH, r31_MBC5_DSM
  rjmp  MBC5_CLK;

.section .MBC5_0110,"ax",@progbits ;0x4260 ROM bank XX
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  sbrs  r25, 2; Check CLK pin that was read pre-jmp. If it's low, then we're in DSM
  ldi   ZH, r31_MBC5_DSM
  rjmp  MBC5_CLK;

.section .MBC5_1000,"ax",@progbits ;0x4280 Reserved. Do nothing
out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC5_CLK;

.section .MBC5_1010,"ax",@progbits ;0x42A0 RAM read
  ; Do nothing. RAM is handled in HW :)
out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC5_CLK;

.section .MBC5_1100,"ax",@progbits ;0x42C0 Reserved. Do nothing
out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC5_CLK;

.section .MBC5_1110,"ax",@progbits ;0x42E0 Reserved. Do nothing
out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC5_CLK;

; ************************* WRITE JUMP TABLE (Read is hi)
.section .MBC5_0001,"ax",@progbits ;0x4210 Ram enable
MBC5_0001:
; Shouldn't beed an extra NOP like MBC3 because this mode already has an extra instruction before IJMP
  nop; This many NOPS may be needed to as data isn't good until WR is asserted
  nop;

  in    r17, _SFR_IO_ADDR(PINB)
  cpi   r17, 0x0A; 0x0A is an enable command
  brne  MBC5_RAM_FLG_DIS
  cbi   _SFR_IO_ADDR(PORTA), PA7;
  rjmp  MBC5_CLK
MBC5_RAM_FLG_DIS:
  sbi   _SFR_IO_ADDR(PORTA), PA7
  rjmp  MBC5_CLK;

.section .MBC5_0011,"ax",@progbits ;0x4230 ROM bank numbers
; Technically I cheat in this loop. This address space is actually broken up into lo8 and hi1 across 
; 0x20-2F and 0x30-3F, instead of using a block address of 0x20-3F like in other MBC modes. The only 
; reason I am ignoring this is because I don't have 8MB of flash, and therefore should have absolutely
; no reason for anyone to access the 9th bit in the upper half of the address space! If they did, I 
; would need to connect ADDR12 to RUMBLE (and change this pin to be an input) and monitor it in this loop
MBC5_0011:
  nop
  nop; Need a NOP here for DATA to be valid... without this, this is 13-16 cycles in, so it should be safe...

  in    r22, _SFR_IO_ADDR(PINB); 8 lo bits of ROM address. Easy! Too bad you can't copy port to port
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the MA mask value. Probably don't need to OUT this here for a WRITE command
  rjmp  MBC5_CLK;

.section .MBC5_0101,"ax",@progbits ;0x4250 RAM bank number  - Rumble doesn't need to be masked anymore - rewired the prototype board.
MBC5_0101:
  in    r18, _SFR_IO_ADDR(PORTA) ;Prep for changing the RAM bank number
  andi  r18, 0xF4 ; CLR the old bits

  in    r17, _SFR_IO_ADDR(PINB);Read in PINB data
  andi  r17, 0x0B; Mask the RAM bank number (because we share a two bit value on the PORT)
  or    r18, r17 ; Put it together and whaddaya got?
  out   _SFR_IO_ADDR(PORTA), r18 
  rjmp  MBC5_CLK;

.section .MBC5_0111,"ax",@progbits ;0x4270 Not used in MBC5
  rjmp  MBC5_CLK;

.section .MBC5_1001,"ax",@progbits ;0x4290 Reserved. Do nothing
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip if set
  rjmp  MBC5_0001;

  rjmp  MBC5_CLK;

.section .MBC5_1011,"ax",@progbits ;0x42B0 RAM write
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip if set
  rjmp  MBC5_0011;
  rjmp  MBC5_CLK;

.section .MBC5_1101,"ax",@progbits ;0x42D0 Reserved. Do nothing
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip if cleared
  rjmp  MBC5_0101;
  rjmp  MBC5_CLK;

.section .MBC5_1111,"ax",@progbits ;0x42F0 Reserved. Do nothing
  rjmp  MBC5_CLK;

; **********************************************************************************
; ************* MBC5 DOUBLE SPEED MODE JUMP TABLE **********************************
; **********************************************************************************
; ************************* READ JUMP TABLE (Read is low)
.section .MBC5_DSM_0000,"ax",@progbits ;0x4400 ROM bank 0
  sbic  _SFR_IO_ADDR(PIND), 2; If CLK is hi here, then we're not in DSM. Change the jump table!
  ldi   ZH, r31_MBC5
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_0010,"ax",@progbits ;0x4420 ROM bank 0
  sbic  _SFR_IO_ADDR(PIND), 2; If CLK is hi here, then we're not in DSM. Change the jump table!
  ldi   ZH, r31_MBC5
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_0100,"ax",@progbits ;0x4440 ROM bank XX
  sbic  _SFR_IO_ADDR(PIND), 2; If CLK is hi here, then we're not in DSM. Change the jump table!
  ldi   ZH, r31_MBC5
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_0110,"ax",@progbits ;0x4460 ROM bank XX
  sbic  _SFR_IO_ADDR(PIND), 2; If CLK is hi here, then we're not in DSM. Change the jump table!
  ldi   ZH, r31_MBC5
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1000,"ax",@progbits ;0x4480 Reserved. Do nothing
  in    r23, _SFR_IO_ADDR(PORTA) ; Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  ori   r23, 0x80; I think I overstep CLK boundaries by adding a fourth instruction here, but I need it...
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1010,"ax",@progbits ;0x44A0 RAM read
  ; For DSM, this is in HW, which gives me 3 free cycles which I'll use to set up RAM bank number and enable/disable
  ; I'm assuming that if you enable RAM, you will read or write to it before disabling it again, which should result
  ; in calling this code. May be a flawed assumption which could break things.
  in    r23, _SFR_IO_ADDR(PORTA)
  mov   r24, r23 ; Make a copy
  cbr   r24, 0x03; CLR the RAM bank bits in prep for ORing it later
  ori   r23, 0x80; I think I overstep CLK boundaries by adding a fourth instruction here, but I need it...
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1100,"ax",@progbits ;0x44C0 Reserved. Do nothing
  in    r24, _SFR_IO_ADDR(PORTA); Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  cbr   r24, 0x03; CLR the RAM bank bits in prep for ORing it later
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1110,"ax",@progbits ;0x44E0 Reserved. Do nothing
  in    r23, _SFR_IO_ADDR(PORTA) ; Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  ori   r23, 0x80; I think I overstep CLK boundaries by adding a fourth instruction here, but I need it...
  rjmp  MBC5_DSM_CLK;

; ************************* WRITE JUMP TABLE (Read is hi)
.section .MBC5_DSM_0001,"ax",@progbits ;0x4410 Ram enable
; No NOPs in DSM. Assume WR is asserted already
; We're going to play dangerously for DSM. Just check for 0x08 instead of 0x0A to save cycles
  out   _SFR_IO_ADDR(PORTA), r23  ; Immediately disable RAM (should have already OR'd this)
  sbic  _SFR_IO_ADDR(PINB), 3     ; Check for 0x08
  cbi   _SFR_IO_ADDR(PORTA), PA7  ; Eugh, this is a 2 cycle instruction, it may break things...
  rjmp  MBC5_DSM_CLK

.section .MBC5_DSM_0011,"ax",@progbits ;0x4430 ROM bank numbers
; Technically I cheat in this loop. This address space is actually broken up into lo8 and hi1 across 
; 0x20-2F and 0x30-3F, instead of using a block address of 0x20-3F like in other MBC modes. The only 
; reason I am ignoring this is because I don't have 8MB of flash, and therefore should have absolutely
; no reason for anyone to access the 9th bit in the upper half of the address space! If they did, I 
; would need to connect ADDR12 to RUMBLE (and change this pin to be an input) and monitor it in this loop
  in    r22, _SFR_IO_ADDR(PINB); 8 lo bits of ROM address. Easy! Too bad you can't copy port to port
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the MA mask value. Probably don't need to OUT this here for a WRITE command
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_0101,"ax",@progbits ;0x4450 RAM bank number  - NEED TO CHANGE THE PINOUT TO LET THE RUMBLE PIN BE MASKED EASILY!
  in    r17, _SFR_IO_ADDR(PINB);Read in PINB data, no mask because speed... Very dangerous.
  or    r24, r17 ; This is dangerous if RAM bank is changed consecutively without reading or writing, because r24 isn't cleared!
  out   _SFR_IO_ADDR(PORTA), r24
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_0111,"ax",@progbits ;0x4470 Not used in MBC5
  in    r24, _SFR_IO_ADDR(PORTA); Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  cbr   r24, 0x03; CLR the RAM bank bits in prep for ORing it later
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1001,"ax",@progbits ;0x4490 Reserved. Do nothing
  in    r23, _SFR_IO_ADDR(PORTA) ; Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  ori   r23, 0x80; I think I overstep CLK boundaries by adding a fourth instruction here, but I need it...
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1011,"ax",@progbits ;0x44B0 RAM write
  ; For DSM, this is in HW, which gives me 3 free cycles which I'll use to set up RAM bank number and enable/disable
  ; I'm assuming that if you enable RAM, you will read or write to it before disabling it again, which should result
  ; in calling this code. May be a flawed assumption which could break things.
  in    r23, _SFR_IO_ADDR(PORTA)
  mov   r24, r23 ; Make a copy
  cbr   r24, 0x03; CLR the RAM bank bits in prep for ORing it later
  ori   r23, 0x80; I think I overstep CLK boundaries by adding a fourth instruction here, but I need it...
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1101,"ax",@progbits ;0x44D0 Reserved. Do nothing
  in    r24, _SFR_IO_ADDR(PORTA); Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  cbr   r24, 0x03; CLR the RAM bank bits in prep for ORing it later
  rjmp  MBC5_DSM_CLK;

.section .MBC5_DSM_1111,"ax",@progbits ;0x44F0 Reserved. Do nothing
  in    r24, _SFR_IO_ADDR(PORTA); Doing stuff in here to try and compensate for a lack of cycles elsewhere. Don't even know if this is useful.
  cbr   r24, 0x03; CLR the RAM bank bits in prep for ORing it later
  rjmp  MBC5_DSM_CLK;

; **********************************************************************************
; ************* MBC3 MODE JUMP TABLE ***********************************************
; **********************************************************************************
; ************************* READ JUMP TABLE (Read is low)
.section .MBC3_0000,"ax",@progbits ;0x4300 ROM bank 0
MBC3_0000:
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC3_CLK;

.section .MBC3_0010,"ax",@progbits ;0x4320 ROM bank 0
MBC3_0010:
  out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC3_CLK;

.section .MBC3_0100,"ax",@progbits ;0x4340 ROM bank XX
MBC3_0100:
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC3_CLK;

.section .MBC3_0110,"ax",@progbits ;0x4360 ROM bank XX
MBC3_0110:
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC3_CLK;

.section .MBC3_1000,"ax",@progbits ;0x4380 Reserved. Do nothing
/*sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip is set
rjmp  MBC3_0000 ; ROM bank 0*/ ;This mode is reserved. It shouldn't care if we change things
out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
  rjmp  MBC3_CLK;

.section .MBC3_1010,"ax",@progbits ;0x43A0 RAM/RTC read
sbic  _SFR_IO_ADDR(PIND), 7; Check A15, skip is cleared - by combining these sections, I save 2 cycles for read
rjmp  MBC3_RTC_STUFF ; ROM bank 0 ; This might cause us problems. Any chance we can swap it with RAM/RTC read?
out   _SFR_IO_ADDR(PORTC), r1; Set PORTC to 0x00
rjmp  MBC3_CLK;

MBC3_RTC_STUFF:
  ; RAM is enabled in hardware. Only RTC code needs be here
  cpi   r28, 0x88; If r28 is >0x88, RTC is enabled and bank 8-C are selected. Push out an RTC value
  brlo  MBC3_NOT_RTC_RD
; In the pre-IJMP loop, do an *ldd indirect from Y and *out to PORTB. Then just make PORTB output here for the read cycle
  ; DDRB output would be hard though. How do I make them an input again without stuffing the timing?
; out   _SFR_IO_ADDR(DDRB), r2 ;-> pre-load r2 with 0xFF to make all outputs
; nop
; nop
; nop
; nop
; out DDRB lo
; out PORTB, r1; Make 0 again, disable all pullups
  rjmp  MBC3_CLK;
MBC3_NOT_RTC_RD:
; This loop is way too slow for a read cycle. Not sure if I can do this. Need some smarter thinking
; out PORTB, r1; Make 0 again, disable all pullups - negates the effect of pre-IJMP code - may need this as a consistent reset before MBC3_CLK
  rjmp  MBC3_CLK;

.section .MBC3_1100,"ax",@progbits ;0x43C0 Reserved. Do nothing
/*sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip is set
rjmp  MBC3_0100 ; ROM bank XX*/ ;This mode is reserved. It shouldn't care if we change things
out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC3_CLK;

.section .MBC3_1110,"ax",@progbits ;0x43E0 Reserved. Do nothing
/*sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip is set
rjmp  MBC3_0110 ; ROM bank XX*/ ;This mode is reserved. It shouldn't care if we change things
out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the stored MA14_18_21 value (r22)
  rjmp  MBC3_CLK;

; ************************* WRITE JUMP TABLE (Read is hi)
.section .MBC3_0001,"ax",@progbits ;0x4310 RAM/RTC enable
MBC3_0001:
nop
  nop
  nop
  in    r17, _SFR_IO_ADDR(PINB) ; No need to mask in MBC3 mode as it has D0-7 inputs
  cpi   r17, 0x0A; 0x0A is an enable command
  brne  MBC3_RAM_FLG_DIS
  sbr   r28, 7; Enable RAM/RTC
  cbi   _SFR_IO_ADDR(PORTA), PA7;
  rjmp  MBC3_CLK
MBC3_RAM_FLG_DIS:
  cbr   r28, 7; Disable RTC
  sbi   _SFR_IO_ADDR(PORTA), PA7; Disable RAM
  rjmp  MBC3_CLK;

.section .MBC3_0011,"ax",@progbits ;0x4330 ROM bank numbers
MBC3_0011:
  nop;
  nop; Need a NOP here for DATA to be valid... without this, this is 13-16 cycles in, so it should be safe...
nop
  in    r22, _SFR_IO_ADDR(PINB); We can read r22 directly in MBC3 mode because there are no other hi ROM bits to worry about
  andi  r22, 0x7F ; Mask D0-6. I don't want to add this, but I haven't tested if MBC data is corrupted otherwise
  tst   r22
  brne  MBC3_NZERO
  inc   r22; MBC3 still doesn't put bank 0 at location 1.
MBC3_NZERO:
  out   _SFR_IO_ADDR(PORTC), MA14_18_21_REGISTER; Set PORTC to the MA mask value. Probably don't need to OUT this here for a WRITE command
  rjmp  MBC3_CLK;

.section .MBC3_0101,"ax",@progbits ;0x4350 RAM/RTC bank number
MBC3_0101:
  in    r18, _SFR_IO_ADDR(PORTA) ;Prep for changing the RAM bank number
  andi  r18, 0xFC ; CLR the old bits on the PORT 
nop
  in    r17, _SFR_IO_ADDR(PINB);Read in PINB data
  andi  r28, 0x80; Clear the old register value, except for RTC enable/disable
  or    r28, r17; Save the bank number for later - Need this to stop READ and WRITE commands from reading RTC values when physical RAM is mapped
  
  cpi   r17, 0x04 ; Valid physical RAM banks are 0-3. Anything higher disables physical RAM
  brsh  MBC3_RTC_REGS; Branch if same or higher
  ; It's a physical bank here 
  or    r18, r17 ; Put the new value into PORTA temp value
  out   _SFR_IO_ADDR(PORTA), r18 ; Push PORTA back out
  sbrc  r28, 7; If bit 7 in r28 is cleared, then RAM isn't enabled. Don't enable RAM!
  cbi   _SFR_IO_ADDR(PORTA), PA7;
  rjmp  MBC3_CLK;
MBC3_RTC_REGS:
  sbi   _SFR_IO_ADDR(PORTA), PA7; Disable physical RAM
  rjmp  MBC3_CLK;

.section .MBC3_0111,"ax",@progbits ;0x4370 Latch RTC data
MBC3_0111:
  ; If I do some fancy timer stuff, I'll latch some data here... some day
  rjmp  MBC3_CLK;

.section .MBC3_1001,"ax",@progbits ;0x4390 Reserved. Do nothing
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip is set
rjmp  MBC3_0001 ; Jump to RAM/RTC enable

  rjmp  MBC3_CLK;

.section .MBC3_1011,"ax",@progbits ;0x43B0 RAM/RTC write
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip is set
rjmp  MBC3_0011 ; Jump to ROM bank numbers

  ; RAM is handled in hardware. RTC is in software, but only if Write is enabled!
  cpi   r28, 0x88; If r28 is >=0x88, RTC is enabled and bank 8-C are selected. Do an RTC write
  brlo  MBC3_NO_INTRAMEN_WR
; Do the RTC write routine here
MBC3_NO_INTRAMEN_WR:
  rjmp  MBC3_CLK;

.section .MBC3_1101,"ax",@progbits ;0x43D0 Reserved. Do nothing
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip if set
rjmp  MBC3_0101 ; Jump to RAM/RTC bank number. Or do it here and make the above SBIC

  rjmp  MBC3_CLK;

.section .MBC3_1111,"ax",@progbits ;0x43F0 Reserved. Do nothing
sbis  _SFR_IO_ADDR(PIND), 7; Check A15, skip is set
rjmp  MBC3_0111 ; Jump to Latch RTC. Or do it here and make the above SBIC

  rjmp  MBC3_CLK;
