// 
// Anpassungen im makefile:
//    ATMega8 => MCU=atmega8 im makefile einstellen
//    lcd-routines.c in SRC = ... Zeile anhängen
// 
#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#include <stdlib.h>			// aus Platzgründen durch eigene Routinen ersetzt
#include "lcd-routines.h"
#include "num2a.h"
#include "io.h"
#include "ctcss_enc.h" 
#include "isr.h"



unsigned char key = 0;				// welche Taste wurde gedrückt
//---------------------------------------
// Für Drehgeber -	Hitachi von Pollin	-
//---------------------------------------
//  unsigned char table[4][4]={{0,1,-1,0},{0,0,0,0},{0,0,0,0},{0,-1,1,0}}; // falsch
  unsigned char table[4][4]={{0,1,-1,0},{0,0,0,0},{0,-1,1,0},{0,0,0,0}}; // richtig
  unsigned char position=5; // will hier nur wissen ob rechts oder links gedreht wurde
  volatile unsigned char quadrature_input = 0; // bit 0 und bit 1 sind Quadratureingaenge
  unsigned char new_quadrature_value, last_quadrature_value;
  unsigned char rot_has_changed = 0;			// FLAG....besser wenn nur Änderung interessiert
//----------------------------------------


//----------------------------------------
//| Globale Variable                     |
//----------------------------------------
int		T500_Status = 0;		// Für Anzeigezwecke
int		ocr_saved;
char	T_ACTIVE = 1;				// Timer ein/aus
int 	PRE;					// Vorteilerwert für Berechnung
char 	lcd_buff[8];			// LCD Puffer
float 	f_freq;					// Frequenz ans Fließkommazahl


//----------------------------------------
//| Tabelle CTCSS Töne -> Flash 1/10 Hz  |
//----------------------------------------
unsigned int mydata[39] PROGMEM =
{
	670, 719, 744, 770, 797, 825, 854, 885, 915, 948, 974, 1000,1035,1072,1109,1148,1188,1230,
	1273,1318,1365,1413,1462,1514,1567,1622,1679,1738,1799,1862,1928,2035,2107,2181,2257,2336,
	2418,2503,17500
};

//=======================================================
//= Zeitgeber0 für Hauptzeitbasis setzen  26.04.2009RM  =
//=======================================================
void init_timer(void)			// Zeitbasis
{
	TCNT0 = COUNT_0;			// Timer vorskalieren ==> defines.h
	TCCR0 |= PRESC_64;			// Prescaler Timer0 auf 8 setzen
	TIMSK |= (1<<TOIE0);		// Overflow Interrupt für Timer0 maskieren
	cnt_10ms = 0;				// Zähler für 10ms Service initialisieren
}

//=======================================================
//= Zeitgeber1 für Sub-Ton Basis setzen   28.07.2010RM  =
//=======================================================
void init_timer1(void)
{
//	TCNT1 = 1;					// Timer vorskalieren ==> unnötig....

	TCCR1A &= ~(1<<COM1A1);		// Toggle OC1A bei Match
	TCCR1A |=  (1<<COM1A0);		// Toggle OC1A bei Match
	TCCR1A |=  (1<<FOC1A);		// Force Output Compare für A

	TCCR1B |= PRESC_8;			// Vorteiler Fquartz / 8
	PRE = 8;					// Achtung, immer auf Vorteilerwert setzen
	TCCR1B |= (1<<WGM12);		// OCR1A = Obere Kippgrenze
	OCR1A = 3746;				// ok, so tuts
}

// Frequenzbestimmendes OCR1A Register berechnen
void calc_ocr(unsigned int freq)		// CTCSS - Frequenz in ZEHNTEL HERZ
{
	float 	hilf;
	int		hilf_int;
	hilf = (((F_CPU / freq)*10) / (2 * PRE)) -1;
	hilf_int = (int)hilf;
	if((hilf - hilf_int) > 0.5)
		hilf_int++;
	if(T_ACTIVE)				// Nur setzen wenn Ton eingeschaltet
		OCR1A = hilf_int;
	ocr_saved = hilf_int;		// Sonst wird er durch Flag gesetzt
}




int main(void)
{
	unsigned int tmp;
	tmp = pgm_read_word(&(mydata[38]));

	init_io();
	init_timer();			// 1ms Interrupt
	init_timer1();
    lcd_init();

	set_cursor(0,1);		// CTCSS
	lcd_string("CTCSS V1.0");
	set_cursor(0,2);		// Freq
/*	lcd_string("1750.0 Hz");
	calc_ocr(tmp);*/

	position = 39;





	 sei();					//interrrupts freigeben;
	 while(1)
	 {
		if(FLAG_10ms)
		{
			handle_10ms();
		}

		if(FLAG_500ms)
		{
			handle_500ms();
		}

		if(rot_has_changed)
		{
		set_cursor(0,2);								// 2.Zeile 1. Zeichen
		lcd_string("          ");
		tmp = pgm_read_word(&(mydata[position-1]));		// Frequenz aus Tabelle lesen
		calc_ocr(tmp);									// OCR Register berechnen und setzen
		set_cursor(0,2);								// 2.Zeile 1. Zeichen
		ftoa(lcd_buff,(float)(tmp/10.00),1);						// String zur Ausgabe vorbereiten
		lcd_string(lcd_buff);							// Und Ausgabe
		lcd_string(" Hz");								// mit Einheit
		if(position == 39)
		{
			T1PORT	|=	(1<<T1);
			T1DDR	|=	(1<<T1);
			set_cursor(15,2);
			lcd_data('H');
		}
		else
		{	
			T1PORT  &= ~(1<<T1);
			T1DDR	&= ~(1<<T1);
			set_cursor(15,2);
			lcd_data('L');
		}
			
			rot_has_changed = 0;
		}
//------------Tasten---------------------------
		key = get_debounced();
//---------Taste am Drehgeber----------Vorwahl-----------
		if( key == 1)		// Knopf am Drehgeber gedrückt, Auswahl ändern
		{	
			if(T_ACTIVE == 1)
			{
				T_ACTIVE = 0;
				OCR1A = 0;
			}
			else
			{
				T_ACTIVE = 1;
				OCR1A = ocr_saved;
			}
			set_cursor(7,1);
			key = 0;				// eigentlich unnötig, aber falls INT Überlauf
		}

//----------Rote Taste------------ Übernahme Daten------------
		if( key == 2)					// Knopf Frequenzübernahme
		{
			key = 0;
		}


	 }
     return(0);
}

// Standard Belegung von 44780 Displays
/*
Pinnummer
LCD 	Bezeichnung 	Anschluss
1 	Vss 	GND
2 	Vcc 	5V
3 	Vee 	GND oder Poti
4 	RS 	PD4 am AVR
5 	RW 	GND
6 	E 	PD5 am AVR
7 	DB0 	nicht angeschlossen
8 	DB1 	nicht angeschlossen
9 	DB2 	nicht angeschlossen
10 	DB3 	nicht angeschlossen
11 	DB4 	PD0 am AVR
12 	DB5 	PD1 am AVR
13 	DB6 	PD2 am AVR
14 	DB7 	PD3 am AVR
15 	A 	Vorsicht! Meistens nicht direkt an +5V anschließbar,
nur über einen Vorwiderstand (z.B. 33O)!
16 	K 	GND 
*/

