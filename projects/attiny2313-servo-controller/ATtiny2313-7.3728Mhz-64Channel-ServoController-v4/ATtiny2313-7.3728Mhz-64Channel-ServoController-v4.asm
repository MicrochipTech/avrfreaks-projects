; ******************************************* 
; ** 64 Channel Serial Servo Controller    **
; **           For ATtiny2313              **
; **            Version 4.0                **
; **                                       **
; **        Copyright (c) July 2009        **
; **             Len Holgate               **
; **                                       **
; ** Note that this controller assumes     **
; ** that we have CD74HCT238E or equivalent**
; ** demultiplexor chips connected to the  **
; ** outputs of PortB and that the required**
; ** address lines for these MUXs are run  **
; ** from pins 3-5 of PortD.               **
; ******************************************* 

.nolist
.include "tn2313def.inc"
.list


.def temp = r0
.def temp1 = r1
.def temp2 = r2

.def count = r16
.def index = r17

.def bankIndex = r19
.def temp20 = r20           
.def temp21 = r21
.def resl  = r22
.def resh  = r23
.def muxAddress = r24
.def serialChar = r25



.equ POSITION_DATA_START  = SRAM_START
.equ NUM_SERVOS           = 64
.equ PWM_DATA_START       = POSITION_DATA_START + NUM_SERVOS
.equ PWM_SERVOS_PER_CYCLE = 8
.equ PWM_BYTES_PER_SERVO  = 3
.equ PWM_DATA_SIZE        = (PWM_SERVOS_PER_CYCLE + 1) * PWM_BYTES_PER_SERVO
.equ SERIAL_DATA_START    = PWM_DATA_START + PWM_DATA_SIZE
.equ SERIAL_DATA_LENGTH   = 2
.equ SERIAL_DATA_END      = SERIAL_DATA_START + SERIAL_DATA_LENGTH

.equ JUMP_TABLE_START    = $0012


.equ clock        = 7372800
.equ baudrate     = 9600
.equ baudconstant = (clock/(16*baudrate))-1


.ORG $0000
    rjmp Init       ; Reset
    reti            ; INT0 Interrupt
    reti            ; INT1 Interrupt
    reti            ; TC1Capt Interrupt
    rjmp TC1CmpA    ; Timer 1 Compare A match
    reti            ; TC1 Overflow Int
    reti            ; TC0 Overflow Int
    reti            ; UART0 RX Int
    reti            ; UART0 TX Int
    reti            ; Ana Comp Int
    reti            ; PCINT
    reti            ; TC1 CompB Int
    reti            ; TC0 Compare Match A Int
    reti            ; TC0 Compare Match B Int
    reti            ; USISTART Int
    reti            ; USIOverflow Int
    reti            ; EERDY Int
    reti            ; WDT Overflow Int

.ORG JUMP_TABLE_START
rjmp PWMSetup 
rjmp PWMPulseStop

; Timer 1 interrupt for CTC timer 

TC1CmpA:

    push temp20         ; save the state of these as they are used by the serial protocol code
    push temp21

    in temp20, SREG     ; save status register
    push temp20

    ijmp                ; Z is set up to point into our jump table (above) so we jump to Z then
                        ; jump to the correct state for our timer handling...


TC1CmpAEnd :

    pop temp20          ; restore the state prior to the interrupt
    out SREG, temp20
    pop temp21
    pop temp20

    reti


; Program start

Init:

    ; Set stack pointer - we use the stack for the return address of the interrupt handler...

    ldi temp20, RAMEND
    out SPL, temp20

    ; Set up the serial port

    ldi temp20, HIGH(baudconstant)          ; Set the baud rate
    out UBRRH, temp20
    ldi temp20, LOW(baudconstant)
    out UBRRL, temp20

    ldi temp20, (1 << RXEN) | (1 << TXEN)   ; enable rx and tx
    out UCSRB, temp20

    ldi temp20, (3 << UCSZ0)                ; 8N1 

    ; Initialise Timer1 in CTC mode

    ldi temp20, HIGH(200)       ; CTC-value - something low to kick us off... Must be longer
    out OCR1AH, temp20          ; than it takes for us to finish our initialisation...
    ldi temp20, LOW(200)        ; Note that the 16-bit timer control registers MUST be written
    out OCR1AL, temp20          ; high byte first then low byte as the low write triggers the
                                ; 16-bit atomic write.


    clr temp20                  ; Controlword A    0x0000
    out TCCR1A, temp20

    ldi temp20, (1<<WGM12) | (1<<CS11) ; CTC mode (WGM12) with the timer running at clock/8
    out TCCR1B, temp20

    ldi temp20, 1 << OCIE1A     ; Enable Timer1 interrupt
    out TIMSK, temp20

    ; Initialise our PWM output pins    

    ldi temp20, $FF             ;set port B as output port
    out DDRB, temp20

    ; Initialise our MUX address select output pins         

    ldi temp20, $38             ; Init MUX address line outputs; we use pins 3-5 on port D
    out DDRD, temp20

    clr muxAddress              ; address channel 0 on the mux's
    out PORTD, muxAddress

    ; We have two different states for our timer interrupt handling, a setup phase and a 
    ; 'pulse switch off' phase. We use an indirect jump via the value of the Z pointer to deal 
    ; with this. Initially we set Z to point to the start of our jump table, which jumps to 
    ; the PWM setup routine, at the end of the setup routine we change Z to point to the next 
    ; entry in our jump table which is the PWM pulse switch off routine. Once all pulses have 
    ; been switched off we reset Z to the PWM setup routine and the next interrupt starts the 
    ; PWM generation for the next bank of servo pins.

    ldi ZL, LOW(JUMP_TABLE_START)   ; set up our first jump table entry for timer handling
    ldi ZH, HIGH(JUMP_TABLE_START)

    ; Clear down the SRAM area so that it is slightly easier to debug in the simulator
    ; Note that this code can be removed without causing any harm...

    ldi XL, LOW(SRAM_START)         ; clear down the area we'll be working in
    ldi XH, HIGH(SRAM_START)        ; to make it easier to debug

    ldi count, SRAM_SIZE
    clr temp20                      ; usually it's easier if all of SRAM is 0x00 in the simulator
    ;ldi temp20, 0xFF               ; but sometimes 0xFF helps too...

InitFillMemoryLoop:

    st X+, temp20
    dec count
    brne InitFillMemoryLoop

    ; End of SRAM memory fill loop

    ; Now set all of our servos to their initial start positions...

    ldi XL, LOW(POSITION_DATA_START)
    ldi XH, HIGH(POSITION_DATA_START)

    ldi count, 64
    ldi temp20, 0x7F

InitLoadInitialPositionsLoop:

    st X+, temp20
    dec count
    brne InitLoadInitialPositionsLoop

    ; Initialisation is done...

    sei                             ; enable interrupts    

    rjmp    SerialStart


PWMSetup : 

    ; Set the timer to 400ms which is much longer than the PWM setup phase takes to run. We 
    ; will adjust this to the correct value when we complete the setup phase.

    ldi temp20,HIGH(400) 
    out OCR1AH,temp20
    ldi temp20, LOW(400)        ; Note that the 16-bit timer control registers MUST be written
    out OCR1AL, temp20          ; high byte first then low byte as the low write triggers the
                                ; 16-bit atomic write.


    ; set the port d bits to control the mux address selection...

    out PORTD, muxAddress       ; mux address is 8 * bank index, 08, 10, 18, 20 etc


    ; Start the PWM signal generation for this bank. All pins on port b are on...

    ldi temp20, $FF
    out PORTB, temp20

    ; We generate 8 PWM signals at a time and repeat each 8 signals every 20ms, since we can
    ; support 64 servos in total this means we need to generate 8 batches of 8 signals and 
    ; each batch can take at most 2.5ms to generate. The signals should be between 600us and
    ; 2.4ms long. 

    ; The data we use to generate the PWM signals is based on the data that the serial I/O
    ; code stores at POSITION_DATA_START. The serial code stores a single byte per servo. The
    ; servo control value can be 0-254 with 127 being in the centre with a pulse length of 
    ; 1.5ms

    ; The PWM generation code uses two byte servo control values that represent the actual
    ; timeout values required to turn off the servo pin, we can have up to 8 off times, as
    ; we merge duplicate off times together. The final off time (which could be number 9 if
    ; we have 8 unique servo control values) is the time remaining between the last servo 
    ; pin being turned off and the start of the next PWM generation cycle. I.e. it is the 
    ; time remaining until the 2.5ms time period has passed.

    ; First we copy our position data from the source buffers that the serial I/O code
    ; can alter into the working buffers that we'll use during PWM generation.
    ; The soruce data is arranged in 8 banks of 8 bytes, we use the bankIndex to index
    ; into the start of the bank that we're working on.

    ; To make it easier to create a board for versions of this controller that don't provide
    ; all 64 channels we reorder the servo data during the copy so that the servo channels 
    ; appear sequentially on the pins of the MUX chips. That is servo 0 is pin 0 on mux 1, 
    ; servo 1 is pin 1 on mux 1, servo 9 is pin 0 on mux 2, etc.

    ; To achieve this we need to step the source pointer of our copy by 8 each time, starting
    ; at the bankIndex address. So for the first cycle we copy from 0, 8, 16, 24, 32, 40, 48, 56
    ; for the next from 1, 9, 17, etc.


    ldi ZL, LOW(POSITION_DATA_START)   ;Load memory location to copy from
    ldi ZH, HIGH(POSITION_DATA_START)

    ; use the bank index as the start offset and increment by 8 for each subsequent index.

    add ZL, bankIndex

    ; Increment bankIndex for next time around the loop and reset to 0 when we get to 8...

    ; Now we set things up for the next time that the PWMSetup code is called, we increment the
    ; mux address by 8, and increment the bank address...

    ldi temp20, 8
    add muxAddress, temp20

    inc bankIndex            ; select next bank of servos, wrap to 0 at 8..

    cpi bankIndex, 8
    brne PWMSetupSetDestination

    clr bankIndex
    clr muxAddress

PWMSetupSetDestination:

    ldi YL, LOW(PWM_DATA_START)
    ldi YH, HIGH(PWM_DATA_START)

    ; The position data consists of a 1 byte value for each servo. The PWM code needs to 
    ; also have the bit that corresponds to the pin that controls this servo. As we copy
    ; from source to destination we expand the data per servo from a 1 byte position value 
    ; to a two byte position and bit combination.

    ldi index,1             ; the first pin is at position 1
    ldi count, 8            ; we are dealing with 8 servos

PWMSetupCopy:

    ld temp,Z               ; read from the position data
    adiw ZL:ZH, 8           ; increment the source pointer by 8...

    st Y+,temp              ; store to the pwm data

    ldi temp20, 0xFF
    eor temp20, index       ; servos are ON when the pin is set, we need to know the unset, OFF 
                            ; value for this pin
    st Y+, temp20           ; store the pwm OFF pin value

    lsl index               ; next pin
    dec count               ; next servo
    brne PWMSetupCopy


    ; Now that we have the servo data copied into our working space and grouped with the 
    ; related pin information we need to sort the data into order from low to high. This
    ; will allow us to set a timer for the next servo control value change, adjust the pins
    ; when the timer fires and then step to the next later servo control value.

    ; We use a simple bubble sort to compare the values of this servo control byte with the
    ; next servo control byte and if the next value is less than this value we swap both the
    ; values and the pins.

    ; During the sort we merge the pin values of servo control values that are equal as we
    ; only need to set the timer once to turn off multiple servos. This leaves us with a 
    ; servo control value that we no longer need, we set this to 0xFF so that it automatically
    ; bubbles up to the end of the list.

    ; set up before sort...

.equ UnusedServoValue = 0xFF

    ldi temp20, UnusedServoValue        ; control value for 'unused pwm control value'

PWMSetupSortLoop:

.def changed = r18

    clr changed                         ; changed is a flag that we set if we need to swap
                                        ; any values this time through the loop, we keep
                                        ; looping until we manage to get through the whole
                                        ; loop without swapping anything, and then we know that
                                        ; the values are in order.

    ldi ZL, LOW(PWM_DATA_START)         ; 'this' value, start at the begining of the PWM data
    ldi ZH, HIGH(PWM_DATA_START)
    ldi YL, LOW(PWM_DATA_START + 2)     ; 'next' value, note we compare values and ignore the pins
    ldi YH, HIGH(PWM_DATA_START + 2)

PWMSetupSort1:

    ld temp1, Z                         ; load this value
    ld temp2, Y                         ; load next value

    cp temp2, temp1                     ; if next < this then swap them
    brlo PWMSetupSwap

    brne PWMSetupSortIncrement1         ; if they are not the same, increment the indices and move
                                        ; on to the next value

    cp temp2, temp20                    ; if they are the same but the are not UnusedServoValue, merge them
    brne PWMSetupMerge

PWMSetupSortIncrement1:

    inc ZL                              ; step on to this pin
    inc YL                              ; step on to next pin

PWMSetupSortIncrement2:

    cpi YL, LOW(PWM_DATA_START + 0x0F)  ; check to see if we're at the last value...        
    breq PWMSetupSortLoopEnd            ; if so see if we need to restart the loop

    inc ZL                              ; step on to the next 'this' value
    inc YL                              ; step on to the next 'next' value


    rjmp PWMSetupSort1                  ; sort the next two values...
    
PWMSetupSwap:

    st Z+, temp2                        ; store the 'next' value in the 'this' position
    st Y+, temp1                        ; store the 'this' value in the 'next' position
    ld temp2, Z                         ; load the 'this' pin data
    ld temp1, Y                         ; load the 'next' pin data         
    st Z, temp1                         ; and swap those two
    st Y, temp2

    inc changed                         ; note that we changed something so that we run the whole 
                                        ; sort loop again

    rjmp PWMSetupSortIncrement2         ; since we swapped the values the indices are at the pin
                                        ; positions (value + 1) therefore we jump to the second 
                                        ; stage of our index increment code...

PWMSetupMerge:

    ; If the servo values are equal then we merge the pin values together and set the later servo
    ; value to UnusedServoValue which is 0xFF and will automatically bubble up to the end of the 
    ; list...

    st Y+, temp20                       ; store UnusedServoValue to the 'next' value, we don't use that 
                                        ; value anymore
    inc ZL                              ; step to 'this' pin
    ld temp2, Z                         ; load 'this' pin
    ld temp1, Y                         ; load 'next' pin
    and temp1, temp2                    ; and the pin values together to merge the bits set in into temp1
    st Z, temp1                         ; and store it as the 'this' pin value

    inc changed                         ; note that we changed something so that we run the whole 
                                        ; sort loop again

    rjmp PWMSetupSortIncrement2         ; since we merged the values the indices are at the pin
                                        ; positions (value + 1) therefore we jump to the second 
                                        ; stage of our index increment code...

PWMSetupSortLoopEnd :

    cpi changed, 0                      ; did we change anything this time through the loop?
    brne PWMSetupSortLoop               ; if so we run the whole sort again...

.undef changed

    ; Now that the servo control data has been sorted into ascending pulse length order and the 
    ; duplicates have been merged we need to run through the pin control values and merge the later ones
    ; with the earlier ones so that each ascending pulse turn off value includes all of the earlier turn
    ; off signals... That is, if we have a sequence of 0x01 0xFE, 0x02 0xEF then we should and the first 
    ; off signal with the second to give a sequence of 0x01 0xFE, 0x02 0xEE. This means that as we work
    ; our way through the sequences we turn off progressively more pulses.

PWMSetupBitMergeSetup:

    ldi ZL, LOW(PWM_DATA_START + 1)     ; 'this' pin data, start at the first pin data element in the PWM
    ldi ZH, HIGH(PWM_DATA_START + 1)    ; data area.
    ldi YL, LOW(PWM_DATA_START + 3)     ; 'next' pin data
    ldi YH, HIGH(PWM_DATA_START + 3)

PWMSetupBitMerge:

    ld temp1, Z+                        ; load 'this' value
    ld temp2, Y                         ; load 'next' value

    and temp1, temp2                    ; merge the two values so that pins that were off for 'this' are
                                        ; also off for 'next'
    st Y, temp1                         ; store the new 'next' value.

    cpi ZL, LOW(PWM_DATA_START + 0x0E)  ; check for last value...        
    breq PWMSetupBitMergeDone

    inc ZL                              ; increment our indices, we already incremented Z once during our load
    inc YL
    inc YL

    rjmp PWMSetupBitMerge

PWMSetupBitMergeDone:


    ; We now have a seqence of servo control values and pin control values sorted in ascending time order and
    ; our pin control values are such that as we step through them we progressively turn off more pins.

    ; Now we need to make the servo control values relative to each other so that the sequential values are 
    ; incremental rather than absolute; for example, 120 127 129 130 becomes 120 7 2 1

    ldi temp20, 0xFF

    ldi ZL,     LOW(PWM_DATA_START)    ; first servo control value
    ldi ZH,    HIGH(PWM_DATA_START)

    ld temp1, Z+
    inc ZL

PWMSetupSequentialLoop :

    ld temp2, Z

    cp temp2, temp20
    breq PWMSetupSequentialLoopEnd

    sub temp2, temp1
    st Z+, temp2
    add temp1, temp2
    inc ZL

    rjmp PWMSetupSequentialLoop

PWMSetupSequentialLoopEnd :


    ; Now we need to convert the single byte servo control values into values suitable for our timer control.
    ; This requires us to multiply by 7 to give us a range of 0-1778 with the centre position as 889 for
    ; a servo control value of 127


    ; This changes the shape of the data from a list of two byte structures (servo control byte, pin control byte)
    ; to a list of three byte structres (servo high, servo low, pin control).

    ; At this point we zero out the entries that were merged and are now extra, i.e. the 0xFFs


    ldi ZL, LOW(PWM_DATA_START + 0x0E)      ; source data, we're working backwards along our list so that we can 
    ldi ZH, HIGH(PWM_DATA_START + 0x0E)     ; overwrite the data in place...
    ldi YL, LOW(PWM_DATA_START + 0x15)      ; our list is now 8 bytes longer than it was before as all the elements
    ldi YH, HIGH(PWM_DATA_START + 0x15)     ; are a byte bigger...

    ldi count, 7                            ; we multiply by 7 to get a range of 0-1778

PWMSetupMultiplyLoop:

    ld temp20, Z+                           ; load the source data
    cpi temp20, 0xFF                        ; if it's 0xFF we skip it
    breq PWMSetupSkipMultiply

    mov temp1, temp20                       ; set up for the multiply call
    mov temp2, count

    rcall MULT8                             ; multiply temp1 by temp2 the results are in resl and resh

    rjmp PWMSetupAfterMultiply

PWMSetupSkipMultiply:

    clr resl                                ; we need these to be zero so that we can write zeroes for the skipped
    clr resh                                ; elements...

PWMSetupAfterMultiply:

    ld temp1, Z                             ; load the pin control data
    st Y+, resh                             ; store the new servo control value; low byte
    st Y+, resl                             ; and high byte
    st Y, temp1                             ; store the pin control data


    cpi ZL, LOW(PWM_DATA_START + 1)         ; check for last value...        
    breq PWMSetupMultplyDone

    dec ZL                                  ; step our source index back 3 (we'd stepped forward 1 to get to the pin data)
    dec ZL
    dec ZL

    dec YL                                  ; step our destination index back 5 (we'd stepped forward two whilst writing this data)
    dec YL
    dec YL
    dec YL
    dec YL

    rjmp PWMSetupMultiplyLoop               ; multiply the next value...

PWMSetupMultplyDone:

    ; Now that we have the expanded relative timeout values in order we need to add our base time to the first
    ; timeout

    ; To get from the new control values to the absolute time in us we need to add 493, which gives a
    ; value of 1500 for 127, and a range of 611us - 2389us (493-2271 clock ticks)

.equ timerBase = 493                        ; if you're running with a 7.3728mhz clock - wider range

    ; Since the AVR doesn't have an add adci we subtract a negative number; see the examples with AVR202 application note

    subi resl, low(-timerBase)
    sbci resh, high(-timerBase)    


    dec YL                                  ; step our destination index back to allow us to overwrite the value
    dec YL

    st Y+, resh
    st Y+, resl

    ; Now that we have our timeouts in order as relative times we need to work out the absolute time for
    ; the final timeout. Once we have this we can work out how much time we have left in our timeslot and 
    ; set a timeout for that time which brings us to the end of this batch of PWM signals and starts us on
    ; the next batch...

    clr resl
    clr resh

    ldi ZL, LOW(PWM_DATA_START)             ; source
    ldi ZH, HIGH(PWM_DATA_START)

PWMSetupAccumulateTotalTimeLoop:

    ld temp20, Z+        ; high
    ld temp21, Z+        ; low

    add resl, temp21

    adc resh, temp20

    ld temp20, Z+
    cpi temp20, 0x00
    brne PWMSetupAccumulateTotalTimeLoop

.equ maxTime = 2304                         ; if you're running with a 7.3728mhz clock 

    ldi temp20, low(maxTime)
    ldi temp21, high(maxTime)

    sub temp20, resl
    sbc temp21, resh

    st Z+, temp21                           ; store the final timer value that takes us to the end of our 2.5ms timeslot...
    st Z+, temp20

    ldi temp20, 0xFF                        ; final sentinel pin control value for the next loop...
    st Z+, temp20

    ; And finally.... Our interrupt processing from the point where the timer goes off to the point where we
    ; have turned off the PWM generation for the pins concerned takes up 1us, so we need to subtract that from the 
    ; times...

    ldi ZL, LOW(PWM_DATA_START)             ; source 
    ldi ZH, HIGH(PWM_DATA_START)
    ldi YL, LOW(PWM_DATA_START)             ; destination 
    ldi YH, HIGH(PWM_DATA_START)

PWMSetupIntTimeAdjustLoop:

    ld resh, Z+
    ld resl, Z+

    subi resl, low(1)
    sbci resh, high(1)

    st Y+, resh
    st Y+, resl
    inc YL

    ld temp20, Z+
    cpi temp20, 0xFF
    brne PWMSetupIntTimeAdjustLoop

    ; Now set up our Y pointer for use by the PWM pulse stop code and 
    ;  adjust the timer that we set so that it goes off at the first PWM stop time

    ldi YL, LOW(PWM_DATA_START)        ; source 
    ldi YH, HIGH(PWM_DATA_START)

    ld temp20, Y+
    out OCR1AH,temp20
    ld temp20,Y+
    out OCR1AL,temp20

    ; Finally set our Z pointer back into the correct place in our jump table so that
    ; we're processing PWM pulse stops when the next interrupt goes off...

    ldi ZL, LOW(JUMP_TABLE_START + 1)       ; set up Z for the next timer interrupt
    ldi ZH, HIGH(JUMP_TABLE_START + 1)      ; we jump to the second entry in the jump table

    ; Setup is complete

    rjmp TC1CmpAEnd


; PWM pulse stop phase. This timer interrupt handler turns off some pins on PORTB and then sets the
; next stop time and returns, once all pins are off the final timeout uses up the remaining part
; of the 2.5ms slot and sets the Z pointer to call the PWMSetup code to start the next batch of servos

PWMPulseStop :

    ld temp20, Y+

    nop                
    nop                
    nop                
    nop
    nop
    nop
    nop
    out PORTB, temp20               ; Turn off servo pins...

    cpi temp20, 0x00                ; If all the pins are now off then the next timeout is our last
    brne PWMPulseSetNextTime        ; and we switch back to setup phase when it expires...

    ldi ZL, LOW(JUMP_TABLE_START)   ; set up our first jump table entry for timer handling
    ldi ZH, HIGH(JUMP_TABLE_START)

PWMPulseSetNextTime:

    ; load the next time...

    ld temp20, Y+
    out OCR1AH,temp20
    ld temp20,Y+                    ; Note that the 16-bit timer control registers MUST be written
    out OCR1AL, temp20              ; high byte first then low byte as the low write triggers the
                                    ; 16-bit atomic write.

    rjmp TC1CmpAEnd


;
; Here we start with the multiplication of the two binaries
; in rm1 und rm2, the result will go to reh:rel (16 Bit)
;
MULT8:
;
; Clear start values
    clr temp ; clear interim storage
    clr resl ; clear result registers
    clr resh
;
; Here we start with the multiplication loop
;
MULT8a:
;
; Step 1: Rotate lowest bit of binary number 2 to the carry
;         flag (divide by 2, rotate a zero into bit 7)
;
    clc ; clear carry bit
    ror temp2 ; bit 0 to carry, bit 1 to 7 one position to
     ; the right, carry bit to bit 7
;
; Step 2: Branch depending if a 0 or 1 has been rotated to
;         the carry bit
;
    brcc MULT8b ; jump over adding, if carry has a 0
;
; Step 3: Add 16 bits in rmh:rml to the result, with overflow
;         from LSB to MSB
;
    add resl,temp1 ; add LSB of rm1 to the result
    adc resh,temp ; add carry and MSB of rm1
;
MULT8b:
;
; Step 4: Multiply rmh:rm1 by 2 (16 bits, shift left)
;
    clc ; clear carry bit
    rol temp1 ; rotate LSB left (multiply by 2)
    rol temp ; rotate carry into MSB and MSB one left
;
; Step 5: Check if there are still one's in binary 2, if
;         yes, go on multiplicating
;
    tst temp2 ; all bits zero?
    brne MULT8a ; if not, go on in the loop
;
; End of the multiplication, result in reh:rel
;

ret


; Serial protocol handling..


SerialStart :

    ; Set up pointer to serial buffer, we start by pointing to the end so that
    ; we need a valid 0xff to start the first command...

    ldi XL, LOW(SERIAL_DATA_END)       
    ldi XH, HIGH(SERIAL_DATA_END)


SerialLoop:

    sbis USR, RXC                           ; check for serial data
    rjmp SerialLoop

    in serialChar, UDR                      ; read the character

    cpi serialChar, 0xFF                    ; is it the start character?
    brne SerialDataCharacter

    ldi XL, LOW(SERIAL_DATA_START)       
    ldi XH, HIGH(SERIAL_DATA_START)

    rjmp SerialLoop                         ; wait for a data character...

SerialDataCharacter :

    cpi XL, SERIAL_DATA_END                 ; if we have already filled our buffer space 
    breq SerialError                        ; that's an error

    st X+, serialChar

    cpi XL, SERIAL_DATA_END                 ; have we filled the buffer?
    breq SerialProcessCommand

    rjmp SerialLoop


SerialError :

    ; we could light a led and only unlight it on valid data...

    rjmp SerialStart

SerialProcessCommand :

    ; we have a 2 byte serial command...

    ldi XL, LOW(SERIAL_DATA_START)       
    ldi XH, HIGH(SERIAL_DATA_START)

    ld temp20, X+

    cpi temp20, NUM_SERVOS                      ; check the servo index is valid
    brge SerialServoOutOfRange

    ld temp21, X+
    cpi temp21, 0xFF                            ; check the servo position is valid
    breq SerialServoOutOfRange
    

    ; deal with it...

    ; we have a servo index 0-NUM_SERVOS in temp20
    ; we have a control value 0-254 in temp21

    ldi XL, LOW(POSITION_DATA_START)       
    ldi XH, HIGH(POSITION_DATA_START)

    add XL, temp20
    brcc SerialStoreData        ; not really needed as our data area is so small..
    inc XH    


SerialStoreData:

    st X, temp21

    ; echo the command back to the sender...

    ldi XL, LOW(SERIAL_DATA_START)        
    ldi XH, HIGH(SERIAL_DATA_START)

    ldi serialChar, 0xFF
    rcall SendSerial
    
    ld serialChar, X+ 
    rcall SendSerial

    ld serialChar, X+ 
    rcall SendSerial

    rjmp SerialStart


SerialServoOutOfRange : 

    ; send the error message back to the sender...

    ldi XL, LOW(SERIAL_DATA_START)       
    ldi XH, HIGH(SERIAL_DATA_START)

    ldi serialChar, 0xFF
    rcall SendSerial
    
    ld serialChar, X+ 
    rcall SendSerial

    ldi serialChar, 0xFF
    rcall SendSerial

    rjmp SerialStart


    ; Send the contents of serialChar out of the serial port...

SendSerial : 

    sbis UCSRA,UDRE             ; wait for transmitter ready
    rjmp SendSerial

    out UDR, serialChar

    ret



