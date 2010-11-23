////////////////////////////////////////////////////////////////////////////
// Name: progmem.cpp
// Author: B. Gian James <gian@BattleDroids.net>
// Description: This file is an effort to fix gcc's broken PROGMEM attribute
//              in C++.  
//
// $Id: progmem.cpp,v 1.3 2009/09/05 04:06:17 gian Exp $
////////////////////////////////////////////////////////////////////////////
#include <avr/pgmspace.h>
#include "progmem.h"

char const 	helpmenu1[] = "                  .oO[ Help Menu ]Oo.\r\n";
char const	helpmenu2[] = ".........................................................................\r\n";
char const	helpmenu3[] = "The following commands are available on the ATmega328P:\r\n";
char const	helpmenu4[] = "help or menu                        This help menu\r\n";
char const	helpmenu4_1[] = "ena [filesystem|vars|script]      Toggle directory services, variables, scripting\r\n";
char const	helpmenu5[] = "write <pin> [high|low]              Write to a digital pin\r\n";
char const	helpmenu6[] = "set   [<pin> [input|output]]        Set a pin for output or input\r\n";
char const	helpmenu7[] = "read [<pin>|lfuse|hfuse|efuse|lock|calibration|signature]\r\n";
char const	helpmenu8[] = "passwd [<passwd>]                   Set a password for root in EEPROM.\r\n";
char const	helpmenu7_5[] = "                                    Read the run-time listed settings\r\n";
char const	helpmenu9[] = "auth <passwd>                       Authorize against the root password\r\n";
char const	helpmenu10[] = "fcpu                                Get the CPU speed.\r\n";
char const	helpmenu11[] = "timer [start|stop]                  Start and stop a real-time timer.\r\n";
char const	helpmenu12[] = "powerup adc|spi|twi|usart|all       Power up the listed interfaces.\r\n";
char const	helpmenu13[] = "powerdown adc|spi|twi|usart|all     Power down the listed interfaces\r\n";
char const	helpmenu14[] = "clock <prescaler>                   Change your CPU speed using a prescaler\r\n";
char const	helpmenu15[] = "\r\n               [ Programming/Scripting Interface ]\r\n";
char const	helpmenu16[] = "cat or echo <%REGISTER>|<$VARIABLE>         Displays the run-time contents of a register or variable\r\n";
char const	helpmenu16_1[] = "%REGISTER[bit] = [1|0]          Sets or clears a bit in a register\r\n";
char const	helpmenu17[] = "<$VARIABLE> = <\"STRING\">             Places the string in EEPROM accesible by <$VARIABLE>\r\n";
char const	helpmenu19[] = "print [registers|variables|tasks]     Prints all known registers, saved variables, and tasks\r\n";
char const 	helpmenu18[] = "Unimplemented features:\r\n";
char const	helpmenu20[] = ":set bit <bit> in <REGISTER>          Scripted way to dynamically set bits at run-time\r\n";
char const 	helpmenu21[] = ":clear bit <bit> in <REGISTER>        Scripted way to dynamicallly clear bits at run-time\r\n";

char const	strCmdRead1[] = "Pin: %s is HIGH\r\n";
char const 	strCmdRead2[] = "Pin: %s is LOW\r\n";

char const	pmUnkCmd[] = "<<ERROR>> %s: unknown command.\r\n";

char const 	endl[] = "\r\n";

// Scripting
char const 	pmHelp[] = "help";
char const	pmMenu[] = "menu";
char const 	pmEna[] = "ena";
char const	pmVars[] = "vars";
char const 	pmScript[] = "script";
char const 	pmEcho[] = "echo";
char const 	pmCat[] = "cat";
char const 	pmRead[] = "read";
char const 	pmWrite[] = "write";
char const 	pmSet[] = "set";
char const 	pmFCPU[] = "fcpu";
char const 	pmTimer[] = "timer";
char const 	pmAuth[] = "auth";
char const 	pmPasswd[] = "passwd";
char const 	pmPrint[] = "print";
char const 	pmPowerup[] = "powerup";
char const 	pmPowerdown[] = "powerdown";
char const 	pmEnaSvc[] = "Service: %s enabled.\r\n";


// PortPins
char const	pmPB0[] = "PB0";
char const	pmPB1[] = "PB1";
char const	pmPB2[] = "PB2";
char const	pmPB3[] = "PB3";
char const	pmPB4[] = "PB4";
char const	pmPB5[] = "PB5";
char const	pmPB6[] = "PB6";
char const	pmPB7[] = "PB7";
char const	pmPC0[] = "PC0";
char const	pmPC1[] = "PC1";
char const	pmPC2[] = "PC2";
char const	pmPC3[] = "PC3";
char const	pmPC4[] = "PC4";
char const	pmPC5[] = "PC5";
char const	pmPC6[] = "PC6";
char const	pmPD0[] = "PD0";
char const	pmPD1[] = "PD1";
char const	pmPD2[] = "PD2";
char const	pmPD3[] = "PD3";
char const	pmPD4[] = "PD4";
char const	pmPD5[] = "PD5";
char const	pmPD6[] = "PD6";
char const	pmPD7[] = "PD7";

// Registers
char const	pmPORTB[] = "PORTB";
char const	pmPORTC[] ="PORTC" ;
char const 	pmPORTD[] = "PORTD";
char const 	pmDDRD[] = "DDRD";
char const 	pmDDRB[] ="DDRB" ;
char const 	pmDDRC[] = "DDRC";
char const	pmPINB[] = "PINB";
char const pmPINC[] = "PINC";
char const 	pmPIND[] = "PIND";
char const 	pmSREG[] = "SREG";
char const	pmGPIOR0[] = "GPIOR0";
char const 	pmGPIOR1[] = "GPIOR1";
char const 	pmGPIOR2[] = "GPIOR2";
char const 	pmTCCR0B[] = "TCCR0B";
char const 	pmOCR0A[] =  "OCR0A";
char const	pmSPCR[] = "SPCR";
char const	pmACSR[] = "ACSR";
char const	pmMCUSR[] = "MCUSR";
char const	pmSPMCSR[] = "SPMCSR";
char const	pmCLKPR[] = "CLKPR";
char const	pmOSCCAL[] = "OSCCAL";
char const	pmEICRA[] = "EICRA";
char const	pmPCMSK1[] = "PCMSK1";
char const	pmTIMSK1[] = "TIMSK1";
char const	pmADCL[] = "ADCL";
char const	pmADCSRA[] = "ADCSRA";
char const	pmADMUX[] = "ADMUX";
char const	pmDIDR1[] = "DIDR1";
char const	pmTCCR1B[] = "TCCR1B";
char const	pmTCNT1L[] = "TCNT1L";
char const	pmTCNT1H[] = "TCNT1H";
char const	pmICR1L[] = "ICR1L";
char const	pmOCR1AL[] = "OCR1AL";
char const	pmOCR1AH[] = "OCR1AH";
char const	pmOCR1BL[] = "OCR1BL";
char const	pmTCCR2A[] = "TCCR2A";
char const	pmTCNT2[] = "TCNT2";
char const	pmOCR2B[] = "OCR2B";
char const	pmTWBR[] = "TWBR";
char const	pmTWAR[] = "TWAR";
char const	pmTWCR[] = "TWCR";
char const	pmUCSR0A[] = "UCSR0A";
char const	pmUCSR0C[] = "UCSR0C";
char const	pmUBRR0L[] = "UBRR0L";
char const	pmUDR0[] = "UDR0";
char const	pmTIFR0[] = "TIFR0";
char const	pmTIFR1[] = "TIFR1";
char const	pmTIFR2[] = "TIFR2";
char const	pmPCIFR[] = "PCIFR";
char const	pmEIFR[] = "EIFR";
char const	pmEIMSK[] = "EIMSK";
char const	pmEECR[] = "EECR";
char const	pmEEDR[] = "EEDR";
char const	pmEEARL[] = "EEARL";
char const	pmEEARH[] = "EEARH";
char const	pmGTCCR[] = "GTCCR";
char const	pmTCCR0A[] = "TCCR0A";
char const	pmTCNT0[] = "TCNT0";
char const	pmOCR0B[] = "OCR0B";
char const	pmSPDR[] = "SPDR";
char const	pmSMCR[] = "SMCR";
char const	pmMCUCR[] = "MCUCR";
char const	pmWDTCSR[] = "WDTCSR";
char const	pmPRR[] = "PRR";
char const	pmPCICR[] ="PCICR" ;
char const	pmPCMSK0[] = "PCMSK0";
char const	pmTIMSK0[] = "TIMSK0";
char const	pmTIMSK2[] = "TIMSK2";
char const	pmADCH[] = "ADCH";
char const	pmADCSRB[] = "ADCSRB";
char const	pmDIDR0[] = "DIDR0";
char const	pmTCCR1A[] = "TCCR1A";
char const	pmTCCR1C[] = "TCCR1C";
char const	pmICR1H[] = "ICR1H";
char const	pmOCR1BH[] ="OCR1BH" ;
char const	pmTCCR2B[] = "TCCR2B";
char const	pmOCR2A[] = "OCR2A";
char const	pmASSR[] = "ASSR";
char const	pmTWSR[] = "TWSR";
char const	pmTWDR[] = "TWDR";
char const	pmTWAMR[] = "TWAMR";
char const	pmUCSR0B[] = "UCSR0B";
char const	pmUBRR0H[] = "UBRR0H";


#ifdef _AVRSH_FILESYSTEM_H_
// DirectoryService
char const	pmRoot[] = "/root";
char const	pmRegisters[] = "/registers";
char const	pmPins[] = "/pins";
char const	pmPeripherals[] = "/peripherals";
char const	pmUsers[] = "/users";
char const	pmDirHead[] =   "%-20s  %-6s  %-8s  %-10s  %-5s  %-40s\r\n";
char const	pmDirFormat[] = "%-20s    %-6s   %-8d 0x%-10x %-5d  %-40s\r\n";
char const	pmNULL[] = "NULL";
#endif

