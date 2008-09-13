/**************************************
        LCD TEST DEF FILE
**************************************/

//Global control
#define HIGH            1
#define LOW             0   
#define PORT_INPUT      0x00
#define PORT_OUPUT      0xFF

//LATCH control
#define ALE_ON          PORTE.1 = 1
#define ALE_OFF         PORTE.1 = 0

//LCD power
#define LCD_PWR_ON      PORTD.3 = 1
#define LCD_PWR_OFF     PORTD.3 = 0

//SPI
#define SPE             6
#define SPR1            1
#define SPR0            0

#define SPI_ON          SPCR |= (1<<SPE)
#define SPI_OFF         SPCR &= ~(1<<SPE)
#define SPI_CLK_125K    SPCR |= (1<<SPR1) | (1<<SPR0)
#define SPI_CLK_4M      SPCR &= ~(1<<SPR1) | ~(1<<SPR0)

//Interupts
#define INT0            6                 

//Touchscreen Flag register
#define TouchPressed    0
#define TouchCalibratie 1
#define TouchCalProcess 2 
#define XY_Value        3           

//Internal EEMPROM
#define EERE            0
#define EEWE            1
#define EEMWE           2     

#define RXC             7

void TouchscreenTest(unsigned int X, unsigned int Y);
