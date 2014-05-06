/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	9.4.2007
*
*Slave serial configuration of Xilinx Spartan-III FPGA's
*Tested with Spartan 3 XC3S400
*
****************************************************************************/

#include <avr/io.h>
#include "dataflash.h"
#include "usart.h"
#include "fpgacon.h"

unsigned long df_byte_nr;


void configure_fpga(unsigned int DF_START_ADR, unsigned long FILE_SIZE){

	//initialize MCU pins
	initialize();
	
	restart_fpga();
	
	//make sure INIT and PROG are high before continuing
	while( !(PINC & (1<<INIT_PIN)) || !(PINC & (1<<PROG_PIN)) );
	
	//usart_transmits(" INIT and PROG are high. Starting downloading of bitstream..."); //debug
	
	upload_data(DF_START_ADR, FILE_SIZE);
	
	read_done();

}

void initialize(void){
	//set input pins with High-Z
	DDRC &= ~( (1<<DONE_PIN) | (1<<PROG_PIN) | (1<<INIT_PIN) );
	PORTC &= ~( (1<<DONE_PIN) | (1<<PROG_PIN) | (1<<INIT_PIN) );
	
	// set output pins with resistor-pullup enabled
	PORTC |= (1<<CCLK_PIN) | (1<<DIN_PIN);
	DDRC |= (1<<CCLK_PIN) | (1<<DIN_PIN);
	
	usart_init(0); //for debuging
}

// restart FPGA by asserting PROG_B low
void restart_fpga(void){
	
	//assert PROG low, initiating a restart
	DDRC |= (1<<PROG_PIN);
	PORTC &= ~(1<<PROG_PIN);
	wait_ms(1);
	
	//assert PROG back as High-Z input
	DDRC &= ~(1<<PROG_PIN);	
	PORTC &= ~(1<<PROG_PIN);
	
	//make sure PROG is high before continuing
	while ( !(PINC & (1<<PROG_PIN)) );
}

void upload_data(unsigned int DF_START_ADR, unsigned long FILE_SIZE){
	
	unsigned char serial_mask;
	unsigned char df_buf;
	unsigned int df_mem_page = DF_START_ADR;
	unsigned int df_byte = 264;
	
	//initialize dataflash
	usart_transmits("Starting configuration of FPGA... ");
	DF_SPI_init();
	
	for (unsigned long i= 0; i < FILE_SIZE; i++){
		
		if (df_byte == 264){
			Page_To_Buffer (df_mem_page, 1);
			df_mem_page++;
			df_byte = 0;
		}
		//unsigned char Buffer_Read_Byte (unsigned char BufferNo, unsigned int IntPageAdr);
		df_buf = Buffer_Read_Byte(1, df_byte);
		df_byte++;
		
		//serialize and shift bits out starting from LSB to MSB
		for(signed char j=7; j >= 0; j--){
			serial_mask = ( ((df_buf>>j) & 1) << DIN_PIN); //000000?0
			PORTC = (PORTC & (~(1<<DIN_PIN)) ) | serial_mask; // xxxxxx0x OR 000000?0
			
			PORTC &= ~(1<<CCLK_PIN); //set clk low
			PORTC |= (1<<CCLK_PIN); //set clk high
		}
	}
}
	

void read_done(void){
	
	//cehck if DONE is pin is high
	if ( (PINC & (1<<DONE_PIN)))
		usart_transmits("...Configuration of FPGA was successful! ");
	else{
		//is INIT high?
		if ( (PINC & (1<<INIT_PIN)))
			usart_transmits("...Configuration of FPGA was NOT successful! INIT pin is HIGH. ");
		else
			usart_transmits("...Configuration of FPGA was NOT successful! INIT pin is LOW. ");
	}
}
	
	
// Delay loop
void wait_ms(unsigned int ms)
{
    unsigned int counter;
   
    while (ms) {
        counter = F_CPU / 5000;
       
        while (counter) {
            asm volatile("nop");
            counter--;
        }
        ms--;
    }
}
