# Attiny10 OLED Simple I2C Bitbang master in assembly with fonts

Uploaded by darth vader on 2021-02-23 12:31:00 (rating 5 out of 5)

## Summary

; Attiny10OLEDtest.asm  

; Created: 12/27/2020 10:29:49 AM  

; Author : pappan


;ATMEL STUDIO 7


;ATTINY10


;connect pb0 of attiny10 to SDA of OLED SSD1306 0.96 INCH – 4 PIN I2C 128X32 Display Module , use 5k resistor to +ve


;connect pb2 of attiny10 to SCL of OLED SSD1306 0.96 INCH – 4 PIN I2C 128X32 Display Module , use 5k resistor to +ve


;This program will print HELLO on the scree at coordinates 0,20


; rows/pages 0,1,2,3


;columns 0-128 for each page


;row or page decides the line number & column decides the position on the lines horizontally.  pos 0,10 means line 0 (top line) and tenth position sideways  

;pb0 is SDA  

;pb2 is SCL


; modify the program to suit . I wrote it to test OLED for making a speedo meter by input capture. This was an attempt to make the OLED work. call pos for coordinates and printf for printing from array.  

.equ address = 0x78  

.equ command = 0x00  

.equ data\_cmd = 0x40  

.equ OLED\_INIT\_LEN = 12    ; 14 if screen flip is needed , 12 if normal screen  

.equ font\_length = 5  

.def temp = r16  

.def data = r17  

.def counter = r18  

.def array\_length = r19  

.def array\_byte = r20  

.def ASCII = r21  

.def temp1 = r22  

.def posx = r23  

.def posy = r24



.macro printf  

ldi temp,@0  

mov ASCII,temp  

rcall print  

.endm 


.macro cursor  

ldi posy,@0  

ldi posx,@1  

rcall set\_cursor  

.endm   

   

.cseg  

reset:  

rcall i2c\_command\_write  

rcall OLED\_INIT  

ldi array\_length,OLED\_INIT\_LEN  

rcall array\_read  

rcall i2c\_stop  

rcall clear\_OLED  

cursor 0, 20  

printf 'H'  

printf 'E'  

printf 'L'  

printf 'L'  

printf 'O'  

cursor 2, 20  

mov ASCII,result\_high  

rcall print  

mov ASCII,result\_low  

rcall print  

mov ASCII,r27  

rcall print  

printf 'K'  

printf 'M'  

printf '/'  

printf 'H'  

rjmp capture


;end:  

;rjmp end


OLED\_INIT:  

ldi ZL,low((2*OLED\_INIT\_BYTES)+0x4000)  

ldi ZH,high((2*OLED\_INIT\_BYTES)+0x4000)  

ret  

OLED\_INIT\_BYTES:  

.DB 0xA8,0x1F,0x22,0x00,0x03,0x20,0x00,0xDA,0x02,0x8D,0x14,0xAF,0xA1,0xC8  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;      

;  0xA8, 0x1F,       // set multiplex (HEIGHT-1): 0x1F for 128x32, 0x3F for 128x64 ;   

;  0x22, 0x00, 0x03, // set min and max page                       ;  

;  0x20, 0x00,       // set horizontal memory addressing mode               ;  

;  0xDA, 0x02,       // set COM pins hardware configuration to sequential          ;  

;  0x8D, 0x14,       // enable charge pump                                         ;  

;  0xAF,             // switch on OLED                                             ;  

;  0xA1, 0xC8        // flip the screen                                            ;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;I2C ROUTINES     register used- temp/data/counter = r16/r17/r18                                                                 ;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

sda\_low:  

sbi ddrb,ddb0  

nop  

ret


sda\_high:  

cbi ddrb,ddb0  

nop  

ret


scl\_low:  

sbi ddrb,ddb2  

nop  

ret


scl\_high:  

cbi ddrb,ddb2  

nop  

ret


i2c\_init:  

ldi temp,0x00  

out ddrb,temp  

out portb,temp  

ret


i2c\_start:  

rcall sda\_low  

rcall scl\_low  

ldi data,address  

rcall i2c\_write  

ret


i2c\_stop:  

rcall sda\_low  

rcall scl\_high  

rcall sda\_high  

ret


i2c\_write:  

ldi counter,0x08  

loop:  

rcall sda\_low  

mov temp,data  

andi temp,0x80  

sbrc temp,7  

rcall sda\_high  

nop  

rcall scl\_high  

lsl data  

rcall scl\_low  

dec counter  

brne loop  

rcall sda\_high  

rcall scl\_high  

nop  

nop  

rcall scl\_low  

rcall sda\_low  

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;I2C\_COMMAND\_WRITE  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


i2c\_command\_write:  

rcall i2c\_init  

rcall i2c\_start  

ldi data,command  

rcall i2c\_write  

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;I2C\_DATA\_WRITE  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

i2c\_data\_write:  

rcall i2c\_init  

rcall i2c\_start  

ldi data,data\_cmd  

rcall i2c\_write  

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;ARRAY READ FUNCTION registers-r19/r20 ;array\_length = r19,array\_byte = r20   

;dependency = use ldi array\_length,( number of bytes ) prior to rcall array\_read  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

array\_read:  

loop1:  

ld array\_byte,Z+  

mov data,array\_byte  

rcall i2c\_write  

dec array\_length  

brne loop1  

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;ADDRESS LOADER 1  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;.macro address  

;ldi ZL,low((2*@0)+0x4000)  

;ldi ZH,high((2*@0)+0x4000)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;PRINT function   uses registers- temp1,temp,array\_byte,ASCII,array\_length  

;characters to be printed has to be passed into register ASCII as ASCII values  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

print:  

ldi ZL,low((2*fonts)+0x4000)  

ldi ZH,high((2*fonts)+0x4000)  

ldi array\_byte,0x20           ;hex value of first ASCII character in font array  

sub ASCII,array\_byte        ;result of subtraction will be position of first byte of the character  

breq ASCII0  

clr temp  

ldi temp1,0x00  

multiply:  

subi r30,low(-6)            ;adding immediate not supported. immediate extends beyond 8bit.using subi & sbci with -ve number will do & loading with hi &lo will propagate carry.  

sbci r31,high(-6)  

inc temp  

cp temp,ASCII  

brne multiply  

ASCII0:  

rcall i2c\_data\_write  

;ldi data,0x00            ;small space between characters (print each time to save flash space).commented out as assembler added a padding byte iof 0 to tech line  

;rcall i2c\_data\_write  

ldi array\_length,6  

rcall array\_read  

rcall i2c\_stop  

ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;FONTS   fonts below 5bytes ,assembler will add one byte of padding with 0. hence array lenth =6  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


fonts:  

.db 0x00, 0x00, 0x00, 0x00, 0x00 //sp 0  0x20  

.DB 0x00, 0x00, 0x2f, 0x00, 0x00 // ! 1  0x21  

.DB 0x00, 0x07, 0x00, 0x07, 0x00 // " 2  0x22  

.DB 0x14, 0x7f, 0x14, 0x7f, 0x14 // # 3  0x23  

.DB 0x24, 0x2a, 0x7f, 0x2a, 0x12 // $ 4  0x24  

.DB 0x62, 0x64, 0x08, 0x13, 0x23 // % 5  0x25  

.DB 0x36, 0x49, 0x55, 0x22, 0x50 // & 6  0x26  

.DB 0x00, 0x05, 0x03, 0x00, 0x00 // ' 7  0x27  

.DB 0x00, 0x1c, 0x22, 0x41, 0x00 // ( 8  0x28  

.DB 0x00, 0x41, 0x22, 0x1c, 0x00 // ) 9  0x29  

.DB 0x14, 0x08, 0x3E, 0x08, 0x14 // * 10 0x2A  

.DB 0x08, 0x08, 0x3E, 0x08, 0x08 // + 11 0x2B  

.DB 0x00, 0x00, 0xA0, 0x60, 0x00 // , 12 0x2C  

.DB 0x08, 0x08, 0x08, 0x08, 0x08 // - 13 0x2D  

.DB 0x00, 0x60, 0x60, 0x00, 0x00 // . 14 0x2E  

.DB 0x20, 0x10, 0x08, 0x04, 0x02 // / 15 0x2F  

.DB 0x3E, 0x51, 0x49, 0x45, 0x3E // 0 16 0x30  

.DB 0x00, 0x42, 0x7F, 0x40, 0x00 // 1 17 0x31  

.DB 0x42, 0x61, 0x51, 0x49, 0x46 // 2 18 0x32  

.DB 0x21, 0x41, 0x45, 0x4B, 0x31 // 3 19 0x33  

.DB 0x18, 0x14, 0x12, 0x7F, 0x10 // 4 20 0x34  

.DB 0x27, 0x45, 0x45, 0x45, 0x39 // 5 21 0x35  

.DB 0x3C, 0x4A, 0x49, 0x49, 0x30 // 6 22 0x36  

.DB 0x01, 0x71, 0x09, 0x05, 0x03 // 7 23 0x37  

.DB 0x36, 0x49, 0x49, 0x49, 0x36 // 8 24 0x38  

.DB 0x06, 0x49, 0x49, 0x29, 0x1E // 9 25 0x39  

.DB 0x00, 0x36, 0x36, 0x00, 0x00 // : 26 0x3A  

.DB 0x00, 0x56, 0x36, 0x00, 0x00 // ; 27 0x3B  

.DB 0x08, 0x14, 0x22, 0x41, 0x00 // < 28 0X3C  

.DB 0x14, 0x14, 0x14, 0x14, 0x14 // = 29 0X3D  

.DB 0x00, 0x41, 0x22, 0x14, 0x08 // > 30 0X3E  

.DB 0x02, 0x01, 0x51, 0x09, 0x06 // ? 31 0X3F  

.DB 0x32, 0x49, 0x59, 0x51, 0x3E // @ 32 0X40  

.DB 0x7C, 0x12, 0x11, 0x12, 0x7C // A 33 0X41  

.DB 0x7F, 0x49, 0x49, 0x49, 0x36 // B 34 0X42  

.DB 0x3E, 0x41, 0x41, 0x41, 0x22 // C 35 0X43  

.DB 0x7F, 0x41, 0x41, 0x22, 0x1C // D 36 0X44  

.DB 0x7F, 0x49, 0x49, 0x49, 0x41 // E 37 0X45  

.DB 0x7F, 0x09, 0x09, 0x09, 0x01 // F 38 0X46  

.DB 0x3E, 0x41, 0x49, 0x49, 0x7A // G 39 0X47  

.DB 0x7F, 0x08, 0x08, 0x08, 0x7F // H 40 0X48  

.DB 0x00, 0x41, 0x7F, 0x41, 0x00 // I 41 0X49  

.DB 0x20, 0x40, 0x41, 0x3F, 0x01 // J 42 0X4A  

.DB 0x7F, 0x08, 0x14, 0x22, 0x41 // K 43 0X4B  

.DB 0x7F, 0x40, 0x40, 0x40, 0x40 // L 44 0X4C  

.DB 0x7F, 0x02, 0x0C, 0x02, 0x7F // M 45 0X4D  

.DB 0x7F, 0x04, 0x08, 0x10, 0x7F // N 46 0X4E  

.DB 0x3E, 0x41, 0x41, 0x41, 0x3E // O 47 0X4F  

.DB 0x7F, 0x09, 0x09, 0x09, 0x06 // P 48 0X50  

.DB 0x3E, 0x41, 0x51, 0x21, 0x5E // Q 49 0X51  

.DB 0x7F, 0x09, 0x19, 0x29, 0x46 // R 50 0X52  

.DB 0x46, 0x49, 0x49, 0x49, 0x31 // S 51 0X53  

.DB 0x01, 0x01, 0x7F, 0x01, 0x01 // T 52 0X54  

.DB 0x3F, 0x40, 0x40, 0x40, 0x3F // U 53 0X55  

.DB 0x1F, 0x20, 0x40, 0x20, 0x1F // V 54 0X56  

.DB 0x3F, 0x40, 0x38, 0x40, 0x3F // W 55 0X57  

.DB 0x63, 0x14, 0x08, 0x14, 0x63 // X 56 0X58  

.DB 0x07, 0x08, 0x70, 0x08, 0x07 // Y 57 0X59  

.DB 0x61, 0x51, 0x49, 0x45, 0x43 // Z 58 0X5A  

.DB 0x00, 0x7F, 0x41, 0x41, 0x00 // [ 59 0X5B  

.DB 0x02, 0x04, 0x08, 0x10, 0x20 // \ 60 0X5C  

.DB 0x00, 0x41, 0x41, 0x7F, 0x00 // ] 61 0X5D  

.DB 0x04, 0x02, 0x01, 0x02, 0x04 // ^ 62 0X5E  

.DB 0x40, 0x40, 0x40, 0x40, 0x40  // \_ 63 0X5F


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;CLEAR\_OLED    r20/r19 registers array\_byte/array\_length used  writes 128*4 0 to OLED  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


clear\_OLED:  

ldi array\_byte,4  

rcall i2c\_data\_write  

loop3:  

ldi array\_length,128  

loop4:  

ldi data,0x00  

rcall i2c\_write  

dec array\_length  

brne loop4  

loop5:  

dec array\_byte  

brne loop3  

ret  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

;SET\_ROW   uses register y/x /temp   r24/r23/r16  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  

set\_cursor:  

;set\_row  

rcall i2c\_command\_write  

ldi data, 0x22  

rcall i2c\_write  

mov data,posy  

rcall i2c\_write  

ldi data,0x03            ;row end - can be 1 ,2,3. we choose last page 3  

rcall i2c\_write  

;set\_column  

ldi data, 0x21  

rcall i2c\_write  

mov data,posx  

rcall i2c\_write  

ldi data,0x7f            ;column begins at 0 to 127 as end in each row. we will choose column end as 127=0x7f  

rcall i2c\_write  

rcall i2c\_stop  

ret

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- #SSD1306OLED #Simple # I2C #Bitbang master in #assembly with #fonts #Attiny10
