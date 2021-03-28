

#define Start_Tx_clock					TCCR0B = ((1 << CS00));	OCR0A =  Tx_clock;
#define Start_Rx_clock					TCCR0B = ((1 << CS00));	OCR0A =  Rx_clock;
#define Start_half_Rx_clock				TCCR0B = ((1 << CS00));	OCR0A =  Half_Rx_clock;
#define Stop_clock  					TCCR0B = 0; 
#define Restart_clock					TCNT0 = 0; GTCCR |= (1 << PSRSYNC); Start_Tx_clock;
#define Reset_clock						TCCR0B = 0;	TCNT0 = 0; GTCCR |= (1 << PSRSYNC);

//Clock settings: 64 to 70  are OK  62 and 72 are NOT


#define Tx_clock            	66
#define Rx_clock            	66
#define Half_Rx_clock   		33


#define input_h                (PINC & (1 << PINC0))
#define input_l        			(!(PINC & (1 << PINC0)))

#define output_h         		DDRC &= (~(1 << DDC0));
#define output_l          		DDRC |= (1 << DDC0);


#define synch_pulse	\
Reset_clock;\
Start_Rx_clock;\
data_byte_Tx = 0x55; \
transmit_byte;


/*
Definitions used to drive the UART interface for programming flash
where speed is critical.
Provide minimum software latency but use a lot of program memory.
*/


/*****************************************/
#define transmitBit \
\
{while (!(TIFR0 & (1 << OCF0A)));\
if(Tx_bit){output_h;}\
else {output_l};\
TIFR0 = 0xFF;\
OCR0A +=  Tx_clock;}



/*****************************************/
#define transmit_byte \
{\
parity_2 = 0;\
Tx_bit = 0;\
 transmitBit;\
\
for(int n = 0; n <= 7; n++){\
(Tx_bit = data_byte_Tx & (1 << n));\
if(Tx_bit)parity_2 += 1;\
transmitBit;}\
 \
if(parity_2%2)Tx_bit = 1;\
else Tx_bit = 0;\
transmitBit;\
\
Tx_bit = 1;\
transmitBit;\
transmitBit;\
\
}




/*****************************************/
//Inverses the polarity bit to pause UPDI repeat
//command proir to executing a break command 

#define transmit_byte_P \
{\
parity_2 = 0;\
Tx_bit = 0;\
 transmitBit;\
\
for(int n = 0; n <= 7; n++){\
(Tx_bit = data_byte_Tx & (1 << n));\
if(Tx_bit)parity_2 += 1;\
transmitBit;}\
\
if(parity_2%2)Tx_bit = 0;\
else Tx_bit = 1;\
transmitBit;\
\
Tx_bit = 1;\
transmitBit;\
transmitBit;\
}




/*****************************************/
#define wait_for_start_bit \
while(input_h);\
Reset_clock;\
Start_half_Rx_clock;\
while (!(TIFR0 & (1 << OCF0A)));\
OCR0A +=  Rx_clock;\
TIFR0 = 0xFF;




/*****************************************/
#define receiveBit \
{while (!(TIFR0 & (1 << OCF0A)));\
if (input_h){Rx_bit = 1;}\
else {Rx_bit = 0;  }\
TIFR0 = 0xFF;\
OCR0A +=  Rx_clock;}




/*****************************************/
#define receive_byte \
\
data_byte_Rx = 0;\
parity_2 = 0;\
wait_for_start_bit;\
\
for(int n = 0; n <= 7; n++){\
receiveBit;\
if(Rx_bit){data_byte_Rx |= (1 << n);parity_2 += 1;}}\
\
receiveBit;\
if ((Rx_bit) && (parity_2%2));\
else P_counter++;\
receiveBit;\
receiveBit;



/*****************************************/
#define break_page_fill \
\
Reset_clock;\
Start_Rx_clock;\
data_byte_Tx = 0x55;\
transmit_byte_P;\
Tx_bit = 0;\
for(int m = 0; m <= 11; m++)transmitBit;\
Tx_bit = 1;transmitBit;transmitBit;\
Reset_clock;\
Start_Tx_clock;\
data_byte_Tx = 0x55; transmit_byte;\
data_byte_Tx = (STCS | 0x09); transmit_byte;\
data_byte_Tx = 0x01; transmit_byte;\
Stop_clock;









