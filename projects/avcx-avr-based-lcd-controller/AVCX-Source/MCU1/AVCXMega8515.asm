;
; Software based on the Grand Prize winning Entry for the
; Circuit Cellar 2004 AVR Contest
; Project AVCX: http://www.circuitcellar.com/avr2004/grand.html
;
; It is re-used here with permission from Circuit Cellar
;
; Modified 2008 by Jan de Rie
;
; Besides a bug fix, the runtime design of MCU2 was completely redesigned
; because the original design was very timing sensitive. For example, a large
; delay was used in the MCU1 line scanning timing to accomodate MCU2
;
; In general, the only time MCU2 can access the video memory is between
; Int6 and Int7 events (the RAM window). Incoming graphics commands are
; stored in a buffer which is processed during the RAM window, when it
; is safe to write to the video memory).
;
; 1. the incoming bytes buffer was only 256 bytes but the CLRSCR command
;	 for example takes 57 mSecs to execute, which equates to 663 incoming
;	 bytes at 115,200 bps. A lot of these bytes could be discarded.
;	 The full bitmap commands expect 76,800 data bytes.
;	 With multibyte commands, the state machine would get out of sync
;
; 2. The graphics command processing during Int6 was not always completed
;	 before Int7 occured, requiring a clever and tricky recovery mechanism
;	 to make sure the previous processing was completed on the next Int6.
;	 This mechanism required that no functions were called while doing
;	 graphic commands processing.
;
; 3. While graphics processing occured during the Int6 ISR, no incoming bytes
;	 were accepted.
;
; 4. the RAM window is fairly long at the end of each screen. From lines 160 to 200,
;	 the RAM window is open all the time because this is "overscan" time. Even though
;	 the HSYNC signal is still properly issues, no data bytes are used by the LCD.
;	 In the original design, the RAM window was closed at line 185, probably because
;	 too many serial input bytes were lost. (the serial input was disabled during
;	 the RAM window in the original design)
;
; The new design uses a 2K input buffer, plenty for the most demanding input.
; It also accepts incoming characters at all times. There are a few short moments
; where interrupts are disabled, mostly to ensure atomic operations on 16 bit pointers
; or when toggling the memory controls
;
; Graphics processing is now done in the forever loop, not in an ISR. The forever loop gets interrupted
; when an Int7 occurs. The Int7 ISR releases the memory bus, sets the state to FROZEN
; and waits until the state changes to UNFREEZE. This state change occurs when Int6 hits.
; Int6 does nothing but changing the state. Int7 grabs the memory bus again and finishes,
; allowing the forever loop to continue graphics processing at the point it was interrupted.
; In summary, when the forever loops runs, it is safe to access the video memory. When it is
; not safe, the code loops inside the Int7 ISR. Incoming bytes are accepted at all times.
; The performance of MCU2, specially wrt incoming bytes, is now guaranteed, independant of
; the specific timing of Int6 and Int7 (as long as they alternate)
; Well almost. There were also screen artifacts at the beginning of each line when MCU2 accessed
; the memory. These artefacts were caused by the fact the MCU2 didn't release the memory bus fast
; enough, causing bus contention. This was fixed by sending the RAM_STOP signal from MCU1 to MCU2
; earlier (from Dot clock 262 to Dot clock 252). This however leaves only 8 dot clocks (16 cycles)
; for MCU2 to access the memory, which is not enough, causing even a 3K input buffer to overflow
; So some freeze (12 uSecs, value = 15, original value was 100) is still present.
;
; During the VSYNC period, the RAM window is now closed at line 200, allowing for a longer RAM window.
;
; Part of the reason why INT7 is slow to respond on MCU2 is the fact that SerialInput interrupts are
; on. When the INT7 interrupt occurs while a serial input is active, the INT7 interrupt is pended
; and won't get triggered until the serial input interrupt is completed. For this reason, the
; Serial Input ISR processing must be a short as possible.
; The same artifacts were showing in the original design even though the serial input interrupt was disabled.
; Need more testing, e.g. by disabling serial input interrupts while the RAM window is open. TODO
; On MCU1, the timing of INT7 is now advanced to minimize this effect, making the RAM window very short
; specially without the Freeze hack.
;
; The modified source files result in an LCD refresh rate of 66.6 Hz, double the original design,
; with little, if any, serial input rate limitations.
;
; The modifications to this source file are conditionalized on JDRMODS
;
.INCLUDE "m8515def.inc"

.equ BIT_0 = 1
.equ BIT_1 = 2
.equ BIT_2 = 4
.equ BIT_3 = 8
.equ BIT_4 = 16
.equ BIT_5 = 32
.equ BIT_6 = 64
.equ BIT_7 = 128

.equ LCD       = PORTD 
.equ LCD_DIR   = DDRD 
.equ HSYNC     = 7
.equ VSYNC     = 6
.equ SHUT_DOWN = 3
.equ CLOCK_ON  = 5

.equ RAM       = PORTB 
.equ RAM_DIR   = DDRB
.equ RAM_PIN   = PINB 
.equ OE        = 0
.equ RAM_START = 2
.equ RAM_STOP  = 1

.equ ADDR_LO     = PORTA
.equ ADDR_LO_DIR = DDRA
.equ ADDR_HI     = PORTC
.equ ADDR_HI_DIR = DDRC

#define JDRMODS 1

#if JDRMODS
.equ FREEZE_TIME = 15
#else
.equ FREEZE_TIME = 100
#endif


.def tmp = r16
.def addr_lsb = r18
.def addr_msb = r19
.def pwrup_state = r20

.CSEG

.ORG 0x000
  rjmp Reset

.ORG 0x012

Reset:
  cli
  ldi tmp, 0
  out SPH, tmp
  ldi tmp, 255
  out SPL, tmp

  ; Release RAM Control to ATmega128
  ; ================================
  ; Disconnect 8515 address bus
  ldi tmp, 0
  out ADDR_LO_DIR, tmp
  out ADDR_HI_DIR, tmp
  out ADDR_LO, tmp
  out ADDR_HI, tmp
  
  ldi tmp, 1 | 1<<RAM_START | 1<<RAM_STOP
  out RAM, tmp

  ldi tmp, 1<<RAM_START | 1<<RAM_STOP
  out RAM_DIR, tmp

  ldi tmp, 1<<HSYNC | 1<<VSYNC | 1<<CLOCK_ON
  out LCD, tmp

  ldi tmp, 1<<HSYNC | 1<<VSYNC | 1<<SHUT_DOWN | 1<<CLOCK_ON
  out LCD_DIR, tmp

  ldi r24, LOW(FREEZE_TIME)
  ldi r25, HIGH(FREEZE_TIME)
  clr addr_lsb
  clr addr_msb
  clr pwrup_state
 ; sbi LCD, SHUT_DOWN


WaitForMega128:
  sbic RAM_PIN, OE
  rjmp WaitForMega128

  ldi tmp, 0xFF
  out ADDR_LO, addr_lsb
  out ADDR_HI, addr_msb
  out ADDR_HI_DIR, tmp
  out ADDR_LO_DIR, tmp

  nop
; Send 240 Dots
; =============
LCD_Line_Start:
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  out ADDR_LO, addr_lsb
  inc addr_lsb
  ;  240 LCD dots
LCD_HSync:
  ; Modify Address
  clr addr_lsb
  inc addr_msb
  ;  241 LCD dots
  ; End of frame?
  cpi addr_msb, 160
  ;  241.5 LCD dots
  brsh LCD_VSync
  ;  242 LCD dots
  ; Release RAM Control to ATmega128
  ; ================================
  ; Disconnect 8515 address bus
  ldi tmp, 0
  out ADDR_LO_DIR, tmp
  ;  243 LCD dots
  out ADDR_HI_DIR, tmp
  ; 243.5
  ; Send RAM_START Interrupt to ATmega128
  cbi RAM, RAM_START
  ;  244.5 LCD dots
  nop
  ;  245 LCD dots
  sbi RAM, RAM_START
  ;  246 LCD dots
;
; The window between RAM_START and RAM_STOP is too short for the Atmega128,
; even with the new paradigm  (and the serial port set to 115,200 bps)
; Add a bunch of wait states here,with the LCD Dot Clock stopped.
;
Loop_01:
  cbi LCD, CLOCK_ON ; Freeze On
  ;  247 LCD dots
  sbiw r25:r24, 1
  ;  248 LCD dots
  breq Get_Ram
  rjmp Loop_01

Get_Ram:
  ldi r24, LOW(FREEZE_TIME)
  ldi r25, HIGH(FREEZE_TIME)
  sbi  LCD, CLOCK_ON ; Freeze Off
  ;  247 LCD dots (according to LCD)
;
#if JDRMODS
;
; The original code showed interference at the beginning of each line
; caused by the fact the MCU2 was not releasing the memory bus fast enough.
; Advance the RAM_STOP signal significantly (from dot 262 to dot 252)
;
  rcall Wait_4_dots
  ; 251 LCD dots
  ; Reclaim RAM Control from ATmega128
  ; ==================================
  ; Send RAM_STOP Interrupt to ATmega128
  cbi RAM, RAM_STOP
  ; 252 LCD dots
  ldi tmp, 0xFF
  nop  
  ; 253 LCD dots
  sbi RAM, RAM_STOP
  ; 254 LCD dots
  ; Wait to activate HSync
  ; ================================
  rcall Wait_4_dots
  ; 258 LCD dots
  nop
  ; 258.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 259.5 LCD dots
  nop
  ; 260 LCD dots 
  rcall Wait_4_dots
  ; 264 LCD dots 
#else
  ; Wait 16th dots to activate HSync
  ; ================================
  rcall Wait_8_dots
  ; 255 LCD dots
  nop
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  ; 257 LCD dots 
  rcall Wait_4_dots
  ; 261 LCD dots 
  ; Reclaim RAM Control from ATmega128
  ; ==================================
  ; Send RAM_STOP Interrupt to ATmega128
  cbi RAM, RAM_STOP
  ; 262 LCD dots
  ldi tmp, 0xFF
  nop  
  ; 263 LCD dots
  sbi RAM, RAM_STOP
  ; 264 LCD dots
#endif
  out ADDR_HI, addr_msb
  out ADDR_LO, addr_lsb
  ; 265 LCD dots
  nop
  nop
  ; 266 LCD dots
  nop
  nop
  ; 267 LCD dots
  nop
  nop
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  ; Connect 8515 address bus
  out ADDR_HI_DIR, tmp
  out ADDR_LO_DIR, tmp
  ; 270 LCD dots
  nop
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start


LCD_VSync:
  ;  242.5 LCD dots
  nop
  ;  243 LCD dots
  cpi addr_msb, 160
  brne WaitForLine_180

  ;  244 LCD dots
  ; Release RAM Control to ATmega128
  ; ================================
  ; Disconnect 8515 address bus
  ldi tmp, 0
  out ADDR_LO_DIR, tmp
  ;  245 LCD dots
  out ADDR_HI_DIR, tmp
  out ADDR_LO, tmp
  ;  246 LCD dots
  out ADDR_HI, tmp
  ;  246.5 LCD dots
  ; Send RAM_START Interrupt to ATmega128
  cbi RAM, RAM_START
  ;  247.5 LCD dots
  nop
  ;  248 LCD dots
  sbi RAM, RAM_START
  ;  249 LCD dots
  rcall Wait_6_dots
  ; 255 LCD dots
  nop
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  nop
  nop
  ; 258 LCD dots
  rcall Wait_10_dots
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  nop
  nop
  ; 270 LCD dots
  nop
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start


WaitForLine_180:
  ;  244.5 LCD dots
  nop
  ;  245 LCD dots
  cpi addr_msb, 180
  breq Line_180
  ;  246 LCD dots
  brpl WaitForLine_185
  ;  246.5 LCD dots
  nop
  ;  247 LCD dots
  rcall Wait_8_dots
  ; 255 LCD dots
  nop
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  nop
  nop
  ; 258 LCD dots
  rcall Wait_10_dots
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  nop
  nop
  ; 270 LCD dots
  nop
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start

Line_180:
  ;  246.5 LCD dots
  nop
  ;  247 LCD dots
  nop
  nop
  ;  248 LCD dots
  nop
  nop
  ;  249 LCD dots
  rcall Wait_6_dots
  ; 255 LCD dots
  nop 
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  nop
  nop
  ; 258 LCD dots
  rcall Wait_10_dots
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  nop
  nop
  ; 270 LCD dots
  ; Activate VSync
  ; ==============
  cbi LCD, VSYNC
  ;  271 LCD dots
  rjmp LCD_Line_Start


WaitForLine_185:
  ;  247 LCD dots
  cpi addr_msb, 185
  breq Line_185
  ;  248 LCD dots
  brpl WaitForLine_200
  ;  248.5 LCD dots
  nop
  ;  249 LCD dots
  rcall Wait_6_dots
  ; 255 LCD dots
  nop
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  nop
  nop
  ; 258 LCD dots
  rcall Wait_10_dots
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  nop
  nop
  ; 270 LCD dots
  nop
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start

Line_185:
  ;  248.5 LCD dots
  nop
  ;  249 LCD dots
#if JDRMODS
  nop
  nop
  nop
  nop
  nop
  nop
  ; 252
#else
  cbi RAM, RAM_STOP
  ; 250
  nop
  nop
  ; 251
  sbi RAM, RAM_STOP
  ; 252
#endif
  nop
  nop
  ; 253
  nop
  nop
  ; 254
  nop
  nop
  ; 255 LCD dots
  nop
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  nop
  nop
  ; 258 LCD dots
  rcall Wait_10_dots
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  nop
  ; 269.5 LCD dots
  ; Turn off VSync
  ; ==============
  sbi LCD, VSYNC
  ; 270.5 LCD dots
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start


WaitForLine_200:
  ; 249 LCD dots
  cpi addr_msb, 200
  breq Line_200
  ; 250 LCD dots
  rcall Wait_4_dots
  ; 254 LCD dots
  nop
  nop
  ; 255 LCD dots
  nop
  ; 255.5 LCD dots
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  nop
  nop
  nop
  ; 258 LCD dots
  rcall Wait_10_dots
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  nop
  nop
  ; 270 LCD dots
  nop
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start



Line_200:
  ; 250.5 LCD dots
  nop
  ; 251 LCD dots
  ; Reset Address
  clr addr_msb
  clr addr_lsb
  ; 252
#if JDRMODS
  ; Reclaim RAM Control from ATmega128
  ; ==================================
  ; Send RAM_STOP Interrupt to ATmega128
  cbi RAM, RAM_STOP
  ;253
  nop
  nop
  ; 254
  sbi RAM, RAM_STOP
  ; 255
#else
  ; Reclaim RAM Control from ATmega128
  ; ==================================
  ; Send RAM_STOP Interrupt to ATmega128
;  cbi RAM, RAM_STOP
     nop
     nop
  ; 253
  nop
  nop
  ; 254
;  sbi RAM, RAM_STOP
     nop
     nop
  ; 255
#endif
  nop
  ; 255.5
  ; Activate HSync
  ; ==============
  cbi LCD, HSYNC
  ; 256.5 LCD dots
  
  rcall Wait_4_dots
  ; 260.5 LCD dots
  nop
  nop
  ; 261.5 LCD dots
  nop  
  ; 262 LCD dots
  nop
  nop
  ; 263 LCD dots
  cpi pwrup_state, 3
  brsh Turn_On_LCD
  ; 264 LCD dots
  inc pwrup_state
  ; 264.5 LCD dots
  nop
  ; 265 LCD dots
  rjmp Dots_266

Turn_On_LCD:
  ; 264.5 LCD dots
  nop
  ; 265 LCD dots
  sbi LCD, SHUT_DOWN

Dots_266:
  ; 266 LCD dots
  out ADDR_HI, addr_msb
  out ADDR_LO, addr_lsb
  ; 267 LCD dots
  ldi tmp, 0xFF
  nop
  ; 268 LCD dots
  ; Turn off HSync
  ; ==============
  sbi LCD, HSYNC
  ; 269 LCD dots
  ; Connect 8515 address bus
  out ADDR_HI_DIR, tmp
  out ADDR_LO_DIR, tmp
  ; 270 LCD dots
  nop
  nop
  ; 271 LCD dots
  rjmp LCD_Line_Start

  
; #########
; ## End ##
; #########
  rjmp reset


  
; ##################
; ## Soubroutines ##
; ##################
Wait_4_dots:
  ; + 1.5 LCD dots
  nop
  ; + 2 LCD dots
  ret 

Wait_6_dots:
  ; + 1.5 LCD dots
  nop
  ; + 2 LCD dots
  nop
  nop
  ; + 3 LCD dots
  nop
  nop
  ; + 4 LCD dots
  ret 

Wait_8_dots:
  ; + 1.5 LCD dots
  nop
  ; + 2 LCD dots
  rcall Wait_4_dots
  ; + 6 LCD dots
  ret 

Wait_10_dots:
  ; + 1.5 LCD dots
  nop
  ; + 2 LCD dots
  rcall Wait_6_dots
  ; + 8 LCD dots
  ret 

Wait_12_dots:
  ; + 1.5 LCD dots
  nop
  ; + 2 LCD dots
  rcall Wait_8_dots
  ; + 10 LCD dots
  ret 

