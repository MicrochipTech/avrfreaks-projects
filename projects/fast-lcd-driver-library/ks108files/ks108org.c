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
//************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "protos.h"
#include "ks108.h"



//###################################################################################
// E ist die Bremse des Displays. Braucht ganz schön lange Delays.
// Geniale Angaben zum Timing im Datenblatt:
// 450ns Minimum für E_low und E_high. Zusammen 1000ns. Fehlen schonmal 100ns.
// Und wo ist das Maximum ? Da ist raten angesagt.
void Wait_for_E(void)
//###################################################################################
{
 
NOP(); /* bei 1 MHz Takt ausreichened.*/
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
unsigned char SetPixel(unsigned char xpos, unsigned char ypos, unsigned char mode)
{
 unsigned char by,tmp,pixpos;
 //unsigned char pixset;

 if(xpos>=LCD_WIDTH) return 1; //Punkt außerhalb der Anzeige
 if(ypos>=LCD_HEIGHT) return 1;

 //pixset=mode; //Pixel gesetzt oder nicht erstmal merken

 SetPosition(xpos,ypos); //Position auf 8 Pixel genau setzen

 by=ReadDisplay();       //8 Pixel lesen

 tmp= ypos % 8; //Position des Pixels im Byte
 pixpos= 1 << tmp;             //An die richtige Position schieben



 if(mode==1 && by&pixpos) return 0; //Pixel hatte den Wert schon
 if(mode==0 && (by&pixpos)==0) return 0; //Pixel hatte den Wert schon

 if(mode==0) by&=~pixpos; //Pixel löschen
 else by|=pixpos; //Pixel setzen 

	
 SetPosition(xpos,ypos); //Position wird nach dem lesen verändert !
                         //Also neu setzen
 WriteDisplay(by);       //8 Pixel schreiben
 return 0;
}

//##########################################################
//Zustand des Pixels an Stelle x,y lesen
unsigned char GetPixel(unsigned char xpos, unsigned char ypos)
{
 int pixel;
 unsigned char by,tmp,pix;

 if(xpos>=LCD_WIDTH) return 1; //Punkt außerhalb der Anzeige
 if(ypos>=LCD_HEIGHT) return 1;

 SetPosition(xpos,ypos); //Position auf 8 Pixel genau setzen

 by=ReadDisplay();       //8 Pixel lesen

 tmp= ypos % 8; //Position des Pixels im Byte
 pix= 1 << tmp;                //An die richtige Position schieben

 if(by&pix) pixel=1; //war gesetzt
 else pixel=0;       //war nicht gesetzt

 return pixel;
}

//##########################################################
//Aus der X-Position die Chipnummer berechnen
unsigned char GetChipNumber(unsigned char xpos)
{
 return xpos/64; //Jeder Chip kontrolliert 64x64 Pixel
}

//##########################################################
//Chipnummer des Displays setzen
void SetChipNumber(unsigned char num)
{
 
 CS1_OFF(); CS2_OFF(); 
 
 if ( MAX_CHIPS > 2 ){
		CS3_OFF(); CS4_OFF();
 }
 
 switch(num)
  {
   case 0: CS1_ON(); break;
   case 1: CS2_ON(); break;
   case 2: CS3_ON(); break;
   case 3: CS4_ON(); break;
  }
 NOP(); 
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

//##########################################################
//Spaltennummer (X-Position) des Chips setzen
unsigned char SetRow(unsigned char xpos)
{
 unsigned char offs;

  RS_OFF(); //Instruction setzen

//Spaltenoffset im Chip berechnen
 offs=xpos % 64; //Rest von xpos durch 64 bestimmen

 WriteDisplay(0x40|offs); //Spaltenummer setzen
 RS_ON(); //Zurück auf Daten
 return 0;
}

//##########################################################
//Gewünschte Koordinate auf dem Display einstellen
unsigned char SetPosition(unsigned char xpos, unsigned char ypos)
{
 unsigned char chip,page;

 if(xpos>=LCD_WIDTH) return 1; //Punkt außerhalb der Anzeige
 if(ypos>=LCD_HEIGHT) return 1;

 chip=GetChipNumber(xpos);
 page=GetPageNumber(ypos);

 SetChipNumber(chip); //Erst den Chip auswählen
 SetPageNumber(page); //Dann die Page setzen
 SetRow(xpos);        //Zum Schluß die X-Position
 return 0;
}

//##########################################################
//An jeden KS0108 muß der DisplayOn Befehl gesendet werden
void DisplayOn(void)
{
 unsigned char i;

 INIT_CONTROL_PINS();

 E_OFF();

 for(i=0; i< MAX_CHIPS; i++)
  {
   SetChipNumber(i); 
    RS_OFF(); //Instruction senden setzen
   WriteDisplay(0x3F); //Befehl Display On
   RS_ON(); //Zurück auf Daten
  } 

}
/* Test only*/
void DisplayOff(void)
{
 unsigned char i;

 //INIT_CONTROL_PINS();

 E_OFF();

 for(i=0; i< MAX_CHIPS; i++)
  {
   SetChipNumber(i); 
    RS_OFF(); //Instruction senden setzen
   WriteDisplay(0x3E); //Befehl Display Off
   RS_ON(); //Zurück auf Daten
  } 

}

//##########################################################
void ClearScreen(void)
//##########################################################
{

 unsigned char i,j,k;

 k=0;
 DisplayOff();
 do
  {
   SetChipNumber(k); 

   for(j=0; j<8; j++)
    {
     SetPageNumber(j);
     RS_ON();
     RW_OFF(); //Display schreiben setzen

     WRITE_DATA(0);
     for(i=0; i<64; i++)
      { E_ON(); 
        E_OFF(); 
      }
    }

  k++;
 }while(k< MAX_CHIPS);
 DisplayOn();

}


