/*
1-Wire code based on Atmel appnote
  AVR318: Dallas 1-Wire master
*/

// Supported devices family id's
#define DS18S20_FAMILY_ID    0x10
#define DS18B20_FAMILY_ID    0x28 



/****************************************************************************
 ROM commands
****************************************************************************/
#define     OWI_ROM_READ    0x33    //!< READ ROM command code.
#define     OWI_ROM_SKIP    0xcc    //!< SKIP ROM command code.
#define     OWI_ROM_MATCH   0x55    //!< MATCH ROM command code.
#define     OWI_ROM_SEARCH  0xf0    //!< SEARCH ROM command code.

/****************************************************************************
 FUNCTION commands
****************************************************************************/
#define OWI_START_CONVERSION         0x44
#define OWI_READ_SCRATCHPAD          0xbe


/*! \brief  Use internal pull-up resistor on 1-Wire buses.
 *
 *  If this symbol is defined, the internal pull-up resister on the GPIO pins 
 *  of the AVR will be used to generate the necessary pull-up on the bus. If 
 *  an external pull-up resistor is used, uncomment this define.
 */
//#define     OWI_USE_INTERNAL_PULLUP


// Port configuration registers for 1-Wire buses.
// Make sure that all three registers belong to the same port.
#define     OWI_PORT        PORTC   //!< 1-Wire PORT Data register.
#define     OWI_PIN         PINC    //!< 1-Wire Input pin register.
#define     OWI_DDR         DDRC    //!< 1-Wire Data direction register.


/*****************************************************************************
 Other defines
*****************************************************************************/
// Pin bitmasks.
#define     OWI_PIN_0       0x01
#define     OWI_PIN_1       0x02
#define     OWI_PIN_2       0x04
#define     OWI_PIN_3       0x08
#define     OWI_PIN_4       0x10
#define     OWI_PIN_5       0x20
#define     OWI_PIN_6       0x40
#define     OWI_PIN_7       0x80


/*****************************************************************************
 Timing parameters
*****************************************************************************/

// Bit timing delays in clock cycles us
#define     OWI_DELAY_A_STD_MODE    6 
#define     OWI_DELAY_B_STD_MODE    64  
#define     OWI_DELAY_C_STD_MODE    60  
#define     OWI_DELAY_D_STD_MODE    10  
#define     OWI_DELAY_E_STD_MODE    9   
#define     OWI_DELAY_F_STD_MODE    55  
//#define     OWI_DELAY_G_STD_MODE  0   
#define     OWI_DELAY_H_STD_MODE    480 
#define     OWI_DELAY_I_STD_MODE    70  
#define     OWI_DELAY_J_STD_MODE    410 

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
// 1-Wire bit functions

void OWI_Init(uint8_t pins);
void OWI_WriteBit1(uint8_t pins);
void OWI_WriteBit0(uint8_t pins);
uint8_t OWI_ReadBit(uint8_t pins);
uint8_t OWI_DetectPresence(uint8_t pins);

/****************************************************************************
 Macros
****************************************************************************/
/*! \brief Pull 1-Wire bus low.
 *
 *  This macro sets the direction of the 1-Wire pin(s) to output and 
 *  pull the line(s) low.
 *  
 *  \param bitMask  A bitmask of the buses to pull low.
 */
#define OWI_PULL_BUS_LOW(bitMask) \
            OWI_DDR |= bitMask; \
            OWI_PORT &= ~bitMask;
            

/*! \def    OWI_RELEASE_BUS(bitMask)
 *
 *  \brief  Release the bus. 
 *
 *  This macro releases the bus and enables the internal pull-up if
 *  it is used.
 *
 *  \param  bitMask A bitmask of the buses to release.
 */
#ifdef OWI_USE_INTERNAL_PULLUP            
// Set 1-Wire pin(s) to input and enable internal pull-up resistor.
#define OWI_RELEASE_BUS(bitMask) \
            OWI_DDR &= ~bitMask; \
            OWI_PORT |= bitMask; 

#else
// Set 1-Wire pin(s) to input mode. No internal pull-up enabled.
#define OWI_RELEASE_BUS(bitMask) \
            OWI_DDR &= ~bitMask; \
            OWI_PORT &= ~bitMask;

#endif





// 1-Wire high level functions

void OWI_SendByte(uint8_t data, uint8_t pins);
uint8_t OWI_ReceiveByte(uint8_t pin);
void OWI_SkipRom(uint8_t pins);
void OWI_ReadRom(uint8_t * romValue, uint8_t pins);

// CRC functions

#define     OWI_CRC_OK      0x00    //!< CRC check succeded
#define     OWI_CRC_ERROR   0x01    //!< CRC check failed


uint8_t OWI_ComputeCRC8(uint8_t inData, uint8_t seed);
uint16_t OWI_ComputeCRC16(uint8_t inData, uint16_t seed);
uint8_t OWI_CheckRomCRC(uint8_t * romValue);


