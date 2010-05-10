//======================================================
//   (c)2008-9 Matthew Ford
//   Forward Computing and Control Pty. Ltd.
//   www.forward.com.au
//   All rights reserved
//
//   You may freely copy and distribute this program for 
//           commercial and non-commercial use provided:-
//   i) this copyright notice remains attached
//   ii) you clearly mark any changes you make to the code
//======================================================
//
// RemoteLedDriver.asm
//
// Main:  ATtiny84
//      mode: ISP mode
//
// Program:  ATtiny84
//    Flash - Input HEX File: yourdir/RemoteLedDriver.hex
//    EEPROM - not used
//
// Fuses:
//    SPIEN    ticked
//    BODLEVEL  Brown Out detection at VCC = 1.8V
//    CKDIV8   ticked
//    SUT_CKSEL  Int RC Osc. 8Mhz; .... 6CK/14CK +64mS
//    Auto Read  ticked
//    Smart Warnings  ticked
//    Verify After programming  ticked
//  then Progam the fuses
//
// LockBits:  
//    LB  no lock features enabled
// 
// Advanced:
//   Calibrate of frequency 8.0Mhz
//  Do Not write any value to the Cal byte.  startup handles this 
//
// HW Setting
//   VTarget  5.1 
//   VREF 5.1 (not used)
//   Clock Generator 3.686Mhz (max)
//
//  Auto:
//  Erase Device
//  Check Signature
//  Program FLASH
//  Verify FLASH
//---------------------------------------------------------
//
// The constants N and C determine the data rate. C selects clock frequency
// the following expression yields the data rate:
//
//            Clk_freq
//     BAUD =  ------     x 1/N 
//             C        
//  
//
// Absolute minimum value for N*C is about 10 (which causes the 
// timer to interrupt to be set again before the interrupt is finished).
//  Absolute maximum is 170.
// (Caused by the 1.5bit-lenght that is necessary to receive bits correctly.)
//
// The RS232 uses SERIAL_OUT_PIN as the transmit pin 
// The receive pin is  SERIAL_IN_PIN 
// You need to adjust the other .equ to match the pins you choose.
//
//
// Since this RS232 module is half duplex, it can either send or recieve data.
// It can't do both simoutaneausly.
//
// To use first call RS232_INIT. (with interrupts disabled)
// Then either call RS232_SET_TO_RECEIVE to wait for a character to arrive
// or RS232_TRANSMIT to transmit the bytes to have previously stored in the
// RS232_BUFFER_?? To transmit you also need to set the RS232_SEND_COUNT
// which is the number of bytes to be send from 1 to RS232_MAX_SEND_BUFFER
//
//
//*************************************************************************

// 1 --  Copy from BluetoothLedDriverDecimal
// 2 --  Added 5sec timer for ADC reading and changed reading to %
// 3 --  Added BRANCH_IF_TRANSMITTING macro and BRANCH_IF_RS232_BUSY
// 4 --  Added error msg and send Amps also but amps not working 
// 5 --  Fixed error msg and send Amps 
// 6 --  Removed IamAlive and added i,I for amps 
// 7 --  Added macros for NEW_LINE and LOAD_CHARS_FROM
// 8 --  Used macros for all msg loads
// 9 --  Added Level setpoint lookup. Still need to do 0,1,2,3 settings
// 10 -- Added 0,1,2,3 settings
// 11 -- Adjusted levels for percived brightness changes
// 12 -- Minor formatting
// 13 -- Corrections to Comments
// 
.include "tn84def.inc"

// start interrupt routine
.macro SAVE_SREG
   push Temp
   in Temp, SREG    // save the SREG
   push Temp
.endmacro

// end interrupt routine
.macro RESTORE_SREG
   pop Temp
   out SREG, Temp    // restore the SREG
   pop Temp
.endmacro

//--------------------------------------------------------
// NEW_LINE
// insert a new line in the Tx buffer
// either Cr or CrLf or just Lf
// edit this macro to suit
// Assumes XH:XL has been set to point to the RS232 Buffer 
// Decrements XH:XL before storing each char
//--------------------------------------------------------
.macro NEW_LINE
    ldi Temp, 13 // Cr
    st -X, Temp  // store leading Cr
 //   ldi Temp, 10 // Lf
 //   st -X, Temp  // store leading Lf
.endmacro

//------------------------
// BRANCH_IF_TRANSMITTING
// usage
// BRANCH_IF_TRANSMITTING labelToBranchToIfTransmitting
// else just continue to next statement after this macro
// uses Temp register
// -----------------------
.macro BRANCH_IF_TRANSMITTING // one argument
    lds Temp, RS232_STATUS
    // select just the busy and send bits
    andi Temp, (1<<RS232_STATUS_BUSY) | (1<<RS232_STATUS_SEND)
    // check if busy ==1 and send ==1
    cpi Temp, (1<<RS232_STATUS_BUSY) | (1<<RS232_STATUS_SEND)
    breq (@0) // skip to this label if busy and sending
.endmacro

//------------------------
// BRANCH_IF_RS232_BUSY
// usage
// BRANCH_IF_RS232_BUSY labelToBranchToIfTransmitting
// else just continue to next statement after this macro
// uses Temp register
// -----------------------
.macro BRANCH_IF_RS232_BUSY // one argument
    lds Temp, RS232_STATUS
    // select just the busy and send bits
    sbrc Temp, RS232_STATUS_BUSY
    // check if busy ==1 
    rjmp (@0) // skip to this label if busy 
    // else skip to here if busy == 0
.endmacro

//-------------------------------
// LOAD_CHARS_FROM
// usage
// LOAD_CHARS_FROM  Sram_Label
// first byte holds the number of chars to load
// chars follow.  
// Assumes XH:XL has been set to point to the RS232 Buffer 
// Decrements XH:XL before storing each char
//
// Note: the .db must have an even number of bytes
// even if you do not load them all
//
// All registers used here are saved and restored
//---------------------------------
.macro LOAD_CHARS_FROM // one argument 
//1849:         push CharCount
PUSH      R15          //  Push register on stack
//1850:         push Temp
PUSH      R16        //    Push register on stack
//1851:         push ZH
PUSH      R31        //    Push register on stack
//1852:         push ZL
PUSH      R30        //    Push register on stack
//1854:         ldi ZH, high(2*Error_Msg_Line1)
LDI       R31,high(2*(@0))    //   Load immediate
//1855:         ldi ZL, low(2*Error_Msg_Line1)     // Set pointer to RAM data
LDI       R30,low(2*(@0))     //  Load immediate
//1856:         lpm Temp2, Z+    // this is the number of chars to load
LPM       R15,Z+      //   Load program memory and postincrement
//@000002F7: Loop
//:         lpm Temp, Z+
LPM       R16,Z+      //   Load program memory and postincrement
//1859:         st -X, Temp
ST        -X,R16      //   Store indirect and predecrement
//1860:         dec Temp2
DEC       R15         //   Decrement
//1861:         brne LOOP
BRNE      PC-0x03     //   Branch if not equal
//@000002FB: END_LOAD_CHARS
//1864:         pop ZL
POP       R30        //    Pop register from stack
//1865:         pop ZH   
POP       R31        //    Pop register from stack
//1866:         pop Temp
POP       R16       //     Pop register from stack
//1867:         pop CharCount
POP       R15       //     Pop register from stack
.endmacro
//--------------------------------------------------


// set the uC clock calibration
// I have allowed for two steps incase you need to move the default more the 0x20
// my default was 0x9E so only one step was needed
// in this case just set step 2 to equal step 1
.equ OSC_CAL_STEP_1 = 0xA5  // first adjustment from the default
.equ OSC_CAL_STEP_2 = 0xA5  // second adjustment from the default.

// 9600 at 8MHz 
.equ    N=103    // == ((8000000/9600) / C) - 1   
.equ    C=8      //    counter prescaler == Divisor
                 // ((0<<CS02)|(1<<CS01)|(0<<CS00))  for divide by 8
                 // the -1 in the above equation is because the counter
                 // counts from 0 to N inclusive (i.e. N+1 counts)

// *********** SERIAL OUTPUT DEFINES
.equ SERIAL_OUT_PIN = PA7 // define name for rs232 output
 // set these to match SERIAL_OUT_PIN
.equ SERIAL_OUT_PORT = PORTA  
.equ SERIAL_OUT_DDR = DDRA

// ************ SERIAL INPUT DEFINES
.equ SERIAL_IN_PIN = PB2  // define name for rs232 input
// set these to match SERIAL_IN_PIN
.equ SERIAL_IN_PORT = PORTB 
.equ SERIAL_IN_PORTIN = PINB
.equ SERIAL_IN_DDR = DDRB
.equ SERIAL_IN_GROUP_INT = PCIE1 // Group enable interrupt for SERIAL_IN_PIN (PCINT10)
.equ SERIAL_IN_PIN_INT = PCINT10 // pin interrupt to PB2
.equ SERIAL_IN_INTERRUPT_MASK = PCMSK1 // where serial_in_pin_int is set
.equ SERIAL_IN_GROUP_INT_FLAG = PCIF1 
// flag set if any of enabled pin in PCINT8-11 changes level 
// PIN_CHANGE_INT has to be set appropiate interrupt addres either PCI1addr or PCI0addr
// in this case it is PCI1addr see //  INTERRUPT VECTORS below

.equ SW_B = PB1  // define name for switch input pin
.equ uC_OUTPUT_B = PB0  // define name for uC output pin

.equ CURRENT_SP_MAX = 1000 // MAX CURRENT SP
.equ CURRENT_SP_HIGH = 1000  // the current setpoint, 1000 = full scale current, approx 500mA
.equ CURRENT_SP_MED = 333  // the current setpoint, 333 = approx 167mA
.equ CURRENT_SP_LOW = 111  // the current setpoint, 111 = approx 55mA

.equ LEVEL_LOW = 4
.equ LEVEL_MED = 7
.equ LEVEL_HIGH = 10

// sets the maximum number of bytes that can sent at one time
.equ RS232_MAX_SEND_BUFFER = 30

.equ NO_GLOBAL_VARS = 42 // number of globals available
// this MUST be greater then the number of globals defined below
// otherwise the stack will be overridded by the globals

.equ RS232_STATUS = RAMEND-0 // the STATUS of the RS232
//**** RS232_STATUS bits
   .equ RS232_STATUS_NEXT_BIT = 0 // the next bit to send or the bit just received
   .equ RS232_STATUS_TX_COMPLETE = 3 // trigger, set if have completed send, cleared by RS232_TRANSMIT and RS232_INIT and RS232_SET_TO_RECEIVE
   .equ RS232_STATUS_SET_TO_RECEIVE = 4 // set if SET_TO_RECEIVE has been called and have not started to receive a byte, cleared by call to RS232_TRANSMIT and RS232_INIT
   .equ RS232_STATUS_DATA_RECEIVED=5    // trigger, set when data byte received, cleared by call to RS232_SET_TO_RECEIVE or RS232_INIT
   .equ RS232_STATUS_SEND=6    // Sending data ==1, receiving == 0  only valid when RS232_STATUS_BUSY == 1
   .equ RS232_STATUS_BUSY=7    // 1== busy sending or receiving see RS232_STATUS_SEND to find out which
//**** END RS232_STATUS bits

.equ RS232_BIT_COUNT = RAMEND-1 // the number of bits sent/recieved on the RS232
.equ RS232_RECEIVE_BYTE = RAMEND-2 // byte being received
.equ RS232_SEND_COUNT = RAMEND-3 // the number of bytes to send from RS232_BUFFER_
.equ RS232_SEND_BYTE = RAMEND-4 // the byte being sent
.equ RS232_BUFFER_PTR_HIGH = RAMEND-5  // holds the pointer to the send buffer high
.equ RS232_BUFFER_PTR_LOW  = RAMEND-6  // holds the pointer to the send buffer low

// send buffer runs from RS232_SEND_BYTE -1 to RS232_SEND_BYTE - RS232_MAX_SEND_BUFFER 
.equ RS232_BUFFER_1 = RAMEND-7 // the first to send
.equ RS232_BUFFER_2 = RAMEND-8 // the second to send
.equ RS232_BUFFER_3 = RAMEND-9 
.equ RS232_BUFFER_4 = RAMEND-10 
.equ RS232_BUFFER_5 = RAMEND-11
.equ RS232_BUFFER_6 = RAMEND-12 
.equ RS232_BUFFER_7 = RAMEND-13 
.equ RS232_BUFFER_8 = RAMEND-14 
.equ RS232_BUFFER_9 = RAMEND-15 
.equ RS232_BUFFER_10 = RAMEND-16 
.equ RS232_BUFFER_11 = RAMEND-17 
.equ RS232_BUFFER_12 = RAMEND-18 
.equ RS232_BUFFER_13 = RAMEND-19 
.equ RS232_BUFFER_14 = RAMEND-20 
.equ RS232_BUFFER_15 = RAMEND-21 
.equ RS232_BUFFER_16 = RAMEND-22 
.equ RS232_BUFFER_17 = RAMEND-23 
.equ RS232_BUFFER_18 = RAMEND-24 
.equ RS232_BUFFER_19 = RAMEND-25 
.equ RS232_BUFFER_20 = RAMEND-26 // 20th byte to send
.equ RS232_BUFFER_21 = RAMEND-27 
.equ RS232_BUFFER_22 = RAMEND-28 
.equ RS232_BUFFER_23 = RAMEND-29 
.equ RS232_BUFFER_24 = RAMEND-30 
.equ RS232_BUFFER_25 = RAMEND-31 
.equ RS232_BUFFER_26 = RAMEND-32 
.equ RS232_BUFFER_27 = RAMEND-33 
.equ RS232_BUFFER_28 = RAMEND-34 
.equ RS232_BUFFER_29 = RAMEND-35 
.equ RS232_BUFFER_30 = RAMEND-36 // 30th byte to send

.equ ADC_HIGH = RAMEND-37 // storage for adc measurements
.equ ADC_LOW = RAMEND-38
.equ HZ_2_COUNTER = RAMEND-39 // counter for 2hz timer

.equ IamAlive_COUNTER = RAMEND-40  // counter for 10sec timer
.equ GLOBAL_42 = RAMEND-41   // 42nd global counting 0 

.def Level = r14 // the current Led level for lookup of setpoint
.equ Level_Max = 10 // max number of levels
// min level is 0

.def Temp2 = r15 // temp reg
.def Temp = r16// Temporary register
  
//------------------------------------------------
// TRIGGER_Flags  Flags set for triggers
// This register is updated by both interrupts and main code
//------------------------------------------------
.def TRIGGER_Flags = r17  // initially 0 no triggers set
    .equ TRIGGER_SwitchChanged = 0 // bit0, 1 if switch just changed else 0
    .equ TRIGGER_Timer1 = 1  // set by 2mS Timer1 
    .equ TRIGGER_2Hz = 2 // set by 2hz timer
    .equ TRIGGER_IamAlive = 3 // set by 10sec timer
    .equ TRIGGER_Send_Error_Msg = 4 // set if need to send error msg
    .equ TRIGGER_Light = 5 // send light level
    .equ TRIGGER_Amps = 6 // send amps
    .equ TRIGGER_Level = 6 // send amps

// Debounce Counter Counts allows up to 0.5sec debounce = 250 count
.def DEBOUNCE_Counter = r18 //  count debounce timeout
    .equ Debounce_Count_Low = 5 // about 0.01 sec  = 5 x 2mS
    .equ Debounce_Count_High = 5 // about 0.01 sec  = 5 x 2mS
// increase the appropiate count if you switch needs a longer debouce in one direction

//------------------------------------------------
//SW_Flags,  Holds the current debounced switch state and last state and change trigger
//------------------------------------------------
.def SW_Flags = r19  // 0b00000000 initially i.e. Switch up and last switch up
    .equ SW_SWDown = 0   // bit0, 1 if button pressed, 0 if button released.
    .equ SW_SWLastInput = 1  // bit1,  1 if last read SW pressed (low) not debounced
//------------------------------------------------

//------------------------------------------------
//New_Torch_State,  Holds the new state of the torch
// updated from current Torch_State by triggers
//------------------------------------------------
.def New_Torch_State = r20 

//------------------------------------------------
//Torch_State,  Holds the current state of the torch
//------------------------------------------------
.def Torch_State = r21   // initially 0 == off
.equ Torch_State_OFF = 0  // the torch state constants
.equ Torch_State_LOW = Torch_State_OFF+1 // 1
.equ Torch_State_MED = Torch_State_LOW+1 // 2
.equ Torch_State_HIGH = Torch_State_MED+1 // 3
.equ Torch_State_EXCEEDED = Torch_State_HIGH+1 // 4 first invalid state when switching levels
.equ Torch_State_VARIABLE = Torch_State_EXCEEDED  // 4 variable levels

// registers to hold current setpoint
.def SP_Low = r22  // holds the low byte of the setpoint current
.def SP_High = r23 // holds the high byte of the setpoint current

.def ADCLow = r24 // low byte of adc
.def ADCHigh = r25 // high byte of adc

/*********
.def    XH    = r27
.def    XL    = r26
.def    YH    = r29
.def    YL    = r28
.def    ZH    = r31
.def    ZL    = r30
***********/

//*************************************************************************
//
//    PROGRAM START - EXECUTION STARTS HERE
//
//*************************************************************************
.cseg
//  INTERRUPT VECTORS 
.org 0x0000
   rjmp RESET
.org  PCI1addr  // pin change interrupt 1 for PCINT10 
   rjmp  PIN_CHANGE_INT
.org OC1Aaddr    // = 0x0006  TIMER1 COMPARE A MATCH
  rjmp TIMER1_COMPARE_A_INT
.org  OC0Aaddr   // counter time 0 Compare A match
   rjmp TIMER0_CMP_A_INT


//**************************************************************************
// PIN_CHANGE_INT 
//
// Called when level changes on SERIAL_IN_PIN, can be a change up or down
// so need to check at the top of this call.
// Looking for start bit ==> low -> high on pin input (ie 1 -> 0) idle is 1
// So check for port input == 1 at top and just return if not.
// Otherwise start the timer to sample in 1.5 bits
// Disable this pin interrupt and clear the interrupt flag
//
// RS232_STATUS_SET_TO_RECEIVE is cleared by this routine
// RS232_STATUS_BUSY is set by this routine
// RS232_STATUS_SEND is cleared by this routine
// 
// When receive completes RS232_STATUS_DATA_RECEIVED will be set
// Clear this flag when you have handled the data
//
//*************************************************************************
PIN_CHANGE_INT:
    SAVE_SREG  // save Temp and SREG

    // see if pin low or high
    sbic SERIAL_IN_PORTIN, SERIAL_IN_PIN   // if SERIAL_IN_PIN low == 1 (set)  just return 
    rjmp END_PIN_CHANGE_INT
    // else want to get the timer started as close as possible
    // to the leading edge on the input

    // start it  with divide by 8
    // this assumes that OCR0A is set to some resonably high value
    // and that timer0 is cleared, RS232_INIT and RS232_STOP should do this
    in Temp, TCCR0B
    sbr Temp, (1<<CS01)  // CS02==0,CS01=1,CS00=0
    out TCCR0B , Temp   // clock select /8 == 1mhZ
    // OK now counting
   
    // set the upper
    ldi Temp,((N+N/2)-(33/C))
    //Set timer reload-value to 1.5 time the bit length
    // and subtract the clocks used so far divided by 
    // the time clock divisor
    // the 1.5 is so that the fisrt sample is 
    // in the middle of the first bit after the start bit
    out OCR0A , TEMP  // set top counter

    in Temp, TIMSK0
    sbr Temp, (1<<OCIE0A)
    out TIMSK0, Temp      // enable COMPARE A overflow interrupt
    in  Temp, TIFR0
    sbr Temp, (1<<OCIE0A)
    out    TIFR0,Temp     // to clear COMPARE A overflow flag

    // the following checks could be added
    // note if already busy here then still receiving/sending !! error should not happen
    // if already receiving here then have not read previous byte received PROCESSING TOO SLOW
    ldi Temp, (1<<RS232_STATUS_BUSY) // set the busy flag and clear the rest
    // clears RS232_STATUS_SET_TO_RECEIVE as have disabled pin interrupt in this interrupt
    sts RS232_STATUS, Temp  // save the status byte

    clr Temp
    sts RS232_BIT_COUNT, Temp  // 0 is first bit which is start bit which is skipped
    // so have already processed it here

    in Temp, SERIAL_IN_INTERRUPT_MASK 
    cbr Temp, (1<<SERIAL_IN_PIN_INT) // pb2 disable pin chage interrupt
    out SERIAL_IN_INTERRUPT_MASK, temp 

    in Temp, GIFR
    sbr Temp,(1<<SERIAL_IN_GROUP_INT_FLAG) 
    out GIFR,Temp ; clear pending pin chage 1 int. 


END_PIN_CHANGE_INT:
    RESTORE_SREG  // restore Temp and SREG and reti
reti              // return from interrupt


//*************************************************************************
//
// TIMER1_CMP_A_INT
// 
//*************************************************************************
TIMER1_COMPARE_A_INT:
    SAVE_SREG
    // Temp not used here but leave extra push in as starndard interrupt preamble 
    sbr TRIGGER_Flags, (1<<TRIGGER_Timer1)  // modifies SREG
    RESTORE_SREG
reti

//*************************************************************************
//
// TIMER0_CMP_A_INT
// 
// This interrupt is called when the timer reaches its Compare A value
// The time then restarts from 0 automatically while these statements are
// being executed so it stays in time.
//
// If sending i.e. RS232_STATUS_SEND set
// set the output pin based on the RS232_STATUS_NEXT_BIT flag
// if RS232BitCount is 9 this is the 9th bit output the stop bit
// if RS232BitCount is 10 then next byte to send or stop if no more
// otherwise set the RS232_STATUS_NEXT_BIT based on bit 0 of the RS232_SEND_BYTE
// and shift the bit out.
// When transmit complete set RS232_STATUS_TX_COMPLETE
// and clear RS232_STATUS_SET_TO_RECEIVE, RS232_STATUS_BUSY and RS232_STATUS_SEND
//
// If receiving 
// sample the pin level and set the carry flag and save the SREG
// because the next comparison will update the carry flag.
// if this is the 9th bit, stop
// otherwise restore SREG and the carry and rotate into the RS232_RECEIVE_BYTE
// When 9th bit (the stop bit is sampled)
// set RS232_STATUS_DATA_RECEIVED
// and clear RS232_STATUS_SET_TO_RECEIVE, RS232_STATUS_BUSY and RS232_STATUS_SEND 
// 
// 
//*************************************************************************
// These two registers are used locally in this interrupt
// They are saved and restored
// Temp r16 also used and restored
.def RS232StatusReg = r17  // used in interrupt saved and restored
.def RS232BitCount = r18   // used in interrupt saved and restored

TIMER0_CMP_A_INT:
    SAVE_SREG  // save Temp and SREG

    push RS232StatusReg
    push RS232BitCount

    lds RS232StatusReg, RS232_STATUS
    lds RS232BitCount, RS232_BIT_COUNT
    inc RS232BitCount  // sending/receiving next bit 

    // are we sending or receiving
    sbrs RS232StatusReg, RS232_STATUS_SEND
    rjmp TIMER0_CMP_A_RECEIVE  // jump if RS232_STATUS_SEND clear
    // else Sending

TIMER0_CMP_A_SEND:
    // start bit was set in transmit call
    // so here can send 
    // set next output bit from status RS232_STATUS_NEXT_BIT 
    sbrc RS232StatusReg, RS232_STATUS_NEXT_BIT    // if RS232_STATUS_NEXT_BIT is 1
    sbi  SERIAL_OUT_PORT,SERIAL_OUT_PIN        //   Set transmit to 1
    sbrs RS232StatusReg, RS232_STATUS_NEXT_BIT    // if RS232_STATUS_NEXT_BIT is 0
    cbi  SERIAL_OUT_PORT,SERIAL_OUT_PIN        //  Set transmit to 0

    // counter == number of bits sent
    // if counter == 9 then send stop bit
    // if counter > 9 stop
    // else set up to send next bit
    cpi RS232BitCount, 9
    breq TIMER0_CMP_A_STOP_BIT // == 9 send stop bit
    brsh TIMER0_CMP_A_STOP_SEND  // > 9 stop

    // else set next bit to next bit in byte
    // the code below takes the same time for either 0 or 1 next bit
    lds Temp, RS232_SEND_BYTE
    sbrc Temp,0
    sbr RS232StatusReg,(1<<RS232_STATUS_NEXT_BIT)
    sbrs Temp,0
    cbr RS232StatusReg,(1<<RS232_STATUS_NEXT_BIT)
    lsr Temp  // shift this bit out of byte
    sts RS232_SEND_BYTE, Temp
    rjmp END_TIMER0_CMP_A    //  jump to exit

TIMER0_CMP_A_STOP_BIT:
    // set next bit to 1
    sbr RS232StatusReg,(1<<RS232_STATUS_NEXT_BIT)
    rjmp END_TIMER0_CMP_A    //  jump to exit


TIMER0_CMP_A_STOP_SEND:
    // check if there is another byte to send
    lds Temp, RS232_SEND_COUNT
    dec Temp;
    sts RS232_SEND_COUNT, Temp

    breq TIMER0_CMP_A_SEND_END // zero so finished
    // else next byte      
    // clear next bit start bit
    andi RS232StatusReg, ~(1<<RS232_STATUS_NEXT_BIT)
    clr RS232BitCount

    // set up the byte to be sent
    // using ZH, ZL locally here so save and restore
    push ZH  // save register values
    push ZL
    lds ZH, RS232_BUFFER_PTR_HIGH
    lds ZL, RS232_BUFFER_PTR_LOW
    ld Temp, -Z // decrement before loading
    sts RS232_SEND_BYTE,Temp
    sts RS232_BUFFER_PTR_LOW, ZL
    sts RS232_BUFFER_PTR_HIGH, ZH
    pop ZL  // restore them
    pop ZH
    rjmp END_TIMER0_CMP_A    //  jump to exit

TIMER0_CMP_A_RECEIVE:
    sec            // Set carry
    sbis SERIAL_IN_PORTIN,SERIAL_IN_PIN    // <=== SAMPLE HERE
    clc        //  clear carry
    in Temp, SREG  // save this bit

    cpi RS232BitCount,9    //if RS232BitCount < 9 (must sample stop-bit)
    brlo TIMER0_CMP_A_RECEIVE_SAVE_BIT    // more bits to come

    // else finished do not save stop bit just set data received flag
    sbr RS232StatusReg,(1<<RS232_STATUS_DATA_RECEIVED)  // data receieved
    // leave receiving flag set until byte buffer read
    rjmp TIMER0_CMP_A_STOP  // stop timer and clear interrupts

TIMER0_CMP_A_RECEIVE_SAVE_BIT:
    out SREG,Temp
    lds Temp, RS232_RECEIVE_BYTE
    ror Temp    // rotate carry into data, right most bit goes to carry
    sts RS232_RECEIVE_BYTE, Temp
    rjmp END_TIMER0_CMP_A

TIMER0_CMP_A_SEND_END:
    sbr  RS232StatusReg, (1<<RS232_STATUS_TX_COMPLETE) // set trigger
    // rjmp TIMER0_CMP_A_STOP drop through

TIMER0_CMP_A_STOP:
    rcall RS232_STOP 
    cbr RS232StatusReg,(1<<RS232_STATUS_SET_TO_RECEIVE)|(1<<RS232_STATUS_BUSY)|(1<<RS232_STATUS_SEND)
    // not busy or sending or set to recieve
    // rjmp END_TIMER0_CMP_A drop through

END_TIMER0_CMP_A:
    ldi Temp, N
    out OCR0A, Temp  // set top compare for one byte time

    sts RS232_BIT_COUNT, RS232BitCount
    sts RS232_STATUS, RS232StatusReg  // save the current status

    pop RS232BitCount
    pop RS232StatusReg
    RESTORE_SREG  // restore Temp and SREG and reti
reti              // return from interrupt

//*****************************************************************
//
// RS232_SET_TO_RECEIVE
//
// If RS232_STATUS_BUSY or already RS232_SET_TO_RECEIVE then
// just return as nothing to do, either already receiving or sending
// and send has precedence, or have already set to receive
// Otherwise enable the pin change interrupt and clear the flag
// Set RS232_STATUS_SET_TO_RECEIVE, this will be cleared when start bit detected
// or when a transmit is started.
// Clear RS232_STATUS_TX_COMPLETE and RS232_STATUS_BUSY and RS232_STATUS_DATA_RECEIVED
// 
// Only Temp is used
// It is saved and restored here
//
//*****************************************************************
RS232_SET_TO_RECEIVE:
    SAVE_SREG  // save Temp and SREG
    cli // prevent interrupt here
        
    lds Temp, RS232_STATUS
    sbrc Temp, RS232_STATUS_BUSY 
    rjmp END_RS232_SET_TO_RECEIVE // if still busy skip this
    // either still sending or receiving a char
    sbrc Temp, RS232_STATUS_SET_TO_RECEIVE // have already called this so just return
    // prevents clearing of pending pin change below
    rjmp END_RS232_SET_TO_RECEIVE // if already called skip this

    rcall RS232_STOP

    in Temp, SERIAL_IN_INTERRUPT_MASK // pin change mask reg 
    sbr Temp, 1<<SERIAL_IN_PIN_INT // pb2 enble pin chage interrupt
    out SERIAL_IN_INTERRUPT_MASK, Temp 

    in Temp, GIFR
    sbr Temp,1<<SERIAL_IN_GROUP_INT_FLAG 
    out GIFR,Temp ; clear pending pin change int. 

    // clear trigger and set to receive
    // set to receive clears all other status bits
    // including RS232_STATUS_TX_COMPLETE  trigger if set
    // and RS232_STATUS_DATA_RECEIVED and RS232_STATUS_BUSY
    ldi Temp, (1<<RS232_STATUS_SET_TO_RECEIVE)
    sts RS232_STATUS, Temp // clear RS232Status
END_RS232_SET_TO_RECEIVE:
    RESTORE_SREG  // restore Temp and SREG and global interrupt flag
ret

//*********************************************************
//  RS232_STOP 
// Should only be called inside interrupt disabled block
// This is a utility routine to disable the interrupts and
// stop the TIMER0 clock
//
// Only Temp is used
// It is assumed the caller has saved Temp
// prior to this call
//**********************************************************
RS232_STOP:
    sbi SERIAL_OUT_PORT,SERIAL_OUT_PIN        //   Set transmit to 1

    in Temp, SERIAL_IN_INTERRUPT_MASK 
    cbr Temp, (1<<SERIAL_IN_PIN_INT) // pb2 disable pin chage interrupt
    out SERIAL_IN_INTERRUPT_MASK, temp 

    in Temp, GIFR
    sbr Temp,(1<<SERIAL_IN_GROUP_INT_FLAG) 
    out GIFR,Temp ; clear pending pin chage 1 int. 

    // stop the clock
    in Temp, TCCR0B
    cbr Temp, (1<<CS02)|(1<<CS01)|(1<<CS00)
    out TCCR0B , Temp   // stop the clock

    // set CTC mode in the counter
    in Temp,TCCR0A
    sbr Temp, (1<<WGM01)
    cbr Temp, (1<<WGM00)
    out TCCR0A, Temp

    ldi Temp, N  // set the compare to N
    out OCR0A, Temp

    // clear the timer
    clr Temp 
    out TCNT0,Temp    

    in  Temp, TIMSK0
    cbr Temp, (1<<OCIE0A) // disable Compare A interrupt on timer 0
    out TIMSK0,Temp           
    
    in Temp, TIFR0
    sbr Temp, (1<<OCF0A) // clear Compare A interrupt flag on timer 0
    out TIFR0, Temp
ret

//*************************************************************************
// RS232_INIT 
// Initializes the RS232
// Should be called with interrupts disabled.
//
// Sets up input and output pin and timer and pin change interrupt group
// Need to call either 
// RS232_SET_TO_RECEIVE or 
// RS232_TRANSMIT
// to actually send or receive
//
// Saves and restores Temp register and SREG
//*************************************************************************
RS232_INIT:    
    SAVE_SREG
    // setup the output pin
    // leave other pins unchanged
    in Temp, SERIAL_OUT_PORT
    sbr Temp, (1<<SERIAL_OUT_PIN) // set pin high
    out SERIAL_OUT_PORT, Temp        
    in Temp, SERIAL_OUT_DDR
    sbr Temp, (1<<SERIAL_OUT_PIN) // set as output
    out SERIAL_OUT_DDR, Temp 

    // set the input pin
    // leave other pins unchanged
    in Temp, SERIAL_IN_PORT
    sbr Temp, (1<<SERIAL_IN_PIN) // set pull up  
    out SERIAL_IN_PORT,Temp
    in Temp, SERIAL_IN_DDR
    cbr Temp, (1<<SERIAL_IN_PIN) // clear to set as input 
    out SERIAL_IN_DDR,Temp

    rcall RS232_STOP  // setup the RS232 for next send or receive
    clr Temp // clear all flags and triggers 
    sts RS232_STATUS, Temp

    in Temp, GIMSK
    sbr Temp, (1<<SERIAL_IN_GROUP_INT)
    out GIMSK, Temp   // enable pin change interrup for this group if not already enabled

    in Temp, GIFR
    sbr Temp,1<<SERIAL_IN_GROUP_INT_FLAG 
    out GIFR,Temp ; clear pending pin change int. 

    clr Temp
    sts RS232_STATUS, Temp // clear RS232Status
    RESTORE_SREG
ret

//*************************************************************************
// RS232_TRANSMIT 
//  
// Assumes RS232_BUFFER_1 to n globals have already set up.
// and that RS232_SEND_COUNT contains the number of bytes to send.
// Load the first byte into RS232_SEND_BYTE
// Set up the buffer pointers to point to the first byte
// Clear RS232_STATUS_NEXT_BIT for the first start bit
// Clear RS232_STATUS_SET_TO_RECEIVE and RS232_STATUS_TX_COMPLETE
// Set RS232_STATUS_SEND and RS232_STATUS_BUSY
// Load half a bit time into the timer to clear the last half bit of any
// previous receive (for the other ends benifit)
// and enable the timer interrupt and start the timer.
//
// Transmit takes precedence over receive
// if receiving now then receiving will be abandoned and transmit will start
//
// Always send at least one byte and at most RS232_MAX_SEND_BUFFER bytes
//
// All registers (including Temp) preserved by this call
//
//*************************************************************************
RS232_TRANSMIT:
    // Temp holds byte to transmit
    SAVE_SREG
    cli // prevent interrupt here 

    lds Temp, RS232_SEND_COUNT
    // make sure tx count is within range of 1 to max send buffer
    cpi Temp, 1
    brsh RS232_TX_ONE
    ldi Temp, 1 // send one 
RS232_TX_ONE:
    cpi Temp, RS232_MAX_SEND_BUFFER+1
    brlo RS232_TX_MAX
    ldi Temp, RS232_MAX_SEND_BUFFER

RS232_TX_MAX:
    sts RS232_SEND_COUNT, Temp

    rcall RS232_STOP  // stop receive if runnning

    clr Temp
    sts RS232_BIT_COUNT, Temp

    lds Temp, RS232_BUFFER_1 // load first byte to send
    sts RS232_SEND_BYTE, Temp

    // set up the buffer pointers 
    ldi Temp, low(RS232_BUFFER_1)
    sts RS232_BUFFER_PTR_LOW, Temp
    ldi Temp, high(RS232_BUFFER_1)
    sts RS232_BUFFER_PTR_HIGH, Temp

    // if transmitting then ignore receives
    lds Temp, RS232_STATUS
    cbr Temp, ( (1<<RS232_STATUS_SET_TO_RECEIVE)|(1<<RS232_STATUS_NEXT_BIT)|(1<<RS232_STATUS_TX_COMPLETE))
    // clear trigger 
    sbr Temp, (1<<RS232_STATUS_SEND)|(1<<RS232_STATUS_BUSY)
    // leave  RS232_STATUS_DATA_RECEIVED as it is if have received char
    // it has been saved in RS232_RECEIVE_BYTE
    sts RS232_STATUS, Temp

    ldi    Temp,N/2  //Set timer delay before transmitting start bit
    // set half bit delay here assumes sender is half-duplex also
    // and it needs to finish sending its stop bit.
    out OCR0A , TEMP  // set top counter

    in Temp, TIMSK0
    sbr Temp, (1<<OCIE0A)
    out TIMSK0, Temp           // enable COMPARE A overflow interrupt
    in  Temp, TIFR0
    sbr Temp, (1<<OCIE0A)
    out    TIFR0,Temp        // to clear COMPARE A overflow flag
        
    // start the clock
    // start it  with divide by 8
    // this assumes that OCR0A is set to some resonably high value
    // and that timer0 is cleared
    in Temp, TCCR0B
    sbr Temp, (1<<CS01)  // CS02==0,CS01=1,CS00=0
    out TCCR0B , Temp   // clock select /8 == 1mhZ
    // OK now counting

    RESTORE_SREG // restores interrupt setting as well
ret



//-----------------------------------------------------
// CONVERT_13BITS_PERCENT
// Converts the, possiably signed, 13bit number in ZH:ZL 
// (actually -9999 to +9999)
// and save acsii char in global SRAM whose start is pointed
// to by XH:XL,
// XH:XL decremented BEFORE each store of a char
// If ZH:ZL is negative a leading '-' is stored
// 
// Format of number is determined by uncommenting
// one of the 3 #defines
// #define CONVERT_13BITS_PERCENT_LEFT
//  left justifies the result  eg |-21 and |2
// #define CONVERT_13BITS_PERCENT_RIGHT
//  right justifies the result eg |  -21 and |    2
// #define CONVERT_13BITS_PERCENT_ZEROS
//  right justifies with leading zeros eg |-0021 and | 0002
// if you uncomment more then one you will get a compile error (duplicate label)
//
// Numbers >9999 are output as 9999
// Numbers <-9999 are output as -9999
// 
// On return ZH:ZL is zero and XH:XL is updated to point to
// the last char stored
//
// all used registers pushed and popped (saved and restored) 
//-----------------------------------------------------
#define CONVERT_13BITS_PERCENT_LEFT
//#define CONVERT_13BITS_PERCENT_RIGHT
//#define CONVERT_13BITS_PERCENT_ZEROS

// register defines for CONVERT_13BITS_PERCENT
.def LeadingDigit = r17 
.def Negative = r18
 
CONVERT_13BITS_PERCENT:
    push Temp
    push LeadingDigit 
    push Negative

    clr Negative // no neg
    clr LeadingDigit // no leading digit yet

    // check if number to be converted is neg
    // if so prepend - and negate
    clr Temp
    cp ZL, Temp
    cpc ZH, Temp
    brpl CONVERT_13BITS_PERCENT_PAD_NEG_SIGN

    // else negate the input
    // Temp zero from above
    //clr Temp
    sub Temp, ZL
    mov ZL, Temp
    clr Temp
    sbc Temp, ZH
    mov ZH, Temp
    ldi Temp, 45 // '-'
    st -X, Temp // store -
    ldi Negative, 1 // set negative flag
    rjmp CONVERT_13BITS_PERCENT_CHECK_MAX_NUMBER

CONVERT_13BITS_PERCENT_PAD_NEG_SIGN:
   #ifndef CONVERT_13BITS_PERCENT_LEFT
    // store a space if you want to padd the '-' position of +ve number
    ldi Temp, 32
    st -X, Temp // store space
   #endif
CONVERT_13BITS_PERCENT_CHECK_MAX_NUMBER:
    // check against max value
    push XH
    push XL
    mov XH, ZH
    mov XL, ZL
    subi XL,  low(9999)
    sbci XH, high(9999)
    pop XL
    pop XH
    brsh CONVERT_13BITS_PERCENT_STORE_9999
    rjmp CONVERT_13BITS_PERCENT_START_CONVERSION
CONVERT_13BITS_PERCENT_STORE_9999:
    ldi Temp, 48 + 9 // '0' + 9
    st -X, Temp
    st -X, Temp
    st -X, Temp
    st -X, Temp
    rjmp END_CONVERT_13BITS_PERCENT

CONVERT_13BITS_PERCENT_START_CONVERSION:
    clr Temp
CONVERT_13BITS_PERCENT_LOOP_1000:
    subi ZL, low(1000)
    sbci  ZH, high(1000)
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_1000
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_1000 // try again
CONVERT_13BITS_PERCENT_ADD_BACK_1000:
    subi ZL, low(-1000)
    sbci  ZH, high(-1000)
CONVERT_13BITS_PERCENT_STORE_1000:
    rcall CONVERT_13BITS_PERCENT_STORE_DIGIT

    clr Temp
CONVERT_13BITS_PERCENT_LOOP_100:       
    subi ZL, low(100)
    sbci  ZH, high(100)
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_100
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_100  // try again
CONVERT_13BITS_PERCENT_ADD_BACK_100:
    subi ZL, low(-100)
    sbci  ZH, high(-100)
CONVERT_13BITS_PERCENT_STORE_100:
    rcall CONVERT_13BITS_PERCENT_STORE_DIGIT

    ldi LeadingDigit, 1 // force last two digits even if 0

    clr Temp
CONVERT_13BITS_PERCENT_LOOP_10:
    sbiw Z, 10 // use word subtraction for < 64       
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_10
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_10  // try again
CONVERT_13BITS_PERCENT_ADD_BACK_10:
    adiw Z, 10 // use word addition for < 64
CONVERT_13BITS_PERCENT_STORE_10:
    rcall CONVERT_13BITS_PERCENT_STORE_DIGIT

    ldi Temp, 46 // '.' 
    st -X, Temp // store decimal point

    clr Temp
CONVERT_13BITS_PERCENT_LOOP_1:       
    sbiw Z, 1 // use word subtraction for < 64       
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_1
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_1  // try again
CONVERT_13BITS_PERCENT_ADD_BACK_1:
    adiw Z, 1 // use word addition for < 64       
CONVERT_13BITS_PERCENT_STORE_1:
     // always store last digit even if 0
    subi Temp, -48 //  temp - (-48) 
    st -X, Temp // store 0 or 1's

    ldi Temp, 37 // '%'
    st -X, Temp // store the % 

END_CONVERT_13BITS_PERCENT:
    pop Negative
    pop LeadingDigit
    pop Temp
ret

//--------------------------
// CONVERT_13BITS_PERCENT_STORE_DIGIT
// Checks Temp against 0 and if not 0
// add 48 '0' and store in location 
// pointed to by XH:XL  (decremented first)
// see code comments for how to store
// leading zeros or padding blanks
//--------------------------
CONVERT_13BITS_PERCENT_STORE_DIGIT:
    //------------------------------
#ifdef CONVERT_13BITS_PERCENT_ZEROS
    // this section stores leading zeros
CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT:
    subi Temp, -48 //  temp - (-48) 
    st -X, Temp  // store
    rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT
#endif //CONVERT_13BITS_PERCENT_ZEROS
    //------------------------------

    //------------------------------
#ifdef CONVERT_13BITS_PERCENT_LEFT
   // this section has no padding
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq END_CONVERT_13BITS_PERCENT_STORE_DIGIT // skip zero

CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT
#endif //CONVERT_13BITS_PERCENT_LEFT
   //------------------------------

   //------------------------------
#ifdef CONVERT_13BITS_PERCENT_RIGHT
   // this section padds with leading spaces
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq CONVERT_13BITS_PERCENT_STORE_ZERO_DIGIT // handle zero

CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT

CONVERT_13BITS_PERCENT_STORE_ZERO_DIGIT:
   // if negative move the - to the right
   tst Negative
   brne CONVERT_13BITS_PERCENT_STORE_PADDING // negative store over neg
   // else no negative dec X before store
   sbiw X, 1 
CONVERT_13BITS_PERCENT_STORE_PADDING:
   ldi Temp, 32 // ' '
   st X, Temp
   // restore neg if there is one
   tst Negative
   breq END_CONVERT_13BITS_PERCENT_STORE_DIGIT // not negative nothing to do
   // store neg in next location
   ldi Temp, 45 // '-'
   st -X, Temp // store -
   //rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT drop throught
#endif // CONVERT_13BITS_PERCENT_RIGHT
   //------------------------------

END_CONVERT_13BITS_PERCENT_STORE_DIGIT:
ret

.undef LeadingDigit
.undef Negative
//=======================================================================

//-----------------------------------------
//  ADC interrupt, result available
// removed this interrupt 
// just check if conversion start still set.
//-----------------------------------------

//-------------------------------------------
// LED_CONTROL
// update the led control if adc trigger set
//-----------------------------------------
LED_CONTROL:
    sbic ADCSRA, ADSC
    rjmp END_LED_CONTROL // ADSC still set no reading yet
    // else have reading
    // get it
    // clear the interrupt flag
    sbi ADCSRA, ADIF
    in ADCLow,ADCL
    in ADCHigh,ADCH

    sts ADC_HIGH,ADCHigh
    sts ADC_LOW,ADCLow

    // start another one now
    sbi ADCSRA, ADSC

    cpi Torch_State, Torch_State_OFF
    breq CURRENT_HIGH  // if led off set output low
    // else led should be on
    sub ADCLow, SP_low 
    sbc ADCHigh, SP_High
    brge CURRENT_HIGH      // branch if ADC >= setpoint  (signed comparison)
    // else ADC < setpoint

CURRENT_LOW:
    // ADC < setpoint  ==> make output high
    sbi  PORTB, uC_OUTPUT_B   // set output high
    rjmp END_LED_CONTROL

CURRENT_HIGH:
    // ADC > setpoint  ==> make output low
    cbi  PORTB, uC_OUTPUT_B   // set output low
    rjmp END_LED_CONTROL

// all paths jmp to here
END_LED_CONTROL:
ret
//-------------------------------

//--------------------------
// TIMER1_2mS
// called each main loop
// but only call Switch_Debounce if trigger
// has been set by timer interrupt
//-------------------------
TIMER1_2mS:
   sbrs TRIGGER_Flags, TRIGGER_Timer1
   rjmp END_TIMER1_2mS
   // else changed clear trigger now
   cbr Trigger_Flags, (1<<TRIGGER_Timer1)
   rcall SWITCH_DEBOUNCE
   // call any other timer counters from here eg 
   rcall TIMER_2hz
END_TIMER1_2mS:
ret
 
//--------------------------
// TIMER_2hz
// Called from TIMER1_2mS each 2 mSec
// counts up to 250 to time 0.5sec i.e. 2Hz
// sets trigger each 0.5sec
//-------------------------
.equ hz_2_Count = 250
TIMER_2hz:
    lds Temp, HZ_2_COUNTER
    dec Temp
    brne END_TIMER_2hz
    // else == 0
    sbr TRIGGER_Flags, (1<<TRIGGER_2Hz)
    ldi Temp,hz_2_Count  // reload
END_TIMER_2hz:
    sts HZ_2_COUNTER, Temp
ret
//-------------------------------------

// ------------------------
// SWITCH_DEBOUNCE
//  this is called each 1mS
//  checks for debounce and updates 
//  SW_SWDown and SW_SWLastInput and SW_SwitchChanged
//  SW_SwitchChanged set to 1 each time switch changes state
//  SW_SWDown set to 1 when switch is pressed (debouced) else 0 when up
//  SW_SWLastInput is set to 1 when last switch read was low/on else set 0 for high/off
// ------------------------
SWITCH_DEBOUNCE:
    cpi DEBOUNCE_Counter, 0xfe    // inc counter but limit to 253
    brsh DEBOUNCE_Counter_LIMITED  // skip increment if at or above limit
    inc DEBOUNCE_Counter ; count up 1KHz counter for debouce

DEBOUNCE_Counter_LIMITED:
    sbic PINB, SW_B   // read SW if SW high jump to released
    rjmp SW_HIGH_RELEASED
    // else drop through to SW_LOW_PRESSED

SW_LOW_PRESSED: // read SW low/pressed
    sbrc SW_Flags, SW_SWLastInput //check if last flag high/on, rjmp if was high 
    rjmp SW_low_check_debounced   // if last input low and this one low check for debounce low
     
    // else this sw low and last high, set last input to high and reset debounce counter
    ori SW_Flags,(1<<SW_SWLastInput)  // 1 for last pressed
    clr DEBOUNCE_Counter;  load counter restart counter for next debounce
    rjmp SWITCH_DEBOUNCE_RETURN // no change yet

SW_low_check_debounced:
    cpi DEBOUNCE_Counter, Debounce_Count_Low 
    brne SWITCH_DEBOUNCE_RETURN  // only do switch change on DEBOUNCE count
    // else update output and return
    sbr SW_Flags,(1<<SW_SWDown) // debounced
    sbr TRIGGER_Flags, (1<<TRIGGER_SwitchChanged) // debouced switch changed
    rjmp SWITCH_DEBOUNCE_RETURN

SW_HIGH_RELEASED: // read SW high/released
    sbrs SW_Flags, SW_SWLastInput  // check if last flag low/off, rjmp if was low
    rjmp SW_high_check_debounced  // if last input high and this one high check for debounce high
     
    // else this sw high and last low, clear last input and reset debounce counter
    andi SW_Flags,~(1<<SW_SWLastInput)
    clr DEBOUNCE_Counter;  load counter restart counter for next debounce
    rjmp SWITCH_DEBOUNCE_RETURN  // no change yet

SW_high_check_debounced:
    cpi DEBOUNCE_Counter, Debounce_Count_High 
    brne SWITCH_DEBOUNCE_RETURN  // only do switch change on DEBOUNCE count
    // update output and return
    cbr SW_Flags, (1<<SW_SWDown) // debounced
    sbr TRIGGER_Flags, (1<<TRIGGER_SwitchChanged) // debouced switch changed
    rjmp SWITCH_DEBOUNCE_RETURN

SWITCH_DEBOUNCE_RETURN:
ret
//-------------------------------------

//-------------------------------------
//PROCESS_STATE_TRIGGERS
//-------------------------------------
PROCESS_STATE_TRIGGERS:
   mov New_TORCH_State, TORCH_State // set initial state
   cpi TRIGGER_Flags, 0  // are any trigger flags set
   breq END_PROCESS_STATE_TRIGGERS   // all zero so just return
   sbrs TRIGGER_Flags, TRIGGER_SwitchChanged
   rjmp Finished_SwitchChanged_TRIGGER_PROCESSING
   // else changed, clear trigger now
   cbr Trigger_Flags, (1<<TRIGGER_SwitchChanged)
   // check switch state
   sbrc SW_Flags, SW_SWDown // skip rcall if switch is up
   rcall PROCESS_SWDown_TRIGGER  // set level accordingly
   rcall UPDATE_TORCH_STATE  // transfer new state to TORCH_State and update setpoint etc.
Finished_SwitchChanged_TRIGGER_PROCESSING:
   // check other triggers here
END_PROCESS_STATE_TRIGGERS:
ret
//-------------------------------------

//-------------------------------------
//PROCESS_SWDown_TRIGGER
//  Press to cycle through levels
//-------------------------------------
PROCESS_SWDown_TRIGGER:
    // else switch pressed down, cycle through states.
   cpi TORCH_State, Torch_State_OFF
   breq PROCESS_SWDown_TRIGGER_TORCH_OFF
   // else
   cpi TORCH_State, TORCH_State_LOW
   breq PROCESS_SWDown_TRIGGER_TORCH_LOW
   // else
   cpi TORCH_State, TORCH_State_MED
   breq PROCESS_SWDown_TRIGGER_TORCH_MED
   // else
   cpi TORCH_State, TORCH_State_HIGH
   breq PROCESS_SWDown_TRIGGER_TORCH_HIGH
   // else invalid state drop through to set to off

PROCESS_SWDown_TRIGGER_TORCH_STATE_INVALID: // invalid -> off
PROCESS_SWDown_TRIGGER_TORCH_HIGH: // high -> off,  variable -> off
   ldi New_TORCH_State, Torch_State_OFF
   rjmp END_PROCESS_SWDown_TRIGGER

PROCESS_SWDown_TRIGGER_TORCH_OFF: // off -> low
   ldi New_TORCH_State, Torch_State_LOW
   rjmp END_PROCESS_SWDown_TRIGGER
    
PROCESS_SWDown_TRIGGER_TORCH_LOW: // low -> med
   ldi New_TORCH_State, Torch_State_MED
   rjmp END_PROCESS_SWDown_TRIGGER

PROCESS_SWDown_TRIGGER_TORCH_MED:  // med -> high
   ldi New_TORCH_State, Torch_State_HIGH
   // rjmp END_PROCESS_SWDown_TRIGGER drop through

END_PROCESS_SWDown_TRIGGER:
ret
//-------------------------------------

//-------------------------------------
//UPDATE_TORCH_STATE
// Set the new torch state and load current setpoint
//-------------------------------------
UPDATE_TORCH_STATE:
   mov TORCH_State, New_TORCH_State // update state to new state
   rcall LOAD_CURRENT_SETPOINT // load new setpoint
ret
//-------------------------------------

//-------------------------------------
//LOAD_CURRENT_SETPOINT
//  Loads the current setpoint base on the torch state
//  Also set SW_LedOn depending on TORCH_State
//-------------------------------------
LOAD_CURRENT_SETPOINT:
   push Temp
   cpi TORCH_State, Torch_State_OFF
   breq SET_TORCH_OFF
   // else
   cpi TORCH_State, TORCH_State_LOW
   breq SET_TORCH_LOW
   // else
   cpi TORCH_State, TORCH_State_MED
   breq SET_TORCH_MED
   // else
   cpi TORCH_State, TORCH_State_HIGH
   breq SET_TORCH_HIGH
INVALID_STATE:  // else invalid turn off
   ldi TORCH_State, TORCH_State_OFF // set valid state
   // drop through to set off
SET_TORCH_OFF:
   ldi Temp, 0;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT
    
SET_TORCH_LOW:
   ldi Temp, LEVEL_LOW;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT

SET_TORCH_MED:
   ldi Temp, LEVEL_MED;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT

SET_TORCH_HIGH:
   ldi Temp, LEVEL_HIGH;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT

END_LOAD_CURRENT_SETPOINT:
   rcall LOAD_SETPOINT_FROM_LEVEL
   pop Temp
ret
//-------------------------------------

//-------------------------------------
//PROCESS_RECEIVED_CHAR
//  0,1,2,3 to set level
// char in Temp
//-------------------------------------
PROCESS_RECEIVED_CHAR:

   cpi Temp, 48 // '0' 0x30
   breq PROCESS_RECEIVED_CHAR_TORCH_OFF
   // else
   cpi Temp, 49 // '1' 0x31
   breq PROCESS_RECEIVED_CHAR_TORCH_LOW
   // else
   cpi Temp, 50 // '2' 0x32
   breq PROCESS_RECEIVED_CHAR_TORCH_MED
   // else
   cpi Temp, 51 // '3' 0x33
   breq PROCESS_RECEIVED_CHAR_TORCH_HIGH
   // else
   cpi Temp, 117 // 'u' 0x75
   breq PROCESS_RECEIVED_CHAR_TORCH_UP

   cpi Temp, 85 // 'U'  0x55
   breq PROCESS_RECEIVED_CHAR_TORCH_BIGUP

   cpi Temp, 100 // 'd' 0x64
   breq PROCESS_RECEIVED_CHAR_TORCH_DOWN

   cpi Temp, 68 // 'D'  0x44
   breq PROCESS_RECEIVED_CHAR_TORCH_BIGDOWN

   cpi Temp, 105 // 'i' 0x69
   breq PROCESS_RECEIVED_CHAR_SEND_ADC_READING

   cpi Temp, 73 // 'I'  0x49
   breq PROCESS_RECEIVED_CHAR_SEND_ADC_READING

   cpi Temp, 13 // cr  0x0d
   breq PROCESS_RECEIVED_CHAR_IGNORE // ignore 

   cpi Temp, 10 // lf  0xoa
   breq PROCESS_RECEIVED_CHAR_IGNORE // ignore

   // else invalid char so just ignore
   // send back error msg + amps
   sbr TRIGGER_Flags, (1<<TRIGGER_Send_Error_Msg) | (1<<TRIGGER_Level)  | (1<<TRIGGER_Light)  
   rjmp RET_PROCESS_RECEIVED_CHAR

PROCESS_RECEIVED_CHAR_IGNORE:
   rjmp RET_PROCESS_RECEIVED_CHAR

PROCESS_RECEIVED_CHAR_SEND_ADC_READING:
   sbr TRIGGER_Flags, (1<<TRIGGER_Amps)  
   rjmp RET_PROCESS_RECEIVED_CHAR

PROCESS_RECEIVED_CHAR_TORCH_OFF: // 0 == off
   ldi New_TORCH_State, Torch_State_OFF
   rjmp PROCESS_RECEIVED_CHAR_UPDATE
    
PROCESS_RECEIVED_CHAR_TORCH_LOW: // 1 == low
   ldi New_TORCH_State, Torch_State_LOW
   rjmp PROCESS_RECEIVED_CHAR_UPDATE

PROCESS_RECEIVED_CHAR_TORCH_MED:  // 2 == med
   ldi New_TORCH_State, Torch_State_MED
   rjmp PROCESS_RECEIVED_CHAR_UPDATE

PROCESS_RECEIVED_CHAR_TORCH_HIGH: // 3 == high
   ldi New_TORCH_State,Torch_State_HIGH 
   // rjmp PROCESS_RECEIVED_CHAR_UPDATE

PROCESS_RECEIVED_CHAR_UPDATE:
   rcall UPDATE_TORCH_STATE
   rjmp PROCESS_RECEIVED_CHAR_SEND_SP

PROCESS_RECEIVED_CHAR_TORCH_UP:
PROCESS_RECEIVED_CHAR_TORCH_BIGUP:
   ldi TORCH_State, Torch_State_VARIABLE
   inc Level
   rcall LOAD_SETPOINT_FROM_LEVEL
   rjmp PROCESS_RECEIVED_CHAR_SEND_SP

PROCESS_RECEIVED_CHAR_TORCH_DOWN:
PROCESS_RECEIVED_CHAR_TORCH_BIGDOWN:
   ldi TORCH_State, Torch_State_VARIABLE
   dec Level
   rcall LOAD_SETPOINT_FROM_LEVEL
   rjmp PROCESS_RECEIVED_CHAR_SEND_SP

PROCESS_RECEIVED_CHAR_SEND_SP:
   sbr TRIGGER_Flags, (1<<TRIGGER_Level)|(1<<TRIGGER_Light)

RET_PROCESS_RECEIVED_CHAR:
 ret
//-------------------------------------

//-------------------------------------
// LOAD_SETPOINT_FROM_LEVEL
// Level is set before entry
// SP_High and SP_Low set by this call
//-------------------------------------
LOAD_SETPOINT_FROM_LEVEL:
  // limit level to Level_Max
  push Temp
  tst Level
  brpl LOAD_SETPOINT_FROM_LEVEL_TEST_MAX
  clr Level // set to zero if negative
LOAD_SETPOINT_FROM_LEVEL_TEST_MAX:
  ldi Temp, Level_Max
  cp Level, Temp
  brmi LOAD_SETPOINT_FROM_LEVEL_LOAD // Level < Level_Max
  mov Level, Temp //set to Level_Max if too large

LOAD_SETPOINT_FROM_LEVEL_LOAD:
  push ZH
  push ZL
  ldi ZH, high(2*Log_SetPoint)
  ldi ZL, low(2*Log_SetPoint)
  // add level offset
  mov Temp, Level
  lsl Temp // 2 times
  add ZL, Temp // add Level to addres to get word to load
  sbci ZH, 0  // sub 0 with carry
 
  lpm Temp, Z+
  mov SP_Low, Temp
  lpm Temp, Z+
  mov SP_High, Temp
  pop Temp
  pop ZL
  pop ZH
END_LOAD_SETPOINT_FROM_LEVEL:
ret

//*************************************************************************
//
// RS232 controlled Led Driver Program
// As well as allowing the led level to be controlled by the push button
// this program changes the level based on the character sent via RS232.
// 0 => off
// 1 => low, 2 => med, 3 => high
// u or U increment setpoint by 10
// d or D decrement setpoint by 10
// limit SP at 0 and 1023
// 
// Each 0.5sec send back via RS232 the ADC reading 
//
//*************************************************************************
RESET:
    cli  // disable interrupts
    // set clock to 8Mhz i.e. remove div8 that is set by programmer
    ldi Temp, (1<<CLKPCE)
    out CLKPR, Temp  // enable clock change for next 4 cycles
    clr Temp
    out CLKPR,Temp  // set 8Mhz clock

    // ******** Stack Pointer Setup Code ********
    ldi Temp,high(RAMEND-(NO_GLOBAL_VARS+1)) ; Stack Pointer Setup
    out SPH,Temp ; Stack Pointer High Byte
    ldi Temp,low(RAMEND-(NO_GLOBAL_VARS+1)) ; Stack Pointer Setup
    out SPL,Temp ; Stack Pointer Low Byte

    // adjust the clock frequency   
    // for this chip
    //  default cal is 9E for 7.77Mhz, 0xA5 gives 8.04Mhz at about 26deg
    ldi Temp, OSC_CAL_STEP_1 //0xA5
    out OSCCAL, Temp
    ldi Temp, OSC_CAL_STEP_2 //0xA5 also in this case
    out OSCCAL, Temp

    // default port settings are all zeros i.e. input tri-state
    // start by clearing all pin interrupt masks
    clr Temp
    out PCMSK0,Temp
    out SERIAL_IN_INTERRUPT_MASK,Temp

    // uC_OUTPUT_B (PB0) is an output for driving the led  DDB0=1, PB0=0 low OFF to start
    // SW_B (PB1) is the switch input  DDB1=0, PB1=1 with pullup
    ldi Temp,(1<<SW_B)
    ldi r17, (1<<uC_OUTPUT_B)
    out PORTB,Temp
    out DDRB,r17

    // Enable ADC
    ldi Temp, (1<<ADEN)  | (1<<ADPS2) | (1<<ADPS1) // ADPS1 was ADPS0 for 4Mhz 
    // (1<<ADEN) enable ADC   // remove | (1<<ADIE) so interrupt not called
    //  ADSC zero do not start conversion yet
    //  ADATE zero do not trigger conversion
    //  (1<<ADIE) enable ADC interrupt need I-bit in SREG set also
     // Now 8Mhz uC clock so use (1<<ADPS2) (1<<ADPS1) == 64
    //  now 8Mhz/64 = 125Khz 
    out ADCSRA, Temp       // set the  ADCSR

    // set Vref 1.1V (1,0) and the ADC mux inputs and gain (101101) PA2+ve, PA1-ve, x20
    ldi Temp, (1<<REFS1) | (1<<MUX5) | (1<<MUX3) | (1<<MUX2) | (1<<MUX0)
    out ADMUX, Temp

    // set up timer1
    ldi Temp, (0<WGM10) | (0<<WGM11)
    out TCCR1A, Temp
    ldi Temp , (0<<WGM13) | (1<<WGM12) 
    // CTC MODE using OCR1A  and clock of 1/64
    out TCCR1B, Temp
    // load couter with 250 -1 = 0x00, 0xF9 for 0.002 sec timer from 8Mhz
    // load high first then low
    ldi Temp,  0x00
    out OCR1AH, Temp
    ldi Temp, 0xF9 
    out OCR1AL, Temp
    // clear the counter 
    clr Temp
    out TCNT1H, Temp // must write high byte first
    out TCNT1L, Temp

    // set the interrupt
    ldi Temp, (1<<OCIE1A)
    out TIMSK1, Temp  // compare A match interrupt
    // clear the interrupt flag
    ldi Temp, (1<<OCF1A)
    out TIFR1, Temp 

    // start timer1
    in Temp, TCCR1B
    ori Temp, (1<<CS11) | (1<<CS10)  // 1/64
    out TCCR1B, Temp  // timer started

    clr Temp
    mov SW_Flags, Temp
    mov TRIGGER_Flags, Temp
    mov Torch_State, Temp
    mov Level, Temp 

// set initial counter values
    rcall RESET_IamAlive_COUNTER
    ldi Temp,hz_2_Count  
    sts HZ_2_COUNTER, Temp 

    rcall RS232_INIT    // Initialize RS232 interrupts still disabled
    sei  // Enable interrupts

    sbi ADCSRA, ADSC  // start first conversion
    rcall RS232_SET_TO_RECEIVE // sets to recieve and enables interrupts

MAIN_LOOP:
    rcall LED_CONTROL_PROCESSING // each time ADC completes and save value
    rcall PROCESS_TIMERS // do this befor other processing that may depend on timer triggers
    rcall PROCESS_RS232  // process char if there is one
    rcall SEND_LEVEL // send level
    rcall SEND_LIGHT // then % light
    rcall PROCESS_RS232_ERROR_MSG // send error msg if needed
    rcall SEND_ADC_READING // send Amps if needed
    rjmp MAIN_LOOP   

//---------------------------------------------------
// LED_CONTROL_PROCESSING:
//
// The LED_CONTROL checks if ADC conversion has finished and if so
// updates the led control output
// The Timer check if the 2mS Timer1 counter has triggers and if so
// runs the pushbutton debouce code and calls the 0.5sec counter
// The Process State Triggers handles any push button triggers generated
// by the debouce code.
//---------------------------------------------------
LED_CONTROL_PROCESSING:
  rcall LED_CONTROL
  rcall TIMER1_2mS  // also updates 0.5sec counter and debounces pushbutton
  rcall PROCESS_STATE_TRIGGERS // handle pushbutton presses
ret

//----------------------------------------------
// PROCESS_RS232
// 
// This check is RS232 has finished sending and can be set back to receive.
// Also checks is a new byte has been received. If so it calles
// PROCESS_RECEIVE_CHAR to change the LED setpoint based on the char received
//----------------------------------------------
PROCESS_RS232:
    lds Temp, RS232_STATUS // get current status

    sbrc Temp, RS232_STATUS_TX_COMPLETE // check if just completed TX
    rjmp PROCESS_RS232_SET_RECEIVE  // if just completed transmit then set to receive

    // else has a new data byte been received
    sbrs Temp,RS232_STATUS_DATA_RECEIVED
    rjmp END_PROCESS_RS232  // no new char so have just return

    // else have received new char
    // get the char to send back
    lds Temp2, RS232_RECEIVE_BYTE

    // now clear RS232_STATUS_DATA_RECEIVED
    // disable interrupts the to prevent loosing some other update of RS232_STATUS
    cli
    lds Temp, RS232_STATUS
    cbr Temp, (1<<RS232_STATUS_DATA_RECEIVED) 
    sts RS232_STATUS, Temp
    sei

    mov Temp, Temp2
    rcall PROCESS_RECEIVED_CHAR 

PROCESS_RS232_SET_RECEIVE:
    rcall RS232_SET_TO_RECEIVE  
    // rjmp END_PROCESS_RS232  drop through

END_PROCESS_RS232:
ret

//-----------------------------------
// PROCESS_TIMERS  
// triggered every 0.5 sec to count longer timers
//
//-----------------------------------
PROCESS_TIMERS:
    sbrs TRIGGER_Flags, TRIGGER_2Hz
    rjmp END_PROCESS_TIMERS
    cbr Trigger_Flags, (1<<TRIGGER_2Hz) // have processed this trigger now
    // if needs be set another flag here for triggering other processes
    rcall UPDATE_IamAlive_TIMER
END_PROCESS_TIMERS:
ret

.equ IamAlive_Count = 16 // 8 secs
UPDATE_IamAlive_TIMER:
    lds Temp, IamAlive_COUNTER
    dec Temp
    brne END_UPDATE_IamAlive_TIMER
    // else == 0
    sbr TRIGGER_Flags, (1<<TRIGGER_IamAlive)
    ldi Temp,IamAlive_Count  // reload
END_UPDATE_IamAlive_TIMER:
    sts IamAlive_COUNTER, Temp
ret

//---------------------------
// RESET_IamAlive_COUNTER
// This is called each time we transmit
// to reset the counter
//---------------------------
RESET_IamAlive_COUNTER:
    ldi Temp, IamAlive_Count
    sts IamAlive_COUNTER, Temp 
ret

//-----------------------------------------------------
// SEND_ADC_READING:
// 
// Send back the ADC reading as a % of full scale (1000) counts
// if the TRIGGER_Amps flag is set
// This flag is set by the 'i' command key
//-----------------------------------------------------
SEND_ADC_READING:
    sbrs TRIGGER_Flags, TRIGGER_Amps
    rjmp END_SEND_ADC_READING
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_ADC_READING  // busy sending so keep trigger and try later
    // else not busy sending so send this ADC reading 

    // clear this trigger now as now sending ADC reading
    cbr TRIGGER_Flags, (1<<TRIGGER_Amps)
    // do not send for now
    rcall CONVERT_AND_SEND_ADC_READING

END_SEND_ADC_READING:
ret

//-------------------------------------------------------
// CONVERT_AND_SEND_ADC_READING
// send back suppressing leading zeros
// ADC nnnn    
//-------------------------------------------------------
CONVERT_AND_SEND_ADC_READING:
    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    NEW_LINE
    LOAD_CHARS_FROM Amps_Msg
    
    push ZH  // save temp registers
    push ZL
    // Note ADC_High and ADC_Low only undate by LED_CONTROL in main loop
    // so no chance of an interrupt modifying the value between then next
    // to statements
    lds ZH, ADC_High // load the 10bit number to send
    lds ZL, ADC_LOW
    rcall CONVERT_13BITS_PERCENT // convert  it
    pop ZL
    pop ZH  // save temp registers

    NEW_LINE

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall TRANSMIT_DATA
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_LIGHT
// send back Light level
//     
//-------------------------------------------------------
SEND_LIGHT:
    sbrs TRIGGER_Flags, TRIGGER_Light
    rjmp END_SEND_LIGHT
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_LIGHT  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending light % reading
    cbr TRIGGER_Flags, (1<<TRIGGER_Light)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    NEW_LINE
    LOAD_CHARS_FROM Light_Msg

    push ZH  // save temp registers
    push ZL
    // Note ADC_High and ADC_Low only undate by LED_CONTROL in main loop
    // so no chance of an interrupt modifying the value between then next
    // to statements
    mov ZH, SP_High // load the 10bit number to send
    mov ZL, SP_LOW
    rcall CONVERT_13BITS_PERCENT // convert  it
    pop ZL
    pop ZH  // save temp registers

    NEW_LINE

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall TRANSMIT_DATA
END_SEND_LIGHT:
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_LEVEL
// send level number
//     
//-------------------------------------------------------
SEND_LEVEL:
    sbrs TRIGGER_Flags, TRIGGER_Level
    rjmp END_SEND_LEVEL
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_LEVEL  // busy sending so keep trigger and try later
    // else not busy sending so send this level 

    // clear this trigger now as now sending level
    cbr TRIGGER_Flags, (1<<TRIGGER_Level)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    NEW_LINE
    LOAD_CHARS_FROM Level_Msg

    // Level limited to 0 to 10 
    push ZH  // save temp registers
    push ZL
    ldi ZH, 0 // load the 10bit number to send
    mov ZL, Level
    rcall CONVERT_13BITS // convert  it
    pop ZL
    pop ZH  // save temp registers

    //NEW_LINE
    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall TRANSMIT_DATA
END_SEND_LEVEL:
ret
//---------------------------------------------------------

TRANSMIT_DATA:
    rcall RESET_IamAlive_COUNTER
    rcall RS232_TRANSMIT // start sending
END_TRANSMIT_DATA:
ret

//---------------------------------
// PROCESS_RS232_ERROR_MSG
// send error msg is trigger set
//--------------------------------
PROCESS_RS232_ERROR_MSG:
    // call if trigger
    sbrs TRIGGER_Flags, TRIGGER_Send_Error_Msg
    rjmp END_PROCESS_RS232_ERROR_MSG

    //check if busy
    BRANCH_IF_RS232_BUSY END_PROCESS_RS232_ERROR_MSG  
    // busy sending so keep trigger and try later

    cbr TRIGGER_Flags, (1<<TRIGGER_Send_Error_Msg)

    push Temp
    push XH
    push XL

    ldi XH, high(RS232_BUFFER_1+1) // add 1 due to -X in  ST -X,Temp below
    ldi XL, low(RS232_BUFFER_1+1) // add 1 due to -X in  ST -X,Temp below

    NEW_LINE
    LOAD_CHARS_FROM Error_Msg_Line1
    NEW_LINE
    LOAD_CHARS_FROM Error_Msg_Line2
    NEW_LINE

    //set number of chars
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers
    pop XL
    pop XH
    pop Temp
    rcall TRANSMIT_DATA // start sending

END_PROCESS_RS232_ERROR_MSG:
ret

//-----------------------------------------------------
// CONVERT_13BITS
// Converts the, possiably signed, 13bit number in ZH:ZL 
// (actually -9999 to +9999)
// and save acsii char in global SRAM whose start is pointed
// to by XH:XL,
// XH:XL decremented BEFORE each store of a char
// If ZH:ZL is negative a leading '-' is stored
// 
// Format of number is determined by uncommenting
// one of the 3 #defines
// #define CONVERT_13BITS_LEFT
//  left justifies the result  eg |-21 and |2
// #define CONVERT_13BITS_RIGHT
//  right justifies the result eg |  -21 and |    2
// #define CONVERT_13BITS_ZEROS
//  right justifies with leading zeros eg |-0021 and | 0002
// if you uncomment more then one you will get a compile error (duplicate label)
//
// Numbers >9999 are output as 9999
// Numbers <-9999 are output as -9999
// 
// On return ZH:ZL is zero and XH:XL is updated to point to
// the last char stored
//
// all used registers pushed and popped (saved and restored) 
//-----------------------------------------------------
#define CONVERT_13BITS_LEFT
//#define CONVERT_13BITS_RIGHT
//#define CONVERT_13BITS_ZEROS

// register defines for CONVERT_13BITS
.def LeadingDigit = r17 
.def Negative = r18
 
CONVERT_13BITS:
    push Temp
    push LeadingDigit 
    push Negative

    clr Negative // no neg
    clr LeadingDigit // no leading digit yet

    // check if number to be converted is neg
    // if so prepend - and negate
    clr Temp
    cp ZL, Temp
    cpc ZH, Temp
    brpl CONVERT_13BITS_PAD_NEG_SIGN

    // else negate the input
    // Temp zero from above
    //clr Temp
    sub Temp, ZL
    mov ZL, Temp
    clr Temp
    sbc Temp, ZH
    mov ZH, Temp
    ldi Temp, 45 // '-'
    st -X, Temp // store -
    ldi Negative, 1 // set negative flag
    rjmp CONVERT_13BITS_CHECK_MAX_NUMBER

CONVERT_13BITS_PAD_NEG_SIGN:
   #ifndef CONVERT_13BITS_LEFT
    // store a space if you want to padd the '-' position of +ve number
    ldi Temp, 32
    st -X, Temp // store space
   #endif
CONVERT_13BITS_CHECK_MAX_NUMBER:
    // check against max value
    push XH
    push XL
    mov XH, ZH
    mov XL, ZL
    subi XL,  low(9999)
    sbci XH, high(9999)
    pop XL
    pop XH
    brsh CONVERT_13BITS_STORE_9999
    rjmp CONVERT_13BITS_START_CONVERSION
CONVERT_13BITS_STORE_9999:
    ldi Temp, 48 + 9 // '0' + 9
    st -X, Temp
    st -X, Temp
    st -X, Temp
    st -X, Temp
    rjmp END_CONVERT_13BITS

CONVERT_13BITS_START_CONVERSION:
    clr Temp
CONVERT_13BITS_LOOP_1000:
    subi ZL, low(1000)
    sbci  ZH, high(1000)
    brmi CONVERT_13BITS_ADD_BACK_1000
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_1000 // try again
CONVERT_13BITS_ADD_BACK_1000:
    subi ZL, low(-1000)
    sbci  ZH, high(-1000)
CONVERT_13BITS_STORE_1000:
    rcall CONVERT_13BITS_STORE_DIGIT

    clr Temp
CONVERT_13BITS_LOOP_100:       
    subi ZL, low(100)
    sbci  ZH, high(100)
    brmi CONVERT_13BITS_ADD_BACK_100
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_100  // try again
CONVERT_13BITS_ADD_BACK_100:
    subi ZL, low(-100)
    sbci  ZH, high(-100)
CONVERT_13BITS_STORE_100:
    rcall CONVERT_13BITS_STORE_DIGIT

    clr Temp
CONVERT_13BITS_LOOP_10:
    sbiw Z, 10 // use word subtraction for < 64       
    brmi CONVERT_13BITS_ADD_BACK_10
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_10  // try again
CONVERT_13BITS_ADD_BACK_10:
    adiw Z, 10 // use word addition for < 64
CONVERT_13BITS_STORE_10:
    rcall CONVERT_13BITS_STORE_DIGIT

    clr Temp
CONVERT_13BITS_LOOP_1:       
    sbiw Z, 1 // use word subtraction for < 64       
    brmi CONVERT_13BITS_ADD_BACK_1
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_1  // try again
CONVERT_13BITS_ADD_BACK_1:
    adiw Z, 1 // use word addition for < 64       
CONVERT_13BITS_STORE_1:
     // always store last digit even if 0
    subi Temp, -48 //  temp - (-48) 
    st -X, Temp // store 0 or 1's

END_CONVERT_13BITS:
    pop Negative
    pop LeadingDigit
    pop Temp
ret

//--------------------------
// CONVERT_13BITS_STORE_DIGIT
// Checks Temp against 0 and if not 0
// add 48 '0' and store in location 
// pointed to by XH:XL  (decremented first)
// see code comments for how to store
// leading zeros or padding blanks
//--------------------------
CONVERT_13BITS_STORE_DIGIT:
   //------------------------------
#ifdef CONVERT_13BITS_ZEROS
   // this section stores leading zeros
CONVERT_13BITS_STORE_NEXT_DIGIT:
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_STORE_DIGIT
#endif //CONVERT_13BITS_ZEROS
   //------------------------------

   //------------------------------
#ifdef CONVERT_13BITS_LEFT
   // this section has no padding
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq END_CONVERT_13BITS_STORE_DIGIT // skip zero

CONVERT_13BITS_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_STORE_DIGIT
#endif //CONVERT_13BITS_LEFT
   //------------------------------

   //------------------------------
#ifdef CONVERT_13BITS_RIGHT
   // this section padds with leading spaces
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq CONVERT_13BITS_STORE_ZERO_DIGIT // handle zero

CONVERT_13BITS_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_STORE_DIGIT

CONVERT_13BITS_STORE_ZERO_DIGIT:
   // if negative move the - to the right
   tst Negative
   brne CONVERT_13BITS_STORE_PADDING // negative store over neg
   // else no negative dec X before store
   sbiw X, 1 
CONVERT_13BITS_STORE_PADDING:
   ldi Temp, 32 // ' '
   st X, Temp
   // restore neg if there is one
   tst Negative
   breq END_CONVERT_13BITS_STORE_DIGIT // not negative nothing to do
   // store neg in next location
   ldi Temp, 45 // '-'
   st -X, Temp // store -
   //rjmp END_CONVERT_13BITS_STORE_DIGIT drop throught
#endif // CONVERT_13BITS_RIGHT
   //------------------------------

END_CONVERT_13BITS_STORE_DIGIT:
ret

.undef LeadingDigit
.undef Negative
//=======================================================================

/**********
//-----------------------------------------------------
// LOAD_CHARS
// loads chars 
//-----------------------------------------------------
.def CharCount = r15 // temp reg
.def Temp     = r16// Temporary register
LOAD_CHARS:
    push CharCount
    push Temp
    push ZH
    push ZL

    ldi ZH, high(2*Error_Msg_Line1)
    ldi ZL, low(2*Error_Msg_Line1)     // Set pointer to RAM data
    lpm Temp2, Z+    // this is the number of chars to load
Loop:
    lpm Temp, Z+
    st -X, Temp
    dec Temp2
    brne LOOP

END_LOAD_CHARS:
    pop ZL
    pop ZH   
    pop Temp
    pop CharCount
ret
//------------------------------------------------------
******************/

Log_SetPoint:
     // 0 == 0, 1==4    2==7   3 == 10
     // Levels
     // 0  1  2  3  4  5   6    7   8     9    10 
.dw     0, 2, 6, 12,26,57,129, 273, 509, 794, 1000 // words 

Level_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
     //  0   1    2   3    4   5     6   7   8   9    //10   11  12    13 
     //           L   e    v   e     l  
.db      7,  32,  76, 101, 118, 101, 108, 32

Light_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
     //  0   1    2   3    4   5     6   7   8   9    //10   11  12    13 
     //           L    i   g    h    t      //  L   e    v   e     l  
.db      7,  32, 76, 105, 103, 104, 116, 32//, 76, 101, 118, 101, 108, 32


Amps_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
     //  0   1    2  3    4   5    
     //      A    m    p    s      
.db      5, 65, 109, 112, 115, 32 


Error_Msg_Line1:
     // .db line must have an even number of chars
     // first value is the number of chars to load
     //  0  1    2    3    4    5  6    7     8    9   10  11
     //          >    u    s    e       k     e    y   s     
.db     11, 32, 62, 117, 115, 101, 32, 107, 101, 121, 115, 32 
Error_Msg_Line2:
    //  0   1   2    3   4    5   6   7   8   9  10  11   12  13    
    //  0      u        d         0       1      2        3        
.db    13, 32, 117, 32, 100, 32, 48, 32, 49, 32, 50, 32,  51, 32
