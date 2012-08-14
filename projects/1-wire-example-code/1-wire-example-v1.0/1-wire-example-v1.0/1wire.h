#ifndef __one_wire_header_
#define __one_wire_header_

/***************************************************************************\
|* Configuration: If this symbol is defined, the internal pull-up resister 
|*                on the GPIO pins of the AVR will be used to generate the 
|*                necessary pull-up on the bus. If an external pull-up 
|*                resistor is used, uncomment this define.
****************************************************************************/
//#define     	ONEWIRE_USE_INTERNAL_PULLUP

/***************************************************************************\
|* Configuration: Define the port and pin registers for the 'bus'
****************************************************************************/
#define     	ONEWIRE_PORT        		PORTD   // 1-Wire PORT Data
#define     	ONEWIRE_PIN         	 	PIND    // 1-Wire Input pin
#define     	ONEWIRE_DDR         		DDRD    // 1-Wire Data direction

/***************************************************************************\
|* Configuration: Bitmasks for the logical bus channels to physical pins
****************************************************************************/
#define 		ONEWIRE_PIN_0				0x01
#define 		ONEWIRE_PIN_1				0x02
#define 		ONEWIRE_PIN_2				0x04
#define 		ONEWIRE_PIN_3				0x08
#define 		ONEWIRE_PIN_4				0x10
#define 		ONEWIRE_PIN_5				0x20
#define 		ONEWIRE_PIN_6				0x40
#define 		ONEWIRE_PIN_7				0x80

/***************************************************************************\
|* ROM commands
****************************************************************************/
#define 		ONEWIRE_READ				(0x33)	// READ ROM 
#define 		ONEWIRE_SKIP				(0xcc)	// SKIP ROM 
#define 		ONEWIRE_MATCH				(0x55)	// MATCH ROM 
#define 		ONEWIRE_SEARCH				(0xf0)	// SEARCH ROM 

/****************************************************************************
|* Return codes
****************************************************************************/
#define 		ONEWIRE_OK					(0x00)
#define 		ONEWIRE_ERROR				(0x01)
#define 		ONEWIRE_SEARCH_COMPLETE 	(0x00)	// Search completed ok
#define 		ONEWIRE_SEARCH_FAILED		(0xff)	// Search failed

/****************************************************************************
|* Macro : Pull the 1-wire bus low. Avoids function-call overhead
****************************************************************************/
#define ONEWIRE_PULL_BUS_LOW(bitMask) \
            ONEWIRE_DDR 	|=  bitMask; \
            ONEWIRE_PORT 	&= ~bitMask;

/****************************************************************************
|* Macro : Release the bus. Avoids function-call overhead
****************************************************************************/
#ifdef OWI_ONEWIRE_USE_INTERNAL_PULLUP
	// Set 1-Wire pin(s) to input and enable internal pull-up resistor.
	#define ONEWIRE_RELEASE_BUS(bitMask) \
            ONEWIRE_DDR 	&= ~bitMask; \
            ONEWIRE_PORT 	|=  bitMask; 

#else
	// Set 1-Wire pin(s) to input mode. No internal pull-up enabled.
	#define ONEWIRE_RELEASE_BUS(bitMask) \
            ONEWIRE_DDR 	&= ~bitMask; \
            ONEWIRE_PORT 	&= ~bitMask;
#endif

/***************************************************************************\
|* Useful structure definition
****************************************************************************/
typedef struct
	{
	uint8_t 		bus;		// Bitmask of the bus that the device is on
	uint8_t 		id[8];		// The 64-bit identifier for the device
	} oneWireDevice;
	

/****************************************************************************
|* API - CRC functions
****************************************************************************/
unsigned char oneWireCrc8(unsigned char inData, unsigned char seed);
unsigned int oneWireCrc16(unsigned char inData, unsigned int seed);
unsigned char oneWireRomCrc(unsigned char * romValue);

/***************************************************************************\
|* API - Bit functions
****************************************************************************/
void oneWireInit(unsigned char pins);
void oneWireWriteBit1(unsigned char pins);
void oneWireWriteBit0(unsigned char pins);
unsigned char oneWireReadBit(unsigned char pins);
unsigned char oneWireDetectPresence(unsigned char pins);

/***************************************************************************\
|* API - Higher-level functions
****************************************************************************/
void oneWireSendByte(unsigned char data, unsigned char pins);
void oneWireSkipRom(unsigned char pins);
void oneWireReadRom(unsigned char * romValue, unsigned char pins);
void oneWireMatchRom(unsigned char * romValue, unsigned char pins);
unsigned char oneWireReceiveByte(unsigned char pin);
unsigned char oneWireSearchRom(unsigned char * bitPattern, 
							   unsigned char lastDeviation, 
							   unsigned char pins);
unsigned char oneWireSearchBuses(oneWireDevice * devices, 
								 unsigned char len, 
								 unsigned char buses);
oneWireDevice * oneWireFindFamily(unsigned char familyID, 
								  oneWireDevice * devices, 
								  unsigned char size);

/***************************************************************************\
|* Timing parameters. Note: using these depends on _builtin_ave_delay_cycles
|* being defined by the compiler, in order to get the correct resolution
****************************************************************************/
#define ONEWIRE_DELAY_480				(480)
#define ONEWIRE_DELAY_6 				(6)
#define ONEWIRE_DELAY_9 				(9)
#define ONEWIRE_DELAY_64 				(64)
#define ONEWIRE_DELAY_60 				(60)
#define ONEWIRE_DELAY_70 				(70)
#define ONEWIRE_DELAY_10 				(10)
#define ONEWIRE_DELAY_55 				(55)
#define ONEWIRE_DELAY_410				(410)

#endif // ! __one_wire_header_
