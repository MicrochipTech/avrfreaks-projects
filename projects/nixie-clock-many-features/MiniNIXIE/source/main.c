//
// MININIXIE
//
// Nixie clock with ATtiny2313
//
// Copyright (C) 2008, 2009  M. Baudisch, F. Bösing
//
// Key debouncing: Author Peter Dannegger
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/*
   8 Mhz, interner Oszillator.

   !!! Achtung, nicht vergessen die Fuse "ckdiv8" auf "unprogrammed" zu setzen.
   !!! BrownOut Fuse auf 4,3 V setzen (sonst Datenverlust im EEPROM möglich )
    (bei Akku Backup auf 2.7 V)

   (LFUSE: 0xe4, HFUSE: 0xd9) Brownout 4.3 V
   (LFUSE: 0xe4, HFUSE: 0xdb) Brownout 2.7 V

   Compiler: WinAvr 20070525 (GCC 4.1.2)
   CFLAGS    += -mint8
                      -ffreestanding


   PINBELEGUNG:


   PA0 : Speaker                    (Ausgang)
   PA1 : PIR                        (Eingang)

   PB7 : D - 74141                  (Ausgang)
   PB6 : C - 74141                  (Ausgang)
   PB5 : B - 74141                  (Ausgang)
   PB4 : A - 74141                  (Ausgang)
   PB3 : Frequenzeingang (1Hz-64Hz) (Eingang)
   PB2 : PWM Anodenspannung         (Ausgang)
   PB1 : Messung Anodenspannung     (Eingang)
   PB0 : Sekundenblinker            (Ausgang)

   PD6 : gemultiplexte Taste        (Eingang)
   PD5 : Anode Nixie 6 & Taste 3    (Ausgang)      Zehner Stunden
   PD4 : Anode Nixie 5 & Taste 2    (Ausgang)      Einer Stunden
   PD3 : Anode Nixie 4 & Taste 1    (Ausgang)      Zehner    Minuten
   PD2 : Anode Nixie 3              (Ausgang)      Einer Minuten
   PD1 : Anode Nixie 2              (Ausgang)      Zehner    Sekunden
   PD0 : Anode Nixie 1              (Ausgang)      Einer Sekunden

 */

#define BUILDDD 12               // Datum der Erstellung, wird als akt. Datum vorgegeben
#define BUILDMM 9
#define BUILDYY 9



#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "main.h"                    //Diverse Definitionen
#include "interrupts.c"              //Interrupt-Routinen. Multiplexing usw. - ok schlechter Stil


//Routinen für platzsparende BCD-Umwandlung
/*==================================================================================*/

extern uint16_t udiv8 (uint8_t, uint8_t) asm ("__udivmodqi4");

static inline uint16_t swap_16 (uint16_t x_in)
{
	asm volatile (
	        "eor %A0, %B0"   "\n\t"
	        "eor %B0, %A0"   "\n\t"
	        "eor %A0, %B0"
		: "=r" (x_in)
		: "0"  (x_in)
	        );

	return x_in;
}


static uint16_t bcd(uint8_t wert)
{

	return swap_16 (udiv8(wert,10));

}

//Für Tastenentprellung
/*==================================================================================*/
//Author: Peter Dannegger
static uint8_t get_key_press( uint8_t key_mask ) {

	key_mask &= key_press;        // read key(s)
	key_press ^= key_mask;        // clear key(s)

	return key_mask;
}


//Speichere Konfiguration in EEPROM
/*==================================================================================*/
static void speichere_ee (void)      //Speichert Datum, Alarm-Zeit und Konfiguration im EEprom
{

	eeprom_write_block(&t.datum, &ee_uhr.datum,
	                   sizeof(ee_uhr.datum) +
	                   sizeof(ee_uhr.al_zeit) +
	                   sizeof(ee_param_setup_conf)
	                   );
}

//Lies Konfiguration aus EEPROM
/*==================================================================================*/
static inline void lese_ee (void) //Liest Zeit, Datum, Alarm-Zeit und Konfiguration aus dem EEprom
{

	eeprom_read_block(&t, &ee_uhr,
	                  sizeof(ee_uhr) +
	                  sizeof(ee_param_setup_conf)
	                  );

}


//Initialisierung der Uhr
/*==================================================================================*/
static inline void init(void)
{

	lese_ee();

	//Setup Ports :


	DDRA  = SPEAKER;
	PORTA = PIR; // Pullup-Widerstand für PIR-Eingang aktivieren.

	DDRB = (1 << PB7) | (1 << PB6) | (1 << PB5) | (1 << PB4) | (1 << PB2) | (1 << PB0);
	DDRD = (1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2) | (1 << PD1) | (1 << PD0);


	//Vorteiler 8,    also 1 Mhz Takt    für 16-BIT Timer 1 :

	TCCR1B = (0 << CS12) | (1 << CS11) | (0 << CS10);

	//Setup PWM für 8-BIT Timer 0:    Kein Vorteiler,    also 8 Mhz Takt    :
	//( "fast-pwm" , "non-inverting Mode", "no prescaling" )
 
	#define _TCCR0A (1 <<  WGM01) | (1 <<  WGM00) | (1 << COM1A1) | (0 << COM1A0)
	#define _TCCR0B (0 <<  WGM02) | (0 <<  CS02 ) | (0 <<   CS01) | (1<<CS00);

	TCCR0A = _TCCR0A;
	TCCR0B = _TCCR0B;

	//Setup Analog    Comparator:

	ACSR = (1 << ACBG);                         //Interne Referenz einschalten,

	//Interrupt-Einstellungen :

	PCMSK = (1 << PCINT3);                       //Nur für PCINT3 :
	GIMSK = (1 <<   PCIE);                       //Pin Change Interrupt enable

	TIMSK = (1 << OCIE1A) | (1 << OCIE1B);       //Compare-Interrupts für Timer 1 einschalten

	MCUCR = (1 << SE); //Sleep enable

	hell=3; //max. Helligkeit der Röhren + Fading
	zeige_sonder=0; //explizites Setzen nötig, da in Registern!
	pir_minuten=0;
	key_state=0;
}


//Zählt eine Sekunde vorwärts (berücksichtigt Schaltjahre)
/*==================================================================================*/
static void inline timetick(void)
{

	if (++t.zeit.ss==60)
	{

		if (pir_minuten) pir_minuten--;  //PIR-Restdauer:
		if (alarm_minuten) alarm_minuten--;

		t.zeit.ss=0;
		if (++t.zeit.mm==60)
		{
			t.zeit.mm=0;

			// Werte in Temporären Vars halten verringert den Flash-Verbrauch:
			uint8_t std   =    t.zeit.hh + 1;
			uint8_t jahr  =    t.datum.jj;
			uint8_t monat =    t.datum.nn;
			uint8_t tag   =    t.datum.tt;

			uint8_t szdiff=param.szdiff;
			uint8_t sztabstart=(jahr-param.sztabyrdaystart)*2+ YEAR_TAB_OFFSET;

			uint8_t szdaystart=param.p[sztabstart];
			uint8_t szdayend  =param.p[sztabstart + 1];
			//Daten Zeitumstellung für akt. Jahr lesen


			//Sommerzeit ?
			if ((monat==param.szmonstart) &&
			    (tag==szdaystart) &&
			    (std==param.szhourstart))

			{
				std+=szdiff;
			}

			//"Winterzeit" ?
			if    (!(bits & SW) && // wurde die Zeit schon zurück gestellt ?
			       (monat==param.szmonend) &&
			       (tag==szdayend) &&
			       (std==param.szhourend))

			{

				bits |= SW;         //Flag setzen: Wir haben soeben die Zeit zurück gestellt.
				std-=szdiff;
			}


			if    (std==24)

			{
				bits &= ~SW;
				tag++;
				std=0;

				if (tag==32)
				{
					monat++;
					tag=1;
				} //(tag==32)

				else if    (tag==31)
				{
					if ((monat==4) || (monat==6) ||
					    (monat==9) || (monat==11))
					{
						monat++;
						tag=1;
					}
				} //(tag==31)

				else if    (tag==30)
				{
					if(monat==2)
					{
						monat++;
						tag=1;
					}
				} //(tag==30)

				else if    (tag==29)
				{
					if((monat==2) && (jahr % 4))
					{
						monat++;
						tag=1;
					}
				} //(tag==29)


				if (monat==13)
				{
					jahr++;
					monat=1;

				}

				t.datum.jj = jahr;
				t.datum.nn = monat;
				t.datum.tt = tag;
				speichere_ee(); // EEPROM nächtlich aktualisieren
			} //(std==24)

			t.zeit.hh = std;

		} //if (++t.zeit.mm==60)    jede Stunde
	} //if (++t.zeit.ss==60)    jede Minute

}


//Schreibt Zeit, Datum oder Alarmzeit ins Display
/*==================================================================================*/
static void zeige (const werte_t *t )
{ //wandelt die Werte in BCD um und legt sie in die display-Variable

	uint8_t i=0;
	uint8_t j=0;


	while (j<3) {

		disp.r[i] = t->werte[j] % 10;
		asm volatile ("std Z+1, r24" ::);
		i+=2;
		j++;
	}

}

//Schaltet Glimmlampe, je nach dem, ob Alarm aktiviert
/*==================================================================================*/
static void setalarmblink(void)
{
	PORTB &=~BLINK;
	if (t.al_zeit.ss) PORTB |= BLINK;
}

//Menü für Konfiguration der Uhr
/*==================================================================================*/
static inline void chk_tasten_conf (uint8_t tasten)
{


	disp_maske =    0x03;


	disp.r56 = bcd(data_idx+1);


	disp.r34 = bcd(param.p[data_idx]);

	switch (tasten) {

	case    TASTE1:

		param.p [data_idx]++;

		if (param.p [data_idx] > eeprom_read_byte(&ee_setup_conf_max.p[data_idx]))
			param.p [data_idx] = 0;
		break;

	case    TASTE2:

		data_idx++;
		if (data_idx < sizeof(param_t))
			break;                          //Alle Parameter wurden eingestellt

	case    TASTE4:

		bits_setup &= ~SETUP_CONF;
		break;

	}

	if (!(bits_setup & SETUP_CONF))                 //Konfiguration ein mal speichern
		speichere_ee();

}

//Menü für Einstellung von Zeit, Datum, Al-Zeit
/*==================================================================================*/
static inline void chk_tasten_setup (uint8_t tasten)
{

	uint8_t setup_min = 0;

	setalarmblink();  //Blinker passend setzen (im ganzen Setup - stört nicht weiter !)

	if (
	        ((bits_setup & SETUP_ALARM) && (data_idx==5)) //wenn Ende Alarmsetup
	        ||
	        (data_idx==0)) //oder Ende normales Setup

	{
		speichere_ee();
		bits_setup=0;

	} else {

		disp_maske = eeprom_read_byte(&setup_maske[data_idx]); //passende Displaymaske setzen

		if (data_idx < 3) {
			t.zeit.ss = 0;
			zeige (&t.zeit);
		} else

		if (data_idx == 3) {

			disp.r34=bcd(t.datum.jj); //Jahr nach links schieben, wegen 4-Röhren Version

		} else

		if (data_idx < 6) {
			zeige (&t.datum);
			setup_min=1;
		} else {

			zeige (&t.al_zeit);

		}

		switch (tasten)  {

		case TASTE1:

			t.data [data_idx]++;
			if (t.data [data_idx] > eeprom_read_byte(&setup_max[data_idx]))
				t.data [data_idx] = setup_min;

			if (data_idx > 6) t.al_zeit.ss=1;  //wenn Alarmzeit geändert wird, Alarm an

			break;


		case TASTE2:                               // nächste Ziffer bei Taste 2 oder 3
		case TASTE3:
			data_idx--;

			break;

		case  TASTE4:                              //Taste 2 oder Taste 3 lange

			if ((bits_setup & SETUP_ALARM)) {  //wenn Alarmsetup - Alarm an/aus
				t.al_zeit.ss^=1;           //Sekunden zw. 0/1 toggeln

				disp.r[0]=t.al_zeit.ss;    //nur letzte Röhre aktualisieren
				disp_maske=~0x3d;          //richtigen Displaymode setzen
				zeige_sonder=3;            //nach Verlassen Alarmeinst. noch 3 Sek anzeigen

			}

			else {                      //wenn normales Setup - zur Parameterconfig wechseln

				bits_setup = SETUP_CONF;
			}
			data_idx = 0; //und raus
		}
	}
}

//Menü im Normalbetrieb
/*==================================================================================*/
static inline void chk_tasten_main (uint8_t tasten)
{

	switch (tasten) {

	case TASTE1:                         //Taste1 : Modus

		if (!(bits & NACHT)) {       //bei neg. Überlauf hell wieder auf 3 setzen
			hell--;
			if (hell>3) hell=3;  //hat höhere Priorität als PIR
		}
		break;

	case  TASTE2:               //Taste2 : Stellmodus einschalten

		data_idx = 5;
		bits_setup = SETUP_MODE;
		break;

	case TASTE3:                //Taste3

		alarm_minuten=0;    //Alarm aus
		disp_maske = ~0x3d;
		zeige_sonder=3;     //3 Sekunden Alarmzeit anzeigen
		zeige (&t.al_zeit);
		setalarmblink();    //Wenn Alarm eingeschaltet, Blinker einschalten
		break;

	case TASTE4:                //Taste4 (Taste3 lang) : Alarmzeit stellen

		data_idx = 8;
		bits_setup = SETUP_ALARM; //Setupmode ein, alles andere abschalten
	}
}

//Menü-Hauptroutine.
/*==================================================================================*/
static inline void chk_tasten (void)

//Fragt PIR-Sensor und Tasten ab, springt weiter zum aktuellen Menü

{

	//Abfrage des PIR-Sensors.
	if (!(PINA & PIR) && param.pirdauer) pir_minuten = param.pirdauer;
	// nur wenn PIR aktiviert PIR-Min setzen (werden auch vom Alarmdisplay verwendet)

	cli();

	uint8_t tasten = get_key_press ( TASTE1 | TASTE2 | TASTE3 | TASTE4);

	if (bits_setup & SETUP_CONF)
		chk_tasten_conf (tasten);


	else

	if (bits_setup & (SETUP_MODE | SETUP_ALARM))
		chk_tasten_setup (tasten);

	else

		chk_tasten_main (tasten);

	sei();

}

//Simuliert eine "Slotmachine"
/*==================================================================================*/
static inline void slotmachine (void)
{

	uint8_t i=t.zeit.ss; //Die Sekunde wird benutzt, um festzustellen, welchen Ziffern stehen.

	if (i<6) {

		cli();
		for    (; i<6; i++) //alle Ziffern bis auf die stehenden um 1 weiterzählen
			if (++disp.r[i]>9) disp.r[i]=0;
		fading = 0;

		sei();
	}

	_delay_ms(17); //Damit das nicht zu schnell passiert, ein wenig warten

}

//Hauptprogramm
/*==================================================================================*/
int main (void)
{

	init ();


	for(;; ) { //Endlosschleife

		if (bits & NEUE_SEKUNDE)
		{
			bits &= ~NEUE_SEKUNDE;

			disp_old = disp; //Für Fading das "alte" Display wegspeichern.

			timetick();      //Eine Sekunde ist vergangen, Zeit & Datum weiterzählen

			if (bits_setup) goto ende;

			if (( t.al_zeit.ss ) && //Alarm eingeschaltet ?
			    ( t.al_zeit.mm == t.zeit.mm ) && //Alarm-Zeit erreicht ?
			    ( t.al_zeit.hh == t.zeit.hh ) )

			{

				if ( t.zeit.ss  == 0) //nur zu Beginn der Minute einschalten
					alarm_minuten=param.alarmdauer;  //Alarm  starten
			}

			//Nacht

			if (t.zeit.hh == param.nachtvon) bits |= NACHT;  //passt die Uhrzeit ?
			if (t.zeit.hh == param.nachtbis) bits &= ~NACHT;



			if (!zeige_sonder) {

				disp_maske=0;

				if (param.supleadzero && (disp.r[5]==0)) disp_maske=~0x1f;
				//wenn Parameter gesetzt, führende 0 unterdrücken





				if  (!(bits & NACHT) && //Nachts kein Datum anzeigen (HS ist aus!) ->bei Taste
				     (t.zeit.ss >= param.datumvon) && //passt die Zeit (Sekunden) ?
				     (t.zeit.ss <= param.datumbis) ) {



					if (param.datedispmode) {
						if (param.datedispmode==1) { //MMDDYY

							disppuffer.werte[2]=t.datum.nn;
							disppuffer.werte[1]=t.datum.tt;
							disppuffer.werte[0]=t.datum.jj;

						} else { //YYMMDD

							disppuffer.werte[2]=t.datum.jj;
							disppuffer.werte[1]=t.datum.nn;
							disppuffer.werte[0]=t.datum.tt;

						}
						zeige (&disppuffer);
					} else

						zeige (&t.datum);  // Das Datum anzeigen DDMMYY

				} else {



					if (param.blinkdauer) PORTB |= BLINK;  //Blinker an (bei Blinkdauer 0 aus)

					if (param.ampmblink==1)  { //AMPM Blinker vormittags an
						if (t.zeit.hh < 12) PORTB |= BLINK; else PORTB &= ~BLINK;
					}

					if (param.ampmblink==2)  { //AMPM Blinker nachmittags an
						if (t.zeit.hh >= 12) PORTB |= BLINK; else PORTB &= ~BLINK;
					}    //Blinker AM an

					uint8_t hhtmp;
					hhtmp=t.zeit.hh; //Stunden retten

					if ((param.timedispmode) && (hhtmp>12)) t.zeit.hh=hhtmp-12;  //ggf manipulieren

					zeige(&t.zeit);           //Zeit anzeigen
					t.zeit.hh=hhtmp;          //zurückschreiben
				}

				cli();                            //Da "Fading" gesetzt wird, Interrupts hier verbieten.
				fading=FADING_MAX;

			} else {

				zeige_sonder--;

			}

		} //if (bits & NEUE_SEKUNDE)....


		cli();

		//Display an oder aus ??

		if   ((bits & PWR_FAULT) || //bei PWR_FAULT Display immer aus
		      (
		              (
		                      ((param.pirdauer||(bits & NACHT)) //wenn PIR aktiviert oder Nachtmodus
		                       && !pir_minuten)                 //aber keine Pir-Minuten anstehen ->aus
		                      || /*oder*/

		                      !hell                             // wenn über Dim-Taste abgeschaltet -> aus
		              )
		              &&  //es sei denn
		              !(
		                      zeige_sonder  ||                  //oder Anzeige im Sondermodus -> an
		                      key_in        ||                  //oder es wurde eine Taste gedrückt -> an
		                      bits_setup                        //oder gerade Setup-Modus -> an
		                      )
		      )
		      )
		{
			TCCR0A = 0; //Anodenspannung aus

		}
		else //Anodenspannung einschalten. Vgl. auch init()

		{    TCCR0A = _TCCR0A; }

		sei();

		if (alarm_minuten)
		{                            //Alarm !!

			pir_minuten = param.alarmdispdauer; //Display ein paar Min einschalten

			if (param.speakermode) {
				bits |= ALARM;
			} else                          //Ton erzeugen

				PORTA |= SPEAKER;       //oder Port auf High setzen

		} else {

			if (    !(bits & NACHT) &&      //Slotmachine: Nicht Nachts (nur bei Tastendruck interessant -> 4 Byte Reserve)
			        (!zeige_sonder) &&      //Nicht bei "Sonderanzeige"
			        (!bits_setup)   &&      //Nicht im    Setup/Konfig
			        ((param.periodeslot) && //alle periodeslot Minuten
			         ((t.zeit.mm % param.periodeslot)==0)))
			{
				slotmachine ();
			}

			bits &=~ALARM;     //Sound aus
			PORTA &= ~SPEAKER; //Alarmausgang auf Low

		}

ende:

		asm volatile ("sleep \n\t" ::); //Alles erledigt. Bis zum nächsten Interrupt schlafen.
		chk_tasten();                   // Tasten bedienen

	} // for(;;)
}
