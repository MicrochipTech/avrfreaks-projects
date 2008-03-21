#ifndef MONITOR_H_
#define MONITOR_H_

#define Mck 	12000000
#define Tpm 	(Mck/1000000)

#define BOUD    56000

#define UBRR_val 	((Mck/(16 * BOUD)) - 1)

//--------- PORTC -----------
#define TMo     5 // SCK

//--------- PORTD -----------
#define TMi0     2 // INT0
#define TMi1     3 // INT1
//---------- ROPTB ----------
#define LED	    0





#endif /*MONITOR_H_*/
