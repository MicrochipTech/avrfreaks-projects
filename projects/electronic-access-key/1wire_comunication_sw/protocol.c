//********************************************************************
//  CPU:         ATMEL "ATmega128" (single chip mode, clock 16 MHz)
//  FILE:        1WIRE.h
//  DESCRIPTION: Procedure for 1Wire (DS1971 I-Button)  
//  VERSION:     1.0
//  DATE:        May 2006
//  AUTHOR:      Ing. Domenico Tupputi
//********************************************************************

//----------------------------------------------------------------------
// HEADER FILES
//----------------------------------------------------------------------
#include "1w_header.h"


//---------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to reset and detect the DS1971 chip on bus												 
// PARAMETERS IN:         
// PARAMETERS OUT:		0x00--> if DS1971 not present on bus
// 			  			0x01--> if DS1971 is present on bus
// LOCAL VARIABLE:      unsigned char data; 
// GLOBAL VARIABLE:     
// COSTANTS:     
//----------------------------------------------------------------------------
unsigned char OneWire_Reset_Presence (void)
{
  unsigned char data;
  uart0_init_9600(); 			//Set UART0 @ 9600bps
     
  data = 0x00;					//Clear data
  TransmitByte(0xF0);			//Transmit 480uS reset pulse
  data = ReceiveByte();			//Receive the transmit data
  if(data == 0xF0)				//Receive 0xF0 if there isn't DS1971 on the bus
    return 0;					
    else	   					//Receive a different data if DS1971 is present on the bus
      return 1;   			
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read DS1971 ROM												 
// PARAMETERS IN:         
// PARAMETERS OUT:		
// LOCAL VARIABLE:      unsigned char i; 
// 		 				unsigned char data;
// GLOBAL VARIABLE:     Buffer_RX;
// COSTANTS:        
//---------------------------------------------------------------------------
void OneWire_Read_ROM (void)
{
  unsigned char i;
  unsigned char chip_presence = 0;
     
  chip_presence = OneWire_Reset_Presence();  
  if (chip_presence == 1)					//If DS1971 is present on the bus
    {
      OneWire_Write_Byte(0x33);     //Read DS1971 ROM
	  for (i=0;i<=8;i++)
	  Buffer_RX[i] = OneWire_Read_Byte();
	 }
  return;
}


//----------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to write, verify one byte to DS1971 EEPROM 											 
// PARAMETERS IN:       unsigned char address;
//                      unsigned char data.
// PARAMETERS OUT:		
// LOCAL VARIABLE:      unsigned char chip_presence;
// GLOBAL VARIABLE:    
// COSTANTS:        
//----------------------------------------------------------------------------
void OneWire_Write_EEPROM (unsigned char address, unsigned char data)
{
  long wait;
  unsigned char chip_presence = 0;
  unsigned char verify_data = 0;
    
  chip_presence = OneWire_Reset_Presence();
  if (chip_presence == 1)
    {
      OneWire_Write_Byte(0xCC);  //Send to DS1971 "Skip ROM" command
      OneWire_Write_Byte(0x0F);  //Send to DS1971 "Write Scratchpad" command

      OneWire_Write_Byte(address);	//Send to DS1971 address 
      OneWire_Write_Byte(data);		//Send to DS1971 data 
  }
  for(wait=0;wait<100000;wait++);
  chip_presence = OneWire_Reset_Presence();
  if (chip_presence == 1)
    {
      OneWire_Write_Byte(0xCC);  //Send to DS1971 "Skip ROM" command
      OneWire_Write_Byte(0xAA);  //Send to DS1971 "Read Scratchpad" command

      OneWire_Write_Byte(address);	     //Send to DS1971 address 
      verify_data = OneWire_Read_Byte(); //Read and verify data  
  }
  for(wait=0;wait<100000;wait++);  
  chip_presence = OneWire_Reset_Presence();
  if (chip_presence == 1)
    {
      OneWire_Write_Byte(0xCC);  //Send to DS1971 "Skip ROM" command
      OneWire_Write_Byte(0x55);  //Send to DS1971 "Copy Scratchpad" command
      OneWire_Write_Byte(0xA5);	 //Send to DS1971 "Validation Key" 
    }
  for(wait=0;wait<100000;wait++);
  return;
}


//----------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read DS1971 total EEPROM 											 
// PARAMETERS IN:       
// PARAMETERS OUT:		
// LOCAL VARIABLE:      
// GLOBAL VARIABLE:     Buffer_RX;
// COSTANTS:        
//----------------------------------------------------------------------------
void OneWire_Read_All_EEPROM (void)
{
  unsigned char chip_presence = 0;
  unsigned char i = 0; 
   
  for(i=0;i<=32;i++)				
    Buffer_RX[i] = 0;				//Clear all receive buffer

  chip_presence = OneWire_Reset_Presence();
  if (chip_presence == 1)
    {
	  OneWire_Write_Byte(0xCC);		//Send to DS1971 "Skip ROM" command	   
      OneWire_Write_Byte(0xF0);     //Send to DS1971 "Read Memory" command
      OneWire_Write_Byte(0x00);		//Send to DS1971 start address 0x00
	   
	  for (i=0;i<=32;i++)
	  Buffer_RX[i] = OneWire_Read_Byte();  
	}
  return;
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to write "0" on the bus 										 
// PARAMETERS IN:         
// PARAMETERS OUT:		
// LOCAL VARIABLE:     
// GLOBAL VARIABLE:     
// COSTANTS:     
//---------------------------------------------------------------------------
void OneWire_Write_Bit0(void)
{
  uart0_init_115200();	 	   //Set UART0 @ 115200bps												 
  TransmitByte(0x00);		   //Transmit logical "0" byte
  return;
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to write "1" on the bus 										 
// PARAMETERS IN:         
// PARAMETERS OUT:		
// LOCAL VARIABLE:     
// GLOBAL VARIABLE:     
// COSTANTS:     
//---------------------------------------------------------------------------
void OneWire_Write_Bit1(void)  
{
  uart0_init_115200();       //Set UART0 @ 115200bps
  TransmitByte(0xFF);	     //Transmit logical "1"
  return;
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to write one byte on the bus										 
// PARAMETERS IN:       unsigned char byte;  
// PARAMETERS OUT:		
// LOCAL VARIABLE:      unsigned char cont;
// GLOBAL VARIABLE:     
// COSTANTS:     
//--------------------------------------------------------------------------- 
void OneWire_Write_Byte(unsigned char byte)
{		 
  unsigned char cont;
 
  for(cont=0;cont<=7;cont++)
    {
     if(byte & (0x01<<cont))
	   OneWire_Write_Bit1();
	   else 
         OneWire_Write_Bit0();
	}
  return;
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read one bit from DS1971										 
// PARAMETERS IN:         
// PARAMETERS OUT:		0x00--> if DS1971 transmit to the master one "0" bit
// 			  			0x01--> if DS1971 transmit to the master one "1" bit
// LOCAL VARIABLE:      unsigned char data;
// GLOBAL VARIABLE:     
// COSTANTS:     
//--------------------------------------------------------------------------- 
unsigned char OneWire_Read_Bit(void)  
{
  unsigned char data;

  uart0_init_115200();    //Set UART0 @ 115200 
  TransmitByte(0xFF);	  //Transmit the byte 0xFF
  data = ReceiveByte();	  //Receive the byte
  if(data == 0xFF)		  //If receive byte is 0xFF DS1971 transmit to the master "1" 
    return 1;					 
    else	   					
      return 0;   		  //If receive byte is different from 0xFF DS1971 transmit to the master "0"	
}


//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read one byte from DS1971										 
// PARAMETERS IN:         
// PARAMETERS OUT:		unsigned char byte;
// LOCAL VARIABLE:      unsigned char cont;
// 		 				unsigned char data_bit;
//						unsigned char byte;
// GLOBAL VARIABLE:     
// COSTANTS:     
//--------------------------------------------------------------------------- 
unsigned char OneWire_Read_Byte(void)
{		 
  unsigned char cont;
  unsigned char data_bit;
  unsigned char byte;

  byte = 0;
 
  for(cont=0;cont<=7;cont++)
    {
     data_bit = OneWire_Read_Bit();
     if (data_bit == 1)
	   byte |= (data_bit << cont); 
	}

  return byte;
}

  