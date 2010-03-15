;----------------------------------------------------------------------
; asmTWIClock.asm - Prototype clock using TWI bus for communication
;                   with a PCF8583 time clock and SAA1064 LED driver. 
;
;   In normal clock mode the MCU reads the real time from the PCF8583
;   and displays it on four seven segment display digits.  One push
;   button allows the momentary display of the month and day.  Another
;   allows the momentary display of minutes and seconds.  
;
;   When a third button is pushed the clock is put in a set time mode.
;   As this button is successively pushed the user can change the year
;   code, month, day, hours, minutes.  A decimal point is illuminated
;   to show which data can be changed by the two other buttons, which
;   in this mode serve as Up and Down buttons.  On the final push of
;   the Set Time button the clock is restarted with the seconds set
;   to zero.  The year code is the year modulo 4, so a 0 represents a
;   leap year.
; 
; Hardware:
;   ATtiny2313 with 1 MHz internal clock (8 MHz internal RC oscillator
;   with the divide-by-8 flag set) on STK500 board.  Fuse settings are
;   Low 0x64, High 0x9F, Extended 0xFF.
;
;   Homebrew prototyping board holding:
;     - SAA1064 LED driver with four 7 segment digit displays and two
;       2N4401 npn transistors
;     - PCF8583 real-time clock with 32.768 clock crystal and trim cap
;   Both of these are wired as per their data sheets.  The board also
;   has two 4.7K pullup resistors for the SDA and SCL lines.
;   
; AVR pin assignments:
;   PORTD0:1 - D0 = SDA, D1 = SCL
;   PORTB0:2 - Three pushbutton inputs
;     B0 = Set time (cycles thru year code, month, day, hours, mins)
;     B1 = Show mins and secs (momentary) / DOWN in set time mode
;     B2 = Show month and day (momentary) / UP in set time mode
;               
; Notes:
;   The Atmel A300 Application Note code is used for bit-bang TWI 
;   operations with the ATtiny2313.  There is a typo in the original
;   file which has been corrected.  The original comments for these
;   TWI functions have been retained in this code.  There is no
;   TWI error checking for this simple system.
;
;   For a stand-alone clock, a better choice for a real time clock
;   would be a model with battery backup if the display were to be
;   turned off.  Also, the SAA1064 is a 5 volt device, and one might
;   prefer a 3.3v system with a different display setup.
;        
;   Since only 5 I/O pins are used a smaller chip with 2K of program
;   flash RAM and some data SRAM could be used.  Also, TWI could be
;   done using the Universal Serial Interface as per the Atmel
;   A310 Application Note.
;
; Andy Palm
; 2010.03.14
;
;----------------------------------------------------------------------
;------------------ includes, defines, equates ------------------------
.nolist
.include "tn2313def.inc"
.list

.equ  FREQ            = 1000000     ; Clock frequency in Hz
.equ  CYCLES_PER_20MS = (FREQ / 50) ; Clock cycles in 10 ms

.equ  b_dir     = 0           ; transfer direction bit num in TWIadr
.equ  TWIrd     = 1           ; Set bit for TWI read
.equ  TWIwr     = 0           ; Clear bit for TWI write

.equ  TWI_DDR   = DDRD        ; TWI port
.equ  TWI_PIN   = PIND
.equ  TWI_PORT  = PORTD
.equ  SDAP      = 0           ; SDA port pin number
.equ  SCLP      = 1           ; SCL port pin number

.equ  PB_DDR      = DDRB      ; Push button port
.equ  PB_PORT     = PORTB
.equ  PB_PIN      = PINB
.equ  SET_TIME_P  = 0         ; Set time PB pin number
.equ  DOWN_P      = 1         ; DOWN PB pin number
.equ  SHOW_SECS_P = 1
.equ  UP_P        = 2         ; UP PB pin number
.equ  SHOW_DATE_P = 2         ; Show date PB pin number

.equ  SAA1064_ADDR  = 0x70    ; SAA1064 shifted slave address
.equ  SAA1064_LGT   = 6       ; Number of bytes to send SAA1064

.equ  DOT     = 0x80          ; To add decimal point to LED display

.equ  PCF8583_ADDR  = 0xA0    ; PCF8583 shifted slave address

.equ  PCF8583_CNTL  = 0x00    ; Word addr of control byte
.equ  PCF8583_CSCS  = 0x01    ; Word addr of centi-seconds
.equ  PCF8583_SECS  = 0x02    ; Word addr of seconds byte
.equ  PCF8583_MINS  = 0x03    ; Word addr of minutes byte
.equ  PCF8583_HRS   = 0x04    ; Word addr of hours byte
.equ  PCF8583_DAYS  = 0x05    ; Word addr of year/day byte
.equ  PCF8583_MTHS  = 0x06    ; Word addr of day-of-week/month byte

.equ  PCF8583_HRS_MASK  = 0x3F  ; Mask for reading hours
.equ  PCF8583_DAYS_MASK = 0x3F  ; Mask for reading day
.equ  PCF8583_MTHS_MASK = 0x1F  ; Mask for reading months

.equ  PCF8583_SCFG_BIT  = 0x80  ; For switching stop count flag

.equ  PCF8583_YRS_MAX = 3   ; Max year code
.equ  PCF8583_YRS_MIN = 0   ; Min year code

.equ  SHOW_TIME_STATE = 0   ; Clock state machine states
.equ  SHOW_DATE_STATE = 1
.equ  SHOW_SECS_STATE = 2
.equ  SET_TIME_STATE  = 3

; Register variables
.def  tempa     = r16       ; Temporary variable
.def  tempb     = r17       ; Temporary variable
.def  tempc     = r18       ; Temporary variable

.def  TWIdata   = r19       ; TWI data transfer register
.def  TWIadr    = r20       ; TWI address and direction register
.def  TWIstat   = r21       ; TWI bus status register

.def  month     = r22       ; For maximum day calculaton
.def  year      = r23
.def  day_max   = r24

.def  Clk_State = r25       ; Clock state machine state

;------------------ SRAM assignments ----------------------------------
.dseg
.org SRAM_START

SAA1064_Write_Bytes:
SAA1064_Inst:   .byte   1   ; SAA1064 instruction byte 0x00
SAA1064_Cntl:   .byte   1   ; SAA1064 control byte 0x17 (3 ma, multi)
SAA1064_Dig1:   .byte   1   ; SAA1064 Digit 1 pattern byte
SAA1064_Dig2:   .byte   1   ; SAA1064 Digit 2 pattern byte
SAA1064_Dig3:   .byte   1   ; SAA1064 Digit 3 pattern byte
SAA1064_Dig4:   .byte   1   ; SAA1064 Digit 4 pattern byte

;------------------ macros --------------------------------------------
; Check status of momentary push button with debounce on down stroke
;
; Use:  Check_Mom_PB bit
;
; where:  bit = input port pin/bit number to check
;
; If button is down T bit is set, if not down T bit is clear
;
.macro  Check_Mom_PB
  clt                           ; Clear T bit for button up
  sbic  PB_PIN, @0              ; Check button pin (0=down, 1=up)
  rjmp  Check_Mom_PB_Done       ; Button up, quit
  rcall Wait_20ms               ; Button down, wait debounce interval
  sbic  PB_PIN, @0              ; Check button pin again
  rjmp  Check_Mom_PB_Done       ; Button released at end of interval, quit
  set                           ; Button still down, set T bit
Check_Mom_PB_Done:
.endmacro

;----------------------------------------------------------------------
; Check status of push button with debounce on both down and up stroke
;
; Use:  Check_PB bit
;
; where:  bit = input port pin/bit number to check
;
; If button has been pushed and released T bit is set, 
; if not T bit is clear
;
.macro  Check_PB
  clt                           ; Clear T bit for button up
  sbic  PB_PIN, @0              ; Check button pin (0=down, 1=up)
  rjmp  Check_PB_Done           ; Button up, quit
  rcall Wait_20ms               ; Button down, wait debounce interval
  sbic  PB_PIN, @0              ; Check button pin again
  rjmp  Check_PB_Done           ; Button released at end of interval, quit
  sbis  PB_PIN, @0              ; Wait for release
  rjmp  PC-1
  rcall Wait_20ms               ; Button released, wait debounce interval
  sbis  PB_PIN, @0              ; If button still released, done
  rjmp  PC-4                    ; If release not sustained, redo
  set                           ; Done, set T bit                         
Check_PB_Done:
.endmacro

;------------------ interrupt vectors ---------------------------------
.cseg
.org 0x0000
  rjmp  Reset       ; Reset service
  reti              ; INT0 external interrupt
  reti              ; INT1 external interrupt
  reti              ; TIMER1 CAPT capture event
  reti              ; TIMER1 COMPA compare match A
  reti              ; TIMER1 OVF overflow
  reti              ; TIMER0 OVF overflow
  reti              ; USART, RXC rx complete
  reti              ; USART, UDRE data register empty
  reti              ; USART, TXC tx complete
  reti              ; ANA_COMP analog comparator
  reti              ; PCINT pin change
  reti              ; TIMER1 COMPB compare match B
  reti              ; TIMER0 COMPA compare match A
  reti              ; TIMER0 COMPB compare match B
  reti              ; USI START USI start condition
  reti              ; USI OVERFLOW USI overflow
  reti              ; EE READY EEPROM ready
  reti              ; WDT OVERFLOW Watchdog timer overflow

;------------------ device initialization -----------------------------
Reset:
  ldi   tempa, RAMEND               ; Set up stack
  out   SPL, tempa

; Port setup
  clr   tempa           ; Set both ports to open collector       
  out   TWI_DDR, tempa
  out   TWI_PORT, tempa
  out   PB_DDR, tempa
  out   PB_PORT, tempa

;  rcall  TWI_init    ; initialize TWI interface

;------------------ main program --------------------------------------
Main:

  rcall Init_SAA1064_Bytes  ; Set up SAA1064 write bytes

  rcall Stop_Count

; Set time to 23:59
  ldi   tempa, PCF8583_HRS
  ldi   tempb, 0x23
  rcall PCF8583_Put

  ldi   tempa, PCF8583_MINS
  ldi   tempb, 0x59
  rcall PCF8583_Put

; Set date to Saturday (6), 03/13, 2010 = 2 mod 4
  ldi   tempa, PCF8583_DAYS
  ldi   tempb, (2 << 6) | 0x13  ; ((Year mod 4) << 6) | Day
  rcall PCF8583_Put

  ldi   tempa, PCF8583_MTHS
  ldi   tempb, (6 << 5) | 0x03  ; ((Day of week) << 5) | Month
  rcall PCF8583_Put

  rcall Start_Count

; Initialize state machine
  ldi   Clk_State, SHOW_TIME_STATE

Main_Loop:

; Check Show Date button
  Check_Mom_PB  SHOW_DATE_P
  brtc  No_SD_Push                  ; Not pushed, quit
  ldi   Clk_State, SHOW_DATE_STATE  ; Set to Show Date state
No_SD_Push:

; Check Show Seconds button
  Check_Mom_PB  SHOW_SECS_P
  brtc  No_SS_Push                  ; Not pushed, quit
  ldi   Clk_State, SHOW_SECS_STATE  ; Set to Show Seconds state
No_SS_Push:

; Check Set Time button
  Check_PB  SET_TIME_P
  brtc  No_ST_Push                  ; Not pushed, quit
  ldi   Clk_State, SET_TIME_STATE   ; Set to Set Time state
No_ST_Push:

; Enter clock state machine
; Show time as normal state
  cpi   Clk_State, SHOW_TIME_STATE
  brne  Load_Date
  rcall Get_Time            ; Get clock hrs and minutes
  rcall SAA1064_Put         ; Send digits to SAA1064
  rjmp  Clk_State_Done

; Show date during momentary push
Load_Date:
  cpi   Clk_State, SHOW_DATE_STATE
  brne  Show_Seconds
  rcall Get_Date            ; Get clock month and day
  rcall Add_DP_Digit3       ; Add decimal point separator
  rcall SAA1064_Put         ; Send data to SAA1064
  ldi   Clk_State, SHOW_TIME_STATE  ; Reset state to show time
  rjmp  Clk_State_Done

; Show minutes and seconds during momentary push
Show_Seconds:
  cpi   Clk_State, SHOW_SECS_STATE
  brne  Set_Time
  rcall Get_Seconds         ; Get clock month and day
  rcall SAA1064_Put         ; Send data to SAA1064
  ldi   Clk_State, SHOW_TIME_STATE  ; Reset state to show time
  rjmp  Clk_State_Done

; Set time
Set_Time:
  cpi   Clk_State, SET_TIME_STATE
  brne  Clk_State_Done

  rcall Stop_Count          ; Stop clock

; Set year code (year mod 4, 0 = Leap year)
  rcall Set_Year

; Set month
  rcall Set_Month

; Set day
  rcall Set_Day

; Set hour
  rcall Set_Hour

; Set minute
  rcall Set_Minute

; Zero seconds and centi-seconds
  ldi   tempb, 0x00
  ldi   tempa, PCF8583_CSCS
  rcall PCF8583_Put
  ldi   tempa, PCF8583_SECS
  rcall PCF8583_Put

; Restart clock and set state machine to show time
  rcall Start_Count
  ldi   Clk_State, SHOW_TIME_STATE

Clk_State_Done:

  rjmp  Main_Loop

;------------------ interrupt service routines ------------------------

;------------------ subroutines ---------------------------------------
;----------------------------------------------------------------------
; Initialize SAA1064 write bytes in RAM
Init_SAA1064_Bytes:
  push  tempa

  ldi   ZH, HIGH(SAA1064_Write_Bytes)   ; Load address
  ldi   ZL, LOW(SAA1064_Write_Bytes)
  ldi   tempa, 0x00          ; Instruction byte (local addr) 0x00
  st    Z+, tempa
  ldi   tempa, 0x17          ; Control byte (3 ma, multiplexed)
  st    Z+, tempa
  ldi   tempa, 0x00          ; Digit 1 is blank
  st    Z+, tempa
  ldi   tempa, 0x00          ; Digit 2 is blank
  st    Z+, tempa
  ldi   tempa, 0x00          ; Digit 3 is blank
  st    Z+, tempa
  ldi   tempa, 0x00          ; Digit 4 is blank
  st    Z, tempa

  pop   tempa
  ret

;----------------------------------------------------------------------
; Send data in RAM locations to SAA1064 for display
SAA1064_Put:
  push  tempa

  ldi   TWIadr, SAA1064_ADDR + TWIwr  ; Set device address and R/W bit
  rcall TWI_start                     ; Send start cond and addr

  ldi   ZH, HIGH(SAA1064_Write_Bytes) ; Load RAM address of data
  ldi   ZL, LOW(SAA1064_Write_Bytes)
  ldi   tempa, SAA1064_LGT            ; Set loop counter
SAA1064_Put1:
  ld    TWIdata, Z+         ; Load data byte for transfer
  rcall TWI_do_transfer     ; Execute transfer
  dec   tempa
  brne  SAA1064_Put1

  rcall TWI_stop    ; Send stop condition

  pop   tempa
  ret

;----------------------------------------------------------------------
; Get real-time clock byte at word addr in tempa, return byte in tempa
PCF8583_Get:
  ldi   TWIadr, PCF8583_ADDR + TWIwr  ; Load clock addr with write bit
  rcall TWI_start                     ; Send start cond and addr

  mov   TWIdata, tempa                ; Load and send word addr
  rcall TWI_do_transfer

  ldi   TWIadr, PCF8583_ADDR + TWIrd  ; Load clock addr with read bit
  rcall TWI_rep_start                 ; Send repeat start cond and addr

  sec                   ; Set no ack (read is followed by a stop cond)
  rcall TWI_do_transfer               ; Read byte at word addr
  rcall TWI_stop                      ; Send stop cond - release bus

  mov   tempa, TWIdata
  ret

;----------------------------------------------------------------------
; Write single byte in tempb to real-time clock at word addr in tempa
PCF8583_Put:
  ldi   TWIadr, PCF8583_ADDR + TWIwr  ; Load clock addr with write bit
  rcall TWI_start                     ; Send start cond and addr

  mov   TWIdata, tempa                ; Load and send word addr
  rcall TWI_do_transfer

  mov   TWIdata, tempb                ; Load and send data byte
  rcall TWI_do_transfer

  rcall TWI_stop                      ; Send stop cond - release bus
  
  ret 

;----------------------------------------------------------------------
; Stop the clock
Stop_Count:
  push  tempa
  push  tempb

  ldi   tempa, PCF8583_CNTL           ; Get current control byte
  rcall PCF8583_Get
  ori   tempa, PCF8583_SCFG_BIT       ; Set stop count flag bit
  mov   tempb, tempa                  ; Load new byte
  ldi   tempa, PCF8583_CNTL
  rcall PCF8583_Put

  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Start the clock
Start_Count:
  push  tempa
  push  tempb

  ldi   tempa, PCF8583_CNTL           ; Get current control byte
  rcall PCF8583_Get
  andi  tempa, ~PCF8583_SCFG_BIT      ; Clear stop count flag bit
  mov   tempb, tempa                  ; Load new byte
  ldi   tempa, PCF8583_CNTL
  rcall PCF8583_Put

  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Get clock hrs and minutes, put bit pattern in RAM for display
Get_Time:
  push  tempa

  ldi   tempa, PCF8583_MINS ; Get clock data for low digits
  rcall PCF8583_Get         ; Data byte now in tempa
  rcall Load_Dig12_RAM

  ldi   tempa, PCF8583_HRS  ; Get clock data for high digits
  rcall PCF8583_Get         ; Data byte now in tempa
  andi  tempa, PCF8583_HRS_MASK ; Mask out AM/PM and format flags
  rcall Load_Dig34_RAM

  pop   tempa
  ret

;----------------------------------------------------------------------
; Get clock month and day, put bit pattern in RAM for display
Get_Date:
  push  tempa

  ldi   tempa, PCF8583_DAYS ; Get clock data for low digits
  rcall PCF8583_Get         ; Data byte now in tempa
  andi  tempa, PCF8583_DAYS_MASK ; Mask out year code
  rcall Load_Dig12_RAM

  ldi   tempa, PCF8583_MTHS  ; Get clock data for high digits
  rcall PCF8583_Get         ; Data byte now in tempa
  andi  tempa, PCF8583_MTHS_MASK ; Mask out day-of-week
  rcall Load_Dig34_RAM

  pop   tempa
  ret

;----------------------------------------------------------------------
; Get clock minutes and seconds, put bit pattern in RAM for display
Get_Seconds:
  push  tempa

  ldi   tempa, PCF8583_SECS ; Get clock data for low digits
  rcall PCF8583_Get         ; Data byte now in tempa
  rcall Load_Dig12_RAM

  ldi   tempa, PCF8583_MINS ; Get clock data for high digits
  rcall PCF8583_Get         ; Data byte now in tempa
  rcall Load_Dig34_RAM

  pop   tempa
  ret

;----------------------------------------------------------------------
; Set clock year code = Year mod 4 (0 = leap year)
Set_Year:
  push  tempa
  push  tempb
  push  tempc

  clr   tempa
  sts   SAA1064_Dig2, tempa ; Blank upper digits in display
  sts   SAA1064_Dig3, tempa 
  sts   SAA1064_Dig4, tempa
  ldi   tempa, PCF8583_DAYS ; Get year/date byte
  rcall PCF8583_Get         ; Byte now in tempa
  mov   tempb, tempa
  andi  tempb, PCF8583_DAYS_MASK ; Save days data
  lsr   tempa               ; Shift year code down
  lsr   tempa
  lsr   tempa
  lsr   tempa
  lsr   tempa
  lsr   tempa
  mov   tempc, tempa        ; Save year code

Set_Year_Display:
  rcall Get_7seg_Code       ; Get diplay pattern for year digit
  ori   tempa, DOT          ; Light decimal point
  sts   SAA1064_Dig1, tempa ; Put bit pattern for ones digit in RAM
  rcall SAA1064_Put

  Check_PB  UP_P            ; UP button pushed?
  brtc  Set_Year_Up_Done
  cpi   tempc, PCF8583_YRS_MAX
  breq  Set_Year_Up_Done
  inc   tempc               ; Increment year code if < max
Set_Year_Up_Done:
  Check_PB  DOWN_P          ; DOWN button pushed?
  brtc  Set_Year_Down_Done
  cpi   tempc, PCF8583_YRS_MIN
  breq  Set_Year_Down_Done
  dec   tempc               ; Decrement year code if > min
Set_Year_Down_Done:
  mov   tempa, tempc

; Save year code and exit if Set Time button pushed
  Check_PB  SET_TIME_P
  brtc  Set_Year_Display
  lsl   tempa
  lsl   tempa
  lsl   tempa
  lsl   tempa
  lsl   tempa
  lsl   tempa
  or    tempb, tempa        ; Recombine year and days data
  ldi   tempa, PCF8583_DAYS
  rcall PCF8583_Put

  pop   tempc
  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Set clock month
Set_Month:
  push  tempa
  push  tempb
  push  tempc

  ldi   tempa, PCF8583_DAYS ; Get day for low digits
  rcall PCF8583_Get         ; Data byte now in tempa
  andi  tempa, PCF8583_DAYS_MASK ; Mask out year code
  rcall Load_Dig12_RAM

  ldi   tempa, PCF8583_MTHS ; Get weekday/month byte
  rcall PCF8583_Get         ; Byte now in tempa
  mov   tempb, tempa
  andi  tempb, ~PCF8583_MTHS_MASK ; Save weekday code
  andi  tempa, PCF8583_MTHS_MASK ; Get month data
  mov   tempc, tempa        ; Save month data
Set_Month_Display:
  mov   tempa, tempc
  rcall Load_Dig34_RAM      ; Put month in RAM
  rcall Add_DP_Digit3       ; Add dot next to month
  rcall SAA1064_Put

  Check_PB  UP_P            ; UP button pushed?
  brtc  Set_Month_Up_Done
  cpi   tempc, 0x12
  breq  Set_Month_Up_Done
  rcall BCD_Incr            ; Increment month
Set_Month_Up_Done:
  Check_PB  DOWN_P          ; DOWN button pushed?
  brtc  Set_Month_Down_Done
  cpi   tempc, 0x01
  breq  Set_Month_Down_Done
  rcall BCD_Decr            ; Decrement month
Set_Month_Down_Done:

; Save month and exit if Set Time button pushed
  Check_PB  SET_TIME_P
  brtc  Set_Month_Display

  or    tempb, tempc        ; Recombine weekday and month data
  ldi   tempa, PCF8583_MTHS
  rcall PCF8583_Put

  pop   tempc
  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Set clock day
Set_Day:
  push  tempa
  push  tempb
  push  tempc

  ldi   tempa, PCF8583_MTHS ; Get month for high digits
  rcall PCF8583_Get         ; Byte now in tempa
  andi  tempa, PCF8583_MTHS_MASK ; Get month data
  mov   month, tempa        ; Save month data for max day calc
  rcall Load_Dig34_RAM

  ldi   tempa, PCF8583_DAYS ; Get days
  rcall PCF8583_Get         ; Data byte now in tempa
  mov   year, tempa         ; Save year code in MSB
  andi  year, ~PCF8583_DAYS_MASK
  andi  tempa, PCF8583_DAYS_MASK
  rcall Calc_Day_Max        ; Calc day max for this month/year
  mov   tempc, tempa        ; Save days
Set_Day_Display:
  mov   tempa, tempc
  rcall Load_Dig12_RAM      ; Put day in RAM
  rcall Add_DP_Digit1       ; Add dot next to hours
  rcall SAA1064_Put

  Check_PB  UP_P            ; UP button pushed?
  brtc  Set_Day_Up_Done
  cp    tempc, day_max
  breq  Set_Day_Up_Done
  rcall BCD_Incr            ; Increment month
Set_Day_Up_Done:
  Check_PB  DOWN_P          ; DOWN button pushed?
  brtc  Set_Day_Down_Done
  cpi   tempc, 0x01
  breq  Set_Day_Down_Done
  rcall BCD_Decr            ; Decrement month
Set_Day_Down_Done:

; Save day and exit if Set Time button pushed
  Check_PB  SET_TIME_P
  brtc  Set_Day_Display

  mov   tempb, tempc
  or    tempb, year         ; Restore year code
  ldi   tempa, PCF8583_DAYS
  rcall PCF8583_Put

  pop   tempc
  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Set clock hour
Set_Hour:
  push  tempa
  push  tempb
  push  tempc

  ldi   tempa, PCF8583_MINS ; Get minutes for low digits
  rcall PCF8583_Get         ; Byte now in tempa
  rcall Load_Dig12_RAM

  ldi   tempa, PCF8583_HRS  ; Get hours
  rcall PCF8583_Get         ; Data byte now in tempa
  mov   tempc, tempa        ; Save hours
Set_Hour_Display:
  mov   tempa, tempc
  rcall Load_Dig34_RAM      ; Put hours in RAM
  rcall Add_DP_Digit3       ; Add dot next to hours
  rcall SAA1064_Put

  Check_PB  UP_P            ; UP button pushed?
  brtc  Set_Hour_Up_Done
  cpi   tempc, 0x23
  breq  Set_Hour_Up_Done
  rcall BCD_Incr            ; Increment month
Set_Hour_Up_Done:
  Check_PB  DOWN_P          ; DOWN button pushed?
  brtc  Set_Hour_Down_Done
  cpi   tempc, 0x00
  breq  Set_Hour_Down_Done
  rcall BCD_Decr            ; Decrement month
Set_Hour_Down_Done:

; Save day and exit if Set Time button pushed
  Check_PB  SET_TIME_P
  brtc  Set_Hour_Display

  mov   tempb, tempc
  ldi   tempa, PCF8583_HRS
  rcall PCF8583_Put

  pop   tempc
  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Set clock minute
Set_Minute:
  push  tempa
  push  tempb
  push  tempc

  ldi   tempa, PCF8583_HRS ; Get minutes for low digits
  rcall PCF8583_Get         ; Byte now in tempa
  rcall Load_Dig34_RAM

  ldi   tempa, PCF8583_MINS ; Get hours
  rcall PCF8583_Get         ; Data byte now in tempa
  mov   tempc, tempa        ; Save hours
Set_Min_Display:
  mov   tempa, tempc
  rcall Load_Dig12_RAM      ; Put hours in RAM
  rcall Add_DP_Digit1       ; Add dot next to hours
  rcall SAA1064_Put

  Check_PB  UP_P            ; UP button pushed?
  brtc  Set_Min_Up_Done
  cpi   tempc, 0x59
  breq  Set_Min_Up_Done
  rcall BCD_Incr            ; Increment month
Set_Min_Up_Done:
  Check_PB  DOWN_P          ; DOWN button pushed?
  brtc  Set_Min_Down_Done
  cpi   tempc, 0x00
  breq  Set_Min_Down_Done
  rcall BCD_Decr            ; Decrement month
Set_Min_Down_Done:

; Save day and exit if Set Time button pushed
  Check_PB  SET_TIME_P
  brtc  Set_Min_Display

  mov   tempb, tempc
  ldi   tempa, PCF8583_MINS
  rcall PCF8583_Put

  pop   tempc
  pop   tempb
  pop   tempa
  ret

;----------------------------------------------------------------------
; Calculate maximum day for current month and year, adjust day value
; in tempa if too high for current month
Calc_Day_Max:
  
  ldi   day_max, 0x30         ; Months with 30 days
  cpi   month, 0x04
  breq  Calc_Day_Max1
  cpi   month, 0x06
  breq  Calc_Day_Max1
  cpi   month, 0x09
  breq  Calc_Day_Max1
  cpi   month, 0x11
  breq  Calc_Day_Max1

  ldi   day_max, 0x31
  cpi   month, 0x02           ; February?
  brne  Calc_Day_Max1         ; No, month has 31 days
  ldi   day_max, 0x28         ; Yes, set days for February
  tst   year                  ; Leap year (year = 0)?
  brne  Calc_Day_Max1         ; No, 28 days in month
  ldi   day_max, 0x29         ; Yes, 29 days in month
Calc_Day_Max1:

  cp    day_max, tempa        ; Current day max < day?
  brge  Calc_Day_Max2         ; No, done
  mov   tempa, day_max        ; Yes, set day equal to day max
Calc_Day_Max2:

  ret

;----------------------------------------------------------------------
; Load LED bit patterns for BCD digits in tempa to RAM for Digits 1, 2
Load_Dig12_RAM:
  push  tempb

  mov   tempb, tempa        ; Save byte
  andi  tempa, 0x0F         ; Mask upper nibble to get ones digit
  rcall Get_7seg_Code       ; Convert digit in tempa to LED bit pattern
  sts   SAA1064_Dig1, tempa ; Put bit pattern for ones digit in RAM
  mov   tempa, tempb        ; Get back original byte
  swap  tempa               ; Swap nibbles
  andi  tempa, 0x0F         ; Mask upper nibble to get tens digit
  rcall Get_7seg_Code       ; Convert digit in tempa to LED bit pattern
  sts   SAA1064_Dig2, tempa ; Put bit pattern for tens digit in RAM

  pop   tempb
  ret

;----------------------------------------------------------------------
; Load LED bit patterns for BCD digits in tempa to RAM for Digits 3, 4
Load_Dig34_RAM:
  push  tempb

  mov   tempb, tempa        ; Save byte
  andi  tempa, 0x0F         ; Mask upper nibble to get ones digit
  rcall Get_7seg_Code       ; Convert digit in tempa to LED bit pattern
  sts   SAA1064_Dig3, tempa ; Put bit pattern for ones digit in RAM
  mov   tempa, tempb        ; Get back original byte
  swap  tempa               ; Swap nibbles
  andi  tempa, 0x0F         ; Mask upper nibble to get tens digit
  rcall Get_7seg_Code       ; Convert digit in tempa to LED bit pattern
  sts   SAA1064_Dig4, tempa ; Put bit pattern for tens digit in RAM

  pop   tempb
  ret

;----------------------------------------------------------------------
; Add decimal point to Digit 1 display
Add_DP_Digit1:
  push  tempa

  lds   tempa, SAA1064_Dig1
  ori   tempa, DOT
  sts   SAA1064_Dig1, tempa

  pop   tempa
  ret

;----------------------------------------------------------------------
; Add decimal point to Digit 3 display
Add_DP_Digit3:
  push  tempa

  lds   tempa, SAA1064_Dig3
  ori   tempa, DOT
  sts   SAA1064_Dig3, tempa

  pop   tempa
  ret

;----------------------------------------------------------------------
; Get 7-segment LED bit pattern for digit in tempa
Get_7seg_Code:
  push  tempb                   ; Save value in tempb

  clr   tempb
  ldi   ZL, LOW(2*Seg_Code_Table)    ; Get segment display codes
  ldi   ZH, HIGH(2*Seg_Code_Table)   ; from table in prgm memory
  add   ZL, tempa               ; Add offset to base address
  adc   ZH, tempb
  lpm   tempa, Z                ; tempa now contains LED bit pattern

  pop   tempb                   ; Restore tempb
  ret

;----------------------------------------------------------------------
; Wait about 20 ms
Wait_20ms:
  ldi   XL, LOW(CYCLES_PER_20MS / 4)
  ldi   XH, HIGH(CYCLES_PER_20MS / 4)
  sbiw  X, 1                    ; 2 cycles
  brne  PC-1                    ; 2 cycles if branches
  ret                           

;----------------------------------------------------------------------
; Increment (packed) BCD value in tempc
BCD_Incr:
  push  tempb

  inc   tempc
  mov   tempb, tempc
  andi  tempb, 0x0F
  cpi   tempb, 0x0A
  brne  BCD_Incr_Done
  subi  tempc, 0x0A
  subi  tempc, -0x10
BCD_Incr_Done:

  pop   tempb
  ret

;----------------------------------------------------------------------
; Decrement (packed) BCD value in tempc
BCD_Decr:
  push  tempb

  dec   tempc
  mov   tempb, tempc
  andi  tempb, 0x0F
  cpi   tempb, 0x0F
  brne  BCD_Decr_Done
  andi  tempc, 0xF9
BCD_Decr_Done:

  pop   tempb
  ret

;----------------------------------------------------------------------

;***************************************************************************
;* FUNCTION
;*  TWI_hp_delay
;*  TWI_qp_delay
;* DESCRIPTION
;*  hp - half TWI clock period delay (normal: 5.0us / fast: 1.3us)
;*  qp - quarter TWI clock period delay (normal: 2.5us / fast: 0.6us)
;*  SEE DOCUMENTATION !!!
;* USAGE
;*  no parameters
;* RETURN
;*  none
;*
;***************************************************************************

;TWI_hp_delay:
;  ldi  TWIdelay,2
;TWI_hp_delay_loop:
;  dec  TWIdelay
;  brne  TWI_hp_delay_loop
;  ret
;
;TWI_qp_delay:
;  ldi  TWIdelay,1  
;TWI_qp_delay_loop:
;  dec  TWIdelay
;  brne  TWI_qp_delay_loop
;  ret

TWI_hp_delay:     ; Single return statement as per documentation
  ret             ; for 1 MHz clock
TWI_qp_delay:
  ret

;***************************************************************************
;* FUNCTION
;*  TWI_rep_start
;* DESCRIPTION
;*  Assert repeated start condition and sends slave address.
;* USAGE
;*  TWIadr - Contains the slave address and transfer direction.
;* RETURN
;*  Carry flag - Cleared if a slave responds to the address.
;* NOTE
;*  IMPORTANT! : This funtion must be directly followed by TWI_start.
;*
;***************************************************************************
TWI_rep_start:
  sbi   TWI_DDR,SCLP         ; force SCL low
  cbi   TWI_DDR,SDAP         ; release SDA
  rcall TWI_hp_delay         ; half period delay
  cbi   TWI_DDR,SCLP         ; release SCL
  rcall TWI_qp_delay         ; quarter period delay

;***************************************************************************
;* FUNCTION
;*  TWI_start
;* DESCRIPTION
;*  Generates start condition and sends slave address.
;* USAGE
;*  TWIadr - Contains the slave address and transfer direction.
;* RETURN
;*  Carry flag - Cleared if a slave responds to the address.
;* NOTE
;*  IMPORTANT! : This funtion must be directly followed by TWI_write.
;*
;***************************************************************************
TWI_start:        
  mov   TWIdata,TWIadr      ; copy address to transmitt register
  sbi   TWI_DDR,SDAP        ; force SDA low
  rcall TWI_qp_delay        ; quarter period delay

;***************************************************************************
;* FUNCTION
;*  TWI_write
;* DESCRIPTION
;*  Writes data (one byte) to the TWI bus. Also used for sending
;*  the address.
;* USAGE
;*  TWIdata - Contains data to be transmitted.
;* RETURN
;*  Carry flag - Set if the slave respond transfer.
;* NOTE
;*  IMPORTANT! : This funtion must be directly followed by TWI_get_ack.
;*
;***************************************************************************
TWI_write:
  sec                        ; set carry flag
  rol   TWIdata              ; shift in carry and out bit one
  rjmp  TWI_write_first
TWI_write_bit:
  lsl   TWIdata              ; if transmit register empty
TWI_write_first:
  breq  TWI_get_ack          ;  goto get acknowledge
  sbi   TWI_DDR,SCLP         ; force SCL low

  brcc  TWI_write_low        ; if bit high
  nop                        ;  (equalize number of cycles)
  cbi   TWI_DDR,SDAP         ;  release SDA
  rjmp  TWI_write_high
TWI_write_low:               ; else
  sbi   TWI_DDR,SDAP         ;  force SDA low
  rjmp  TWI_write_high       ;  (equalize number of cycles)
TWI_write_high:
  rcall TWI_hp_delay         ; half period delay
  cbi   TWI_DDR,SCLP         ; release SCL
  rcall TWI_hp_delay         ; half period delay

  rjmp  TWI_write_bit

;***************************************************************************
;* FUNCTION
;*  TWI_get_ack
;* DESCRIPTION
;*  Get slave acknowledge response.
;* USAGE
;*  (used only by TWI_write in this version)
;* RETURN
;*  Carry flag - Cleared if a slave responds to a request.
;*
;***************************************************************************
TWI_get_ack:
  sbi   TWI_DDR,SCLP         ; force SCL low
  cbi   TWI_DDR,SDAP         ; release SDA
  rcall TWI_hp_delay         ; half period delay
  cbi   TWI_DDR,SCLP         ; release SCL

TWI_get_ack_wait:
  sbis  TWI_PIN,SCLP         ; wait SCL high 
                             ;(In case wait states are inserted)
  rjmp  TWI_get_ack_wait

  clc                        ; clear carry flag
  sbic  TWI_PIN,SDAP         ; if SDA is high
  sec                        ;  set carry flag
  rcall TWI_hp_delay         ; half period delay
  ret

;***************************************************************************
;* FUNCTION
;*  TWI_do_transfer
;* DESCRIPTION
;*  Executes a transfer on bus. This is only a combination of TWI_read
;*  and TWI_write for convenience.
;* USAGE
;*  TWIadr - Must have the same direction as when TWI_start was called.
;*  see TWI_read and TWI_write for more information.
;* RETURN
;*  (depends on type of transfer, read or write)
;* NOTE
;*  IMPORTANT! : This funtion must be directly followed by TWI_read.
;*
;***************************************************************************
TWI_do_transfer:
  sbrs  TWIadr,b_dir         ; if dir = write
  rjmp  TWI_write            ;  goto write data

;***************************************************************************
;* FUNCTION
;*  TWI_read
;* DESCRIPTION
;*  Reads data (one byte) from the TWI bus.
;* USAGE
;*  Carry flag -   If set no acknowledge is given to the slave
;*      indicating last read operation before a STOP.
;*      If cleared acknowledge is given to the slave
;*      indicating more data.
;* RETURN
;*  TWIdata - Contains received data.
;* NOTE
;*  IMPORTANT! : This funtion must be directly followed by TWI_put_ack.
;*
;***************************************************************************
TWI_read:
  rol   TWIstat              ; store acknowledge
                             ; (used by TWI_put_ack)
  ldi   TWIdata,0x01         ; data = 0x01
TWI_read_bit:                ; do
  sbi   TWI_DDR,SCLP         ;  force SCL low
  rcall TWI_hp_delay         ;  half period delay

  cbi   TWI_DDR,SCLP         ;  release SCL
  rcall TWI_hp_delay         ;  half period delay

  clc                        ;  clear carry flag
  sbic  TWI_PIN,SDAP         ;  if SDA is high
  sec                        ;  set carry flag

  rol   TWIdata              ; store data bit
  brcc  TWI_read_bit         ; while receive register not full

;***************************************************************************
;* FUNCTION
;*  TWI_put_ack
;* DESCRIPTION
;*  Put acknowledge.
;* USAGE
;*  (used only by TWI_read in this version)
;* RETURN
;*  none
;*
;***************************************************************************
TWI_put_ack:
  sbi   TWI_DDR,SCLP         ; force SCL low

  ror   TWIstat              ; get status bit
  brcc  TWI_put_ack_low      ; if bit low goto assert low
  cbi   TWI_DDR,SDAP         ;  release SDA
  rjmp  TWI_put_ack_high
TWI_put_ack_low:             ; else
  sbi   TWI_DDR,SDAP         ;  force SDA low
TWI_put_ack_high:

  rcall TWI_hp_delay         ; half period delay
  cbi   TWI_DDR,SCLP         ; release SCL
TWI_put_ack_wait:
  sbis  TWI_PIN,SCLP         ; wait SCL high
  rjmp  TWI_put_ack_wait
  rcall TWI_hp_delay         ; half period delay

  cbi   TWI_DDR,SDAP         ; release SDA 
  ret

;***************************************************************************
;* FUNCTION
;*  TWI_stop
;* DESCRIPTION
;*  Assert stop condition.
;* USAGE
;*  No parameters.
;* RETURN
;*  None.
;*
;***************************************************************************
TWI_stop:
  sbi   TWI_DDR,SCLP       ; force SCL low
  sbi   TWI_DDR,SDAP       ; force SDA low
  rcall TWI_hp_delay       ; half period delay
  cbi   TWI_DDR,SCLP       ; release SCL
  rcall TWI_qp_delay       ; quarter period delay
  cbi   TWI_DDR,SDAP       ; release SDA
  rcall TWI_hp_delay       ; half period delay
  ret

;***************************************************************************
;* FUNCTION
;*  TWI_init
;* DESCRIPTION
;*  Initialization of the TWI bus interface.
;* USAGE
;*  Call this function once to initialize the TWI bus. No parameters
;*  are required.
;* RETURN
;*  None
;* NOTE
;*  TWI_PORT and TWI_DDR pins not used by the TWI bus interface will be
;*  set to Hi-Z (!).
;* COMMENT
;*  This function can be combined with other TWI_PORT initializations.
;*
;***************************************************************************
;
;TWI_init:
;  clr   TWIstat            ; clear TWI status register (used
;                           ; as a temporary register)
;  out   TWI_PORT,TWIstat      ; set TWI pins to open colector
;  out   TWI_DDR,TWIstat
;  ret

;------------------ rom constants and tables --------------------------
Seg_Code_Table:
; Seven-segment display code table for digits 0 to F
; A one bit means segment is on.
;     D           D
;     Pgfedcba    Pgfedcba
.db 0b00111111, 0b00000110     ; 0, 1
.db 0b01011011, 0b01001111     ; 2, 3
.db 0b01100110, 0b01101101     ; 4, 5
.db 0b01111101, 0b00000111     ; 6, 7
.db 0b01111111, 0b01101111     ; 8, 9
.db 0b01110111, 0b01111100     ; A, B
.db 0b00111001, 0b01011110     ; C, D
.db 0b01111001, 0b01110001     ; E, F

;------------------ eeprom --------------------------------------------
