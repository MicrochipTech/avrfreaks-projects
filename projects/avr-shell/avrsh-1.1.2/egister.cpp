////////////////////////////////////////////////////////////////////////////
// Name: register.cpp
// Author: B. Gian James <gian@BattleDroids.net>
// Description: This file declares the scripting components that allow 
//              the user to check registers in real-time.
//
// $Id: register.cpp,v 1.5 2009/09/02 12:55:17 gian Exp $
////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <string.h>
#include "progmem.h"
#include "common.h"
#include "register.h"


// If you really need to conserve RAM, you can put each of these strings
// individually into program memory and then put the array there, too.
// Entered strings and struct array into PROGMEM 8.30.09 -gian
RegisterVars
Register::Registers[] PROGMEM =
	{
		{ &PORTB, pmPORTB }, { &TIFR0, pmTIFR0 },
		{ &PORTC, pmPORTC }, { &TIFR1, pmTIFR1 },
		{ &PORTD, pmPORTD }, { &TIFR2, pmTIFR2 },
		{ &DDRD,  pmDDRD},   { &PCIFR, pmPCIFR },
		{ &DDRB,  pmDDRB},   { &EIFR,  pmEIFR},
		{ &DDRC,  pmDDRC},   { &EIMSK, pmEIMSK },
		{ &PINB,  pmPINB},   { &EECR,  pmEECR},
		{ &PINC,  pmPINC},   { &EEDR, pmEEDR },
		{ &PIND,  pmPIND},  
		{ &SREG,  pmSREG},   { &EEARL, pmEEARL },
		{ &GPIOR0, pmGPIOR0 }, { &EEARH, pmEEARH },
		{ &GPIOR1,  pmGPIOR1}, { &GTCCR, pmGTCCR },
		{ &GPIOR2,  pmGPIOR2}, { &TCCR0A, pmTCCR0A },
		{ &TCCR0B, pmTCCR0B}, { &TCNT0, pmTCNT0 },
		{ &OCR0A, pmOCR0A }, { &OCR0B, pmOCR0B },
		{ &SPCR, pmSPCR }, { &SPDR, pmSPDR },
		{ &ACSR, pmACSR }, { &SMCR, pmSMCR },
		{ &MCUSR, pmMCUSR }, { &MCUCR, pmMCUCR },
		{ &SPMCSR,pmSPMCSR  }, { &WDTCSR, pmWDTCSR },
		{ &CLKPR, pmCLKPR }, { &PRR, pmPRR },
		{ &OSCCAL,pmOSCCAL  }, { &PCICR, pmPCICR },
		{ &EICRA, pmEICRA }, { &PCMSK0, pmPCMSK0 },
		{ &PCMSK1, pmPCMSK1 }, { &TIMSK0, pmTIMSK0 },
		{ &TIMSK1, pmTIMSK1 }, { &TIMSK2, pmTIMSK2 },
		{ &ADCL, pmADCL }, { &ADCH, pmADCH },
		{ &ADCSRA, pmADCSRA }, { &ADCSRB, pmADCSRB },
		{ &ADMUX,  pmADMUX}, { &DIDR0, pmDIDR0 },
		{ &DIDR1, pmDIDR1 }, { &TCCR1A, pmTCCR1A },
		{ &TCCR1B, pmTCCR1B }, { &TCCR1C, pmTCCR1C },
		{ &TCNT1L, pmTCNT1L },
		{ &TCNT1H, pmTCNT1H },
		{ &ICR1L, pmICR1L }, { &ICR1H, pmICR1H },
		{ &OCR1AL, pmOCR1AL },
		{ &OCR1AH, pmOCR1AH }, 
		{ &OCR1BL, pmOCR1BL }, { &OCR1BH, pmOCR1BH },
		{ &TCCR2A, pmTCCR2A }, { &TCCR2B, pmTCCR2B },
		{ &TCNT2, pmTCNT2 }, { &OCR2A, pmOCR2A },
		{ &OCR2B, pmOCR2B }, { &ASSR, pmASSR },
		{ &TWBR, pmTWBR }, { &TWSR, pmTWSR },
		{ &TWAR, pmTWAR }, { &TWDR, pmTWDR },
		{ &TWCR, pmTWCR}, { &TWAMR, pmTWAMR },
		{ &UCSR0A, pmUCSR0A }, { &UCSR0B, pmUCSR0B },
		{ &UCSR0C, pmUCSR0C }, 
		{ &UBRR0L, pmUBRR0L }, { &UBRR0H, pmUBRR0H },
		{ &UDR0, pmUDR0 },
		{ NULL, NULL }
	};

void
Register::WriteRegister(uint8 * reg)
{
	char buff[30];
	uint8 byte[8];
	for(uint8 x = 0; x < 8; x++) {
		if (BCHK(*reg,x))
			byte[(7-x)] = 1;
		else
			byte[(7-x)] = 0;
	}
	sprintf_P(buff,PSTR("(0x%x) set to [%d%d%d%d%d%d%d%d]"),
		*reg,byte[0],byte[1],byte[2],byte[3],byte[4],byte[5],byte[6],byte[7]);
	WriteRAM(buff,0);

}

char * 
Register::getRegNameFromPgmSpace(uint8 idx)
{
	RegisterVars * reg = &Registers[idx];
	char * rname = (char*)pgm_read_word(&reg->name);
	return rname;
}

void
Register::PrintRegisters()
{ 
	uint8 x = 1;
	char buff[60];
	WriteLine(PSTR("I know about the following registers:\r\n"),0);
	char * rname = getRegNameFromPgmSpace(x);
	char crname[7]; 
	while(rname != NULL) {
		strcpy_P(crname,rname);
		sprintf_P(buff,PSTR("%-7s    "),crname);
		WriteRAM(buff,0);
		if (!(x % 6))
			WriteLine(PSTR("\r\n"),0);
		++x;
		rname = getRegNameFromPgmSpace(x);
	}
	sprintf_P(buff,PSTR("%-7s     \r\n"),crname);
	WriteRAM(buff);
}

uint8 * 
Register::GetRegisterByName(const char * n)
{
	uint8 x = 0;
	char * rname = getRegNameFromPgmSpace(x);

	while(rname != NULL) {
		RegisterVars * reg = &Registers[x];
		if (!strcmp_P(n,rname))
			return (uint8*)pgm_read_byte(&reg->reg);
		++x;
		rname = getRegNameFromPgmSpace(x);
	}
	return (uint8*)NULL;

}


