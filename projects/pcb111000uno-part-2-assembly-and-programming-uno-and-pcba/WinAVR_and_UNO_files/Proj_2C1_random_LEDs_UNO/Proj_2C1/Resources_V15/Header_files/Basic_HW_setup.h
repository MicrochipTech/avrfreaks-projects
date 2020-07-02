
/*
ATMEGA 168 EEPROM reservations
0x1FF		User cal value
0x1FE		User cal value
0x1FD		Default cal value
0x1FC		Used by PRN_8bit_GEN()
0x1FC/B		Used by PRN_16bit_GEN()
*/



int Mux_cntl_2;
char MCUSR_copy;
char watch_dog_reset = 0;
char User_response;

#define T0_delay_5ms 5,220
#define T0_delay_4ms 4,227
#define T0_delay_10ms 5,178

#define T1_delay_250ms 5,0xF85F

#define T1_delay_10ms 3, 0xF63C
#define T1_delay_20ms 3, 0xEC79
#define T1_delay_30ms 3, 0xE2B6
#define T1_delay_50ms 3, 0xCF31
#define T1_delay_100ms 3, 0x9E62
#define T1_delay_500ms 5,0xF0BE

#define T1_delay_1sec 5,0xE17D
#define T1_delay_2sec 5,0xC2FB
#define T1_delay_3sec 5, 0xA479
#define T1_delay_4sec 5,0x85F7

#define T2_delay_2ms 4,0
#define T2_delay_8ms 7,192
#define T2_delay_32ms 7,0


/*****************************************************************************/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>


/*****************************************************************************/
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}
#define clear_digits {for(int m = 0; m<=7; m++)digits[m]=0;}
#define pause {String_to_PC(" AK");waitforkeypress();}
#define shift_digits_left {for (int n = 0; n < 7; n++){digits[7-n] = digits[6-n];}}
#define shift_digits_right \
{int n = 0; while(digits[n+1] && (n<=6)){digits[n] = digits[n+1]; n++;} digits[n]=0;}




/*****************************************************************************/
//HW setup for PCB 111000
#define setup_HW \
setup_watchdog;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
set_up_switched_inputs;\
Unused_I_O;\
eeprom_write_byte((uint8_t*)(0x1FD),OSCCAL);\
while (!(PIND & (1 << PD1)));\
Timer_T0_sub(T0_delay_5ms);\
OSC_CAL_168;\
USART_init(0,16);\
\
Timer_T0_10mS_delay_x_m(1);\
if(!(watch_dog_reset))\
{I2C_Tx_LED_dimmer();\
diagnostic_mode;}


/************************************/
#define diagnostic_mode \
I2C_TX_328_check();\
waiting_for_I2C_master;\
if (receive_byte_with_Nack()==1)\
{TWCR = (1 << TWINT);\
String_to_PC("\r\nPress\r\n\
1 for OS version\r\n\
2 for system data\r\n\
3 Message from the OS (x to escape)\r\n\
4 Default project\r\n\
0 to escape\r\n");\
switch (waitforkeypress()){\
case '0':break;\
case '1':I2C_Rx_get_version('0');break;\
case '2':I2C_Rx_get_version('1');break;\
case '3':do\
{Read_Hello_world_string();newline();}\
while (waitforkeypress() != 'x');\
break;\
case '4':I2C_Tx_snowstorm_display(); break;}}\
else TWCR = (1 << TWINT);


/************************************/
#define setup_HW_basic \
setup_watchdog;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
set_up_switched_inputs;\
Unused_I_O;\
eeprom_write_byte((uint8_t*)(0x1FD),OSCCAL);\
while (!(PIND & (1 << PD1)));\
Timer_T0_sub(T0_delay_5ms);\
OSC_CAL_168;\
USART_init(0,16);\
Timer_T0_10mS_delay_x_m(1);\
if(!(watch_dog_reset))\
I2C_Tx_LED_dimmer();




/*****************************************************************************/
#define setup_UNO \
CLKPR = (1 << CLKPCE);\
CLKPR = (1 << CLKPS0);\
setup_watchdog;\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
set_up_switched_inputs;\
Unused_I_O;\
Timer_T0_10mS_delay_x_m(5);\
USART_init(0,16);\
if (((PINB & 0x04)^0x04) && \
((PIND & 0x04)^0x04))\
I2C_Tx_LED_dimmer_UNO();\
\
if(((PIND & 0x04)^0x04) && \
((PIND & 0x80)^0x80))\
Cal_UNO_pcb_A();


/************************************/
#define setup_UNO_extra \
CLKPR = (1 << CLKPCE);\
CLKPR = (1 << CLKPS0);\
\
MCUSR_copy = eeprom_read_byte((uint8_t*)0x3FC);\
if (MCUSR_copy & (1 << PORF)){MCUSR_copy = (1 << PORF);\
eeprom_write_byte((uint8_t*)0x3F4,0);}\
setup_watchdog_UNO_extra;\
\
set_up_I2C;\
ADMUX |= (1 << REFS0);\
set_up_switched_inputs;\
Unused_I_O;\
Timer_T0_10mS_delay_x_m(5);\
USART_init(0,16);\
\
User_app_commentary_mode;\
\
if (((PINB & 0x04)^0x04) && \
((PIND & 0x04)^0x04))\
I2C_Tx_LED_dimmer_UNO();\
\
if(((PIND & 0x04)^0x04) && \
((PIND & 0x80)^0x80))\
Cal_UNO_pcb_A();



/************************************/
//Test programmmer writes 0x40 to 0x3F4 before quitting 
//Text verification.c increments 0x3F4 each time a string is printed in commentary mode
//bit 7 of 0x3F4 tells the bootloader that the WDTout is not due to a user app

#define User_app_commentary_mode \
\
if(eeprom_read_byte((uint8_t*)0x3F4) == 0xFF)\
eeprom_write_byte((uint8_t*)0x3F4,0);\
\
if(eeprom_read_byte((uint8_t*)0x3F4) == 0x40){\
for(int m = 0; m < 10; m++)String_to_PC("\r\n");\
String_to_PC("Project commentary: Press 'X' to escape or AOK\r\n");\
\
eeprom_write_byte((uint8_t*)0x3F4,0x41);}\
\
if ((eeprom_read_byte((uint8_t*)0x3F4) & 0x40)){\
eeprom_write_byte((uint8_t*)0x3F4,\
(eeprom_read_byte((uint8_t*)0x3F4) | 0x80));\
\
for(int m = 0; m < 4; m++)String_to_PC("\r\n");\
\
asm("jmp 0x6C00");}		//Was 0x6C30 ??????	






/*****************************************************************************/
#define setup_watchdog \
if (MCUSR & (1 << WDRF))watch_dog_reset = 1;\
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;



#define setup_watchdog_UNO_extra \
if (MCUSR_copy & (1 << WDRF))watch_dog_reset = 1;\
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;


#define wdr()  __asm__ __volatile__("wdr")

#define wd_timer_off \
wdr();\
MCUSR &= (~(1 << WDRF));\
WDTCSR |= (1<<WDCE) | (1<<WDE);\
WDTCSR = 0x00;


/*****************************************************************************/
#define One_25ms_WDT_with_interrupt \
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = (1<< WDE) | (1 << WDIE) |  (1 << WDP0)  |  (1 << WDP1);


#define Two_50ms_WDT_with_interrupt \
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = (1<< WDE) | (1 << WDIE) |  (1 << WDP2);


#define One_sec_WDT_with_interrupt \
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = (1<< WDE) | (1 << WDIE) |  (1 << WDP1) | (1 << WDP2);



#define SW_reset_with_interrupt \
wdr();\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = (1<< WDE) | (1 << WDIE) |  (1 << WDP0);\
sei(); while(1);


#define Arduino_non_WDTout \
Char_from_EEPROM(0x3F1)


#define set_Arduino_WDTout \
Char_to_EEPROM( 0x3F1, 0);

#define clear_Arduino_WDT_flag \
Char_to_EEPROM( 0x3F1, 0xFF);


/*****************************************************************************/
#define set_up_I2C \
TWAR = 0x02;


/*****************************************************************************/
#define set_up_switched_inputs \
MCUCR &= (~(1 << PUD));\
DDRD &= (~((1 << PD2)|(1 << PD7)));\
PORTD |= ((1 << PD2) | (1 << PD7));\
DDRB &= (~(1 << PB6));\
PORTB |= (1 << PB6);


/*****************************************************************************/
#define Unused_I_O \
MCUCR &= (~(1 << PUD));\
DDRB &= (~((1 << PB2)|(1 << PB7)));\
DDRC &= (~((1 << PC0)|(1 << PC1)|(1 << PC2)));\
DDRD &= (~((1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6)));\
PORTB |= ((1 << PB2)|(1 << PB7));\
PORTC |= ((1 << PC0)|(1 << PC1)|(1 << PC2));\
PORTD |= ((1 << PD3)|(1 << PD4)|(1 << PD5)|(1 << PD6));


/*****************************************************************************/
#define OSC_CAL_168 \
if ((eeprom_read_byte((uint8_t*)0x1FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x1FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x1FE)\
== eeprom_read_byte((uint8_t*)0x1FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x1FE);}



/*****************************************************************************/
#define OSC_CAL_328 \
if ((eeprom_read_byte((uint8_t*)0x3FE) > 0x0F)\
&&  (eeprom_read_byte((uint8_t*)0x3FE) < 0xF0) && (eeprom_read_byte((uint8_t*)0x3FE)\
== eeprom_read_byte((uint8_t*)0x3FF))) {OSCCAL = eeprom_read_byte((uint8_t*)0x3FE);}


//Note: both WinAVR and Arduino read the EEPROM as unsigned 8 bit chars


/*****************************************************************************/
#define User_prompt \
while(1){\
do{String_to_PC("R?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'R') || (User_response == 'r'))break;} String_to_PC("\r\n");



/*****************************************************************************/
#define waiting_for_I2C_master \
TWCR = (1 << TWEA) | (1 << TWEN);\
while (!(TWCR & (1 << TWINT)));\
TWDR;

#define clear_I2C_interrupt \
TWCR = (1 << TWINT);


