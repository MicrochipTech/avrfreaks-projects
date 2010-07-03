/**
   @author ErikDT,vektor dewanto
   @ver Jan 10
*/
/*
  CodeVisionAVR C Compiler
  (C) 1998-2006 Pavel Haiduc, HP InfoTech S.R.L.

  stdint.h standard header
*/
typedef signed char int8_t;
typedef signed char int8_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;
// An integer type large enough to be able to hold a pointer to RAM.
typedef short intptr_t;
typedef unsigned short uintptr_t;
// An integer type large enough to be able to hold a pointer to FLASH.
typedef short flash_intptr_t;
typedef unsigned short flash_uintptr_t;
// An integer type large enough to be able to hold a pointer to EEPROM.
typedef short eeprom_intptr_t;
typedef unsigned short eeprom_uintptr_t;
/*
  CodeVisionAVR C Compiler
  (C) 1998-2007 Pavel Haiduc, HP InfoTech S.R.L.

  stdbool.h standard header
*/
typedef struct
typedef struct
_Packet
    {
        uint8_t checksum;
        uint8_t length; /*!< Payload length. */
        uint8_t payload [14];
    }
Packet;
#pragma used+
	unsigned char packet_check (Packet* packet);
	unsigned char packet_compute_header (Packet* packet, uint8_t length);
	unsigned char packet_send (Packet* packet);
	unsigned char packet_receive (Packet* packet);
#pragma used-
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega8
#pragma used+
#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      // 16 bit access
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   // 16 bit access
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrw ICR1=0x26;   // 16 bit access
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  // 16 bit access
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  // 16 bit access
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  // 16 bit access
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-
// Interrupt vectors definitions
// Needed by the power management functions (sleep.h)
#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm
unsigned char packet_check (Packet* packet)
    {
        uint8_t i;
        uint8_t sum = 0;
        uint8_t len = packet->length;
        if (len > 14)
            {
                return 0;
            }
        for (i = 0; i < len + 2; ++i)
            {
                sum += ((uint8_t*) packet)[i];
            }
        return sum == 0;
    }
unsigned char packet_compute_header (Packet* packet, uint8_t length)
    {
        uint8_t sum = length;
        uint8_t i;
        if (length > 14)
            {
                return 0;
            }
        packet->length = length;
        for (i = 0; i < length; ++i)
            {
                sum += packet->payload [i];
            }
        packet->checksum = -sum;
        return 1;
    }
unsigned char packet_send (Packet* packet)
    {
        if (packet_check (packet))
            {
                uint8_t num = packet->length + 2;
                uint8_t i;
                for (i = 0; i < num; ++i)
                    {
                        putchar (((uint8_t*)packet)[i]);//depends on USARTn used
                    }
                return 1;
            }
        return 0;
    }
/*!
 *  \param  length  Payload length.
 */
unsigned char packet_receive (Packet* packet)
    {
        uint8_t i;
        packet->checksum = getchar (); //depends on USARTn used
        packet->length = getchar (); //depends on USARTn used
        if (packet->length > 14)
            {
                return 0;
            }
        for (i = 0; i < packet->length; ++i)
            {
                packet->payload[i] = getchar ();//depends on USARTn used
            }
        return packet_check (packet);
    }
