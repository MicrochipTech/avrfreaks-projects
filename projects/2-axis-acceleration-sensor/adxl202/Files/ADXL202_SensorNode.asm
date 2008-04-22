;******************************************************************************
;*                                                                            *
;* 2 Axis Acceleration Sensor Node using ADXL202                              *
;*                                                                            *
;* Calculate the X & Y acceleration and transmit in groups of 8               *
;*                                                                            *
;* (c)2008 the WizardHobo                                                     *
;*                                                                            *
;*    This program is free software: you can redistribute it and/or modify    *
;*    it under the terms of the GNU General Public License as published by    *
;*    the Free Software Foundation, either version 3 of the License, or       *
;*    (at your option) any later version.                                     *
;*                                                                            *
;*    This program is distributed in the hope that it will be useful,         *
;*    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
;*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
;*    GNU General Public License for more details.                            *
;*                                                                            *
;*    You should have received a copy of the GNU General Public License       *
;*    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
;*                                                                            *
;* email: the.WizardHobo@theJuker.org                                         *
;*                                                                            *
;******************************************************************************
;
;Theory of operation
;
; 0. Context
;
;    The code presented here is originally written to create a sensor module
;    which will be embedded in a distributed hierarchical mesh. Ultimately
;    this mesh will become the central nervous system controlling my hooverbot.
;    As this mesh is constructed from nodes linked by pipes, these
;    elements are present in the code.
;
; 1. Measuring the acceleration
;
;    The ADXL202 produces 2 pulse width modulated signals which carry the
;    acceleration information. One signal for acceleration along the X axis,
;    and one for acceleration along the Y axis. These PW modulated signals
;    have a nominal duty cycle of 50% which deviates by 12.5% per g.
;    Thus by measuring the time between flanks, the duty cycle and hence the
;    accelerarion can be calculated.
;
;    The highest counting efficiency is accomplished by counting X and Y
;    acceleration interleaved (application note Analog Devices).
;
;    -2g @X      +---            ----            ----    
;          ------    +-----------    ------------    ------
;      
;    +2g @Y    ------------    +-----------    ------------  
;          ----            ----            +---      
;    T           a   b         c           d
;
;
;     0g @X        +-------        --------        --------  
;          --------        +-------        --------
;          
;     0g @Y        --------        +-------        --------
;          --------        --------        +-------
;    T             a       b       c       d
;
;
;    +2g @X    +-----------    ------------    ------------  
;          ----            ----            ----      
;    +2g @Y    ------------    +-----------    ------------  
;          ----            +---            +---      
;    T         a           b   c           d
;
;    The X-axis signal is connected with INT0 and the Y-axis signal is
;    connected with INT1. Both inputs trigger an interrupt on either a
;    rising or falling edge. In all four distinct interrupt service handlers
;    are executed. Each stores the current value of a free running counter
;    except for the Ta interrupt service handler which resets the counter
;    to zero.
;
;    The acceleration can be calculated from:
;
;    a(g) = ((T1 / T2) - D0) / D1
;
;    T1 = number of counts between a rising and a falling edge
;    T2 = number of counts between two rising edges.
;    D0 = duty cycle at 0g
;    D1 = duty cycle deviation per g
;
;    As a first (uncalibrated) approximation D0 = .5 and D1 = .125
;
;    a(g) = ((T1 / T2) - .5) / .125   = 8 * (T1/T2) - 4 
;         = 4 * ((2 * T1/T2) - 1)
;         = 4 * ((2 * T1/T2) - T2/T2) = 4 * ((2 * T1 - T2)/T2)
;
;    In this application the factor 2 * T1 is implemented by adding the
;    measured T1 times for two concecutive measurement together.
;
; 2. Timing
;
; 2a. Selection of the ADXL cycle time.
;
;     To use maximum dynamic range of the counters the counter frequency must
;     be selected such that a counter should almost reach 0xFFFF at Td when
;     reset at Ta. The duty cycle changes with 12.5% per g and the maximum
;     deviation is 25% either way from the central 50% duty cycle @ 0g.
;
;        + Cycle length 10mS 
;            - longest time interval Td - Ta = 17.5mS
;            - time per increment            = 17.5mS / 65535 = .267E-6 S
;            - max counter freq              = 3.7MHz
;        + Cycle length 4mS 
;            - longest time interval Td - Ta = 7mS
;            - time per increment            = 7mS / 65535 = .107E-6 S
;            - max counter freq              = 9.3MHz
;
;    For AVR's, the counter freq can be set to the cpu freq divided by 1, 8,
;    64, 256 or 1024. This means that the counter frequencies which produce
;    the highest resolution are:
;
;        + Cycle length 10mS
;            - Counter freq = 1MHz
;            - Resolution   = 0.8mG
;        + Cycle length 4mS
;            - Counter freq = 8MHz
;            - Resolution   = 0.25mG
;
;     A 4mS cycle length is used in this application as this makes maximum
;     use of the available timer resolution.
;
; 2b. Interrupt response time
;
;     It is assumed that the CPU is running at 8MHz and that it is in sleep
;     mode when an interrupt occurs..
;
;          6 cycles (sleep wakeup time)
;        + 4 cycles (extra time after wake-up)
;        + 4 cycles (loading ivec into PC)
;        + 3 cycles (jump instruction to actual routine)
;       = 17 cycles @ .125uS = 2.125uS
;
; 2c. Mimimum available interrupt service time
;
;    When an AVR without capture register is used, the counter value must be
;    cleared at point Ta and sampled at points Tb, Tc and Td in an interrupt
;    handler routine. Running at 8Mhz, an AVR executes approximately 4e6
;    instructions per second. This sets the upper limit to the number of
;    instructions that can be executed.
;    The minimum time avaialble to this routine is at +2g or -2g when the 
;    duty cycle is only 25%. 
;
;        + Cycle length 4mS
;            - min pulse time   = 1 mS   (25% @ 2g)
;            - max instructions = 4000
;        + Cycle length 10mS
;            - min pulse time   = 2.5 mS (25% @ 2g)
;            - max instructions = 10000
;
; 3. Transmission of measurement data
;
;    A double buffered design is used to tranmit a burst of eight measurements.
;    While one frame is being transmitted, the other is filled with new data.
;
;    Each measurement consists of two 15 bit fixed point number between 0 and
;    2 with a sign bit. Eight measurements are collected into a packet and
;    transmitted over the UART embedded in a frame.
; 
; 3a. The dividing result
;
;     To calculate the acceleration from the counter values the factor
;     (2 * T1 - T2) must be divided by T2. As T2 is guaranteed to be larger
;     than this factor the result of the division is obviously smaller than
;     1. The result must therefore not be interpreted in the traditional way
;     where bit15 represents a value of 2^15 and bit0 a value of 2^0. Instead
;     bit15 is the sign bit, bit14 represents 2^0, bit13 represents 2^-1,
;     bit12 represents 2^-2, and so forth.
;
; 3b. Frame format
;
;     Each frame starts with the length of bytes to follow (i.e. not including
;     the length byte itself). This allows an endless sequence of $00 to be
;     a valid frame stream.
;     Following the length byte is a token indicating the type of frame being
;     transmitted. Currently only the values 'A' (0x41) and 'S' (0x53) are
;     defined for actuator data frames and sensor data frames respevtively.
;     The frame type token is followed by a node number. This number is
;     arbitrarily assigned and indicates the originator of the frame. Receivers
;     can use this to filter unwanted traffic.
;     After the node number follows a command/data type byte used by the
;     receiver to decide what to do with the payload and the length - 4 bytes
;     of actual payload.
;     Finally, a frame is terminated with the inverse of the node number. This
;     termination bytes is provided as a very crude collision check as the
;     intended use of this communication protocol is in a multi-master network.
;
;------------------------------------------------------------------------------
;-- Communication protocol definitions                                       --
;------------------------------------------------------------------------------
;
;-- Node Numbers ----
.equ    AcclNodeID   = $FB
.equ    AcclNodeColl = ~AcclNodeID

;
;-- Frame structure ----
;
.equ    F_PacketLenIndex = 0
.equ    F_PipeTypeIndex  = 1
.equ    F_NodeIndex      = 2
.equ    F_CmdIndex       = 3
.equ    F_DataStartIndex = 4
;
;-- Pipe types ----
;
;Pipes transport data packets in one direction. Data targeted at actuators
;are transported through the 'A' pipe while data generated by sensors are
;transportorted though the 'S' pipe.
.equ    A_PipeType   = 'A'
.equ    S_PipeType   = 'S'
;
;-- Actuator Commands ----
;
	;The Acceleromer node does not incorporate an actuator node.
;
;
;-- Sensor Commands ----
;
.equ    Accl2Cmd         = $03
;
;-- Sensor Command Parameter count
;
.equ    Accl2ParamCount   = 8                   ;Eight XY acceleration
                                                ;measurement pairs
;
;-- Packet lengths ----
;
.equ    EmptyPacketLen   = 1 + 2                ;PipeType + Node# + ~Node#
.equ    CmdOnlyPacketLen = EmptyPacketLen + 1   ;Empty packet + command byte
;
;-- Actuator Command Packet lengths ----
;
	;The Accelerometer node does not incorporate an actuator node.
;
;
;-- Sensor Command Packet lengths ----
;
.equ    Accl2PayloadLen   = Accl2ParamCount * 4
.equ    Accl2CmdPacketLen = CmdOnlyPacketLen + Accl2PayloadLen  

;
;-- Pipe speed ----
;
.equ    BaudRate = 38400

;------------------------------------------------------------------------------
;-- Device specific                                                          --
;------------------------------------------------------------------------------
.include "TN2313def.inc"     

.equ    clock         = 8000000                    ;clock frequency = 8MHz
.equ    BaudPrescaler = (clock/(16*BaudRate))-1

;
;ATtiny2313 pin assignment:
;   1 - power on reset
;   2 - RxD
;   3 - TxD
;   5 - Xtal1
;   4 - Xtal2
;   6 - INT0 = X axis PWM input
;   7 - INT1 = Y axis PWM input
;  17 - PB5  = Frame transmission in progress
;  18 - PB6  = Y axis T1 measurement in progress
;  19 - PB7  = X axis T1 measurement in progress
;
;-- Port assignments ----
;
.equ    X_AXIS = INT0    ;pin6
.equ    Y_AXIS = INT1    ;pin7

;
;-- Register assignments ----
;

.def    Zero             = r0
.def    SampleCount      = r1
.def    Time_T1xL_Sum    = r2
.def    Time_T1xH_Sum    = r3
.def    Time_T1xL        = r4
.def    Time_T1xH        = r5
.def    Time_T1yL_Sum    = r6
.def    Time_T1yH_Sum    = r7
.def    Time_T1yL        = r8
.def    Time_T1yH        = r9
.def    Time_T2L         = r10
.def    Time_T2H         = r11
.def    DividendH        = r12
.def    DividendL        = r13
.def    S_TxPacketPtr    = r14
.def    FillingFrame1    = r15

.def    IrqTemp1         = r16
.def    IrqTemp2         = r17
.def    IrqTemp3         = r18
.def    GlobalTemp1      = r19
.def    GlobalTemp2      = r20
.def    DivSign          = r21
.def    LoopCount        = r22
.def    S_TxFrameCount   = r23
;Xl:h = r26:r27
;Yl:h = r28:r29
;Zl:h = r30:r31

;------------------------------------------------------------------------------
;-- SRAM data                                                                --
;------------------------------------------------------------------------------
.DSEG
S_TxPipeFrame1:   .byte Accl2CmdPacketLen + 1
S_TxPipeFrame2:   .byte Accl2CmdPacketLen + 1

;------------------------------------------------------------------------------
;-- EEPROM data                                                              --
;------------------------------------------------------------------------------
.ESEG
S_FrameProto:   .DB Accl2CmdPacketLen
                .DB 'S', AcclNodeID, Accl2Cmd 
                .DB 'X', 'x', 'Y', 'y'          ;pair 1
                .DB 'X', 'x', 'Y', 'y'          ;pair 2
                .DB 'X', 'x', 'Y', 'y'          ;pair 3
                .DB 'X', 'x', 'Y', 'y'          ;pair 4
                .DB 'X', 'x', 'Y', 'y'          ;pair 5
                .DB 'X', 'x', 'Y', 'y'          ;pair 6
                .DB 'X', 'x', 'Y', 'y'          ;pair 7
                .DB 'X', 'x', 'Y', 'y'          ;pair 8
                .DB AcclNodeColl

;------------------------------------------------------------------------------
;-- Macro's                                                                  --
;------------------------------------------------------------------------------
.macro FALSE
    clr     @0
.endmacro
.macro TRUE
    FALSE   @0
    com     @0
.endmacro
.macro IFTRUE
    tst     @0
    brne    @1
.endmacro
.macro IFFALSE
    tst     @0
    breq    @1
.endmacro
    
;------------------------------------------------------------------------------
;-- Code starts here                                                         --
;------------------------------------------------------------------------------
.CSEG
;------------------------------------------------------------------------------
;-- Reset vector                                                             --
;------------------------------------------------------------------------------
    .org 0x0000
    rjmp    ColdBoot

;------------------------------------------------------------------------------
;-- Interrupt vectors                                                        --
;------------------------------------------------------------------------------
;External Interrupt0 -- X axis accelerometer edges
    .org    INT0addr
    rjmp    Irq_Xaxis

;External Interrupt1 -- Y axis accelerometer edges
    .org    INT1addr
    rjmp    Irq_Yaxis

;Input capture interrupt 1 -- not used
    .org    ICP1addr
    reti

;Timer/Counter1 Compare Match A -- not used
    .org    OC1Aaddr
    reti

;Overflow1 Interrupt -- not used
    .org    OVF1addr
    reti

;Timer0 overflow interrupt -- not used
    .org    OVF0addr
    reti

;USART0 RX Complete Interrupt -- not used
    .org    URXC0addr
    reti

;USART0 Data Register Empty Interrupt -- Transmit the next byte in a packet
    .org    UDRE0addr
    rjmp    Irq_TxUartByte

;USART0 TX Complete Interrupt -- not used (only needed for half duplex)
    .org    UTXC0addr
    reti

;Analog Comparator Interrupt -- not used
    .org    ACIaddr
    reti

;Pin Change Interrupt -- not used
    .org    PCIaddr
    reti

;------------------------------------------------------------------------------
;-- Interrupt handlers                                                       --
;------------------------------------------------------------------------------
;
;Reading both ADXL inputs efficiently:
;
;     _______         _______
;X __|       |_______|       |_______
;
;    Ta      Tb
;      _____           _____
;Y ___|     |_________|     |_______
;
;                     Tc    Td
;
;For debugging and timing purposes, output PB7 is high during Ta - Tb while
;output PB6 is high during Tc - Td.
 
.equ    X_EdgeSignBit = ISC00
.equ    Y_EdgeSignBit = ISC10
.equ    X_AxisFlag    = INTF0
.equ    Y_AxisFlag    = INTF1

Irq_Xaxis:
    in      IrqTemp1, SREG
    push    IrqTemp1

    ;up or down edge?
    in      IrqTemp3, MCUCR
    bst     IrqTemp3, X_EdgeSignBit
    ;The edge sign bit is copied into the T flag
    ;  if this flag is 0 then a falling edge caused
    ;  this interrupt and Tb must be recorded,
    brtc    Irq_Xaxis_Tb
    ;  if it is 1 however then a rising edge caused
    ;  this interrupt and Ta resets the counter
    out     TCNT1H, Zero
    out     TCNT1L, Zero

    ;Extra work in this interrupt cycle
    ;
    tst     SampleCount
    breq    Irq_Xaxis1

    mov     IrqTemp1, SampleCount
    cpi     IrqTemp1, 1
    brne    Irq_Xaxis2

    ;  If the first of a pair of measurements was made in the
    ;  previous cycle then it is moved to the summing registers
    movw    Time_T1xH_Sum:Time_T1xL_Sum, Time_T1xH:Time_T1xL
    movw    Time_T1yH_Sum:Time_T1yL_Sum, Time_T1yH:Time_T1yL
    rjmp    Irq_Xaxis1
Irq_Xaxis2:
    ;  If a measurement has been made in 2 previous cycles
    ;  then the acceleration is calculated here.
    ;    a(g) = (T1/T2 - 50%) / 12.5%
    ;    a(g) = 4 * (2*T1-T2)/T2
    ;    a(g) = 4 * ((T1a+T1b)-T2)/T2
    ;
    ;Add the second measurement to the first 
    add     Time_T1xL_Sum, Time_T1xL
    adc     Time_T1xH_Sum, Time_T1xH
    add     Time_T1yL_Sum, Time_T1yL
    adc     Time_T1yH_Sum, Time_T1yH
    ;Substract T2 from T1a + T1b
    sub     Time_T1xL_Sum, Time_T2L
    sbc     Time_T1xH_Sum, Time_T2H
    sub     Time_T1yL_Sum, Time_T2L
    sbc     Time_T1yH_Sum, Time_T2H

    ;Use Time_T1xL, Time_T1xH as Quotient register
    ;Use Time_T1yL, Time_T1yH as Partial remainder register
    movw    DividendH:DividendL, Time_T1xH_Sum:Time_T1xL_Sum
    rcall   Div16x16
    ;Store the result in the frame buffer which
    ;is not being transmitted
    st      X+, Time_T1xH
    st      X+, Time_T1xL

    movw    DividendH:DividendL, Time_T1yH_Sum:Time_T1yL_Sum
    rcall   Div16x16
    ;Store the result in the frame buffer which
    ;is not being transmitted
    st      X+, Time_T1xH
    st      X+, Time_T1xL

    ;Clear the sample counter
    clr     SampleCount

    inc     S_TxFrameCount

Irq_Xaxis1:
    ;Test make PB7 == 1
    sbi     PORTB, PB7
    ;
    ;Next interrupt on falling edge (Tb, Td)
    andi    IrqTemp3, ~( (1<<X_EdgeSignBit)|(1<<Y_EdgeSignBit) )
    out     MCUCR, IrqTemp3
    rjmp    Irq_Exit

Irq_Xaxis_Tb:
    ;Record 16bit timer value; reading low bytes latches high byte
    in      IrqTemp1, TCNT1L
    in      IrqTemp2, TCNT1H
    ;Test make PB7 == 0
    cbi     PORTB, PB7
    ;
    ;-- Store Tb - Ta as T1x -----------------------------------------------
    ;
    movw    Time_T1xH:Time_T1xL, IrqTemp2:IrqTemp1
    ;
    ;Next interrupt on rising edge (Ta, Tc)
    ori     IrqTemp3, (1<<X_EdgeSignBit)|(1<<Y_EdgeSignBit)
    out     MCUCR, IrqTemp3
    ;
    ;Clear any pending interrupt requests for the Y axis
    ;  before enabling Y axis measurement
    ldi     IrqTemp3, (1<<Y_AxisFlag)
    out     EIFR, IrqTemp3
    ;
    ;Y axis edge measurements come next
    ldi     IrqTemp1, (1<<Y_AXIS)
    out     GIMSK, IrqTemp1
    rjmp    Irq_Exit

;----------------------------------------------------------------------------
Irq_Yaxis:
    in      IrqTemp1, SREG
    push    IrqTemp1
    ;up or down edge?
    in      IrqTemp3, MCUCR
    bst     IrqTemp3, Y_EdgeSignBit
    ;The edge sign bit is copied into the T flag
    ;  if this flag is 0 then a falling edge caused
    ;  this interrupt and Td must be recorded,
    brtc    Irq_Yaxis_Td
    ;
    ;If T bit is 1 then a rising edge caused
    ;  this interrupt and Tc must be recorded.
    in      IrqTemp1, TCNT1L
    in      IrqTemp2, TCNT1H
    ;Test make PB6 == 1
    sbi     PORTB, PB6
    ;
    ;-- Store Tc temporarily in T1y ------------------------------------------
    ;
    movw    Time_T1yH:Time_T1yL, IrqTemp2:IrqTemp1 
    ;
    ;Next interrupt on falling edge (Td)
    andi    IrqTemp3, ~((1<<X_EdgeSignBit)|(1<<Y_EdgeSignBit))
    out     MCUCR, IrqTemp3
    rjmp    Irq_Exit

Irq_Yaxis_Td:
    ;Record 16bit timer value; reading low bytes latches high byte
    in      IrqTemp1, TCNT1L
    in      IrqTemp2, TCNT1H
    ;Test make PB6 == 0
    cbi     PORTB, PB6
    ;
    ;-- Calculate T2 and T1y ------------------------------------------------
    ;
    ;1. Store Td in T2
    movw    Time_T2H:Time_T2L, IrqTemp2:IrqTemp1
    ;2. Calculate T1y = Td - Tc
    sub     IrqTemp1, Time_T1yL
    sbc     IrqTemp2, Time_T1yH
    ;3. Store Td - Tc in T1y
    movw    Time_T1yH:Time_T1yL, IrqTemp2:IrqTemp1
    ;4.  Calculate T2 = Td - T1y/2 - T1x/2
    ;4a. Calculate Ty1/2
    lsr     IrqTemp2
    ror     IrqTemp1
    ;4b. Substract 4a. from 1.
    sub     Time_T2L, IrqTemp1
    sbc     Time_T2H, IrqTemp2
    ;4c. Calculate T1x/2
    movw    IrqTemp2:IrqTemp1, Time_T1xH:Time_T1xL
    lsr     IrqTemp2
    ror     IrqTemp1
    ;4d. Substract 4c. from 4b.
    sub     Time_T2L, IrqTemp1
    sbc     Time_T2H, IrqTemp2
    ;One XY sample capture complete
    inc     SampleCount
    ;--------------------------------------------------------------------------
    ;Next interrupt on rising edge (Ta,Tc)
    ori     IrqTemp3, (1<<X_EdgeSignBit)|(1<<Y_EdgeSignBit)
    out     MCUCR, IrqTemp3
    ;
    ;Clear any pending interrupt requests for the Y axis
    ;  before enabling Y axis measurement
    ldi     IrqTemp3, (1<<X_AxisFlag)
    out     EIFR, IrqTemp3
    ;
    ;X axis edge measurements come next
    ldi     IrqTemp1, (1<<X_AXIS)
    out     GIMSK, IrqTemp1
    rjmp    Irq_Exit
  
;------------------------------------------------------------------------------
Irq_Exit:
    pop     IrqTemp1
    out     SREG, IrqTemp1
    reti

;------------------------------------------------------------------------------
Irq_TxUartByte:
    in      IrqTemp1, SREG
    push    IrqTemp1
    ;
    ldi     IrqTemp1, Accl2CmdPacketLen  ;fetch the packet length
    inc     IrqTemp1                     ;and add the packet lenght byte
    cp      S_TxPacketPtr, IrqTemp1
    brsh    Irq_TxUartDone
    ld      IrqTemp1, Y+
    out     UDR, IrqTemp1
    inc     S_TxPacketPtr
    rjmp    Irq_Exit
Irq_TxUartDone:
    cbi     UCSRB, UDRIE
    cbi     PORTB, PB5
    rjmp    Irq_Exit
  
;------------------------------------------------------------------------------
;-- Routines                                                                 --
;------------------------------------------------------------------------------
;Read this frame prototype BEFORE interrupts are enabled
ReadSFrameProto:
    sbic    EECR,EEPE			; Wait for completion of previous write
    rjmp    ReadSFrameProto

    ldi     YH, high(S_TxPipeFrame1)
    ldi     YL, low (S_TxPipeFrame1)
    ldi     ZH, high(S_TxPipeFrame2)
    ldi     ZL, low (S_TxPipeFrame2)
    ldi     XL, Accl2CmdPacketLen
    inc     XL
    ldi     GlobalTemp2, S_FrameProto
ReadCopy:
    out     EEARL, GlobalTemp2
    sbi     EECR,EERE			; Start eeprom read by writing EERE
    in      GlobalTemp1,EEDR		; Read data from data register
    st      Y+, GlobalTemp1
    st      Z+, GlobalTemp1
    inc     GlobalTemp2
    dec     XL
    brne    ReadCopy
    ret

;-- 16 x 16 divider returns a result between 1 and 0 if divisor > dividend
;   MSbit is used as sign bit
;
;-- input ---
;
;Dividend = DividendH:DividendL
;Divisor  = Time_T2H:Time_T2L
;
;-- output ---
;
;Quotient  = Time_T1xH:Time_T1xL
;Remainder = Time_T1yH:Time_T1yL
;
Div16x16:
    FALSE   DivSign;
    ldi     LoopCount, 16

    ;Store the dividend in the partial remainder registers
    movw    Time_T1yH:Time_T1yL, DividendH:DividendL
    ;Clear the Quotient register
    eor     Time_T1xH, Time_T1xH
    eor     Time_T1xL, Time_T1xL
    ;Normalise the dividend (the divisor MUST be positive!)
    tst     DividendH
    brpl    Div16x16_Normal
    TRUE    DivSign
    neg     Time_T1yH
    neg     Time_T1yL
Div16x16_Normal:
    clc

Div16x16_Loop:
    dec     LoopCount
    breq    Div16x16_Renorm
    ;Shift partial remainder and quotient left one bit
    ;  while adding result from previous substraction result
    rol     Time_T1xL
    rol     Time_T1xH
    rol     Time_T1yL
    rol     Time_T1yH
    ;Substract divisor from partial remainder
    sub     Time_T1yL, Time_T2L
    sbc     Time_T1yH, Time_T2H
    ;If result is minus restore the partial remainder
    sec
    brpl    Div16x16_Loop
    add     Time_T1yL, Time_T2L
    adc     Time_T1yH, Time_T2H
    clc
    brcc    Div16x16_Loop

Div16x16_Renorm:
    IFFALSE DivSign, Div16x16_Done
    neg     Time_T1xH
    neg     Time_T1xL
Div16x16_Done:
    ret

;------------------------------------------------------------------------------
;-- ColdBoot                                                                 --
;------------------------------------------------------------------------------
ColdBoot:
    clr     Zero
    clr     SampleCount
    clr     Time_T1xH
    clr     Time_T1xL
    clr     Time_T1yH
    clr     Time_T1yL

;--
;-- Initialiaze the stack
;--
    ldi     GlobalTemp1, low(RAMEND)               
    out     SPL, GlobalTemp1

;--
;-- Set clock prescaler to 1 (Fuse is set to DIV8)
;--
    ldi     GlobalTemp1, (1<<CLKPCE)
    out     CLKPR, GlobalTemp1
    clr     GlobalTemp1
    out     CLKPR, GlobalTemp1
        
;--
;-- Initialiaze the S_Pipe packet template in SRAM
;--
    rcall   ReadSFrameProto

;--
;-- Initialiaze the ports
;--
    ldi     GlobalTemp1, (1<<PB5)|(1<<PB6)|(1<<PB7)
    out     DDRB, GlobalTemp1                      
    out     PORTB, globalTemp1
    ldi     GlobalTemp1, (1<<X_AXIS)
    out     GIMSK, GlobalTemp1

;--
;-- Initialiaze USART
;--
    ;Set the baud rate
    ldi     GlobalTemp1, low(BaudPrescaler)              
    out     UBRRL, GlobalTemp1
    ldi     GlobalTemp1, high(BaudPrescaler)
    out     UBRRH, GlobalTemp1
 
    ;Frame-Format: 8 Bit, 2stop bit
    ;UCSRC = %0, UMSEL, UPM1, UPM0, USBS, UCSZ1, UCSZ0, UCPOL
    ;UMSEL   %0  = asynchronous mode
    ;UPM1,0  %00 = parity disabled
    ;USBS    %1  = 2 stop bits
    ;UCSZ1,0 %11 = 8 data bits
    ;UCPOL   %x  = don't care for asynchronous mode 
    ldi     GlobalTemp1, (1<<USBS)|(3<<UCSZ0)
    out     UCSRC,GlobalTemp1
 
    ;UCSRB = RXCIE, TXCIE, UDRIE, RXEN, TXEN, UCSZ2, RXB8, TXB8
    ;RXCIE %1 = interrupt on receive complete enabled
    ;TXCIE %0 = interrupt on transmit complete disabled
    ;UDRIE %1 = interrupt on transmit buffer empty enabled
    ;RXEN  %1 = Rx enabled
    ;TXEN  %1 = Tx enabled
    ;
    ldi     GlobalTemp1, (0<<RXCIE)|(1<<UDRIE)|(0<<RXEN)|(1<<TXEN)
    out     UCSRB, GlobalTemp1

;--
;-- Initialize Timer0
;--
    ;TCCRxA = COMxA1, COMxA0, COMxB0, COMxB1, %0, %0, WGMx1, WGMx0
    ;COM0A1,0 %00 = Normal port operation, OC0A is disconnected
    ;COM0B1,0 %00 = Normal port operation, OC0B is disconnected
    ;WGM01,0  %00 = Normal Timer/counter mode of operation
    ldi     GlobalTemp1, 0b00000000
    out     TCCR0A, GlobalTemp1
    ;TCCR0B = FOC0A, FOC0B, %0, %0, WGM02, CS02, CS01, CS00
    ;FOC0A,B  %00  = Force Output Compare strobe
    ;WGM02    %0   = TOP = $FF
    ;CS02,1,0 %000 = no I/O clock source; timer/counter stopped
    ;CS02,1,0 %001 = I/O clock source without prescaling
    ;CS02,1,0 %010 = I/O clock source div 8
    ;CS02,1,0 %011 = I/O clock source div 64
    ;CS02,1,0 %100 = I/O clock source div 256
    ;CS02,1,0 %101 = I/O clock source div 1024
    ldi     GlobalTemp1, 0b00000000
    out     TCCR0B, GlobalTemp1


;--
;-- Initialize Timer1 for pulsewidth measurement
;--

    ;COM1A1,0 %00 = Norma port operation, OC1A is disconnected
    ;COM1B1,0 %00 = Norma port operation, OC1B is disconnected
    ;WGM11,0  %00 = normal timer/counter operation
    ldi     GlobalTemp1, 0b00000000
    out     TCCR1A, GlobalTemp1

    ;TCCR1B = ICNC1, ICES1, %0, WGM13, WGM12, CS12, CS11, CS10
    ;ICNC1    %0   = Input Capture Noise Canceler
    ;ICES1    %0   = Input Capture Edge Select
    ;WGM13,2  %00  = normal timer/counter operation
    ;CS12,1,0 %000 = no I/O clock source; timer/counter stopped
    ;CS12,1,0 %001 = I/O clock source without prescaling
    ;CS12,1,0 %010 = I/O clock source div 8
    ;CS12,1,0 %011 = I/O clock source div 64
    ;CS12,1,0 %100 = I/O clock source div 256
    ;CS12,1,0 %101 = I/O clock source div 1024
    ldi     GlobalTemp1, 0b00000001
    out     TCCR1B, GlobalTemp1

;--
;-- Enable CPU sleep mode & INT0,1 positive edge triggered interrupt
;--
    ldi     GlobalTemp1,(1<<SE)|(1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00)
    out     MCUCR, GlobalTemp1

;--
;-- Initialize double buffering
;--
    ldi     XH, high(S_TxPipeFrame1)
    ldi     XL, low (S_TxPipeFrame1)
    adiw    XH:XL, F_DataStartIndex
    clr     S_TxFrameCount
    TRUE    FillingFrame1

;------------------------------------------------------------------------------
;-- main                                                                     --
;------------------------------------------------------------------------------
main:
    sei
    sleep
    cli
    ;Do we have a full frame to send?
    cpi     S_TxFrameCount, Accl2ParamCount
    brne    main
     
    IFTRUE  FillingFrame1, Main_TxFrame1
    ;Set transmit buffer to most recently filled frame
    ldi     YH, high(S_TxPipeFrame2)
    ldi     YL, low (S_TxPipeFrame2)
    ;and set fill buffer to last sent frame
    ldi     XH, high(S_TxPipeFrame1)
    ldi     XL, low (S_TxPipeFrame1)
    adiw    XH:XL, F_DataStartIndex
    TRUE    FillingFrame1
    rjmp    Main_Tx

Main_TxFrame1:
    ;Set transmit buffer to most recently filled frame
    ldi     YH, high(S_TxPipeFrame1)
    ldi     YL, low (S_TxPipeFrame1)
    ldi     XH, high(S_TxPipeFrame2)
    ldi     XL, low (S_TxPipeFrame2)
    adiw    XH:XL, F_DataStartIndex
    FALSE   FillingFrame1

    ;transmitting most recently filled frame
Main_Tx:
    clr     S_TxFrameCount
    clr     S_TxPacketPtr
    sbi     PORTB, PB5		;set PB5 high during Tx of frame
    sbi     UCSRB, UDRIE	;Enable interupt driven Tx
    rjmp    main
