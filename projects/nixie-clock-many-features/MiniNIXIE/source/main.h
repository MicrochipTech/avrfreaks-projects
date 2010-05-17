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


//Der AVR hat "General Purpose Register" zum Speichern von Werten.
//spart Programmspeicher und RAM

#define    bits           GPIOR0          //Wir speichern die Flags im GPIOR0

#define    NEUE_SEKUNDE     (1 << 0)
#define    PWR_FAULT        (1 << 1)       //1 = Stromausfall
#define    NACHT            (1 << 2)       //Nachtmodus
#define    ALARM            (1 << 3)       //Alarm-Sound an
#define    SW               (1 << 4)       //Wird benötigt, damit nur ein mal auf "Winterzeit" gestellt wird.
#define    TRIGGER_NETZFREQ (1 << 5)       //Flag für Trigger auf Netzfrequenz



#define    bits_setup     GPIOR1           //für Setup

#define    SETUP_MODE     (1 << 0)
#define    SETUP_ALARM    (1 << 1)
#define    SETUP_CONF     (1 << 2)


/*==================================================================================*/

#define    TASTE1         (1<<PD3)
#define    TASTE2         (1<<PD4)
#define    TASTE3         (1<<PD5)
#define    TASTEN_COMMON  (1<<PD6)

#define    TASTE4         (1<<0)         //"Virtuelle Taste" für langen Tastendruck
                                         // der Taste 2 oder Taste 3


#define    TASTE_LANG_DAUER   180;       // x * etwa 20ms

#define    BLINK          (1<<PB0)       //Sekundenblinker
#define    SPEAKER        (1<<PA0)
#define    PIR            (1<<PA1)



/*==================================================================================*/
//Typ-Definitionen

typedef    union { // Typdefinition für  Zeit oder Datum

	uint8_t werte[3];

	struct    {
		uint8_t ss; // Sekunden
		uint8_t mm; // Minuten
		uint8_t hh; // Stunden
	};

	struct    {
		uint8_t jj; // Jahr
		uint8_t nn; // Monat
		uint8_t tt; // Tag
	};

} werte_t;


typedef    union { // Die Zeiten und das Datum

	uint8_t data[ sizeof (werte_t) * 3 ];

	struct    {
		werte_t zeit;
		werte_t datum;
		werte_t al_zeit;
	};

} uhr_t;


typedef    union { //Das, was unmittelbar von den Nixies 1-6 dargestellt wird.

	uint8_t r[6];

	uint16_t rw[3];

	struct    {
		uint8_t r1;
		uint8_t r2;
		uint8_t r3;
		uint8_t r4;
		uint8_t r5;
		uint8_t r6;
	};

	struct    {
		uint16_t r12;
		uint16_t r34;
		uint16_t r56;

	};

} disp_t;


typedef    union { //Die einstellbaren Parameter

	struct    {
/*01*/ uint8_t timedispmode;   // Art der Zeitanzeige(0/1): 0-24h; 1-12h
/*02*/ uint8_t datedispmode;   // Art der Datumsanzeige(0..2): 0-DDMMYY; 1-MMDDYY; 2-YYMMDD
/*03*/ uint8_t supleadzero;    // Führende 0 unterdrücken(0/1): 0-wird angezeigt; 1-wird unterdrückt
/*04*/ uint8_t blinkdauer;     // Blink-Dauer (in 1/(hzmax*2) Schritten,(10 ms bei 50 Hz)): 0-aus; 0-99
/*05*/ uint8_t ampmblink;      // AM/PM Blinker (0..2): 0-aus; 1-Blinker AM an, 2-Blinker PM an
/*06*/ uint8_t periodeslot;    // Wiederholung Slotmachine(Min.): 0-aus; 0-60
/*07*/ uint8_t datumvon;       // Zeige Datum von(einschl.Sek.): START der Datumsanzeige(in jeder Minute); 0-59
/*08*/ uint8_t datumbis;       // Zeige Datum bis(einschl.Sek.): ENDE Datumsanzeige, wenn ENDE < START -> disabled; 0-59
/*09*/ uint8_t nachtvon;       // Nachtmodus von (einschl.Stunde):START Nachtabschaltung; 0-23
/*10*/ uint8_t nachtbis;       // Nachtmodus bis (Beginn dieser Stunde):ENDE Nachtabschaltung, wenn ENDE == START -> disabled; 0-23
/*11*/ uint8_t alarmdispdauer; // Einschaltdauer des Displays bei Alarm(Min.): 0-aus); 0-99
/*12*/ uint8_t alarmdauer;     // Alarm Dauer(Min.): 0-global disabled (unabhängig von den Weckeinstellungen); 0-99
/*13*/ uint8_t speakermode;    // Speakermode(0/1): 0: bei Alarm high (für ext. Soundquelle), 1: Sound (für Speaker)
/*14*/ uint8_t pirdauer;       // Displaydauer PIR(Min.): 0-aus; 0-99
/*15*/ uint8_t hzmax;          // Netzfreqenz(Hz): Achtung!!"0" - undefiniert, 1 Hz..21 Hz ohne Backup,22 Hz..64 Hz mit Backup; 0-64
/*16*/ uint8_t osccalib;       // Kalibrierung des int. Oszillators bei Netzausfall
/*17*/ uint8_t szmonstart;     // Sommerzeit startet in diesem Monat (0-keine Umstellung); 0-12
/*18*/ uint8_t szmonend;       // Sommerzeit endet in diesem Monat (0-keine Umstellung); 0-12
/*19*/ uint8_t szhourstart;    // Zeit wird zu dieser Stunde bei Sommerzeit-Start vorgestellt; 0-24
/*20*/ uint8_t szhourend;      // Zeit wird zu dieser Stunde bei Ende der SZ zurück gestellt; 0-24
/*21*/ uint8_t szdiff;         // Zeit wird um diese Stundenzahl verstellt; 0-24

/*22*/ uint8_t sztabyrdaystart;// Tabelle der Umstellungstage für jedes Jahr beginnt ab Jahr; 0-99

/*23*/ uint8_t springdayyear0; // Tag Start 1-tes Jahr(das in Parameter 22) SZ; 1-31
/*24*/ uint8_t falldayyear0;   // Tag Ende  1-tes Jahr(das in Parameter 22) SZ; 1-31

/*25*/ uint8_t springdayyear1; /*26*/ uint8_t falldayyear1;
/*27*/ uint8_t springdayyear2; /*28*/ uint8_t falldayyear2;
/*29*/ uint8_t springdayyear3; /*30*/ uint8_t falldayyear3;
/*31*/ uint8_t springdayyear4; /*32*/ uint8_t falldayyear4;
/*33*/ uint8_t springdayyear5; /*34*/ uint8_t falldayyear5;
/*35*/ uint8_t springdayyear6; /*36*/ uint8_t falldayyear6;
/*37*/ uint8_t springdayyear7; /*38*/ uint8_t falldayyear7;
/*39*/ uint8_t springdayyear8; /*40*/ uint8_t falldayyear8;
/*41*/ uint8_t springdayyear9; /*42*/ uint8_t falldayyear9;
/*43*/ uint8_t springdayyear10; /*44*/ uint8_t falldayyear10;
/*45*/ uint8_t springdayyear11; //12-tes Jahr Starttag SZ
/*46*/ uint8_t falldayyear11;   //12-tes Jahr Endtag SZ
/*47*/ uint8_t springdayyear12; //13-tes Jahr Starttag SZ
/*48*/ uint8_t falldayyear12;   //13-tes Jahr Endtag SZ
/*49*/ uint8_t springdayyear13; //14-tes Jahr Starttag SZ
/*50*/ uint8_t falldayyear13;   //14-tes Jahr Endtag SZ

	};

	uint8_t p[1]; // Die wirkliche Größe ist natürlich sizeof(param_t).

} param_t;

#define YEAR_TAB_OFFSET 22

/*==================================================================================*/
//EEprom-Inhalt

uint8_t setup_maske[sizeof(uhr_t)] EEMEM = {     //Eingeschaltete Nixies im Setup für :
	~0x3f, ~0x3c, ~0x30,            //Uhrzeit
	~0x0c, ~0x3c, ~0x30,            //Datum
	~0x3d, ~0x3c, ~0x30             //Alarm-Zeit
};

uint8_t setup_max[sizeof(uhr_t)] EEMEM = {       //Maximal einstallbare Werte für:
	59, 59, 23,                     //Uhrzeit
	99, 12, 31,                     //Datum
	1, 59, 23                       //Alarm-Zeit
};


//Achtung, ee_uhr und ee_param_setup_conf müssen zwingend hintereinander deklariert werden,
//damit die Fkt. "speichere_ee()" kompakter formuliert werden kann.

uhr_t ee_uhr EEMEM = { {                    // Vorgaben beim ersten Einschalten der Uhr
			       54,59,12,                // Zeit         ss,mm,hh
			       BUILDYY,BUILDMM,BUILDDD, // Datum        jj,mm,tt Erstellungsdatum der SW
			       0, 0, 6                  // Alarmzeit    ss,mm,hh (Sekunde =0 -> Alarm aus)
		       } };

param_t ee_param_setup_conf EEMEM = { {    // Konfigurationseinstellungen Vorgaben
/*01*/ 0,            // Art der Zeitanzeige(0/1): 0-24h
/*02*/ 0,            // Art der Datumsanzeige(0..2): 0-DDMMYY
/*03*/ 0,            // Führende 0 unterdrücken(0/1): 0 (nein)
/*04*/ 50,           // Blink-Dauer (in 1/(hzmax*2) Schritten,(10 ms bei 50 Hz)): 50
/*05*/ 0,            // AM/PM Blinker (0..2): 0 (aus)
/*06*/ 10,           // Wiederholung Slotmachine(Min.): 10 (Slotmachine alle 10 Min.)
/*07*/ 20,           // Zeige Datum von(einschl.Sek.): 20 (Anzeige des Datums startet in der 20. Sek.)
/*08*/ 25,           // Zeige Datum bis(einschl.Sek.): 25 (Anzeige des Datums endet in der 25. Sek.)
/*09*/ 23,           // Nachtmodus von (einschl.Stunde): 23 (Röhren aus ab 23 Uhr)
/*10*/ 6,            // Nachtmodus bis (Beginn dieser Stunde): 6 (Röhren ab 6 Uhr wieder an)
/*11*/ 10,           // Einschaltdauer des Displays bei Alarm(Min.): 10 (wenn Nachtmodus, Display 10 Min. an)
/*12*/ 30,           // Alarm Dauer(Min.): 30 (Alarm ertönt 30 Min (wenn man ihn nicht ausschaltet))
/*13*/ 1,            // Speakermode(0/1): 1 (Sound)
/*14*/ 0,            // Displaydauer PIR(Min.): 0 (abgeschaltet, Röhren sind also ständig an)
/*15*/ 50,           // Netzfreqenz(Hz): 50 (Hz)
/*16*/ 50,           // OscCalib: 50 (unkalibriert)

// Sommerzeiteinstellungen für Deutschland ab Jahr 2010

/*17*/ 3,            // Sommerzeit startet in diesem Monat (0-keine Umstellung): 3 (März)
/*18*/ 10,           // Sommerzeit endet in diesem Monat (0-keine Umstellung): 10 (Oktober)
/*19*/ 2,            // Zeit wird zu dieser Stunde bei Sommerzeit-Start vorgestellt: 2 Uhr
/*20*/ 3,            // Zeit wird zu dieser Stunde bei Ende der SZ zurück gestellt: 3 Uhr
/*21*/ 1,            // Zeit wird um diese Stundenzahl verstellt: 1 Stunde
/*22*/ 10,           // Tabelle der Umstellungstage für jedes Jahr beginnt ab Jahr: 2010

/*23*/ 28,           //März 2010
/*24*/ 31,           //Oktober 2010

/*25*/ 27,           //März 2011
/*26*/ 30,           //Oktober 2011

/*27*/ 25,           //März 2012
/*28*/ 28,           //Oktober 2012

/*29*/ 31,           //März 2013
/*30*/ 27,           //Oktober 2013

/*31*/ 30,           //März 2014
/*32*/ 26,           //Oktober 2014

/*33*/ 29,           //März 2015
/*34*/ 25,           //Oktober 2015

/*35*/ 27,           //März 2016
/*36*/ 30,           //Oktober 2016

/*37*/ 26,           //März 2017
/*38*/ 29,           //Oktober 2017

/*39*/ 25,           //März 2018
/*40*/ 28,           //Oktober 2018

/*41*/ 31,           //März 2019
/*42*/ 27,           //Oktober 2019

/*43*/ 29,           //März 2020
/*44*/ 25,           //Oktober 2020

/*45*/ 28,           //März 2021
/*46*/ 31,           //Oktober 2021

/*47*/ 27,           //März 2022
/*48*/ 30,            //Oktober 2022

/*49*/ 26,           //März 2023
/*50*/ 29,           //Oktober 2023

				      } };


param_t ee_setup_conf_max EEMEM = { { // Maximalwerte für Konfigurationseinstellungen
/*01*/ 1,            // Art der Zeitanzeige(0/1):   max 1
/*02*/ 2,            // Art der Datumsanzeige(0..2): max 2
/*03*/ 1,            // Führende 0 unterdrücken(0/1): max 1
/*04*/ 99,           // Blink-Dauer (in 1/(hzmax*2) Schritten,(10 ms bei 50 Hz)): max 99
/*05*/ 2,            // AM/PM Blinker (0..2): max 2
/*06*/ 60,           // Wiederholung Slotmachine(Min.): max 60 (zu jeder vollen Stunde)
/*07*/ 59,           // Zeige Datum von(einschl.Sek.): max 59
/*08*/ 59,           // Zeige Datum bis(einschl.Sek.): max 59
/*09*/ 23,           // Nachtmodus von (einschl.Stunde): max 23
/*10*/ 23,           // Nachtmodus bis (Beginn dieser Stunde): max 23
/*11*/ 99,           // Einschaltdauer des Displays bei Alarm(Min.): max 99
/*12*/ 99,           // Alarm Dauer(Min.): max 99
/*13*/ 1,            // Speakermode(0/1):max 1
/*14*/ 99,           // Displaydauer PIR(Min.): max 99
/*15*/ 64,           // Netzfreqenz(Hz): 50 (Hz): max 64
/*16*/ 99,           // OscCalib: max 99
/*17*/ 12,           // Sommerzeit startet in diesem Monat (0-keine Umstellung): max 12
/*18*/ 12,           // Sommerzeit endet in diesem Monat (0-keine Umstellung): max 12
/*19*/ 24,           // Zeit wird zu dieser Stunde bei Sommerzeit start 1h vogestellt: max 24
/*20*/ 24,           // Zeit wird zu dieser Stunde bei Ende der SZ 1h zurück gestellt: max 24
/*21*/ 23,           // Zeit wird um die Stundenzahl verstellt: max 23
/*22*/ 99,           // Tabelle der Umstellungstage für jedes Jahr beginnt ab Jahr: max 99

/*23*/ 31,           // Tag Start 1-tes Jahr(das in Parameter 21) SZ: max 31
/*24*/ 31,           // Tag Ende  1-tes Jahr(das in Parameter 21) SZ: max 31

/*25..46*/ 31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,

/*47*/ 31,           //13-tes Jahr Starttag SZ : max 31
/*48*/ 31,           //13-tes Jahr Endtag SZ   : max 31
/*49*/ 31,           //14-tes Jahr Starttag SZ : max 31
/*50*/ 31,           //14-tes Jahr Endtag SZ   : max 31

				    } };


/*==================================================================================*/
//Variable-Definitionen
//Einfache Zahlen in Registern halten, verringert drastisch den Flash-Verbrauch:


register int16_t fading       asm ("r2");

register uint8_t data_idx     asm ("r4");
register uint8_t mplx_state   asm ("r5");
register uint8_t hzcount      asm ("r6");
register uint8_t pir_minuten  asm ("r7");
register uint8_t zeige_sonder asm ("r8");
register uint8_t hell         asm ("r9");
register uint8_t mplx         asm ("r10");
register uint8_t disp_maske   asm ("r11");
register uint8_t key_ct0      asm ("r12");
register uint8_t key_ct1      asm ("r13");
register uint8_t key_state    asm ("r14");
register uint8_t key_press    asm ("r15");

disp_t disp;
disp_t disp_old;                //alte Zeit (bzw. Datum) für fading.
uhr_t t;
param_t param;                  //Einstellbare Parameter
werte_t disppuffer;             //Zwischenspeicher für versch. Zeit und Datumsformate

#define key_in GPIOR2

uint8_t key_rpt;
uint8_t alarm_minuten;

//spart 2 Byte
int main (void) __attribute__ ((noreturn));
