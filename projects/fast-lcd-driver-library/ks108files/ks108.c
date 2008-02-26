//************************************************************************
// KS108.C
// LowLevel Routinen für LCD-Displays mit KS0108
//
// Hier für Displaytech 64240A
//
// Mit bis zu vier KS108 Chips also bis 240x64
// Ausgelegt bisher nur auf diesen Displaytyp
//
// Der Nullpunkt des Displays liegt oben,links. Für unten rechts
// müssen dann zusätzliche Umrechnungen stattfinden.
//
// hk@holger-klabunde.de
// http://www.holger-klabunde.de
// 11.10.2003
// Compiler WinAVR
//
// MODS by Peter Menges 2007:
// To speed up the performance of the output to the display
// SRAM of size 1k is allocated. All modifications (drawing something) is
// made internal and the display is updated in one shot. 
// This consumes much SRAM, but the performance is more than twice of the 
// original code.
// Be aware that the timing is coded for 1 MHz clock.
// See the notes (prefixed with PME ) I made at several routines. To port for higher clock rates
// only some minor modifications are necessary.
// 
// Thanks to Holger for his basic coding!
//
// Coding is for ATMega32 running at 1 MHz from internal clocksource.
//************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "protos.h"
#include "ks108.h"
#include "uart.h"

unsigned char Dis_Buf[1024]={0}; /* PME 128 x 64 Pixel / 8 = 1024 */

//###################################################################################
// E ist die Bremse des Displays. Braucht ganz schön lange Delays.
// Geniale Angaben zum Timing im Datenblatt:
// 450ns Minimum für E_low und E_high. Zusammen 1000ns. Fehlen schonmal 100ns.
// Und wo ist das Maximum ? Da ist raten angesagt.
// PME reading of the E - PIN is obsolete because some displays do not handle this pin
// If you running into to data loss, increment the number of nops. This is the
// main timing for the display.
//
void Wait_for_E(void)
//###################################################################################
{ // PME Note: Add some more NOPs. One for each MHz above 1 MHz
 
NOP(); /* PME Sufficent for 1 MHz clock*/
}

//##########################################################
// Liest ein Byte aus dem Display
unsigned char ReadDisplay(void)
{
 unsigned char by;

 
 RS_ON();
 RW_ON(); //Wir wollen lesen

//Display muß zweimal gelesen werden !
//Dummy Read
E_ON();
E_OFF();

 DATA_DIR_IN();

 RS_ON();
 RW_ON(); //Wir wollen lesen

 E_ON();
 by=READ_DATA(); //DATA RAM lesen
 E_OFF();

 RW_OFF(); //zurück auf schreiben

 DATA_DIR_OUT();
 
 return by;
}

	
//##########################################################
//Ein Byte in das Display schreiben
//Register Select muß vorher entsprechend gesetzt werden !
void WriteDisplay(unsigned char dat)
{
 DATA_DIR_OUT();

 RW_OFF(); //Display schreiben setzen

 WRITE_DATA(dat);
 E_ON();
 E_OFF();

}

//##########################################################
//Pixel auf dem Display setzen
//mode=0; Pixel löschen
//mode=1; Pixel setzen
unsigned char SetPixel(unsigned int x, unsigned int y, unsigned char val )
{
 unsigned char *p, tmp;
 int idx;
  
 
 
 if(x >= LCD_WIDTH) return 1; //Punkt außerhalb der Anzeige
 if(y >= LCD_HEIGHT) return 1;
 
	idx = 0;
	
	idx = y/8;
			
	idx = idx * 64;
	
		if ( x <= 63 )
				idx += x; 
		else 
				idx += (512 + ( x - 64));
				
	if ( idx > 1023 || idx < 0){
		(PORTD) |= (1 << (5)); //PME LED 1 ON ->Debugging purpose only
		return 1;
	}
							

	p = &Dis_Buf[ idx ];
	tmp = y % 8;
	
	if(val)
	*p |= ( 1 << tmp);		
	else 
	*p &= ~( 1 << tmp);

	 return 0;
}

//##########################################################
//Aus der X-Position die Chipnummer berechnen
unsigned char GetChipNumber(unsigned int xpos)
{
 return xpos/64; //Jeder Chip kontrolliert 64x64 Pixel
}

//##########################################################
//Chipnummer des Displays setzen
void SetChipNumber(unsigned char num)
{
 E_OFF();
 CS1_OFF();
 CS2_OFF();
 switch (num){
	case 0: CS1_ON(); break;
	case 1: CS2_ON(); break;
	default: break;
 }
 
}

//##########################################################
//Aus der Y-Position die Pagenummer berechnen
unsigned char GetPageNumber(unsigned char ypos)
{
 return ypos/8; //Jede Page ist 8 Zeilen hoch
}

//##########################################################
//Pagenummer des Chips setzen
unsigned char SetPageNumber(unsigned char num)
{
 
 RS_OFF(); //Instruction setzen
 WriteDisplay(0xB8|num); //Pagenummer setzen

 RS_ON(); //Zurück auf Daten
 return 0;
}
// INIT IO
//
void InitDisplay(void)
{
	INIT_CONTROL_PINS();
	RSTB_OFF(); // RESET
	_delay_us(10);
	RSTB_ON(); // Go
	_delay_us(10);
	DisplayOn();
/* PME Debugging helper*/ 
	uart_init();
	uart_puts("InitDisplay done\r\n");
	
}

//##########################################################
//An jeden KS0108 muß der DisplayOn Befehl gesendet werden
void DisplayOn(void)
{
 unsigned char i;

 E_OFF();

 for(i=0; i< MAX_CHIPS; i++)
  {
   SetChipNumber(i); 
    RS_OFF(); //PME prepare for instruction
   WriteDisplay(0x3F); //PME Set instruction display on
   RS_ON(); //PME Switch back to data
  } 

}
/* Disable displaying the content of the display ram*/
void DisplayOff(void)
{
 unsigned char i;

  E_OFF();

 for(i=0; i< MAX_CHIPS; i++)
  {
   SetChipNumber(i); 
    RS_OFF(); //PME prepare for instruction 
   WriteDisplay(0x3E); //PME Set instruction display off
   RS_ON(); //PME Switch back to data
  } 

}

//##########################################################
void ClearScreen(void)
//##########################################################
{ //PME Sure that this could be done much simpler ;-)
  // But avoiding libraries saves flash ram

 unsigned int i;
 
 for(i=0; i < 1024; i++ ) Dis_Buf[i] = 0x00;
 
 
}
 

void UpdateDisplay(void)
{
	unsigned char j,i;
	char k;
	unsigned char  *p;
	
	
	
	
	DisplayOff(); // PME Do not show the update. Doesn't work with all displays
	k= 0; 
	p = Dis_Buf;
	
	
	 do
	  {
	   SetChipNumber(k); 
	
	   for(j=0; j<8; j++) // PME Run down the "video" memory and write it to the display
		{
		 SetPageNumber(j);
		 RS_OFF(); //PME Do not forget to: Set Instruction 13.4.2007 15:35 Uhr
		 WriteDisplay(0x40); //PME Set Column 0 
         RS_ON();
		 RW_OFF(); //PME Set display to write operation
	
		 
		 for(i=0; i<64; i++)
		  {  
			WRITE_DATA(*p++);
						
			E_ON();
			E_OFF(); 
		  }
		}
	
	  k++;
	 }while(k < MAX_CHIPS);
 

 
 DisplayOn(); // PME Now show the result
	
}



