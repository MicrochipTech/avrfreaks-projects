/*
 * eXmega_sdram_driver.hpp
 *
 * Version: 1.01
 *
 *  Device:	Xmega-A1 Xplained
 *		 CPU: ATXmega128A1
 *
 * Created: 15. 1. 2013 1:00:26
 * Updated: 18. 2. 2013
 */ 


#ifndef _eXmega_sdram_driver_hpp_
#define _eXmega_sdram_driver_hpp_

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Address = max. 24 bits address
//					 SRAM or SDRAM (not FLASH !!!)
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class eSDRAM_Def
{
	public:
		unsigned long BaseAddress;			// DO NOT CHANGE !
		unsigned long SizeInBytes;			// 8 MB (DO NOT CHANGE)
		
		eSDRAM_Def();
					
		void Init();
					
		unsigned char Read8( const unsigned long Address );
		unsigned short Read16( const unsigned long Address );
		unsigned long Read32( const unsigned long Address );
					
		void Write8( const unsigned long Address, unsigned char Value );
		void Write16( const unsigned long Address, unsigned short Value );
		void Write32( const unsigned long Address, unsigned long Value );

		void ReadBlock( const unsigned long Source_Address, const unsigned char *SRAM_Target_Data, const unsigned short Size );								
		void WriteBlock( const unsigned long Target_Address, const unsigned char *SRAM_Source_Data, const unsigned short Size );
		void FillBlock( const unsigned long Address, const unsigned long Size, const unsigned char FillByte );
		
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
eSDRAM_Def::eSDRAM_Def()
{
	BaseAddress = 0x800000;			// DO NOT CHANGE !
	SizeInBytes = 0x800000;			// 8 MB (DO NOT CHANGE)
}	

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::Init()
{
	// SETUP EBI PORT DIRECTIONS
	PORTJ.DIR = 0xFF;
	PORTK.DIR = 0xFF;
	PORTH.DIR = 0xFF;
	PORTH.OUT = 0xFF;							// [0] !WE, [1] !CAS, [2] !RAS, [3] !DQM = MALI BY MAT PULL-UP (na vystupe neviem ako :-) )
	
	// EBI SDRAM INIT
	EBI.CTRL = 0x00 |							// [7..6] 00 = 4 bits Data
						 0x0C |							// [3..2] 11 = NOALE = no address multiplexing
						 0x01;							// [1..0] 01 = EBI enabled, IFMODE: 01 = 3 PORTS
	// S CL = 2 NECHCE IST DOBRE (HORNE ADRESY NESTIHA)
	EBI.SDRAMCTRLA = (1 << 2) |		// [3] SDCAS = SDRAM CAS latency (0 = 2 CLK, 1 = 3 CLK)
									 (1 << 1) |		// [2] SDROW = SDRAM ROW bits (0 = 11 bits, 1 = 12 bits)
									 0x02;				// [1..0] SDCOL = SDRAM COLUMN bits (00 = 8 bits, 01 = 9 bits, 10 = 10 bits, 11 = 11 bits)
	/* BACKUP: 13. 2. 2013
	EBI.SDRAMCTRLB = 0x80 |				// [7..6] MRDLY = SDRAM MODE DELAY (10 = 2 clks)
									 0x08 |				// [5..3] ROWCYCDLY = SDRAM ROW CYCLE DELAY (001 = 1 clks)
									 0x03;				// [2..0] RPDLY = SDRAM ROW TO PRECHARGE DELAY (011 = 3 clks)
	EBI.SDRAMCTRLC = 0x80 |				// [7..6] WRDLY = SDRAM WRITE RECOVERYDELAY (10 = 2 clks)
									 0x28 |				// [5..3] ESRDLY = SDRAM EXIT SELF-REFRESH DELAY (101 = 5 clks)
									 0x01;				// [2..0] ROWCOLDLY = SDRAM ROW TO COLUMN DELAY (001 = 1 clks)
	*/
	EBI.SDRAMCTRLB = 0x80 |				// [7..6] MRDLY = SDRAM MODE DELAY (10 = 2 clks)
									 0x38 |				// [5..3] ROWCYCDLY = SDRAM ROW CYCLE DELAY (111 = 7 clks)
									 0x07;				// [2..0] RPDLY = SDRAM ROW TO PRECHARGE DELAY (111 = 7 clks)
	EBI.SDRAMCTRLC = 0x40 |				// [7..6] WRDLY = SDRAM WRITE RECOVERYDELAY (01 = 1 clks)
									 0x38 |				// [5..3] ESRDLY = SDRAM EXIT SELF-REFRESH DELAY (111 = 7 clks)
									 0x07;				// [2..0] ROWCOLDLY = SDRAM ROW TO COLUMN DELAY (111 = 7 clks)
	// EBI.REFRESH = 1000;						// [9..0] REFRESH = SDRAM REFRESH period : 64000000 / 1000000 = 64 ClkPer2 na 1 us => 1000 / 64 = 15.625 us
	EBI.REFRESH = 960;						// [9..0] REFRESH = SDRAM REFRESH period : 64000000 / 1000000 = 64 ClkPer2 na 1 us => 960 / 64 = 15 us (max. 15.625 us - najdlhsi mozny cas)
	// EBI.INITDLY = 6400;						// [13..0] INITDLY = SDRAM initialization delay : 64000000 / 1000000 = 64 ClkPer2 na 1 us * 100 => 6400 = 100 us
	EBI.INITDLY = 7680;						// [13..0] INITDLY = SDRAM initialization delay : 64000000 / 1000000 = 64 ClkPer2 na 1 us * 120 => 7680 = 120 us
	EBI.CS3.BASEADDR = (0x800000UL >> 8) & 0xFFF0;
	EBI.CS3.CTRLA = 0x3C |				// [6..2] ASIZE = SDRAM ADDRESS SIZE IN BYTES (01111 = 8 MB)
									0x03;					// [1..0] MODE = SDRAM CHIP SELECT MODE (11 = SDRAM)
	EBI.CS3.CTRLB = 0x00;					// [2] SDRAM SELF-REFRESH ENABLE (0 - disabled)
	// [1..0] SDRAM MODE = (0 = Normal Mode)
	while ((EBI.CS3.CTRLB & 0x80) != 0x80);			// Cakaj kym sa inicializuje
	
	// CAS LATENCY = 3 CLK
	// MODE DELAY = 2 CLK
	// ROW CYSLE DELAY = 7 CLK
	// ROW TO PRECHARGE DELAY = 7 CLK
	// WRITE RECOVERY DELAY = 1 CLK
	// SELF REFRESH TO ACTIVE DELAY = 7 CLK
	// ROW TO COL DELAY = 7 CLK
}	

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::ReadBlock( const unsigned long Source_Address, const unsigned char *SRAM_Target_Data, const unsigned short Size )
{
	if (Size == 0) { return; }
		
	asm
	(
		"push r30 \n\t"								// zalohuj Z
		"push r31 \n\t"
		"push r26 \n\t"								// zalohuj X
		"push r27 \n\t"
		"push r25 \n\t"								// zalohuj r25
		
		"in r25, %1 \n\t"							// zalohuj RAMPZ
		"push r25 \n\t"
		
		"in r25, %4 \n\t"							// zalohuj RAMPX
		"push r25 \n\t"
		
		"push r24 \n\t"								// zalohuj r24
		"push r23 \n\t"								// zalohuj r23
		
		// LOAD SIZE
		"mov r24, %B3 \n\t"						// Size (High byte)
		"mov r23, %A3 \n\t"						// Size (Low byte)
		
		// LOAD SOURCE ADDRESS
		"movw r30, %A0 \n\t"					// Z (word) = Low 16 bits of Address (24 bits)
		"out %1, %C0 \n\t"						// RAMPZ (byte) = High 8 bits of Address (24 bits)
		
		// LOAD TARGET ADDRESS
		"movw r26, %A2 \n\t"					// X (word) = SRAM pointer
		"clr r25 \n\t"
		"out %4, r25 \n\t"						// RAMPX (byte) = High 8 bits of address (24 bits) 0x002000 (start SRAM ?)
		
		// COPY
		"LOOP_eSDRAM_ReadBlock_%=: \n\t"
		
		"ld r25, Z+ \n\t"							// r25 = (Z)+
		"st X+, r25 \n\t"							// (X)+ = r25
		
		// DEC SIZE, LOOP
		"subi r23, 1 \n\t"						// substract with carry by 1
		"sbci r24, 0 \n\t"						// substract with carrt by 0
		"or r23, r23 \n\t"						// is r23 == 0 ?
		"brne LOOP_eSDRAM_ReadBlock_%= \n\t"
		"or r24, r24 \n\t"						// is r24 == 0 ?
		"brne LOOP_eSDRAM_ReadBlock_%= \n\t"
		
		"pop r23 \n\t"								// obnov r23
		"pop r24 \n\t"								// obnov r24
		
		"pop r25 \n\t"
		"out %4, r25 \n\t"						// Obnov RAMPX
		
		"pop r25 \n\t"								// Obnov RAMPZ
		"out %1, r25 \n\t"
		
		"pop r25 \n\t"								// obnov r25
		"pop r27 \n\t"
		"pop r26 \n\t"								// obnov X
		"pop r31 \n\t"
		"pop r30 \n\t"								// obnov Z
		:
		: "r"(Source_Address), "i"(&RAMPZ), "r"(SRAM_Target_Data), "r"(Size), "i"(&RAMPX)	
	);
	
}	

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::WriteBlock( const unsigned long Target_Address, const unsigned char *SRAM_Source_Data, const unsigned short Size )
{
	if (Size == 0) { return; }
		
	asm
	(
		"push r30 \n\t"								// zalohuj Z
		"push r31 \n\t"
		"push r26 \n\t"								// zalohuj X
		"push r27 \n\t"
		"push r25 \n\t"								// zalohuj r25
		
		"in r25, %1 \n\t"							// zalohuj RAMPZ
		"push r25 \n\t"
		
		"in r25, %4 \n\t"							// zalohuj RAMPX
		"push r25 \n\t"
		
		"push r24 \n\t"								// zalohuj r24
		"push r23 \n\t"								// zalohuj r23
		
		// LOAD SIZE
		"mov r24, %B3 \n\t"						// Size (High byte)
		"mov r23, %A3 \n\t"						// Size (Low byte)
		
		// LOAD SOURCE ADDRESS
		"movw r30, %A0 \n\t"					// Z (word) = Low 16 bits of Address (24 bits)
		"out %1, %C0 \n\t"						// RAMPZ (byte) = High 8 bits of Address (24 bits)
		
		// LOAD TARGET ADDRESS
		"movw r26, %A2 \n\t"					// X (word) = SRAM pointer
		"clr r25 \n\t"
		"out %4, r25 \n\t"						// RAMPX (byte) = High 8 bits of address (24 bits) 0x002000 (start SRAM ?)
		
		// COPY
		"LOOP_eSDRAM_WriteBlock_%=: \n\t"
		
		"ld r25, X+ \n\t"							// r25 = (X)+
		"st Z+, r25 \n\t"							// (Z)+ = r25
		
		// DEC SIZE, LOOP
		"subi r23, 1 \n\t"						// substract with carry by 1
		"sbci r24, 0 \n\t"						// substract with carrt by 0
		"or r23, r23 \n\t"						// is r23 == 0 ?
		"brne LOOP_eSDRAM_WriteBlock_%= \n\t"
		"or r24, r24 \n\t"						// is r24 == 0 ?
		"brne LOOP_eSDRAM_WriteBlock_%= \n\t"
		
		"pop r23 \n\t"								// obnov r23
		"pop r24 \n\t"								// obnov r24
		
		"pop r25 \n\t"
		"out %4, r25 \n\t"						// Obnov RAMPX
		
		"pop r25 \n\t"								// Obnov RAMPZ
		"out %1, r25 \n\t"
		
		"pop r25 \n\t"								// obnov r25
		"pop r27 \n\t"
		"pop r26 \n\t"								// obnov X
		"pop r31 \n\t"
		"pop r30 \n\t"								// obnov Z
		:
		: "r"(Target_Address), "i"(&RAMPZ), "r"(SRAM_Source_Data), "r"(Size), "i"(&RAMPX)	
	);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::FillBlock( const unsigned long Address, const unsigned long Size, const unsigned char FillByte )
{
	asm volatile
	(
		"push r25 \n"
		
		"push r20 \n"
		"push r21 \n"
		"push r22 \n"
		
		"push r30 \n"
		"push r31 \n"
		
		"in r25, %3 \n"					// push RAMPZ
		"push r25 \n"
		
		// Load Address
		"movw r30, %A0 \n"			// Z = Address.Word0
		"mov r25, %C0 \n"
		"out %3, r25 \n"				// RAMPZ = Address.Byte2
		
		// Load Size
		"mov r20, %A1 \n"
		"mov r21, %B1 \n"
		"mov r22, %C1 \n"
		
		// Load FillByte
		"mov r25, %2 \n"				// FillByte
		
		"LOOP_eSDRAM_FillBlock_%=: \n"
		
		// Fill
		"st Z+, r25 \n"
		
		// Size--
		"subi r20, 1 \n"
		"sbci r21, 0 \n"
		"sbci r22, 0 \n"
		
		// Compare: Count == 0
		"or r20, r20 \n"
		"brne LOOP_eSDRAM_FillBlock_%= \n"
		"or r21, r21 \n"
		"brne LOOP_eSDRAM_FillBlock_%= \n"
		"or r22, r22 \n"
		"brne LOOP_eSDRAM_FillBlock_%= \n"
		
		"pop r25 \n"
		"out %3, r25 \n"				// pop RAMPZ
		
		"pop r31 \n"
		"pop r30 \n"						// pop Z
		
		"pop r22 \n"
		"pop r21 \n"
		"pop r20 \n"
		
		"pop r25 \n"
		
		:
		: "l"(Address),
			"l"(Size),
			"l"(FillByte),
			"i"(&RAMPZ)
	);
}	

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned char eSDRAM_Def::Read8( const unsigned long Address )
{
	unsigned char Value;

	asm	volatile
	(
		"movw r30, %A1 \n\t"
		"out %2, %C1 \n\t"
		"ld %0, Z \n\t"
		"out %2, __zero_reg__ \n\t"
		: "=r"(Value)
		: "r"(Address), "i"(&RAMPZ)
		: "r30", "r31"
	);

	return( Value );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned short eSDRAM_Def::Read16( const unsigned long Address )
{
	unsigned short Value;

	asm	volatile
	(
		"movw r30, %A1 \n\t"
		"out %2, %C1 \n\t"
		"ld %A0, Z+ \n\t"
		"ld %B0, Z \n\t"
		"out %2, __zero_reg__ \n\t"
		: "=r"(Value)
		: "r"(Address), "i"(&RAMPZ)
		: "r30", "r31"
	);

	return( Value );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned long eSDRAM_Def::Read32( const unsigned long Address )
{
	unsigned long Value;

	asm	volatile
	(
		"movw r30, %A1 \n\t"
		"out %2, %C1 \n\t"
		"ld %A0, Z+ \n\t"
		"ld %B0, Z+ \n\t"
		"ld %C0, Z+ \n\t"
		"ld %D0, Z \n\t"
		"out %2, __zero_reg__ \n\t"
		: "=r"(Value)
		: "r"(Address), "i"(&RAMPZ)
		: "r30", "r31"
	);

	return( Value );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::Write8( const unsigned long Address, unsigned char Value )
{
	asm	volatile
	(
		"movw r30, %A0 \n\t"
		"out %2, %C0 \n\t"
		"st Z, %1 \n\t"
		"out %2, __zero_reg__ \n\t"
		:
		: "r"(Address), "r"(Value), "i"(&RAMPZ)
		: "r30", "r31"
	);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::Write16( const unsigned long Address, unsigned short Value )
{
	asm	volatile
	(
		"movw r30, %A0 \n\t"
		"out %2, %C0 \n\t"
		"st Z+, %A1 \n\t"
		"st Z, %B1 \n\t"
		"out %2, __zero_reg__ \n\t"
		:
		: "r"(Address), "r"(Value), "i"(&RAMPZ)
		: "r30", "r31"
	);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eSDRAM_Def::Write32( const unsigned long Address, unsigned long Value )
{
	asm	volatile
	(
		"movw r30, %A0 \n\t"
		"out %2, %C0 \n\t"
		"st Z+, %A1 \n\t"
		"st Z+, %B1 \n\t"
		"st Z+, %C1 \n\t"
		"st Z, %D1 \n\t"
		"out %2, __zero_reg__ \n\t"
		:
		: "r"(Address), "r"(Value), "i"(&RAMPZ)
		: "r30", "r31"
	);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



#endif