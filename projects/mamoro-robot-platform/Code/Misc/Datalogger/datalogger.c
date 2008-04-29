/***************************************************
*Dataflash functionality test
*
*Jidan Al-Eryani
*µC reads character from the terminal and writes it in the dataflash. Then opens the dataflash and reads the character and puts it in the termianl.
*****************************************************/

#include <avr/io.h>
#include "dataflash.h"
#include "usart.h"
#include <avr/interrupt.h>
#include <string.h>


void Initialization(void);

int main(void)
{    

    // initalization
    DF_SPI_init();
	
	char txt[] = "Hello World!";
	char rtxt[13];
	char c_usart;
	char c_dataflash;
	
	USART_init();
	while(1){
	//read from terminal
	c_usart = USART_Receive();
		
	//write to Dataflash
	Buffer_Write_Byte (1,0,c_usart);
	Buffer_To_Page(1,0);
    DF_CS_inactive;
	
	//read from flash
    DF_CS_active;	
	Page_To_Buffer(0,1);
	c_dataflash = Buffer_Read_Byte(1,0);
	DF_CS_inactive;	
	
	//write to terminal
	USART_Transmit(c_dataflash);
	}


	return 0; // mt
}
