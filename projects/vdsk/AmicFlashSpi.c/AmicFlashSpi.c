/**
 *
 * AmicFlashSpi.c is part of the FIGnition firmware.
 *
 * The FIGnition firmware is the built-in software for the
 * FIGnition DIY 8-bit computer and compatible computers.
 *
 * Copyright (C) 2011  Julian Skidmore.
 *
 * The FIGnition firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Version. Date (DD/MM/YYY)
 * ************************
 *
 * 1.0.0.  01/07/2011. Released as part of the FIGnition VDsk Flash Driver.
 *
 * Contact
 * *******
 * TheOriginalSnial@Gmail.com
 *
 *
 * Introduction:
 * *************
 *
 */


//#define __TESTSPIDEBUG__

#ifdef __TESTSPIDEBUG__

void _SpiDebug(byte data)
{
	Emit('#');
	DotHex(data);
	Emit(' ');
}

#else

#define _SpiDebug(data)

#endif

#define __TESTSPI__

#define kSpiMosiBit 3
#define kSpiMosi (1<<kSpiMosiBit)
#define kSpiSck (1<<5)
#define kSpiMisoBit 4
#define kSpiMiso (1<<kSpiMisoBit)
#define kSpiCsSram (1<<1)
#define kSpiCsFlash (1<<2)


#ifdef __TESTSPI__

ushort gSeed=0;
ushort Rnd(ushort max)
{
	gSeed=(gSeed+1)*75; // next rnd.
	return (ushort)(((ulong)max*gSeed)>>16);
}

#define __AUTOSPI__

//byte gSpiResVal;

#ifdef __AUTOSPI__

void SpiMasterInit(void) 
{ 
	/* Set MOSI and SCK output, all others input */ 
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK); 
	DDRB = (DDRB & ~(1<<4)) | (1<<3) | (1<<5) | (1<<1) | (1<<2);
	PORTB =(PORTB&~((1<<4)|(1<<3) | (1<<5)))|  (1<<1) | (1<<2); // and set high (not selected).
	/* Enable SPI, Master, set clock rate fck/16, 1MHz. */ 
	// @TODO Change to faster speed after testing.
	SPSR |= (1<<SPI2X);	// 
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);  // fck/2
	//DDRB |=3; // enable CSs.
	//PORTB |=3; // and set high (not selected).
} 

//#define __TESTSPIREADY__

void SpiMasterTransmit(byte cData) 
{ 
	/* Start transmission */ 
#ifdef __TESTSPIREADY__
	short timeout;
#endif
	_SpiDebug(cData);
	SPDR = cData; 
	/* Wait for transmission complete */ 
#ifdef __TESTSPIREADY__
	timeout=gClock+50;
#endif
	while(!(SPSR & (1<<SPIF))) {
#ifdef __TESTSPIREADY__
		SetLed((gClock>>4)&1);
		if(timeout-gClock<0) { // bad!
			Emit('@');
			DotHex(SPCR);
			Emit(' ');
			DotHex(SPSR);
			Emit(' ');
			DotHex(DDRB);
			Emit(' ');
			DotHex(PORTB);
			timeout=gClock+50;
		}
#endif
	}
}

byte SpiMasterReadByte(void)
{
	byte res;
	SpiMasterTransmit(0);	// dummy data.
	res=SPDR;
	return res;	// return the data.
}

#else

void SpiMasterInit(void) 
{ 
	/* Set MOSI and SCK output, all others input */ 
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK); 
	DDRB = (DDRB & ~(1<<4)) | (1<<3) | (1<<5) | (1<<1) | (1<<2);
	// @TODO Change to faster speed after testing.
	//DDRB |=3; // enable CSs.
	PORTB =(PORTB&~((1<<4)|(1<<3) | (1<<5)))|  (1<<1) | (1<<2); // and set high (not selected).
} 

// Precon: SCK is low, CS is low.
// Sequence: bit=> MOSI, 1 => SCK, pause, 0 => SCK, pause.
//#define kDelayTicks 6
#define kDelayTicks 2
byte ClockBit(byte bit)
{
	DelayTick(kDelayTicks);
	PORTB =(PORTB&~kSpiMosi)|(bit<<kSpiMosiBit);	// data out.
	DelayTick(kDelayTicks*2);
	PORTB |=kSpiSck; // clock high.
	bit=(PINB>>kSpiMisoBit)&1;	// read the input bit.
	DelayTick(kDelayTicks*4);
	PORTB &=~kSpiSck;	// clock low.
	DelayTick(kDelayTicks*2);	// finish off!
	return bit;
}

byte SpiMasterTransmit(byte cData)
{
	byte aBit,res=0;
	// want MSB first.
	_SpiDebug(cData);
	for(aBit=0;aBit<8;aBit++) {
		res=(res<<1)|ClockBit((cData>>7)&1);
		cData<<=1;
	}
	//gSpiResVal=res;
	return res;
}

#define SpiMasterReadByte() SpiMasterTransmit(0)

#endif

#endif

//#define __TestSerialFlash__

#define kFlashManuIDAmic 0x37

#define kFlashCS 2

#define SerialFlashEnableCS() PORTB&=~(1<<kFlashCS); // enable CS.
#define SerialFlashDisableCS() PORTB|=(1<<kFlashCS); // disable CS.

#define kSerialFlashWREN (0x6)
#define kSerialFlashWRDI (0x4)
#define kSerialFlashRDSR (0x5)
#define kSerialFlashWRSR (0x1)
#define kSerialFlashREAD (0x3)
#define kSerialFlashFAST_READ (0xb)
#define kSerialFlashPP (0x2)
// kSerialFlashSE used on Sym Flashes, kSerialFlashBE on Bot boot.
#define kSerialFlashSE (0x20)
#define kSerialFlashBE (0xd8)
#define kSerialFlashCE (0xc7)
#define kSerialFlashDP (0xb9)
#define kSerialFlashRDID (0x9f)
#define kSerialFlashRES (0xab)

#define kSerialFlashWIP 1

byte SerialFlashRDSR()
{
	byte res;
	SerialFlashEnableCS();
	SpiMasterTransmit(kSerialFlashRDSR);
	res=SpiMasterReadByte();
	SerialFlashDisableCS();	
	return res;
}

void SerialFlashWaitReady()
{
	byte sr=0;
	while((sr=SerialFlashRDSR())&kSerialFlashWIP) {
#ifdef __TestSerialFlash__
		// don't need status if we're not testing flash.
		byte oldX,oldY;
		oldX=gSysVars.gCurX;
		oldY=gCurY;
		PrintAt(8,0);
		Dot(gClock);
		Emit(' ');
		DotHex(sr);
		PrintAt(oldX,oldY);
#endif
	}
}

void SerialFlashWriteEnable(void)
{
	SerialFlashEnableCS();
	SpiMasterTransmit(kSerialFlashWREN);
	SerialFlashDisableCS();	
}

/**
 * 
 **/
ushort SerialFlashID(void)
{
	ushort id;
	SerialFlashEnableCS();
	SpiMasterTransmit(kSerialFlashRDID);
	id=SpiMasterReadByte(); // continuation?
	if(id==0x7f)
		id=SpiMasterReadByte(); // yes so Manu ID is next.
	if(id==kFlashManuIDAmic) {	// it's a valid ID.
		id=SpiMasterReadByte();
		id=(id<<8)|SpiMasterReadByte(); // OK, got the full 16-bit ID.
	}
	else
		id=-1;	// invalid.
	SerialFlashDisableCS();
	return id;
}

void SerialFlashEraseSector(ushort sector)
{
	ushort eraseCmd=kSerialFlashSE;
	if((SerialFlashID()&0xff00)==0x2000)
		eraseCmd=kSerialFlashBE;	// BotBoot chips use Block Erase.
	SerialFlashWriteEnable();
	SerialFlashEnableCS();
	SpiMasterTransmit(eraseCmd);
	SpiMasterTransmit(sector>>8);
	SpiMasterTransmit(sector&0xff);
	SpiMasterTransmit(0);	// page aligned.
	SerialFlashDisableCS();
	SerialFlashWaitReady();
}

void SerialFlashReadBlock(ushort block, byte *dest)
{
	ushort ix;
	SerialFlashEnableCS();
	SpiMasterTransmit(kSerialFlashREAD);
	SpiMasterTransmit(block>>8);
	SpiMasterTransmit(block&0xff);
	SpiMasterTransmit(0);	// page aligned.
	for(ix=0;ix<kSerialFlashBlockSize;ix++) {	// blocks are 256b
		//SpiMasterReadByte();
		*dest++ = SpiMasterReadByte(); // gSpiResVal;
	}
	SerialFlashDisableCS();	
}

void SerialFlashWriteBlock(ushort block, byte *src)
{
	ushort ix;
	SerialFlashWriteEnable();
	SerialFlashEnableCS();
	SpiMasterTransmit(kSerialFlashPP);
	SpiMasterTransmit(block>>8);
	SpiMasterTransmit(block&0xff);
	SpiMasterTransmit(0);	// page aligned.
	for(ix=0;ix<kSerialFlashBlockSize;ix++) {	// blocks are 256b
		SpiMasterTransmit(*src++);
	}
	SerialFlashDisableCS();
	SerialFlashWaitReady();
}
