//
//	1-wire API based on material from Maxim/Dallas
//
//
//


#include "1-Wire.h"
#include "1-wire_bus.h"
#include <avr/pgmspace.h>




#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif



// global search state
//unsigned char	ROM_NO[8];
unsigned int	LastDiscrepancy;
unsigned int	LastFamilyDiscrepancy;
unsigned int	LastDeviceFlag;
unsigned char 	crc8;
unsigned char	OWire_pin;
unsigned char	rom_buf[9];
unsigned char	Last_OW_Pin;



unsigned char OWGetROM(unsigned int OneWirePin, unsigned char* ROM)
{
	unsigned char index;

	if (OneWirePin != Last_OW_Pin) 
	{
		OWInit(OneWirePin);
		Last_OW_Pin = OneWirePin;
	}

	if(OWSearch(OneWirePin, rom_buf))
	{		
		for (index =0;index<8;index++)
		{
			ROM[index] = rom_buf[index];
		}
		return(1);
	}
	
	return(0);
}
	
	
	
	
	
	
//	Dim Result as Byte
	

//	If (OneWirePin <> Last_OW_Pin) then 'reset the search state
//		LastDiscrepancy = 0
//		LastDeviceFlag = 0
//		LastFamilyDiscrepancy = 0
//		Last_OW_Pin = OneWirePin
//'Debug.print "Reset the last pin to current pin"		
//	End If
	
//'Debug.print "starting a search"
//	Call OWSearch(OneWirePin, Result)
//'Debug.print "completed a search, result is ";Cstr(Result)
	
//	If (Result = 1) then
//		Dim Index as Byte
//		For Index = 1 to 8
//			Rom(Index) = ROM_NO(Index)
//		Next 'Index
//	End If
	
//	GetOWROM = CBool(Result)





//--------------------------------------------------------------------------
//
//
//	Initialize the 1-wire search and bus
//

void OWInit(unsigned int OWPin)
{
	LastDiscrepancy = 0;
	LastDeviceFlag = FALSE;
	LastFamilyDiscrepancy = 0;
}

//--------------------------------------------------------------------------
// Find the 'first' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : no device present
//
//int OWFirst()
//{
//   // reset the search state
//   LastDiscrepancy = 0;
//   LastDeviceFlag = FALSE;
//   LastFamilyDiscrepancy = 0;
//
//   return OWSearch();
//}




//--------------------------------------------------------------------------
// Find the 'next' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
//int OWNext()
//{
   // leave the search state alone
//   return OWSearch();
//}



//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
unsigned int OWSearch(unsigned int OWPin, unsigned char *ROM_NO)
{
   int id_bit_number;
   int last_zero, rom_byte_number, search_result;
   int id_bit, cmp_id_bit;
   unsigned char rom_byte_mask, search_direction;

   // initialize for search
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;
   crc8 = 0;

   // if the last call was not the last one
   if (!LastDeviceFlag)
   {
      // 1-Wire reset
      if (!OWReset(OWPin))
      {
         // reset the search
         LastDiscrepancy = 0;
         LastDeviceFlag = FALSE;
         LastFamilyDiscrepancy = 0;
         return FALSE;
      }

      // issue the search command 
      OWWriteByte(OWPin,0xF0);  

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = OWReadBit(OWPin);
         cmp_id_bit = OWReadBit(OWPin);

         // check for no devices on 1-wire
         if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
               search_direction = id_bit;  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
              ROM_NO[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            OWWriteBit(OWPin, search_direction);

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!((id_bit_number < 65) || (crc8 != 0)))
      {
         // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
         LastDiscrepancy = last_zero;

         // check for last device
         if (LastDiscrepancy == 0)
            LastDeviceFlag = TRUE;
         
         search_result = TRUE;
      }
   }

   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !ROM_NO[0])
   {
      LastDiscrepancy = 0;
      LastDeviceFlag = FALSE;
      LastFamilyDiscrepancy = 0;
      search_result = FALSE;
   }

   return search_result;
}

//--------------------------------------------------------------------------
// Verify the device with the ROM number in ROM_NO buffer is present.
// Return TRUE  : device verified present
//        FALSE : device not present
//
unsigned int OWVerify(unsigned int OWPin, unsigned char *ROM_NO)
{
   unsigned char rom_backup[8];
   int i,rslt,ld_backup,ldf_backup,lfd_backup;

   // keep a backup copy of the current state
   for (i = 0; i < 8; i++)
      rom_backup[i] = ROM_NO[i];
   ld_backup = LastDiscrepancy;
   ldf_backup = LastDeviceFlag;
   lfd_backup = LastFamilyDiscrepancy;

   // set search to find the same device
   LastDiscrepancy = 64;
   LastDeviceFlag = FALSE;

   if (OWSearch(OWPin, ROM_NO))
   {
      // check if same device found
      rslt = TRUE;
      for (i = 0; i < 8; i++)
      {
         if (rom_backup[i] != ROM_NO[i])
         {
            rslt = FALSE;
            break;
         }
      }
   }
   else
     rslt = FALSE;

   // restore the search state 
   for (i = 0; i < 8; i++)
      ROM_NO[i] = rom_backup[i];
   LastDiscrepancy = ld_backup;
   LastDeviceFlag = ldf_backup;
   LastFamilyDiscrepancy = lfd_backup;

   // return the result of the verify
   return rslt;
}

//--------------------------------------------------------------------------
// Setup the search to find the device type 'family_code' on the next call
// to OWNext() if it is present.
//
//void OWTargetSetup(unsigned char family_code)
//{
//   int i;
//
   // set the search state to find SearchFamily type devices
   //ROM_NO[0] = family_code;
   //for (i = 1; i < 8; i++)
      //ROM_NO[i] = 0;
   //LastDiscrepancy = 64;
   //LastFamilyDiscrepancy = 0;
   //LastDeviceFlag = FALSE;
//}

//--------------------------------------------------------------------------
// Setup the search to skip the current device type on the next call
// to OWNext().
//
//void OWFamilySkipSetup()
//{
   // set the Last discrepancy to last family discrepancy
   //LastDiscrepancy = LastFamilyDiscrepancy;
   //LastFamilyDiscrepancy = 0;

   // check for end of list
   //if (LastDiscrepancy == 0)
      //LastDeviceFlag = TRUE;
//}

//--------------------------------------------------------------------------
// 1-Wire Functions to be implemented for a particular platform
//--------------------------------------------------------------------------


static const PROGMEM unsigned char dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current 
// global 'crc8' value. 
// Returns current global crc8 value
//
unsigned char docrc8(unsigned char value)
{
   // See Application Note 27
   
   // TEST BUILD
   crc8 = pgm_read_byte_near(&dscrc_table[crc8 ^ value]);
   return crc8;
}



//
//	Alternate method is to calculate the CRC the slow way
//
//
//
/*
unsigned char docrc8(unsigned char value)
{
	unsigned char Shift_Reg, SR_lsb, Data_Bit, v, FB_bit;
	unsigned int i, j

	Shift_Reg = crc8;	//initialize the shift regsiter

	for (j=0; j<7;j++)	//for each bit
	{
		Data_Bit = Data & 0x01;	//isolate least sig bit
		SR_lsb = Shift_Reg & 0x01;
		FB_bit = (Data_Bit ^ SR_lsb) & 0x01;
		//calculate the feed back bit
		Shift_Reg = Shift_Reg >> 1;	//shift right

		if (FB_bit)
		{
			Shift_Reg ^= 0x8c;
		}
		//shift next bit to least sig bit position
		Data >>= 1;
	}
	return (Shift_Reg);
}
*/

