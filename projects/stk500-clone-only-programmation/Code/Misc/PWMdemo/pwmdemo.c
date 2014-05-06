/******************************************************************************
* PWM Demo
*
* Jidan Al-Eryani
*
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>


/*
 * constants
 */


#define XTAL		  8000000    // Crystal frequency in Hz
#define OC1A_PIN PB5              // OC1A pin (for AT90S8515 use PD5, for ATmega8 use PB1)
#define OC1A_DDR DDRB             // OC1A DDR (for AT90S8515 use DDRD, for ATmega8 use DDRB)


//function prototypes
void delay_ms(unsigned int ms);


int main(void)
{
    uint8_t i = 0;


    DDRB  = 0xff;                  // use all pins on PortB for output 
    PORTB = 0x00;                  // set output high -> turn all LEDs off


    // set OC1A pin as output, required for output toggling
    OC1A_DDR |= (1<<OC1A_PIN);
    
    // enable 8 bit PWM, select inverted PWM
    TCCR1A =  (1<<WGM10) | (1<<COM1A1) | (1<<COM1A0); //Mode: Fast PWM 8-bit
    
    // timer1 running on 1/8 MCU clock with clear timer/counter1 on Compare Match
    // PWM frequency will be MCU clock / 8 / 512, e.g. with 4Mhz Crystal 977 Hz. @8MHz: 1953 Hz
    TCCR1B = (1<<CS11) | (1<<WGM12);
 
    
    /*
     *  Dimm LED on and off in interval of 2.5 seconds
     */
    for (;;)
    {         
        /* dimm LED off */
        for( i=255; i>0; i--)
        {
            OCR1AL = i;            
            delay_ms(10);   // delay 10 ms
        }
    }
}


// Delay loop
void delay_ms(unsigned int ms)
{
    unsigned int zaehler;
   
    while (ms) {
        zaehler = XTAL / 5000;
       
        while (zaehler) {
            asm volatile("nop");
            zaehler--;
        }
        ms--;
    }
}


/* Another example from: http://www.kreatives-chaos.com/artikel/codeschnipsel-avr-gcc
#include <avr/io.h>

// Timer1 initialisieren
void timer1_init(void)
{
    // normale 8-bit PWM aktivieren (nicht invertiert),
    // Das Bit WGM10 wird im Datenblatt auch als PWM10 bezeichnet
    TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM10);

    // Einstellen der PWM-Frequenz auf 14 kHz (Prescaler = 1)
    TCCR1B = (1<<CS10);

    // Interrupts für Timer1 deaktivieren
    // Achtung: Auch die Interrupts für die anderen Timer stehen in diesem Register
    TIMSK &= ~0x3c;
}


int main(void)
{
    // Timer1 initialisieren
    timer1_init();
   
    // Werte für die beiden PWM Kanäle setzen.
    // Zum Beispiel 100% Tastverhältnis für OC1A
    OCR1AL = 0xff;
    // und ca. 50% Tastverhältnis (= 127/255) für OC1B
    OCR1BL = 127;

    while (1)
        ;
}*/
