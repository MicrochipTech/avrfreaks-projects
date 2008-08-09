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
void inisialisasiSPILowSpeed(void)
{
    // SPI initialization
    // SPI Type: Master
    // SPI Clock Rate: 86,400 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR=0x53;
    SPSR=0x00;    
}
void inisialisasiSPIHighSpeed(void)
{     
    // SPI initialization
    // SPI Type: Master
    // SPI Clock Rate: 2*172,800 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR=0x52;
    SPSR=0x01;
}
unsigned char SPIMasterKirimData(unsigned char data)
{  
    //Start Transmission
    SPDR = data;
    //Tunggu sampai SPI Interrupt Flag set
    while(!(SPSR & (1<<7   )));      
    return SPDR;
} 
void enterProgrammingMode(void)
{
    /*** Instruksi Programming Enable pada Serial Programming ***/
    //Reset harus diberi pulsa positif minimal selama 2 siklus clock CPU setelah pin SCK diset 0 
    PORTB.7 =0;  
    PORTB|=(1<<4);
    delay_ms(1);
    PORTB&=(0<<4); 
    //Menunggu waktu tunda minimal selama 20 ms
    delay_ms(20);
    SPIMasterKirimData(0xAC);  
    SPIMasterKirimData(0x53); 
    SPIMasterKirimData(0x00); 
    SPIMasterKirimData(0x00);  
}
void chipErase(void)
{
    SPIMasterKirimData(0xAC);  
    SPIMasterKirimData(0x80);  
    SPIMasterKirimData(0x00);  
    SPIMasterKirimData(0x00);
    //Menunggu waktu tunda minimal selama 10 ms
    delay_ms(10);
}  
void leaveProgrammingMode(void)
{
    PORTB|=(1<<4);
    //Indikator Leave Programming Mode yaitu matinya LED reset pada MK Target
}
/************************************************************************************************************/
unsigned char cekEksistensiDevais(void)
{
   unsigned char status_eksistensi_devais; 
   unsigned char respon_devais;
      do
   {
       enterProgrammingMode();
       SPIMasterKirimData(0xAC);  
       SPIMasterKirimData(0x53); 
       respon_devais= SPIMasterKirimData(0x00); 
       SPIMasterKirimData(0x00);
       if(respon_devais!=0x53) kirimKonfirmasiGAGAL();
       else status_eksistensi_devais= 1;
       leaveProgrammingMode(); 
   }
   while(!status_eksistensi_devais);
   return status_eksistensi_devais;   
}
