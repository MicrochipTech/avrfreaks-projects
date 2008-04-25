//////////////////
// Include
//////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

//////////////////
// Defines
//////////////////
#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))
volatile int sec = 0;
volatile int min = 0;
volatile int hrs = 0;
char tmpstr[3];
char tempsec;
char tempmin;
char temphr;
ISR(TIMER2_COMP_vect)
{
   PORTB ^= (1 << 0); // Toggle the LED
	sec++;

	if (sec==60) {
		min++;
		sec = 0;

		if (min==60) {
			hrs++;
			min = 0;

			if (hrs==24) {
				sec = 0;
				min = 0;
				hrs = 0;
			}
		}
	}
}
void main() {



DDRB |= (1 << 0); // Set LED as output
TCCR2 |= (1<<WGM21) | (1<<CS22) | (1<<CS21) | (1<<CS20);
OCR2 = 32;
ASSR |= (1<<AS2);
TIMSK |= (1 << OCIE2); // Enable CTC interrupt
sei();

//lcd_init(LCD_DISP_ON);
//lcd_clrscr();
//lcd_puts("Clock V2");
//_delay_ms(1000);
//lcd_clrscr();
//lcd_puts("00:00:00");

//_delay_ms(1000);
//lcd_clrscr();
int i=0;
for (i=0; i>=10; i++) {
//lcd_clrscr();
sprintf(tmpstr,"%d",i);
//lcd_puts(tmpstr);
//_delay_ms(100);



}

		/*	sprintf(tempsec, "%d", sec);
			lcd_gotoxy(6,0);
			lcd_puts("0");
			lcd_gotoxy(7,0);
			lcd_puts(tempsec);
			*/

}
