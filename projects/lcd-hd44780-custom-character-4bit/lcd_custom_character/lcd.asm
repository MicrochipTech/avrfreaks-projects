
.include "m32def.inc"  ;Includes the 8515 definitions file

.def temp1 = r16
.def temp2 = r17
.def temp3 = r18

	ldi r27,0xFF
	out DDRC,r27

           ldi temp1, LOW(RAMEND)      ; LOW-Byte der obersten RAM-Adresse
           out SPL, temp1
           ldi temp1, HIGH(RAMEND)     ; HIGH-Byte der obersten RAM-Adresse
           out SPH, temp1
	ldi temp1, (1<<JTD)
	out MCUCSR, temp1
	out MCUCSR, temp1				; jtag disable!!!
           ldi temp1, 0xFF    ;Port D = Ausgang
           out DDRC, temp1

           rcall lcd_init     ;Display initialisieren
           rcall lcd_clear    ;Display löschen

           ldi temp1, 'T'     ;Zeichen anzeigen
           rcall lcd_data

           ldi temp1, 'e'     ;Zeichen anzeigen
           rcall lcd_data
           
           ldi temp1, 's'     ;Zeichen anzeigen
           rcall lcd_data

           ldi temp1, 't'     ;Zeichen anzeigen
           rcall lcd_data

		   		; making a custom character
				; *****
				; *****
				; 0***0
				; 00*00
				; 00*00
				; 0***0
				; *****
				; *****

			ldi temp1, 0b01001000	; choose cg adress 1
			call lcd_command
			ldi temp1, 0b00011111	; line 1, *****
			call lcd_data
			ldi temp1, 0b00011111	; line 2, *****
			call lcd_data
			ldi temp1, 0b00001110	; line 3, 0***0
			call lcd_data
			ldi temp1, 0b00000100	; line 4, 00*00
			call lcd_data
			ldi temp1, 0b00000100	; line 5, 00*00
			call lcd_data
			ldi temp1, 0b00001110	; line 6, 0***0
			call lcd_data
			ldi temp1, 0b00011111	; line 7, *****
			call lcd_data
			ldi temp1, 0b00011111	; line 8, *****
			call lcd_data



	ldi temp1, 0xC0
	call lcd_command			; to second line on lcd

	ldi temp1, 0b00000001		; show custom character
	call lcd_data

loop:
           rjmp loop


 ;sendet ein Datenbyte an das LCD
lcd_data:
           mov temp2, temp1             ;"Sicherungskopie" für
                                        ;die Übertragung des 2.Nibbles
           swap temp1                   ;Vertauschen
           andi temp1, 0b00001111       ;oberes Nibble auf Null setzen
           sbr temp1, 1<<4              ;entspricht 0b00010000
           out PORTC, temp1             ;ausgeben
           rcall lcd_enable             ;Enable-Routine aufrufen
                                        ;2. Nibble, kein swap da es schon
                                        ;an der richtigen stelle ist
           andi temp2, 0b00001111       ;obere Hälfte auf Null setzen 
           sbr temp2, 1<<4              ;entspricht 0b00010000
           out PORTC, temp2             ;ausgeben
           rcall lcd_enable             ;Enable-Routine aufrufen
           rcall delay50us              ;Delay-Routine aufrufen
           ret                          ;zurück zum Hauptprogramm

 ;sendet einen Befehl an das LCD
lcd_command:                            ;wie lcd_data, nur ohne RS zu setzen
           mov temp2, temp1
           swap temp1
           andi temp1, 0b00001111
           out PORTC, temp1
           rcall lcd_enable
           andi temp2, 0b00001111
           out PORTC, temp2
           rcall lcd_enable
           rcall delay50us
           ret

 ;erzeugt den Enable-Puls
lcd_enable:
           sbi PORTC, 5                 ;Enable high
           nop                          ;3 Taktzyklen warten
           nop
           nop
           cbi PORTC, 5                 ;Enable wieder low
           ret                          ;Und wieder zurück                     

 ;Pause nach jeder Übertragung
delay50us:                              ;50us Pause
           ldi  temp1, $42
delay50us_:dec  temp1
           brne delay50us_
           ret                          ;wieder zurück

 ;Längere Pause für manche Befehle
delay5ms:                               ;5ms Pause
           ldi  temp1, $21
WGLOOP0:   ldi  temp2, $C9
WGLOOP1:   dec  temp2
           brne WGLOOP1
           dec  temp1
           brne WGLOOP0
           ret                          ;wieder zurück

 ;Initialisierung: muss ganz am Anfang des Programms aufgerufen werden
lcd_init:
           ldi	temp3,50
powerupwait:
           rcall	delay5ms
           dec	temp3
           brne	powerupwait
           ldi temp1, 0b00000011        ;muss 3mal hintereinander gesendet
           out PORTC, temp1             ;werden zur Initialisierung
           rcall lcd_enable             ;1
           rcall delay5ms
           rcall lcd_enable             ;2
           rcall delay5ms
           rcall lcd_enable             ;und 3!
           rcall delay5ms
           ldi temp1, 0b00000010        ;4bit-Modus einstellen
           out PORTC, temp1
           rcall lcd_enable
           rcall delay5ms
           ldi temp1, 0b00101000        ;noch was einstellen...
           rcall lcd_command
           ldi temp1, 0b00001100        ;...nochwas...
           rcall lcd_command
           ldi temp1, 0b00000100        ;endlich fertig
           rcall lcd_command
           ret

 ;Sendet den Befehl zur Löschung des Displays
lcd_clear:
           ldi temp1, 0b00000001   ;Display löschen
           rcall lcd_command
           rcall delay5ms
           ret

