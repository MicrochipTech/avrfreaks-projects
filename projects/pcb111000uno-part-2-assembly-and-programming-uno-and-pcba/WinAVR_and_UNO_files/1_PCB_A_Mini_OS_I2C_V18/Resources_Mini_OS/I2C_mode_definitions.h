

# define I2C_Tx_BWops; \
for (int m = 0; m < 8; m++){\
display_buf[m] = I2C_data[m];}



/********************************************************************************************/
#define I2C_Tx_2_integers; \
for (int m = 0; m < 4; m++){\
	display_buf[m] = I2C_data[m];}\
	for (int m = 4; m < 8; m++)display_buf[m] = 0;
	
	
/********************************************************************************************/
#define I2C_Rx_get_version; \
switch (I2C_data[0]){\
case '0': string_to_slave_I2C(SW_Version); break;\
case '1': string_to_slave_I2C(SW_info); break;\
default: break;}



/********************************************************************************************/
# define I2C_Tx_LED_dimmer; \
switch(I2C_data[0]){\
case 1: eeprom_write_byte((uint8_t*)(0x3FB),0xFD); break;\
case 2: eeprom_write_byte((uint8_t*)(0x3FB),0xFE); break;\
case 3: eeprom_write_byte((uint8_t*)(0x3FB),0xFF); break;}\
T0_interupt_cnt = 0;




/********************************************************************************************/
#define I2C_Tx_any_segment; \
letter = I2C_data[0];\
digit_num = I2C_data[1];\
switch (letter){\
case 'A': case 'a': letter = 0; break;\
case 'B': case 'b': letter = 3; break;\
case 'C': case 'c': letter = 4; break;\
case 'D': case 'd': letter = 2; break;\
case 'E': case 'e': letter = 5; break;\
case 'F': case 'f': letter = 6; break;\
case 'G': case 'g': letter = 1; break;\
default: letter = 8; break;	}\
if(letter == 8)break;\
if((digit_num >= 8) || (digit_num < 0))break;\
\
if (mode == 2){clear_digits;clear_display;clear_display_buffer;}\
if (mode == 3){if(display_buf[letter] & (1 << digit_num))display_buf[letter] &= (~(1 << digit_num));\
else display_buf[letter] |= (1 << digit_num);} 



/********************************************************************************************/
#define I2C_initiate_10mS_ref; \
TCCR2B = 2;\
TIMSK2 |= ((1 << OCIE2A) | (1 << TOV2));\
wdt_enable(WDTO_120MS);



/********************************************************************************************/
#define I2C_Tx_long; \
L_number = 0;\
for (int m = 0; m < 4; m++){\
L_number += I2C_data[m];\
if(m<3) L_number = L_number << 8;}\
Display_num(L_number);



/********************************************************************************************/
#define I2C_Tx_float_num; \
L_number = 0;\
for (int m = 0; m < 4; m++){\
L_number += I2C_data[m];\
if(m<3) L_number = L_number << 8;}\
expnt = I2C_data[4];\
float_to_askii(L_number, expnt,Sc_Num_string);\
TCCR2B = 2;\
TIMSK2 |= (1 << TOIE2);




/********************************************************************************************/
#define I2C_displayToLong; \
L_number=0;\
sign = '+';\
{int m = 7; while (!(display_buf[m]))m--;\
if (display_buf[m] == '-') {sign = '-'; m--;}\
while(m>=0){L_number = L_number*10 + display_buf[m] - '0'; m--;}}\
if (sign == '-')L_number = L_number * (-1);\
for(int m = 0; m <= 3; m++){result[3-m] = L_number; L_number = L_number >> 8;}\
long_to_slave_I2C;


/********************************************************************************************/

#define I2C_Tx_real_num; \
L_number = 0;\
for (int m = 0; m < 4; m++){\
L_number += I2C_data[m];\
if(m<3) L_number = L_number << 8;}\
Display_real_num(L_number);



/********************************************************************************************/
#define diagnostic_mode; \
Initialise_I2C_master_write;\
I2C_master_transmit(eeprom_read_byte((uint8_t*)(0x3FC)));\
TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);\
eeprom_write_byte((uint8_t*)0x3FC, 0x0);



/********************************************************************************************/
#define Read_AT328_EEPROM; \
Initialise_I2C_master_write;\
{int m = 0x200;\
while (m <= 0x3F6)\
{I2C_master_transmit(eeprom_read_byte((uint8_t*)m));\
m++;}\
I2C_master_transmit(eeprom_read_byte((uint8_t*)0x3FE));\
I2C_master_transmit(eeprom_read_byte((uint8_t*)0x3FF));\
TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}



/********************************************************************************************/
# define I2C_Tx_8_byte_array; \
for (int m = 0; m < 8; m++){\
display_buf[m] = I2C_data[m];}



/********************************************************************************************/
#define I2C_Tx_display_char;\
output_2 =  I2C_data[0];\
sign_bit = I2C_data[1];\
for (int m = 0; m < 8; m++)display_buf[m] = 0;\
if (sign_bit == 's') {Disp_CharS (output_2);}\
if (sign_bit == 'u') {Disp_CharU (output_2);}



/********************************************************************************************/
#define PCB_test; \
TIMSK0 &= (~(1 << TOIE0));\
clear_digits; clear_display;\
Initialise_I2C_master_read;\
test_num = I2C_master_receive(1);\
switch(test_num){\
case '0': zero; break;\
case '1': one; break;\
case '2': two; break;\
case '3': three; break;\
case '4': four; break;\
case '5': five; break;\
case '6': six; break;\
case '7': seven; break;\
case '8': eight; break;\
case '9': nine; break;}\
test_digit = I2C_master_receive(0);\
switch(test_digit){\
case '0': digit_0; break;\
case '1': digit_1; break;\
case '2': digit_2; break;\
case '3': digit_3; break;\
case '4': digit_4; break;\
case '5': digit_5; break;\
case '6': digit_6; break;\
case '7': digit_7; break;}\
TWCR =  (1 << TWINT ) | (1 << TWEN ) | (1 << TWSTO );



/********************************************************************************************/
#define Multiplexer_demo; \
\
eeprom_write_byte((uint8_t*)(0x3FB), 0xFE);\
MUX_cntl = I2C_data[0];\
\
T0_interupt_cnt = 0;\
switch (MUX_cntl){\
case 0:	timer_T0_sub_with_interrupt(T0_delay_500us);break;\
case 1: timer_T0_sub_with_interrupt(T0_delay_1ms);break;\
case 2:	timer_T0_sub_with_interrupt(T0_delay_1500us);break;\
case 3: timer_T0_sub_with_interrupt(T0_delay_2ms);break;\
case 4:	timer_T0_sub_with_interrupt(T0_delay_2500us);break;\
case 5: timer_T0_sub_with_interrupt(T0_delay_3ms);break;\
case 6: timer_T0_sub_with_interrupt(T0_delay_5ms);break;\
case 7: timer_T0_sub_with_interrupt(T0_delay_8ms);break;\
default: timer_T0_sub_with_interrupt(T0_delay_500us);break;}\
\
for(int p = 0; p <= 7; p++)display_buf[p] = p + '0';




/********************************************************************************************/