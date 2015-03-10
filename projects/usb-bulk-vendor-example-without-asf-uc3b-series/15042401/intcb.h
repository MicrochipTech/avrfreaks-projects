/*
*
*
* 	tcj 3-2-2015
*
*	intcb.h
*
*	15030501
*
*	Header file to support interrupts in Atmel AV32UC3B parts (AV32UC3B ONLY!!!!)
*	using the Atmel AVR32 gcc toolchain without the Atmel Software Framework
*
* 	
*	 Copyright 2015 Tom C. Jewell
*
*	 This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.*
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.****
*
*
*
*	Must be compiled using -std=gnu99 compiler option.  This is mandatory, as I have declared variables in the function body.
*
*
*
*
*	For one or more interrupts, of the 19 (numbered 0 to 18) in the datasheet, Main file must contain:
*	
*	1. At top, outside main(), #defines, one per line, for each interrupt used: e.g. #define INTCB5 or #define INTCB12
*
*	2. Below the defines (but also at top outside main()): #include "intcb.h"    (this file)
*
*	3. In file, outside main(), ISR functions, named specifically thus, without prototypes:
*
*		void intcb5(void)
*		{
*			[body of function]
*		}	
*
*		There should be no 'return' or 'return from interrupt'
*
*		The user is responsible for determining, if necessary, which line of the particular interrupt
*		group has triggered, and for clearing the interrupt.
*
*		The ISR function can call other functions, but as it
*		is an interrupt, it should be kept as brief in time as possible
*
*	4. In function main(), setup_intcb() should be called near the top (setup_intcb is in this header file)
*
*	5. In main() or other functions, call as needed the macros START_GLOB_INTS and STOP_GLOB_INTS, which do what they say
*
*	6. The process of setting up each peripheral used should include selecting the interrupt priority level
*	   (0 to 3, 0 is default) and most importantly, enabling the individual interrupt for the peripheral
*
*
*/

#define START_GLOB_INTS 	asm("CSRF 16"); asm("nop"); asm("nop");
#define STOP_GLOB_INTS 		asm("SSRF 16"); asm("nop"); asm("nop");



void setup_intcb(void)
{
	uint32_t interrupt_area;
	uint32_t evba_base;
	
//------  move EVBA table and set up interrupt

	static uint8_t area_block[730] __attribute__((aligned(0x400))) = {0};	// grab 730 bytes of memory, aligned so it ends in 10 zeroes (binary)
																			// static because a) this area can't be allowed to evaporate and
																			// b) a bug in gcc keeps alignment from working because it puts array on stack unless static

	interrupt_area = (uint32_t)(&area_block);	// turn the address of area_block into a number so it can be indexed

			// the last item in the table is at 0x100 (relative to EVBA), so...
			// now just vacuum up a few more than 256 bytes starting at __builtin_mfsr(AVR32_EVBA)
			// (which by the by appears always to be at 0, the bottom) and put them in the new area

	evba_base = __builtin_mfsr(AVR32_EVBA);			// find present EVBA so we can move it

	for(uint16_t emove_counter = 0; emove_counter < 272; emove_counter++)  *(volatile uint8_t *) (interrupt_area + emove_counter) =		// read byte from old EVBA
									 	*(volatile uint8_t *) (evba_base + emove_counter);	// and stuff in new EVBA

 	__builtin_mtsr(AVR32_EVBA, (uint32_t)interrupt_area);  // tell the EVBA register where the new table is



//------  set up autovectors from 0 to 18

	for(uint8_t i = 0; i <19; i++)	AVR32_INTC.IPR[i].autovector = 272 + 24*i;



//------  make pointers to addresses of intcb0 through intcb18, then put each pointer where respective jump routine can find it, then put jump routine where it belongs

	#ifdef INTCB0
		void intcb0(void);
		void (*intcb0Ptr)(void) = intcb0;	
		*(volatile uint32_t *) (interrupt_area + 292) =  (uint32_t)intcb0Ptr;
		uint8_t c0_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0x24, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 272 + j) = c0_array[j];	
	#endif

	#ifdef INTCB1
		void intcb1(void);
		void (*intcb1Ptr)(void) = intcb1;
		*(volatile uint32_t *) (interrupt_area + 316) =  (uint32_t)intcb1Ptr;
		uint8_t c1_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0x3c, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 296 + j) = c1_array[j];	
	#endif

	#ifdef INTCB2
		void intcb2(void);
		void (*intcb2Ptr)(void) = intcb2;
		*(volatile uint32_t *) (interrupt_area + 340) =  (uint32_t)intcb2Ptr;
		uint8_t c2_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0x54, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 320 + j) = c2_array[j];	
	#endif

	#ifdef INTCB3
		void intcb3(void);
		void (*intcb3Ptr)(void) = intcb3;
		*(volatile uint32_t *) (interrupt_area + 364) =  (uint32_t)intcb3Ptr;
		uint8_t c3_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0x6c, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 344 + j) = c3_array[j];			
	#endif

	#ifdef INTCB4
		void intcb4(void);
		void (*intcb4Ptr)(void) = intcb4;
		*(volatile uint32_t *) (interrupt_area + 388) =  (uint32_t)intcb4Ptr;
		uint8_t c4_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0x84, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 368 + j) = c4_array[j];			
	#endif

	#ifdef INTCB5
		void intcb5(void);
		void (*intcb5Ptr)(void) = intcb5;
		*(volatile uint32_t *) (interrupt_area + 412) =  (uint32_t)intcb5Ptr;
		uint8_t c5_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0x9c, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 392 + j) = c5_array[j];			
	#endif

	#ifdef INTCB6
		void intcb6(void);
		void (*intcb6Ptr)(void) = intcb6;
		*(volatile uint32_t *) (interrupt_area + 436) =  (uint32_t)intcb6Ptr;
		uint8_t c6_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0xb4, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 416 + j) = c6_array[j];			
	#endif

	#ifdef INTCB7
		void intcb7(void);
		void (*intcb7Ptr)(void) = intcb7;
		*(volatile uint32_t *) (interrupt_area + 460) =  (uint32_t)intcb7Ptr;
		uint8_t c7_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0xcc, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 440 + j) = c7_array[j];			
	#endif

	#ifdef INTCB8
		void intcb8(void);
		void (*intcb8Ptr)(void) = intcb8;
		*(volatile uint32_t *) (interrupt_area + 484) =  (uint32_t)intcb8Ptr;
		uint8_t c8_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0xe4, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 464 + j) = c8_array[j];			
	#endif

	#ifdef INTCB9
		void intcb9(void);
		void (*intcb9Ptr)(void) = intcb9;
		*(volatile uint32_t *) (interrupt_area + 508) =  (uint32_t)intcb9Ptr;
		uint8_t c9_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x01, 0xfc, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 488 + j) = c9_array[j];			
	#endif

	#ifdef INTCB10
		void intcb10(void);
		void (*intcb10Ptr)(void) = intcb10;
		*(volatile uint32_t *) (interrupt_area + 532) =  (uint32_t)intcb10Ptr;
		uint8_t c10_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0x14, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 512 + j) = c10_array[j];			
	#endif

	#ifdef INTCB11
		void intcb11(void);
		void (*intcb11Ptr)(void) = intcb11;
		*(volatile uint32_t *) (interrupt_area + 556) =  (uint32_t)intcb11Ptr;
		uint8_t c11_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0x2c, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 536 + j) = c11_array[j];			
	#endif

	#ifdef INTCB12
		void intcb12(void);
		void (*intcb12Ptr)(void) = intcb12;
		*(volatile uint32_t *) (interrupt_area + 580) =  (uint32_t)intcb12Ptr;
		uint8_t c12_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0x44, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 560 + j) = c12_array[j];			
	#endif

	#ifdef INTCB13
		void intcb13(void);
		void (*intcb13Ptr)(void) = intcb13;
		*(volatile uint32_t *) (interrupt_area + 604) =  (uint32_t)intcb13Ptr;
		uint8_t c13_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0x5c, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 584 + j) = c13_array[j];			
	#endif

	#ifdef INTCB14
		void intcb14(void);
		void (*intcb14Ptr)(void) = intcb14;
		*(volatile uint32_t *) (interrupt_area + 628) =  (uint32_t)intcb14Ptr;
		uint8_t c14_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0x74, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 608 + j) = c14_array[j];			
	#endif

	#ifdef INTCB15
		void intcb15(void);
		void (*intcb15Ptr)(void) = intcb15;
		*(volatile uint32_t *) (interrupt_area + 652) =  (uint32_t)intcb15Ptr;
		uint8_t c15_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0x8c, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 632 + j) = c15_array[j];			
	#endif

	#ifdef INTCB16
		void intcb16(void);
		void (*intcb16Ptr)(void) = intcb16;
		*(volatile uint32_t *) (interrupt_area + 676) =  (uint32_t)intcb16Ptr;
		uint8_t c16_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0xa4, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 656 + j) = c16_array[j];			
	#endif

	#ifdef INTCB17
		void intcb17(void);
		void (*intcb17Ptr)(void) = intcb17;
		*(volatile uint32_t *) (interrupt_area + 700) =  (uint32_t)intcb17Ptr;
		uint8_t c17_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0xbc, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 680 + j) = c17_array[j];			
	#endif

	#ifdef INTCB18
		void intcb18(void);
		void (*intcb18Ptr)(void) = intcb18;
		*(volatile uint32_t *) (interrupt_area + 724) =  (uint32_t)intcb18Ptr;
		uint8_t c18_array[16]  =	{0xE1, 0xB8, 0x00, 0x01, 0xE0, 0x69, 0x02, 0xd4, 0x12, 0x08, 0x11, 0x09, 0x5D, 0x19, 0xD6, 0x03}; // the machine code ISR
		for(uint8_t j = 0; j < 16; j++)  *(volatile uint8_t *) (interrupt_area + 704 + j) = c18_array[j];			
	#endif

}


