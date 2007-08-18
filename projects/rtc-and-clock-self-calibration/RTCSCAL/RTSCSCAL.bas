    '(
    Copyright (C) 2007  Teodor Lazar

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This program running on an ATMEGA8 exhibits:
        1. a fast reliable self-calibration procedure of the internal RC
            oscillator when using a 32768Hz crystal with any supply voltage
        2. a routine written in assembler for determining the MCU speed
            in the above conditions (_Get_MCU_Speed)
        2. a way to achieve a high serial baud rate of 115200bps when the MCU
            is clocked at very low speeds for preserving battery power
        3. a RTC interrupt routine written in assembler which doesn't affect
            any register and takes only 67 words from the flash memory.
            The RTC keeps the right time and date from 2001 to 2099(_isr_T2OVF).

    For questions feel free to ask me at teodor.lazar@gmail.com
    ')

    $REGFILE = "m8def.dat"
    $CRYSTAL = 921600
    '$NORAMCLEAR
    $BAUD = 57600
    $FRAMESIZE = 32
    $HWSTACK = 64
    $SWSTACK = 32
    '$LIB "mcs.lib"
    '$INITMICRO
    'goto _init_Start
'_init_micro:
'        ret
'_init_Start:


    const _xtal1024 = _xtal \ 1024      'use _xtal1024 constant instead _xtal
                                        'to fit MCU speed in one word

    config portd.0 = input              'config RX pin as input
    config portd.1 = output             'config TX pin as output
    portd.0 = 1                          'enable pullup on Rx
    set ucsra.u2x                        'double the baud rate to 115200bps

    dim glng as long , glng2 as long    'dim some general purpose vars.

    dim wMCU_Speed as word              'this stores the real current MCU speed

    'RTC settings
    config clock = user
    _sec = 00 : _min = 00 : _hour = 00 : _day = 01 : _month = 01 : _year = 07
    dim _tick128 as byte               'this new RTC var. is needed
                                        '  it gets incremented every 1/128 sec.
    dim flagRTC_Changed as bit         'optional variable
    'dim bTimeoutCounter as byte        'optional variable
                                        '   their meaning is explained later

    config timer2 = timer , async = on , prescale = 1
    on OVF2 _isr_T2OVF nosave
    enable interrupts
    enable OVF2

    rcall _Get_MCU_Speed                'get the initial MCU speed (div by 1024)
    glng2 = wMCU_Speed                  'store it
    while wMCU_Speed > _xtal1024        'while MCU speed > desired speed
        decr osccal                     '   decrement osccal
        rcall _Get_MCU_Speed            '   and get the new MCU speed
    wend
    while wMCU_Speed < _xtal1024        'while MCU speed < desired speed
        incr osccal                     '   increment osccal
        rcall _Get_MCU_Speed            '   and get the new MCU speed
    wend
    'the above sequence will always leave the MCU speed slightly higher than
    '   the desired speed, but no more than 0.3-0.5%


    glng = glng2 * 1024                 'now get the stored initial speed
    print "Initial Speed = " ; glng     'and print it
    glng = wMCU_Speed * 1024            'and get the current speed
    print "Actual Speed = " ; glng      'and print it for comparison


    'now the main code sends every second the current time and date to serial
    flagRTC_Changed = 0                 'reset the RTC_Changed flag
    do
        idle                            'go in power save mode until MCU is
                                        '   waken up by an interrupt
        if flagRTC_Changed = 1 then     'if flag is set than a second passed
                                        '   and print the current time and date
            print   _year ; "/" ; _month ; "/" ; _day ; " - " ; _
                    _hour ; ":" ; _min ; ":" ; _sec
            flagRTC_Changed = 0         'reset the flag
        endif
    loop                                'and wish you happy looping
end

'-------------------------------------------------------------------------------
_Get_MCU_Speed:
        cli                             'disable all interrupts
_GMS_L1:
            in      r23,tifr
            sbrs    r23,tov2            'wait until timer2 overflows
        rjmp    _GMS_L1

        ldi     r23,2^tov2              'reset timer2 overflow flag
        !out    tifr,r23

        ldi     r24,1                   'initialise a 16bit counter with 1
        ldi     r25,0

_GMS_L2:
            adiw    r24,1               'increment the 16bit counter
            adiw    r24,0               'add 2 more cycles
                                        '   to make a 8 cycles loop
            in      r23,tifr
            sbrs    r23,tov2            'exit this loop when timer2 overflows
        rjmp    _GMS_L2
'
'the counter will now hold the MCU speed divided by 1024
' (the counts taken in 1 / 128 seconds , 8 cycles for each count)
'
        sts     {wMCU_Speed+0},r24      'and store the 16bit counter
        sts     {wMCU_Speed+1},r25      '

        rcall   _isr_T2OVF              'call the RTC routine
                                        '   to compensate one missed tick
        sei                             'enable interrupts and trigger the OVF2
                                        '   interrupt because TOV2 flag is set
                                        '   to compensate the 2nd missed tick
                                        'this way the RTC won't lag and this
                                        '   routine can be used safely even
    ret                                 '   when RTC is running


'-------------------------------------------------------------------------------
_isr_T2OVF:
        push    r16
        in      r16,sreg
        push    r16                     'save R16 and SREG in stack

'insert this block if bTimeoutConter is defined (as byte)
'this counter may be used to timeout operations
'   that usually takes less than 2 seconds (with a 7.8ms resolution)
'the counter is started when the main code initialises it with a value
'   greater than zero, representing a multiple of 7.8ms
'when the main code reads zero from this counter, then
'   the timeout condition has occured
#if varexist( "bTimeoutCounter")
        lds     r16,{bTimeoutCounter}   'decrement bTimeoutCounter every 7.8ms
        cpi     r16,0                   '   until it reaches zero
        breq    _isr_T2OVF_BTC
        dec     r16
        sts     {bTimeoutCounter},r16
_isr_T2OVF_BTC:
#endif

        lds     r16,{_tick128}          'increment _tick128 every 1/128s
        inc     r16
        sts     {_tick128},r16
        andi    r16,&H7F                'if _tick128 is not an 128 multiple
        brne    _T2OVF_END              '   then exit isr
                                        '   else a second has passed
                                        '       therefore we go updating RTC

'insert this block if flagRTC_Changed is defined (as bit)
'this flag will be set up every time a change occurs in RTC (every second)
'it's the main code duty to reset the flag after noticing it
#if varexist( "flagRTC_Changed")
        lds     r16,{flagRTC_Changed}
        sbr     r16,2^bit.flagRTC_Changed
        sts     {flagRTC_Changed},r16
#endif

        push    r17                     'now save all used registers
        push    r18                     '   since we have to do all RTC stuff
        push    XL
        push    XH

        'increment RTC
        loadadr _sec , X               'load XH:XL with the address of _sec
        ldi     r17,3                   'R17 holds a loop counter for
                                        '   processing _sec, _min and _hour
        ldi     r18,60                  'R18 holds the maximum value allowed
                                        '   for the first RTC component (_sec)
_T2OVF_SEC_MIN:
            ld      r16,X               'read component value
            inc     r16                 'increment component value
            st      X,r16               'store component value
            sbrs    r17,1               'if R17 is 1, then we now check hours
            ldi     r18,24              '   and R18 has to hold 24
            cp      r16,r18             'compare the current RTC component with
                                        '   the maximum allowed value (60 or 24)
            brcs    _T2OVF_END1         'if maximum not reached then exit isr
            clr     r16                 'if maximum reached, clear component
            st      X+,r16              'and go to the next one
            dec     r17                 'decrement the loop counter
        brne    _T2OVF_SEC_MIN          'loop if counter is not zero
        'now process _day, _mounth and _year
        ld      r16,X+                  'R16 holds _day
        ld      r17,X+                  'R17 holds _month
        ld      r18,X                   'R18 holds _year

        andi    r18,&H03                'check if _year is not leap
        ldi     r18,28+1                'R18 now holds the maximum(+1) allowed
                                        '   days of the month(we presume it Feb)
                                        '   that leaves the zero flag unaffected
        brne    _T2OVF_YEAR_NOT_LEAP    'if year is not leap then skip next
        ldi     r18,29+1                '   else, R18 holds 29 days(+1)
_T2OVF_YEAR_NOT_LEAP:
        cpi     r17,2                   'now check if it's really Feb.
        breq    _T2OVF_SHORT_MONTH      'if true, then we got the right
                                        '   number of max.days(+1) and jmp over
                                        '   else, keep checking for other month
        ldi     r18,30+1                'now presume we have a 30 days(+1) month
        cpi     r17,4                   'check for Apr.
        breq    _T2OVF_SHORT_MONTH
        cpi     r17,6                   'check for Jun.
        breq    _T2OVF_SHORT_MONTH
        cpi     r17,9                   'check for Sep.
        breq    _T2OVF_SHORT_MONTH
        cpi     r17,11                  'check for Nov.
        breq    _T2OVF_SHORT_MONTH
        ldi     r18,31+1                'if not any from the above
                                        '   then we have a 31 days(+1) month
_T2OVF_SHORT_MONTH:                     'now we have the maximum days of the
                                        '   current month in R18 (+1)
        inc     r16                     'increment current day
        cp      r16,r18                 'check it against maximum days allowed
        ld      r18,X                   'restore the year in R18
                                        '   carry flag is unaffected by that
        brcs    _T2OVF_STORE_DMY        'if day still less than maximum (+1)
                                        '   then jmp over
                                        '   (month and year stay the same)
        ldi     r16,1                   'else the day become the first
        inc     r17                     'in the next month
        cpi     r17,13                  'check if it's not the last month
        brcs    _T2OVF_STORE_DMY        'if it's not the last one, jmp over
        ldi     r17,1                   'else the month become the first

        inc     r18                     'in the next year

_T2OVF_STORE_DMY:
        st      X,r18                   'now store the updated (or not)
        st      -X,r17                  '   year, month and day
        st      -X,r16

_T2OVF_END1:
        pop     XH                      'restore registers
        pop     XL
        pop     r18
        pop     r17
_T2OVF_END:
        pop     r16
        !out    sreg,r16
        pop     r16
    return                              'and exit isr
'-------------------------------------------------------------------------------
'the following void routines are needed if you want to use
'   the time/date bascom functions. You may fill them as you need
settime:
    return
setdate:
    return
