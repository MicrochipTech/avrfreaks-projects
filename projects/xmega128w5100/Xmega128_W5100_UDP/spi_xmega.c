#include <avr/io.h> 
#include "spi_xmega.h"

void SPID_Init()
{
    /* Init SS pin as output with wired AND and pull-up. */
    PORTD.DIRSET = SPID_SS_bm;
    PORTD.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

    /* Set SS output to high. (No slave addressed). */
    PORTD.OUTSET = SPID_SS_bm;

    // SPI setup. options choosen from spi.f for atmega128
    // MSB first => DORD must be null => not set
    SPID.CTRL = SPI_PRESCALER_DIV4_gc|  SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc; //| SPI_CLK2X_bm ;

//    SPIREG.INTCTRL = SPI_INTLVL_LO_gc; // interrupt level

    	/* MOSI and SCK as output. */
    PORTD.DIRSET = SPID_MOSI_bm | SPID_SCK_bm; 

}


void SPIC_Init()
{
    /* Init SS pin as output with wired AND and pull-up. */
    PORTC.DIRSET = PIN4_bm;
    PORTC.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

    /* Set SS output to high. (No slave addressed). */
    PORTC.OUTSET = PIN4_bm;

    // SPI setup. options choosen from spi.f for xmega128a1
    // MSB first => DORD must be null => not set
    SPIC.CTRL = SPI_PRESCALER_DIV16_gc|  SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_3_gc;// | SPI_CLK2X_bm ;

//    SPIREG.INTCTRL = SPI_INTLVL_LO_gc; // interrupt level

    	/* MOSI and SCK as output. */
    PORTC.DIRSET = SPIC_MOSI_bm | SPIC_SCK_bm; 

}




void IINCHIP_SpiSendData(uint8_t data)
{
    SPID.DATA = data ; //send data
    while(!(SPID.STATUS & SPI_IF_bm)) {}; //wait for transmition complete
}

uint8_t IINCHIP_SpiRecvData()
{
    return SPID.DATA;
}

