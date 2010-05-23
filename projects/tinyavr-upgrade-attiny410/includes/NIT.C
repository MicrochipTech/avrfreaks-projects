//*****************************************************************************
//
//	Grundinitialisierung des Tiny10
//
//*****************************************************************************
#include <iotiny10.h>

//*****************************************************************************
//	Initialisierung nach RESET
//  Input:	-
//	Output:	-
//*****************************************************************************
void INIT(void)
//unsigned char INIT(void)
{
//unsigned char uc_Start;
//   CCP = 0xd8;    //CPU Change Protection - 4Taktezyklen freigeben, um CLKMSR zu aendern
//   CLKMSR = 0x00; //8MHz interner CLK verwenden

//   CCP = 0xd8;    //CPU Change Protection - 4Taktezyklen freigeben, um CLKPSR zu aendern
//   CLKPSR = 0x03; //Vorteiler DIV 8 -> 1MHz Takt

//   uc_Start = RSTFLR;  
//   RSTFLR = 0;    //Resetflags zurücksetzen
  
//   EICRA = 0x00;  //extended ext ints
//   EIMSK = 0x00;
//   EIFR  = 0x00;

//   PCICR = 0x00; //Pin Change INT
//   PCIFR = 0x00;
//   PCMSK = 0x00;
       
   // Input/Output Ports initialization as input
   // Port B
   DDRB   = 0x0F;
   PUEB   = 0x0F; //Pull up
   PORTCR = 0x00;
   PORTB  = 0x00;
   
   //ADC
   DIDR0 = 0x00; //fuer Analog-Inputpin Digital-IO ausschalten
   
   // Analog Comparator initialization
   // Analog Comparator: Off
   ACSR=0x80;
   
//  return(uc_Start);
}


