//Deklarasi Prototipe Fungsi Operasi-Operasi Memori Flash
void ISPWriteFlashMemory(unsigned int jml_page_data); 
unsigned int ISPWriteHugeFlashMemory(unsigned int jml_page_data, unsigned int addr_page_flash_terakhir);
void ISPReadFlashMemory(unsigned int addr_flash_max);
unsigned int ISPReadHugeFlashMemory(unsigned int ukuran_halaman_flash, unsigned int addr_page_flash_terakhir);
void ISPPollingFlashMemory(unsigned int addr_rd_prog_memory);  
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
/***  Operasi Penulisan Memori Flash Page Mode***/
void ISPWriteFlashMemory(unsigned int jml_page_data)
{
    //Variabel Tulis Data Flash Memory 
    unsigned int addr_flash_memory_page;
    unsigned char data_low_byte, data_high_byte;
    unsigned int i; 
        addr_flash_memory_page=0x00;
    i=0;  
         while(jml_page_data>0)
    {  
        //Baca data memori dari SRAM Eksternal
        data_low_byte=bacaExtSRAM(i++);
        data_high_byte=bacaExtSRAM(i++); 
                /*** Instruksi Load Program Memory Page pada Serial Programming ***/  
        //Load Program Memory Low Byte  
        SPIMasterKirimData(0x40);  
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        SPIMasterKirimData(data_low_byte);  //data low byte
                                          /*** Instruksi Load Program Memory Page pada Serial Programming ***/  
        //Load Program Memory High Byte   
        SPIMasterKirimData(0x48);    
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        SPIMasterKirimData(data_high_byte);  //data high byte 
                  /*** Instruksi Write Program Memory Page pada Serial Programming ***/ 
        SPIMasterKirimData(0x4C);    
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        SPIMasterKirimData(0x00);
            /*** Polling Data Flash Memory ***/ 
        ISPPollingFlashMemory(addr_flash_memory_page); 
                  /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */    
        addr_flash_memory_page++; 
                jml_page_data--; 
    };  
} 
/***  Operasi Penulisan Memori Flash Page Mode Secara Bertahap***/
unsigned int ISPWriteHugeFlashMemory(unsigned int jml_page_data, unsigned int addr_page_flash_terakhir)
{
    //Variabel Tulis Data Flash Memory 
    unsigned int addr_flash_memory_page;
    unsigned char data_low_byte, data_high_byte;
    unsigned int i; 
        addr_flash_memory_page=addr_page_flash_terakhir;
    i=0;
          if(awal_akses_devais) 
    {
        enterProgrammingMode();
        chipErase();
        awal_akses_devais=0;
    }    
        //enterProgrammingMode(); 
    while(jml_page_data>0)
    {  
        //Baca data memori dari SRAM Eksternal
        data_low_byte=bacaExtSRAM(i++);
        data_high_byte=bacaExtSRAM(i++); 
                /*** Instruksi Load Program Memory Page pada Serial Programming ***/  
        //Load Program Memory Low Byte  
        SPIMasterKirimData(0x40);  
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        SPIMasterKirimData(data_low_byte);  //data low byte
                                          /*** Instruksi Load Program Memory Page pada Serial Programming ***/  
        //Load Program Memory High Byte   
        SPIMasterKirimData(0x48);    
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        SPIMasterKirimData(data_high_byte);  //data high byte 
                  /*** Instruksi Write Program Memory Page pada Serial Programming ***/ 
        SPIMasterKirimData(0x4C);    
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        SPIMasterKirimData(0x00);
            /*** Polling Data Flash Memory ***/ 
        ISPPollingFlashMemory(addr_flash_memory_page); 
                  /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */    
        addr_flash_memory_page++; 
                //Simpan alamat halaman flash terakhir yang telah diakses
        addr_page_flash_terakhir= addr_flash_memory_page;
                //Decrement
        jml_page_data--; 
    }; 
        if(akhir_akses_devais)leaveProgrammingMode(); 
    return addr_page_flash_terakhir;  
}
/***  Operasi Pembacaan Memori Flash Page Mode ***/
void ISPReadFlashMemory(unsigned int page_flash_max)
{
     unsigned char data_flash; 
     unsigned int addr_flash_memory_page;
     unsigned int i;  
          data_flash= 0;
     addr_flash_memory_page= 0;
     i= 0; 
          while(addr_flash_memory_page<page_flash_max)
     {
          /*** Instruksi Read Program Memory pada Serial Programming ***/
          //Read Program Memory Low Byte
          SPIMasterKirimData(0x20);    
          SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
          SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
          data_flash= SPIMasterKirimData(0x00); 
          tulisExtSRAM(i++, data_flash);
                    //Read Program Memory High Byte
          SPIMasterKirimData(0x28);    
          SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
          SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
          data_flash= SPIMasterKirimData(0x00);  
          tulisExtSRAM(i++, data_flash);  
                    /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */    
          addr_flash_memory_page++;   
     }; 
} 
/***  Operasi Pembacaan Memori Flash Page Mode ***/
unsigned int ISPReadHugeFlashMemory(unsigned int jml_halaman_flash, unsigned int addr_page_flash_terakhir)
{
     unsigned char data_flash; 
     unsigned int addr_flash_memory_page;
     unsigned int i;  
          addr_flash_memory_page= addr_page_flash_terakhir;
     i= 0; 
          if(awal_akses_devais) 
    {
        enterProgrammingMode();
        awal_akses_devais=0;
    }  
          while(jml_halaman_flash>0)
     {
          /*** Instruksi Read Program Memory pada Serial Programming ***/
          //Read Program Memory Low Byte
          SPIMasterKirimData(0x20);    
          SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
          SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
          data_flash= SPIMasterKirimData(0x00); 
          tulisExtSRAM(i++, data_flash);
                    //Read Program Memory High Byte
          SPIMasterKirimData(0x28);    
          SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
          SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
          data_flash= SPIMasterKirimData(0x00);  
          tulisExtSRAM(i++, data_flash);  
                    /* Autoincrement alamat untuk writing flash memory dan parameter polling flash memory */    
          addr_flash_memory_page++; 
                    //Simpan alamat halaman flash terakhir yang telah diakses
          addr_page_flash_terakhir= addr_flash_memory_page;
                 //Decrement
         jml_halaman_flash--;  
     };   
     if(akhir_akses_devais)leaveProgrammingMode();
     return addr_page_flash_terakhir;
}
/***  Operasi Polling Memori Flash Byte Mode untuk Menunggu Kesiapan Penulisan Byte Berikutnya***/
void ISPPollingFlashMemory(unsigned int addr_flash_memory_page)
{
    unsigned char ulang_polling_flash;
    unsigned char data_flash_memory; 
    //Reload jumlah pengulangan polling data flash memory
    ulang_polling_flash=2 ;  
          //Polling Address Low Byte
    while(ulang_polling_flash>0) 
    {
        /*** Instruksi Read Program Memory pada Serial Programming ***/
        SPIMasterKirimData(0x20);  
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        data_flash_memory=SPIMasterKirimData(0x00);  
        ulang_polling_flash--; 
        if(data_flash_memory==0xFF) 
        {
            delay_ms(4.5);
            ulang_polling_flash= 0; 
        } 
    }; 
        //Reload jumlah pengulangan polling data flash memory
    ulang_polling_flash=2 ;
        //Polling Address High Byte
    while(ulang_polling_flash>0) 
    {
        /*** Instruksi Read Program Memory pada Serial Programming ***/
        SPIMasterKirimData(0x28);  
        SPIMasterKirimData((unsigned char)(addr_flash_memory_page>>8));   
        SPIMasterKirimData((unsigned char) addr_flash_memory_page);  
        data_flash_memory=SPIMasterKirimData(0x00);
        ulang_polling_flash--; 
        if(data_flash_memory==0xFF) 
        {
            delay_ms(4.5);
            ulang_polling_flash= 0; 
        }
    };   
}               
