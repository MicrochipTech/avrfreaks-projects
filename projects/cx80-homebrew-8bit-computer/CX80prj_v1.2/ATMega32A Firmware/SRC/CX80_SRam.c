/*
 * CX80_SRam.c
 *
 * Created: 21/02/2013 11:59:54
 *  Author: Calogiuri Enzo Antonio
 */ 
#include "CX80.h"

/*Read a byte from ram chip*/
byte RMem(register word Addr)
{
	register byte value;
	
	SRam_Deselect();
	
	Setup_CX80_Data_Bus_Input();	
	
	Latch_LE_High();
	asm ("nop");
	
	ADDRESS_BUS_PORT = LoByte(Addr);
	
	asm ("nop");
	Latch_LE_Low();	
	//Latch_LE_High();
	
	ADDRESS_BUS_PORT = HiByte(Addr);
	
	SRam_WE_High();
	//asm ("nop");
	SRam_Select();
	asm ("nop");
	asm ("nop");
	value = DATA_BUS_PORT_READ;
	
	SRam_Deselect();
	
	return value;
}

/*Read a word from ram chip*/
word RMem16(register word Addr)
{
	return (RMem(Addr) | (RMem(Addr + 1) << 8));
}

/*Write a byte to ram chip*/
void WMem(register word Addr, register byte Value)
{
	SRam_Deselect();
	
	Setup_CX80_Data_Bus_Output();
	
	Latch_LE_High();
	asm ("nop");
	
	ADDRESS_BUS_PORT = LoByte(Addr);
	
	asm ("nop");
	Latch_LE_Low();
	//asm ("nop");
	//Latch_LE_High();
	
	ADDRESS_BUS_PORT = HiByte(Addr);	
	
	DATA_BUS_PORT = Value;
	
	SRam_WE_Low();
	SRam_Select();
	asm ("nop");	
	asm ("nop");
		
	SRam_Deselect();
	SRam_WE_High();
}

/*Write a word to ram chip*/
void WMem16(register word Addr, register word Value)
{
	WMem(Addr, Value & 255);
	WMem(Addr + 1, (Value >> 8) & 255);
}