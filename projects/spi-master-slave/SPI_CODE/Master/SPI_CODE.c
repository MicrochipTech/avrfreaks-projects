/*----------------------------------------------------------------
-----------------CONNECTIONS WITH MEGA8---------------------------
------------------------------------------------------------------
 
 AVR ATMEGA8            74HC595
 SS   (16)     ->      LatchClock (12)
 MOSI (17)     ->      SerialIn   (14)
 SCK  (19)     ->      ShiftClock (11)
 GND  (8)	   ->      GND        
 					   Reset      (10) -> VCC
                       OE         (13) -> GND
       //This is for testing   Q1         (1)  -> 1K Resistor -> LED -> GND
   
---------------------------------------------------------------------*/

/*----------------------------------------------------------------
-----------------HEADER FILES-------------------------------------
-----------------------------------------------------------------*/
#define F_CPU 1000000UL
#include <inttypes.h>
#include <avr/io.h>
#include<compat/deprecated.h>		//HEADER FILE FOR FUNCTIONS LIKE SBI AND CBI
#include<util/delay.h>				//HEADER FILE FOR DELAY

/*----------------------------------------------------------------
-----------------DEFINATION OF SPI PINS---------------------------
-----------------------------------------------------------------*/

#define SPI_CHIP_SELECT PB2             // SS pin is used here, but any pin can be used
#define SPI_MOSI        PB3             // SPI MOSI pin 
#define SPI_SCK         PB5             // SPI SCK pin
#define LED_PWR			PB0
#define SPI_DDR         DDRB            // port used by SPI

/*----------------------------------------------------------------
-----------------MAIN PROGRAM-------------------------------------
-----------------------------------------------------------------*/

int main(void)
{ 	
    uint16_t Data = 0x02ED;   // here address = 0x02; and Data = 0xED;
							  // Here 255 address with 255 data can be transmitted
	uint16_t  address,cdata;

	uint8_t i;
    
    SPI_DDR  = _BV(SPI_CHIP_SELECT) + _BV(SPI_MOSI) + _BV(SPI_SCK) + _BV(LED_PWR);    /* SPI port initialization (/SS, MOSI, SCK output, MISO input) */
	sbi(PORTB,0);					//LED1 ON (INDICATION FOR READY TO USE)	
    SPCR = _BV(SPE) +_BV(MSTR) + _BV(CPOL) + _BV(CPHA);    			/* SPI interrupt disabled, SPI port enabled, master mode, MSB first, SPI mode 3, SPI Clock = XTAL/4 */
     
    for(i=0; i<=0; i++)
    {
        address = Data;
		address = ((address & 0xFF00)>>8); 
		PORTB &= ~_BV(SPI_CHIP_SELECT);     	// enable SPI device
        SPDR = address;                         	// send data to SPI device (turn LED on/off)
        while (!(SPSR & _BV(SPIF)));        	// wait until write complets
        PORTB |= _BV(SPI_CHIP_SELECT);      	// disable SPI device

                               	// toggle LED
     	_delay_ms(1000);                       	// delay 0.1 seconds
		
		cdata = Data;
		cdata = (cdata & 0x00FF);
		PORTB &= ~_BV(SPI_CHIP_SELECT);     	// enable SPI device
        SPDR = cdata;                         	// send data to SPI device (turn LED on/off)
        while (!(SPSR & _BV(SPIF)));        	// wait until write complets
        PORTB |= _BV(SPI_CHIP_SELECT);      	// disable SPI device

                               	// toggle LED
		_delay_ms(1000); 
   }
}
