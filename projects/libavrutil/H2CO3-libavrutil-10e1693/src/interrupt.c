/*
 * interrupt.c
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "interrupt.h"

static volatile avr_interrupt_handler __avr_intr_handlers[] = {
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
#endif /* __AVR_ATmega1280__ || __AVR_ATmega2560__ */
	NULL,
	NULL
};


void avr_interrupt_set_handler(uint8_t intnum, avr_interrupt_handler hndl, uint8_t mode)
{
	if (intnum >= AVR_INTERRUPTS_NUM) /* No such external interrupt */
	{
		return;
	}

	__avr_intr_handlers[intnum] = hndl;

	/*
	 * Configure the interrupt trigger mode. The mode constants were chosen
	 * to correspond to the configuration bits in the hardware register,
	 * so we can simply shift the mode into place.
	 */

	switch (intnum)
	{
#if defined(EICRA) && defined(EICRB) && defined(EIMSK)
		case 2:
			EICRA = (EICRA & ~(_BV(ISC00) | _BV(ISC01))) | (mode << ISC00);
			EIMSK |= _BV(INT0);
			break;
		case 3:
			EICRA = (EICRA & ~(_BV(ISC10) | _BV(ISC11))) | (mode << ISC10);
			EIMSK |= _BV(INT1);
			break;
		case 4:
			EICRA = (EICRA & ~(_BV(ISC20) | _BV(ISC21))) | (mode << ISC20);
			EIMSK |= _BV(INT2);
			break;
		case 5:
			EICRA = (EICRA & ~(_BV(ISC30) | _BV(ISC31))) | (mode << ISC30);
			EIMSK |= _BV(INT3);
			break;
		case 0:
			EICRB = (EICRB & ~(_BV(ISC40) | _BV(ISC41))) | (mode << ISC40);
			EIMSK |= _BV(INT4);
			break;
		case 1:
			EICRB = (EICRB & ~(_BV(ISC50) | _BV(ISC51))) | (mode << ISC50);
			EIMSK |= _BV(INT5);
			break;
		case 6:
			EICRB = (EICRB & ~(_BV(ISC60) | _BV(ISC61))) | (mode << ISC60);
			EIMSK |= _BV(INT6);
			break;
		case 7:
			EICRB = (EICRB & ~(_BV(ISC70) | _BV(ISC71))) | (mode << ISC70);
			EIMSK |= _BV(INT7);
			break;
#else /* EICRA && EICRB && EIMSK */
		case 0:
#if defined(EICRA) && defined(ISC00) && defined(EIMSK)
			EICRA = (EICRA & ~(_BV(ISC00) | _BV(ISC01))) | (mode << ISC00);
			EIMSK |= _BV(INT0);
#elif defined(MCUCR) && defined(ISC00) && defined(GICR)
			MCUCR = (MCUCR & ~(_BV(ISC00) | _BV(ISC01))) | (mode << ISC00);
			GICR |= _BV(INT0);
#elif defined(MCUCR) && defined(ISC00) && defined(GIMSK)
			MCUCR = (MCUCR & ~(_BV(ISC00) | _BV(ISC01))) | (mode << ISC00);
			GIMSK |= _BV(INT0);
#else
#error Interrupt could not be set!
#endif
			break;

		case 1:
#if defined(EICRA) && defined(ISC10) && defined(ISC11) && defined(EIMSK)
			EICRA = (EICRA & ~(_BV(ISC10) | _BV(ISC11))) | (mode << ISC10);
			EIMSK |= _BV(INT1);
#elif defined(MCUCR) && defined(ISC10) && defined(ISC11) && defined(GICR)
			MCUCR = (MCUCR & ~(_BV(ISC10) | _BV(ISC11))) | (mode << ISC10);
			GICR |= _BV(INT1);
#elif defined(MCUCR) && defined(ISC10) && defined(GIMSK) && defined(GIMSK)
			MCUCR = (MCUCR & ~(_BV(ISC10) | _BV(ISC11))) | (mode << ISC10);
			GIMSK |= _BV(INT1);
#else
#error Interrupt could not be set!
#endif
			break;
		default:
			break;
#endif /* EICRA && EICRB && EIMSK */
	}
}

void avr_interrupt_remove_handler(uint8_t intnum)
{
	if (intnum >= AVR_INTERRUPTS_NUM) /* No such external interrupt */
	{
		return;
	}
	
	switch (intnum)
	{
#if defined(EICRA) && defined(EICRB) && defined(EIMSK)
		case 2:
			EIMSK &= ~_BV(INT0);
			break;
		case 3:
			EIMSK &= ~_BV(INT1);
			break;
		case 4:
			EIMSK &= ~_BV(INT2);
			break;
		case 5:
			EIMSK &= ~_BV(INT3);
			break;
		case 0:
			EIMSK &= ~_BV(INT4);
			break;
		case 1:
			EIMSK &= ~_BV(INT5);
			break;
		case 6:
			EIMSK &= ~_BV(INT6);
			break;
		case 7:
			EIMSK &= ~_BV(INT7);
			break;
#else /* EICRA && EICRB && EIMSK */
		case 0:
#if defined(EIMSK) && defined(INT0)
			EIMSK &= ~_BV(INT0);
#elif defined(GICR) && defined(ISC00)
			GICR &= ~_BV(INT0);
#elif defined(GIMSK) && defined(INT0)
			GIMSK &= ~_BV(INT0);
#else
#error Interrupt handler could not be removed!
#endif
			break;

		case 1:
#if defined(EIMSK) && defined(INT1)
			EIMSK &= ~_BV(INT1);
#elif defined(GICR) && defined(INT1)
			GICR &= ~_BV(INT1);
#elif defined(GIMSK) && defined(INT1)
			GIMSK &= ~_BV(INT1);
#else
#error Interrupt handler could not be removed!
#endif
			break;
#endif /* EICRA && EICRB && EIMSK */
		default:
			break;
		}

		__avr_intr_handlers[intnum] = NULL;
}

#if defined(EICRA) && defined(EICRB)

ISR(INT0_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_2])
	{
		__avr_intr_handlers[AVR_INTR_EXT_2]();
	}
}

ISR(INT1_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_3])
	{
		__avr_intr_handlers[AVR_INTR_EXT_3]();
	}
}

ISR(INT2_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_4])
	{
		__avr_intr_handlers[AVR_INTR_EXT_4]();
	}
}

ISR(INT3_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_5])
	{
		__avr_intr_handlers[AVR_INTR_EXT_5]();
	}
}

ISR(INT4_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_0])
	{
		__avr_intr_handlers[AVR_INTR_EXT_0]();
	}
}

ISR(INT5_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_1])
	{
		__avr_intr_handlers[AVR_INTR_EXT_1]();
	}
}

ISR(INT6_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_6])
	{
		__avr_intr_handlers[AVR_INTR_EXT_6]();
	}
}

ISR(INT7_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_7])
	{
		__avr_intr_handlers[AVR_INTR_EXT_7]();
	}
}

#else /* EICRA && EICRB */

ISR(INT0_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_0])
	{
		__avr_intr_handlers[AVR_INTR_EXT_0]();
	}
}

ISR(INT1_vect)
{
	if (__avr_intr_handlers[AVR_INTR_EXT_1])
	{
		__avr_intr_handlers[AVR_INTR_EXT_1]();
	}
}

#endif /* EICRA && EICRB */

