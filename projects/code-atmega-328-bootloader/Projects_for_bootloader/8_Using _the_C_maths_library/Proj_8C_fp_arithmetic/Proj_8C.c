
/*Proj_8C_fp_arithmetic
*****************************************************/


/*A bit more practice with the maths library.*/





#include "../../Resources_V12/Basic_HW_setup.h"
#include "../../Resources_V12/scanf_and_printf_subroutines.c"
#include <math.h>


FILE  mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_input_Double = FDEV_SETUP_STREAM(NULL, uart_getDouble, _FDEV_SETUP_READ);



int main (void){
char op;
double result, x1, x2;


setup_HW;
stdout = &mystdout;


stdin = &uart_input_Double;
printf("Enter fpn (cr) -op- fpn (cr)\r");
scanf("%lf", &x1);

while(1){

stdin = &mystdin;
op = getchar();
if(op == 'x'){ SW_reset;}

stdin = &uart_input_Double;
scanf("%lf", &x2);
printf("%g %c %g", x1, op, x2);

switch(op){
case '+':result = x1+x2; break;
case '-':result = x1-x2; break;
case '*':result = x1*x2; break;
case '/':result = x1/x2; break;
case '^':result = pow(x1,x2); break;
default: result = 0; break;}
printf(" = %g  ?",result);
x1 = result; printf("\r");}
return 0;}



