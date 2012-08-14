/******************************************************************************\
|* One-wire library code
|*
|* Ported from the Atmel application note AVR318 and accompanying source code
|* to the GCC compiler (the appnote uses IAR)
|*
\******************************************************************************/
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "1wire.h"


/******************************************************************************\
|* Compute an 8-bit CRC 
|*
|* Pass a seed of 0 to start the CRC of a byte-stream. Pass the result as 
|* the seed for subsequent bytes
\******************************************************************************/
unsigned char oneWireCrc8(unsigned char inData, unsigned char seed)
	{
    unsigned char bitsLeft;
    unsigned char temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    	{
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
            seed >>= 1;
        else
        	{
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        	}
        inData >>= 1;
    	}
    return seed;    
	}
	
/******************************************************************************\
|* Compute a 16-bit CRC 
|*
|* Pass a seed of 0 to start the CRC of a byte-stream. Pass the result as 
|* the seed for subsequent bytes
\******************************************************************************/
unsigned int oneWireCrc16(unsigned char inData, unsigned int seed)
	{
    unsigned char bitsLeft;
    unsigned char temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    	{
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
            seed >>= 1;
        else
        	{
            seed ^= 0x4002;
            seed >>= 1;
            seed |= 0x8000;
        	}
        inData >>= 1;
    	}
    return seed;    
	}
	
/******************************************************************************\
|* Compute and check the CRC for a ROM identifier 
\******************************************************************************/
unsigned char oneWireRomCrc(unsigned char * romValue)
	{
    unsigned char i;
    unsigned char crc8 = 0;
    
    for (i = 0; i < 7; i++)
    	{
        crc8 = oneWireCrc8(*romValue, crc8);
        romValue++;
    	}
    if (crc8 == (*romValue))
    	{
        return ONEWIRE_OK;
    	}
    return ONEWIRE_ERROR;
	}
	
/******************************************************************************\
|* Sends one byte of data on the 1-Wire(R) bus(es).
\******************************************************************************/
void oneWireSendByte(unsigned char data, unsigned char pins)
	{
    unsigned char temp;
    unsigned char i;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    	{
        // Determine if lsb is '0' or '1' and transmit corresponding
        // waveform on the bus.
        temp = data & 0x01;
        if (temp)
            oneWireWriteBit1(pins);
        else
            oneWireWriteBit0(pins);
        
		// Right shift the data to get next bit.
        data >>= 1;
    	}
	}
	
/******************************************************************************\
|* Receives one byte of data from the 1-Wire(R) bus.
\******************************************************************************/
unsigned char oneWireReceiveByte(unsigned char pin)
	{
    unsigned char data;
    unsigned char i;

    // Clear the temporary input variable.
    data = 0x00;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    	{
        // Shift temporary input variable right.
        data >>= 1;
        
		// Set the msb if a '1' value is read from the bus.
        if (oneWireReadBit(pin))
            data |= 0x80;
        
    	}
    return data;
	}
	
/******************************************************************************\
|* Sends the SKIP ROM command to the 1-Wire bus(es).
\******************************************************************************/
void OWI_SkipRom(unsigned char pins)
	{
    // Send the SKIP ROM command on the bus.
    oneWireSendByte(ONEWIRE_SKIP, pins);
	}

/******************************************************************************\
|* Sends the READ ROM command to the 1-Wire bus(es).
\******************************************************************************/
void oneWireReadRom(unsigned char * romValue, unsigned char pin)
	{
    unsigned char bytesLeft = 8;

    // Send the READ ROM command on the bus.
    oneWireSendByte(ONEWIRE_READ, pin);
    
    // Do 8 times.
    while (bytesLeft > 0)
    	{
        // Place the received data in memory.
        *romValue++ = oneWireReceiveByte(pin);
        bytesLeft--;
    	}
	}

/******************************************************************************\
|* Sends the MATCH ROM command to the 1-Wire bus(es).
\******************************************************************************/
void oneWireMatchRom(unsigned char * romValue, unsigned char pins)
	{
    unsigned char bytesLeft = 8;   
    
    // Send the MATCH ROM command.
    oneWireSendByte(ONEWIRE_MATCH, pins);

    // Do once for each byte.
    while (bytesLeft > 0)
    	{
        // Transmit 1 byte of the ID to match.
        oneWireSendByte(*romValue++, pins);
        bytesLeft--;
    	}
	}

/******************************************************************************\
|* Search for slave devices on the 1-wire bus. 
|*
|* bitPattern ought to be an 8-byte buffer, initialised to 0 on first call, 
|* then left to hold the returned value on subsequent calls. lastDeviation 
|* ought to be 0 on first call, and the return value of the function on 
|* subsequent calls. Pin is the bus to use
\******************************************************************************/
unsigned char oneWireSearchRom(unsigned char * bitPattern, 
							   unsigned char lastDeviation, 
							   unsigned char pin)
	{
    unsigned char currentBit = 1;
    unsigned char newDeviation = 0;
    unsigned char bitMask = 0x01;
    unsigned char bitA;
    unsigned char bitB;
	
	//unsigned char bitsA[65];
	//unsigned char bitsB[65];
	//bitsA[64] = bitsB[64] = '\0';

    // Send SEARCH ROM command on the bus.
    oneWireSendByte(ONEWIRE_SEARCH, pin);
    
    // Walk through all 64 bits.
    while (currentBit <= 64)
    	{
        // Read bit from bus twice.
        bitA = oneWireReadBit(pin);
        bitB = oneWireReadBit(pin);
		
		//bitsA[currentBit-1] = (bitA ) ? '1':'0';
		//bitsB[currentBit-1] = (bitB ) ? '1':'0';

        if (bitA && bitB)
        	{
            // Both bits 1 (Error).
            newDeviation = ONEWIRE_SEARCH_FAILED;
            return ONEWIRE_SEARCH_FAILED;
        	}
        else if (bitA ^ bitB)
        	{
            // Bits A and B are different. All devices have the same bit here.
            // Set the bit in bitPattern to this value.
            if (bitA)
            	{
                (*bitPattern) |= bitMask;
            	}
            else
            	{
                (*bitPattern) &= ~bitMask;
            	}
        	}
        else // Both bits 0
        	{
            // If this is where a choice was made the last time,
            // a '1' bit is selected this time.
            if (currentBit == lastDeviation)
                (*bitPattern) |= bitMask;

            // For the rest of the id, '0' bits are selected when
            // discrepancies occur.
            else if (currentBit > lastDeviation)
            	{
                (*bitPattern) &= ~bitMask;
                newDeviation = currentBit;
            	}
            // If current bit in bit pattern = 0, then this is
            // out new deviation.
            else if ( !(*bitPattern & bitMask)) 
                newDeviation = currentBit;
            // IF the bit is already 1, do nothing.
            else
            	{}
        	}
                
        
        // Send the selected bit to the bus.
        if ((*bitPattern) & bitMask)
            oneWireWriteBit1(pin);
        else
            oneWireWriteBit0(pin);

        // Increment current bit.    
        currentBit++;

        // Adjust bitMask and bitPattern pointer.    
        bitMask <<= 1;
        if (!bitMask)
        	{
            bitMask = 0x01;
            bitPattern++;
        	}	
    	}
	//printf("A:%s\nB:%s\n\n", bitsA, bitsB);
	//_delay_ms(10000);
    return newDeviation;
	}


/******************************************************************************\
|* Initialise everything
\******************************************************************************/
void oneWireInit(unsigned char pins)
	{
    ONEWIRE_RELEASE_BUS(pins);
	
    // The first rising edge can be interpreted by a slave as the end of a
    // Reset pulse. Delay for the required reset recovery time (480 uS) to be 
    // sure that the real reset is interpreted correctly.
    _delay_us(ONEWIRE_DELAY_480);
	}

/******************************************************************************\
|* Write a '1' bit to the bus(es)
\******************************************************************************/
void oneWireWriteBit1(unsigned char pins)
	{    
    // Disable interrupts.
    uint8_t sreg = SREG;
    cli();
    
    // Drive bus low and delay.
    ONEWIRE_PULL_BUS_LOW(pins);
    _delay_us(ONEWIRE_DELAY_6);
    
    // Release bus and delay.
    ONEWIRE_RELEASE_BUS(pins);
    _delay_us(ONEWIRE_DELAY_64);
    
    // Restore interrupts.
    SREG=sreg;
	}
	
/******************************************************************************\
|* Write a '0' bit to the bus(es)
\******************************************************************************/
void oneWireWriteBit0(unsigned char pins)
	{
    // Disable interrupts.
    uint8_t sreg = SREG;
    cli();
  
    // Drive bus low and delay.
    ONEWIRE_PULL_BUS_LOW(pins);
    _delay_us(ONEWIRE_DELAY_60);
    
    // Release bus and delay.
    ONEWIRE_RELEASE_BUS(pins);
    _delay_us(ONEWIRE_DELAY_10);
    
    // Restore interrupts.
    SREG=sreg;
	}

	
/******************************************************************************\
|* Read a bit from the bus(es).
\******************************************************************************/
unsigned char oneWireReadBit(unsigned char pins)
	{
    unsigned char bitsRead;
    
    // Disable interrupts.
    uint8_t sreg = SREG;
    cli();
    
    // Drive bus low and delay.
    ONEWIRE_PULL_BUS_LOW(pins);
    _delay_us(ONEWIRE_DELAY_6);
    
    // Release bus and delay.
    ONEWIRE_RELEASE_BUS(pins);
    _delay_us(ONEWIRE_DELAY_9);
    
    // Sample bus and delay.
    bitsRead = ONEWIRE_PIN & pins;
    _delay_us(ONEWIRE_DELAY_55);
    
    // Restore interrupts.
    SREG=sreg;
    
    return bitsRead;
	}

/******************************************************************************\
|* Send a Reset signal and listen for Presence signal
\******************************************************************************/
unsigned char oneWireDetectPresence(unsigned char pins)
	{
    unsigned char presenceDetected;
    
     // Disable interrupts.
    uint8_t sreg = SREG;
    cli();

     // Drive bus low and delay.
    ONEWIRE_PULL_BUS_LOW(pins);
    _delay_us(ONEWIRE_DELAY_480);
      
    // Release bus and delay.
    ONEWIRE_RELEASE_BUS(pins);
    _delay_us(ONEWIRE_DELAY_70);
    
    // Sample bus to detect presence signal and delay.
    presenceDetected = ((~ONEWIRE_PIN) & pins);
    _delay_us(ONEWIRE_DELAY_410);
    
    // Restore interrupts.
    SREG=sreg;
    
    return presenceDetected;
	}


/******************************************************************************\
|* Perform a 1-wire search on the identified buses. 
|* Returns ONEWIRE_SEARCH_COMPLETE on success, ONEWIRE_SEARCH_FAILED on error
\******************************************************************************/
unsigned char oneWireSearchBuses(oneWireDevice * devices, 
								 unsigned char len, 
								 unsigned char buses)
	{
	/**************************************************************************\
	|* Initialize all addresses as zero, on bus 0 (does not exist).
	\**************************************************************************/
    for (uint8_t i = 0; i < len; i++)
    	{
        devices[i].bus = 0x00;
        for (uint8_t j = 0; j < 8; j++)
            devices[i].id[j] = 0x00;
    	}
    
	/**************************************************************************\
	|* Find the buses with slave devices
	\**************************************************************************/
    uint8_t presence 	= oneWireDetectPresence(buses);
    uint8_t numDevices  = 0;
    uint8_t *newID  	= devices[0].id;
   
	/**************************************************************************\
	|* Go through all buses with slave devices
	\**************************************************************************/
    for (uint8_t currentBus = 0x01; currentBus; currentBus <<= 1)
    	{
        uint8_t lastDeviation 	= 0;
        uint8_t *currentID  	= newID;

		/**********************************************************************\
		|* Are any devices available on this bus
		\**********************************************************************/
        if (currentBus & presence)
        	{
			/******************************************************************\
			|* Do slave search on each bus, and place identifiers and 
			|* corresponding bus "addresses" in the array.
			\******************************************************************/
            do  
            	{
                memcpy(newID, currentID, 8);
                oneWireDetectPresence(currentBus);
				
                lastDeviation 	= oneWireSearchRom(newID, 
												   lastDeviation, 
												   currentBus);
                currentID 		= newID;
                devices[numDevices].bus = currentBus;
                numDevices++;
                newID=devices[numDevices].id;                
            	}  
			while(lastDeviation != ONEWIRE_SEARCH_COMPLETE);            
        	}
    	}

		/**************************************************************************\
	|* Go through all the devices and do CRC check
	\**************************************************************************/
    for (uint8_t i = 0; i < numDevices; i++)
    	{
		/**********************************************************************\
		|* If any id has a crc error, return error
		\**********************************************************************/
        if(oneWireRomCrc(devices[i].id) == ONEWIRE_ERROR)
            {
			PORTA = 1;
			return ONEWIRE_SEARCH_FAILED;
        	}
    	}
    // Else, return Successful.
			PORTA = 4;
    return ONEWIRE_SEARCH_COMPLETE;
	}

/******************************************************************************\
|* Return a pointer to a one-wire descriptor that matches a given family id
\******************************************************************************/
oneWireDevice * oneWireFindFamily(unsigned char familyID, 
								  oneWireDevice * devices, 
								  unsigned char size)
	{
	uint8_t i = 0;
	/**************************************************************************\
	|* Search through the array
	\**************************************************************************/
    while (i < size)
    	{
        // Return the pointer if there is a family id match.
        if ((*devices).id[0] == familyID)
            return devices;
        devices++;
        i++;
    	}

	/**************************************************************************\
	|* Couldn't find it
	\**************************************************************************/
    return NULL;
	}
	
