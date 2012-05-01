# Torch led

Uploaded by maquad on 2012-05-01 07:48:00 (rating 0 out of 5)

## Summary

;miniUPS 1.0v  

;-l'orologio OK (funziona correttamente)  

;-Per sopperire allo sfarfallio visibile all'accensione del led si implementa:  

;1)Calcolo di Media pesata sull'acquisizione di 32 valori(bisognerebbe migliorare il calcolo della media non troncandola al numero intero inferiore (2,9=2), ma ad un corretto bilanciamento (2,9=3;2,5=3))  

;2)per sopperire alle leggere fluttuazioni + o - si Ã¨ implementato una funzione che determina entro una soglia stabilitÃ  se se si tratta di un rumore/disturbo o regolazione manuale  

;-carica della batteria dovrebbe interrompersi in funzione del tempo di carica e tensione attuale misurata  

;1)Monitorare e salvare in eeprom: misura il numero di volte che si accende il circuito in carica/scarica della batteria  

;Vinmax=16V->Vsource=5V Vinmin=12V->Vsource=3,5V  

;micro legge tensioni di 1,7Volt = che equivale a 1 Logico  

;06/03/2011 abbiamo cambiato il circuito che si interfaccia con INT0 e INT1 del micro [09/03/2011 IMPLEMENTATO]  

;08/03/2011 se Vbatt<0,6V (batteria non presente) allora CHARGER OFF, e LED LUXEON ON[19/03/2011 IMPLEMENTATO] Ã¨ importante lasciare lo switch chiuso se no, non funziona!!  

;17/03/2011 ho trovato un errore nel settare il valore del pwm. Prima si setta il valore ALTO E poi quello basso altrimenti vediamo il led che sfarfalla [CORRETTO]  

; out OCR1AH,r16 ;set PWM value High  

; out OCR1AL,r17 ;set PWM value Low


;-----------------------------------------------------------------------------  

;Atmega8:  

; 1 - PC6 (RESET) -RESET  

; 2 - PD0 (RXD) -RXD  

; 3 - PD1 (TXD) -TXD  

; 4 - PD2 (INT0) -SWITCH ON/OFF  

; 5 - PD3 (INT1) -STEP DOWN out  

; 6 - PD4 (XCK/T0) -  

; 7 - VCC  

; 8 - GND  

; 9 - PB6 (XTAL1/TOSC1) -4 MHz  

; 10 - PB7 (XTAL2/TOSC2)-4 MHz  

; 11 - PD5 (T1) -OUT LED ROSSO  

; 12 - PD6 (AIN0) -OUT LED GIALLO  

; 13 - PD7 (AIN1) -OUT LED VERDE2  

; 14 - PB0 (ICP1) -OUT LED VERDE1  

; 15 - PB1 (OC1A) -OUT PWM1 LED LUXEON  

; 16 - PB2 (SS/OC1B) -  

; 17 - PB3 (MOSI/OC2) -OUT PWM2 CHARGER  

; 18 - PB4 (MISO) -  

; 19 - PB5 (SCK) -  

; 20 - AVCC -VCC  

; 21 - AREF -VCC  

; 22 - GND  

; 23 - PC0 (ADC0) -BATTERY  

; 24 - PC1 (ADC1) -  

; 25 - PC2 (ADC2) -  

; 26 - PC3 (ADC3) -  

; 27 - PC4 (ADC4/SDA) -  

; 28 - PC5 (ADC5/SCL) -POTENZIOMETRO ESTERNO  

;-----------------------------------------------------------------------------  

//FUSEBIT : RSTDISBL=1;WDTON=1;SPIEN=0;CKOPT=1;EESAVE=1;BOOTSZ1=0;BOOTSZ0=0;BOOTRST=1;BODLEVEL=1;BODEN=1;SUT1=1;SUT0=1;CKSEL3=1;CKSEL2=1;CKSEL1=1;CKSEL0=1;


.include "m8def.inc"


;5V/1024=0,0048828125V 3V/0,0048828125V=614  

.equ vbat\_h\_max = 0b00000010 ;tensione massima di batteria valore alto  

.equ vbat\_l\_max = 0b01100110 ;tensione massima di batteria valore basso


.equ vbat\_h\_min = 0b00000001 ;tensione minima di batteria valore alto  

.equ vbat\_l\_min = 0b01011010 ;tensione minima di batteria valore basso  

 ;se Vbatt<= 1,0Volt allora stato [0]=0  

 ;se Vbatt<= 1,8Volt allora LedRosso=on  

 ;se Vbatt<= 2,15Volt allora LedGiallo=on  

 ;se Vbatt<= 2,5Volt allora LedVerde2=on  

 ;se Vbatt> 2,5Volt allora LedVerde1=on  

.equ vbat\_withoutH = 0b00000000 ;Vbatt<= 1,0Volt (204,8)  

.equ vbat\_withoutL = 0b11001110 ;  

.equ vbat\_redH = 0b00000001 ;Vbatt<= 1,8Volt (368,64)  

.equ vbat\_redL = 0b01100101 ;  

.equ vbat\_yellowH = 0b00000001 ;Vbatt<= 2,15Volt (440,32)  

.equ vbat\_yellowL = 0b10110110 ;  

.equ vbat\_greenH = 0b00000010 ;Vbatt<= 2,5Volt (512)  

.equ vbat\_greenL = 0b00000011 ;


;evitare surriscaldamento led LUXEON  

.equ max\_pwm1 = 255 ;PWM1 maximum  

.equ min\_pwm1 = 0 ;PWM1 minimum  

;evitare surriscaldamento step down  

.equ max\_pwm2 = 255 ;PWM2 maximum  

.equ min\_pwm2 = 0 ;PWM2 minimum  

;evitare correnti troppo forti  

.equ correntemax = 1  

.equ correntemin = 254  

//.equ correntefix = 100 ;0,5A - bollente in pochi minuti  

//.equ correntefix = 125 ;0,45A -  

//.equ correntefix = 150 ;0,37A -  

//.equ correntefix = 162 ;0,34A - scotta  

.equ correntefix = 175 ;0,29A -  

//.equ correntefix = 187 ;0,25A - medio  

//.equ correntefix = 200 ;0,2A -


;soglia prefissata per evitare oscillazioni di luce  

.equ soglialuce = 32;8;14;254


;nella conversione da ASCII a VOLT  

.equ unitavolt = 46 ;46mVolt


;dimensione buffer  

.equ dimbuff = 0x20 ;16Word=32byte


;valori dei pwm


.equ val\_TCCR1A = 0b10000011 ;COM1A1,COM1A0,COM1B1,COM1B0,FOC1A,FOC1B,WGM11,WGM10  

.equ val\_TCCR1B = 0b00000010;0b00001010 ;ICNC1,ICES1,-,WGM13,WGM12,CS12,CS11,CS10  

.equ val\_TCCR2 = 0b01100010;0b01101010 ;FOC2,WGM20,COM21,COM20,WGM21,CS22,CS21,CS20


//CSx2|CSx1|CSx0| PRESCALE TCCR1 | PRESCALE TCCR2 |  

// 0 | 0 | 0 | NO CLOCK SOURCE | NO CLOCK SOURCE |  

// 0 | 0 | 1 | CLK/1 | CLK/1 |  

// 0 | 1 | 0 | CLK/8 | CLK/8 |  

// 0 | 1 | 1 | CLK/64 | CLK/32 |  

// 1 | 0 | 0 | CLK/256 | CLK/64 |  

// 1 | 0 | 1 | CLK/1024 | CLK/128 |  

// 1 | 1 | 0 | EXT CLOCK ON T1 PIN,ON FALLING EDGE | CLK/256 |  

// 1 | 1 | 1 | EXT CLOCK ON T1 PIN,ON RISING EDGE | CLK/1024 |


//COM1x1|COM1x0  

// 0 | 0 NORMAL PORT, OC1A/OC1B DISCONNECTED  

// 0 | 1 TOGGLE OC1A/OC1B ON COMPARE MATCH  

// 1 | 0 CLEAR OC1A/OC1B ON COMPARE MATCH  

// 1 | 1 SET OC1A/OC1B ON COMPARE MATCH


//WGMx3|WGMx2|WGMx1|WGMx0| MODE TCCR1 | MODE TCCR2  

// 0 | 0 | 0 | 0 | NORMAL | NORMAL  

// 0 | 0 | 0 | 1 | PWM,PHASE CORRECT, 8BIT | PWM,PHASE CORRECT  

// 0 | 0 | 1 | 0 | PWM,PHASE CORRECT, 9BIT | CTC  

// 0 | 0 | 1 | 1 | PWM,PHASE CORRECT, 10BIT | FAST PWM  

// 0 | 1 | 0 | 0 | CTC (TOP=ICR1)  

// 0 | 1 | 0 | 1 | FAST PWM, 8BIT  

// 0 | 1 | 1 | 0 | FAST PWM, 9BIT  

// 0 | 1 | 1 | 1 | FAST PWM, 10BIT  

// 1 | 0 | 0 | 0 | PWM,PHASE&FREQUENCY CORRECT (TOP=ICR1)  

// 1 | 0 | 0 | 1 | PWM,PHASE&FREQUENCY CORRECT (TOP=OCR1A)  

// 1 | 0 | 1 | 0 | PWM,PHASE CORRECT (TOP=ICR1)  

// 1 | 0 | 1 | 1 | PWM,PHASE CORRECT (TOP=OCR1A)  

// 1 | 1 | 0 | 0 | CTC (TOP=ICR1)  

// 1 | 1 | 0 | 1 | (RESERVED)  

// 1 | 1 | 1 | 0 | FAST PWM (TOP=ICR1)  

// 1 | 1 | 1 | 1 | FAST PWM (TOP=OCR1A)


;FAST PWM Frequenza=4000000/(prescale*256)  

;PWM,PHASE CORRECT Frequenza=4000000/(prescale*510)


;registri microcontrollore  

.def tem0=R16  

.def tem1=R17  

.def tem2=R18  

.def tem3=R19  

.def tem4=R20  

.def tem5=R21  

.def tem6=R22  

.def tem7=R23  

.def tem8=R24  

.def stato=R25  

;variabile stato gestisce il main  

;bit7 Not used  

;bit6 0=  

;bit5 0=  

;bit4 0=Vtemperature not measured;1=Vtemperature already measured;  

;bit3 0=Vpotentiometer not measured;1=Vpotentiometer already measured;  

;bit2 0=Vbattery not measured;1=Vbattery already measured;  

;bit1 0=STEP UP DISCONNECTED(INT1);1=STEP UP CONNECTED(INT1);  

;bit0 0=SWITCH OFF(INT0);1=SWITCH ON(INT0);


//.def XL =r26  

//.def XH =r27  

//.def YL =r28  

//.def YH =r29  

//.def ZL =r30  

//.def ZH =r31


//VARIABLE SRAM  

.equ VbattL = 0x0060  

.equ VbattH = 0x0061  

.equ VpoteL = 0x0062  

.equ VpoteH = 0x0063  

.equ VtempL = 0x0064  

.equ VtempH = 0x0065


.equ microsecL = 0x0070  

.equ microsecH = 0x0071  

.equ millisecL = 0x0072  

.equ millisecH = 0x0073  

.equ secondi = 0x0074  

.equ minuti = 0x0075  

.equ ore = 0x0076


.equ mediabattYL = 0x00  

.equ mediabattYH = 0x01  

.equ mediapoteYL = 0x20  

.equ mediapoteYH = 0x01


//VARIABLE EEPROM durata 100000 cicli Clear/Write  

;questo determina un limite di durata del micro (ipotesi 50 accensioni al giorno ->100000/(52sett*5giorni*50volte)= 7,7anni)  

;100000=0x186A0 servono 3 byte  

;3 byte ContPSU[2],ContPSU[1],ContPSU[0]  

.equ ContPSU = 0x00  

;3 byte ContSWT[2],ContSWT[1],ContSWT[0]  

.equ ContSWT = 0x03


.cseg  

.org 0x0000  

// interrupt vectors  

rjmp Start ; Reset Handler  

rjmp EXT\_INT0; IRQ0 Handler  

rjmp EXT\_INT1 ; IRQ1 Handler  

rjmp Start;TIM2\_COMP ; Timer2 Compare Handler  

rjmp Start;TIM2\_OVF ; Timer2 Overflow Handler  

rjmp Start;TIM1\_CAPT ; Timer1 Capture Handler  

rjmp Start;TIM1\_COMPA ; Timer1 CompareA Handler  

rjmp Start;TIM1\_COMPB ; Timer1 CompareB Handler  

rjmp Start;TIM1\_OVF ; Timer1 Overflow Handler  

rjmp TIM0\_OVF ; Timer0 Overflow Handler  

rjmp Start;SPI\_STC ; SPI Transfer Complete Handler  

rjmp Start;USART\_RXC ; USART RX Complete Handler  

rjmp Start;USART\_UDRE ; UDR Empty Handler  

rjmp Start;USART\_TXC ; USART TX Complete Handler  

rjmp Start;ADConversion; ADC Conversion Complete Handler  

rjmp Start;EE\_RDY ; EEPROM Ready Handler  

rjmp Start;ANA\_COMP ; Analog Comparator Handler  

rjmp Start;TWSI ; Two-wire Serial Interface Handler  

rjmp Start;SPM\_RDY ; Store Program Memory Ready Handler


Start:  

// INITIALIZE CPU, STACK POINTER  

ldi r16, low(RAMEND)  

out SPL, r16  

ldi r16, high(RAMEND)  

out SPH, r16  

//INTERRUPT: SLEEP=b7EN;b6,b5,b4=standby mode;INT1=b3,b2;INT0=b1,b0 logical level  

ldi r16, 0b00000101;0b00000001;0b11100101;  

out MCUCR, r16  

;ISC11|ISC10|Description  

; 0 | 0 |The low level of INT1 generates an interrupt request.  

; 0 | 1 |Any logical change on INT1 generates an interrupt request.  

; 1 | 0 |The falling edge of INT1 generates an interrupt request.  

; 1 | 1 |The rising edge of INT1 generates an interrupt request.


// DISABLE PULLUP = SFIOR  

ldi R16, 0b00000111 ;b3=ACME,b2=PUD,b1=PSR2,b0=PSR10  

out SFIOR,r16  

//EXTERNAL INTERRUPT  

;ldi r16, 0b00000000 //DISABLE INT0,INT1  

ldi r16, 0b11000000 //ENABLE INT0,INT1  

out GICR, r16  

//COMPARATOR DISABLE  

ldi R16, 0b10000000  

out ACSR,r16  

// initialization PORT B  

ldi r16, 0b00000001  

out DDRB, r16 //set port B0 as output  

ldi r16, 0b00000000  

out PORTB, r16  

// initialization PORT C  

clr r16  

out DDRC, r16  

out PORTC, r16  

// initialization PORT D  

ldi r16, 0b11100000 //set port D7,D6,D5 as output  

out DDRD, r16  

ldi r16, 0b00000000  

out PORTD, r16  

//WATCHDOG OFF  

/*ldi r16, 0b00011000  

out WDTCR, r16  

ldi r16, 0b00010000  

out WDTCR, r16*/  

// initialization USART  

;SET UCSRA BIT  

;7=RXC(R);6=TXC(R/W);5=UDRE(R);4=FE(R);3=DOR(R);2=PE(R);1=U2X(R/W);0=MPXM(R/W)  

ldi R16, 0b00000000  

out UCSRA, R16  

;SET UCSRB BIT  

;7=RXCIE(R/W);6=TXCIE(R/W);5=UDRIE(R/W);4=RXEN(R/W);3=TXEN(R/W);2=UCSZ2(R/W);1=RXB8(R);0=TXB8(R/W)  

;ldi R16, 0b00000000  

ldi R16, 0b00011000  

out UCSRB, R16  

;Baud rate  

ldi R16, 0b00000000  

out UBRRH, R16  

ldi R16, 25 ;25=9600BPS@4MHz  

out UBRRL, R16  

;SET UCSRC BIT  

;7=URSEL(R/W);6=UMSEL(R/W);5=UPM1(R/W);4=UPM0(R/W);3=USBS(R/W);2=UCSZ1(R/W);1=UCSZ0(R/W);0=UCPOL(R/W)  

ldi R16, 0b10000110  

out UCSRC, R16  

;inizializzazione registri PWM  

;PWM1  

clr r16 ;0% PWM at start -> nmos spento  

out OCR1AH,r16 ;set PWM value  

out OCR1AL,r16 ;set PWM value  

;PWM2  

clr r16 ;modalitÃ  con quarzo micro 4MHz  

out ASSR,r16  

ldi r16,correntefix ;100% PWM at start -> pmos spento  

out OCR2,r16  

;inizializzo Variabili  

rcall resetRam  

clr stato


;VERIFICA STATO DI ACCENSIONE  

;controlla se interruttore chiuso (INT1 - SWITCH ON)  

sbic PIND,3 ;if b3==0  

rjmp loop\_bornint10  

 /*;inverti interrupt fronte salita  

 in r16,MCUCR  

 sbr r16,4 ;b2=1  

 sbr r16,8 ;b3=1  

 out MCUCR,r16 ;MCUCR=xxxx11xxb*/  

 ;inverti interrupt soglia alta  

 in r16,MCUCR  

 sbr r16,4 ;b2=1  

 cbr r16,8 ;b3=0  

 out MCUCR,r16 ;MCUCR=xxxx01xxb  

 ;aggiorna variabile stato  

 sbr stato,2 ;b1=1  

loop\_bornint10:  

;controllo se Caricabatterie inserito (INT0 - CHARGER ON)  

sbis PIND,2  

rjmp loop\_bornint20  

 /*;inverti interrupt0 fronte discesa  

 in r16,MCUCR  

 cbr r16,1 ;b0=0  

 sbr r16,2 ;b1=1  

 out MCUCR,r16 ;MCUCR=0bxxxxxx10*/  

 ;inverti interrupt0 soglia bassa  

 in r16,MCUCR  

 cbr r16,1 ;b0=0  

 cbr r16,2 ;b1=0  

 out MCUCR,r16 ;MCUCR=0bxxxxxx00  

 ;aggiorna variabile stato  

 sbr stato,1 ;CHARGER ON;b0=1  

 cbr stato,2 ;LED LUXEON OFF;b1=0  

 ;VERIFICO SE NON C'Ã¨ LA BATTERIA  

 ;acquisistco la tensione  

 ;seleziono ADC0  

 ldi R16, 0b01000000  

 out ADMUX, R16  

 ;enable ADC and initialization first conversion  

 ldi R16, 0b11000001 ;prescale b2b1b0 000=2;001=2;010=4;011=8;100=16;101=32;110=64;111=128  

 out ADCSRA, R16  

 sbic ADCSRA,ADSC  

 rjmp PC-1  

 ;end conversion 25cycles 1Â° acquisizione  

 ;start again  

 sbi ADCSRA,ADSC  

 sbic ADCSRA,ADSC  

 rjmp PC-1  

 ;end conversion 25cycles 2Â° acquisizione  

 ;carica i valori  

 in r19,ADCH  

 in r18,ADCL  

 ;se Vbatt<= 1,0Volt allora stato[0]=0  

 cpi r19, vbat\_withoutH  

 brlo PC+7  

 breq PC+2  

 rjmp loop\_bornint20  

 cpi r18, vbat\_withoutL  

 brlo PC+3  

 breq PC+2  

 rjmp loop\_bornint20  

 rcall LED\_OFF  

 ;siamo senza batterie di conseguenza l'alimentazione Ã¨ data dal caricatore  

 ;aggiorna variabile stato  

 cbr stato,1 ;CHARGER OFF;b0=0  

 sbr stato,2 ;LED LUXEON ON;;b1=1  

 sbr stato,128 ;Blocco la misurazione su Vbatt  

loop\_bornint20:


////////////////////////////////////////////////////////////////////  

/////DA UTILIZZARE UNA VOLTA SOLA PER INIZIALIZZARE L'EEPROM////////  

////////////////////////////////////////////////////////////////////  

/*  

;AGGIORNAMENTO VARIABILI EEPROM  

//Verifico se siamo senza batteria  

sbrs stato,7  

rjmp loop\_eepromint10  

 rjmp loop\_eepromint30 ;esco e non incremento niente  

//Verifico se CHARGER ON-INT0 (PWM2)  

loop\_eepromint10:  

sbrs stato,0  

rjmp loop\_eepromint20  

 ;ContPSU[2] = 0x00;ContPSU[1] = 0x01;ContPSU[0] = 0x02  

 //LEGGO DATI PRECEDENTI  

 ldi r19,3 ;contatore  

 ldi ZH,0x00 ;puntatore alto  

 ldi ZL,ContPSU ;puntatore basso  

 //EEPROM\_read  

 sbic EECR,EEWE ;prox operazione? La lettura Ã¨ immediata non c'Ã¨ bisogno di questo controllo  

 rjmp PC-1  

 out EEARH, ZH ; Set up address  

 out EEARL, ZL  

 sbi EECR,EERE ; Start eeprom read by writing EERE  

 in r16,EEDR ;Read data from data register  

 push r16 ;stack  

 inc ZL  

 dec r19  

 brne PC-9  

 //INCREMENTO DI UNO  

 pop r18 ;carico ContPSU[0]  

 pop r17 ;carico ContPSU[1]  

 pop r16 ;carico ContPSU[2]  

 inc r16  

 brne PC+8 ;salva solo r16 ;attenzione agli inc con riporto brcc non funzionano ------------------------------------------------------------------------  

 inc r17  

 brne PC+4 ;salva solo r16,r17  

 inc r18  

 ;salva r16,r17,r18  

 inc r19 ;contatore  

 push r18  

 inc r19 ;contatore  

 push r17  

 inc r19 ;contatore  

 push r16 ;ARRIVO  

 //SALVO I DATI NELLA EEPROM  

 ldi ZH,0x00 ;puntatore alto  

 ldi ZL,ContPSU ;puntatore basso  

 pop r16 ;carico primo valore  

 //EEPROM\_write  

 sbic EECR,EEWE ;prox operazione?  

 rjmp PC-1  

 out EEARL, ZL  

 out EEDR,r16 ; Write data (r16) to data register  

 sbi EECR,EEMWE ; Write logical one to EEMWE  

 sbi EECR,EEWE ;Start eeprom write by setting EEWE  

 inc ZL  

 dec r19  

 brne PC-9  

 rjmp loop\_eepromint30 ;esco e non incremento niente  

//Verifico se SWITCH ON-INT1 (PWM1)  

loop\_eepromint20:  

sbrs stato,1  

rjmp loop\_eepromint30  

 ;ContSWT[2] = 0x03;ContSWT[1] = 0x04;ContSWT[0] = 0x05  

 //LEGGO DATI PRECEDENTI  

 ldi r19,3 ;contatore  

 ldi ZH,0x00 ;puntatore alto  

 ldi ZL,ContSWT ;puntatore basso  

 //EEPROM\_read  

 sbic EECR,EEWE ;prox operazione? La lettura Ã¨ immediata non c'Ã¨ bisogno di questo controllo  

 rjmp PC-1  

 out EEARH, ZH ; Set up address  

 out EEARL, ZL  

 sbi EECR,EERE ; Start eeprom read by writing EERE  

 in r16,EEDR ; Read data from data register  

 push r16 ;stack  

 inc ZL  

 dec r19  

 brne PC-9  

 //INCREMENTO DI UNO  

 pop r18 ;carico ContPSU[0]  

 pop r17 ;carico ContPSU[1]  

 pop r16 ;carico ContPSU[2]  

 inc r16  

 brne PC+8 ;salva solo r16 ;attenzione agli inc con riporto brcc non funzionano ------------------------------------------------------------------------  

 inc r17  

 brne PC+4 ;salva solo r16,r17  

 inc r18  

 ;salva r16,r17,r18  

 inc r19 ;contatore  

 push r18  

 inc r19 ;contatore  

 push r17  

 inc r19 ;contatore  

 push r16  

 //SALVO I DATI NELLA EEPROM  

 ldi ZH,0x00 ;puntatore alto  

 ldi ZL,ContSWT ;puntatore basso  

 pop r16 ;carico primo valore  

 //EEPROM\_write  

 sbic EECR,EEWE ;prox operazione?  

 rjmp PC-1  

 out EEARL, ZL  

 out EEDR,r16 ; Write data (r16) to data register  

 sbi EECR,EEMWE ; Write logical one to EEMWE  

 sbi EECR,EEWE ; Start eeprom write by setting EEWE  

 inc ZL  

 dec r19  

 brne PC-9  

loop\_eepromint30:  

*/


loop\_init\_timer0:  

// initialization TIMER0  

;ldi r16, 0b00000000 //TIMER=DISABLE  

ldi r16, 0b00000010//prescale 2E-6 Sec  

out TCCR0, r16  

ldi r16, 0b00000001 ///ENABLE INTERRUPT TIMER0 ad ogni 2E-6 Sec * 256 = 512E-6  

out TIMSK, r16  

;prescale B2,B1,B0  

;000=disable  

;001=clk 8MHz->0,125E-6Sec 4MHz->0,25E-6 Sec  

;010=clk/8 1MHz->1E-6Sec 0,5MHz->2E-6 Sec  

;011=clk/64 125KHz->8E-6Sec 62,5KHz->16E-6 Sec  

;100=clk/256 31250Hz->32E-6Sec 15625Hz->64E-6 Sec  

;101=clk/1024 7812,5Hz->128E-6Sec 3906,25Hz->256E-6 Sec  

;110=extT0 falling edge  

;111=ext T0 rising edge  

;ENABLE GENERAL INTERRUPT  

sei  

// main routine loop  

Main:  

loop\_main00:  

//////////////////////////////////////////  

///////////// MISURE ///////////////////  

////////////////////////////////////////  

 ;eseguiamo la misura sulla batteria  

; sbrc stato,7 ;se stato[7]=0 eseguiamo l'aquisizione di Vbatt altrimenti acquisisco solo Vpote  

; rjmp loop\_misure10  

 ;seleziono ADC0  

 ldi R16, 0b01000000  

 out ADMUX, R16  

 ;puntatore al buffer media Vbatt  

 ldi YH, mediabattYH  

 ldi YL, mediabattYL  

 rcall ADCRESULT ;eseguo l'acquisizione e il calcolo della media pesata su 16 elementi  

 ;carico i valori nella ram  

 ldi ZH,0  

 ldi ZL,VbattL  

 st Z+,r17 ;risultato della media pesata bassa  

 st Z+,r18 ;risultato della media pesata alta  

loop\_misure10:  

 ;eseguiamo la misura sul potenziometro  

 ;seleziono ADC5  

 ldi R16, 0b01000101  

 out ADMUX, R16  

 ;puntatore al buffer media Vpote  

 ldi YH, mediapoteYH  

 ldi YL, mediapoteYL  

 rcall ADCRESULT ;eseguo l'acquisizione  

 ;carico i valori nella ram  

 ldi ZH,0  

 ldi ZL,VpoteL  

 st Z+,r17 ;risultato della media pesata bassa  

 st Z+,r18 ;risultato della media pesata alta


loop\_main10:  

//////////////////////////////////////////  

/////// LED = STATO BATTERIA ////////////  

////////////////////////////////////////  

;Siamo senza batteria?  

 sbrc stato,7 ;se stato[7]=0 visualizziamo lo stato  

 rjmp loop\_main30 ;inutile caricare la batteria se non c'Ã¨  

;compariamo la tensione batteria con valori fissi di default  

loop\_led10:  

 //funzione primitiva che determina accensione led  

 ;carico valori batteria attuali in variabili temporanee  

 ;5V/1024=0,0048828125V 2V/0,0048828125V=409  

 ldi ZH,0  

 ldi ZL,VbattL  

 ld r16,Z+  

 ld r17,Z  

loop\_led20:  

 ;se Vbatt<= 1,8Volt allora LedRosso=on  

 cpi r17, vbat\_redH  

 brlo PC+7  

 breq PC+2  

 rjmp loop\_led30  

 cpi r16, vbat\_redL  

 brlo PC+3  

 breq PC+2  

 rjmp loop\_led30  

 rcall LED\_RED  

 rjmp loop\_main20  

loop\_led30:  

 ;se Vbatt<= 2,15Volt allora LedGiallo=on  

 cpi r17, vbat\_yellowH  

 brlo PC+7  

 breq PC+2  

 rjmp loop\_led40  

 cpi r16, vbat\_yellowL  

 brlo PC+3  

 breq PC+2  

 rjmp loop\_led40  

 rcall LED\_YELLOW  

 rjmp loop\_main20  

loop\_led40:  

 ;se Vbatt<= 2,5Volt allora LedVerde2=on  

 cpi r17, vbat\_greenH  

 brlo PC+7  

 breq PC+2  

 rjmp loop\_led50  

 cpi r16, vbat\_greenL  

 brlo PC+3  

 breq PC+2  

 rjmp loop\_led50 ;maggiore  

 rcall LED\_GREEN2  

 rjmp loop\_main20  

loop\_led50:  

 ;se Vbatt> 2,5Volt allora LedVerde1=on  

 rcall LED\_GREEN1  

 rjmp loop\_main20


loop\_main20:  

//////////////////////////////////////////  

////// INT0 - CARICA BATTERIA ///////////  

////////////////////////////////////////  

 ;Se stato[0]=0 charger OFF  

 sbrc stato,0  

 rjmp loop\_main25  

 ;SPENGO PMOS-CHARGER (PWM2 OFF)  

 cbi DDRB,3  

 cbi PORTB,3  

 ldi r16,0x00  

 out OCR2,r16  

 ldi r16,0x00 ;disable PWM  

 out TCCR2,r16  

 rjmp loop\_main30  

loop\_main25:  

 ;Se stato[0]=1 charger ON  

 sbrs stato,0  

 rjmp loop\_main30  

 ;ACCENDO PMOS-CHARGER(PWM2 ON)  

 sbi DDRB,3  

 sbi PORTB,3  

 ldi r16,correntefix  

 out OCR2,r16 ;set PWM value  

 ldi r16,val\_TCCR2  

 out TCCR2,r16


loop\_main30:  

//////////////////////////////////////////  

//////// INT1 - SWITCH ON/OFF ///////////  

////////////////////////////////////////  

 ;Se stato[1]=0 LED LUXEON OFF  

 sbrc stato,1  

 rjmp loop\_main35  

 ;SPENGO NMOS-LED (PWM1 OFF)  

 cbi DDRB,1  

 cbi PORTB,1  

 ldi r16,0b00000000  

 out OCR1AH,r16 ;set PWM value  

 out OCR1AL,r16 ;set PWM value  

 out TCCR1A,r16  

 out TCCR1B,r16  

 rjmp loop\_main40  

loop\_main35:  

 ;se stato[1]=1 LED LUXEON ON  

 sbrs stato,1  

 rjmp loop\_main40  

 ;ACCENDO NMOS-LED (PWM1 ON)  

 sbi DDRB,1  

 cbi PORTB,1  

 ldi ZH,0  

 ldi ZL,VpoteL  

 ld r16,Z+  

 ld r17,Z+  

 out OCR1AH,r17 ;set PWM value  

 out OCR1AL,r16 ;set PWM value  

 ldi r16,val\_TCCR1A  

 out TCCR1A,r16  

 ldi r16,val\_TCCR1B  

 out TCCR1B,r16 ;start clock


loop\_main40:  

//////////////////////////////////////////  

//USART = INVIA MISURE A HYPERTERMINAL //  

////////////////////////////////////////  

loop\_usart10:  

 ;NUOVA RIGA  

 ldi R19,13  

 rcall INVIA  

 ldi R19,10  

 rcall INVIA  

 ;TEMPO  

 ;ORE  

 ldi ZH,0  

 ldi ZL,ore  

 ld r16,Z ;carico ore  

 rcall CONVASCIIDEC  

 ldi R19,':'  

 rcall INVIA  

 ;MINUTI  

 ldi ZH,0  

 ldi ZL,minuti  

 ld r16,Z ;carico minuti  

 rcall CONVASCIIDEC  

 ldi R19,'.'  

 rcall INVIA  

 ;SECONDI  

 ldi ZH,0  

 ldi ZL,secondi  

 ld r16,Z ;carico secondi  

 rcall CONVASCIIDEC  

 ldi R19,' '  

 rcall INVIA  

 ;TENSIONE BATTERIA  

 ldi ZH,0  

 ldi ZL,VbattH  

 ld r16,Z  

 rcall CONVASCIIBIN  

 ldi ZL,VbattL  

 ld r16,Z  

 rcall CONVASCIIBIN  

 ldi R19,' '  

 rcall INVIA  

 ;TENSIONE POTENZIOMETRO  

 ldi ZH,0  

 ldi ZL,VpoteH  

 ld r16,Z  

 rcall CONVASCIIBIN  

 ldi ZL,VpoteL  

 ld r16,Z  

 rcall CONVASCIIBIN  

 ldi R19,' '  

 rcall INVIA  

 mov r16,stato  

 rcall CONVASCIIBIN  

 ldi R19,' '  

 rcall INVIA


/* //LEGGO DATI PRECEDENTI  

 ldi r17,3 ;contatore  

 ldi ZH,0x00 ;puntatore alto  

 ldi ZL,ContPSU ;puntatore basso  

 //EEPROM\_read  

 sbic EECR,EEWE ;prox operazione? La lettura Ã¨ immediata non c'Ã¨ bisogno di questo controllo  

 rjmp PC-1  

 out EEARH, ZH ; Set up address  

 out EEARL, ZL  

 sbi EECR,EERE ; Start eeprom read by writing EERE  

 in r16,EEDR ;Read data from data register  

 rcall CONVASCIIDEC  

 ldi R19,' '  

 rcall INVIA  

 inc ZL  

 dec r17  

 brne PC-11 ;*/


 rjmp Main


;====================================================================  

// INTERRUPT INT0  

;====================================================================  

EXT\_INT0: ;Caricabatterie inserito o disinserito (STEP DOWN ON/OFF)  

 push r16  

 push r17  

 push ZL  

 push ZH  

 in r16,SREG  

 push r16  

 ;controlla se Caricabatterie inserito (STEP DOWN ON)  

 sbis PIND,2  

 rjmp loop\_EXT\_INT0\_10  

 sbrc stato,7 ;se stato[7]=0 possiamo modificare lo stato  

 rjmp loop\_EXT\_INT0\_10  

 /*;inverti interrupt fronte discesa  

 in r16,MCUCR  

 cbr r16,1 ;b0=0  

 sbr r16,2 ;b1=1  

 out MCUCR,r16 ;MCUCR=xxxxxx10b*/  

 ;inverti interrupt soglia bassa  

 in r16,MCUCR  

 cbr r16,1 ;b0=0  

 cbr r16,2 ;b1=0  

 out MCUCR,r16 ;MCUCR=xxxxxx00b  

 ;aggiorna variabile stato  

 sbr stato,1  

 cbr stato,2  

 rjmp loop\_EXT\_INT0\_20  

loop\_EXT\_INT0\_10:  

 ;controlla se Caricabatterie disinserito (STEP DOWN OFF)  

 sbic PIND,2  

 rjmp loop\_EXT\_INT0\_20  

 /*;inverti interrupt fronte salita  

 in r16,MCUCR  

 sbr r16,1 ;b0=1  

 sbr r16,2 ;b1=1  

 out MCUCR,r16 ;MCUCR=xxxxxx11b*/  

 ;inverti interrupt soglia alta  

 in r16,MCUCR  

 sbr r16,1 ;b0=1  

 cbr r16,2 ;b1=0  

 out MCUCR,r16 ;MCUCR=xxxxxx01b  

/* ;SPENGO PMOS-CARICA (PWM2 OFF)  

 cbi DDRB,3  

 cbi PORTB,3  

 ldi r16,0x00  

 out OCR2,r16  

 ldi r16,0x00 ;disable PWM 8-bit,  

 out TCCR2,r16 ;*/  

 ;aggiorna variabile stato  

 cbr stato,1  

 ;SE SWITCH ERA CHIUSO  

 sbic PIND,3  

 rjmp loop\_EXT\_INT0\_20  

/* ;ACCENDO NMOS-LED (PWM1 ON)  

 sbi DDRB,1  

 cbi PORTB,1  

 ldi ZH,0  

 ldi ZL,VpoteL  

 ld r16,Z+  

 ld r17,Z+  

 out OCR1AH,r17 ;set PWM value  

 out OCR1AL,r16 ;set PWM value  

 ldi r16,val\_TCCR1A ;PWM 10-bit; b2,b1,b0=prescale  

 out TCCR1A,r16  

 ldi r16,val\_TCCR1B ;b2,b1,b0 prescale clk/8=010 pag102  

 out TCCR1B,r16 ;*/  

 ;aggiorna variabile stato  

 sbr stato,2  

loop\_EXT\_INT0\_20:  

 pop r16  

 out SREG,r16  

 pop ZH  

 pop ZL  

 pop r17  

 pop r16  

 ;riabilita interrupt globali  

 reti  

;====================================================================  

// INTERRUPT INT1  

;====================================================================  

EXT\_INT1: ;LED acceso o spento (STEP UP ON/OFF)  

 push r16  

 push r17  

 push ZL  

 push ZH  

 in r16,SREG  

 push r16  

 ;controlla se interruttore aperto (SWITCH OFF)  

 sbis PIND,3  

 rjmp loop\_EXT\_INT1\_10  

 /*;inverti interrupt fronte discesa  

 in r16,MCUCR  

 cbr r16,4 ;b2=0  

 sbr r16,8 ;b3=1  

 out MCUCR,r16 ;MCUCR=xxxx10xxb*/  

 ;inverti interrupt livello basso  

 in r16,MCUCR  

 cbr r16,4 ;b2=0  

 cbr r16,8 ;b3=0  

 out MCUCR,r16 ;MCUCR=xxxx00xxb  

 cbr stato,2  

 rjmp loop\_EXT\_INT1\_20  

loop\_EXT\_INT1\_10:  

 ;controlla se interruttore chiuso (SWITCH ON)  

 sbic PIND,3  

 rjmp loop\_EXT\_INT1\_20  

 /*;inverti interrupt fronte salita  

 in r16,MCUCR  

 sbr r16,4 ;b2=1  

 sbr r16,8 ;b3=1  

 out MCUCR,r16 ;MCUCR=xxxx11xxb*/  

 ;inverti interrupt livello alto  

 in r16,MCUCR  

 sbr r16,4 ;b2=1  

 cbr r16,8 ;b3=0  

 out MCUCR,r16 ;MCUCR=xxxx01xxb  

 ;Prima di accendere il led devo verificare che non ci sia giÃ  il caricatore attivo  

 sbis PIND,2  

 rjmp loop\_EXT\_INT1\_15 ;carica batterie spento  

 ;carica batterie acceso allora mantieni led spento ed esci  

 ;aggiorna variabile stato  

 sbr stato,1 ;caricabatterie acceso  

 cbr stato,2 ;led spento ;  

 rjmp loop\_EXT\_INT1\_20  

loop\_EXT\_INT1\_15:  

 ;aggiorna variabile stato  

 cbr stato,1 ;caricabatterie spento  

 sbr stato,2 ;led acceso  

 rjmp loop\_EXT\_INT1\_20  

loop\_EXT\_INT1\_20:  

 ;riabilita interrupt globali  

 pop r16  

 out SREG,r16  

 pop ZH  

 pop ZL  

 pop r17  

 pop r16  

 reti  

//////////////////////////////////////////  

// TIM0\_OVF aggiornamento orologio interno  

// durata istruzioni per aggiornamento microsecondi = 24 usec  

// durata istruzioni per aggiornamento ora = 35,25 usec  

// durata tra un TIM0\_OVF e un altro TIM0\_OVF = 512 usec =0x0200  

TIM0\_OVF:  

 ;disabilita interrup globale  

 push r16  

 push r17  

 push r18  

 push r19  

 push r20  

 push r21  

 push r22  

 push r23  

 push ZL  

 push ZH  

 in r16,SREG  

 push r16  

 ;carico tutte le variabili dalla ram ai registriper le operazioni necessarie di calcolo  

 ldi ZH,0  

 ldi ZL,microsecL  

 ;carico il valore precedente  

 ld r16,Z+ ;r16=microsecL  

 ld r17,Z+ ;r17=microsecH  

 ld r18,Z+ ;r18=millisecL  

 ld r19,Z+ ;r19=millisecH  

 ld r20,Z+ ;r20=secondi  

 ld r21,Z+ ;r21=minuti  

 ld r22,Z+ ;r22=ore  

 ;e lo incremento della durata di Timer0 = 512 microsec(255+255+2)  

 ldi r23,255  

 clc  

 adc r16,r23  

 brcc PC+2 ;se r16>255 incrementa r17  

 inc r17  

 ldi r23,255  

 clc  

 adc r16,r23  

 brcc PC+2 ;se r16>255 incrementa r17  

 inc r17  

 ldi r23,2  

 clc  

 adc r16,r23  

 brcc PC+2 ;se r16>255 incrementa r17  

 inc r17  

TIM0\_OVF\_micro:  

 ;if(microsec>=1000)  

 cpi r17,0b00000011 ;0x03  

 brlo TIM0\_OVF\_save  

 brne PC+3 ;se Ã¨ maggiore deve sottrarre 1000  

 cpi r16,0b11101000 ;0xE8  

 brlo TIM0\_OVF\_save  

 ;microsec=microsec-0x03E8;  

 clc  

 subi r17,0x03  

 subi r16,0xE8  

 brcc PC+2 ;r16>=E8  

 dec r17 ;r16
 ;millisec++;  

 inc r18  

 ;if(millisecL>255)millisecH++;  

 brne PC+2  

 inc r19  

 ;if(millisec>=1000)  

 cpi r19,0b00000011 ;0x03  

 brlo TIM0\_OVF\_micro  

 brne PC+3 ;se Ã¨ maggiore azzera r19  

 cpi r18,0b11101000 ;0xE8  

 brlo TIM0\_OVF\_micro  

 ;millisec=millisec-0x03E8;  

 clc  

 subi r19,0x03  

 subi r18,0xE8  

 brcc PC+2 ;r18>=E8  

 dec r19 ;r18
 ;sec++;  

 inc r20  

 ;if(sec>=60)  

 cpi r20,0b00111100 ;0x3C  

 brlo TIM0\_OVF\_micro  

 ;sec=sec-0x3C;  

 clc  

 subi r20,0x3C ;clr r20  

 ;min++;  

 inc r21  

 ;if(min>=60);  

 cpi r21,0b00111100 ;0x3C  

 brlo TIM0\_OVF\_micro  

 ;min=min-0x3C;  

 clc  

 subi r21,0x3C ;clr r21  

 ;ore++;  

 inc r22


TIM0\_OVF\_save:  

 ;puntatore alla variabile ore per il salvataggio finale  

 ldi ZH,0  

 ldi ZL,ore  

 inc ZL


/* ;puntatorre alla variabile ram microsecL  

 ldi ZH,0  

 ldi ZL,microsecL  

 ;carico il valore precedente  

 ld r16,Z+ ;r16=microsecL  

 ld r17,Z+ ;r17=microsecH  

 ;e lo incremento della durata di Timer0 = 512 microsec(255+255+2)  

 ldi r23,255  

 clc  

 adc r16,r23  

 brcc PC+2 ;se r16>255 incrementa r17  

 inc r17  

 ldi r23,255  

 clc  

 adc r16,r23  

 brcc PC+2 ;se r16>255 incrementa r17  

 inc r17  

 ldi r23,2  

 clc  

 adc r16,r23  

 brcc PC+2 ;se r16>255 incrementa r17  

 inc r17  

;clc ;quando microsecondi=1000 bisogna incrementare un millisec  

 cpi r17,0b00000011 ;0x03  

 brlo loop\_microsec  

 brne PC+3 ;se Ã¨ maggiore deve sottrarre 1000  

 cpi r16,0b11101000 ;0xE8  

 brlo loop\_microsec  

 ;devo sottrarre il valore attuale in R17,R16 con 0x03E8  

 clc  

 subi r17,0x03  

 subi r16,0xE8  

 brcc PC+3 ;r16>E8  

 breq PC+2 ;r16=E8-------------------------------------------------------------------------------------------------------------------------------------  

 dec r17 ;r16
 ;devo incrementare millisec  

 ld r18,Z+ ;r18=millisecL  

 ld r19,Z+ ;r19=millisecH  

 inc r18  

 ;se r18>255 incrementa r19  

 brne PC+2  

 inc r19  

;clc ;quando millisecondi=1000 bisogna incrementare un secondo  

 cpi r19,0b00000011 ;0x03  

 brlo loop\_millisec  

 brne PC+3 ;se Ã¨ maggiore azzera r19  

 cpi r18,0b11101000 ;0xE8  

 brlo loop\_millisec  

 ;devo sottrarre il valore attuale in R19,R18 con 0x03E8  

 clr r19 ;subi r19,0x03  

 clr r18 ;subi r18,0xE8  

 ;devo incrementare secondi  

 ld r20,Z+ ;r20=secondi  

 inc r20  

 ;se secondi(r20)>=60 [inc minuti(r21) e secondi(r20)=0]  

 cpi r20,0b00111100 ;0x3C  

 brne loop\_secondi  

 clr r20 ;subi r20,0x3C  

 ld r21,Z+ ;r21=minuti  

 inc r21  

 ;se minuti(r21)>=60 [inc ore(r22) e minuti(r21)=0]  

 cpi r21,0b00111100  

 brne loop\_minuti  

 clr r21 ;subi r21,0x3C  

 ld r22,Z+ ;r22=ore  

 inc r22  

*/


 ;aggiorniamo in ram  

loop\_ore:  

 st -Z,r22  

loop\_minuti:  

 st -Z,r21  

loop\_secondi:  

 st -Z,r20  

loop\_millisec:  

 st -Z,r19  

 st -Z,r18  

loop\_microsec:  

 st -Z,r17  

 st -Z,r16  

loop\_TIM0\_OVF90:  

 ;riabilita interrupt globali  

 pop r16  

 out SREG,r16  

 pop ZH  

 pop ZL  

 pop r23  

 pop r22  

 pop r21  

 pop r20  

 pop r19  

 pop r18  

 pop r17  

 pop r16  

 reti  

;====================================================================  

// SUBROUTINES  

;====================================================================  

;RITARDO  

delay: //delay 2,463834s@4MHz  

 ldi r16, 255  

 ldi r17, 255  

 ldi r18, 50  

Loop\_delay:  

 dec r16 ; fast count  

 brne Loop\_delay  

 dec r17 ; medium count  

 brne Loop\_delay  

 dec r18 ; slow count  

 brne Loop\_delay  

 ret ; done


;RICEVI USART  

;OUT= R19  

RICEVI:  

 sbis UCSRA, RXC  

 rjmp RICEVI  

 in r19, UDR  

 ret


;INVIA USART  

;IN= R19  

INVIA:  

; Wait for empty transmit buffer  

 sbis UCSRA,UDRE  

 rjmp INVIA  

 out UDR,r19  

 ret


;Conversione ASCII BINARIO  

;IN= R16  

CONVASCIIBIN:  

 push r16  

 push r19  

;if R16>128  

 cpi r16,128  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,128  

 rjmp loop\_convasciibin10  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin10:  

;if R16>64  

 cpi r16,64  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,64  

 rjmp loop\_convasciibin20  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin20:  

;if R16>32  

 cpi r16,32  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,32  

 rjmp loop\_convasciibin30  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin30:  

;if R16>16  

 cpi r16,16  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,16  

 rjmp loop\_convasciibin40  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin40:  

;if R16>8  

 cpi r16,8  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,8  

 rjmp loop\_convasciibin50  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin50:  

;if R16>4  

 cpi r16,4  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,4  

 rjmp loop\_convasciibin60  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin60:  

;if R16>2  

 cpi r16,2  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,2  

 rjmp loop\_convasciibin70  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin70:  

;if R16>1  

 cpi r16,1  

 brmi PC+5  

 ldi r19,49 ;'1'  

 rcall INVIA  

 subi r16,1  

 rjmp loop\_convasciibin80  

 ldi r19,48 ;'0'  

 rcall INVIA  

loop\_convasciibin80:  

 pop r19  

 pop r16  

 ret


;Conversione ascii DEC  

;IN= R16  

CONVASCIIDEC:  

 push r16  

 push r19  

 push r20  

 push r21  

 push r22  

 clr r20 ;centinaia  

 clr r21 ;decine  

 clr r22 ;unitÃ   

;if r16>=100  

 cpi r16,100  

 brmi loop\_convasciidec10  

 inc r20  

 subi r16,100  

 rjmp PC-4  

loop\_convasciidec10:  

;if R16>=10  

 cpi r16,10  

 brmi loop\_convasciidec20  

 inc r21  

 subi r16,10  

 rjmp PC-4  

loop\_convasciidec20:  

;if R16>1  

 cpi r16,1  

 brmi loop\_convasciidec30  

 inc r22  

 subi r16,1  

 rjmp PC-4  

loop\_convasciidec30:  

 ;traslazione ascii  

 ldi r16,48  

 add r20,r16  

 add r21,r16  

 add r22,r16  

 ;invio valori  

 ;se vale 0 non lo visualizzo  

 cpi r20,48  

 breq PC+3  

 mov r19,r20  

 rcall INVIA  

 mov r19,r21  

 rcall INVIA  

 mov r19,r22  

 rcall INVIA  

 pop r22  

 pop r21  

 pop r20  

 pop r19  

 pop r16  

 ret


;Conversione ascii HEX  

;IN= R16  

CONVASCIIHEX:  

 push r16  

 push r19  

 push r21  

 push r22  

 clr r21 ;decine  

 clr r22 ;unitÃ   

;if r16>=16  

 cpi r16,16  

 brmi loop\_convasciihex10  

 inc r21  

 subi r16,16  

 rjmp PC-4  

loop\_convasciihex10:  

;if R16>=1  

 cpi r16,1  

 brmi loop\_convasciihex20  

 inc r22  

 subi r16,1  

 rjmp PC-4  

loop\_convasciihex20:  

 ;traslazione ascii  

 ;if r21>=A  

 cpi r21,10  

 brmi PC+4  

 ldi r16,55 ;corrisponde allo A ascii  

 add r21,r16  

 rjmp loop\_convasciihex30  

 ldi r16,48 ;corrisponde alla 0 ascii  

 add r21,r16  

loop\_convasciihex30:  

 ;if r22>=A  

 cpi r22,10  

 brmi PC+4  

 ldi r16,55 ;corrisponde allo A ascii  

 add r22,r16  

 rjmp loop\_convasciihex40  

 ldi r16,48 ;corrisponde alla 0 ascii  

 add r22,r16  

loop\_convasciihex40:  

 ;invio valori  

 mov r19,r21  

 rcall INVIA  

 mov r19,r22  

 rcall INVIA  

 pop r22  

 pop r21  

 pop r19  

 pop r16  

 ret


;Conversione ascii -> Tensione 5Volt=0x3ff e 0Volt=0x000  

;la misura Ã¨ a 10 bit 1=0,0048828125Volt  

;IN= R16,R17  

CONVASCIIVOLT:  

 push r16  

 push r17  

 push r18 ;x,xx00 Volt  

 push r19 ;x,00xx Volt  

 push r20 ;0,xxxx Volt  

 clr r20  

 clr r19  

 clr r18  

loop\_convasciivolt10:  

 ;condizione per uscire dal loop  

 cpi r16,0  

 brne loop\_convasciivolt20 ;decrementa  

 cpi r17,0 ;controlla la parte bassa  

 brne loop\_convasciivolt20 ;decrementa  

 rjmp loop\_convasciivolt90 ;esci  

loop\_convasciivolt20:  

 ;decrementa valore  

 cpi r16,0  

 brne loop\_convasciivolt30  

 ;devo decrementare r17 e r16  

 dec r17  

loop\_convasciivolt30:  

 dec r16  

 ;incrementa  

 ldi r21,unitavolt ;costante da incrementare  

 add r18,r21  

 cpi r18,100  

 ;se r18 Ã¨ maggiore o uguale a 100 dobbiamo incrementare r19  

 breq PC+2  

 brmi loop\_convasciivolt10  

 subi r18,100  

 inc r19  

 cpi r19,100  

 ;se r19 Ã¨ maggiore o uguale a 100 dobbiamo incrementare r20  

 breq PC+2  

 brmi loop\_convasciivolt10  

 subi r19,100  

 inc r20  

 rjmp loop\_convasciivolt10  

loop\_convasciivolt90:  

 ;stampiamo i valori  

 mov r16,r20  

 rcall CONVASCIIDEC  

 mov r16,r19  

 ldi r19,','  

 rcall invia  

 rcall CONVASCIIDEC  

; mov r16,r18  

; rcall CONVASCIIDEC  

 ldi r19,'V'  

 rcall invia  

 ldi r19,'o'  

 rcall invia  

 ldi r19,'l'  

 rcall invia  

 ldi r19,'t'  

 rcall invia  

 pop r20  

 pop r19  

 pop r18  

 pop r17  

 pop r16  

 ret


//////////////////////////////////////////////////////////////////////////////////////////////////  

// ADCRESULT = acquisisce il valore lo inserisce nel buffer, e restituisce in r16 la media pesata  

//  

// IN: YH; YL; STATO  

// OUT: R16=(ram[0x80]+ram[0x81]+ram[0x82]+ram[0x83]+...ram[0x8F])/16; STATO  

// ha una durata di 141,75usec  

//////////////////////////////////////////////////////////////////////////////////////////////////  

ADCRESULT:  

 ;enable ADC and initialization first conversion  

 ldi R16, 0b11000001 ;prescale b2b1b0 000=2;001=2;010=4;011=8;100=16;101=32;110=64;111=128  

 out ADCSRA, R16  

;| ADEN | ADSC | ADFR | ADIF | ADIE |ADPS2|ADPS1|ADPS0|Prescaler Select Bits  

;|ADCEnable|StrtCnv|FreeRun|IntFlag|IntEnable| prescale  

 ;end conversion 25cycles  

 sbic ADCSRA,ADSC  

 rjmp PC-1  

 ;start again 25cycles  

 sbi ADCSRA,ADSC  

 ;end conversion  

 sbic ADCSRA,ADSC  

 rjmp PC-1  

 ;carica il volore in r16  

 in r17,ADCL  

 in r18,ADCH  

 ;disable ADC  

 ldi R16, 0b00010000  

 out ADCSRA, R16  

; ldi XH, 0x00  

; ldi XL,VtempL  

; st X+,r17 ;carico il valore attuale BASSO nella variabile VtempL  

; st X,r18 ;carico il valore attuale ALTO nella variabile VtempH  

 //ELIMINO LE MICRO OSCILLAZIONI  

 ldi r23,soglialuce ;carico il valore soglia basso in r23  

 mov r22,r18 ;valore attuale alto in r22  

 mov r21,r17 ;valore attuale basso in r21  

 ld r20,Y ;carico il valore acquisito precedente alto in r20  

 ldd r19,Y+1 ;carico il valore acquisito precedente basso in r19  

 ;se stato[2]==0 scendo  

 sbrc stato,2  

 rjmp loop\_adcresult50  

 ;confronto valore attuale ALTO con quello memorizzato precedentemente alto  

 cp r22,r20  

 ;se il valore acquisito attualmente ALTO Ã¨ uguale rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 breq PC+3 ;uguale  

 ;se il valore acquisito attualmente ALTO Ã¨ minore rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 brlo loop\_adcresult80  

 ;se il valore acquisito attualmente ALTO Ã¨ maggiore rispetto a quello precedentemente acquisito DIPENDE DA COME Ã© LA SOGLIA  

 rjmp loop\_adcresult10  

 ;confronto valore attuale BASSO con quello memorizzato precedentemente basso  

 cp r21,r19  

 ;se il valore acquisito attualmente BASSO Ã¨ uguale rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 breq loop\_adcresult80  

 ;se il valore acquisito attualmente BASSO Ã¨ minore rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 brlo loop\_adcresult80  

 ;se il valore acquisito attualmente BASSO Ã¨ maggiore rispetto a quello precedentemente acquisito DIPENDE DA COME Ã© LA SOGLIA  

 rjmp loop\_adcresult10  

loop\_adcresult10:  

 push r22  

 push r21  

 clc  

 sub r22,r20 ;valore assoluto in r22:r21  

 sub r21,r19  

 brcc PC+2  

 dec r22  

 ;vediamo se cambia stato  

 cpi r22,0 ;se r22>0 allora cambia la direzione  

 brne PC+6  

 cp r21,r23  

 pop r21  

 pop r22  

 breq loop\_adcresult70 ;lo stato rimane uguale al precedente e il memoriziamo il valore PRECEDENTE  

 brlo loop\_adcresult70 ;lo stato rimane uguale al precedente e il memoriziamo il valore PRECEDENTE  

 ;lo stato cambia in 1  

 sbr stato,4  

 rjmp loop\_adcresult80 ;lo stato cambia e il memoriziamo il valore ATTUALE  

loop\_adcresult50:  

 ;se stato[2]==1 salgo  

 sbrs stato,2  

 rjmp loop\_adcresult90  

 ;confronto valore attuale con quello memorizzato precedentemente  

 ;confronto valore attuale ALTO con quello memorizzato precedentemente alto  

 cp r22,r20  

 ;se il valore acquisito attualmente ALTO Ã¨ uguale rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 breq PC+3  

 ;se il valore acquisito attualmente ALTO Ã¨ minore rispetto a quello precedentemente acquisito DIPENDE DA COME Ã© LA SOGLIA  

 brlo loop\_adcresult60  

 ;se il valore acquisito attualmente ALTO Ã¨ maggiore rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 rjmp loop\_adcresult80  

 ;confronto valore attuale BASSO con quello memorizzato precedentemente basso  

 cp r21,r19  

 ;se il valore acquisito attualmente BASSO Ã¨ uguale rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 breq loop\_adcresult80  

 ;se il valore acquisito attualmente BASSO Ã¨ minore rispetto a quello precedentemente acquisito DIPENDE DA COME Ã© LA SOGLIA  

 brlo loop\_adcresult60  

 ;se il valore acquisito attualmente BASSO Ã¨ maggiore rispetto a quello precedentemente acquisito memorizza il valore ATTUALE  

 rjmp loop\_adcresult80  

loop\_adcresult60:  

 push r20  

 push r19  

 clc  

 sub r20,r22 ;valore assoluto in r20:r19  

 sub r19,r21  

 brcc PC+2  

 dec r20  

 ;vediamo se cambia stato  

 cpi r20,0 ;se r20>0 allora cambia la direzione  

 brne PC+6  

 cp r19,r23  

 pop r19  

 pop r20  

 breq loop\_adcresult70 ;lo stato rimane uguale al precedente e il memoriziamo il valore PRECEDENTE  

 brlo loop\_adcresult70 ;lo stato rimane uguale al precedente e il memoriziamo il valore PRECEDENTE  

 ;lo stato cambia in 0  

 cbr stato,4  

 rjmp loop\_adcresult80 ;lo stato cambia e il memoriziamo il valore ATTUALE  

loop\_adcresult70:  

 ;memoriziamo il valore PRECEDENTE  

 mov r18,r20  

 mov r17,r19  

 rjmp loop\_adcresult90  

loop\_adcresult80:  

 ;memoriziamo il valore ATTUALE  

 mov r18,r22  

 mov r17,r21  

 rjmp loop\_adcresult90  

/////////R18;r17  

loop\_adcresult90:  

 //////////////////////////////////////////////////////  

 /////////SHIFTO il buffer per il calcolo della media  

 //////////////////////////////////////////////////////  

 ldi r19,dimbuff-3 ;contatore, mi posizione sul penultimo valore e lo sposto all'ultimo  

 add YL,r19  

 inc r19 ;correzione per un corretto numero dei valori nel buffer  

 ld r16,Y ;carica il valore in r16  

 std Y+2,r16  

 dec YL  

 dec r19  

 brne PC-4  

 ;carico il valore appena acquisito nella pila del buffer media  

 inc YL  

 st Y,R18  

 std Y+1,R17  

 /////////////////////////////////////////////////////////////  

 /////////MEDIA PESATA: il risultato deve stare in r18 e r17  

 /////////////////////////////////////////////////////////////  

 ldi r19,dimbuff-2 ;contatore per la somma  

 ;somma parte bassa con il successivo  

 ldd r16,Y+3  

 clc  

 adc r17,r16  

 brcc PC+2  

 inc r18  

 ;somma parte alta con il successivo  

 ldd r16,Y+2  

 clc  

 adc r18,r16  

 ;sposto il puntatore  

 inc YL  

 inc YL  

 ;condizione di ciclo  

 dec r19  

 dec r19  

 brne PC-12  

 ;ora faccio la divisione  

 clc  

 ror r18  

 ror r17  

 clc  

 ror r18  

 ror r17  

 clc  

 ror r18  

 ror r17  

 clc  

 ror r18  

 ror r17  

 ret


/////////////////////////////////////////////////////  

// resetRam evita che la luce oscilli all'accensione  

// durata 124usec  

/////////////////////////////////////////////////////  

resetRam:  

 ;resetto le variabili utilizzate per la RAM  

 ldi ZH,0  

 ldi ZL,VbattL  

 ldi r17,32 ;variabile counter  

 clr r16  

 st Z+,r16  

 dec r17  

 brne PC-2  

 ;il buffer mediabatt  

 ldi ZH, mediabattYH  

 ldi ZL, mediabattYL  

 ldi r17,dimbuff ;variabile counter  

 clr r16  

 st Z+,r16  

 dec r17  

 brne PC-2  

 ;il buffer mediapote  

 ldi ZH, mediapoteYH  

 ldi ZL, mediapoteYL  

 ldi r17,dimbuff ;variabile counter  

 clr r16  

 st Z+,r16  

 dec r17  

 brne PC-2  

 ret


LED\_OFF:  

 cbi PORTD,5  

 cbi PORTD,6  

 cbi PORTD,7  

 cbi PORTB,0  

 ret


LED\_RED:  

 cbi PORTB,0  

 cbi PORTD,7  

 cbi PORTD,5  

 sbi PORTD,6  

 ret


LED\_YELLOW:  

 cbi PORTB,0  

 cbi PORTD,7  

 cbi PORTD,6  

 sbi PORTD,5  

 ret


LED\_GREEN2:  

 cbi PORTB,0  

 cbi PORTD,5  

 cbi PORTD,6  

 sbi PORTD,7  

 ret


LED\_GREEN1:  

 cbi PORTD,7  

 cbi PORTD,5  

 cbi PORTD,6  

 sbi PORTB,0  

 ret

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
