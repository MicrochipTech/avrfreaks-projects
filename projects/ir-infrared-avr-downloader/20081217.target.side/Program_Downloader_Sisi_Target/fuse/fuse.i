//Deklarasi Prototipe Fungsi Operasi-Operasi Fuse Bits
/*** Read Signature Byte ***/
void ISPReadDevaisSignature(void);
/*** Fuse Low Byte Operation ***/
unsigned char ISPReadLowFuseBits(void); 
void ISPWriteLowFuseBits(unsigned char fuse_low_byte);    
/*** Fuse High Byte Operation ***/
unsigned char ISPReadHighFuseBits(void);  
void ISPWriteHighFuseBits(unsigned char fuse_high_byte); 
/*** Fuse Extended Byte Operation ***/
unsigned char ISPReadExtendedFuseBits(void); 
void ISPWriteExtendedBits(unsigned char fuse_extended_byte); 
 void inisialisasiExtSRAM(void);
unsigned char bacaExtSRAM(unsigned int addr_read_ext_SRAM);
void tulisExtSRAM(unsigned int addr_write_ext_SRAM, unsigned char data);
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega162
#pragma used+
sfrb UBRR1L=0;
sfrb UCSR1B=1;
sfrb UCSR1A=2;
sfrb UDR1=3;
sfrb OSCCAL=4;
sfrb OCDR=4;
sfrb PINE=5;
sfrb DDRE=6;
sfrb PORTE=7;
sfrb ACSR=8;
sfrb UBRR0L=9;
sfrb UCSR0B=0xa;
sfrb UCSR0A=0xb;
sfrb UDR0=0xc;
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
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   // 16 bit access
sfrb UBRR0H=0x20;
sfrb UCSR0C=0x20;
sfrb WDTCR=0x21;
sfrb OCR2=0x22;
sfrb TCNT2=0x23;
sfrb ICR1L=0x24;
sfrb ICR1H=0x25;
sfrb ASSR=0x26;
sfrb TCCR2=0x27;
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
sfrb OCR0=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb EMCUCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb UBRR1H=0x3c;
sfrb UCSR1C=0x3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-
 // Interrupt vectors definitions
// Needed by the power management functions (sleep.h)
#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x20
	.SET power_ctrl_reg=mcucr
	#endif
#endasm
/*
  CodeVisionAVR C Compiler
  (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.

  Prototype for SPI access function */
  #pragma used+
unsigned char spi(unsigned char data);
#pragma used-
#pragma library spi.lib
//Deklarasi Prototipe Fungsi Operasi-Operasi SPI-ISP
void inisialisasiSPILowSpeed(void);
void inisialisasiSPIHighSpeed(void);
unsigned char SPIMasterKirimData(unsigned char data);
void enterProgrammingMode(void);  
void chipErase(void);  
void leaveProgrammingMode(void);  
unsigned char cekEksistensiDevais(void);
      /*** Operasi Baca Kode Devais ***/
void ISPReadDevaisSignature(void)
{ 
    unsigned char i;
          /*** Instruksi Read Signature Byte pada Serial Programming ***/  
    //Devais Signature Byte ATmega8535
    //0x00 0x1E
    //0x01 0x93
    //0x02 0x08
    inisialisasiSPILowSpeed();    
    enterProgrammingMode();    
    for(i=0;i<3;i++)
    {
        SPIMasterKirimData(0x30);    
        SPIMasterKirimData(0x00);   
        SPIMasterKirimData(i);  
        devais_signature[i]=SPIMasterKirimData(0x00);
    }
    leaveProgrammingMode();   
    inisialisasiSPIHighSpeed();
}  
/****************************************************************************************/ 
//0xD9 -->> unprog clk (int clk), unprog EESAVE, prog BOOTSz1 & BOOTSz1, unprog BOOTRST  
//0xC9 -->> prog clk (ext clk), unprog EESAVE, prog BOOTSz1 & BOOTSz1, unprog BOOTRST
//0xE4 -->> programmed clk 8MHz, programmed SUT0 
//0xEF -->> unprogrammed clk, programmed SUT0 
/****************************************************************************************/ 
//************** Operasi Fuse Low Byte **************//
/* 
Konfigurasi Fuse Low Byte
Bit 7 -->> BODLEVEL  |  Bit 3 -->> CKSEL3   
Bit 6 -->> BODEN     |  Bit 2 -->> CKSEL2
Bit 5 -->> SUT1      |  Bit 1 -->> CKSEL1
Bit 4 -->> SUT0      |  Bit 0 -->> CKSEL1 
"0" = programmed 
"1" = unprogrammed
*/
/*** Operasi Baca Fuse Low Byte ***/
unsigned char ISPReadLowFuseBits(void)
{ 
    unsigned char fuse_low_byte;
         /*** Instruksi Read Fuse Low Bits pada Serial Programming ***/
    SPIMasterKirimData(0x50);    
    SPIMasterKirimData(0x00);
    SPIMasterKirimData(0x00);  
    fuse_low_byte=SPIMasterKirimData(0x00);
    return fuse_low_byte;
} 
/*** Operasi Tulis Fuse Low Byte ***/
void ISPWriteLowFuseBits(unsigned char fuse_low_byte)
{ 
    /*** Instruksi Write Fuse Low Bits pada Serial Programming ***/
    SPIMasterKirimData(0xAC);    
    SPIMasterKirimData(0xA0);
    SPIMasterKirimData(0x00);  
    SPIMasterKirimData(fuse_low_byte);
    delay_ms(5);
}
 //************** Operasi Fuse High Byte **************//
/* 
Konfigurasi Fuse High Byte
Bit 7 -->> @@@@@     |  Bit 3 -->> EESAVE   
Bit 6 -->> @@@@@     |  Bit 2 -->> BOOTSZ1
Bit 5 -->> SPIEN     |  Bit 1 -->> BOOTSZ0
Bit 4 -->> CKOPT     |  Bit 0 -->> BOOTRST 
"0" = programmed 
"1" = unprogrammed 
Ket: @@@@@ Tergantung Devais
*/
/*** Operasi Baca Fuse High Byte ***/
unsigned char ISPReadHighFuseBits(void)
{ 
    unsigned char fuse_high_byte;
          /*** Instruksi Read Fuse High Bits pada Serial Programming ***/
    SPIMasterKirimData(0x58);    
    SPIMasterKirimData(0x08);
    SPIMasterKirimData(0x00);  
    fuse_high_byte=SPIMasterKirimData(0x00);
    return fuse_high_byte;
} 
/*** Operasi Tulis Fuse High Byte ***/
void ISPWriteHighFuseBits(unsigned char fuse_high_byte)
{
    /*** Instruksi Write Fuse High Bits pada Serial Programming ***/
    SPIMasterKirimData(0xAC);    
    SPIMasterKirimData(0xA8);
    SPIMasterKirimData(0xFF);  
    SPIMasterKirimData(fuse_high_byte);
    delay_ms(5);
}       
unsigned char ISPReadExtendedFuseBits(void)
{ 
    unsigned char extended_fuse_byte;
          /*** Instruksi Read Extended Fuse Byte pada Serial Programming ***/
    SPIMasterKirimData(0x50);    
    SPIMasterKirimData(0x08);
    SPIMasterKirimData(0x00);  
    extended_fuse_byte= SPIMasterKirimData(0x00);
    return extended_fuse_byte;
} 
void ISPWriteExtendedBits(unsigned char fuse_extended_byte)
{
    /*** Instruksi Write Fuse High Bits pada Serial Programming ***/
    SPIMasterKirimData(0xAC);    
    SPIMasterKirimData(0xA4);
    SPIMasterKirimData(0xFF);  
    SPIMasterKirimData(fuse_extended_byte);
    delay_ms(5);
}
