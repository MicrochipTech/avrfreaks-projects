//************************************************************************
// Draw.C
//
// Linien zeichnen, Strichbreite immer 1
// Rechtecke zeichnen und gefüllte Flächen
//
// Der Nullpunkt der Anzeige ist links oben
//
// hk@holger-klabunde.de
// http://www.holger-klabunde.de
// 08.20.2003
// Compiler AVR-GCC 3.2
//************************************************************************
#include <stdlib.h>
#include "protos.h"

//####################################################################
// Zeichnet eine Linie absolut von X1,Y1 bis X2,Y2
// Für horizontale, vertikale Linien besser die anderen Funktionen
// DrawXLine(), DrawYLine() benutzen !
// Gefunden im Internet. http://www.prog.cz/swag/swag/graphics/
void DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
//####################################################################
{
 long x,y,count,xs,ys,xm,ym;

 x=(long)x1; y=(long)y1;
 xs=(long)x2-(long)x1; ys=(long)y2-(long)y1;
 if(xs<0) xm=-1; else if(xs>0) xm=1; else xm=0;
 if(ys<0) ym=-1; else if(ys>0) ym=1; else ym=0;
// xs=abs(xs);  ys=abs(ys);
 if(xs<0) xs=-xs;
 if(ys<0) ys=-ys;

 SetPixel((unsigned int)x,(unsigned int)y,1);

 if(xs>ys) // Flache Linie <45 Grad
  {
   count=-(xs/2);

   while(x!=x2 )
    {
     count=count+ys;
     x=x+xm;

     if(count>0)
      {
       y=y+ym;
       count=count-xs;
      }

     SetPixel((unsigned int)x,(unsigned int)y,1);
    }
   }
  else // Steile Linie >=45 Grad
   {
    count=-(ys/2);

    while(y!=y2)
	 {
      count=count+xs;
      y=y+ym;

      if(count>0)
       {
        x=x+xm;
        count=count-ys;
       }

      SetPixel((unsigned int)x,(unsigned int)y,1);
     }
   }
}

//####################################################################
// Zeichnet eine horizontale Linie absolut von X1 bis X2 auf Zeile Y
void DrawXLine(unsigned int x1, unsigned int x2, unsigned int y)
{
 unsigned int i,xstart,xend;

 if(x2>x1) //Immer von links nach rechts zeichnen
  { xstart=x1; xend=x2; }
 else
  { xstart=x2; xend=x1; }
 
  
 for(i=xstart; i<=xend; i++)
  {
   SetPixel(i,y,1);
  }
}

//##################################################################
// Zeichnet eine vertikale Linie absolut von Y1 bis Y2 auf Spalte X
void DrawYLine(unsigned int y1, unsigned int y2, unsigned int x)
{
 unsigned int i,ystart,yend;

 if(y2>y1)
  { ystart=y1; yend=y2; }
 else
  { ystart=y2; yend=y1; }

 for(i=ystart; i<=yend; i++)
  {
   SetPixel(x,i,1);
  }
}

//#########################################################################
// Zeichnet eine horizontale Linie mit Länge len ab Position X auf Zeile Y
// Von links nach rechts
void DrawXLineLen(unsigned int x, unsigned int len, unsigned int y)
{ DrawXLine(x,x+len-1,y); }

//#########################################################################
// Zeichnet eine vertikale Linie mit Länge len ab Position Y auf Spalte X 
// Von oben nach unten
void DrawYLineLen(unsigned int y, unsigned int len, unsigned int x)
{ DrawYLine(y,y+len-1,x); }

//#########################################################################
// Zeichnet ein Rechteck mit Breite width, Höhe height ab Position X,Y 
void DrawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
 DrawXLineLen(x,width,y);
 DrawXLineLen(x,width,y+height-1);
 DrawYLineLen(y,height,x);
 DrawYLineLen(y,height,x+width-1);
}

//#########################################################################
// Füllt ein Rechteck mit Breite width, Höhe height ab Position X,Y 
void FillRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
 unsigned int i;

 for(i=y; i<(y+height); i++) DrawXLineLen(x,width,i);
}
