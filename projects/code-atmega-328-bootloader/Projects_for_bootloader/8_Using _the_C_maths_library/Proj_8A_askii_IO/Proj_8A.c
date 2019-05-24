



#include "../../Resources_V12/Basic_HW_setup.h"
#include "../../Resources_V12/scanf_and_printf_subroutines.c"



/*******Define data streams*****************/
FILE  uart_output 			= FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);

FILE uart_input_any_char 	= FDEV_SETUP_STREAM(NULL, uart_getchar, 	_FDEV_SETUP_READ);
FILE uart_input_Hex 		= FDEV_SETUP_STREAM(NULL, uart_getHex, 		_FDEV_SETUP_READ);
FILE uart_input_Integer		= FDEV_SETUP_STREAM(NULL, uart_getDecimal, 	_FDEV_SETUP_READ);
FILE uart_input_Long 		= FDEV_SETUP_STREAM(NULL, uart_getDecimal, 	_FDEV_SETUP_READ);
FILE uart_input_Double 		= FDEV_SETUP_STREAM(NULL, uart_getDouble, 	_FDEV_SETUP_READ);





int main(void)
{	char input;
	int integer_number;
	long long_number;
	double Floating_point_num;

	setup_HW;
	stdout = &uart_output;
	stdin  = &uart_input_any_char;

do{putchar('.');}while((isCharavailable(100) == 0));
getchar();

printf("\r\nTo enter data, type c, s, h, d, l, e or x\r\n");				//char, string, hex, decimal, long floating (note: Long float is not supported)



while(1){

input = getchar();														//uses subroutine uart_getchar
putchar('\n');	
	
	switch (input){
	case 'c':
	{printf("Enter chars or return to escape.\r\nEcho: "); }
	while(1){
	input = getchar();if((input== '\r')||(input== '\n'))break;
	putchar(input);
	}break;		
	
	case 's':
	printf("Enter string. Return or space to escape.\r\nEcho: ");
	scanf("%s", &input);
	printf("%s\n", &input);
	break;
	
	case 'h':
	stdin  = &uart_input_Hex;
	printf("Enter hex number (zero to escape)");
	while(1){scanf("%x", &integer_number);
	if(!(integer_number))break;
	printf("\r\n%x  ", integer_number);
	printf("%o  ", integer_number);
	printf("%d  ", integer_number);
	printf("%u  ", integer_number);}
	break;
	
	case 'd':
	stdin  = &uart_input_Integer;
	printf("Enter decimal number <0 or >0 (zero to escape)");
	while(1){scanf("\n%d", &integer_number);
	if(!(integer_number))break;
	printf("\r\n%d  ", integer_number);
	printf("%x  ", integer_number);}
	break;
	
	
	case 'l':
	stdin  = &uart_input_Long;
	printf("Enter long number <0 or >0 (zero to escape)");
	while(1){scanf("\n%ld", &long_number);
	if(!(long_number))break;
	printf("\r\n%ld  ", long_number);
	printf("%lx  ", long_number);}
	break;
	
		
	case 'e':		
	stdin  = &uart_input_Double;
	printf("Enter scientific number <0 or >0 (zero to escape)");
	while(1){scanf("%lf", &Floating_point_num);
	if(!(Floating_point_num))break;
	printf("\r\n%g  ", Floating_point_num);}
	break;
	
	case ' ' :
	case '\r' :
	case '\n' : 
	continue;
	
	case 'x': SW_reset; break;
	
	
	default: break;}printf("\r\nAgain?\r\n");stdin  = &uart_input_any_char;}
	
	
	return 0;}
	

