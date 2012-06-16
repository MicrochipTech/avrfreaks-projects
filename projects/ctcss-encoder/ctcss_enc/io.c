/*******************************************************************
 * io.c
 * written by      Ralf Meeh 26.07.2010
 * last changed by Ralf Meeh 29.07.2010
 * Function:       I/O Funktionen,  Projektabhängig
 *******************************************************************/
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
//#include <stdlib.h>
//#include <avr/pgmspace.h>
// eigene Headerdateien
#include "defines.h"
#include "num2a.h"
#include "lcd-routines.h"
#include "io.h"
#include "isr.h"
#include "ctcss_enc.h"

#include <util/delay.h>


// **********************************
// Ein-Ausgabe Register vorbereiten
// **********************************
void init_io(void)
{


	KEYDDR 		&= ~(1<<KEY1);			// Taster am Drehgeber
	KEYDDR 		&= ~(1<<KEY2);			// Reserve
	KEYPORT 	&= ~(1<<KEY1);			// Kein Pullup
	KEYPORT 	&= ~(1<<KEY2);			// Kein Pullup

	T1DDR		|=  (1<<T1);			// Port kurzfristig als Ausgang
	T1PORT		|=	(1<<T1);			// Transistor T1 durchschalten
	T1DDR		&= ~(1<<T1);			// Und Pin hochohmig machen (T1 gesperrt)


//	Der Drehgeber
	ROTARYDDR	&= ~(1<<ROTARY1);
	ROTARYDDR	&= ~(1<<ROTARY2);
	ROTARYPORT	|=	(1<<ROTARY1);	// Pullup ein
	ROTARYPORT	|=	(1<<ROTARY2);	// Pullup ein

	LED_DDR		|=  (1<<LED);		// Ausgang für LED (Betrieb)
	SC_DDR		|=	(1<<SC);		// Sub Carrier Ausgang
	tick=0;

}

// **********************************
// Eine Taste liefern ohne Entprellen 
// **********************************
unsigned char scan_key(void)
{
	unsigned char help = 0;
	if((KEYS & (1<<KEY1)))
	{
  		help =1;
	}
	if((KEYS & (1<<KEY2)))
  		help =2;
	if(help)
		return(help);
	else
		return(0);
}


// **********************************
// Eine entprellte Taste liefern 
// **********************************
unsigned char get_debounced(void)
{
  static unsigned char old_scan;
  unsigned char result;
  result = scan_key();
  if(!result)
  {
    old_scan = 0;
	T_10[0] = 0;
	return(0);
  }
  if(T_10[0]>40)	 // Entprellzeit öffnen läuft noch
  {
    return(0);
  }
  if(T_10[0]==40)	// Öffnungs-Entprellzeit abgelaufen
  {
    T_10[0]=0;
	return(0);
  }
  if(result != old_scan)	  // Erster Tastendruck
  {
    old_scan = result;
	T_10[0] = 30;
	return(0);
  }
  if(T_10[0]==0)			   // Ab hier kommt Tastenprellen
  {
    T_10[0]=30;				   // 20ms Entprellzeit starten
	return(0);
  }
  if(T_10[0]==10)			   // 20ms Entprellzeit abgelaufen
  {
    old_scan = 0;		   // alten Puffer löschen
	T_10[0]=30;		       // Das Loslassen der Taste entprellen und autorepeat verzögern
    return(result);		   // entprelltes Tastensignal weiterreichen
  }
  else
  {
    return(0);
  }
}
// **********************************
// Stellung Drehgeber ermitteln
// **********************************
void read_rotary(void)
{
             quadrature_input = ((PINC & 0x0C) >> 2);    // nur bit 3 und 4 nach 1 u 2 geschoben
             new_quadrature_value=quadrature_input;
             if(last_quadrature_value!=new_quadrature_value)
             {
                   position+=table[last_quadrature_value][new_quadrature_value];
                   last_quadrature_value=new_quadrature_value;
				   rot_has_changed = 1;
             }
             if(position <1)
                         position = 39;
             if(position > 39)
                         position = 1;

}











