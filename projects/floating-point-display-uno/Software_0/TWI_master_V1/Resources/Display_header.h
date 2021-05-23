




long I_number;															//Integer number received from UNO as string or binary
float flt_num;															//Floating point number received from UNO as string or binary
volatile char cr_keypress = 0;											//Set to one when carriage return terminates numerical string entry.

volatile int buf_ptr;													//Points to display_buff digits
char display_buf[12];													//Array used to drive display
volatile char int_counter;												//Counts T0 overflow interrupts
volatile char transaction_type;											//Integer, floating point, string etc..


char * char_ptr;														//Addresses bytes in a floating point number
volatile float* f_num_ptr;												//Address floating point number
char flt_array[16];														//Holds displayed number in floating point form
char Non_exp_array[16];													//Display without exponent
int array_cntr;															//Points to characters in flt_array
volatile char Display_mode;												//Display a result or acquire data from UNO
volatile char intensity_control;
volatile char request_counter;											//Maximum number of times master will poll slave

/***********************************************************************/

#define setup_ATtiny_HW \
setup_watchdog;\
setup_IO;\
cal_device;



/***********************************************************************/
#define wdr()  __asm__ __volatile__("wdr")

#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCR |= (1 <<WDCE) | (1<< WDE);\
WDTCR = 0;


#define setup_IO \
MCUCR &= (~(1 << PUD));\
DDRA = 0;\
PORTA = 0xFF;\
DDRB = 0;\
PORTB = 0xFF;\
USIPP |= 1 << USIPOS;


#define cal_device \
eeprom_write_byte((uint8_t*)(EE_size - 3), OSCCAL);\
if ((eeprom_read_byte((uint8_t*)(EE_size - 2)) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)(EE_size - 2)) < 0xF0) && (eeprom_read_byte((uint8_t*)(EE_size - 2))\
== eeprom_read_byte((uint8_t*)(EE_size - 1)))) OSCCAL = eeprom_read_byte((uint8_t*)(EE_size - 2));






/*********************************DRIVERS for Least Significant Bits of a CA display**********************************************/
//IC is on the underside of the board
//and hosts the master TWI


#define	digit_4		PORTB |= (1 << PB6);
#define	digit_5		PORTB |= (1 << PB1);
#define	digit_6		PORTB |= (1 << PB2);
#define	digit_7		PORTB |= (1 << PB5);

#define	seg_a 	(1 << PB4)
#define	seg_b 	(1 << PB0)
#define	seg_c 	(1 << PA6)
#define	seg_d 	(1 << PA4)
#define	seg_e 	(1 << PA3)
#define	seg_f 	(1 << PB3)
#define	seg_g 	(1 << PA7)
#define	DP		(1 << PA5)


#define set_digit_drivers \
DDRA |= (1 << DDA3) | (1 << DDA4) | (1 << DDA5) | (1 << DDA6) | (1 << DDA7);\
DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4) | (1 << DDB5) | (1 << DDB6);

#define clear_digits	PORTB &= (~((1 << PB1) | (1 << PB2) | (1 << PB5) | (1 << PB6))); 


#define clear_display \
PORTA |= (seg_c | seg_d | seg_e | seg_g | DP);\
PORTB |= (seg_a | seg_b | seg_f);

#define clear_display_buffer	for(int m = 0; m<=3; m++)display_buf[m] = 0;


#define one_U 	PORTB &= ~(seg_b);
#define ONE_U 	PORTA &= ~(seg_e);
#define one_L 	PORTA &= ~(seg_c);
#define ONE_L 	PORTA &= ~(seg_e);
#define ONE		PORTA &= ~(seg_e); 
#define one 	PORTB &= ~(seg_b); PORTA &= ~(seg_c);
#define two 	PORTB &= (~(seg_a | seg_b )); PORTA &= (~(seg_d | seg_e | seg_g));
#define three 	PORTB &= (~(seg_a | seg_b)); PORTA &= (~(seg_c | seg_d | seg_g));
#define four 	PORTB &= (~(seg_b | seg_f)); PORTA &= (~(seg_c | seg_g));
#define five 	PORTB &= (~(seg_a | seg_f )); PORTA &= (~(seg_c | seg_d | seg_g));
#define six 	PORTB &= (~(seg_f)); PORTA &= (~(seg_c | seg_d | seg_e | seg_g));
#define seven 	PORTB &= (~(seg_a | seg_b)); PORTA &= (~(seg_c));
#define eight 	PORTB &= (~(seg_a | seg_b | seg_f));  PORTA &= (~(seg_c | seg_d | seg_e | seg_g));
#define nine	PORTB &= (~(seg_a | seg_b | seg_f)); PORTA &= (~( seg_c | seg_g));
#define zero	PORTB &= (~(seg_a | seg_b | seg_f )); PORTA &= (~( seg_c | seg_d | seg_e  ));
#define minus	PORTA &= (~(seg_g));

#define exponent	PORTB &= (~(seg_a | seg_f)); PORTA &= (~(seg_d | seg_e | seg_g ));

#define zero_point		zero; PORTA &= ~DP;
#define one_point		one; PORTA &= ~DP;
#define two_point		two;  PORTA &= ~DP;
#define three_point		three;  PORTA &= ~DP;
#define four_point		four;  PORTA &= ~DP;
#define five_point		five;  PORTA &= ~DP;
#define six_point		six;  PORTA &= ~DP;
#define seven_point		seven;  PORTA &= ~DP;
#define eight_point		eight;  PORTA &= ~DP;
#define nine_point		nine;  PORTA &= ~DP;
#define minus_point		minus; PORTA &= ~DP;

/*****************************************************************************************/

/*********************************DRIVERS for Least Significant Bits of a CC display**********************************************/
//IC is on the underside of the board
//and hosts the master TWI

/*
#define	digit_4		PORTB &= (~(1 << PB6));
#define	digit_5		PORTB &= (~(1 << PB1));
#define	digit_6		PORTB &= (~(1 << PB2));
#define	digit_7		PORTB &= (~(1 << PB5));

#define	seg_a 	(1 << PB4)
#define	seg_b 	(1 << PB0)
#define	seg_c 	(1 << PA6)
#define	seg_d 	(1 << PA4)
#define	seg_e 	(1 << PA3)
#define	seg_f 	(1 << PB3)
#define	seg_g 	(1 << PA7)
#define	DP		(1 << PA5)


#define set_digit_drivers \
DDRA |= (1 << DDA3) | (1 << DDA4) | (1 << DDA5) | (1 << DDA6) | (1 << DDA7);\
DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4) | (1 << DDB5) | (1 << DDB6);


#define clear_digits	PORTB |= ((1 << PB1) | (1 << PB2) | (1 << PB5) | (1 << PB6));


#define clear_display \
PORTA &= (~((seg_c | seg_d | seg_e | seg_g | DP)));\
PORTB &= (~((seg_a | seg_b | seg_f)));


#define clear_display_buffer	for(int m = 0; m<=3; m++)display_buf[m] = 0;

#define one_U 	PORTB |= seg_b;
#define ONE_U 	PORTA |= seg_e;
#define one_L 	PORTA |= seg_c;
#define ONE_L 	PORTA |= seg_e;
#define ONE		PORTA |= seg_e;
#define one 	PORTB |= seg_b; PORTA |= seg_c;
#define two 	PORTB |= seg_a | seg_b; PORTA |= seg_d | seg_e | seg_g;
#define three 	PORTB |= seg_a | seg_b; PORTA |= seg_c | seg_d | seg_g;
#define four 	PORTB |= seg_b | seg_f; PORTA |= seg_c | seg_g;
#define five 	PORTB |= seg_a | seg_f; PORTA |= seg_c | seg_d | seg_g;
#define six 	PORTB |= seg_f; PORTA |= seg_c | seg_d | seg_e | seg_g;
#define seven 	PORTB |= seg_a | seg_b; PORTA |= seg_c;
#define eight 	PORTB |= seg_a | seg_b | seg_f;  PORTA |= seg_c | seg_d | seg_e | seg_g;
#define nine	PORTB |= seg_a | seg_b | seg_f; PORTA |= seg_c | seg_g;
#define zero	PORTB |= seg_a | seg_b | seg_f ; PORTA |= seg_c | seg_d | seg_e ;
#define minus	PORTA |= seg_g;


#define exponent	PORTB |= (seg_a | seg_f); PORTA |= (seg_d | seg_e | seg_g );


#define zero_point		zero; PORTA |= DP;
#define one_point		one; PORTA |= DP;
#define two_point		two;  PORTA |= DP;
#define three_point		three;  PORTA |=DP;
#define four_point		four;  PORTA |= DP;
#define five_point		five;  PORTA |= DP;
#define six_point		six;  PORTA |= DP;
#define seven_point		seven;  PORTA |= DP;
#define eight_point		eight;  PORTA |= DP;
#define nine_point		nine;  PORTA |= DP;
#define minus_point		minus; PORTA |= DP;

*/
