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



#define    PWM_MIN        0                //Minimalwert für PWM-Tastverhältnis
#define    PWM_MAX        100              //Maximalwert für PWM-Tastverhältnis
#define    PWM_STEP       1                //Spannungsregelung-Schrittweite


//Alle Zahlen sind Mikrosekunden
//Zeiten
#define    MS_5           5000             // in us
#define    MS_6           6000             // in us
#define    MS_30          30000            // in us
#define    T_NIXIE_MIN    100              // us Mindest-Einschaltzeit
#define    T_NIXIE        2700-T_NIXIE_MIN // Zeit    pro Nixie (us)

#define    FADING_STEP    10               // Schrittweitein us für das Fading
#define    FADING_MAX     2400             // Fading klappt bis ca. 2500

#define DIMM_NIXIE_AN     1700             //Nixie- "An"-Zeit im Dimm-Modus
#define DIMM_NIXIE_AUS    2000             //Nixie- "Aus"-zeit im Dimm-Modus

//Regelt PWM mit Comparator
/*==================================================================================*/
static inline void  spannungsregelung (void)  {


	if (TCCR0A) { //Wenn Anodenspannung eingeschaltet ist:

		uint8_t pwm = OCR0A;

		if (ACSR & (1 << ACO)) {  //ACO-Bit gesetzt ? --> Die Spannung ist zu niedrig.

			if (pwm < PWM_MAX)    //Tastverhältnis unter Maximalwert ?
				pwm += PWM_STEP;  //Ok, erhöhen.

		} else {                  //Die Spannung ist zu hoch.

			if (pwm > PWM_MIN)    //Tastverhältnis über Minimalwert ?
				pwm -=  PWM_STEP; // Ok, niedriger machen.

		}

		OCR0A = pwm;

	}
}


//Für Tastenentprellung
/*==================================================================================*/
static inline void debounce (void) { //Author: Peter Dannegger


	//Taste 4 =Langer Tastendruck der Taste 2 oder Taste 3

	if (key_state & (TASTE2 | TASTE3)) {

		key_in &= ~TASTE4;

		if (key_rpt) {
			key_rpt--;
		} else {
			key_in |= TASTE4;
		}

	} else key_rpt=TASTE_LANG_DAUER;

	uint8_t tmp = key_state ^ key_in;

	key_ct0 = ~( key_ct0 & tmp );
	key_ct1 = key_ct0 ^ (key_ct1 & tmp);
	tmp &= key_ct0 & key_ct1;
	key_state ^= tmp;
	key_press |= key_state & tmp;


}

//Interrupt alle 1/(Netzfreqreq * 2) Sek.  (Flankenwechsel)
/*==================================================================================*/
ISR (PCINT_vect) {

//Netzfilter
	GIMSK = 0;   //Interrupt disablen bis COMPB ihn wieder einschaltet
	OCR1B=TCNT1+MS_6; //das passiert in 6 ms (Wert passend für 50 und 60 Hz)

	bits &= ~PWR_FAULT;         // Netzspannung vorhanden
	bits |= TRIGGER_NETZFREQ;   // letze Triggerung aus Netzfrequenz


	if ((++hzcount==param.blinkdauer) && !zeige_sonder && !param.ampmblink) PORTB &= ~BLINK;
	// wenn Blinkdauer rum und kein AM/PM -> Sekunden Blinker aus

	if(hzcount==(param.hzmax*2)) {
		hzcount = 0;
		bits |= NEUE_SEKUNDE; //Eine Sekunde ist rum
	}

}

//Netzfilter und Weiterzählen bei Akkubetrieb
/*==================================================================================*/
ISR(TIMER1_COMPB_vect) {

//PCINT wieder aktivieren (Netzfilter)
	EIFR  = (1 << PCIF); //alte Interrupts löschen
	GIMSK = (1 << PCIE); // wieder freigeben

//Wenn keine Netzfrequenz anliegt (Akkubetrieb)
	if (!(bits & TRIGGER_NETZFREQ) && (param.hzmax>=22)) { // es erfolgte seit dem letzten Aufruf kein Netztrigger
		                                                   //klappt nur ab ca. 20 Hz
		bits |= PWR_FAULT;                                 //Stromausfall-Flag setzen
		OCR1B += MS_5;                                     //nächster Aufruf in 5 ms
		if (++hzcount >= 150+param.osccalib) { //5 ms Schritte, damit kann man Genauigkeit mit OSCCALIB feiner einstellen
			hzcount = 0;
			bits |= NEUE_SEKUNDE; //Eine Sekunde ist rum
		}
	}

	bits &= ~TRIGGER_NETZFREQ; //Flag für Netfrequenztrigger Rücksetzen
}

//Interrupt für Multiplexing, Fading, Spannungsregelung
/*==================================================================================*/
ISR(TIMER1_COMPA_vect) {

	uint16_t ocr_tmp;

	uint8_t mplx_n;

	if (bits & ALARM) PINA = SPEAKER;            //toggled SPEAKER, wenn Alarm + Sound

	mplx_n = (1 << mplx);

	switch(mplx_state) {

	case 0: if (hell<3)
		{
			if (hell==1) fading=DIMM_NIXIE_AN;
			goto case1;
		}



		fading -= FADING_STEP;

		if (fading >= 0)
		{                   //alte Ziffern anzeigen
			if (mplx_n & disp_maske )
			{
				PORTD = 0;

			} else {

				PORTB = (PORTB & 0x0f) | ((disp_old.r[mplx]) << 4 );
				PORTD =  mplx_n;
			}

			ocr_tmp = T_NIXIE_MIN + fading;
			mplx_state = 1;
			break;

		}  else

			fading = 0;  //hier wirklich kein "break!"

	case 1:
case1:
		//Tasten abfragen:
		PORTD = mplx_n; //PORTD setzen, um Taste lesen zu können
		//Da PIND nicht sofort nach dem setzen von PORTD gelesen werden sollte,
		//machen wir zwischendurch etwas sinnvolles und setzen schonmal mplx_state.
		mplx_state = 2;
		//jetzt können wir PIND abfragen, um die gedrückten Tasten festzustellen:
		if (PIND & TASTEN_COMMON) key_in |= mplx_n;

		if ( mplx_n & disp_maske )
		{ //Die Nixie ist abgeschaltet, am Multiplex also nichts ausgeben:

			PORTD  = 0;

		} else {
			// BCD-Wert zum 74141 schicken und Multiplexleitung setzen:

			PORTB  = (PORTB & 0x0f) | ( (disp.r[mplx]) << 4 );
			PORTD  =  mplx_n;
		}

		ocr_tmp = T_NIXIE_MIN + T_NIXIE - fading;

		break;

	default:

		PORTD = 0;

		if (++mplx >= 6) { //zur nächsten Röhre schalten

			debounce();    // Tasten entprellen
			key_in = 0;    // Tastenpuffer leeren.
			mplx = 0;
		}

		mplx_state    = 0; //Weitermachen mit state 0 (nächste Nixie)


		if (hell == 1)
		{
			ocr_tmp = DIMM_NIXIE_AUS;
		} else    {
			ocr_tmp = 100;
		}

	}

	spannungsregelung(); // Anodenspannung regeln

	OCR1A += ocr_tmp;
}
