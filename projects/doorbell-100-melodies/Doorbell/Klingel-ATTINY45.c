//Türklingel mit 100 Melodien M. Baudisch 12/2008

#include <inttypes.h>		// uint8_t usw.
#include <avr/io.h>
#include <avr/interrupt.h>	// Interrupts
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>		// definiert _delay_ms und _delay_loop_2 

#include "lied.h"           //Melodien, Offsets und Tonhöhen

#define     SPEAKER         (1<<PB1)
#define     TASTE           (1<<PB2)
#define     SOUNDON         TCCR1=(1<<CTC1)|(1<<COM1A0)|(1<<CS10)|(1<<CS11)|(1<<CS12)


#define     SOUNDOFF        TCCR1=0

//TCCR1 = (1<<CTC1) |           Timer wird nach erreichen des Vergleichswertes 
//                              OCR1A rückgesetzt OCR1A -> Frequenz
//        (1<<COM1A0) |         Output (OC1A/B1) wird getoggled
//        (1<<CS10)|(1<<CS11)|(1<<CS12);  //Vorteiler 64


//Initialisierungswert des Zufallsgenerators ist EEPROM Speicherstelle

uint8_t EEMEM seed;

//Vars in Registern, spart Platz

volatile register int16_t soundtime  asm("r2");

register uint16_t liedcount    asm("r4");  
register uint16_t liedende     asm("r6");
register uint8_t liednr        asm("r8");
register uint8_t speed         asm("r9");
register uint8_t dauer         asm("r10");
register uint8_t hoehe         asm("r11");
register uint8_t note          asm("r12");


// Interruptroutine ohne Inhalt zum Aufwachen aus dem Sleep
EMPTY_INTERRUPT(PCINT0_vect);


ISR(TIM0_COMPA_vect) {    //Interrupt Zeitsteuerung

if (soundtime) soundtime--;

}


//Zufallszahlengenerator

#define MYRAND_MULTIPLIER  17
#define MYRAND_CONSTANT    37


uint8_t g_nLastRandom;


uint8_t MyRandGet()
{
  //copy into local variables to avoid 16-bit integer expansion
  uint8_t Multiplier = MYRAND_MULTIPLIER;
  uint8_t Constant = MYRAND_CONSTANT;

  //implicit %256 by 8-bit datatype overflow
  g_nLastRandom = (g_nLastRandom * Multiplier + Constant);
  
  return g_nLastRandom % ANZ;
}
 



int main()
{

PORTB = TASTE; //Pull up an

//Timer 0 initialisieren

TCCR0B=(1<<CS02)|(0<<CS01)|(1<<CS00); //Vorteiler 1024 
TCCR0A=(1<<WGM01); //CTC Mode


TIMSK = (1 << OCIE0A); //Interrupts auf Comp Wert A zulassen
OCR0A=155; //ergibt ca. 20ms bzw. 50Hz

PCMSK = (1<<PCINT2); //Pichange an PB2 konfigurieren

GIMSK=(1<<PCIE); // Pinchange Interrupt zulassen


set_sleep_mode(SLEEP_MODE_PWR_DOWN);    

//Zufallsgenerator mit Seed initialisieren
g_nLastRandom = eeprom_read_byte(&seed);

eeprom_write_byte(&seed,MyRandGet()); //neuen Seed schreiben

// da Variablen in Registern, explizite Initialisierung nötig!

liednr=0;
speed=0;
soundtime=0;

sei(); //Interrupts ein


for(;;)
{

if (liednr) //wenn Melodie aktiv
{ 
    if (!soundtime) //wenn kein Ton oder Pause läuft
        {  //keine Tonzeitdauer aktiv 
        SOUNDOFF;     //Tonwiedergabe aus
        if (!speed) //wir sind am Liedanfang
            {
            liedcount=eeprom_read_word(&liedofs[liednr-1]);
            if (liednr==ANZ) liedende=sizeof(lied); else 
                                            //bei letztem Lied ist Endwert Größe des Arrays
            liedende=eeprom_read_word(&liedofs[liednr]); //ansonsten 1 Byte vor nächstem Lied
            
            speed=pgm_read_byte(&lied[liedcount]);
            DDRB  = SPEAKER; //Speaker an
            liedcount++;
           } else //mitten im lied
           {
            _delay_ms(20); // um Töne zu trennen
            //Ton ausgeben
            note=pgm_read_byte(&lied[liedcount]); //Note holen
            hoehe=note & 0x1f; //Tonhöhe ausmaskieren (5 Bit)
            dauer= zeit[(note >> 5)]; // 5 Bit nach rechts schieben und Zeit holen
            if (hoehe) //wenn keine Pause 
                    {
                    OCR1C=eeprom_read_byte(&ton[hoehe-1]);
                    SOUNDON;
                    }

            cli();     //da Variable vom Interrupt während der Zuweisung geändert werden könnte  
            soundtime=dauer*speed;
            sei();             
            
            liedcount++;

            //Lied zu Ende
            if (liedcount>liedende)
            {
            speed =0;
            liednr=0;
            
            }
           }




        } //(!soundtime)

} //liednr


else {

    SOUNDOFF; //falls noch Ton an
    
    DDRB  &= !(SPEAKER); //Speaker Port hochohmig
    sleep_mode();
    liednr=MyRandGet()+1;  

    }

} //for


} //main
