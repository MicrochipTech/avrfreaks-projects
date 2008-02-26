//************************************************************************
// KS108.H
// LowLevel Routinen für LCD-Displays mit KS0108
//
// Hier für Displaytech 64240A
//
// hk@holger-klabunde.de
// http://www.holger-klabunde.de
// 11.10.2003
// Compiler WinAVR
//************************************************************************

#ifndef KS108_H
#define KS108_H

//#####################################################
//# Number of NOP's below is based on 11.059MHz Clock #
//#####################################################

#define LCD_WIDTH	128  //Display Breite
#define LCD_HEIGHT	64   //Display Höhe
#define MAX_CHIPS	2 	 //Wieviel Chips gibt es. Jeder kontrolliert 64x64 Pixel!

// We use PORTA for the databus and PC0-5 as control pins Peter Menges 3.4.2007

#define INIT_CONTROL_PINS() { DDRA=0xFF; DDRC=0xFF; RSTB_ON(); } //control pins to output

#define DATA_PORT	PORTA
#define DATA_DIR_IN()  	{ DDRA=0x00; DATA_PORT=0xFF; } // set io-pins to inputs with pullups
#define DATA_DIR_OUT()	{ DDRA=0xFF; DATA_PORT=0x00;} // set io-pins to outputs 
                                                                 // see Manual
#define READ_DATA()   	PINA	  // read PIN, ! NOT ! PORT
#define WRITE_DATA(a)  	DATA_PORT=(a); // write to data port

// Zu meiner Erinnerung:
// Im Assemblerlisting wurde PORTF gelesen, modifiziert, und zurückgeschrieben.
// Wieso kein kürzeres "sbi PORTF,BIT" ??? Datenblatt lesen !
// sbi/cbi funktionieren nur mit SFR's von 0x00 bis 0x1F. PORTF liegt darüber.
// Es bringt jetzt aber hier auch nichts PORTF und PORTD zu tauschen (habs versucht).
// Das was man dann gewinnt muß man in Wait_for_E() wieder draufgeben weil das Timing
// für "E_ON/E_OFF" nicht mehr stimmt.
//
// Für andere Projekte sollte man aber in Erinnerung behalten das PORTF nicht für
// extrem schnelle Anwendungen geeignet ist ;)
/* COMPAT ONLY! 3.4.2007 Me*/
#define sbi(port,bit)  	   (port) |= (1 << (bit))
#define cbi(port,bit)  	   (port) &= ~(1 << (bit))
/* COMPAT ENDS*/

#define RS_PIN		2		// RS_PIN ist der D/I Pin am Display (Instruction or data ) Instruction = Low
#define RS_PORT		PORTC
#define RS_ON()	       	{ sbi(RS_PORT,RS_PIN); }
#define RS_OFF()       	{ cbi(RS_PORT,RS_PIN); }

#define RW_PIN		3
#define RW_PORT		PORTC
#define RW_ON()	       	{ sbi(RW_PORT,RW_PIN); NOP(); NOP(); }
#define RW_OFF()       	{ cbi(RW_PORT,RW_PIN); NOP(); NOP(); } //min 140ns

#define E_PIN		4
#define E_PORT		PORTC
#define E_ON()	       	{ sbi(E_PORT,E_PIN); NOP(); } //min 450ns
#define E_OFF()       	{ cbi(E_PORT,E_PIN); NOP(); } //min 450ns

#define RSTB_PIN	5  // RESET PIN /RST == Low active
#define RSTB_PORT	PORTC
#define RSTB_ON()      	sbi(RSTB_PORT,RSTB_PIN);
#define RSTB_OFF()     	cbi(RSTB_PORT,RSTB_PIN);

#define CS1_PIN		0
#define CS1_PORT	PORTC
#define CS1_ON()      	sbi(CS1_PORT,CS1_PIN);
#define CS1_OFF()     	cbi(CS1_PORT,CS1_PIN);

#define CS2_PIN		1
#define CS2_PORT	PORTC
#define CS2_ON()      	sbi(CS2_PORT,CS2_PIN);
#define CS2_OFF()     	cbi(CS2_PORT,CS2_PIN);
// Not yet, we only have 128x64 3.4.2007 Me
#define CS3_PIN		2
#define CS3_PORT	PORTC
#define CS3_ON()      	sbi(CS3_PORT,CS3_PIN);
#define CS3_OFF()     	cbi(CS3_PORT,CS3_PIN);

#define CS4_PIN		3
#define CS4_PORT	PORTC
#define CS4_ON()      	sbi(CS4_PORT,CS4_PIN);
#define CS4_OFF()     	cbi(CS4_PORT,CS4_PIN);

extern unsigned char ReadDisplay(void);
extern void WriteDisplay(unsigned char dat);
extern unsigned char SetPixel(unsigned char xpos, unsigned char ypos, unsigned char mode);
extern unsigned char GetPixel(unsigned char xpos, unsigned char ypos);
extern unsigned char GetChipNumber(unsigned char xpos);
extern void SetChipNumber(unsigned char num);
extern unsigned char GetPageNumber(unsigned char ypos);
extern unsigned char SetPageNumber(unsigned char num);
extern unsigned char SetRow(unsigned char xpos);
extern unsigned char SetPosition(unsigned char xpos, unsigned char ypos);
extern void DisplayOn(void);
extern void ClearScreen(void);
/*extern unsigned char CheckBusy(void); Not used*/

#endif
