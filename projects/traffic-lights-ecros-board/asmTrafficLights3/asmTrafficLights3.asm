;----------------------------------------------------------------------
; asmTrafficLights3.asm - Traffic Light Control Exercise 3
;
; Hardware:
;   ATtiny2313V with 8.000 MHz crystal (PA0:1) on STK-500 or similar.
;
;   RS-232 driver to connect with PC running Tera Term Pro.  This
;   program was developed using the STK-500 and its built-in
;   spare RS-232 port.  Later the connection was modified to use two
;   FriendCom FT-221/AP wireless RF modules (no details given here).
;
;   ECROS Technology Traffic Lights project board, modified.
;   See http://www.ecrostech.com/General/TrafficLights/index.htm for
;   a description of the ERCOS Technology Traffic Lights board.
;  
;   In order to work with PB7 on the ATtiny2313V, the push button
;   pullup and current limiting resistors on the ECROS board were
;   changed as follows:  
;       R7, R11, R20, and R21 changed from 3.3K to 10K ohms
;       R12, R13, R22, and R23 changed from 2K to 150 ohms
;   
; AVR pin assignments:
;   PORTD0:1 - USART RX and TX connected to RS-232 driver
;   PORTB0:7 - Connection to ERCOS Technology Traffic Lights
;              project board, as follows:
;		PB0 - E/W green light driver (1 = on)
;		PB1 - E/W yellow light driver
;		PB2 - E/W red light driver
;		PB3 - E/W vehicle sensor push button (0 = pushed)
;		PB4 - N/S green light driver
;		PB5 - N/S yellow light driver
;		PB6 - N/S red light driver
;		PB7 - N/S vehicle sensor push button
;
; Notes:  This version controls a normal light sequence and reports
;         light status and passing vehicles to the serial port with
;         a time stamp.  System tick increased to 200 ms (5 ticks per
;         second) to allow time for serial link writes within a single
;         tick.  Time is accurate to about 10 seconds per day with
;         the crystal used.
;
;         This program has a simple round-robin structure with the
;         main loop executed once each system tick.  The system tick
;         is set to 200 ms to allow time for messages to be sent via
;         the USART.  This is not a flexible design, but its simplicity
;         made it fairly easy to write.  Increaseing the functionality
;         would probably require the use of ISRs.
;
;         The main loop has two sections, one for functions executed
;         once per tick (vehicle detectors), and the other for
;         functions executed once per second (stop light sequence and
;         clock).  
;
;         The light cycle function is essentially a state machine that
;         uses the cycle_sec counter to determine light status.  The
;         cycle timing can be changed by modifying the appropriate
;         equate statements.
;
;         The vehicle sensor input is debounced using the system tick.
;         A detection flag is set if the sensor is still down after
;         the wait period and the pass report is sent after the sensor
;         is subsequently released.  This sensor detection sequence is
;         handled by a state machine with 3 states.
;
;         The text messages to the PC are stored in program memory.
;         The time stamp text and values are stored in sram.  There
;         separate functions to send these message to the USART.
;        
; Andy Palm
; 2010.01.30
;
;----------------------------------------------------------------------
;------------------ includes, defines, equates ------------------------
.nolist
.include "tn2313def.inc"
.list

.equ  FREQ          = 8000000     ; Clock frequency in Hz

.equ  TICK_PRESCALE = 256         ; Tick timer prescaler value
.equ  TICKS_PER_S   = 5           ; Ticks per second for 200 ms tick
.equ  TIMER1_START  = 65536 - (FREQ/(TICKS_PER_S*TICK_PRESCALE))

.equ  BAUD          = 9600        ; Baud rate
.equ  BDDIV         = (FREQ/(16*BAUD))-1 ; Baud rate divisor for USART

.equ  DEB_MS        = 200         ; Ms in debounce delay
.equ  DEB_IVL       = (DEB_MS*TICKS_PER_S)/1000
                                  ; Ticks per debounce delay
; Control Port
.equ  CNTL_PORT     = PORTB	  ; Control port is PORTB
.equ  CNTL_DDR      = DDRB
.equ  CNTL_PIN      = PINB

.equ  EW_SEN_IN_BIT = PORTB3      ; Control port pins
.equ  NS_SEN_IN_BIT = PORTB7
.equ  EW_SF_BIT     = 0           ; Associated bit numbers in
.equ  NS_SF_BIT     = 1           ; sensor_flags

.equ  LIGHT_STATE_0 = 0b01000001  ; E/W = Green, N/S = Red
.equ  LIGHT_STATE_1 = 0b01000010  ; E/W = Yellow, N/S = Red
.equ  LIGHT_STATE_2 = 0b00010100  ; E/W = Red, N/S = Green
.equ  LIGHT_STATE_3 = 0b00100100  ; E/W = Red, N/S = Yellow

.equ  LIGHT_CYCLE_S = 30          ; Seconds in normal light cycle
.equ  LIGHT_COUNT_0 = LIGHT_CYCLE_S - 0
.equ  LIGHT_COUNT_1 = LIGHT_CYCLE_S - 12
.equ  LIGHT_COUNT_2 = LIGHT_CYCLE_S - 15
.equ  LIGHT_COUNT_3 = LIGHT_CYCLE_S - 27
.equ  LIGHT_COUNT_4 = 0

.equ  CR            = 0x0D        ; Carriage return
.equ  LF            = 0x0A        ; Line feed 
.equ  BS            = 0x08        ; Backspace
.equ  PROMPT        = '>'         ; Prompt character
.equ  NULL          = 0x00        ; Null character

; Register variables
.def  ew_db_count   = r2          ; E/W veh sensor debounce count
.def  ns_db_count   = r3          ; N/S veh sensor debounce count
.def  ew_db_state   = r4          ; E/W veh sensor debounce state
.def  ns_db_state   = r5          ; N/S veh sensor debounce state
.def  seconds       = r6          ; Time of day seconds
.def  minutes       = r7          ; Time of day minutes
.def  hours         = r8          ; Time of day hours

.def  tempa         = r16
.def  tempb         = r17
.def  cycle_sec     = r18         ; Count of seconds for light cycle

.def  sensor_flags  = r21         ; Sensor state flags
                                  ; bit 0 = E/W veh, bit 1 = N/S veh
.def  ones_digit    = r22         ; Storage for binary to ascii conv
.def  tens_digit    = r23

.def  count_tick_L  = r24         ; Low 8 bits of tick counter
.def  count_tick_H  = r25         ; High 8 bits of tick counter

;------------------ SRAM assignments ----------------------------------
.dseg
.org SRAM_START

Time_Stamp_Msg:     ; Time stamp message in format " HH:MM:SS"
Leading_Blank:
  .byte   1
Hr_Tens:
  .byte   1
Hr_Ones:
  .byte   1
Colon1:
  .byte   1
Min_Tens:
  .byte   1
Min_Ones:
  .byte   1
Colon2:
  .byte   1
Sec_Tens:
  .byte   1
Sec_Ones:
  .byte   1
Time_Stamp_End:
  .byte   1

;------------------ macros --------------------------------------------
; Macro to calculate ascii tens and ones digits from binary time
; values to allow transmission or display
; 
; Call is
;            Calc_Digits binary_value_register
;
; where binary_value_register contains the binary hours, minutes, or
; seconds value.  Results are written into tens_digit and ones_digit
; register variables.
;
.macro  Calc_Digits

  mov   tempb, @0
  clr   tens_digit        ; Tens digit result
  clr   ones_digit        ; Ones digit result
  ldi   tempa, 8          ; Counter for number of bits
Calc_Digits1:
  lsl   tens_digit
  lsl   tempb
  rol   ones_digit
  cpi   ones_digit, 10
  brmi  Calc_Digits2
  inc   tens_digit
  subi  ones_digit, 10
Calc_Digits2:
  dec   tempa
  brne  Calc_Digits1
  ldi   tempa, '0'        ; Convert digits to ascii values
  add   tens_digit, tempa
  add   ones_digit, tempa

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
  ldi   tempa, 0b01110111
  out   CNTL_DDR, tempa
  ldi   tempa, 0
  out   CNTL_PORT, tempa

; Set up TIMER1 for system tick
  ldi   tempa, HIGH(TIMER1_START)  ; Load counter start value
  out   TCNT1H, tempa
  ldi   tempa, LOW(TIMER1_START)   ; Load counter start value
  out   TCNT1L, tempa
  ldi   tempa, (1<<CS12)|(0<<CS11)|(0<<CS10)
  out   TCCR1B, tempa                ; Start with 256x prescaler

; Set up USART
  ldi   tempa, BDDIV
  out   UBRRL, tempa
  ldi   tempa, (1<<RXEN)|(1<<TXEN)  ; Enable rx and tx
  out   UCSRB, tempa


;------------------ main program --------------------------------------
Main:

; Initialize variables
  clr   ew_db_state           ; Sensor debounce states
  clr   ns_db_state
  clr   sensor_flags          ; Sensor flags

  ldi   tempa, 12             ; Time of day hours
  mov   hours, tempa
  ldi   tempa, 0              ; Time of day minutes
  mov   minutes, tempa
  clr   seconds               ; Time of day seconds

  ldi   count_tick_L, LOW(TICKS_PER_S)  ; Initialize 1 sec counter
  ldi   count_tick_H, HIGH(TICKS_PER_S)

; Initialize time stamp message in sram
; Format is " HH:MM:SS" with trailing null
  ldi   tempa, ' '
  sts   Leading_Blank, tempa
  clr   tempa
  sts   Time_Stamp_End, tempa
  ldi   tempa, ':'
  sts   Colon1, tempa
  sts   Colon2, tempa
  ldi   tempa, '0'
  sts   Hr_Tens, tempa
  sts   Hr_Ones, tempa
  sts   Min_Tens, tempa
  sts   Min_Ones, tempa
  sts   Sec_Tens, tempa
  sts   Sec_Ones, tempa

  rcall Wait_for_Tick               ; Wait for wireless modules
                                    ; to initialize

; Send start-up message to terminal
  ldi   ZH, HIGH(2*Startup_Msg)
  ldi   ZL, LOW(2*Startup_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp

; Initialize Light Cycle to State 0
  ldi   cycle_sec, LIGHT_COUNT_0    ; Initialize second counter
  ldi   tempa, LIGHT_STATE_0        ; Lights into State 0
  out   CNTL_PORT, tempa
; Send light state message
  ldi   ZH, HIGH(2*Light_State0_Msg)
  ldi   ZL, LOW(2*Light_State0_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp

Main_Loop:
  
  rcall Wait_for_Tick               ; Wait for system tick

; At this point one system tick has passed
; Execute actions that use ticks as their fundamental time unit

  rcall Check_EW_Sensor             ; Check vehicle sensors and
  rcall Check_NS_Sensor             ; set sensor flags accordingly

  rcall Report_Sensor_Flag          ; Print msg on veh detection

  sbiw  count_tick_H:count_tick_L, 1  ; Decr 1 sec tick counter
  brne  Count_Wait                  ; Check if 1 second has passed

; At this point one second has passed
; Execute actions that use seconds as their fundamental time unit
  rcall Time_Of_Day                 ; Maintain time of day clock

  rcall Normal_Light_Cycle          ; Operate normal light cycle

  ldi   count_tick_L, LOW(TICKS_PER_S)  ; Reload tick counter
  ldi   count_tick_H, HIGH(TICKS_PER_S)

Count_Wait:                         ; End of seconds loop

  rjmp  Main_Loop

;------------------ interrupt service routines ------------------------

;------------------ subroutines ---------------------------------------
Wait_for_Tick:
  in    tempa, TIFR             ; Check overflow flag for TIMER1
  sbrs  tempa, TOV1
  rjmp  Wait_for_Tick
  ldi   tempa, HIGH(TIMER1_START)  ; Load counter start value
  out   TCNT1H, tempa
  ldi   tempa, LOW(TIMER1_START)   ; Load counter start value
  out   TCNT1L, tempa
  ori   tempa, (1<<TOV1)        ; Clear flag by writing 1 to it
  out   TIFR, tempa
  ret

;----------------------------------------------------------------------
; Maintain time of day clock.  Assumes called once per second by
; routine.  Format is military time 00:00:00 to 23:59:59 hours.
;
Time_Of_Day:
  inc   seconds                 ; Incr second counter
  ldi   tempa, 60
  cp    seconds, tempa          ; Equal to 60?
  brne  Time_Of_Day_Done        ; If no, return
  clr   seconds                 ; Yes, set seconds to zero
  inc   minutes                 ; and increment minutes counter
  cp    minutes, tempa          ; Equal to 60?
  brne  Time_Of_Day_Done        ; If no, return
  clr   minutes                 ; Yes, set minutes to zero
  inc   hours                   ; and increment hours counter
  ldi   tempa, 24
  cp    hours, tempa            ; Equal to 24?
  brne  Time_Of_Day_Done        ; If no, return
  clr   hours                   ; Yes, set hours to zero

Time_Of_Day_Done:
  ret

;----------------------------------------------------------------------
; Send a message in program memeory to the PC via RS-232.  Assumes
; message address is in Z register.  Example use:
;
;  ldi   ZH, HIGH(2*Com_Msg1)
;  ldi   ZL, LOW(2*Com_Msg1)
;  rcall Send_Com_Msg
;
Send_Com_Msg:
  sbis  UCSRA, UDRE                 ; Wait for empty tx buffer
  rjmp  Send_Com_Msg
  lpm   tempa, Z+                   ; Get next char in message
  tst   tempa                       ; If NULL, finished
  breq  Send_Com_Msg_Done
  out   UDR, tempa                  ; Put char in tx buffer
  rjmp  Send_Com_Msg
Send_Com_Msg_Done:
  ret

;----------------------------------------------------------------------
; Send a message in data space to the PC via RS-232.  Assumes
; message address is in Z register.  Example use:
;
;  ldi   ZH, HIGH(Data_Msg1)
;  ldi   ZL, LOW(Data_Msg1)
;  rcall Send_Data_Msg
;
Send_Data_Msg:
  sbis  UCSRA, UDRE                 ; Wait for empty tx buffer
  rjmp  Send_Data_Msg
  ld    tempa, Z+                   ; Get next char in message
  tst   tempa                       ; If NULL, finished
  breq  Send_Data_Msg_Done
  out   UDR, tempa                  ; Put char in tx buffer
  rjmp  Send_Data_Msg
Send_Data_Msg_Done:
  ret

;----------------------------------------------------------------------
; Send time stamp to the PC via RS-232.
;
Send_Time_Stamp:
; Convert binary time in registers to ascii in data space msg
  Calc_Digits hours
  sts   Hr_Tens, tens_digit
  sts   Hr_Ones, ones_digit
  Calc_Digits minutes
  sts   Min_Tens, tens_digit
  sts   Min_Ones, ones_digit
  Calc_Digits seconds
  sts   Sec_Tens, tens_digit
  sts   Sec_Ones, ones_digit

; Send time stamp message in data space
  ldi   ZH, HIGH(Time_Stamp_Msg)
  ldi   ZL, LOW(Time_Stamp_Msg)
  rcall Send_Data_Msg

  ret

;----------------------------------------------------------------------
; Check E/W vehicle sensor and debounce using system ticks
; and state machine (0 = wait for sensor down, 1 = wait for debounce
; count to complete, 2 = wait for sensor up)
Check_EW_Sensor:
  tst   ew_db_state
  brne  Check_EW_Sensor1
  sbic  CNTL_PIN, EW_SEN_IN_BIT   ; In State 0, check sensor position
  rjmp  Check_EW_Sensor_Done      ; Sensor up, do nothing
  inc   ew_db_state               ; Sensor down, set to State 1
  ldi   tempa, DEB_IVL            ; Initialize debounce count
  mov   ew_db_count, tempa
  rjmp  Check_EW_Sensor_Done
Check_EW_Sensor1:
  ldi   tempa, 2                  ; Test for State 1 or 2
  cp    ew_db_state, tempa
  breq  Check_EW_Sensor2
  dec   ew_db_count               ; In State 1, debounce count down
  brne  Check_EW_Sensor_Done
  clr   ew_db_state               ; Wait is over, reset state
  sbic  CNTL_PIN, EW_SEN_IN_BIT   ; Sensor still down after wait?
  rjmp  Check_EW_Sensor_Done      ; No so state reset to 0
  sbr   sensor_flags, (1<<EW_SF_BIT)  ; Yes, set veh sensor flag
  ldi   tempa, 2                  ; And switch to State 2
  mov   ew_db_state, tempa
  rjmp  Check_EW_Sensor_Done
Check_EW_Sensor2:
  sbis  CNTL_PIN, EW_SEN_IN_BIT   ; State 2, button still down?
  rjmp  Check_EW_Sensor_Done      ; Yes, do nothing
  clr   ew_db_state               ; No, reset state to 0
  ldi   ZH, HIGH(2*EW_Veh_Sensor_Msg) ; Send vehicle passed msg
  ldi   ZL, LOW(2*EW_Veh_Sensor_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
Check_EW_Sensor_Done:
  ret

;----------------------------------------------------------------------
; Check N/S vehicle sensor and debounce using system ticks
; and state machine (0 = wait for sensor down, 1 = wait for debounce
; count to complete, 2 = wait for sensor up)
Check_NS_Sensor:
  tst   ns_db_state
  brne  Check_NS_Sensor1
  sbic  CNTL_PIN, NS_SEN_IN_BIT   ; In State 0, check sensor position
  rjmp  Check_NS_Sensor_Done      ; Sensor up, do nothing
  inc   ns_db_state               ; Sensor down, set to State 1
  ldi   tempa, DEB_IVL            ; Initialize debounce count
  mov   ns_db_count, tempa
  rjmp  Check_NS_Sensor_Done
Check_NS_Sensor1:
  ldi   tempa, 2                  ; Test for State 1 or 2
  cp    ns_db_state, tempa
  breq  Check_NS_Sensor2
  dec   ns_db_count               ; In State 1, debounce count down
  brne  Check_NS_Sensor_Done
  clr   ns_db_state               ; Wait is over, reset state
  sbic  CNTL_PIN, NS_SEN_IN_BIT   ; Sensor still down after wait?
  rjmp  Check_NS_Sensor_Done      ; No so state reset to 0
  sbr   sensor_flags, (1<<NS_SF_BIT)  ; Yes, set veh sensor flag
  ldi   tempa, 2                  ; And switch to State 2
  mov   ns_db_state, tempa
  rjmp  Check_NS_Sensor_Done
Check_NS_Sensor2:
  sbis  CNTL_PIN, NS_SEN_IN_BIT   ; State 2, button still down?
  rjmp  Check_NS_Sensor_Done      ; Yes, do nothing
  clr   ns_db_state               ; No, reset state to 0
  ldi   ZH, HIGH(2*NS_Veh_Sensor_Msg) ; Send vehicle passed msg
  ldi   ZL, LOW(2*NS_Veh_Sensor_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
Check_NS_Sensor_Done:
  ret

;----------------------------------------------------------------------
; Test sensor_flag operation by printing message
Report_Sensor_Flag:
  sbrs  sensor_flags, EW_SF_BIT     ; E/W sensor flag set?
  rjmp  EW_Flag_Report_Done         ; No, do nothing
  ldi   ZH, HIGH(2*EW_Veh_Sensor_Msg2)  ; Yes, send message
  ldi   ZL, LOW(2*EW_Veh_Sensor_Msg2)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
  cbr   sensor_flags, (1<<EW_SF_BIT)    ; And clear flag
EW_Flag_Report_Done:
  sbrs  sensor_flags, NS_SF_BIT     ; N/S sensor flag set?
  rjmp  NS_Flag_Report_Done         ; No, do nothing
  ldi   ZH, HIGH(2*NS_Veh_Sensor_Msg2)  ; Yes, send message
  ldi   ZL, LOW(2*NS_Veh_Sensor_Msg2)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
  cbr   sensor_flags, (1<<NS_SF_BIT)    ; And clear flag
NS_Flag_Report_Done:
  ret

;----------------------------------------------------------------------
; Operate normal light cycle
Normal_Light_Cycle:
  dec   cycle_sec                     ; Decr cycle second counter
; Check for start of Light Cycle State 1
  cpi   cycle_sec, LIGHT_COUNT_1
  brne  PC+8
  ldi   tempa, LIGHT_STATE_1
  out   CNTL_PORT, tempa
  ldi   ZH, HIGH(2*Light_State1_Msg)  ; Send light state message
  ldi   ZL, LOW(2*Light_State1_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
  rjmp  Cycle_Wait
; Check for start of Light Cycle State 2
  cpi   cycle_sec, LIGHT_COUNT_2
  brne  PC+8
  ldi   tempa, LIGHT_STATE_2
  out   CNTL_PORT, tempa
  ldi   ZH, HIGH(2*Light_State2_Msg)  ; Send light state message
  ldi   ZL, LOW(2*Light_State2_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
  rjmp  Cycle_Wait
; Check for start of Light Cycle State 3
  cpi   cycle_sec, LIGHT_COUNT_3
  brne  PC+8
  ldi   tempa, LIGHT_STATE_3
  out   CNTL_PORT, tempa
  ldi   ZH, HIGH(2*Light_State3_Msg)  ; Send light state message
  ldi   ZL, LOW(2*Light_State3_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
  rjmp  Cycle_Wait
; Check for return to Light Cycle State 0
  cpi   cycle_sec, LIGHT_COUNT_4
  brne  PC+8
  ldi   tempa, LIGHT_STATE_0          ; Return to initial state
  out   CNTL_PORT, tempa
  ldi   ZH, HIGH(2*Light_State0_Msg)  ; Send light state message
  ldi   ZL, LOW(2*Light_State0_Msg)
  rcall Send_Com_Msg
  rcall Send_Time_Stamp
  ldi   cycle_sec, LIGHT_COUNT_0      ; Re-init cycle sec count
Cycle_Wait:
  ret

;------------------ rom constants and tables --------------------------
; Messages for RS-232 PC terminal status reporting
Startup_Msg:
  .db CR, LF, CR, LF, PROMPT, "System Actived", NULL
Light_State0_Msg:
  .db CR, LF, PROMPT, "EW=G, NS=R", NULL
Light_State1_Msg:
  .db CR, LF, PROMPT, "EW=Y, NS=R", NULL
Light_State2_Msg:
  .db CR, LF, PROMPT, "EW=R, NS=G", NULL
Light_State3_Msg:
  .db CR, LF, PROMPT, "EW=R, NS=Y", NULL
EW_Veh_Sensor_Msg:
  .db CR, LF, PROMPT, "EW veh pass ", NULL
NS_Veh_Sensor_Msg:
  .db CR, LF, PROMPT, "NS veh pass ", NULL
EW_Veh_Sensor_Msg2:
  .db CR, LF, PROMPT, "EW veh detect ", NULL
NS_Veh_Sensor_Msg2:
  .db CR, LF, PROMPT, "NS veh detect ", NULL


;------------------ eeprom --------------------------------------------
