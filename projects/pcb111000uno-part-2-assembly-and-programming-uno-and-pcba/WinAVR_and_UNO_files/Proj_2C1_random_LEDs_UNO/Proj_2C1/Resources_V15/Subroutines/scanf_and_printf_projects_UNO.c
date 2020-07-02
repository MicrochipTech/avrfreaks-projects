
// See https://www.tutorialspoint.com/c_standard_library/c_function_scanf.htm
//For details of scanf.
//https://appelsiini.net/2011/simple-usart-with-avr-libc/

#include "../Header_files/I_O_from_switches_UNO.h"

int uart_putchar(char c, FILE *mystr_output);
int uart_getchar(FILE *mystr_input);				//accepts any char
int uart_getHex(FILE *mystr_input);					//Ignores non-Hex chars
int uart_getDecimal(FILE *mystr_input);				//Ignores non-Decimal chars
int uart_getDouble(FILE *mystr_input);				//Ignores non-double chars
void NumericString_to_display_R(char *);		
	

/*************************************************************************************/
int uart_putchar(char c, FILE *mystr_output)
	{if (c == '\n')
	uart_putchar('\r', mystr_output);
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;}
	
	
/*************************************************************************************/
int uart_getchar(FILE *mystr_input)
{while (!(UCSR0A & (1 << RXC0)));
return UDR0;}


/*************************************************************************************/
int uart_getHex(FILE *mystr_input)
{char keypress;
{while (!(UCSR0A & (1 << RXC0)));
keypress = UDR0;
while(!(hex_digit (keypress)) && (keypress != '\r') && (keypress != '\n'))
{while((isCharavailable(100) == 0));keypress = UDR0;}
return keypress;}}


/*************************************************************************************/
int uart_getDecimal(FILE *mystr_input)
{char keypress;
{while (!(UCSR0A & (1 << RXC0)));
keypress = UDR0;
while(!(decimal_digit (keypress)) && (keypress != '\r') && (keypress != '\n')&& (keypress != '-'))
{while((isCharavailable(100) == 0));keypress = UDR0;}
return keypress;}}


/*************************************************************************************/
int uart_getDouble(FILE *mystr_input)
{char keypress;
{while (!(UCSR0A & (1 << RXC0)));
keypress = UDR0;

if((keypress == '\r') || (keypress == '\n'))
{if(isCharavailable(1))receiveChar();}

while(!(decimal_digit (keypress)) && (keypress != '\r')
&& (keypress != '\n')&& (keypress != '-')&& (keypress != '.')
&& (keypress != 'e') && (keypress != 'E'))
{while((isCharavailable(100) == 0));keypress = UDR0;}
return keypress;}}



/*************************************************************************************/
void NumericString_to_display_R(char array[]){
char display[8];
int m_max;

m_max = 0; while(array[m_max]){if(array[m_max]=='e'){array[m_max] = 'X';} 
m_max +=1;}m_max -= 1;

if(m_max <= 7){for(int m = 0; m <= m_max; m++)display[m_max - m] = array[m];
I2C_Tx_8_byte_array(display);}	

else{

while (1){
for(int m = 0; m <= 7; m++){display[7-m] = array[m];
I2C_Tx_8_byte_array(display);}

Timer_T0_10mS_delay_x_m(50);

for(int n = 1; n <= m_max+1; n++){while(switch_2_down);
for(int m = 0; m <= 7; m++){display[7-m] = array[m + n];
I2C_Tx_8_byte_array(display);}
Timer_T0_10mS_delay_x_m(25);
if(switch_1_down)break;	}if(switch_1_down)break;}}}
