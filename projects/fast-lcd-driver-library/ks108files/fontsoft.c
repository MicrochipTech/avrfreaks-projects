//#################################################################
// fontsoft.c
//
// Text mit Software Fonts schreiben
//
// Bisher für Fonts bis 8 Pixel Breite und bis zu 16 Pixel Höhe
// Die Zeichensätze der Fonts sind nicht alle gleich !
//
// Die Fonts ohne xxxfv am Ende enthalten auch Zeichen unter 0x20
// Herzchen, Smiley usw
//
// Die Fonts werden ab Position x,y von oben nach unten gezeichnet.
// Man gibt also die linke obere Ecke des Buchstabens an.
//
// Bei vertikaler Zeichweise (90° nach links gekippt) wird von unten
// nach oben geschrieben.
//
// Aufbau der Fonts mit bis zu 8 Pixel Höhe :
// Jedes Byte gehört zu einer Spalte des Buchstabens.
// Das erste Byte ist ganz links und D7 ist das unterste Pixel.
//
// Aufbau der Fonts mit 9 Pixel bis 16 Pixel Höhe:
// Jeweils zwei aufeinander folgende Bytes gehören zu einer Spalte
// des Buchstabens. Die ersten 8 Byte sind der obere, die zweiten
// 8 Byte der untere Teil. Das 1. und 9. Byte bilden die linke Kante.
// D7 des jeweiligen Bytes ist das unterste Pixel.
//
// Es handelt sich um DOS-Fonts !
//
// hk@holger-klabunde.de
// http://www.holger-klabunde.de
// 08.10.2003
// Compiler AVR-GCC 3.2
//#################################################################
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "protos.h"

unsigned char fontwidth,fontheight;   //Breite,Höhe in Pixeln
unsigned char *font;

//Namensgebung der Fonts: f[Höhe]x[Breite]
extern prog_uchar f8x6[];
/*extern prog_uchar f8x8a[];
extern prog_uchar f8x8b[];
extern prog_uchar f16x8a[];
extern prog_uchar f16x8b[];
extern prog_uchar f16x8c[];

extern prog_uchar f6x4fv[]; 
extern prog_uchar f6x5fv[]; 
extern prog_uchar f8x6fv[]; 
extern prog_uchar f8x8fv[]; */

//########################################################################
void SetFont(unsigned char num)
//Die Zuordnung müßte mal nach Größe umsortiert werden
//########################################################################
{
 switch(num)
  {
   case 1: fontwidth=6; fontheight=8; font=f8x6; break;
   /*case 2: fontwidth=8; fontheight=8; font=f8x8b; break;
   case 3: fontwidth=8; fontheight=8; font=f8x8b; break;
   case 4: fontwidth=8; fontheight=16; font=f16x8a; break;
   case 5: fontwidth=8; fontheight=16; font=f16x8b; break;
   case 6: fontwidth=8; fontheight=16; font=f16x8c; break;
   case 7: fontwidth=4; fontheight=6; font=f6x4fv; break;
   case 8: fontwidth=5; fontheight=6; font=f6x5fv; break;
   case 9: fontwidth=6; fontheight=8; font=f8x6fv; break;
   case 10: fontwidth=8; fontheight=8; font=f8x8fv; break;*/
   default: fontwidth=6; fontheight=8; font=f8x6;break;
  }
}

//########################################################################
//Schreibt ein einzelnes Zeichen an Position x,y
void LCDSoftChar(unsigned char ch, unsigned int xpos, unsigned int ypos)
//########################################################################
{
unsigned char by,mask;
unsigned char i,j;
 
 for(i=0; i<fontwidth; i++) //Über die Breite des Zeichens
  {
   if(fontheight<=8)
    {
     by=pgm_read_byte(&font[ch*fontwidth+i]); //Ein Byte des Zeichens holen
     mask=0x01;               //Bei D0 anfangen

//Bei den kleinen Fonts wie 6x4 könnte man nicht benutzte Bits
//nicht schreiben um die Textzeilen eng übereinander zu plazieren,
//bzw. um angrenzende Grafikbereiche nicht zu löschen.
//Wenn man immer von oben nach unten schreibt geht es aber auch so.
     for(j=0; j<8; j++)
      {
       SetPixel(xpos+i,ypos+j,(by&mask));
       mask<<=1; //Nächstes Bit
      }//for j
    }//if(fontheight<=8)
    
   if(fontheight>8 && fontheight<=16)
    {
     by=pgm_read_byte(&font[ch*2*fontwidth+i]); //Ein Byte des Zeichens holen oberer Teil

     mask=0x01;               //Bei D0 anfangen
     for(j=0; j<8; j++)
      {
       SetPixel(xpos+i,ypos+j,(by&mask));
       mask<<=1; //Nächstes Bit
      }//for j

     by=pgm_read_byte(&font[ch*2*fontwidth+i+8]); //Ein Byte des Zeichens holen unterer Teil

     mask=0x01;               //Bei D0 anfangen
     for(j=0; j<8; j++)
      {
       SetPixel(xpos+i,ypos+j+8,(by&mask));
       mask<<=1; //Nächstes Bit
      }//for j
    }//if(fontheight>8....

  }//for i
}

//########################################################################
//Schreibt einen Text an Position x,y
//Mit automatischem Zeilenvorschub falls der Text zu lang war.
//Noch keine Steuerzeichen wie TAB,CR,LF integriert
void LCDSoftText(char *__putstr, unsigned int xpos, unsigned int ypos)
//########################################################################
{
 char __ps;
 unsigned int xp,yp;

 if(fontwidth==0) return; //Kein Font ausgewählt

 xp=xpos; yp=ypos;
 
 while((__ps = *__putstr))
  {
   __putstr++;
   if (__ps== 0) break;
   LCDSoftChar(__ps,xp,yp);
   xp+=fontwidth; //Nächstes Zeichen
   if(xp>=LCD_WIDTH) { xp=0; yp+=fontheight; } //Nächste Zeile
  }
}

//########################################################################
//Zeigt eine Übersicht der im Font enthaltenen Zeichen
void ShowSoftFont(unsigned char num)
//########################################################################
{
 unsigned int i,xp,yp;
 
 ClearScreen();

 xp=0; yp=0;
 SetFont(num);
 
 for(i=0; i<255; i++)
  {
   LCDSoftChar(i,xp,yp);
   xp+=fontwidth; //Nächstes Zeichen
   if(xp>=LCD_WIDTH) { xp=0; yp+=fontheight; } //Nächste Zeile
  }
}

//########################################################################
//Zeigt eine Übersicht der im Font enthaltenen Zeichen
void ShowSoftFontV(unsigned char num)
//########################################################################
{
 unsigned int i,xp,yp;
 
 ClearScreen();

 xp=0; yp=0;
 SetFont(num);
 
 for(i=0; i<255; i++)
  {
   LCDSoftCharV(i,xp,yp);
   yp-=fontwidth; //Nächstes Zeichen
   if(yp>=LCD_HEIGHT) { yp=LCD_HEIGHT-1; xp+=fontheight; } //Nächste Zeile
  }
}

//########################################################################
//Schreibt einen Text 90° gekippt an Position x,y
//Mit automatischem Zeilenvorschub falls der Text zu lang war.
//Noch keine Steuerzeichen wie TAB,CR,LF integriert
void LCDSoftTextV(char *__putstr, unsigned int xpos, unsigned int ypos)
//########################################################################
{
 char __ps;
 unsigned int xp,yp;

 if(fontwidth==0) return; //Kein Font ausgewählt

 xp=xpos; yp=ypos;
 
 while((__ps = *__putstr))
  {
   __putstr++;
   if (__ps== 0) break;
   LCDSoftCharV(__ps,xp,yp);
   yp-=fontwidth; //Nächstes Zeichen
   if(yp>=LCD_HEIGHT) { yp=LCD_HEIGHT-1; xp+=fontheight; } //Nächste Zeile
  }
}

//########################################################################
//Schreibt ein einzelnes Zeichen 90° gekippt an Position x,y
void LCDSoftCharV(unsigned char ch, unsigned int xpos, unsigned int ypos)
//########################################################################
{
unsigned char by,mask;
unsigned char i,j;
 
 for(i=0; i<fontwidth; i++) //Über die Breite des Zeichens
  {
   if(fontheight<=8)
    {
     by=pgm_read_byte(&font[ch*fontwidth+i]); //Ein Byte des Zeichens holen
     mask=0x01;               //Bei D0 anfangen

     for(j=0; j<8; j++)
      {
       SetPixel(xpos+j,ypos-i,(by&mask));
       mask<<=1; //Nächstes Bit
      }//for j
    }//if(fontheight<=8)
    
   if(fontheight>8 && fontheight<=16)
    {
     by=pgm_read_byte(&font[ch*2*fontwidth+i]); //Ein Byte des Zeichens holen oberer Teil

     mask=0x01;               //Bei D0 anfangen
     for(j=0; j<8; j++)
      {
       SetPixel(xpos+j,ypos-i,(by&mask));
       mask<<=1; //Nächstes Bit
      }//for j

     by=pgm_read_byte(&font[ch*2*fontwidth+i+8]); //Ein Byte des Zeichens holen unterer Teil

     mask=0x01;               //Bei D0 anfangen
     for(j=0; j<8; j++)
      {
       SetPixel(xpos+j+8,ypos-i,(by&mask));
       mask<<=1; //Nächstes Bit
      }//for j
    }//if(fontheight>8....

  }//for i
}

