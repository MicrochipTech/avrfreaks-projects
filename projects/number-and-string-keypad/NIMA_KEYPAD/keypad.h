



#ifndef _KEYPAD_4X4
#define _KEYPAD_4X4

#define	keypad_port	PORTA
#define keypad_pin	PINA
#define keypad_ddr	DDRA

#define max_str		15


#define end_char	13
#define enter		13

#define	up			4
#define	down		8
#define more		16
#define next_char	15
#define back		12
#define del_char	12


///////////////////////////////////////////////
//
//						 |	    |      |      |
//		PORTX.0		-----+------+------+------+--- 
//						 |	    |      |      |
//						 |      |	   |	  |	
//						 |      |	   |	  |
//		PORTX.1		-----+------+------+------+--- 
//						 |	    |      |      |
//						 |	    |      |      |
//						 |	    |      |      |
//		PORTX.2		-----+------+------+------+--- 
//						 |	    |      |      |
//						 |	    |      |      |
//						 |	    |      |      |
//		PORTX.3		-----+------+------+------+--- 
//						 |	    |      |      |
//
//					PORTX.4 PORTX.5 PORTX.6 PORTX.7
//
//
	
	
	
	
	void 	keypad_init(void);	
	char	keypad_c(void);
	char* 	keypad_s(unsigned char x,unsigned char y); 









#endif

