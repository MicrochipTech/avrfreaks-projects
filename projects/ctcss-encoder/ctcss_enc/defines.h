/*******************************************************************
 * defines.h
 * written by      Ralf Meeh 26.04.2009
 * last changed by Ralf Meeh 26.04.2009
 * Function:       Definitionen, meist CPU abh�ngig
 *******************************************************************/

#ifndef F_CPU 
#define F_CPU 7372800.0
#endif



// Grundeinstellung Timer0 f�r Zeitbasis 1ms
#define COUNT_0			(long)(255 - ((F_CPU/1000)/64)+1)+3

//F�r UART
#define UART0_Baud		9600.00           //9600 Baudrate
// Init. Schnittstelle mit Rundung
#define UBRRwert0		(F_CPU/(16.0*UART0_Baud) +0.5)-1.0   

// Zeitgeber Daten f�r Timer0
#define PRESC_STOP 		0
#define PRESC_CPU  		1
#define PRESC_8    		2
#define PRESC_64   		3
#define PRESC_256  		4
#define PRESC_1024 		5
#define PRESC_EXTF 		6
#define PRESC_EXTR 		7
