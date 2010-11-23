////////////////////////////////////////////////////////////////////////////
// Name: shell.cpp
// Author: B. Gian James <gian@BattleDroids.net>
// Description: The shell file for the AVR Shell. Written to be used with
//              the AVR Telnet Gateway.  Contains the shell functionality.
//
// $Id: shell.cpp,v 1.7 2009/09/05 04:06:17 gian Exp $
////////////////////////////////////////////////////////////////////////////
#include "shell.h"
#include "rbuff.h"
#include <string.h>
#include <avr/boot.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "progmem.h"

#define MAX_ARGS	6
#define MAX_PASSWD	12
#define WHITE_SPACE " \r\t\n"

static char		buff[40];

static uint8 * 	getPortByName(char *);
static uint8	getPinByName(char *);
static uint8 *	getDDRByName(char *);

char ee_passwd[MAX_PASSWD] EEMEM;

uint8 * getDDRByName(char * pin)
{
	for(uint8 i = 0; i < NUM_PINS; i++)
	{
		PortPin * ppp = &PortPins[i];
		char * pname = (char*)pgm_read_word(&ppp->name);
		if (!strcasecmp_P(pin,pname)) {
			return (uint8*)pgm_read_word(&ppp->ddr);
		}
	}
	return (uint8*)NUM_PINS;
}
uint8* getPortByName(char * pin)
{
	for(uint8 i = 0; i < NUM_PINS; i++)
	{
		PortPin * ppp = &PortPins[i];
		char * pname = (char*)pgm_read_word(&ppp->name);
		if (!strcasecmp_P(pin,pname))
			return (uint8*)pgm_read_word(&ppp->port);
	}
	return (uint8*)NUM_PINS;
}

uint8 getPinByName(char * pin)
{
	for (uint8 i = 0; i < NUM_PINS; i++)
	{
		PortPin * ppp = &PortPins[i];
		char * pname = (char*)pgm_read_word(&ppp->name);
		if (!strcasecmp_P(pin,pname))
			return (uint8)pgm_read_byte(&ppp->pin);
	}
	return NUM_PINS;
}

void
AVRShell::cmdPowerUp(char ** args)
{
	if (!strcmp(*args,"adc")) {
		power_adc_enable();
	} else if (!strcmp(*args,"spi")) {
		power_spi_enable();
	} else if (!strcmp(*args,"uart")) {
		power_usart0_enable();
	} else if (!strcmp(*args,"twi")) {
		power_twi_enable();
	} else if (!strcmp(*args,"all")) {
		power_all_enable();
	}
	sprintf_P(buff,PSTR("Powerup of %s complete.\r\n"),args[0]);
	WriteRAM(buff);
}

void
AVRShell::cmdPowerDown(char ** args)
{
	if (!strcmp(*args,"adc")) {
		power_adc_disable();
	} else if (!strcmp(*args,"spi")) {
		power_spi_disable();
	} else if (!strcmp(*args,"uart")) {
		power_usart0_disable();
	} else if (!strcmp(*args,"twi")) {
		power_twi_disable();
	} else if (!strcmp(*args,"all")) {
		power_all_disable();
	}
	sprintf_P(buff,PSTR("Powerdown of %s complete.\r\n"),args[0]);
	WriteRAM(buff);
}

void
AVRShell::cmdPasswd(char ** args)
{
	char passwd[MAX_PASSWD];
	eeprom_read_block((void*)&passwd,(const void*)ee_passwd,MAX_PASSWD);
	if (passwd[0] == 0xFF) {
		eeprom_write_block((const void*)args[0],(void*)ee_passwd,MAX_PASSWD);
		WriteLine(PSTR("Wrote root password to EEPROM\r\n"));
	} else if (bIsAuthorized) {
		eeprom_write_block((const void*)args[0],(void*)ee_passwd,MAX_PASSWD);
		WriteLine(PSTR("Wrote NEW root password to EEPROM\r\n"));
	} else
		WriteLine(PSTR("You must authorize yourself first.\r\n"));
}

void
AVRShell::cmdSet(char ** args)
{
	if (!strcmp(*args,"root")) {
		char passwd[MAX_PASSWD];
		memset(passwd,0xFF,MAX_PASSWD);
		eeprom_write_block((const void*)&passwd,(void*)ee_passwd,MAX_PASSWD);
		WriteLine(PSTR("Erased root password.\r\n"));
	} if (!strcmp(*args,"pin")) {
		uint8 * ddr = getDDRByName(args[1]);
		if (!strcmp(args[2],"output")) {
			BSET(*ddr,getPinByName(args[1]));
		} else {
			BCLR(*ddr,getPinByName(args[1]));
		}
		sprintf_P(buff,PSTR("Set %s for %s\r\n"), args[1],args[2]);
		WriteRAM(buff);
	}
}

void
AVRShell::cmdAuth(char ** args)
{
	char passwd[MAX_PASSWD];
	eeprom_read_block(passwd,(const void*)ee_passwd,MAX_PASSWD);
	if (passwd[0] == 0xFF) {
		WriteLine(PSTR("No password set. Use \'passwd\'\r\n"));
	} else if (!strcmp(passwd,*args)) {
		bIsAuthorized = 1;
		WriteLine(PSTR("Authorized.\r\n"));
	} else
		WriteLine(PSTR("That password is not correct.\r\n"));
}

void
AVRShell::cmdClock(char ** args)
{
	if (!strcmp(*args,"ckdiv1")) {
		clock_prescale_set(clock_div_1);
	} else if (!strcmp(*args,"ckdiv2")) {
		clock_prescale_set(clock_div_2);
	}else if (!strcmp(*args,"ckdiv4")) {
		clock_prescale_set(clock_div_4);
	}else if (!strcmp(*args,"ckdiv8")) {
		clock_prescale_set(clock_div_8);
	}else if (!strcmp(*args,"ckdiv16")) {
		clock_prescale_set(clock_div_16);
	}else if (!strcmp(*args,"ckdiv32")) {
		clock_prescale_set(clock_div_32);
	}else if (!strcmp(*args,"ckdiv64")) {
		clock_prescale_set(clock_div_64);
	}else if (!strcmp(*args,"ckdiv128")) {
		clock_prescale_set(clock_div_128);
	}else if (!strcmp(*args,"ckdiv256")) {
		clock_prescale_set(clock_div_256);
	}
	WriteLine(PSTR("Successfully changed CPU prescaler.\r\n"));
}

void
AVRShell::cmdTimer(char ** args)
{
	if (!strcmp(*args,"start")) {
		timect = 0;
		BSET(TCCR0B,CS00);
		BSET(TCCR0B,CS02);
		BSET(TCCR0A,WGM01);
		BSET(TIMSK0,OCIE0A);
		OCR0A = 156;
		WriteLine(PSTR("Started timer.\r\n"));
	} else if (!strcmp(*args,"stop")) {
		BCLR(TCCR0B,CS10);
		BCLR(TCCR0B,CS12);
		BCLR(TIMSK0,OCIE0A);
		TCNT0 = 0;
		sprintf_P(buff,PSTR("Elapsed time: ~ %d seconds\r\n"),timeSec);
		WriteRAM(buff);
		timeSec = 0;
	}
}

void
AVRShell::cmdFCPU()
{
	sprintf_P(buff,PSTR("CPU Freq: %lu\r\n"),F_CPU);
	WriteRAM(buff);
}


void
AVRShell::cmdHelp()
{
	out->WriteLine(helpmenu1);
	out->WriteLine(helpmenu2);
	out->WriteLine(helpmenu3);
	out->WriteLine(helpmenu4);
	out->WriteLine(helpmenu4_1);
	out->WriteLine(helpmenu5);
	out->WriteLine(helpmenu6);
	out->WriteLine(helpmenu7);
	out->WriteLine(helpmenu7_5);
	out->WriteLine(helpmenu8);
	out->WriteLine(helpmenu9);
	out->WriteLine(helpmenu10);
	out->WriteLine(helpmenu11);
	out->WriteLine(helpmenu12);
	out->WriteLine(helpmenu13);
	out->WriteLine(helpmenu14);
	out->WriteLine(helpmenu15);
	out->WriteLine(helpmenu16);
	out->WriteLine(helpmenu16_1);
	out->WriteLine(helpmenu17);
	out->WriteLine(helpmenu18);
	out->WriteLine(helpmenu19);
	out->WriteLine(helpmenu20);
	WriteLine(helpmenu21,0);
	WriteLine(endl);
}

void
AVRShell::cmdRead(char ** args)
{
	if (!strcmp(*args,"pin")) 
	{
		uint8 *	port = (uint8*)getPortByName(args[1]);
		if (*port == NUM_PINS) {
			WriteLine(PSTR("Problem finding that pin.\r\n"));
			return;
		}
		if (*port == PORTB)
		{
			if (BCHK(PINB,getPinByName(args[1])))
				sprintf_P(buff,strCmdRead1,args[1]);
			else
				sprintf_P(buff,strCmdRead2,args[1]);
			WriteRAM(buff);
		} else if (*port == PORTC)
		{
			if (BCHK(PINC,getPinByName(args[1])))
				sprintf_P(buff,strCmdRead1,args[1]);
			else
				sprintf_P(buff,strCmdRead2,args[1]);
			WriteRAM(buff);
		} else if (*port == PORTD)
		{
			if (BCHK(PIND,getPinByName(args[1])))
				sprintf_P(buff,strCmdRead1,args[1]);
			else
				sprintf_P(buff,strCmdRead2,args[1]);
			WriteRAM(buff);
		}
	} else if (!strcmp(*args,"lfuse")) {
		char hex[5];
		uint8 lf = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
		sprintf_P(buff,PSTR("Lower Fuse: 0x%s\r\n"),itoa(lf,hex,16));
		WriteRAM(buff);
	} else if (!strcmp(*args,"hfuse")) {
		char hex[5];
		uint8 lf = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
		sprintf_P(buff,PSTR("High Fuse: 0x%s\r\n"),itoa(lf,hex,16));
		WriteRAM(buff);
	} else if (!strcmp(*args,"efuse")) {
		char hex[5];
		uint8 lf = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
		sprintf_P(buff,PSTR("Extended Fuse: 0x%s\r\n"),itoa(lf,hex,16));
		WriteRAM(buff);
	} else if (!strcmp(*args,"lock")) {
		char hex[5];
		uint8 lf = boot_lock_fuse_bits_get(GET_LOCK_BITS);
		sprintf_P(buff,PSTR("Lock Bits: 0x%s\r\n"),itoa(lf,hex,16));
		WriteRAM(buff);
	}
#if 0 
	else if (!strcmp(*args,"calibration")) {
		char hex[5];
		uint8 lf = boot_signature_byte_get(0x0001);
		sprintf_P(buff,PSTR("RC Calibration: 0x%s\r\n"),itoa(lf,hex,16));
		WriteRAM(buff);
	} else if (!strcmp(*args,"signature")) {
		char hex[5];
		uint8 dsb1 = boot_signature_byte_get(0x0000);
		uint8 dsb2 = boot_signature_byte_get(0x0002);
		uint8 dsb3 = boot_signature_byte_get(0x0004);
		sprintf_P(buff,PSTR("Device Signature: 0x%s%s%s\r\n")
			,itoa(dsb1,hex,16),itoa(dsb2,hex,16),itoa(dsb3,hex,16));
		WriteRAM(buff);
	}
#endif
}

void
AVRShell::cmdWrite(char ** args)
{
	if (!strcmp(*args,"pin")) {
		uint8 * port = getPortByName(args[1]);
		if (!strcmp(args[2],"high")) 
			BSET(*port,getPinByName(args[1]));
		else
			BCLR(*port,getPinByName(args[1]));
		sprintf_P(buff,PSTR("Wrote logic %s to pin %s\r\n"),args[2],args[1]);
		WriteRAM(buff);
	}
}

void
AVRShell::cmdEna(char ** args)
{
	if (!strcmp_P(args[0],pmVars)) {
	
	
		sprintf_P(buff,pmEnaSvc,args[0]);
		WriteRAM(buff);
	} else if (!strcmp_P(args[0],pmScript)) {
	
	
		sprintf_P(buff,pmEnaSvc,args[0]);
		WriteRAM(buff);
	}  else
		WriteLine(pmUnkCmd);

}


uint8
AVRShell::ExecCmd(char * c, char ** args)
{
	if ( !strcmp_P(c,pmCat) || !strcmp_P(c,pmEcho) )
		script->parseEcho(args);
	else if (!strcmp_P(c,pmHelp) || !strcmp_P(c,pmMenu))
		cmdHelp();
	else if (!strcmp_P(c,pmEna))
		cmdEna(args);
	else if (!strcmp_P(c,pmRead))
		cmdRead(args);
	else if (!strcmp_P(c,pmWrite))
		cmdWrite(args);
	else if (!strcmp_P(c,pmSet))
		cmdSet(args);
	else if (!strcmp_P(c,pmFCPU))
		cmdFCPU();
	else if (!strcmp_P(c,pmTimer))
		cmdTimer(args);
	else if (!strcmp_P(c,pmAuth))
		cmdAuth(args);
	else if (!strcmp_P(c,pmPasswd))
		cmdPasswd(args);
	else if (!strcmp_P(c,pmPrint))
		script->parsePrint(args);
	else if (c[0] == '$')
		script->parseVar(c,args);
	else if (!strcmp(c,"clock"))
		cmdClock(args);
	else if (!strcmp_P(c,pmPowerup))
		cmdPowerUp(args);
	else if (!strcmp_P(c,pmPowerdown))
		cmdPowerDown(args);
	else if (c[0] == '%') {
		script->parseRegister(c,args);
	} else if (c[0] == ':') {
		script->Interpret(c,args);
	} else {
		sprintf_P(buff,pmUnkCmd,c);
		WriteRAM(buff);
	}
	
	return 0;
}

uint8
AVRShell::Process(const char * b)
{
	// make a quick copy in case the ring buffer gets more data
	char	buff[MAX_TXBUFF];
	memcpy(buff,b,MAX_TXBUFF);
	
	int	idx = 0;
	char *cmd, *ptr, *args[MAX_ARGS];
	if (strtok(buff,WHITE_SPACE) == NULL)
		return 1;
	cmd = buff;
	
	while( (ptr = strtok(NULL,WHITE_SPACE)) != NULL) {
		args[idx] = ptr;
		if (++idx == (MAX_ARGS-1))
			break;
	}
	args[idx] = NULL;
	
	return ExecCmd(cmd,args);
}