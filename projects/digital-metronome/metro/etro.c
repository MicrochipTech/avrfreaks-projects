// Digital Metronome Program
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

/*
PBo - (out/in) Data/ S3 (On/Off)
PB1 - (in) S2 
PB2 - (in) S1
PB3 - (out) Clock
PB4 - (out) Beeper
PB5 - Reset
*/

#define onTime 50 	//Piezo on-time per beat - the longer the louder
#define timeDiff 4 	//Ratio of accent on-time to non-accent on-time
#define DIV 256 	//Counter division

void num_out(char);
int get_period(int);

int periods[] PROGMEM = 
{
0,
(F_CPU/DIV)/(5/(double)60),
(F_CPU/DIV)/(10/(double)60),
(F_CPU/DIV)/(15/(double)60),
(F_CPU/DIV)/(20/(double)60),
(F_CPU/DIV)/(25/(double)60),
(F_CPU/DIV)/(30/(double)60),
(F_CPU/DIV)/(35/(double)60),
(F_CPU/DIV)/(40/(double)60),
(F_CPU/DIV)/(45/(double)60),
(F_CPU/DIV)/(50/(double)60),
(F_CPU/DIV)/(55/(double)60),
(F_CPU/DIV)/(60/(double)60),
(F_CPU/DIV)/(65/(double)60),
(F_CPU/DIV)/(70/(double)60),
(F_CPU/DIV)/(75/(double)60),
(F_CPU/DIV)/(80/(double)60),
(F_CPU/DIV)/(85/(double)60),
(F_CPU/DIV)/(90/(double)60),
(F_CPU/DIV)/(95/(double)60),
(F_CPU/DIV)/(100/(double)60),
(F_CPU/DIV)/(105/(double)60),
(F_CPU/DIV)/(110/(double)60),
(F_CPU/DIV)/(115/(double)60),
(F_CPU/DIV)/(120/(double)60),
(F_CPU/DIV)/(125/(double)60),
(F_CPU/DIV)/(130/(double)60),
(F_CPU/DIV)/(135/(double)60),
(F_CPU/DIV)/(140/(double)60),
(F_CPU/DIV)/(145/(double)60),
(F_CPU/DIV)/(150/(double)60),
(F_CPU/DIV)/(155/(double)60),
(F_CPU/DIV)/(160/(double)60),
(F_CPU/DIV)/(165/(double)60),
(F_CPU/DIV)/(170/(double)60),
(F_CPU/DIV)/(175/(double)60),
(F_CPU/DIV)/(180/(double)60),
(F_CPU/DIV)/(185/(double)60),
(F_CPU/DIV)/(190/(double)60),
(F_CPU/DIV)/(195/(double)60),
(F_CPU/DIV)/(200/(double)60),
(F_CPU/DIV)/(205/(double)60),
(F_CPU/DIV)/(210/(double)60),
(F_CPU/DIV)/(215/(double)60),
(F_CPU/DIV)/(220/(double)60),
(F_CPU/DIV)/(225/(double)60),
(F_CPU/DIV)/(230/(double)60),
(F_CPU/DIV)/(235/(double)60),
(F_CPU/DIV)/(240/(double)60),
(F_CPU/DIV)/(245/(double)60),
(F_CPU/DIV)/(250/(double)60),
(F_CPU/DIV)/(255/(double)60),
(F_CPU/DIV)/(260/(double)60),
(F_CPU/DIV)/(265/(double)60),
(F_CPU/DIV)/(270/(double)60),
(F_CPU/DIV)/(275/(double)60),
(F_CPU/DIV)/(280/(double)60),
(F_CPU/DIV)/(285/(double)60),
(F_CPU/DIV)/(290/(double)60),
(F_CPU/DIV)/(295/(double)60),
};

volatile int tempo;
volatile int period;
volatile char sMode;
char dig1;
char dig2;
char dig3;

volatile char beat = 3;
volatile char beatsPerBar = 4;
volatile int counter = 0;

int main(void)
{
	OSCCAL = 90;	/* Calibration number - use an accurate tempo source
					** such as http://www.metronomeonline.com/ to adjust this
					** number (at the initial 120bpm) until the metronome accurately
					** keeps in time with the source. Increasing OSCCAL speeds the metronome
					** up, and decreasing slows it down.
					*/
	
	dig1 = 1;
	dig2 = 2;
	dig3 = 0;
	
	tempo = (dig1*100) + (dig2*10) + dig3;
	
	TCCR0B = (1 << CS00) | (0 << CS01) | (0 << CS02);
	GIMSK = (1 << PCIE);
	PCMSK = (1 << PB0) | (1 << PB1) | (1 << PB2);
	
	DDRB = (1 << PB0) | (0 << PB1) | (0 << PB2) | (1 << PB3) | (1 << PB4);
	period = get_period(tempo);
	num_out(11);
	
	TIMSK0 = (1 << TOIE0);
	sei(); // Enable interupts

	while (1);

}

void num_out(char number)
{
	// Outputs number to shift register
	
	int i;

	DDRB |= (1 << PB0); // Clock and Data are outputs
	for (i = 3; i>=0; i--) {
		if (number & (1 << i)) {
			PORTB |= (1 << PB0);
		} else {
			PORTB &= ~(1 << PB0);
		}
		PORTB |= (1 << PB3); // Clock the register
		PORTB &= ~(1 << PB3);
	}
	DDRB &= ~(1 << PB0); // Make PB0 input again
	return;
}

int get_period(int tempo)
{
	tempo /= 5;
	return pgm_read_word(&periods[tempo]);
}

ISR (PCINT0_vect)
{	
	//if (!(PINB & ((1<< PB0) | (1 << PB1)))) return;
	PORTB &= ~(1 << PB4);
	
	if ((PINB & (1 << PB1)) > 0) {
		if (sMode == 0) {
			sMode = 1; // Now in tempo setting sMode (First digit)
			num_out(dig1);
		} else if (sMode == 1) {
			sMode = 2; // 2nd digit
			num_out(dig2);
		} else if (sMode == 2) {
			sMode = 3; // 3rd digit
			num_out(dig3);
		} else if (sMode == 3) {
			sMode = 4; // Beats per bar
			num_out(beatsPerBar);
		} else if (sMode == 4) {
			// Set new tempo
			tempo = (dig1*100) + (dig2*10) + dig3;
			period = get_period(tempo);
			beat = beatsPerBar - 1;
			//counter = 0;
			sMode = 0; // Return to normal sMode
			num_out(11);
		}
	} else if ((PINB & (1 << PB0)) > 0) {
		if (sMode == 1) {
			dig1 = (dig1 + 1) % 3; // 1st digit
			num_out(dig1);
		} else if (sMode == 2) {
			dig2 = (dig2 + 1) % 10; // 2nt digit
			num_out(dig2);
		} else if (sMode == 3) {
			dig3 = (dig3 + 5) % 10; // 3rd digit
			num_out(dig3);
		} else if (sMode == 4) {
			beatsPerBar = (beatsPerBar + 1) % 10; // BPB
			num_out(beatsPerBar);
		}
	} else {
		return;
	}
	
	_delay_ms(200); //Debounce
	while (PINB & ((1<< PB0) | (1 << PB1)));
	
}

ISR (TIM0_OVF_vect)
{
	
	if (sMode) return;
	
	counter++;
	if (counter == (timeDiff * onTime)) {
		if (beat >= beatsPerBar) {
			beat = 0;
			PORTB &= ~(1 << PB4);
		}
	} else if (counter == onTime) {
		if (beat != beatsPerBar) {
			PORTB &= ~(1 << PB4);
		}
	}
	if (counter >= period) {
		beat++;
		counter = 0;
		PORTB |= (1 << PB4);
	}
	
}
