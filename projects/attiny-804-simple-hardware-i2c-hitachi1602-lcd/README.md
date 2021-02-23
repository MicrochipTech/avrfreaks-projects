# ATTINY 804 Simple hardware I2C for Hitachi1602 LCD

Uploaded by darth vader on 2021-02-23 06:18:00 (rating 5 out of 5)

## Summary

;tested & found working


;  

; 804Hitachi.asm  

; ATTINY 804  

; LCD = hitachiHD44780chinese clone with PCF8574T-I2C backpack  

; Atmel studio 7  

; This LCD module PCF8574T with backpack does not ack. I had to comment out the ACK procedure to get this work other wise garbage is printed on the LCD  

; POLARISU UPDIPROG was used with CH340FTDI module to program the ATTINY804 from window7 PC (thanks to the programmer) github has the details & commands, easy to use.  

; disconnect all connections from the MCU except + ,- & updi when programming other wise the programming may corrupt  

; change main: to suit your needs  

; the TWI pins works directly without setting them as output, changing TWI BAUD between 50 -180 didn't affect the display visually. I don't have any measuring instruments to check actual values  

; Simple hardware TWI procedure is used . no buss error is taken into account, works for simple hobby projects.  

; PB1 = SDA = pin8 , PB0 = SCL = pin9. use with external pullup of 4k7 on each pin  

; programming- vcc = pin1,gnd =pin14, PA0=UPDI=pin10  

; The delay subroutines were taken from 4 bit LCD code written by Donald Weiman    ([weimandn@alfredstate.edu](mailto:weimandn@alfredstate.edu))   ;       

; Gratitude to the numerous individuals who have posted on the internet from which I have taken shamelessly.   

; Created: 2/14/2021 8:58:33 PM  

; Author : pappan  

; Kerala



.equ data\_command1 = 0b00001001        ; data control nibble ,led on P3, EN 0 on P2, R/W 0 (write) in P1 , RS 1 (0 instruction, 1 data) = 1001  =0x09  

.equ data\_command2 = 0b00001101        ; data control nibble , 1101  = 0x0D   - EN goes hi=1  

.equ data\_command3 = 0b00001001        ; data control nibble , 1001  = 0x09   - EN goes low=0  

.equ inst\_command1 = 0b00001000        ;instruction control nibble ,  led on en-lo,Rw-0,rs =0   = 1000   = 0x08  

.equ inst\_command2 = 0b00001100        ;instruction control nibble ,  led on, EN hi , rs/RW 0    = 1100   = 0x0C  

.equ inst\_command3 = 0b00001000        ;instruction control nibble  , led on, EN lo ,rs/rw 0    = 1000   = 0x08  

.EQU SLAVE\_ADDRESSW = 0X4E   ; LCD1602 =4E  

.equ fclk = 10000000  

.DEF  SLAVE\_REG = R17  

.DEF  TEMP = R16


.macro micros                ;call this macro for delay in micro seconds - max value is 255us, calling convention; micros delay value ;eg micros 20  

ldi temp,@0  

rcall delayTx1uS  

.endm



.macro millis                ;call this macro for delay in milli seconds - max value is 65.535 seconds, calling convention; millis + delay value ;eg millis 20  

ldi YL,low(@0)  

ldi YH,high(@0)  

rcall delayYx1mS  

.endm



.macro pos                ;call this macro for setting coordinates on the LCD, pos Y,X. eg pos 1,8 = line 1(2ndline),8th position;pos 0,2 = line0 (1st line) 2nd postion  

ldi r16,@1  

mov r6,r16  

ldi r16,@0  

rcall posi  

.endm



.macro arrayread            ; call this macro for sending strings from flash or sending arrays. arrayread + address = arrayread string (eg in this code)  

ldi ZL,low(2*@0)  

ldi ZH,high(2*@0)  

rcall array  

.endm



.DSEG  

PAD: .BYTE 1                ; reserve one byte in SRAM with address name PAD for COMMAND\_WRITE & DATA\_WRITE calculations



.CSEG  

.ORG 0X00  

rjmp reset


reset:  

        rcall i2c\_init        ;subroutine to initialize and send TWI start  

        rcall LCD\_INIT        ;subroutine to initialize Hitachi LCD  

        rcall PROT\_WRITE    :subroutine to change MCU clock speed from default 3.3Mhz to 10Mhz  

    main:  

        pos 0,0            ;cursor position to Y = 0 , X = 0  

        ldi SLAVE\_REG,'S'    ;load ASCII 'S' to r17  

        rcall DATA\_WRITE    ;call DATA\_WRITE function to do a 4 bit TWI transmit to LCD  

        ldi SLAVE\_REG,'A'    ;load ASCII 'A' to r17  

        rcall DATA\_WRITE    ;call DATA\_WRITE function to do a 4 bit TWI transmit to LCD  

        ldi SLAVE\_REG,'J'    ;load ASCII 'J' to r17  

        rcall DATA\_WRITE    ;call DATA\_WRITE function to do a 4 bit TWI transmit to LCD  

        ldi SLAVE\_REG,'E'    ;load ASCII 'E' to r17  

        rcall DATA\_WRITE    ;call DATA\_WRITE function to do a 4 bit TWI transmit to LCD  

        ldi SLAVE\_REG,'E'    ;load ASCII 'E' to r17  

        rcall DATA\_WRITE    ;call DATA\_WRITE function to do a 4 bit TWI transmit to LCD  

        ldi SLAVE\_REG,'V'    ;load ASCII 'V' to r17  

        rcall DATA\_WRITE    ;call DATA\_WRITE function to do a 4 bit TWI transmit to LCD  

        pos 1,0            ;cursor positioned to Y = 1 , X = 0  

        arrayread string    ;reads array "Hello!",0 stored in flash address 'string'  

                rcall TWI\_STOP        ; TWI stopped



      

end:  

        rjmp end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;PROTECTED WRITE  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;out OSCCFG,r16 ;;; this makes the chip run at 10Mhz by selecting prescaler 0x01 = divided by 2  

PROT\_WRITE:  

        ldi r16,0Xd8  

        out CPU\_CCP,r16  

        ldi r16,0x01  

        STS CLKCTRL\_MCLKCTRLB,R16  

        RET  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;I2C ROUTINES  

;this routine does not use ACK as the I2C chip on LCD does not properly ack which prints garbage on the screen  

;if used on slaves with proper ACK ,uncomment the ACK routine & CHK\_CLKHOLD below  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


TWI\_INIT:  

        ldi r16,80                ;load baud rate for I2C , (fclck/2*bitrate)-10 , freq = 10Mhz ,10000000/(200000 -10)   

        sts TWI0\_MBAUD,R16            ; store into baud register .50 is the correct Baud for 100KHZ. Used 80 to slow down the I2C  

        LDI R16,0b00000001            ;ENABLE TWI  

        STS TWI0\_MCTRLA,R16  

        LDI R16,0b00001000            ;FLUSH ADDR & DATA REGISTERS  

        STS TWI0\_MCTRLB,R16  

        LDI R16,0b00000001            ;FORCE IDLE , the bus is forced to idle state ie both SDA & SCL high  

        STS TWI0\_MSTATUS,R16  

        ret


TWI\_START:  

        ldi SLAVE\_REG,SLAVE\_ADDRESSW        ; write address of the Hitachi HD44780 LCD ,0x4E  

        MOV R16,SLAVE\_REG            ; SLAVE\_REG IS R17, READ OR WRITE ADDRESS SHOULD BE LOADED HERE PRIOR TO CALL INIT  

        STS TWI0\_MADDR,R16            ;store in TWI address register to be transmitted over I2C bus  

        RCALL WAIT\_WIF                ;call routine that waits for write interrupt flag  

        RET


TWI\_WRITE:  

        ;rcall CHK\_CLKHOLD  

        MOV R16,SLAVE\_REG            ;mov data from SLAVE\_REG/r17 to r16  

        STS TWI0\_MDATA,R16            ;copy r16 to MDATA register for I2C transmission  

        RCALL WAIT\_WIF                ;call WAIT\_WIF subroutine to check write interrupt flag is set after writing finished  

        micros 30                ;30us delay for data to settle  

        RET



TWI\_STOP:  

        LDI R16,0b00000011                       ;STOP  

        STS TWI0\_MCTRLB,R16  

        RET



WAIT\_WIF:  

        LDS R16,TWI0\_MSTATUS            ;copy flag data in TWI0\_MSTATUS register to R16 to check write interrupt flag is set(WIF)  

        SBRS R16,6                ;CHECK WIF IS SET,IF SET SKIP NEXT INSTRUCTION .(WIF=0 transmitting, WIF=1 write finished)  

        RJMP WAIT\_WIF                ;sit in a tight loop till write finished  

        ret  

;ACK:  

        ;LDS R16,TWI0\_MSTATUS  

        ;SBRC R16,4                ;  

        ;RJMP ACK  

        ;RET



;CHK\_CLKHOLD:  

        ;LDS R16,TWI0\_MSTATUS  

        ;SBRS R16,5                ;  

        ;RJMP CHK\_CLKHOLD  

        ;RET


i2c\_init:  

        LDI SLAVE\_REG,SLAVE\_ADDRESSW        ;this routine is a single unit to initiate the I2C which clubs load address,init TWI & send start signal  

         RCALL TWI\_INIT                ;after this routine multiple commands & data can be sent to the LCD with out stopping the bus  

        rcall TWI\_START  

        ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;LCD ROUTINES  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


                            ;command write is routine for sending LCD commands such as position , init  etc.  

                            ; the routine sends the upper 4 bits of the LCD command by oring it with the RS,D/C,E & backlight bits and then sends the lower 4 bits by the same method  

                            ;the serially sent command is then applied to the LCD by the I2C IC on the LCD  

                            ;load the LCD command to the SLAVE\_REG and rcall the COMMAND\_WRITE to send commands to LCD followed by delay  

                            ;as in the main loop above 


COMMAND\_WRITE:  

        STS PAD,R17                ;copy SLAVE\_REG to SRAM address PAD for processing  

        ANDI R17,0XF0                ;preserve upper nibble of SLAVE\_REG by anding with 0xF0 ,lower 4 becomes 0  

        ORI R17,inst\_command1            ;add instruction\_command1 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,inst\_command2            ;add instruction\_command2 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,inst\_command3            ;add instruction\_command3 to lower nibble of r17 by OR ing it  

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20          

        LDS R17,PAD                ;copy back data from PAD to r17 for processing the remaining lower nibble  

        SWAP R17                ;swap the nibbles in R17 so the lower nibble will occupy the upper area of reg  

        ANDI R17,0XF0                ;preserve upper nibble of SLAVE\_REG by anding with 0xF0  

        ORI R17,inst\_command1            ;add instruction\_command1 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,inst\_command2            ;add instruction\_command2 to lower nibble of r17 by OR ing it  

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,inst\_command3            ;add instruction\_command3 to lower nibble of r17 by OR ing it  

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ;RCALL TWI\_STOP                ;call TWI\_STOP  

        RET    


                            ;DATA\_WRITE is a routine to transmit data to DDRAM of the LCD  

                            ;the routine sends the upper 4 bits of the LCD data by oring it with the RS,D/C,E & backlight bits and then sends the lower 4 bits by the same method  

                            ;load data into the SLAVE\_REG and rcall DATA\_WRITE followed by a 30us delay  

      

DATA\_WRITE:  

        STS PAD,R17                          ;copy SLAVE\_REG to SRAM address PAD for processing  

        ANDI R17,0XF0                       ;preserve upper nibble of SLAVE\_REG by anding with 0xF0,lower 4 becomes 0  

        ORI R17,data\_command1         ;add data\_command1 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                   ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,data\_command2            ;add data\_command2 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,data\_command3            ;add data\_command3 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        LDS R17,PAD                        ;copy back data from PAD to r17 for processing the remaining lower nibble  

        SWAP R17                            ;swap the nibbles in R17 so the lower nibble will occupy the upper area of reg  

        ANDI R17,0XF0                    ;preserve upper nibble of SLAVE\_REG by anding with 0xF0  

        ORI R17,data\_command1      ;add data\_command1 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,data\_command2      ;add data\_command2 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ORI R17,data\_command3      ;add data\_command3 to lower nibble of r17 by OR ing it   

        RCALL TWI\_WRITE                ;call TWI\_WRITE routine to transmit command data to LCD  

        micros 20  

        ;RCALL TWI\_STOP                ;call TWI\_STOP  

        RET    



                                        ;this is the initialization procedure as discribed in the hitachi HD44780 data sheet  

LCD\_INIT:  

        millis 50                            ;call to macro millis for 50 ms delay  

        LDI R17,0b00111100                        ;LCD command  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay  

        LDI R17,0B00111000                        ;LCD command  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        millis 50                            ; value loaded here (20) decides the number of milli seconds in the delay below  

         


          

        LDI R17,0b00111100                        ;LCD command, r17=SLAVE\_REG  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay  

        LDI R17,0B00111000                        ;LCD command  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        millis 50                            ; value loaded here (20) decides the number of milli seconds in the delay below  

         


          

        LDI R17,0b00111100                        ;LCD command  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay  

        LDI R17,0B00111000                        ;LCD command  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        millis 50                            ; value loaded here (20) decides the number of milli seconds in the delay below  

         


          

        LDI R17,0b00101100                        ;LCD command,4bit  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay  

        LDI r17,0b00101000                        ;LCD command,4bit  

        RCALL TWI\_WRITE                            ;TWI transmit the above LCD command  

        millis 50                            ; value loaded here (20) decides the number of milli seconds in the delay below  

         


          

        LDI R17,0b00101000                        ;LCD command, FUNCTION SET ,4 bit,2lines,5x8 font  

        RCALL COMMAND\_WRITE                        ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay


          

        LDI R17,0b00001100                        ;LCD command, DISPLAY, display on ,cursor off, blink off  

        RCALL COMMAND\_WRITE                        ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay


          

        LDI R17,0b00000110                        ;LCD command, ENTRY MODE ,cursor increment, shift off   

        RCALL COMMAND\_WRITE                        ;TWI transmit the above LCD command  

        micros 20                            call to macro micro for 20us delay


          

        LDI R17,0b00000001                        ;LCD command, CLEAR DISPLAY  

        RCALL COMMAND\_WRITE                        ;TWI transmit the above LCD command  

        micros 20                            ;call to macro micro for 20us delay  

        RET



                                        ;This routine is used by the pos macro to calculate the position of the   

                                        ;DDRAM where the data is to be sent on the LCD screen. This decides the line  

                                        ;number or Y and then adds the horizontal postion X to get the correct position  

                                        ; the pos macro is supplied with coordinates - pos Y , X. where Y = line # & X = column#  

                                        ;COMMAND\_WRITE is included in the routine so data can be directly written after the pos call                                      

posi:  

        cpi r16,0x00                             ;compare (current data in r16 is Y = @0) in r16 to 0.   

        breq line1                                ;If 0 go to label line 1  

        ldi r16,0xc0                             ;If not 0 the remaining line is #1 for 1602 LCD which has start address of 0xC0 (0b11000000) as per data sheet  

        add r6,r16                               ;add r6,r16 = r6 data which is horizontal address of DDRAM and r16 line starting address = r6 + 0xC0 = position 0n 2nd line  

        mov SLAVE\_REG,r6                  ;copy data to SLAVE\_REG for transmit on I2C  

        rcall COMMAND\_WRITE             ;call COMMAND\_WRITE as this position data is a LCD command and will write to command register  

        ret  

    line1:  

        ldi r16,0x80                            ;load value 0x80 which is the address of the first DDRAM on th 1st line (line0),left corner top row as per data sheet  

        add r6,r16                            ;add column data with row data to get exact position on 1st line, r6 has row data .r6+r16 = r6 + 0x80 = position on 1st line  

        mov SLAVE\_REG,r6                        ;copy data to SLAVE\_REG for transmit on I2C  

        rcall COMMAND\_WRITE                        ;call COMMAND\_WRITE as this position data is a LCD command and will write to   

        ret


                                        ;this routine is used to write a string/array to LCD  

                                        ;arrayread macro has to be called and supplied with address of the array  

              

array:  

        lpm SLAVE\_REG,Z+                        ;the Z pointer is used to load the data into r17 from the array in flash   

        cpi SLAVE\_REG,0                            ;compare the loaded data to null (checks whether the array reached the end)  

        breq exit                            ;if null exit the routine  

        rcall DATA\_WRITE                        ;if not null call TWI\_WRITE to transmit the data to LCD  

        rjmp array                            ; loop through the array until a null is encountered  

    exit:     ret                                ;return to main



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


 string:  

.db "Hello!",0

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- ATtiny804
- TWI
- I2C
- ATTINY804
