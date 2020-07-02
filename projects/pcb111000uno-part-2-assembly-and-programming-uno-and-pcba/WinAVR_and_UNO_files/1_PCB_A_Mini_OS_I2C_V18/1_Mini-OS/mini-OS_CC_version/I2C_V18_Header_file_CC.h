

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>


void start_T2_for_ATMEGA_168_cal(char);

void I2C_master_transmit(char);
char I2C_master_receive(char);
void manual_cal_PCB_A_device(void);
void I2C_Tx_2URNs_from_IO (void);
void I2C_Tx_Uarithmetic_OP (void);
void I2C_Tx_Compile_tables(void);
void I2C_Tx_accumulator_1(void);
void I2C_Tx_accumulator_2(void);
void Message_from_the_OS(void);
void basic_clock (void);
void multi_mode_clock(void);
void stop_watch(void);


volatile char payload_size;							
volatile int disp_ptr;
volatile char Char_received, exit_loop = 0, display_mask;		
volatile char mode=1, timer_mode, inc_secs, inc_mins;				
volatile char display_buf[8], clock_buf[8], stop_watch_buf[8],strobe[8];
volatile int buf_ptr, mode_C_ptr;
volatile char T1_ovf_flag=0, timer_2_counter;
volatile signed char output_2;
volatile int Ten_mS_tick = 41;
unsigned long number[6];
signed char exponent[3];

volatile char entry_point;
char Sc_Num_string[16];
int Sc_Num_string_length, Sc_Num_string_pointer, display_char_skip_counter;

volatile int EA_counter, EA_buff_ptr;
int buffer[41];	
volatile char T1_OVF;
volatile long error_SUM;
volatile char MUX_cntl, T0_interupt_cnt;


char I2C_data[10];
char  display_backup[9];
signed char exponent_BKP[2];
int RN;
char keypres_counter_old;
char overflow;


char Op;
unsigned long RHSofDP;
signed char expnt_result;
char result[4];
int accumlator;
int interim_result;

char OSCCAL_WV;
char OSCCAL_DV;
volatile char cal_mode; 			//Defines number of averages used when measuring osccal_error	

char clock_flag;
char PIC_cmd;
char Ten_mS_tick_counter;



#define setup_watchdog;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define wdr()  __asm__ __volatile__("wdr")

#define cal_PCB_A_328 \
eeprom_write_byte((uint8_t*)0x3FD, OSCCAL);\
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);}


#define initialise_IO;\
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;\


#define Reset_UNO_low \
PORTC &= (~(1 << PC3));\
DDRC |= (1 << DDC3);

#define Reset_UNO_high \
DDRC &= (~(1 << DDC3));\
PORTC |= (1 << PC3);


#define initialise_Arithmetic_variables; \
exponent_BKP[0]=0; exponent_BKP[1]=0;\
expnt=0;\
Op = 0;\
accumlator=0;\
RN=0;keypres_counter_old=0; overflow=0;\
RHSofDP = 0;expnt_result = 0;




#define two_fifty_mS_delay; timer_T1_sub_with_interrupt(T1_delay_250ms);while (T1_ovf_flag == 0);T1_ovf_flag = 0;
#define T1_delay_250ms 5,0xF861
#define two_fifty_mS_delay_with_interrupts; TIMSK1 |= (1 << TOIE1);two_fifty_mS_delay; TIMSK1 &= (~(1 << TOIE1));

#define one_sec_delay; timer_T1_sub_with_interrupt(T1_delay_1sec);while (T1_ovf_flag == 0);T1_ovf_flag = 0;
#define T1_delay_1sec 5,0xE17B
#define one_sec_delay_with_interrupts; TIMSK1 |= (1 << TOIE1);one_sec_delay; TIMSK1 &= (~(1 << TOIE1));


#define Initialise_dislay_brightness; \
if ((eeprom_read_byte((uint8_t*)(0x3FB)) != 0xFF) &&\
(eeprom_read_byte((uint8_t*)(0x3FB)) != 0xFE) &&\
(eeprom_read_byte((uint8_t*)(0x3FB)) != 0xFD))\
eeprom_write_byte((uint8_t*)(0x3FB), 0xFD);\
T0_interupt_cnt = 0;




#define	digit_3		PORTB &= (~(1 << PB0));
#define	digit_2		PORTB &= (~(1 << PB2));
#define	digit_1		PORTB &= (~(1 << PB3));
#define	digit_0		PORTB &= (~(1 << PB4));
#define	toggle_digit_0	PORTB ^= (1 << PB4);


#define	digit_7		PORTB &= (~(1 << PB5));
#define	digit_6		PORTC &= (~(1 << PC0));
#define	digit_5		PORTC &= (~(1 << PC1));
#define	digit_4		PORTC &= (~(1 << PC2));

#define	seg_a 	(1 << PB1)
#define	seg_b 	(1 << PD2)
#define	seg_c 	(1 << PD3)
#define	seg_d 	(1 << PD4)
#define	seg_e 	(1 << PD5)
#define	seg_f 	(1 << PD6)
#define	seg_g 	(1 << PD7)

#define set_digit_drivers;\
DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB4) | (1 << DDB5);\
DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2);\
DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);

#define clear_digits; PORTB |= ((1 << PB0) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5));\
PORTC |= ((1 << PC0) | (1 << PC1) | (1 << PC2));

#define clear_display; PORTB &= (~(seg_a ));\
PORTD &= (~(seg_b | seg_c | seg_d | seg_e | seg_f | seg_g));

#define clear_display_buffer; for(int m = 0; m<=7; m++)display_buf[m] = 0;


#define one_U 	PORTD |= (seg_b);
#define ONE_U 	PORTD |= (seg_f);
#define one_L 	PORTD |= (seg_c);
#define ONE_L 	PORTD |= (seg_e);
#define ONE		PORTD |= (seg_e | seg_f);
#define one 	PORTD |= (seg_b | seg_c);
#define two 	PORTB |= (seg_a); PORTD |= (seg_b | seg_d | seg_e | seg_g);
#define three 	PORTB |= (seg_a); PORTD |= (seg_b | seg_c | seg_d | seg_g);
#define four 	PORTD |= (seg_b | seg_c | seg_f | seg_g);
#define five 	PORTB |= (seg_a); PORTD |= (seg_c | seg_d | seg_f | seg_g);
#define six 	PORTD |= (seg_c | seg_d | seg_e | seg_f | seg_g);
#define seven 	PORTB |= (seg_a); PORTD |= (seg_b | seg_c );
#define eight 	PORTB |= (seg_a);  PORTD |= (seg_b | seg_c | seg_d | seg_e | seg_f | seg_g);
#define nine	PORTB |= (seg_a); PORTD |= (seg_b | seg_c | seg_f | seg_g);
#define zero	PORTB |= (seg_a); PORTD |= (seg_b | seg_c | seg_d | seg_e | seg_f );
#define decimalP	PORTB |= (seg_a);	PORTD |= (seg_b | seg_e | seg_f | seg_g );


#define T0_delay_33ms 5,0
#define T0_delay_20ms 5,100
#define T0_delay_16ms 5,131
#define T0_delay_12ms 5,162
#define T0_delay_10ms 5,178
#define T0_delay_8ms 5,194
#define T0_delay_6ms 4,70
#define T0_delay_5ms 4,100
#define T0_delay_4ms 4,130
#define T0_delay_3ms 4,162
#define T0_delay_2500us 4,178
#define T0_delay_2ms 3,0
#define T0_delay_1500us 3,69
#define T0_delay_1ms 3,128
#define T0_delay_667us 3,173
#define T0_delay_500us 3,192
#define T0_delay_250us 2,6
#define T0_delay_125us 3,240


#define T1_delay_3S 5, 0xA492
#define T1_delay_500ms 5, 0xF0C3
#define T1_delay_250ms 5, 0xF862
#define T1_delay_125ms 5, 0xFC31
#define T1_delay_100ms 5,0xFCF2
#define T1_delay_10ms 3, 0xF63C

#define refresh_clock_display;   for (int n = 0; n < 8; n++){display_buf[n] = clock_buf[n];}
#define refresh_stop_watch_display;  for (int n = 0; n < 8; n++){display_buf[n] = stop_watch_buf[n];}
#define initiate_stop_watch_display;  for (int n = 0; n < 8; n++){stop_watch_buf[n] = '0';}

#define increment_seconds;	if (clock_buf[0] < '9') clock_buf[0]++; else {clock_buf[0] = '0';\
if (clock_buf[1] < '5') clock_buf[1]++;else {clock_buf[1] = '0';\
if (clock_buf[3] < '9') clock_buf[3]++; else {clock_buf[3] = '0';\
if (clock_buf[4] < '5') clock_buf[4]++;else {clock_buf[4] = '0';}}}}

#define decrement_seconds; if (clock_buf[0] > '0') clock_buf[0]--; else {clock_buf[0] = '9';\
if (clock_buf[1] > '0') clock_buf[1]--; else {clock_buf[1] = '5';\
if (clock_buf[3] > '0') clock_buf[3]--; else {clock_buf[3] = '9';\
if (clock_buf[4] > '0') clock_buf[4]--;else {clock_buf[4] = '5';}}}}


#define increment_minutes; if (clock_buf[3] < '9') clock_buf[3]++; else {clock_buf[3] = '0';\
if (clock_buf[4] < '5') clock_buf[4]++;else {clock_buf[4] = '0';\
switch (clock_buf[7]){\
case '0': case '1': if (clock_buf[6] < '9') clock_buf[6]++; else {clock_buf[6] = '0';clock_buf[7]++;}break;\
case '2': if (clock_buf[6] < '3') clock_buf[6]++; else {clock_buf[6] = '0';clock_buf[7] = '0';}}}}


#define decrement_minutes; if (clock_buf[3] > '0') clock_buf[3]--; else {clock_buf[3] = '9';\
if (clock_buf[4] > '0') clock_buf[4]--;else {clock_buf[4] = '5';\
switch (clock_buf[7]){\
case '1': case '2': if (clock_buf[6] > '0') clock_buf[6]--; else {clock_buf[6] = '9';clock_buf[7]--;}break;\
case '0': if (clock_buf[6] > '0') clock_buf[6]--; else {clock_buf[6] = '3';clock_buf[7] = '2';}}}}


#define Initialise_I2C_master_write;\
	while(1){\
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));\
	TWDR = 0x02;\
	TWCR = (1 << TWINT) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));\
	if (TWSR == 0x18)break;	}	
	
#define Initialise_I2C_master_read;\
	while(1){\
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));\
	TWDR = 0x03;\
	TWCR = (1 << TWINT) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));\
	if (TWSR == 0x40)break;	}


#define long_to_slave_I2C;\
	while(1){\
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));\
	TWDR = 0x02;\
	TWCR = (1 << TWINT) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));\
	if (TWSR == 0x18)break;	}\
	for(int m = 0; m<=3; m++)\
	{TWDR = result[m];\
	TWCR = (1 << TWINT) | (1 << TWEN);\
	while (!(TWCR & (1 << TWINT)));}\
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);	


