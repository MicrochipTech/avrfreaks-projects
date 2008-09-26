#include <avr/io.h>
#include <avr/wdt.h>

void __Init         (void) __attribute__ ((naked)) __attribute__ ((section (".vectors")));
void __do_copy_data (void) __attribute__ ((naked)) __attribute__ ((section (".init4")));
void __jumpMain     (void) __attribute__ ((naked)) __attribute__ ((section (".init9")));

void __Init(void)
{
	// init stack here, bug in WinAVR 20071221 does not init stack based on __stack
	__asm__ __volatile__
	(
		".set __stack, %0	\n\t"
		"ldi r24, %1		\n\t"
		"ldi r25, %2		\n\t"
		"out __SP_H__, r25	\n\t"
		"out __SP_L__, r24	\n\t"

		/* GCC depends on register r1 set to zero */
		"clr __zero_reg__	\n\t"
		:
		: "i" (RAMEND), "M" (RAMEND & 0xff), "M" (RAMEND >> 8)
	);

	// set SREG to 0
	SREG = 0;

	// set extended indirect jump high address
	#if defined(EIND)
		EIND = 1;
	#endif

//-------------------------------------------------------------------
//	 Turn off Watchdog Timer
//-------------------------------------------------------------------

// Clear wdt reset flag - needed for enhanced wdt devices
	#if defined(MCUCSR)
		MCUCSR = 0;
	#elif defined(MCUSR)
		MCUSR = 0;
	#endif

	__asm__ __volatile__
	(
		"ldi r24, %0			\n\t"
		"sts %1, r24			\n\t"
		"sts %1, __zero_reg__	\n\t"

		/* Jump over our data section */
		"rjmp __do_copy_data    \n\t"
		:
		: "M" ((1<<_WD_CHANGE_BIT) | (1<<WDE)),	"M" (_SFR_MEM_ADDR(_WD_CONTROL_REG))
	);
}


void __do_copy_data(void)
{
}


void __jumpMain(void)
{
	// jump to main()
	asm volatile ( "rjmp main");
}
