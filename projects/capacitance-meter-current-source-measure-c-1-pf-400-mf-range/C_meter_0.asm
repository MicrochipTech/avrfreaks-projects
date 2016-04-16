//--------------------------------
//--- C meter firmware for AVR ---
//---      2016-03-17          ---
//--------------------------------

//---------------------
// Lcd = PORTB.4,5,6,7
// RS=PORTB.2
// EN=PORTB.3
// LCD_TYPE = 16 * 2
//---------------------

//-------------------------------------
//--- for AT90S2313 and ATtiny2313 ----
//-------------------------------------

///-------------------------
//--- our private deines ---
//--------------------------
.equ     RAMSTART   = $60
.equ     RAMEND     = $DF
//-
.equ     ACSR       = $08

.equ     DDRD       = $11
.equ     PORTD      = $12

.equ     DDRB       = $17
.equ     PORTB      = $18

.equ     ICR1H      = $25
.equ     ICR1L      = $24

.equ     TCNT1L     = $2c
.equ     TCNT1H     = $2d
.equ     TCCR1B     = $2e
.equ     TCCR1A     = $2f

.equ     TIFR       = $38
.equ     TIMSK      = $39

.equ     SPL        = $3d

//-
.def	XL	=	r26
.def	XH	=	r27
.def	YL	=	r28
.def	YH	=	r29
.def	ZL	=	r30
.def	ZH	=	r31
//---------------------------------------------
//--- controll for change microcontroller's ---
//---------------------------------------------


//--------
.equ     CLEAR_FLAG_VALUE    = 0b11000000  // a value of clear interrupt flag from TOV1 and COMPARE_TC1
.equ     ACSR_START_VALUE    = 0b00000111  // start value for ACSR
.equ     DDRB_START_VALUE    = 0b11111100  // initial inout/output value of PORTB
.equ     TIMSK_START_VALUE   = 0b00001000  // IRQ CAPTURE TC1 enable value 
.equ     TIMER1_START_VALUE  = 0b01000001  // value for run TC1
//---------------------------------------------------------
//--- charge/discharge  = PD4 (0 = charge ; 1 = DIS_charge)
//--- channel 1:        = PD5 (1 = enable ; 0 = disable)
//--- channel 2:        = PD3 (1 = enable ; 0 = disable)
//--- channel 3         = PD2 (1 = enable ; 0 = disable)
//---------------------------------------------------------
.equ     start_value_DDRD  = 0b00111100  // our bits to manipulate, charge, discharge, range - as OUTPUT
.equ     start_value_PORTD = 0b00010000  // a start value - everything in hi except a discharge bit

.equ     pre_value_1       = 0b00110000  // discharge + charge chanel 1
.equ     load_value_1      = 0b00100000  // only charge   by  channel 1

.equ     pre_value_2       = 0b00011000  // discharge + charge chanel 2
.equ     load_value_2      = 0b00001000  // only charge   by  channel 2

.equ     pre_value_3       = 0b00010100  // discharge + charge chanel 3
.equ     load_value_3      = 0b00000100  // only charge   by  channel 3
//--------
.equ     OUT_ASCII_BUFF    = RAMSTART+1            // 10 bytes for ASCII decimal string 32bit unsigned value
.equ     LEN_ASCII_BUFF    = OUT_ASCII_BUFF + 11   // length of ASCII digits
.equ     MY_LONG_INT       = OUT_ASCII_BUFF + 12   // 4 bytes LONG INTEGER - 32bit unsigned value 
.equ     offs1             = OUT_ASCII_BUFF + 16   // calibrate value of channel 1
.equ     offs2             = OUT_ASCII_BUFF + 17   // calibrate value of channel 2
.equ     offs3             = OUT_ASCII_BUFF + 18   // calibrate value of channel 3
.equ     calibrate         = OUT_ASCII_BUFF + 19   // calibrate indicator (0 = do calibrate ; <> 0  = no calibrate)
.equ     range             = OUT_ASCII_BUFF + 20   // a range of meassure 0,1,2 = channel 1,2,3
.equ     pre_val           = OUT_ASCII_BUFF + 21   // for actual used channel PRE_charged value
.equ     load_val          = OUT_ASCII_BUFF + 22   // for actual used channel ENABLE (charge by channel) value
.equ     offset            = OUT_ASCII_BUFF + 23   // calibrate value for actual used channel

//--------
.def     var1          =     r1   // divide | multiply - our long integer is here
.def     var2          =     r2
.def     var3          =     r3
.def     var4          =     r4

.def     mod10         =     r16  // helps variable for divide | multiply
.def     cntr_1        =     r17  // helps variable for divide | multiply
.def     divisor       =     r18  // disor for divide proc
.def     multiplier    =     r18  // multiplier for multiply routine
//--------
          
          .CSEG
.ORG     0x0000

//------------------------------
//-- vectors of interrupts -----
//------------------------------

               rjmp     _RESET_PROC
               reti     // _IRQ_INT0  - not used
               reti     // _IRQ_INT1  - not used
               cli      // _IRQ_CAPTURE_TC1 - ok block all IRQ
               rjmp     _IRQ_CAPTURE_TC1  // go to IRQ service

               // rest of INTERRUPTS - NOT used

//==============================================================
_RESET_PROC:
               cli      // not necessary but... maybe other models of AVR's...
               ldi      yl,low(RAMEND)
               out      SPL,yl  // stack init
               ldi      yh,high(RAMEND)
               sbiw     yl,32
//====================================================
               rcall    stop_meassure  // set up pinout to ground - discharge...

               rcall    LcdIni

               ldi      zl, low(welcome * 2)
               ldi      zh, high(welcome * 2) 
               rcall    _LSc  // print welcome info from 1st line

               ldi      r24,1 // print calibrate info from 2nd line
               rcall    _from_line2
               ldi      zl, low(info_txt * 2)
               ldi      zh, high(info_txt * 2)
               rcall    _LSc

               rcall    _Wsec  // wait 1sec. for stabilize pins
set_calibrate:
               ldi      zl, low(RAMSTART+1)
               ldi      zh, high(RAMSTART+1)

//---
               ldi      r16, 0x20   // clear data ram 32 bytes of variables, string etc...
               clr      xh
setcall2:
               st       Z+, XH
               dec      r16
               brne     setcall2
//---
               ldi      r16, 120    // pre calibrate value channel 1
               sts      offs1, r16
               ldi      r16, 0x13   // pre calibrate value channel 2
               sts      offs2, r16
               ldi      r16, 0x11   // pre calibrate value channel 3
               sts      offs3, r16

//=====================================
//==========   repeat main loop =======
//=====================================
starter:
               ldi      yl,low(RAMEND)
               out      SPL,yl          // stack reinit
               ldi      yh,high(RAMEND)
               sbiw     yl,32

               rcall    start_meassure  // OK - let's go rocket :)
//====================================
selfee:          
               in       r20, TIFR
               andi     r20, 0x80
               breq     selfee
               cli                              // block interrupts and counting overflow
               ldi      r20, CLEAR_FLAG_VALUE   // clear interrupts flag from TOV and COMPARE_TC1
               out      TIFR, r20
               sts      calibrate, r20  // if OVERFLOW was raeched - calibrate is impossible
                                        // calibrate <> 0 = DON'T calibrate
               rcall    read_hi_data    // r22 = range, after this rcall

//-------------------------------------------------------------
//--- IF TC1 is overflowed extend 16bit to 32 counter value ---
//-------------------------------------------------------------
               inc      r20
               sts      MY_LONG_INT+2,r20
               brne     no_increase
               inc      r21
               sts      MY_LONG_INT+3,r21
no_increase:
               cpi      r22, 0
               brne     second_range
               cpi      r20, 0x30   // first range after reached 3145728 tick's is change onto next range
               brne     out_call    
               rcall    increment_range
               rjmp     chg_range

second_range:
               cpi      r22, 1
               brne     out_call
               cpi      r20, 0x40  // 4194304 - about half seconds
               brne     out_call
               rcall    increment_range
               rjmp     chg_range
//-----
out_call:
               sei               // unlock interrupts
               rjmp     selfee   // OK repeat over and over again

//----------------------------------------------------
//--- after CAPTURED TIMER1 perform calc and print ---
//----------------------------------------------------
continuum:
               ldi      yl,low(RAMEND)
               out      SPL,yl // stack re-init
               ldi      yh,high(RAMEND)
               sbiw     yl,32
//---
               rcall    stop_meassure  // ok - stop timer, disable IRQ allowed, let's start discharge etc...
               rcall    SUBSTRACT_MY_LONG_INT // substract CALIBRATE OFFSET and repair offset if RESULT < 0
               lds      r16, range
               cpi      r16, 0
               brne     continuum_2
               rcall    make_picofarads  // prepare data and string for channel 1 range
               rjmp     continuum_4
continuum_2:
               cpi      r16, 1
               brne     continuum_3
               rcall    make_nanofarads  // prepare data and string for channel 2 range
               rjmp     continuum_4
continuum_3:
               rcall    make_microfarads  // prepare data and string for channel 3 range

continuum_4:
               rcall    LcdIni  // for visible effect screen refresh

//--------------------------------------------------
//------     print string to LCD display     -------
//--------------------------------------------------
//-----
               rcall    get_string_pointers  // make ASCII string and get address of variables/registers

               cpi      r16,0
               brne     pr_N_F

               ldi      r24, 'p'
               rcall    print_meassure   // pF
               ldi      r24, 'n'         // additional print as nF
               rjmp     noprt

pr_N_F:
//-----
               cpi      r16,1
               brne     pr_U_F

               ldi      r24, 'n'
               rcall    print_meassure   // nF
               ldi      r24, 'u'         // additional print as uF
               rjmp     noprt
pr_U_F:
               ldi      r24, 'u'
               rcall    print_meassure  // uF
               ldi      r24, 'm'        // additional print as mF
noprt:
               rcall    three_decades_low  // convert string for additional value
               rcall    print_big_number   // and print our fixed point

               ldi      zl, 0x00
               out      PORTB,zl  // lines to GND for stabilize

               rcall    _Wsec    // 3 sec wait and stabilize circuit
               rcall    _Wsec
               rcall    _Wsec

//----------------------------------
//--------------- adt chck ---------
//----------------------------------
               
			   lds      r16, range       
               cpi      r16,2
               brne     no_adt_disch

               rcall    divide_by_100  // divide to 100 uF per one
               ldi      divisor, 50    // divide to 5mF per one
               rcall    make_divide    // 5 mF per second discharge

			   rcall    copy_long_to_registers
dischg_loop:
			   rcall    copy_registers_to_long
               rcall    clear_top_line
               ldi      zl, low(dischg_info  * 2)
               ldi      zh, high(dischg_info * 2) 
               rcall    _LSc  // print discharge time info from 1st line
               rcall    get_string_pointers
print_buff:
               ld       r24, Z+
               rcall    _Lch
               dec      r17
			   cpi      r17,0
               brne     print_buff

               rcall    _Wsec

               rcall    clear_top_line

               rcall    copy_long_to_registers               

               ldi      r16, 1       // decrement long integer
               clr      r20

               sub      var1, r16
               sbc      var2, r20
               sbc      var3, r20
               sbc      var4, r20
               brcc     dischg_loop

no_adt_disch:
//-------------------------------------
               ldi      r24, 15
               rcall    _from_line1
               ldi      r24, '*'     // meassuring sign for reader
               rcall    _Lch

               ldi      zl, 0x00
               out      PORTB,zl  //// lines to GND for stabilize

               rcall    increment_range  // change range - try meassure in next range

               rjmp     starter
//-------------------------
three_decades_low:
               push     r24  // save r24
               ldi      r24, 1
               rcall    _from_line2
               rcall    divide_by_100  // delete 2 decades
               rcall    get_string_pointers 
               pop      r24  // restore r24
               ret
//---
get_string_pointers:
               rcall    BIN32u_2_ASCII   // first make the properly string
               lds      r16, range       // and get address of variables/registers
               lds      r17, LEN_ASCII_BUFF
               ldi      zl,low(OUT_ASCII_BUFF+10)
               ldi      zh,high(OUT_ASCII_BUFF+10)
               sub      zl, r17
               clr      r0
               sbc      zh, r0
               ret
//---
print_range:
               ldi      r24, 32
               rcall    _Lch
               pop      r24   // in r24 on the stack was "p" | "n" | "u" | "m" 
               rcall    _Lch
               ldi      r24, 'F'
               rjmp     _Lch
//---
print_three_last_digit:
               cpi      r18, 2
               breq     print_two_last_digit

               lds      r24, (OUT_ASCII_BUFF+7)
               rcall    _Lch
print_two_last_digit:
               lds      r24, (OUT_ASCII_BUFF+8)
               rcall    _Lch
print_last_digit:
               lds      r24, (OUT_ASCII_BUFF+9)
               rcall    _Lch
               rjmp     print_range
//---
print_0_dot:
               rcall    print_0
print_dot:     
               ldi      r24, '.'
               rjmp     _Lch
//-
print_0:
               ldi      r24, 0x30
               rjmp     _Lch
//-------------------------
print_meassure:
               ldi      r18, 2
               ldi      r19, 3
               rjmp     print_fixed_point
print_big_number:
               ldi      r18, 3
               ldi      r19, 4
print_fixed_point:
               push     r24
               cp       r17, r19
               brcc     print_from_buff
               rcall    print_0_dot
               cp       r17, r18
               breq     print_three_last_digit
               rcall    print_0
               cpi      r17, 2
               breq     print_two_last_digit
               rcall    print_0
               rjmp     print_last_digit
//---
print_from_buff:
               ld       r24, Z+
               rcall    _Lch
               dec      r17
               cp       r17,r18
               brne     print_from_buff

               rcall    print_dot
               rjmp     print_three_last_digit
//-------------------------
read_hi_data:
               lds      r22, range
               lds      r20, MY_LONG_INT+2
               lds      r21, MY_LONG_INT+3
               ret
;---------------------------
clear_top_line:
               rcall    set_top_sursor
			   ldi      r17, 0x10
clear_line:
               ldi      r24, 0x20     // clear top line
               rcall    _Lch
			   dec      r17
			   cpi      r17, 0
			   brne     clear_line
set_top_sursor:
               ldi      r24, 1
               rjmp    _from_line1
//================================================
stop_meassure:

               rcall    discharge                
//--------------------------
//--- disable interrupts ---
//---    stop   TIMER1   ---
//--------------------------
               clr      zl
               out      TIMSK, zl
               out      TCCR1B, zl
               out      TCCR1A, zl
               out      ACSR, zl

               in       r0, TIFR
               out      TIFR, r0  // clear all IRQ flags
               ret
//------------------------------------------------
start_meassure:
//------------------------
//--- block interrupts ---
//------------------------
               cli
               rcall    stop_meassure
               rcall    set_params
//--------------------------------------------------------------------------
//-----   PORTB 6 lines as output and ain0 ain1 = input (comparator)   -----
//--------------------------------------------------------------------------

               ldi      r23, DDRB_START_VALUE 
               out      DDRB,r23
               clr      r23
               out      PORTB,r23  // pin to GND 

//---------------------------------
//-----        Timer1 = 0    ------
//---------------------------------
               rcall    CLEAR_MY_LONG_INT
               clr      zh
               out      TCNT1H,zh
               out      TCNT1L,zh
               out      TCCR1A, zh
//--------------------------------------------------------
//----  allowed for IRQ capture TIMER1 by comparator  ----
//--------------------------------------------------------
               ldi      r20, TIMSK_START_VALUE
               out      TIMSK, r20
//---------------------------------------------------------------
//---------           setup prescaler = 1             -----------
//------       by bits CS12 CS11 CS10 in TCCR1B          --------
//---------              0    0    1                  -----------
//----------         PHISICAL START TIMER1           ------------
//---------------------------------------------------------------
               
               lds      zl, pre_val  // discharge and enable actual "range - channel"
               out      PORTD, zl
               ldi      zl, 0x06    // a few cycles delay for switch a transistor
ctt0:          dec      zl
               brne     ctt0

               lds      r21, load_val

               ldi      r20,TIMER1_START_VALUE      // CS12 CS11 CS10 = 0 1 0 
               out      TCCR1B, r20                 // START TIMER1

               out      PORTD,  r21                 // stop discharge stard charging _C_appacitor via "channel - range"

//--------------------------------------------------------
//-----             SETUP ACSR REGISTER              -----
//--------------------------------------------------------
//-----         allowed trigging of capture          -----
//-----        TIMER1  via comparator  ACIC = 1      -----
//-----   - - - - - - - - - - - - - - - - - - - - -  -----
//-----         event for starting interrupt         -----
//-----    rising edge on the out of comparator      -----
//-----       by bit ACIS1 ACIS0 in ACSR register    -----
//-----                1     1                       -----
//--------------------------------------------------------
               ldi      r24, ACSR_START_VALUE
               out      ACSR,r24

//-----------------------
//--- clear IRQ flags ---
//-----------------------
               in       R24,TIFR
               out      TIFR, R24
//---------

//-------------------------
//--- unlock interrupts ---
//-------------------------
               sei  // ok - wait for capture TC1
               ret
//***********************************************

//==================================
//========    discharge     ========
//==================================
discharge:
               ldi      r23, start_value_DDRD
               out      DDRD, r23
               ldi      r23, start_value_PORTD
               out      PORTD, r23
               ret
//------------------------
//------ IRQ SERVICE -----
//------------------------
_IRQ_CAPTURE_TC1:
//-----------------------------------------------
//-----      read captured TIMER1 value     -----
//-----      and store her to variable      -----
//-----------------------------------------------
               in       r0, ICR1L
               sts      MY_LONG_INT, r0
               
               in       r16, ICR1H
               sts      MY_LONG_INT+1, r16  // OK TC1 value is SAVED

               lds      r17, calibrate  // now check - we are in a calibrate mode or no ?
               cpi      r17,0
               brne     no_calibrate
//============================================
//=== IF calibrate is true so do it bellow ===
//============================================
               rcall    store_offset_value  // stor and next range do or finish
chg_range:
               rcall    discharge
               ldi      zl,50
               rcall    _Wms
               rjmp     starter   // meassure again next range
//---------------------------------------------------
//--- IF calibrate = false then we gotta meassure ---
//---------------------------------------------------
no_calibrate:
               rcall    read_hi_data
               cpi      r22, 1        // in R22 actual range
               brne     third_range

               cpi      r20, 0       // here we are in 1st range
               brne     first_range
               cpi      r16, 0
               brne     first_range
               mov      r16, r0
               subi     r16, 30     // check the minimal value for range and change it if necesseary
               brcc     first_range

set_first_range:
               sts      range, r20
               rjmp     chg_range

third_range:
               cpi      r22, 2
               brne     first_range
               ldi      divisor, 100 
               rcall    copy_long_to_registers               
               rcall    div32u_by_8u

               ldi      r16, 8       // check minimal value for 3rd range
               clr      r20

               sub      var1, r16
               sbc      var2, r20
               sbc      var3, r20
               sbc      var4, r20
               brcs     set_first_range
first_range:
               rjmp     continuum
//-----------------------------------------------------------
//---  save from R0 offset value for actual  tested range ---
//-----------------------------------------------------------
store_offset_value:
               cpi      r16,0
               breq     store_2
               sts      calibrate, r16
               ret
store_2:     
               mov     r24, r0
               rcall   print_hex  // print offset value for actual tested range

               lds     r16, range
               cpi     r16, 0
               brne    store_3
               sts     offs1, r0
increment_range:
               lds     r16, range
               inc     r16
               cpi     r16, 3
               brne    incr_r_2
               clr     r16
incr_r_2:      sts     range, r16
               ret
store_3:       cpi     r16,1
               brne    store_4
               sts     offs2, r0
               rjmp    increment_range
store_4:
               sts     offs3,r0
               sts     calibrate, r16  // r16 <> 0 so end of calibrate sign
store_5:
               rcall   discharge  // after store discharge, stabilize circuit and next range do...
               rcall   _Wsec
               rjmp    increment_range
//-----------------------------------

//----------------------------
//------ IRQ SERVICE END -----
//----------------------------

//-----------------------------------------------------
//--- store parameters for actual range to meassure ---
//-----------------------------------------------------
set_params:
               lds     r19, range
               andi    r19, 0x03
               breq    set_first
               cpi     r19, 0x01
               breq    set_second
set_third:
               ldi     r16, pre_value_3
               ldi     r17, load_value_3
               lds     r18 , offs3
               rjmp    write_params
set_second:
               ldi     r16, pre_value_2
               ldi     r17, load_value_2
               lds     r18 , offs2
               rjmp    write_params
set_first:
               ldi     r16, pre_value_1
               ldi     r17, load_value_1
               lds     r18 , offs1
write_params:
               sts     pre_val, r16
               sts     load_val, r17
               sts     offset, r18
               ret
//---------------------------
//-----------------------------------------------------------------------------:
// 32bit Unsigned convert BIN to ASCII
//
// Register Variables
//  Call:  
//         var[1:4]           = 32bit unsigned value (0x00000000..0xffffffff)
//         mod10              = <don't care> (high register must be allocated)
//         cntr_1             = <don't care> (high register must be allocated)
//
//
//  Result:
//         OUT_ASCII_BUFF[0:9] = 10 bytes will be overwrite as ASCII decimal bin representation
//         var[1:4]            = (0x00000000)
//         mod10               = length of ASCII numbers
//         cntr_1              = 0
//         xl,xh               = @OUT_ASCII_BUFF first ASCII number
//
//---------------

BIN32u_2_ASCII:
               rcall   copy_long_to_registers

               ldi     xl, low(OUT_ASCII_BUFF+10)
               ldi     xh, high(OUT_ASCII_BUFF+10)

               ldi     divisor, 10   //divide by 10

BIN_TO_ASCII:
               rcall   div32u_by_8u  //make divide by 10
//---------------------------
               ori     mod10, 0x30   // O_o HERE we gotta our the rest of div10 :] o_O
               st      -x,mod10      // store as ASCII NUMBER
               cpi     xl, low(OUT_ASCII_BUFF)
               brne    BIN_TO_ASCII

               ldi     mod10,9

clr_lead:      ld      cntr_1, x
               cpi     cntr_1, 0x30
               brne    stop_it
               ldi     cntr_1, 0x20
               st      x+, cntr_1
               dec     mod10
               brne    clr_lead
stop_it:
               inc     mod10
               sts     LEN_ASCII_BUFF, mod10  // save how many bytes ASCII NUMBERS strig is here
               ret
//---
//------------------------------------------------------------
//--- divide unsigned 32bit value by 8bit unsigned divisor ---
//------------------------------------------------------------
//--- input :                                              ---
//--- var 1,2,3,4 = 32 bit unsigned value                  ---
//--- divisor: our 8bit unsigned value of divide           ---
//------------------------------------------------------------
//--- output:                                              ---
//--- mod10 is touched, rest of divide                     ---
//--- var 1,2,3,4 = RESULT                                 ---
//--- cntr_1 is touched = 0                                ---
//------------------------------------------------------------
div32u_by_8u:
               clr     mod10          // initialize mod10 = 0

               ldi     cntr_1, 32     // lc = 32 (times bits to shift)
divloop:                              // simple 32bit_u divide by const 8bit_u
               lsl     var1          
               rol     var2
               rol     var3
               rol     var4
               rol     mod10
               cp      mod10, divisor     // divisor value
               brcs    divlp2
               inc     var1
               sub     mod10, divisor     // divisor value
divlp2:
               dec     cntr_1
               brne    divloop
               ret
//---
//-----------------------------------------------------------------------
//--- copy from STATIC RAM our 32bit value into registers var 1,2,3,4 ---
//-----------------------------------------------------------------------
//--- input : don't care                                              ---
//--- output :                                                        ---
//--- var 1,2,3,4 = 32bit value                                       ---
//--- XL, XH, ZL, ZH, cntr_1 is touched                               ---
//--- cntr_1 = 0                                                      ---
//--- XL,XH  = @(var4 + 1)                                            ---
//--- ZL,ZH  = @(MY_LONG_INT + 4)                                     ---
//-----------------------------------------------------------------------
copy_long_to_registers:
               rcall   set_addr_var
COPY_1:
               ld      var4, Z+
               st      x+, var4
               dec     cntr_1
               brne    COPY_1
               ret
//---
set_addr_var:
               ldi     xl, 1
               clr     xh

set_my_long_addr:

               ldi     zl, low(MY_LONG_INT)
               ldi     zh, high(MY_LONG_INT)
               ldi     cntr_1,4
               ret
//-------------
copy_registers_to_long:
               rcall   set_addr_var
COPY_2:
               ld      var1, X+
               st      Z+, var1
               dec     cntr_1
               brne    COPY_2
               ret
//-------------
CLEAR_MY_LONG_INT:
               rcall   set_my_long_addr
               clr     r0
COPY_4:
               st      Z+, r0
               dec     cntr_1
               brne    COPY_4
               ret
//-------------
//------------------------------------------------------------------
//--- substract CALIBRATE OFFSET and repair offset if RESULT < 0 ---
//------------------------------------------------------------------
SUBSTRACT_MY_LONG_INT:
               
               lds     r0, offset
               rcall   set_my_long_addr

               clc     // prepare carry for substract

COPY_5:
               ld      r1, Z
               sbc     r1, r0
               st      Z+, r1
               clr     r0
               dec     cntr_1
               brne    COPY_5
               brcs    repair_offset
               ret
//---
repair_offset:
               lds     r0, offset
               lds     r1, MY_LONG_INT
               add     r0, r1

               lds     r2, range

               ldi     ZH, high(offs1)
               ldi     ZL, low(offs1)
               add     ZL, r2
               st      Z,      r0
               rjmp    CLEAR_MY_LONG_INT
//-----------------------------------------------------------------------------:

//-------------------------------------------------------------------
//----------   operations for prepare fixed point value   -----------
//-------------------------------------------------------------------
make_nanofarads:
               ldi     multiplier, 2   // multiple 2 times
               rjmp    make_mult
make_picofarads:
               ldi     multiplier, 25
make_mult:
               rcall   copy_long_to_registers               
               rcall   mul32u
               rjmp    copy_registers_to_long

//------------------------------------------------------------------
// ---------- like above - prepare our fixed point value  ----------
//------------------------------------------------------------------
divide_by_100:
               ldi     divisor, 100
make_divide:
               rcall   copy_long_to_registers               
               rcall   div32u_by_8u
               rjmp    copy_registers_to_long
//---
make_microfarads:
               ldi     divisor, 10   //divide by 95
               rjmp    make_divide
//-------------

//-----------------------------------------------------------------------------
// 32bit x 8bit unsigned multiply
//
// Register Variables
//  Call:  var1-4         = 32bit multiplicand
//         multiplier     = multiply 8 bit unsigned value
//         mod10          = <don't care>
//         cntr_1         = <don't care> (high register must be allocated)
//
//  Result:var1-4         = 32bit result
//         multiplier     = <not changed>
//         cntr_1         = 0
//
//-------------------------
mul32u:               
               ldi     cntr_1,33
               sub     mod10, mod10  // prepare carry
mul_loop:
               brcc    mul_lp2
               add     mod10,multiplier
mul_lp2:
               ror     mod10               
               ror     var4
               ror     var3
               ror     var2
               ror     var1
               dec     cntr_1
               brne    mul_loop
               ret
//-----------------------------------------
print_hex:          
               push    r24
               swap    r24
               rcall   pr_h1
               pop     r24
pr_h1:         andi    r24,0x0f
               cpi     r24,10
               brcs    pr_h2
               subi    r24,-7
pr_h2:         subi    r24,-'0'
               rjmp    _Lch
//----------------------------------------
//-------------------------
//---  init LCD display ---
//-------------------------
//---------------------
// Lcd = PORTB.4,5,6,7
// RS=PORTB.2
// EN=PORTB.3
// LCD_TYPE = 16 * 2
//---------------------
LcdIni:     
               ldi     zl, DDRB_START_VALUE  // 11111100
               out     DDRB,zl
               //sbi          DDRB,2
               //sbi          DDRB,3
               cbi     PORTB,2
               cbi     PORTB,3
               ldi     zl, 0x18
               rcall   _Wms
               ldi     r24, 0x03
               rcall   _LOut
               ldi     zl, 0x08
               rcall   _Wms
               rcall   _LEN
               ldi     zl,0x0d
               rcall   _Wus
               rcall   _LEN
               cbi     PORTB,4
               rcall   _LEN
               ldi     r24, 0x28
               rcall   _LCtr
               ldi     r24, 0x06
               rcall   _LCtr
               ldi     r24, 0x0c
               rcall   _LCtr
_CLS:
               ldi     r24, 0x01
               rcall   _LCtr
               ldi     zl, 0x04
               rjmp    _Wms
//---------------------------
//--- print char from R24 ---
//---------------------------
_Lch:          sbi     PORTB,2
               rjmp    _LNib
//---
_from_line2:
               ori     r24, 0x40
_from_line1:
               ori     r24, 0x80
               dec     r24
//---
_LCtr:         cbi     PORTB,2
//---
_LNib:         mov     r21, r24
               swap    r24
               rcall   _LOut
               mov     r24, r21
_LOut:         in      r23, PORTB
               andi    r23, 0x0f
               swap    r24
               andi    r24, 0xf0
               or      r24, r23
               out     PORTB, r24
//---------------------
//--- EN line setup ---
//---------------------
_LEN:          sbi     PORTB, 3
               sbi     PORTB, 3 // 2 cycles delay !! FOR 8 MHZ !!!
               sbi     PORTB, 3 // 2 cycles delay !! FOR 8 MHZ !!!
               cbi     PORTB, 3
               push    zl
               ldi     zl, 0x04
               rcall   _Wus
               pop     zl
_stop:          
               ret
//--------------------------------------
//--- print string from flash memory ---
//--------------------------------------
_LSc:          lpm
               tst     r0
               breq    _stop
               mov     r24,r0
               rcall   _Lch
               adiw    zl,1
               rjmp    _LSc
//------------------------------------------
_Wsec:
               ldi     zl, 232
               rcall   _Wms
_Whalf:
               rcall   _Wms
               rcall   _Wms
//------------------------------------
//---- !!! FOR 8MHZ CLOCK  !!!  ------
//------------------------------------

//----------------------------
//--- a mili seconds delay ---
//----------------------------
_Wms:          ldi     r20, 0x14
_Wms1:         ldi     r21, 0x85
_Wms2:         dec     r21
               brne    _Wms2
               dec     r20
               brne    _Wms1
               dec     zl
               brne    _Wms
               ret
//-----------------------------
//--- a micro seconds delay ---
//-----------------------------
_wus:          ldi     r22, 25
_wus1:         dec     r22
               brne    _wus1
               dec     zl
               brne    _wus
               ret
//--------------------------
//-- a start info string ---
//--------------------------
welcome:
               .db "  WEGI C meter" , 0
info_txt:
               .db "calibr. 0x" , 0

dischg_info:
               .db "discharge " , 0
//-------------------
// todo :
// ATmega328 version for ardiuno uno R3 shield C_meter
//-------------------
