/*
1-Wire code based on Atmel appnote
  AVR318: Dallas 1-Wire master
*/

#include "OWI.h"

// 1-Wire bit functions

/*! \brief Initialization of the one wire bus(es). (Software only driver)
 *  
 *  This function initializes the 1-Wire bus(es) by releasing it and
 *  waiting until any presence sinals are finished.
 *
 *  \param  pins    A bitmask of the buses to initialize.
 */
void OWI_Init(uint8_t pins)
{
    OWI_RELEASE_BUS(pins);
    // The first rising edge can be interpreted by a slave as the end of a
    // Reset pulse. Delay for the required reset recovery time (H) to be 
    // sure that the real reset is interpreted correctly.
    _delay_us(OWI_DELAY_H_STD_MODE);
}


/*! \brief  Write a '1' bit to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '1' bit on the 1-Wire
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void OWI_WriteBit1(uint8_t pins)
{
    uint8_t intState;
    
    // Disable interrupts.
    intState = SREG;
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_B_STD_MODE);
    
    // Restore interrupts.
	SREG = intState;
    sei();
}


/*! \brief  Write a '0' to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '0' bit on the 1-Wire(R)
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void OWI_WriteBit0(uint8_t pins)
{
    uint8_t intState;
    
    // Disable interrupts.
    intState = SREG;
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_C_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_D_STD_MODE);
    
    // Restore interrupts.
   SREG = intState;
    sei();
}


/*! \brief  Read a bit from the bus(es). (Software only driver)
 *
 *  Generates the waveform for reception of a bit on the 1-Wire(R) bus(es).
 *
 *  \param  pins    A bitmask of the bus(es) to read from.
 *
 *  \return A bitmask of the buses where a '1' was read.
 */
uint8_t OWI_ReadBit(uint8_t pins)
{
    uint8_t intState;
    uint8_t bitsRead;
    
    // Disable interrupts.
    intState = SREG;
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_E_STD_MODE);
    
    // Sample bus and delay.
    bitsRead = OWI_PIN & pins;
    _delay_us(OWI_DELAY_F_STD_MODE);
    
    // Restore interrupts.
   SREG = intState;
    sei();
    
    return bitsRead;
}


/*! \brief  Send a Reset signal and listen for Presence signal. (software
 *  only driver)
 *
 *  Generates the waveform for transmission of a Reset pulse on the 
 *  1-Wire(R) bus and listens for presence signals.
 *
 *  \param  pins    A bitmask of the buses to send the Reset signal on.
 *
 *  \return A bitmask of the buses where a presence signal was detected.
 */
uint8_t OWI_DetectPresence(uint8_t pins)
{
    uint8_t intState;
    uint8_t presenceDetected;
    
    // Disable interrupts.
    intState = SREG;
    cli();
    
    // Drive bus low and delay.
    OWI_PULL_BUS_LOW(pins);
    _delay_us(OWI_DELAY_H_STD_MODE);
    
    // Release bus and delay.
    OWI_RELEASE_BUS(pins);
    _delay_us(OWI_DELAY_I_STD_MODE);
    
    // Sample bus to detect presence signal and delay.
    presenceDetected = ((~OWI_PIN) & pins);
    _delay_us(OWI_DELAY_J_STD_MODE);
    
    // Restore interrupts.
	SREG = intState;
    sei();
    
    return presenceDetected;
}





// 1-Wire high level functions

/*! \brief  Sends one byte of data on the 1-Wire(R) bus(es).
 *  
 *  This function automates the task of sending a complete byte
 *  of data on the 1-Wire bus(es).
 *
 *  \param  data    The data to send on the bus(es).
 *  
 *  \param  pins    A bitmask of the buses to send the data to.
 */
void OWI_SendByte(uint8_t data, uint8_t pins)
{
    uint8_t temp;
    uint8_t i;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    {
        // Determine if lsb is '0' or '1' and transmit corresponding
        // waveform on the bus.
        temp = data & 0x01;
        if (temp)
        {
            OWI_WriteBit1(pins);
        }
        else
        {
            OWI_WriteBit0(pins);
        }
        // Right shift the data to get next bit.
        data >>= 1;
    }
}


/*! \brief  Receives one byte of data from the 1-Wire(R) bus.
 *
 *  This function automates the task of receiving a complete byte 
 *  of data from the 1-Wire bus.
 *
 *  \param  pin     A bitmask of the bus to read from.
 *  
 *  \return     The byte read from the bus.
 */
uint8_t OWI_ReceiveByte(uint8_t pin)
{
    uint8_t data;
    uint8_t i;

    // Clear the temporary input variable.
    data = 0x00;
    
    // Do once for each bit
    for (i = 0; i < 8; i++)
    {
        // Shift temporary input variable right.
        data >>= 1;
        // Set the msb if a '1' value is read from the bus.
        // Leave as it is ('0') else.
        if (OWI_ReadBit(pin))
        {
            // Set msb
            data |= 0x80;
        }
    }
    return data;
}


/*! \brief  Sends the SKIP ROM command to the 1-Wire bus(es).
 *
 *  \param  pins    A bitmask of the buses to send the SKIP ROM command to.
 */
void OWI_SkipRom(uint8_t pins)
{
    // Send the SKIP ROM command on the bus.
    OWI_SendByte(OWI_ROM_SKIP, pins);
}


/*! \brief  Sends the READ ROM command and reads back the ROM id.
 *
 *  \param  romValue    A pointer where the id will be placed.
 *
 *  \param  pin     A bitmask of the bus to read from.
 */
void OWI_ReadRom(uint8_t * romValue, uint8_t pin)
{
    uint8_t bytesLeft = 8;

    // Send the READ ROM command on the bus.
    OWI_SendByte(OWI_ROM_READ, pin);
    
    // Do 8 times.
    while (bytesLeft > 0)
    {
        // Place the received data in memory.
        *romValue++ = OWI_ReceiveByte(pin);
        bytesLeft--;
    }
}



// CRC functions

/*! \brief  Compute the CRC8 value of a data set.
 *
 *  This function will compute the CRC8 or DOW-CRC of inData using seed
 *  as inital value for the CRC.
 *
 *  \param  inData  One byte of data to compute CRC from.
 *
 *  \param  seed    The starting value of the CRC.
 *
 *  \return The CRC8 of inData with seed as initial value.
 *
 *  \note   Setting seed to 0 computes the crc8 of the inData.
 *
 *  \note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC8 value of a
 *          longer string of data.
 */
uint8_t OWI_ComputeCRC8(uint8_t inData, uint8_t seed)
{
    uint8_t bitsLeft;
    uint8_t temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
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


/*! \brief  Compute the CRC16 value of a data set.
 *
 *  This function will compute the CRC16 of inData using seed
 *  as inital value for the CRC.
 *
 *  \param  inData  One byte of data to compute CRC from.
 *
 *  \param  seed    The starting value of the CRC.
 *
 *  \return The CRC16 of inData with seed as initial value.
 *
 *  \note   Setting seed to 0 computes the crc16 of the inData.
 *
 *  \note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC16 value of a
 *          longer string of data.
 */
uint16_t OWI_ComputeCRC16(uint8_t inData, uint16_t seed)
{
    uint8_t bitsLeft;
    uint8_t temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
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


/*! \brief  Calculate and check the CRC of a 64 bit ROM identifier.
 *  
 *  This function computes the CRC8 value of the first 56 bits of a
 *  64 bit identifier. It then checks the calculated value against the
 *  CRC value stored in ROM.
 *
 *  \param  romvalue    A pointer to an array holding a 64 bit identifier.
 *
 *  \retval OWI_CRC_OK      The CRC's matched.
 *  \retval OWI_CRC_ERROR   There was a discrepancy between the calculated and the stored CRC.
 */
uint8_t OWI_CheckRomCRC(uint8_t * romValue)
{
    uint8_t i;
    uint8_t crc8 = 0;
    
    for (i = 0; i < 7; i++)
    {
        crc8 = OWI_ComputeCRC8(*romValue, crc8);
        romValue++;
    }
    if (crc8 == (*romValue))
    {
        return OWI_CRC_OK;
    }
    return OWI_CRC_ERROR;
}
