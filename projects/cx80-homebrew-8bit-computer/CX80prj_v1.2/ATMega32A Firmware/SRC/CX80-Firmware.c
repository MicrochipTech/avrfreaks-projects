/*
 * CX80_Firmware.c
 *
 * Created: 20/02/2013 11:23:23
 *  Author: Calogiuri Enzo Antonio
 */ 

#include "CX80.h"
#include "CX80_DiskIO.h"
#include "CX80_Ff.h"
#include "CX80_Keyboard.h"
#include "CX80_OS.h"

FATFS FatFs;
FIL Files[3];
char FileSlot[3] = {0};

/*Setup the address bus of system*/
void Setup_CX80_Address_Bus(void)
{
	ADDRESS_BUS_PORT_DDR = 0xFF;
	ADDRESS_BUS_PORT = 0;
}

/*Setup the data bus of system for input operations*/
void Setup_CX80_Data_Bus_Input(void)
{
	DATA_BUS_PORT_DDR = 0;
	DATA_BUS_PORT = 0xFF;
}

/*Setup the data bus of system for output operations*/
void Setup_CX80_Data_Bus_Output(void)
{
	DATA_BUS_PORT_DDR = 0xFF;
	DATA_BUS_PORT = 0;
}

/*Setup the system control bus*/
void Setup_CX80_System_Bus(void)
{
	SYSTEM_BUS_PORT_DDR |= (1 << SYSTEM_SND) | (1 << SYSTEM_SD_SEL) | (1 << SYSTEM_RAM_WR) | 
						   (1 << SYSTEM_RAM_SEL) | (1 << SYSTEM_LATCH);
}

/*Setup the data bus for SDCard operations*/
void Setup_CX80_SD_Bus(void)
{
	SRam_Deselect();
	Latch_LE_High();
	
	DATA_BUS_PORT = 0x0;
	DATA_BUS_PORT_DDR = 0b00000011;	
	
	SD_CS_High();
	
	DATA_BUS_PORT = 0b00000101;
}

/*Execute primary system test*/
void CX80_PowerOn(void)
{
	byte v,r;
	DSTATUS res;
	
	CX80_Do_MoveCursor(2, 7);
	CX80_Do_PrintString("CX80 - Homebrew Computer");
	CX80_Do_MoveCursor(3, 10);
	CX80_Do_PrintString("(c)Calogiuri E. A.");
	
	CX80_Do_PrintString("\r\r");
	
	WMem(0x120, (byte)133);
	WMem(0x130, (byte)80);
	
	v = RMem(0x120);
	r = RMem(0x130);	
	
	CX80_Do_PrintString("RAM:");	
	
	if (!((v == 133) && (r == 80)))
	{
		CX80_Do_PrintString("Error!");
		
		Enable_CX80_Led(SDR_LED);
		
		while(1);
	}
	
	CX80_Do_PrintString("Ok!\r");	
	
	CX80_Do_PrintString("SD:");	
	
	res = disk_initialize();
	
	if (!(res == 0))
	{
		CX80_Do_PrintString("Error!");
		
		Enable_CX80_Led(SDR_LED);
		
		while(1);
	}
	
	CX80_Do_PrintString("Ok!\r");	
	
	CX80_Do_PrintString("SD MOUNT:");	
	
	res = f_mount(0, &FatFs);
	
	if (!(res == 0))
	{
		CX80_Do_PrintString("Error!");
		
		Enable_CX80_Led(SDR_LED);
		
		while(1);
	}
	
	CX80_Do_PrintString("Ok!");
	CX80_Do_PrintString("\r\r");
}

int main(void)
{
	_delay_ms(400);		//Delay for stable Vcc
	
	Seput_CX80_Usart();
	Setup_CX80_Led_Port();
	Setup_CX80_Address_Bus();
	Setup_CX80_System_Bus();
	
	SRam_Deselect();
	Latch_LE_High();
	SD_CS_High();
	
	IOREQ_PORT_DDR = 0xFF;
	IOREQ_PORT |= (1 << IOREQ);
	
	Enable_CX80_Led(PWR_LED);
	Disable_CX80_Led(SDW_LED);
	Disable_CX80_Led(SDR_LED);
	
    /*Enable the video chip (Tellymate compatible)*/
	CX80_Usart_SendChar('U');
	CX80_Do_ClearScreen();
	
	CX80_KeyboardInit();
	
	CX80_PowerOn();
	
	CX80_InitZ80CPU();
		
    CX80_Run_OS();
}