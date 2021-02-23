# Attiny13 200psi pressure gauge 1602 Hitachi LCD with I2C backpack bitbang

Uploaded by darth vader on 2021-02-23 11:58:00 (rating 5 out of 5)

## Summary

; Program for attiny13A PRESSURE GUAGE with Hitachi 1602LCD and I2C back pack  

; uses pressure sensor from 0-200 psi range  

; PB0: SDA, PB1: SCL, PB2:A/D input or pressure sensor connection


;connect Attiny13 PB0 to LCD SDA, Attiny13 PB1 to SCL . Both PB1 & PB0 should be connected to positive via 5K resistor individually as required by I2C bus . Pressure sensor signal connected ;to PB1. Compile with Atmel studio7 and burn HEX with USB ASP clone. AVRDUDESS is a good GUI based program ,  

;thanks for danni from AVR freaks for the binary to ASCII routine   

;Thanks to Kodera2t for I2C routine   

;thanks to Donald Weiman    ([weimandn@alfredstate.edu](mailto:weimandn@alfredstate.edu)) for delay routines  

;  

.equ    fclk    = 1000000        ; system clock frequency (for delays)   

.DEF ANSL = R0                    ;To hold low-byte of answer       

.DEF ANSH = R1                    ;To hold high-byte of answer  

.DEF    A = R12                   ;To hold multiplicand  

.DEF    B = R11                   ;To hold multiplier  

.DEF    C = R10                   ;To hold bit counter  

.def    D = R9  

.DEF nibble1 = R25              ;register to store data nibble  

.DEF command1 = R22  

.DEF command2 = R23  

.DEF command3 = R24  

.DEF command4 = R15  

.DEF command5 = R14  

.DEF command6 = R13  

.DEF loader = R26  

.def temp = R19  

.def data = R16  

.def cnt = R17  

.def cnt2 = R18  

.def display = R25  

.def temp1 = R20  

.def temp2 = R21  

.def free1 = R9  

.def free2 = R8  

.equ count\_start = 0x04  

.equ count\_end = 0x01  

.equ subnum = 0x01  

.equ    SUB\_COUNT    = 0x04  

.equ long\_delay =0xFF  

.equ data\_command1 = 0b00001001        ; data control nibble ,led on P3, EN 0 on P2, R/W 0 (write) in P1 , RS 1 (0 instruction, 1 data) = 1001  =0x09  

.equ data\_command2 = 0b00001101        ; data control nibble , 1101  = 0x0D   - EN goes hi=1  

.equ data\_command3 = 0b11111011        ; data control nibble , 1001  = 0x09   - EN goes low=0  

.equ inst\_command1 = 0b00001000        ;instruction control nibble ,  led on en-lo,Rw-0,rs =0   = 1000   = 0x08  

.equ inst\_command2 = 0b00001100        ;instruction control nibble ,  led on,EN hi , rs/RW 0    = 1100   = 0x0C  

.equ inst\_command3 = 0b11111011        ;instruction control nibble  , led on, EN lo ,rs/rw 0    = 1000   = 0x08  

.equ slave\_address = 0b01001110        ;0x4E 


; PB0: SDA, PB1: SCL, PB2:A/D input


setup:  

; Init stack for subroutines  

    ldi display,LOW(RAMEND)         ; Init stack  

    out SPL,display                 ; to stack pointer


    ldi command1,inst\_command1        ;lower nibble is I2c command  led on en,Rw,rs =0  

    ldi command2,inst\_command2        ;lower nibble led on,EN hi , rs/RW 0  

    ldi command3,inst\_command3        ;loer nibble  led on, EN lo ,rs/rw 0  

    ldi loader,data\_command1  

    mov command4,loader  

    ldi loader,data\_command2  

    mov command5,loader  

    ldi loader,data\_command3  

    mov command6,loader  

; delay timer for upto 65 seconds (for main loop operations)----- use command "rcall delayYx1mS" for delay as per enetred value in the brakets in millisecs.


    ;ldi YL, low(1000)            ;enter desired milliseconds(ms) for the delay needed in the (.......),used for delay in the range of seconds  

    ;ldi YH, high(1000)            ;enter desired milliseconds(ms) for the delay needed in the (.......),used for delay in the range of seconds


; PB0 (SDA) and PB1 (SCL) are tristated for hi and DDR is changed to output for low on I2C bus


    ldi temp1, 0b00  

    out DDRB, temp1                ;pb0 & pb1 is now input   

    out portb,temp1                ;pb0 & pb1 is now tristated , both i2c lines are pulled up by ext resitors to hi


; initially, SDA and SCL are high  

      

    rcall lcd\_init                  ; LCD initialize as per hitachi data sheet HD44780


; starting A/D converter, free running mode  

    ldi    temp1, ((1<<ADEN)+(1<<ADSC)+(1<<ADATE)+(1<<ADIE)+(0<<ADIF)+(1<<ADPS2)+(0<<ADPS1)+(1<<ADPS2))  

    out   ADCSRA,temp1  

    ldi temp1, 0x00  

    out ADCSRB, temp1  

; selecting PB2 for A/D input  

    ldi    temp1, (1<<MUX1)+(0<<MUX0)+(1<<ADLAR)       ; adc2,PB4  , adc result left adjusted  

    out   ADMUX,temp1  

    ldi    temp1, 1<<ADC2D  

    out    DIDR0, temp1  

    ldi temp ,5  

    rcall delayTx1uS  

     


; reading A/D result in free1  

    in free1,ADCL  

    in free2,ADCH  

    mov loader, free2


    push data  

    push cnt  

    push cnt2  

    push temp  

    push temp1  

    


;3. MANUALLY MULTIPLYING TWO SINGLE-BYTE NUMBERS  

;To do standard multiplication we examine how binary multiplication is achieved, we notice that when a digit in the multiplier is a one we add a shifted version of the multiplicand to our result. When the multiplier digit is a zero we need to add zero, which means we do nothing.  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

   

;The routine below mimics the hardware multiply (MUL) by leaving the multiplicand and multiplier untouched, and the result appears in the register pair R1 and R0. It shifts the bits of the multiplier into the carry bit and uses the contents of the carry to add the multiplicand if it is a one or skip it if the carry is a zero. The routine takes about sixty cycles.


;.DEF ANSL = r16  data          ;To hold low-byte of answer       

;.DEF ANSH = R17  cnt          ;To hold high-byte of answer  

;.DEF    A = R18  cnt2         ;To hold multiplicand  

;.DEF    B = R19  temp         ;To hold multiplier  

;.DEF    C = R20  temp1         ;To hold bit counter


        MOV cnt2,loader      ;Load multiplicand into A  

        LDI temp,79  

        ;MOV temp,loader      ;Load multiplier into B ((5/255)* (200psi/5V) = 0.788 psi / ADC count)  

MUL8x8:  

        LDI temp1,8  

        ;MOV temp1,loader           ;Load bit counter into C  

        CLR cnt          ;Clear high-byte of answer  

        MOV data,temp        ;Copy multiplier into low-byte of answer  

        LSR data          ;Shift low-bit of multiplier into Carry  

LOOP:    BRCC SKIP        ;If carry is zero then skip addition   

        ADD cnt,cnt2        ;Add multiplicand to answer  

SKIP:   ROR cnt          ;Shift low-bit of high-byte   

        ROR data          ;of answer into low-byte  

        DEC temp1             ;Decrement bit-counter  

         BRNE LOOP        ;Check if done all eight bits  

                          ;Result  left in ANSL and ANSH


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;.def    a0              = r0 = ANSL  = 16 ,data  

;.def    a1              = r1 = ANSH  =17,cnt  

;.def    a2              = r12 = A    =18 , cnt2  

;.def    a3              = r11 = B    =19 ,temp  

;.def    a4              = r10 = C    =20, temp1  

;************************************************************************  

;*                                                                      *  

;*                      binary to ASCII decimal conversion              *  

;*                                                                      *  

;************************************************************************  

;input: a1, a0 = 16 bit value 0 ... 65535  

;output: a4, a3, a2, a1, a0 = digits  

;cycle: 27 .. 183  

;bytes: 42  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


binbcd:  

        ldi temp1,-1 + '0'  

        ;mov     temp1, loader       ;0x2F  = -1 decimal + ascii 0 (decimal 48 or 0x30)  decimal 47 or 0x2F  

\_bib1:  inc     temp1                    ; times operation done  

        subi    data, low(10000)  

        sbci    cnt, high(10000)    ; if carry flag set  

        brcc    \_bib1                ; branch if carry flag clear till-ve operation (original value less than 10000)


    ldi temp,10 + '0'  

        ;mov    B, loader        ;0x3A  , 10 decimal + ascii 0=46 in hex a+30 =3A  = decimal 58  

\_bib2:  dec     temp                    ; decrease one count from 58 to reach 48  

        subi    data, low(-1000)        ; subtract with -1000 decimal (adds 1000)  

        sbci    cnt, high(-1000)        ; adds 1000 till carry is set  

        brcs    \_bib2                ; branch if carry set


    ldi cnt2, -1 + '0'  

        ;mov     A, loader        ;0x2F  = -1 decimal + ascii 0 (decimal 48 or 0x30)  decimal 47 or 0x2F  

\_bib3:  inc     cnt2                    ;times operation done  

        subi    data, low(100)        ;  

        sbci    cnt, high(100)        ;subtract 100 till carry flag is set indicating negative number  

        brcc    \_bib3                ;branch if carry flag clear  

      

    ldi cnt, 10 + '0'  

        ;mov     ANSH, loader        ;0x3A  , 10 decimal + ascii 0=46 in hex a+30 =3A  = decimal 58  

\_bib4:  dec     cnt  

        subi    data, -10  

        brcs    \_bib4


        subi    data, -'0'  

 ;       ret  

 mov r0,data ; unit  

 mov r1,cnt  ; ten  

 mov r2,cnt2 ; hundred  

 mov r3,temp ; thousand  

 mov r4,temp1; tenthousand  

       

 pop temp1  

 pop temp  

 pop cnt2  

 pop cnt  

 pop data


    ldi loader,0b11000000            ;0b1100\_000 ; (0xC0) second line,first digit  ,DDRAM address as per data sheet is 0x47 (1000000)+ 10000000 is the DDRAM control code = 0b11000000  

    mov nibble1,loader                     ;loads info in loader to nibble register for AND,OR,SWAP etc to write data  

    rcall nibble\_write\_instruction        ;calls LCD instruction/command write subroutine


    mov loader,r4                ;10000 position ASCII value  

    mov nibble1,loader  

    rcall nibble\_write\_data                ;calls LCD data write subroutine


    


    mov loader,r3                ;1000 position ASCII value  

    mov nibble1,loader  

    rcall nibble\_write\_data


    mov loader,r2                ;100 position ASCII value  

    mov nibble1,loader  

    rcall nibble\_write\_data


      

    ldi loader,0b00111110            ;decimal symbol  

    mov nibble1,loader  

    rcall nibble\_write\_data


    mov loader,r1                ;10 position ASCII value  

    mov nibble1,loader  

    rcall nibble\_write\_data


    mov loader,r0                ;1 position ASCII value  

    mov nibble1,loader  

    rcall nibble\_write\_data


    ldi loader,0b11001000            ;load DDRAM address C8 (8th position)  

    mov nibble1,loader  

    rcall nibble\_write\_instruction


    ldi loader, 0b01010000            ;ASCII for P  

    mov nibble1,loader  

    rcall nibble\_write\_data


    ldi loader,0b01010011            ;ASCII for S  

    mov nibble1,loader  

    rcall nibble\_write\_data


    ldi loader,0b01001001            ;ASCII for I  

    mov nibble1,loader  

    rcall nibble\_write\_data


start:  

    ldi temp1, 0b01                   ;scl 1 , sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b11                ;scl 0 , sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ret  

    ;cl, da  

          

ends:  

    ldi temp1,0b11                ;both scl & sda pulled low so as next step ensures a low to hi transition of SDA for stop  

    out ddrb, temp1                ; scl 0 , sda 0  

    rcall delay10uS  

    ldi temp1,0b01                ;scl 1 ,sda 0  

    out ddrb, temp1                ;while scl is hi ,sda is low , next step sda will go hi creating a stop condition  

    rcall delay10uS  

    ldi temp1,0b00                ;scl 1 . sda 1  

    out ddrb, temp1                ;sda reached hi from low in the previous step  

    rcall delay10uS      

    ret


init:  

    ldi temp1, 0b00             ;   

    out ddrb,temp1                ; data direction set as input = SCL & SDA now hi  

    out PORTB, temp1            ; ports also loaded with 0 , Zstate  

    rcall delay10uS  

    ret  

    ;cl da  

bit\_high:  

    ldi temp1, 0b11                ; scl 0 ,sda 0  

    out ddrb, temp1                ; writing 1 to ddr will make it output and alreday port is 0 so SDA & SCL is pulled low  

    rcall delay10uS  

    ldi temp1, 0b10                ; scl 0 ,sda 1  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b00                ;scl 1 , sda 1  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b10                ; scl 0 , sda 1  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b11                ; scl 0 ,sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ret  

    ;cl=1 da=0  

      

bit\_low:  

    ldi temp1, 0b11                ;scl 0 ,sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b11                ;scl 0 , sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b01                ; scl 1 , sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b11                ; scl 0 , sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b11                ; scl 0 , sda 0  

    out ddrb, temp1  

    rcall delay10uS  

    ret  

    ;cl da


ack:  

    ldi temp1, 0b10                ; scl 0 , sda 1  

    out ddrb,temp1  

;recheck:      

    ;sbic pinb,pinb0            ; if pinb0 is 0 then slave ACKed  

    ;rjmp recheck                ; if pinb0 is 1 wait for ACK  

    rcall delay10uS  

    ldi temp1, 0b00                ; scl 1, sda 1 ( if ACK sda will become 0 when slave pulls low)  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b10  

    out ddrb, temp1  

    rcall delay10uS  

    ldi temp1, 0b11                ; scl 0 , sda 0   

    out ddrb, temp1  

    rcall delay10uS  

    ret  

      

    


; writing data is stored in data  

writedata:  

    ldi cnt,0x00  

    ldi cnt2,0x02  

rep:  

    mov temp2, data  

    andi temp2,0b10000000  

    cpi temp2, 0b10000000  

    breq highbit  

    rcall bit\_low  

    rjmp sendend  

highbit:  

    rcall bit\_high  

sendend:  

    lsl data  

    inc cnt  

    cpi cnt,8  

    brne rep  

    ldi temp1, 0b10                ; scl 0 , sda 1 ( sda released for slave to send ACK)  

    out ddrb, temp1  

    ret


nibble\_write\_instruction:  

    rcall init  

    rcall start  

    ldi data,slave\_address  

    rcall writedata  

    rcall ack  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command1  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader                 ;copies data for upper nibble operation  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command2  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader                 ;copies data for upper nibble operation  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    AND nibble1,command3  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader  

    swap nibble1                    ;previous lower nibble is swapped to high nibble position  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command1  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader                ;copies data for upper nibble operation  

    swap nibble1                    ;previous lower nibble is swapped to high nibble position  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command2  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader  

    swap nibble1                    ;previous lower nibble is swapped to high nibble position  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    AND nibble1,command3  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    rcall ends  

    ldi temp,20  

     rcall delayTx1mS  

    ret


nibble\_write\_data:  

    rcall init  

    rcall start  

    ldi data,slave\_address  

    rcall writedata  

    rcall ack  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command4                ;adding the upper nibble and LCD commands as lower bits of the 8 bits in write data  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader                 ;copies data for upper nibble operation  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command5  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader                 ;copies data for upper nibble operation  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    AND nibble1,command6  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader  

    swap nibble1                    ;previous lower nibble is swapped to high nibble position  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command4  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader                ;copies data for upper nibble operation  

    swap nibble1                    ;previous lower nibble is swapped to high nibble position  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    OR nibble1,command5  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    mov nibble1,loader  

    swap nibble1                    ;previous lower nibble is swapped to high nibble position  

    ANDi nibble1,0XF0               ;upper nibble is preserved in register nibble1  

    AND nibble1,command6  

    mov data,nibble1  

    rcall writedata  

    rcall ack  

    rcall ends  

    ldi temp,5  

    rcall delayTx1uS  

    ret


    


lcd\_init:  

    ldi temp,50  

    rcall delayTx1mS                  ;50ms delay as part of startup  

      

    rcall init  

    rcall start  

    ldi data,slave\_address                 ;0x27 and write bit =0x4E  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only during initialization, any 4 bit LCD is done by instruction\_write or data\_write subroutine after initialisation  

    rcall ack  

    ldi data,0b00111100                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only during initialization, any 4 bit LCD is done by instruction\_write or data\_write subroutine after initialisation  

    rcall ack  

    ldi data,0b00111000                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only during initialization, any 4 bit LCD is done by instruction\_write or data\_write subroutine after initialisation  

    rcall ack  

    rcall ends  

    ldi temp,20                            ; value loaded here (20) decides the number of milli seconds in the delay below  

     rcall delayTx1mS                    ;20 milli seconds delay, slightly above hitachi data sheet 


      

    rcall init  

    rcall start  

    ldi data,slave\_address                 ;0x27 and write bit =0x4E  

    rcall writedata  

    rcall ack  

    ldi data,0b00111100                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata  

    rcall ack  

    ldi data,0b00111000                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata  

    rcall ack  

    rcall ends  

    ldi temp,20  

     rcall delayTx1mS                    ;20 milli seconds delay, slightly above hitachi data sheet  

      

      

      

    rcall init  

    rcall start  

    ldi data,slave\_address                 ;0x27 and write bit =0x4E  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only during initialization, any 4 bit LCD is done by instruction\_write or data\_write subroutine after initialisation  

    rcall ack  

    ldi data,0b00111100                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata  

    rcall ack  

    ldi data,0b00111000                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata  

    rcall ack  

    rcall ends  

    ldi temp,20  

     rcall delayTx1mS                    ;20 milli seconds delay, slightly above hitachi data sheet


    rcall init  

    rcall start  

    ldi data,slave\_address                 ;0x27 and write bit =0x4E  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only during initialization , any 4 bit LCD is done by instruction\_write or data\_write subroutine  

    rcall ack  

    ldi data,0b00101100                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only during initialization , any 4 bit LCD is done by instruction\_write or data\_write subroutine  

    rcall ack  

    ldi data,0b00101000                 ;Function set LCD , high nibble 0011 & lower commands led = 1,EN,RW,RS all 0.  

    rcall writedata                        ;writedata subroutine is called to command PCF8574T i2c chip only , any 4 bit LCD is done by instruction\_write or data\_write subroutine  

    rcall ack  

    rcall ends  

    ldi temp,20  

     rcall delayTx1mS                    ;20 milli seconds delay, slightly above hitachi data sheet


      

   

    ldi loader,0b00101000                ; (0x28 4 bit ,2 line ,5x7)Function set LCD ,  

    mov nibble1,loader                    ;loads info in loader to nibble register for AND,OR,SWAP etc to write data  

    rcall nibble\_write\_instruction         ;calls LCD instruction/command write subroutine  

              

      

    ldi loader,0b00001110                 ;Function set LCD , (0x0C ). DISPLAY ON  

    mov nibble1,loader                    ;loads info in loader to nibble register for AND,OR,SWAP etc to write data  

    rcall nibble\_write\_instruction        ;calls LCD instruction/command write subroutine


      

    ldi loader,0b00000110                ;Function set LCD  (entry mode set 0x06),  ENTRY MODE SET  

    mov nibble1,loader                    ;loads info in loader to nibble register for AND,OR,SWAP etc to write data  

    rcall nibble\_write\_instruction        ;calls LCD instruction/command write subroutine  

      

      

          

    ldi loader,0b00000001                 ;Function set LCD  (clear display 0x01),  CLEAR DISPLAY  

    mov nibble1,loader                    ;loads info in loader to nibble register for AND,OR,SWAP etc to write data  

    rcall nibble\_write\_instruction        ;calls LCD instruction/command write subroutine  

    


    ret


; ============================== Time Delay Subroutines =====================  

; Name:     delayYx1mS  

; Purpose:  provide a delay of (YH:YL) x 1 mS  

; Entry:    (YH:YL) = delay data  

; Exit:     no parameters  

; Notes:    the 16-bit register provides for a delay of up to 65.535 Seconds  

;           requires delay1mS


delayYx1mS:  

    rcall    delay1mS                        ; delay for 1 mS  

    sbiw    YH:YL, 1                        ; update the the delay counter  

    brne    delayYx1mS                      ; counter is not zero


; arrive here when delay counter is zero (total delay period is finished)  

    ret  

; ---------------------------------------------------------------------------  

; Name:     delayTx1mS  

; Purpose:  provide a delay of (temp) x 1 mS  

; Entry:    (temp) = delay data  

; Exit:     no parameters  

; Notes:    the 8-bit register provides for a delay of up to 255 mS  

;           requires delay1mS


delayTx1mS:  

    rcall    delay1mS                        ; delay for 1 mS  

    dec     temp                            ; update the delay counter  

    brne    delayTx1mS                      ; counter is not zero


; arrive here when delay counter is zero (total delay period is finished)  

    ret


; ---------------------------------------------------------------------------  

; Name:     delay1mS  

; Purpose:  provide a delay of 1 mS  

; Entry:    no parameters  

; Exit:     no parameters  

; Notes:    chews up fclk/1000 clock cycles (including the 'call')


delay1mS:  

    push    YL                              ; [2] preserve registers  

    push    YH                              ; [2]  

    ldi     YL, low(((fclk/1000)-18)/4)     ; [1] delay counter              (((fclk/1000)-18)/4)  

    ldi     YH, high(((fclk/1000)-18)/4)    ; [1]                            (((fclk/1000)-18)/4)


delay1mS\_01:  

    sbiw    YH:YL, 1                        ; [2] update the the delay counter  

    brne    delay1mS\_01                     ; [2] delay counter is not zero


; arrive here when delay counter is zero  

    pop     YH                              ; [2] restore registers  

    pop     YL                              ; [2]  

    ret                                     ; [4]


; ---------------------------------------------------------------------------  

; Name:     delayTx1uS  

; Purpose:  provide a delay of (temp) x 1 uS with a 16 MHz clock frequency  

; Entry:    (temp) = delay data  

; Exit:     no parameters  

; Notes:    the 8-bit register provides for a delay of up to 255 uS  

;           requires delay1uS


delayTx1uS:  

    rcall    delay10uS                        ; delay for 1 uS  

    dec     temp                            ; decrement the delay counter  

    brne    delayTx1uS                      ; counter is not zero


; arrive here when delay counter is zero (total delay period is finished)  

    ret


; ---------------------------------------------------------------------------  

; Name:     delay10uS  

; Purpose:  provide a delay of 1 uS with a 16 MHz clock frequency ;MODIFIED TO PROVIDE 10us with 1200000cs chip by Sajeev  

; Entry:    no parameters  

; Exit:     no parameters  

; Notes:    add another push/pop for 20 MHz clock frequency


delay10uS:  

    ;push    temp                            ; [2] these instructions do nothing except consume clock cycles  

    ;pop     temp                            ; [2]  

    ;push    temp                            ; [2]  

    ;pop     temp                            ; [2]  

    ;ret                                     ; [4]  

     nop  

     nop  

     nop  

     ret


; ============================== End of Time Delay Subroutines ==============

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- Attiny13
- 200psi pressure gauge
- 1602 Hitachi LCD
- with I2C backpack
- bit bang I2C master.
