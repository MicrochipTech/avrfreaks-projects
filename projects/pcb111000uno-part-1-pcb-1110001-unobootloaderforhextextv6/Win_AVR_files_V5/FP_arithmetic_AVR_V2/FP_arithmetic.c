
/*Proj_8C_fp_arithmetic
*****************************************************/


/*A bit of practice with the maths library and WinAVR I/O functions

Key in a real number and terminate with the enter key;
Press +, -,*, / or ^ followed by a second real number to do some arithmetic

Press S or C to obtain the sin of cosine
Press s or c to obtain the arc sin or arc cos
Press L or l to obtain the log or antilog values


Set the terminal program to 57.6kbaud, 8 data bits, no parity, 1 stop bit and no handshaking.
*/


/*

Disconnect the programmer,power up and press the reset button.

At the h/t/r/D prompt press "t" and download the commentary file followd by the hex file

At each  the next two user prompts press zero

The following print out should be generated

		Hex_file_size:	0x13C9   d'loaded:  0x13C9  in:  0x13C9  out
		UNO Fuses E, H, L and lock:	FD	D0	FF	EF
		
At the h/t/r/D prompt press "r" to run the program

*/

#include "FP_arithmetic_header_file.h"

#include <math.h>



int main (void){
char op;
double result = 0, x1, x2, result_rouded;


stdout = &uart_output;
stdin = &uart_input_Double;
setup_HW;

printf("\nEnter number then +-*/^ or S/s,C/c,L/l,or R to clear the result\n");
scanf("%lf", &x1);
putchar('?');


while(1){
stdin = &uart_input;
op = getchar();

switch(op){
case '+': case '-': case '*': 
case '/': case '^':printf("%c ", op);
stdin = &uart_input_Double;scanf("%lf", &x2);break;
default: break;}

switch(op){
case '+':result = x1+x2; break;
case '-':result = x1-x2; break;
case '*':result = x1*x2; break;
case '/':result = x1/x2; break;
case '^':result = pow(x1,x2); break;
case 'S':result = sin(x1/57.2958);break;
case 's': result = asin(x1)*57.2958; break;
case 'C':result = cos(x1/57.2958);break;
case 'c': result = acos(x1)*57.2958; break;
case 'L': result = log10(x1); break;
case 'l': result = pow(10,x1); break;

case 'R':stdin = &uart_input_Double;printf("\n?");scanf("%lf", &x1);putchar('?');break; }

if (op == 'R') continue;
result_rouded = round(result*1e4)/1e4;

switch(op){
case '+': case '-': case '*': 
case '/': case '^': printf(" = %g  ?",result_rouded);break;
case 'S': printf("\nSin %g = %g",x1, result_rouded);break;
case 's': printf("\nArcsin %g = %g",x1, result_rouded);break;
case 'C': printf("\nCos %g = %g",x1, result_rouded);break;
case 'c': printf("\nArcos %g = %g",x1, result_rouded);break;
case 'L': printf("\nlog10 %g = %g",x1, result_rouded);break;
case 'l': printf("\nAnti-log10 %g = %g",x1, result_rouded);break;
}
x1 = result;}


return 0;}




void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}



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

if ((keypress != '\r') && (keypress != '\n'))putchar(keypress);

return keypress;}}




/*********************************************************************/
char receiveChar(void)
{return UDR0;}



/*********************************************************************/
char isCharavailable (char m){int n = 0;
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>8000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}


/*************************************************************************************/
char decimal_digit (char data){
if (((data > '9') || (data < '0')) )return 0;
else return 1;}

