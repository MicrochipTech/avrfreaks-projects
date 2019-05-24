

#include "../../Resources_V12/Basic_HW_setup.h"
#include "../../Resources_V12/scanf_and_printf_subroutines.c"
#include <math.h>





/*******Define input streams*****************/
FILE  uart_output = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input_any_char = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_input_Double_from_csv = FDEV_SETUP_STREAM(NULL, uart_getDouble_from_csv, _FDEV_SETUP_READ);





int main(void)
{	char input;
	double Floating_point_num, accumulator_A, accumulator_G;

	setup_HW;
	
	stdout = &uart_output;
	stdin  = &uart_input_any_char;

do{putchar('.');}while((isCharavailable(100) == 0));
getchar();	
	
/*************Display Table headings following POR or programming and then reset************/	


	printf("Select 9.6k baud rate, then AK to continue\r\n");
	USART_init(0,103);
	while((isCharavailable(100) == 0));
	input = getchar();
	printf("\r\nSend data file\r\nArithmetic mean\tGeometric mean\t data items");


USART_init(0,103);
	stdout = &uart_output;
	stdin  = &uart_input_Double_from_csv;
	while(1){
	num_counter = 0;
	
	accumulator_G = 0;
	accumulator_A = 0;
	while(1){scanf("%lf", &Floating_point_num);
	if(!(Floating_point_num))break;
	accumulator_A += Floating_point_num;
	accumulator_G += log10(Floating_point_num);}
	binUnwantedChars();
	printf("\r\n%g  ", (accumulator_A/(num_counter-1)));
	printf("\t\t%g  ", pow(10,(accumulator_G/(num_counter-1))));
	printf("\t\t%d  ", (num_counter-1));
	}return 0;}
	




/*********************************************************************/
