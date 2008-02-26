//************************************************************************
// Datei: display.c
// Hauptprogramm
//
// Demoprogramm um Text und Grafik auf
// KS108 Grafik-LCD-Display anzuzeigen
// Display: Displaytech 64240A 240x64 Pixel
//
// Erläuterungen zu den Funktionen stehen in den entsprechenden
// *.c Dateien.
//
// hk@holger-klabunde.de
// http://www.holger-klabunde.de
// 04.10.2003
// Compiler WinAVR 3.3
//************************************************************************
// Prozessor: ATMega32
//************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "protos.h"




int main(void)
{
    int i;
	

 DDRD=0xff;	/* Output */
 (PORTD) |= (1 << (5)); // LED1 ON Debugging purposes only//
 
  

 InitDisplay();

//Beginn Endlosschleife
 while(1)
  {
    
    ClearScreen();
    UpdateDisplay();
    
DrawRect(0, 0, 127, 63); //Doppelter Rahmen zeichnen
DrawRect(3,3,123,59);

UpdateDisplay();
Delay1ms(1000);

DrawLine(4,4,123,59);
DrawLine(123,4,4,59);

UpdateDisplay();
Delay1ms(1000);
Circle(63, 32, 25, 2);

UpdateDisplay();
Delay1ms(1000);

FillCircle(63, 32, 20, 2);
UpdateDisplay();
Delay1ms(1000);
ClearScreen();
UpdateDisplay();
Delay1ms(1000);
SetFont(1);
LCDSoftText("ES FUNZT!!!!",10,25);
UpdateDisplay();
Delay1ms(1000);



 
   } //Endlosschleife
}

//###################################################################################
//Nicht sonderlich genau !!
void Delay1ms(unsigned int time)
//###################################################################################
{
	int part;
 for ( ;time > 200; time -= 200 ){
 part = time - 200;
 
_delay_ms((double) part);
}
	_delay_ms((double) time);
 
}


