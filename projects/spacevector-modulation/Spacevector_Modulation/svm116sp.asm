;Author: Dimitri Tseronis
;Project: Spacevector Modulation
;Version 116sp - 27.05.2002 - Berlin
;AD1 - Frequency
;AD0 - Modulation Index (Amplitude without overmodulation limited to 86,6%)
;PB7 - Direction

.INCLUDE "8535def.inc"  ;register definitions for the AT97S8535

.DEF  liml=r8
.DEF  limh=r9
.DEF  vt0=r10
.DEF  vt0ab=r11
.DEF  vta=r12
.DEF  vtb=r13
.DEF  freq=r14
.DEF  amp=r15
.DEF  tmp=r16
.DEF  tmph=r17
.DEF  itmp=r18
.DEF  itmph=r19
.DEF  sector=r20

.CSEG
rjmp  init                 ;reset vector
reti  ;INT0_interrupt      ;External Interrupt0
reti  ;INT1_interrupt      ;External Interrupt1
reti  ;OC2_interrupt       ;Timer2 compare match
reti  ;OVF2_interrupt      ;Timer2 overflow
reti  ;ICP1_interrupt      ;Timer1 Input Capture
reti  ;OC1A_interrupt      ;Timer1 Output Compare A Interrupt
reti  ;OC1B_interrupt      ;Timer1 Output Compare B Interrupt
rjmp  OVF1_interrupt       ;Overflow1 Interrupt
reti  ;OVF0_interrupt      ;Overflow0 Interrupt
reti  ;SPI_interrupt       ;SPI Interrupt
reti  ;URXC_interrupt      ;UART Receive Complete Interrupt
reti  ;UDRE_interrupt      ;UART Data Register Empty Interrupt
reti  ;UTXC_interrupt      ;UART Transmit Complete Interrupt
reti  ;ADCC_interrupt      ;ADC Conversion Complete Interrupt
reti  ;ERDY_interrupt      ;EEPROM Write Complete Interrupt
reti  ;ACI_interrupt       ;Analog Comparator Interrupt

init:                      ;Initialisation routine begin
ldi   tmp,High(RamEnd)     ;stack pointer definition high byte
out   SPH,tmp              ;read from xxxxdef.inc file
ldi   tmp,Low(RamEnd)      ;stack pointer definition low byte
out   SPL,tmp              ;read from xxxxdef.inc file

init_interrupts:           ;Initialisation interrupt registers
ldi   tmp,0b00000000       ;
out   GIMSK,tmp            ;
ldi   tmp,0b00000100       ;ocie2,toie2,ticie1,ocie1a,ocie1b,toie1,-,toie0
out   TIMSK,tmp            ;timer/counter interrupt mask register

init_timer:                ;Initialisation Timer registers
ldi   tmp,0b11110001       ;com1a1,com1a0,com1b1,com1b0,-,-,pwm11,pwm10
out   TCCR1A,tmp

sync_start:                ;The Syncronisation of the Timers is needed to Force Timer1 and 2 to beheave like one.
ldi   tmp,2                ;For a SVM without  deadtimes you need 4 Time Points. 3 Capture Compare for the 3-Phases and one for the PWM Period.
out   TCNT2,tmp            ;Symmetric PWM Mode is used.,(Less switching loss) and a CLK/1 Prescaler for 15.686 Hz Switching Frequency. With a Prescaler of CLK/8 =>2kHz Switching Frequency.
ldi   tmp,0b00000001       ;icnc1,ices1,-,-,ctc1,cs12,cs11,cs10
out   TCCR1B,tmp
ldi   tmp,0b01110001       ;-,pwm,com21,com20,ctc2,cs22,cs21,cs20
out   TCCR2,tmp
ldi   tmp,0b00000001

init_IO_Ports:             ;Analog Input PA0 Amplitude, PA1 Frequency
ldi   tmp,0b00000000
out   PORTA,tmp
ldi   tmp,0b00000000
out   DDRA,tmp
                           ;Only PB7 used as Input for the Direction
ldi   tmp,0b00000000
out   PORTB,tmp
ldi   tmp,0b00000000
out   DDRB,tmp

ldi   tmp,0b11111111       ;NOT USED
out   PORTC,tmp
ldi   tmp,0b11111111
out   DDRC,tmp

ldi   tmp,0b10110000       ;SVM OUTPUT on Pins 4,5,7
out   PORTD,tmp
ldi   tmp,0b11111111
out   DDRD,tmp

init_values:
clr   r0
clr   r1
clr   XL
clr   XH
clr   sector
clr   vta
clr   vtb
clr   vt0
clr   vt0ab

ldi   tmp,$FF
ldi   tmph,$07
mov   liml,tmp
mov   limh,tmph

ldi   tmp,50      ;frequenz
mov   freq,tmp
ldi   tmp,250     ;amplitude
mov   amp,tmp
clt               ;richtung

enable_interrupts:
sei

main:
sbic  PINB,7
set
sbis  PINB,7
clt

AD0_0:
ldi   tmp,$00
out   ADMUX,tmp
ldi   tmp,0b11000001    ;aden,adsc,adfr,adif,adie,adps2,adps1,adps0
out   ADCSR,tmp         ; No precision needed
AD0_1:
sbic  ADCSR,ADSC
rjmp  AD0_1
in tmp,ADCL
lsr   tmp
lsr   tmp
in tmph,ADCH
lsl   tmph
lsl   tmph
lsl   tmph
lsl   tmph
lsl   tmph
lsl   tmph
or tmph,tmp
add   amp,tmph
ror   amp             ;something like a  Sotware Lowpassfilter

AD1_0:
ldi   tmp,$01
out   ADMUX,tmp
ldi   tmp,0b11000001    ;aden,adsc,adfr,adif,adie,adps2,adps1,adps0
out   ADCSR,tmp
AD1_1:
sbic  ADCSR,ADSC
rjmp  AD1_1
in tmp,ADCL
lsr   tmp
lsr   tmp
in tmph,ADCH
lsl   tmph
lsl   tmph
lsl   tmph
lsl   tmph
lsl   tmph
lsl   tmph
or tmph,tmp
add   freq,tmph
ror   freq

rjmp  main

OVF1_interrupt:
cpi   sector,0
breq  sector0
cpi   sector,1
breq  sector1
cpi   sector,2
breq  sector2
cpi   sector,3
breq  sector3
cpi   sector,4
breq  sector4
cpi   sector,5
breq  sector5
rjmp  OVF1_interrupt

sector0:
out   OCR1AL,vt0
out   OCR1BL,vta
out   OCR2,vt0ab
rjmp  get_time

sector1:
out   OCR1AL,vtb
out   OCR1BL,vt0
out   OCR2,vt0ab
rjmp  get_time

sector2:
out   OCR1AL,vt0ab
out   OCR1BL,vt0
out   OCR2,vta
rjmp  get_time

sector3:
out   OCR1AL,vt0ab
out   OCR1BL,vtb
out   OCR2,vt0
rjmp  get_time

sector4:
out   OCR1AL,vta
out   OCR1BL,vt0ab
out   OCR2,vt0
rjmp  get_time

sector5:
out   OCR1AL,vt0
out   OCR1BL,vt0ab
out   OCR2,vtb

get_time:

calc_vtb:
ldi   ZL,low(2*Table)
ldi   ZH,high(2*Table)
add   ZL,XL
adc   ZH,XH
lpm

push  amp               ;SW Multiply from ATMEL App.Note 200A
mulb_0:  clr   itmpH    ;clear result High byte
   ldi   itmp,8         ;init loop counter
   lsr   amp            ;rotate multiplier

mulb_1:  brcc  mulb_2   ;carry set
   add   itmpH,r0       ;   add multiplicand to result High byte
mulb_2:  ror   itmpH    ;rotate right result High byte
   ror   amp            ;rotate right result L byte and multiplier
   dec   itmp           ;decrement loop counter
   brne  mulb_1         ;if not done, loop more
pop   amp
mov   vtb,itmph

calc_vta:
ldi   ZL,low(2*Table)
ldi   ZH,high(2*Table)
add   ZL,limL
adc   ZH,limH
sub   ZL,XL
sbc   ZH,XH
lpm

push  amp
mula_0:  clr   itmpH    ;clear result High byte
   ldi   itmp,8         ;init loop counter
   lsr   amp            ;rotate multiplier

mula_1:  brcc  mula_2   ;carry set
   add   itmpH,r0       ;    add multiplicand to result High byte
mula_2:  ror   itmpH    ;rotate right result High byte
   ror   amp            ;rotate right result L byte and multiplier
   dec   itmp           ;decrement loop counter
   brne  mula_1         ;if not done, loop more
pop   amp
mov   vta,itmph

brtc  calc_vt0
mov   itmp,vtb
mov   vtb,itmph
mov   vta,itmp

calc_vt0:
clr   vt0ab
add   vt0ab,vta
add   vt0ab,vtb
mov   vt0,vt0ab      ;AB Vector
com   vt0

ptr_ctrl:            ;The Table Index(Step) is added to the Pointer(Z) until the Table Limit is reached
lsr   vt0            ;Divide/2
add   vta,vt0
add   vtb,vt0
add   vt0ab,vt0

clr   itmph
add   XL,freq
adc   XH,itmph

cpi   XH,$08
breq  ch_sec
reti

ch_sec:           ;(sector)Sector Change
clr   XL
clr   XH
brts  rev_sector

fwd_sector:       ;Moving Forward
cpi   sector,5
breq  fwd_cycle
inc   sector
reti
fwd_cycle:
clr   sector
reti

rev_sector:      ;Moving Reverse
cpi   sector,0
breq  rev_cycle
dec   sector
reti
rev_cycle:
ldi   sector,5
reti


Table:   ;0.866-2048 - Some value Dosent fit. Thats why you get a zapp every 60 Degrees
.db   $00,$00,$00,$00,$00,$00,$00,$00,$01,$01,$01,$01,$01,$01,$01,$01
.db   $02,$02,$02,$02,$02,$02,$02,$02,$03,$03,$03,$03,$03,$03,$03,$04
.db   $04,$04,$04,$04,$04,$04,$04,$05,$05,$05,$05,$05,$05,$05,$05,$06
.db   $06,$06,$06,$06,$06,$06,$07,$07,$07,$07,$07,$07,$07,$07,$08,$08
.db   $08,$08,$08,$08,$08,$08,$09,$09,$09,$09,$09,$09,$09,$0A,$0A,$0A
.db   $0A,$0A,$0A,$0A,$0A,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0C,$0C,$0C
.db   $0C,$0C,$0C,$0C,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0E,$0E,$0E,$0E
.db   $0E,$0E,$0E,$0E,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$10,$10,$10,$10,$10
.db   $10,$10,$10,$11,$11,$11,$11,$11,$11,$11,$11,$12,$12,$12,$12,$12
.db   $12,$12,$13,$13,$13,$13,$13,$13,$13,$13,$14,$14,$14,$14,$14,$14
.db   $14,$14,$15,$15,$15,$15,$15,$15,$15,$16,$16,$16,$16,$16,$16,$16
.db   $16,$17,$17,$17,$17,$17,$17,$17,$17,$18,$18,$18,$18,$18,$18,$18
.db   $18,$19,$19,$19,$19,$19,$19,$19,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A
.db   $1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1D
.db   $1D,$1D,$1D,$1D,$1D,$1D,$1D,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1F
.db   $1F,$1F,$1F,$1F,$1F,$1F,$1F,$20,$20,$20,$20,$20,$20,$20,$21,$21
.db   $21,$21,$21,$21,$21,$21,$22,$22,$22,$22,$22,$22,$22,$22,$23,$23
.db   $23,$23,$23,$23,$23,$23,$24,$24,$24,$24,$24,$24,$24,$25,$25,$25
.db   $25,$25,$25,$25,$25,$26,$26,$26,$26,$26,$26,$26,$26,$27,$27,$27
.db   $27,$27,$27,$27,$27,$28,$28,$28,$28,$28,$28,$28,$29,$29,$29,$29
.db   $29,$29,$29,$29,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2B,$2B,$2B,$2B
.db   $2B,$2B,$2B,$2B,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2D,$2D,$2D,$2D,$2D
.db   $2D,$2D,$2D,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2F,$2F,$2F,$2F,$2F
.db   $2F,$2F,$2F,$30,$30,$30,$30,$30,$30,$30,$30,$31,$31,$31,$31,$31
.db   $31,$31,$32,$32,$32,$32,$32,$32,$32,$32,$33,$33,$33,$33,$33,$33
.db   $33,$33,$34,$34,$34,$34,$34,$34,$34,$34,$35,$35,$35,$35,$35,$35
.db   $35,$35,$36,$36,$36,$36,$36,$36,$36,$36,$37,$37,$37,$37,$37,$37
.db   $37,$37,$38,$38,$38,$38,$38,$38,$38,$39,$39,$39,$39,$39,$39,$39
.db   $39,$3A,$3A,$3A,$3A,$3A,$3A,$3A,$3A,$3B,$3B,$3B,$3B,$3B,$3B,$3B
.db   $3B,$3C,$3C,$3C,$3C,$3C,$3C,$3C,$3C,$3D,$3D,$3D,$3D,$3D,$3D,$3D
.db   $3D,$3E,$3E,$3E,$3E,$3E,$3E,$3E,$3E,$3F,$3F,$3F,$3F,$3F,$3F,$3F
.db   $3F,$40,$40,$40,$40,$40,$40,$40,$40,$41,$41,$41,$41,$41,$41,$41
.db   $41,$42,$42,$42,$42,$42,$42,$42,$43,$43,$43,$43,$43,$43,$43,$43
.db   $44,$44,$44,$44,$44,$44,$44,$44,$45,$45,$45,$45,$45,$45,$45,$45
.db   $46,$46,$46,$46,$46,$46,$46,$46,$47,$47,$47,$47,$47,$47,$47,$47
.db   $48,$48,$48,$48,$48,$48,$48,$48,$49,$49,$49,$49,$49,$49,$49,$49
.db   $4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B
.db   $4C,$4C,$4C,$4C,$4C,$4C,$4C,$4C,$4D,$4D,$4D,$4D,$4D,$4D,$4D,$4D
.db   $4E,$4E,$4E,$4E,$4E,$4E,$4E,$4E,$4E,$4F,$4F,$4F,$4F,$4F,$F4,$F4
.db   $4F,$50,$50,$50,$50,$50,$50,$50,$50,$51,$51,$51,$51,$51,$51,$51
.db   $51,$52,$52,$52,$52,$52,$52,$52,$52,$53,$53,$53,$53,$53,$53,$53
.db   $53,$54,$54,$54,$54,$54,$54,$54,$54,$55,$55,$55,$55,$55,$55,$55
.db   $55,$56,$56,$56,$56,$56,$56,$56,$56,$57,$57,$57,$57,$57,$57,$57
.db   $57,$57,$58,$58,$58,$58,$58,$58,$58,$58,$59,$59,$59,$59,$59,$59
.db   $59,$59,$5A,$5A,$5A,$5A,$5A,$5A,$5A,$5A,$5B,$5B,$5B,$5B,$5B,$5B
.db   $5B,$5B,$5C,$5C,$5C,$5C,$5C,$5C,$5C,$5C,$5C,$5D,$5D,$5D,$5D,$5D
.db   $5D,$5D,$5D,$5E,$5E,$5E,$5E,$5E,$5E,$5E,$5E,$5F,$5F,$5F,$5F,$5F
.db   $5F,$5F,$5F,$60,$60,$60,$60,$60,$60,$60,$60,$60,$61,$61,$61,$61
.db   $61,$61,$61,$61,$62,$62,$62,$62,$62,$62,$62,$62,$63,$63,$63,$63
.db   $63,$63,$63,$63,$63,$64,$64,$64,$64,$64,$64,$64,$64,$65,$65,$65
.db   $65,$65,$65,$65,$65,$66,$66,$66,$66,$66,$66,$66,$66,$66,$67,$67
.db   $67,$67,$67,$67,$67,$67,$68,$68,$68,$68,$68,$68,$68,$68,$68,$69
.db   $69,$69,$69,$69,$69,$69,$69,$6A,$6A,$6A,$6A,$6A,$6A,$6A,$6A,$6B
.db   $6B,$6B,$6B,$6B,$6B,$6B,$6B,$6B,$6C,$6C,$6C,$6C,$6C,$6C,$6C,$6C
.db   $6D,$6D,$6D,$6D,$6D,$6D,$6D,$6D,$6D,$6E,$6E,$6E,$6E,$6E,$6E,$6E
.db   $6E,$6F,$6F,$6F,$6F,$6F,$6F,$6F,$6F,$6F,$70,$70,$70,$70,$70,$70
.db   $70,$70,$71,$71,$71,$71,$71,$71,$71,$71,$71,$72,$72,$72,$72,$72
.db   $72,$72,$72,$72,$73,$73,$73,$73,$73,$73,$73,$73,$74,$74,$74,$74
.db   $74,$74,$74,$74,$74,$75,$75,$75,$75,$75,$75,$75,$75,$76,$76,$76
.db   $76,$76,$76,$76,$76,$76,$77,$77,$77,$77,$77,$77,$77,$77,$77,$78
.db   $78,$78,$78,$78,$78,$78,$78,$79,$79,$79,$79,$79,$79,$79,$79,$79
.db   $7A,$7A,$7A,$7A,$7A,$7A,$7A,$7A,$7A,$7B,$7B,$7B,$7B,$7B,$7B,$7B
.db   $7B,$7B,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7D,$7D,$7D,$7D,$7D,$7D
.db   $7D,$7D,$7D,$7E,$7E,$7E,$7E,$7E,$7E,$7E,$7E,$7E,$7F,$7F,$7F,$7F
.db   $7F,$7F,$7F,$7F,$7F,$80,$80,$80,$80,$80,$80,$80,$80,$80,$81,$81
.db   $81,$81,$81,$81,$81,$81,$81,$82,$82,$82,$82,$82,$82,$82,$82,$82
.db   $83,$83,$83,$83,$83,$83,$83,$83,$83,$84,$84,$84,$84,$84,$84,$84
.db   $84,$84,$85,$85,$85,$85,$85,$85,$85,$85,$85,$86,$86,$86,$86,$86
.db   $86,$86,$86,$86,$87,$87,$87,$87,$87,$87,$87,$87,$87,$88,$88,$88
.db   $88,$88,$88,$88,$88,$88,$89,$89,$89,$89,$89,$89,$89,$89,$89,$8A
.db   $8A,$8A,$8A,$8A,$8A,$8A,$8A,$8A,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B
.db   $8B,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8C,$8D,$8D,$8D,$8D,$8D,$8D
.db   $8D,$8D,$8D,$8D,$8E,$8E,$8E,$8E,$8E,$8E,$8E,$8E,$8E,$8F,$8F,$8F
.db   $8F,$8F,$8F,$8F,$8F,$8F,$90,$90,$90,$90,$90,$90,$90,$90,$90,$91
.db   $91,$91,$91,$91,$91,$91,$91,$91,$91,$92,$92,$92,$92,$92,$92,$92
.db   $92,$92,$93,$93,$93,$93,$93,$93,$93,$93,$93,$93,$94,$94,$94,$94
.db   $94,$94,$94,$94,$94,$95,$95,$95,$95,$95,$95,$95,$95,$95,$96,$96
.db   $96,$96,$96,$96,$96,$96,$96,$96,$97,$97,$97,$97,$97,$97,$97,$97
.db   $97,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$99,$99,$99,$99,$99
.db   $99,$99,$99,$99,$99,$9A,$9A,$9A,$9A,$9A,$9A,$9A,$9A,$9A,$9B,$9B
.db   $9B,$9B,$9B,$9B,$9B,$9B,$9B,$9B,$9C,$9C,$9C,$9C,$9C,$9C,$9C,$9C
.db   $9C,$9C,$9D,$9D,$9D,$9D,$9D,$9D,$9D,$9D,$9D,$9E,$9E,$9E,$9E,$9E
.db   $9E,$9E,$9E,$9E,$9E,$9F,$9F,$9F,$9F,$9F,$9F,$9F,$9F,$9F,$9F,$A0
.db   $A0,$A0,$A0,$A0,$A0,$A0,$A0,$A0,$A0,$A1,$A1,$A1,$A1,$A1,$A1,$A1
.db   $A1,$A1,$A1,$A2,$A2,$A2,$A2,$A2,$A2,$A2,$A2,$A2,$A2,$A3,$A3,$A3
.db   $A3,$A3,$A3,$A3,$A3,$A3,$A3,$A4,$A4,$A4,$A4,$A4,$A4,$A4,$A4,$A4
.db   $A4,$A5,$A5,$A5,$A5,$A5,$A5,$A5,$A5,$A5,$A5,$A6,$A6,$A6,$A6,$A6
.db   $A6,$A6,$A6,$A6,$A6,$A7,$A7,$A7,$A7,$A7,$A7,$A7,$A7,$A7,$A7,$A8
.db   $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8,$A9,$A9,$A9,$A9,$A9,$A9,$A9
.db   $A9,$A9,$A9,$A9,$AA,$AA,$AA,$AA,$AA,$AA,$AA,$AA,$AA,$AA,$AB,$AB
.db   $AB,$AB,$AB,$AB,$AB,$AB,$AB,$AB,$AC,$AC,$AC,$AC,$AC,$AC,$AC,$AC
.db   $AC,$AC,$AC,$AD,$AD,$AD,$AD,$AD,$AD,$AD,$AD,$AD,$AD,$AE,$AE,$AE
.db   $AE,$AE,$AE,$AE,$AE,$AE,$AE,$AE,$AF,$AF,$AF,$AF,$AF,$AF,$AF,$AF
.db   $AF,$AF,$B0,$B0,$B0,$B0,$B0,$B0,$B0,$B0,$B0,$B0,$B0,$B1,$B1,$B1
.db   $B1,$B1,$B1,$B1,$B1,$B1,$B1,$B1,$B2,$B2,$B2,$B2,$B2,$B2,$B2,$B2
.db   $B2,$B2,$B3,$B3,$B3,$B3,$B3,$B3,$B3,$B3,$B3,$B3,$B3,$B4,$B4,$B4
.db   $B4,$B4,$B4,$B4,$B4,$B4,$B4,$B4,$B5,$B5,$B5,$B5,$B5,$B5,$B5,$B5
.db   $B5,$B5,$B5,$B6,$B6,$B6,$B6,$B6,$B6,$B6,$B6,$B6,$B6,$B6,$B7,$B7
.db   $B7,$B7,$B7,$B7,$B7,$B7,$B7,$B7,$B7,$B8,$B8,$B8,$B8,$8B,$B8,$B8
.db   $B8,$B8,$B8,$B8,$B9,$B9,$B9,$B9,$B9,$B9,$B9,$B9,$B9,$B9,$B9,$BA
.db   $BA,$BA,$BA,$BA,$BA,$BA,$BA,$BA,$BA,$BA,$BA,$BB,$BB,$BB,$BB,$BB
.db   $BB,$BB,$BB,$BB,$BB,$BB,$BC,$BC,$BC,$BC,$BC,$BC,$BC,$BC,$BC,$BC
.db   $BC,$BD,$BD,$BD,$BD,$BD,$BD,$BD,$BD,$BD,$BD,$BD,$BD,$BE,$BE,$BE
.db   $BE,$BE,$BE,$BE,$BE,$BE,$BE,$BE,$BF,$BF,$BF,$BF,$BF,$BF,$BF,$BF
.db   $BF,$BF,$BF,$BF,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0
.db   $C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C2,$C2,$C2,$C2,$C2
.db   $C2,$C2,$C2,$C2,$C2,$C2,$C2,$C3,$C3,$C3,$C3,$C3,$C3,$C3,$C3,$C3
.db   $C3,$C3,$C3,$C4,$C4,$C4,$C4,$C4,$C4,$C4,$C4,$C4,$C4,$C4,$C4,$C5
.db   $C5,$C5,$C5,$C5,$C5,$C5,$C5,$C5,$C5,$C5,$C5,$C6,$C6,$C6,$C6,$C6
.db   $C6,$C6,$C6,$C6,$C6,$C6,$C6,$C7,$C7,$C7,$C7,$C7,$C7,$C7,$C7,$C7
.db   $C7,$C7,$C7,$C7,$C8,$C8,$C8,$C8,$C8,$C8,$C8,$C8,$C8,$C8,$C8,$C8
.db   $C9,$C9,$C9,$C9,$C9,$C9,$C9,$C9,$C9,$C9,$C9,$C9,$C9,$CA,$CA,$CA
.db   $CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CB,$CB,$CB,$CB,$CB,$CB,$CB
.db   $CB,$CB,$CB,$CB,$CB,$CB,$CC,$CC,$CC,$CC,$CC,$CC,$CC,$CC,$CC,$CC
.db   $CC,$CC,$CC,$CD,$CD,$CD,$CD,$CD,$CD,$CD,$CD,$CD,$CD,$CD,$CD,$CD
.db   $CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CF,$CF,$CF
.db   $CF,$CF,$CF,$CF,$CF,$CF,$CF,$CF,$CF,$CF,$D0,$D0,$D0,$D0,$D0,$D0
.db   $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1
.db   $D1,$D1,$D1,$D1,$D1,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2
.db   $D2,$D2,$D2,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3
.db   $D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D5,$D5
.db   $D5,$D5,$D5,$D5,$D5,$D5,$D5,$D5,$D5,$D5,$D5,$D5,$D6,$D6,$D6,$D6
.db   $D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7,$D7,$D7,$D7,$D7,$D7
.db   $D7,$D7,$D7,$D7,$D7,$D7,$D7,$D7,$D7,$D8,$D8,$D8,$D8,$D8,$D8,$D8
.db   $D8,$D8,$D8,$D8,$D8,$D8,$D8,$D9,$D9,$D9,$D9,$D9,$D9,$D9,$D9,$D9
.db   $D9,$D9,$D9,$D9,$D9,$D9,$DA,$DA,$DA,$DA,$DA,$DA,$DA,$DA,$DA,$DA
.db   $DA,$DA,$DA,$DA,$DA,$DB,$DB,$DB,$DB,$DB,$DB,$DB,$DB,$DB,$DB,$DB
.db   $DB,$DB,$DB,$DB,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC

.EXIT ;Thats it.