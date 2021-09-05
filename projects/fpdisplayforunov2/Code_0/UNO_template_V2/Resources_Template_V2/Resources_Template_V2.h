

volatile char One_wire_Rx_char, One_wire_Tx_char;
volatile int One_wire_Rx_int;
volatile char Tx_complete, Rx_complete;


long Long_Num_to_UNO;
long Long_Num_from_UNO = 0;         
unsigned char num_byte[4];


char display_buffer[12], buffptr = 0;
volatile char cr_keypress;



#define Send_int_num_string \
One_wire_Tx_char = 'A'; UART_Tx_1_wire();\
for(int m = 0; m <= 7; m++){One_wire_Tx_char = display_buffer[m]; UART_Tx_1_wire();}\
One_wire_Tx_char = cr_keypress;  UART_Tx_1_wire();


#define Send_float_num_string \
One_wire_Tx_char = 'B'; UART_Tx_1_wire();\
for(int m = 0; m <= 7; m++){One_wire_Tx_char = display_buffer[m]; UART_Tx_1_wire();}\
One_wire_Tx_char = cr_keypress;  UART_Tx_1_wire();


#define Tx_clock_1     		200			//10K Baud rate	
#define Rx_clock_1     		200		
#define Start_clock_1    	TCNT0 = 0;  TCCR0B = (1 << CS01);
#define Half_Rx_clock_1 	100		


#define wait_for_comms_tick \
OCR0A +=  Rx_clock_1;\
while (!(TIFR0 & (1 << OCF0A)));\
TIFR0 = 0xFF;


#define wait_for_half_comms_tick \
OCR0A =  Half_Rx_clock_1;\
while (!(TIFR0 & (1 << OCF0A)));\
TIFR0 = 0xFF;  


#define PINC4_down  ((PINC & 0x10)^0x10)
#define PINC3_down  ((PINC & 0x08)^0x08)


#define User_prompt_template \
while(1){\
do{sendString("f/i?    ");}	 while((isCharavailable(250) == 0));\
User_response = receiveChar();\
if((User_response == 'f') || (User_response == 'i'))break;} sendString("\r\n");







