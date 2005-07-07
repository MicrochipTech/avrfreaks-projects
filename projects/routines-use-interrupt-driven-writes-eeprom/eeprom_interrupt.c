#include "eeprom_interrupt.h"
#include <avr/pgmspace.h>

#define FALSE 0
#define TRUE 1

//designed for atmega128, but is either directly portable to others, or with minimal modification.

//global variables relating to EEPROM usage
volatile char eeprom_busy = FALSE;
volatile void* eeprom_dest;
volatile void* eeprom_data;
volatile char eeprom_data_size;



//example test program
/*
unsigned char data_buffer[32];		//arbitrary size is 32 bytes of data.  This ought to do for this purpose
unsigned char e_log[50] [32]__attribute__((section(".eeprom"))) ;	//no initialization,  size is 32*50 = 1600 bytes out of 4096 in the atmega128
#include <avr/eeprom.h>

void test_eeprom_write(void)
{
	unsigned char counter;
	
	//fill the data buffer with an arbitrary number
	for (counter = 0; counter<sizeof(data_buffer); counter++)	//fill the buffer with an arbitrary number
	{
		data_buffer[counter] = counter+1;
	}
	
	//write to the eeprom
	
	
	//write the 32 bytes to the first block in the eeprom array
	while(eeprom_busy||(EECR & (1<<EEWE)));		//explicitly wait for eeprom to be free & idle (hopefully rarely necessary)
	eeprom_interrupt_write_block(data_buffer, &e_log[0], sizeof(data_buffer));
	while(eeprom_busy);	//wait for the eeprom to not be busy
	
	//refill the buffer with something else
	for (counter = 0; counter<sizeof(data_buffer); counter++)	//fill the buffer with zeros
	{
		data_buffer[counter] = 0;
	}
	
	// read back the data
	eeprom_read_block(data_buffer, &e_log[0], sizeof(data_buffer));	//read back the data
	// that is it
*/





//this initializes the eeprom writing data, and enables the eeprom interrupt.
//the eeprom interrupt fires continuously while it is ready to accept data
// after that the eeprom interrupt clicks thru the data as fast as the HW allows, then shuts off

//"source" is a pointer to the data buffer in SRAM
//"dest" is a pointer to the destination in EEPROM
//"num_bytes" is sizeof() the data buffer

//returns a (-1) if the eeprom is busy with a previous usage of this function
//returns a (-2) if the eeprom is busy with a write function (e.g. something else is using the eprom,
//     or the last write command is still in process just after the "eeprom_busy" flag is cleared


//another potential usage of this command is:
//while(eeprom_interrupt_write_block (void* source, void* dest, char num_bytes));
//this will keep retrying the function until it returns a (0) which indicates success
//this avoids explicitly testing that the eeprom is ready for the next operation.






char eeprom_interrupt_write_block (void* source, void* dest, char num_bytes)
{
	if (eeprom_busy) return (-1);			//fail if eeprom is still working on a previous request
	if (EECR & (1<<EEWE)) return (-2);		//check if something else might have the eeprom busy
	
	
	//set up the stuff for the interrupt to use
	eeprom_dest = dest;					//point to the first location 
	eeprom_data = source;					//same for this
	eeprom_data_size = num_bytes ;		//total bytes
	
	eeprom_busy = TRUE;	
	EECR |= (1<<EERIE);		//enable the eeprom ready interrupt.  It will now take over all the data shifting
	
	//now the interrupt will do the rest of the job.
	return(0);
}

//-------------------------------------------------------------------------------------------------------------------------


SIGNAL(SIG_EEPROM_READY)
{
	//process the next byte and move the pointers for the next, if any
	EEAR = (unsigned int)eeprom_dest;				//set the eeprom address
	EEDR = ((unsigned char*)eeprom_data)[0];		//enter the data byte
		
	EECR |= (1<<EEMWE);			//first enable master write
	EECR |= (1<<EEWE);				//now enable the actual write.  This sequence is required by the HW.
	
	eeprom_data_size--;				//decrement the number of bytes left
	if (!eeprom_data_size)				//if no bytes left....
	{
		EECR &= ~(1<<EERIE);		//clear the eeprom ready interrupt
		eeprom_busy = FALSE;		// release control of the eeprom
	}
	else 							//with bytes left......
	{
		eeprom_data++;			//point to the NEXT location 
		eeprom_dest++;			//same for this
	}	
	
//optionally you can avoid testing that eeprom_data_size is zero now, and test on the next interrupt cycle.
//This way you will clear the busy flag only when the eeprom is totally idle.

	
}

