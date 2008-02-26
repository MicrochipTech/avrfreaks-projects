//************************************************************************
// Circle.c
//
// Um nicht zuviel mit sin() cos() zu rechnen, berechnet man nur einen
// Viertelkreis und trägt die Punkte geeignet in alle vier Quadranten ein.
// Das reduziert die Zeit zum zeichnen des Kreises auf fast 1/4
// Schneller geht es nur noch wenn man eine Tabelle benutzt.
//
// Die Anzahl der Berechnungen kann man nochmal minimieren wenn man auf 
// den Radius optimiert. Bei Radius=30 braucht man auch nur 30 Punkte auf 90°.
// Also theoretisch nur 90/30 Grad ein Punkt. Dann franst der Kreis aber
// oft aus. Besser ein paar Punkte mehr. Manchmal braucht man gar keinen
// voll gezeichneten Kreis. Dafür habe ich den Parameter steps eingebaut.
// Wenn er auf 1 steht wird immer ein voller Kreis gezeichnet. Bei einigen
// Radien reicht auch 2 oder 3 für einen voll gezeichneten Kreis. Da ist
// halt ausprobieren angesagt !
//
// hk@holger-klabunde.de
// http://www.holger-klabunde.de
// 08.10.2003
// Compiler WinAVR 3.3
//************************************************************************

#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#include "protos.h"

#define GRAD_TO_RAD (double)(M_PI / 180.0)

//***************************************************************************************
// Zeichnet einen Kreis auf Position xpos,ypos mit Radius radius
void Circle(unsigned int xpos, unsigned int ypos, unsigned int radius, unsigned char step)
//***************************************************************************************
{
 unsigned char i;
 long x,y;

 if(step==0) return; //Sonst gibts einen Hänger !

 for(i=0; i<=90; i+=step)
  {
   x=(int)((double)radius * cos((double)i * GRAD_TO_RAD));
   y=(int)((double)radius * sin((double)i * GRAD_TO_RAD));
   SetPixel((unsigned int)(xpos+x),(unsigned int)(ypos+y),1);
   SetPixel((unsigned int)(xpos-x),(unsigned int)(ypos+y),1);
   SetPixel((unsigned int)(xpos+x),(unsigned int)(ypos-y),1);
   SetPixel((unsigned int)(xpos-x),(unsigned int)(ypos-y),1);
  }
}

//***************************************************************************************
// Zeichnet einen gefüllten Kreis auf Position xpos,ypos mit Radius radius
void FillCircle(unsigned int xpos, unsigned int ypos, unsigned int radius, unsigned char step)
//***************************************************************************************
{
 unsigned char i;
 long x,y;

 if(step==0) return; //Sonst gibts einen Hänger !

// SetGCursor(xpos,ypos);
 
 for(i=0; i<=90; i+=step)
  {
   x=(long)((double)radius * cos((double)i * GRAD_TO_RAD));
   y=(long)((double)radius * sin((double)i * GRAD_TO_RAD));

//So geht es leider nicht :( Wäre schön einfach gewesen
//Der Kreis wird nicht ganz ausgefüllt. Wenn man step auf
//9 oder mehr erhöht gibts Strahlen aus der Mitte des Kreises.
//Sieht ganz nett aus. Dann oben auch SetGCursor() aktivieren
//  
//   DrawLineRel(x,y,1);
//   DrawLineRel(-x,y,1);
//   DrawLineRel(x,-y,1);
//   DrawLineRel(-x,-y,1);

//Dann eben so:
   DrawXLineLen((unsigned int)(xpos-x),(unsigned int)(2*x),(unsigned int)(ypos+y));
   DrawXLineLen((unsigned int)(xpos-x),(unsigned int)(2*x),(unsigned int)(ypos-y));
  }
}
