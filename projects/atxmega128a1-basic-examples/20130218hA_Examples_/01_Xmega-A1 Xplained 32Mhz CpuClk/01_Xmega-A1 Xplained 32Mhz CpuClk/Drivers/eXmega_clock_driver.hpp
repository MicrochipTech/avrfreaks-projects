/*
 * eXmega_clock_driver.hpp
 *
 * Devices: ATXmega128A1
 *     Kit: Xmega-A1 Xplained
 *
 * Version: 1.01
 * Created: 13. 1. 2013 2:02:27
 * Updated: -
 */ 


#ifndef _eXmega_clock_driver_hpp_
#define _eXmega_clock_driver_hpp_

typedef enum
{
	ectInternal_Oscilator_2_Mhz = 0,
	ectInternal_Oscilator_32_Mhz,
	ectPLL_128_Mhz_64_Mhz_32_Mhz,
} eClockTypeDef;

// =============================================================================================================================================================================================
class eSystemClockDef
{
	private:
		static void eCPPSet( volatile uint8_t * address, uint8_t value );
		
	public:

		static void SetClock( eClockTypeDef ClockType );
						
};

// =============================================================================================================================================================================================
void eSystemClockDef::SetClock( eClockTypeDef ClockType )
{
	unsigned char Value, Clock_Source_Mask;

	// SELECT CLOCK SOURCE: 2 Mhz Int. Osc. Clock 
	Value = (0 << 2) | (0 << 1) | (0 << 0);						// 2 Mhz Int. Osc.
	eCPPSet( &CLK.CTRL, Value );

	// DISABLE PLL
	switch( ClockType )
	{
		default: { break; }
		case ectPLL_128_Mhz_64_Mhz_32_Mhz:
		{
			Value = OSC.CTRL & ~(1 << 4);									// DISABLE PLL
			eCPPSet( &OSC.CTRL, Value );
			break;
		}
	};			

	// SETUP PLL
	switch( ClockType )
	{
		default:
		{
			OSC.PLLCTRL = (0 << 7) | (0 << 6) |						// 2 Mhz Int. Osc.
										(1);														// 1x
			break;
		}			
		case ectPLL_128_Mhz_64_Mhz_32_Mhz:
		{
			OSC.PLLCTRL = (1 << 7) | (0 << 6)	|						// 32 Mhz. Int. Osc.
										(16);														// 16x = 32 / 4 * 16 = 128 MHz (ClkPer4)
			break;
		}
	};			
									
	// ENABLE CLOCK SOURCES
	switch( ClockType )
	{
		// case ectInternal_Oscilator_2_Mhz:
		default:
		{
			Clock_Source_Mask = (1 << 0);							// 2 Mhz Int. Osc.
			break;
		}
		case ectInternal_Oscilator_32_Mhz:
		{
			Clock_Source_Mask = (1 << 1);							// 32 Mhz Int. Osc.		
			break;
		}
		case ectPLL_128_Mhz_64_Mhz_32_Mhz:
		{
			Clock_Source_Mask = (1 << 4) | (1 << 1);	// PLL
			break;
		}				
	};
	/*
	Clock_Source_Mask = (1 << 2) |								// 32 kHz Int. Osc.
											(1 << 1) |								// 32 Mhz Int. Osc.
											(1 << 4);									// PLL
	 */
	// Clock_Source_Mask |= (1 << 2);								// 32 kHz Int. Osc.
	eCPPSet( &OSC.CTRL, Clock_Source_Mask );

	// SET PRESCALLERS
	switch( ClockType )
	{
		case ectPLL_128_Mhz_64_Mhz_32_Mhz:
		{
			Value = (0 << 6) | (0 << 5) | (0 << 4) | (0 << 3) | (0 << 2) |					// PSADIV = DIV 1	(ClkPer4 = 128 Mhz) 
							(1 << 1) | (1 << 0);																						// PSBDIV = 2 (ClkPer2 = 64 Mhz), PSCDIV = 2 (ClkSys, ClkCpu = 32 Mhz)
		  break;
		}
		default:
		{		
			Value = (0 << 6) | (0 << 5) | (0 << 4) | (0 << 3) | (0 << 2) |					// PSADIV = DIV 1
							(0 << 1) | (0 << 0);																						// PSBDIV = 1, PSCDIV = 1 
			break;
		}
	};			
	eCPPSet( &CLK.PSCTRL, Value );
	
	// WAIT FOR CLOCK SOURCES IS READY
	while ( (OSC.STATUS & Clock_Source_Mask) != Clock_Source_Mask );				
							 
	// SELECT CLOCK SOURCE: PLL	
	switch( ClockType )
	{
		case ectInternal_Oscilator_2_Mhz:
		{
			Value = (0 << 2) | (0 << 1) | (0 << 0);															// 2 Mhz. Int. Osc.
			break;
		}
		case ectInternal_Oscilator_32_Mhz:
		{
			Value = (0 << 2) | (0 << 1) | (1 << 0);															// 32 Mhz. Int. Osc.
			break;
		}
		case ectPLL_128_Mhz_64_Mhz_32_Mhz:
		{				
			Value = (1 << 2) | (0 << 1) | (0 << 0);															// PLL
			break;
		}
	};			
	eCPPSet( &CLK.CTRL, Value );
	
	// DISABLE OTHER CLOCK SOURCES
	eCPPSet( &OSC.CTRL, Clock_Source_Mask );		
	
	// SET CALIBRATION
	/*
	DFLLRC32M.COMP0 = 0x50;
	DFLLRC32M.COMP1 = 0x37;
	DFLLRC32M.COMP2 = 0xA0;
	DFLLRC32M.CTRL = (1 << 0);		// AUTO CALIB. ENABLE
	*/
									
}	

// =============================================================================================================================================================================================
void eSystemClockDef::eCPPSet( volatile uint8_t * address, uint8_t value )
{
	#ifdef __ICCAVR__

	// Store global interrupt setting in scratch register and disable interrupts.
	asm( "in  R1, 0x3F \n"
	"cli"
	);

	// Move destination address pointer to Z pointer registers.
	asm("movw r30, r16");
	
	#ifdef RAMPZ
	asm( "ldi  R16, 0 \n"
	"out  0x3B, R16"
	);
	#endif

	asm( "ldi  r16,  0xD8 \n"
	"out  0x34, r16  \n"
	#if (__MEMORY_MODEL__ == 1)
	"st     Z,  r17  \n");
	#elif (__MEMORY_MODEL__ == 2)
	"st     Z,  r18  \n");
	#else /* (__MEMORY_MODEL__ == 3) || (__MEMORY_MODEL__ == 5) */
	"st     Z,  r19  \n");
	#endif /* __MEMORY_MODEL__ */

	// Restore global interrupt setting from scratch register.
	asm( "out  0x3F, R1");

	#elif defined __GNUC__
	// AVR_ENTER_CRITICAL_REGION( );
	
	volatile uint8_t * tmpAddr = address;
	
	#ifdef RAMPZ
	RAMPZ = 0;
	#endif
	
	asm volatile
	(
	"movw r30,  %0"	      "\n\t"
	"ldi  r16,  %2"	      "\n\t"
	"out   %3, r16"	      "\n\t"
	"st     Z,  %1"       "\n\t"
	:
	: "r" (tmpAddr), "r" (value), "M" (0xD8), "i" (&CCP)
	: "r16", "r30", "r31"
	);
	// AVR_LEAVE_CRITICAL_REGION( );
	#endif
}



#endif 