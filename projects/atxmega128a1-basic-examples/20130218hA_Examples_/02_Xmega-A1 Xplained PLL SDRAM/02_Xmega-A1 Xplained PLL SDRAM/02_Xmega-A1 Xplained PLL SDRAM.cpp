/*
 * 02_Xmega-A1 Xplained PLL SDRAM.cpp
 *
 *
 * Version: 1.00
 *  Device: ATXmega128A1, SDRAM (MT48LC16M4A2TG, 8 MB, 4 bits data bus :-) )
 *	   Kit: Xmega-A1 Xplained
 *
 * Created: 18. 2. 2013 10:43:05
 * Updated: -
 *
 * Description:
 *    - useing PLL (64 Mhz for SDRAM, 32 Mhz for cpu clock)
 *		-	read and write operation with SDRAM
 *
 * Note.1: I'm not professional programer. I use own, non-standard programming style and techniques. :-)
 *			 	 
 *
 */ 

#define F_CPU 32000000UL

#include <avr/io.h>
#include <util/delay.h>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Drivers/eXmega_clock_driver.hpp"						// Clock
#include "Drivers/eXmega_sdram_driver.hpp"						// SDRAM (XmegaA1-Explained)

eSystemClockDef eSystemClock;
eSDRAM_Def eSDRAM;

// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
  // INIT CLOCK, SDRAM
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// Setup PLL and clock
	eSystemClock.SetClock( ectPLL_128_Mhz_64_Mhz_32_Mhz );				// ClkPer4 = 128 Mhz, ClkPer2 = 64 Mhz (use SDRAM), SysClk = CpuClk = 32 Mhz
	eSDRAM.Init();																								// Init SDRAM via EBI, Start Address = 0x800000, End Address = 0xFFFFFF, Size = 0x800000 (8 MB)
	
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	unsigned char Data[10] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0xFF };			// this is in SRAM
	unsigned char DataIndex = 0, TempByte;

	// PORTE (Leds) as OUTPUT
	PORTE.DIR = 0xFF;
	
	// Write Data to SDRAM at Address 0x812345 (0x800000 = BaseAddress, 0x012345 = Offset)
	eSDRAM.WriteBlock( 0x812345UL, Data, sizeof(Data) );
		
	// Clear Data array
	for (DataIndex = 0; DataIndex < sizeof(Data); DataIndex++)
	{
		Data[ DataIndex ] = 0;
	}	
	
	// Read back from SDRAM
	eSDRAM.ReadBlock( 0x812345UL, Data, sizeof(Data) );
	
	// Show Data on Leds
	DataIndex = 0;
	while(1)
  {
		
		PORTE.OUT = ~Data[ DataIndex ];																	// Show Data on leds
		_delay_ms( 1000 );																							// Wait 1 second
		
		DataIndex++;
		if (DataIndex >= sizeof(Data)) { DataIndex = 0; }
			
		// if you want to read byte from sdram can you do this
		TempByte = eSDRAM.Read8( 0x812345 );														// Read Byte from SDRAM
		TempByte = eSDRAM.Read8( eSDRAM.BaseAddress + 0x012345UL );				
		TempByte = eSDRAM.Read8( eSDRAM.BaseAddress + 0x012345UL + (unsigned long)DataIndex );
  }
}