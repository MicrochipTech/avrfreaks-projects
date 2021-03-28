/*
 Basic_UART.c
 Uses 20MHz clock
 ATtiny has signature bytes 0x1E, 0x94, 0x24
 Default value of CLK_PER is 3.333MHz
 All CLKCTRL registers are left if their default states
 
 TxD pin is PB2 pin 9
 RxD pin is PB3 pin 8
 TCA 
*/

#define device_type 1606
#define text_bytes  2048

//#define device_type 806
//#define text_bytes  1024


void Delay(void);
#include "Basic_UART.h"

int main(void)
{char keypress, User_response;
	setup_HW; 
	
	User_prompt;
	string_to_PC("\r\nPress s to continue\r\n");
	
	while(1){
	while(!(USART0.STATUS & USART_RXCIF_bm ));
	keypress = char_from_PC();
	if(keypress == 's')break;}
	Timer_TCA(TCA_500ms);
	string_to_PC("Press 1,2,3,4 or 5 or AOK to exit\r\n");
		
	while(1){
	keypress = waitforkeypress();
	switch(keypress){
		case '1':string_to_PC("100mS pause\t");Timer_TCA(TCA_100ms);break;
		case '2':string_to_PC("500mS pause\t");Timer_TCA(TCA_500ms);break;
		case '3':string_to_PC("1S pause\t\t");Timer_TCA(TCA_1s);break;
		case '4':string_to_PC("5S pause\t\t");Timer_TCA(TCA_5s);break;
		case '5':string_to_PC("10S pause\t");Timer_TCA(TCA_10s);break;
		case '.':string_to_PC("?\r\n");SW_reset; break;
		default: SW_reset;}
		
		string_to_PC("ABCDEabcdefghijklFGHIJKLMNOPQRSTUABCDEabcdefghijklFGHIJKLMNO\r\n");}
		
}
	

