
/*
 * Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef SD_RAW_CONFIG_H
#define SD_RAW_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \addtogroup sd_raw
 *
 * @{
 */
/**
 * \file
 * MMC/SD support configuration (license: GPLv2 or LGPLv2.1)
 */

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD write support.
 *
 * Set to 1 to enable MMC/SD write support, set to 0 to disable it.
 */
#define SD_RAW_WRITE_SUPPORT 1

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD write buffering.
 *
 * Set to 1 to buffer write accesses, set to 0 to disable it.
 *
 * \note This option has no effect when SD_RAW_WRITE_SUPPORT is 0.
 */
#define SD_RAW_WRITE_BUFFERING 1

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD access buffering.
 * 
 * Set to 1 to save static RAM, but be aware that you will
 * lose performance.
 *
 * \note When SD_RAW_WRITE_SUPPORT is 1, SD_RAW_SAVE_RAM will
 *       be reset to 0.
 */
#define SD_RAW_SAVE_RAM 1

/**
 * \ingroup sd_raw_config
 * Controls support for SDHC cards.
 *
 * Set to 1 to support so-called SDHC memory cards, i.e. SD
 * cards with more than 2 gigabytes of memory.
 */
/* ORIGINAL 
#define SD_RAW_SDHC 0 */
#define SD_RAW_SDHC 1

/**
 * @}
 */

/* defines for customisation of sd/mmc port access */
#if defined(__AVR_ATmega8__) || \
    defined(__AVR_ATmega48__) || \
    defined(__AVR_ATmega48P__) || \
    defined(__AVR_ATmega88__) || \
    defined(__AVR_ATmega88P__) || \
    defined(__AVR_ATmega168__) || \
    defined(__AVR_ATmega168P__) || \
    defined(__AVR_ATmega328P__)
    #define configure_pin_mosi() DDRB |= (1 << DDB3)
    #define configure_pin_sck() DDRB |= (1 << DDB5)
    #define configure_pin_ss() DDRB |= (1 << DDB2)
    #define configure_pin_miso() DDRB &= ~(1 << DDB4)

    #define select_card() PORTB &= ~(1 << PORTB2)
    #define unselect_card() PORTB |= (1 << PORTB2)
#elif defined(__AVR_ATmega16__) || \
      defined(__AVR_ATmega32__)
    #define configure_pin_mosi() DDRB |= (1 << DDB5)
    #define configure_pin_sck() DDRB |= (1 << DDB7)
    #define configure_pin_ss() DDRB |= (1 << DDB4)
    #define configure_pin_miso() DDRB &= ~(1 << DDB6)

    #define select_card() PORTB &= ~(1 << PORTB4)
    #define unselect_card() PORTB |= (1 << PORTB4)
#elif defined(__AVR_ATmega64__) || \
      defined(__AVR_ATmega128__) || \
      defined(__AVR_ATmega169__)
    #define configure_pin_mosi() DDRB |= (1 << DDB2)
    #define configure_pin_sck() DDRB |= (1 << DDB1)
    #define configure_pin_ss() DDRB |= (1 << DDB0)
    #define configure_pin_miso() DDRB &= ~(1 << DDB3)

    #define select_card() PORTB &= ~(1 << PORTB0)
    #define unselect_card() PORTB |= (1 << PORTB0)

#elif defined(__AVR_ATmega644P__)
	//#warning "check sd/mmc pin mapping!"
	#define configure_pin_mosi() DDRB |= (1 << DDB5)
	#define configure_pin_sck() DDRB |= (1 << DDB7)
	#define configure_pin_ss() DDRC |= (1 << DDC0)
	#define configure_pin_miso() DDRB &= ~(1 << DDB6)

	#define select_card() PORTC &= ~(1 << PORTC7)
	#define unselect_card() PORTC |= (1 << PORTC7)

#elif defined(__AVR_ATmega324P__)
	//#pragma message "check sd/mmc pin mapping ATmega324p!"
	//#warning "check sd/mmc pin mapping ATmega324p!"
	#define configure_pin_mosi() DDRB |= (1 << DDRB5)
	#define configure_pin_miso() DDRB &= ~(1 << DDRB6)
	#define configure_pin_sck() DDRB |= (1 << DDRB7)
	#define configure_pin_ss() DDRC |= (1 << DDRC6)

	#define select_card() PORTC &= ~(1 << PORTC6)
	#define unselect_card() PORTC |= (1 << PORTC6)
	
	#define SPCR SPCR0  
	#define SPR0 SPR00  
	#define SPR1 SPR10  
	#define CPHA CPHA0  
	#define CPOL CPOL0  
	#define MSTR MSTR0  
	#define DORD DORD0 
	#define SPE SPE0   
	#define SPIE SPIE0  

	#define SPSR SPSR0 
	#define SPI2X SPI2X0 
	#define WCOL WCOL0  
	#define SPIF SPIF0  
	#define SPDR SPDR0

#else
    #error "no sd/mmc pin mapping available!"
#endif

/*-----------------------------------------------------------------*/
/* ORIGINALS
#define configure_pin_available() DDRC &= ~(1 << DDC4)
#define configure_pin_locked() DDRC &= ~(1 << DDC5)

#define get_pin_available() (PINC & (1 << PINC4))
#define get_pin_locked() (PINC & (1 << PINC5))
*/

/* Card socket has no Card-Detect and/or Card-Lock switches */
#define configure_pin_available()	/*DDRC &= ~(1 << DDC4)*/
#define configure_pin_locked()		/*DDRC &= ~(1 << DDC5)*/

#define get_pin_available()		0	/*(PINC & (1 << PINC4))*/
#define get_pin_locked()		1	/*(PINC & (1 << PINC5))*/
/*-----------------------------------------------------------------*/

#if SD_RAW_SDHC
    typedef uint64_t offset_t;
#else
    typedef uint32_t offset_t;
#endif

/* configuration checks */
#if SD_RAW_WRITE_SUPPORT
#undef SD_RAW_SAVE_RAM
#define SD_RAW_SAVE_RAM 0
#else
#undef SD_RAW_WRITE_BUFFERING
#define SD_RAW_WRITE_BUFFERING 0
#endif

#ifdef __cplusplus
}
#endif

#endif

