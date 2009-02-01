// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               main.c
* \li Compiler:           IAR EWAAVR 3.20a
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All AVRs.
*
* \li Application Note:   AVR318 - Dallas 1-Wire(R) master.
*                         
*
* \li Description:        Example on how to use the 1-Wire(R) interface
*                         master.
*
*                         $Revision: 1.7 $
*                         $Date: Thursday, August 19, 2004 14:27:18 UTC $
****************************************************************************/

#include <ioavr.h>
#include <inavr.h>

#include "OWIPolled.h"
#include "OWIHighLevelFunctions.h"
#include "OWIBitFunctions.h"
#include "..\common_files\OWIcrc.h"

#include <string.h> // Used for memcpy.


// Defines used only in code example.
#define DS1820_FAMILY_ID                0x10 
#define DS1820_START_CONVERSION         0x44
#define DS1820_READ_SCRATCHPAD          0xbe
#define DS1820_ERROR                    -1000   // Return code. Outside temperature range.

#define DS2890_FAMILY_ID                0x2c
#define DS2890_WRITE_CONTROL_REGISTER   0X55
#define DS2890_RELEASE_CODE             0x96
#define DS2890_WRITE_POSITION           0x0f

#define SEARCH_SUCCESSFUL               0x00
#define SEARCH_CRC_ERROR                0x01

#define FALSE       0
#define TRUE        1

#define MAX_DEVICES 8       //!< Max number of devices to search for.

#define BUSES   (OWI_PIN_0 | OWI_PIN_1) //!< Buses to search.


/*! \brief  Data type used to hold information about slave devices.
 *  
 *  The OWI_device data type holds information about what bus each device
 *  is connected to, and its 64 bit identifier.
 */
typedef struct
{
    unsigned char bus;      //!< A bitmask of the bus the device is connected to.
    unsigned char id[8];    //!< The 64 bit identifier.
} OWI_device;


// Prototypes of functions used in exemples.
unsigned char SearchBuses(OWI_device * devices, unsigned char len, unsigned char buses);
OWI_device * FindFamily(unsigned char familyID, OWI_device * devices, unsigned char size);
signed int DS1820_ReadTemperature(unsigned char bus, unsigned char * id);


/*! \brief  Example application for the polled drivers.
 *
 *  Example application for the software only and polled UART driver.
 *  This example application will find all devices (upper bounded by MAX_DEVICES) 
 *  on the buses defined by BUSES. It then tries to find either a DS1820 or DS2890 
 *  device on a bus, and communicate with them to read temperature (DS1820) or 
 *  set wiper position (DS2890).
 *  This example is not written in a very optimal way. It is merely intended to show
 *  how the polled 1-Wire(R) driver can be used.
 */
void main(void)
{
    static OWI_device devices[MAX_DEVICES];
    OWI_device * ds1820;
    OWI_device * ds2890;
    signed int temperature = 0;
    unsigned char wiperPos = 0;
    
    // Initialize PORTB as output. Can be used to display values on
    // the LEDs on a STK500 development board.
    DDRB = 0xff;

    OWI_Init(BUSES);
    
    // Do the bus search until all ids are read without crc error.    
    while(SearchBuses(devices, MAX_DEVICES, BUSES) != SEARCH_SUCCESSFUL)
    {
    
    }
    
    // See if there is a DS1820 or DS2890 on a bus.
    ds1820 = FindFamily(DS1820_FAMILY_ID, devices, MAX_DEVICES);
    ds2890 = FindFamily(DS2890_FAMILY_ID, devices, MAX_DEVICES);
    
    // Do something useful with the slave devices in an eternal loop.
    for (;;)
    {
        // If there is a DS1820 temperature sensor on a bus, read the
        // temperature.
        // The DS1820 must have Vdd pin connected for this code to work.
        if (ds1820 != NULL)
        {
            temperature = DS1820_ReadTemperature((*ds1820).bus, (*ds1820).id);
        }        
        // If there is a DS2890 digital potentiometer, increment the
        // wiper value.
        if (ds2890 != NULL)
        {
            DS2890_SetWiperPosition(wiperPos++, (*ds2890).bus, (*ds2890).id);
        }
        
        // Discard lsb of temperature and output to PORTB.
        PORTB = ~(temperature >> 1);
    }
}


/*! \brief  Perform a 1-Wire search
 *
 *  This function shows how the OWI_SearchRom function can be used to 
 *  discover all slaves on the bus. It will also CRC check the 64 bit
 *  identifiers.
 *
 *  \param  devices Pointer to an array of type OWI_device. The discovered 
 *                  devices will be placed from the beginning of this array.
 *
 *  \param  len     The length of the device array. (Max. number of elements).
 *
 *  \param  buses   Bitmask of the buses to perform search on.
 *
 *  \retval SEARCH_SUCCESSFUL   Search completed successfully.
 *  \retval SEARCH_CRC_ERROR    A CRC error occured. Probably because of noise
 *                              during transmission.
 */
unsigned char SearchBuses(OWI_device * devices, unsigned char len, unsigned char buses)
{
    unsigned char i, j;
    unsigned char presence;
    unsigned char * newID;
    unsigned char * currentID;
    unsigned char currentBus;
    unsigned char lastDeviation;
    unsigned char numDevices;
    
    // Initialize all addresses as zero, on bus 0 (does not exist).
    // Do a search on the bus to discover all addresses.    
    for (i = 0; i < len; i++)
    {
        devices[i].bus = 0x00;
        for (j = 0; j < 8; j++)
        {
            devices[i].id[j] = 0x00;
        }
    }
    
    // Find the buses with slave devices.
    presence = OWI_DetectPresence(BUSES);
    
    numDevices = 0;
    newID = devices[0].id;
    
    // Go through all buses with slave devices.
    for (currentBus = 0x01; currentBus; currentBus <<= 1)
    {
        lastDeviation = 0;
        currentID = newID;
        if (currentBus & presence) // Devices available on this bus.
        {
            // Do slave search on each bus, and place identifiers and corresponding
            // bus "addresses" in the array.
            do  
            {
                memcpy(newID, currentID, 8);
                OWI_DetectPresence(currentBus);
                lastDeviation = OWI_SearchRom(newID, lastDeviation, currentBus);
                currentID = newID;
                devices[numDevices].bus = currentBus;
                numDevices++;
                newID=devices[numDevices].id;                
            }  while(lastDeviation != OWI_ROM_SEARCH_FINISHED);            
        }
    }

    // Go through all the devices and do CRC check.
    for (i = 0; i < numDevices; i++)
    {
        // If any id has a crc error, return error.
        if(OWI_CheckRomCRC(devices[i].id) != OWI_CRC_OK)
        {
            return SEARCH_CRC_ERROR;
        }
    }
    // Else, return Successful.
    return SEARCH_SUCCESSFUL;
}

/*! \brief  Find the first device of a family based on the family id
 *
 *  This function returns a pointer to a device in the device array
 *  that matches the specified family.
 *
 *  \param  familyID    The 8 bit family ID to search for.
 *
 *  \param  devices     An array of devices to search through.
 *
 *  \param  size        The size of the array 'devices'
 *
 *  \return A pointer to a device of the family.
 *  \retval NULL    if no device of the family was found.
 */
OWI_device * FindFamily(unsigned char familyID, OWI_device * devices, unsigned char size)
{
    unsigned char i = 0;
    
    // Search through the array.
    while (i < size)
    {
        // Return the pointer if there is a family id match.
        if ((*devices).id[0] == familyID)
        {
            return devices;
        }
        devices++;
        i++;
    }
    // Else, return NULL.
    return NULL;
}


/*! \brief  Read the temperature from a DS1820 temperature sensor.
 *
 *  This function will start a conversion and read back the temperature
 *  from a DS1820 temperature sensor.
 *
 *  \param  bus A bitmask of the bus where the DS1820 is located.
 *  
 *  \param  id  The 64 bit identifier of the DS1820.
 *
 *  \return The 16 bit signed temperature read from the DS1820.
 */
signed int DS1820_ReadTemperature(unsigned char bus, unsigned char * id)
{
    signed int temperature;
    
    // Reset, presence.
    if (!OWI_DetectPresence(bus))
    {
        return DS1820_ERROR; // Error
    }
    // Match the id found earlier.
    OWI_MatchRom(id, bus);
    // Send start conversion command.
    OWI_SendByte(DS1820_START_CONVERSION, bus);
    // Wait until conversion is finished.
    // Bus line is held low until conversion is finished.
    while (!OWI_ReadBit(bus))
    {
    
    }
    // Reset, presence.
    if(!OWI_DetectPresence(bus))
    {
        return -1000; // Error
    }
    // Match id again.
    OWI_MatchRom(id, bus);
    // Send READ SCRATCHPAD command.
    OWI_SendByte(DS1820_READ_SCRATCHPAD, bus);
    // Read only two first bytes (temperature low, temperature high)
    // and place them in the 16 bit temperature variable.
    temperature = OWI_ReceiveByte(bus);
    temperature |= (OWI_ReceiveByte(bus) << 8);
    
    return temperature;
}



    // Reset, presence.
    if(!OWI_DetectPresence(bus))
    {
        return;
    }
    //Match id.
    OWI_MatchRom(id, bus);
    
    // Send Write control register command.
    OWI_SendByte(DS2890_WRITE_CONTROL_REGISTER, bus);
    
    // Write 0x4c to control register to enable charge pump.
    OWI_SendByte(0x4c, bus);
    
    // Check that the value returned matches the value sent.
    if (OWI_ReceiveByte(bus) != 0x4c)
    {
        return;
    }
    
    // Send release code to update control register.
    OWI_SendByte(DS2890_RELEASE_CODE, bus);
    
    // Check that zeros are returned to ensure that the operation was
    // successful.
    if (OWI_ReceiveByte(bus) == 0xff)
    {
        return;
    }
    
    // Reset, presence.
    if (!OWI_DetectPresence(bus))
    {
        return;
    }
    
    // Match id.
    OWI_MatchRom(id, bus);
    
    // Send the Write Position command.
    OWI_SendByte(DS2890_WRITE_POSITION, bus);
    
    // Send the new position.
    OWI_SendByte(position, bus);
    
    // Check that the value returned matches the value sent.
    if (OWI_ReceiveByte(bus) != position)
    {
        return;
    }
    
    // Send release code to update wiper position.
    OWI_SendByte(DS2890_RELEASE_CODE, bus);
    
    // Check that zeros are returned to ensure that the operation was
    // successful.
    if (OWI_ReceiveByte(bus) == 0xff)
    {   
        return;
    }
}
