/*****************************************************
Project : Final Assignment 
Version : 
Date    : 18/08/2008
Author  : BaSu-Key                       
Company : kulirobot                    
Comments: 
Kesimpulan sementara sampai dengan program ini dibuat adalah sebagai berikut:
1. Baudrate yang digunakan adalah 2400 bps karena keterbatasan hardware IR.   
2. Satu byte data terbaca dalam waktu 5 ms dengan baudrate 2400 bps.
3. Delay perbyte data yang dikirim minimal adalah 6 ms untuk menghindari kesalahan pembacaan berdasarkan poin nomor 2.
4. Delay perpaket data yang dikirim adalah 8 ms untuk memberikan kesempatan sisi penerima data untuk memproses data.
5. PANJANG_PAKET yang pernah dicoba dengan hasil memuaskan adalah 32 byte, 64 byte dan yang terakhir adalah 128 byte. 
6. Penentuan USART_TIMEOUT dihitung dengan menggunakan perhitungan (HEADER+PANJANG_PAKET+CRC)*(delay perpaket+delay perbyte).
7. Fclk SCK low speed yang digunakan adalah 86,400 kHz khusus pada devais baru atau devais yang memiliki clock rendah. 
8. Fclk SCK high speed yang digunakan adalah 691,200 kHz. Uji coba fclk high speed dengan ATmega8535L dan diperoleh fakta
   bahwa jika fclk yang digunakan adalah 2764,800 kHz maka terjadi kesalahan pada penulisan devais.
9. Operasi SPI-ISP yang telah berhasil dilakukan adalah sebagai berikut:  
    -->> Enter Programming Mode                     <OK> <IR>
    -->> Read Devais Signature                      <OK> <IR>
    -->> Read Fuse Low Byte                         <OK> <IR>
    -->> Read Fuse High Byte                        <OK> <IR>
    -->> Write Fuse Low Byte                        <OK> <IR>
    -->> Write Fuse High Byte                       <OK> <IR>
    -->> Erasing Chip                               <OK> <IR>
    -->> Writting Flash Program Memory Page Access  <OK> <IR> 
    -->> Writting Flash Program Memory Byte Access  <NO>   
    -->> Polling Flash Program Memory Byte Access   <OK> <IR>
    -->> Verifikasi Flash (Read Flash)              <OK> <IR>
    -->> Writting EEPROM Data Memory Byte Access    <OK> <IR>
    -->> Writting EEPROM Data Memory Page Access    <NO> 
    -->> Polling EEPROM Program Memory Byte Access  <OK> <IR> 
    -->> Verifikasi EEPROM (Read EEPROM)            <OK> <IR>
    -->> Autoincrement Addressing Page Mode         <OK> <IR>
    -->> Leave Programming Mode                     <OK> <IR>  

Chip type           : ATmega162
Program type        : Application
Clock frequency     : 11,059200 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 512
*****************************************************/
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
// CodeVisionAVR C Compiler
// (C) 1998-2000 Pavel Haiduc, HP InfoTech S.R.L.
#pragma used+
void delay_us(unsigned int n);
void delay_ms(unsigned int n);
#pragma used-
unsigned int hitungCRC16(unsigned char *data, unsigned char len);
//debugging
void tulis_EEPROM(unsigned char data);  
//Deklarasi Prototipe Fungsi Operasi-Operasi Memori EEPROM
void ISPWriteEEPROM(unsigned int jml_byte_data);
void ISPReadEEPROM(unsigned int addr_EEPROM_max);
void ISPPollingEEPROM(unsigned int addr_EEPROM_memory);   
 void inisialisasiExtSRAM(void);
unsigned char bacaExtSRAM(unsigned int addr_read_ext_SRAM);
void tulisExtSRAM(unsigned int addr_write_ext_SRAM, unsigned char data);
//Deklarasi Prototipe Fungsi Operasi-Operasi Memori Flash
void ISPWriteFlashMemory(unsigned int jml_page_data); 
unsigned int ISPWriteHugeFlashMemory(unsigned int jml_page_data, unsigned int addr_page_flash_terakhir);
void ISPReadFlashMemory(unsigned int addr_flash_max);
unsigned int ISPReadHugeFlashMemory(unsigned int ukuran_halaman_flash, unsigned int addr_page_flash_terakhir);
void ISPPollingFlashMemory(unsigned int addr_rd_prog_memory);  
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
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega162
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
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega162
 					void inisialisasiTimer(void);
// CodeVisionAVR C Compiler
// (C) 1998-2004 Pavel Haiduc, HP InfoTech S.R.L.
// I/O registers definitions for the ATmega162
 					//USART Control and Status Register A – UCSRA 
//USART Control and Status Register B – UCSRB 
void inisialisasiUSART(void); 
//Pengiriman data serial menggunakan IR
void kirimDataSerialIR(unsigned char data); 
//Pengiriman data serial untuk debugging program
void kirimDataSerial(unsigned char data);
//Kapasitas SRAM maksimal yang digunakan pada perangkat
//sisi target adalah 8 kbyte
//D-Latch
/******************************************************/
bit awal_akses_devais;          //bit status untuk awal akses devais MK target
bit akhir_akses_devais;         //bit status untuk akhir akses devais MK target 
//bit data_ukuran_besar;        //bit status untuk penanda data lebih dari kapasitas SRAM
bit status_paket_perintah; 
bit status_paket_data;
bit status_paket_akhir;
bit status_konfirmasi;
bit tunggu_konfirmasi;
bit akhir_transmisi;   
bit status_perintah_tulis;    
bit status_tulis_bertahap;
bit status_baca_bertahap;  
bit status_data_baru_sram;
/******************************************************/ 
unsigned char buffer_konfirmasi[3]; 
unsigned char data_load[128   ];                  //Menyimpan data yang akan diload ke bufferPaketData 
unsigned char fuse_bits[3];                              //Variabel yang berisi fuse bits (lfuse, hfuse, exfuse) 
unsigned char ID_paket;
unsigned char ID_paket_gagal; 
unsigned char ID_paket_OK;
unsigned char ID_perintah;
unsigned char indek_konfirmasi;
unsigned char indek_data;
unsigned char jml_paket;   
unsigned char devais_signature[3];                       //Variabel yang berisi devais signature  
unsigned char kode_devais[3];                            //Kode devais dari pemrogram
unsigned char status_paket;     
unsigned char instruksi_pemrogram;
/******************************************************/  
unsigned int alamat_awal;
unsigned int alamat_akhir;                              //Alamat terakhir akses memori flash 
unsigned int CRC; 
unsigned int idxBufExtSRAM;                             //Indek untuk akses buffer data pada SRAM   
unsigned int jml_byte_data;                             //Jumlah byte data yang diterima -> deteksi paket sram   
unsigned int jml_page_data;
unsigned int time1;             
/***********************************************/
//Fungsi-Fungsi Interrupt Terima Data Serial IR
/***********************************************/ 
/************************************************************************************************************/
// USART0 Receiver buffer
char rx_buffer0[256];
unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
/************************************************************************************************************/
// USART0 Receiver interrupt service routine
interrupt [20] void usart0_rx_isr(void)
{
    unsigned char status,data;  
    status=UCSR0A;
    data=UDR0;
    if ((status & (((1<<4))|((1<<3) )))==0)
       {
           rx_buffer0[rx_wr_index0]= data;
           if (++rx_wr_index0 == 256) rx_wr_index0= 0;
           if (++rx_counter0 == 256) rx_counter0= 0;
       }
}
/************************************************************************************************************/
// Fungsi Pembacaan Data Serial pada Buffer Interrupt RXC
unsigned char bacaDataSerialIR(void)
{
    unsigned char data;
        while (rx_counter0==0);
    data= rx_buffer0[rx_rd_index0];
    if (++rx_rd_index0 == 256) rx_rd_index0= 0;
    #asm("cli") ;
    --rx_counter0;
    #asm("sei");                                                                            
    return data;
} 
/************************************************************************************************************/ 
/*****************************************************************/ 
//Fungsi-Fungsi Protokol Komunikasi untuk Operasi Pengiriman Data
/*****************************************************************/ 
/************************************************************************************************************/
// Fungsi Load Data Transmisi 
void loadDataBaru(unsigned char indek_paket)
{ 
	unsigned char i; 
	//unsigned int jml_byte_data_sram;
	unsigned int indek_data;
		if(status_baca_bertahap)
	{
    	// Pengindekan paket (ID Paket) dimulai dari 0 
    	indek_paket= indek_paket-1;
    	    	// Pengambilan data dari SRAM Eksternal sama dengan panjang paket dengan indek mulai 0
    	indek_data= (unsigned int)indek_paket*128   ;
    	    	for(i=0;i<128   ;i++)
        {
        	data_load[i]= bacaExtSRAM(indek_data++);     //Ambil data dari SRAM Eksternal
        	jml_byte_data++;   
        }
	}
	else
	{
        // Pengindekan paket (ID Paket) dimulai dari 0 
    	indek_paket= indek_paket-1;
    	    	// Pengambilan data dari SRAM Eksternal sama dengan panjang paket dengan indek mulai 0
    	indek_data= (unsigned int)indek_paket*128   ;
    	    	for(i=0;i<128   ;i++)
        	data_load[i]= bacaExtSRAM(indek_data++);     //Ambil data dari SRAM Eksternal
    }
    if(jml_byte_data>1024*8                        ) 
    {
        status_data_baru_sram= 0; 
        indek_data= 0; 
    }
}
/************************************************************************************************************/  
// Fungsi Pengiriman Paket Awal Transmisi 
// ID_perintah sama dengan yang dikirim oleh pemrogram
void kirimPaketPerintah(void)
{
    unsigned char i; 
    unsigned char buffer_paket_data[9];
      	    #asm("cli") ;
        buffer_paket_data[0]= '<';
    buffer_paket_data[1]= 0x46;       // ID Sisi Target
    buffer_paket_data[2]= '>';
    buffer_paket_data[3]= ID_perintah;
	    // Informasi jumlah paket yang dikirimkan
    buffer_paket_data[4]= '|';
    buffer_paket_data[5]= jml_paket;  // Jumlah Paket yang akan dikirimkan
    buffer_paket_data[6]= '|';
    CRC= hitungCRC16(buffer_paket_data, 7);
    buffer_paket_data[7]= (unsigned char) CRC;
    buffer_paket_data[8]= (unsigned char)(CRC>>8);
       //Persiapan pengiriman data 
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
	    for(i= 0; i <=8 ; i++)
        kirimDataSerialIR(buffer_paket_data[i]);
      	    #asm("sei");
} 
/************************************************************************************************************/  
// Fungsi Pengiriman Paket Data Transmisi 
void kirimPaketData(void)
{
    unsigned char i, j;
    unsigned char buffer_paket_data[128   +9];      // Ukuran buffer adalah [HEADER(7)+ DATA(PANJANG_PAKET)+CRC(2)]
	    #asm("cli") ;
	    if(!status_konfirmasi) ID_paket= ID_paket_gagal;
    else ID_paket++;
    loadDataBaru(ID_paket);    
        // Header Paket Data
    buffer_paket_data[0]= '<';
    buffer_paket_data[1]= 0x46;
    buffer_paket_data[2]= '>';
    buffer_paket_data[3]= ID_perintah; // Berisi informasi ID Perintah
    buffer_paket_data[4]= '|';
    buffer_paket_data[5]= ID_paket;    // Berisi informasi ID Paket atau Indek Paket
    buffer_paket_data[6]= '|';
	    // Data yang dikirim
	for(i=0; i<128   ; i++)
	{
		j= i+7;
		buffer_paket_data[j]= data_load[i];
	};
    CRC= hitungCRC16(buffer_paket_data, (128   +7));           //HEADER(7)+DATA(PANJANG_PAKET)
    buffer_paket_data[128   +7]= (unsigned char)  CRC;         //HEADER(7)+DATA(PANJANG_PAKET)+CRC(low byte)
    buffer_paket_data[128   +8]= (unsigned char) (CRC>>8);     //HEADER(7)+DATA(PANJANG_PAKET)+CRC(high byte)
	    //Persiapan pengiriman data 
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
        for(i= 0; i <= (128   +8) ; i++)
        kirimDataSerialIR(buffer_paket_data[i]);
		#asm("sei"); 
}
/************************************************************************************************************/  
// Fungsi Pengiriman Paket Akhir Transmisi 
void kirimPaketAkhir(void)
{
	unsigned char i; 
	unsigned char buffer_paket_data[9];
		#asm("cli") ;  
    buffer_paket_data[0]= '<';
    buffer_paket_data[1]= 0x46;
    buffer_paket_data[2]= '>';
	    // Informasi penanda paket akhir
    buffer_paket_data[3]= '\n';
    buffer_paket_data[4]= '|';
    buffer_paket_data[5]= '\r';
    buffer_paket_data[6]= '|';
    CRC= hitungCRC16(buffer_paket_data, 7);
    buffer_paket_data[7]= (unsigned char) CRC;
    buffer_paket_data[8]= (unsigned char) (CRC>>8);
	    //Persiapan pengiriman data 
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
        for(i= 0; i <=8 ; i++)
        kirimDataSerialIR(buffer_paket_data[i]);
		#asm("sei"); 
}
/************************************************************************************************************/
// Fungsi Cek Konfirmasi Paket Awal dan Akhir
void olahKonfirmasiPaketPerintahDanAkhir(void)
{
	unsigned char data_konfirmasi= 0;  
		while(tunggu_konfirmasi)
	{
		data_konfirmasi= bacaDataSerialIR(); 
		//kirimDataSerial(data_konfirmasi); 
		buffer_konfirmasi[indek_konfirmasi++]= data_konfirmasi;
		if((indek_konfirmasi == 1 && data_konfirmasi != '|') || (indek_konfirmasi == 3 && data_konfirmasi != '|'))
	            indek_konfirmasi= 0;
		else if(indek_konfirmasi == 4)
		{
			if((buffer_konfirmasi[1] == 0xAA) && (data_konfirmasi == 0xAA))
			{
				status_konfirmasi= 1;
				tunggu_konfirmasi= 0;
				if(status_paket_perintah) 
				{
				    status_paket_perintah= 0;
				    status_paket_data= 1;
				}
				else if(status_paket_akhir) status_paket_akhir= 0;	
			}
    		else tunggu_konfirmasi= 0;
		    indek_konfirmasi= 0;
		}
	};
}
/************************************************************************************************************/
// Fungsi Cek Konfirmasi Paket Data
void olahKonfirmasiPaketData(void)
{
	unsigned char data_konfirmasi= 0;   
	while(tunggu_konfirmasi)
	{       
		data_konfirmasi= bacaDataSerialIR();  
		//kirimDataSerial(data_konfirmasi);
		buffer_konfirmasi[indek_konfirmasi++]= data_konfirmasi; 
		if((indek_konfirmasi == 1 && data_konfirmasi != '|') || (indek_konfirmasi == 3 && data_konfirmasi != '|'))
	            indek_konfirmasi= 0;
		else if(indek_konfirmasi == 4)
		{
			if((buffer_konfirmasi[1] == 0xAA) && (data_konfirmasi == ID_paket))
			{
				status_konfirmasi= 1;
				tunggu_konfirmasi= 0;
			}
			else
			{
				if(buffer_konfirmasi[1] == 0x55)
				{
					status_konfirmasi= 0;
					tunggu_konfirmasi= 0;
									ID_paket_gagal= data_konfirmasi;
				}
				else
				{
					status_konfirmasi= 0;
					tunggu_konfirmasi= 0;
				}		
			}
			indek_konfirmasi= 0;
		}
	}; 
} 
/************************************************************************************************************/ 
/******************************************************************/
//Fungsi-Fungsi Protokol Komunikasi untuk Operasi Penerimaan Data
/******************************************************************/ 
// Fungsi Pengiriman Kondisi IDLE
void kirimKonfirmasiIDLE(void)
{	
	UCSR0B&= ~(1<<7  );
	UCSR0B&= ~(1<<4);
	#asm("cli") ;
		kirimDataSerialIR(0x00);  
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
		kirimDataSerialIR('|');
	kirimDataSerialIR('\n');
	kirimDataSerialIR('|');
	kirimDataSerialIR('\r');
		#asm("sei");       
	UCSR0B|= (1<<4);                   
	UCSR0B|= (1<<7  );
}
/************************************************************************************************************/
// Fungsi Pengiriman Konfirmasi Paket OK 
void kirimKonfirmasiPaketOK(void)
{
	UCSR0B&= ~(1<<7  );
	UCSR0B&= ~(1<<4);
	#asm("cli") ;
		kirimDataSerialIR(0x00);  
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
		kirimDataSerialIR('|');
	kirimDataSerialIR(0xAA);
	kirimDataSerialIR('|');
	//Berisi informasi ID Paket yang Berhasil
	kirimDataSerialIR(ID_paket_OK);            
		#asm("sei");               
	UCSR0B|= (1<<4);           
	UCSR0B|= (1<<7  );
}
/************************************************************************************************************/
// Fungsi Pengiriman Konfirmasi OK 
void kirimKonfirmasiOK(void)
{	
	UCSR0B&= ~(1<<7  );
	UCSR0B&= ~(1<<4);
	#asm("cli") ;
		kirimDataSerialIR(0x00);  
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
		kirimDataSerialIR('|');
	kirimDataSerialIR(0xAA);
	kirimDataSerialIR('|');
	kirimDataSerialIR(0xAA);
		#asm("sei");               
	UCSR0B|= (1<<4);           
	UCSR0B|= (1<<7  ); 
}
/************************************************************************************************************/
// Fungsi Pengiriman Konfirmasi Paket Gagal
void kirimKonfirmasiGAGAL(void)
{
	UCSR0B&= ~(1<<7  );
	UCSR0B&= ~(1<<4);
	#asm("cli") ;
		kirimDataSerialIR(0x00);  
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR('|');
	kirimDataSerialIR(0x55);
	kirimDataSerialIR('|');
	//Berisi informasi ID Paket yang Gagal
	kirimDataSerialIR(ID_paket);
		#asm("sei");               
	UCSR0B|= (1<<4);           
	UCSR0B|= (1<<7  );
}
/************************************************************************************************************/
// Timer 1 output compare A interrupt service routine  
// Interrupt setiap 20 ms
interrupt [14] void timer1_compa_isr(void)
{
      TCNT1L= 0x00;
      TCNT1H= 0x00;
      	      if(time1 > 0)
      --time1;
      	      if(time1 == 0)
      {
            time1= 68 ;
            		            if(status_paket == 2)
            {   
                  //Reset indek data pada buffer data sementara
                  indek_data= 0;
                  kirimKonfirmasiGAGAL();
            }
            else if(status_paket == 1)
                  kirimKonfirmasiOK();
            else if(status_paket == 3)
                  kirimKonfirmasiPaketOK();
            else if(status_paket == 5    )
                  kirimKonfirmasiIDLE();   
      }
}
/************************************************************************************************************/ 
//Rutin inisialisasi hardware dan variabel
void inisialisasi(void)
{ 
    unsigned char eksistensi_devais;
        //ID_paket mulai dari 1 sampai dengan jumlah paket maksimal
    //Diincrement pada saat pemanggilan fungsi load data 
    ID_paket= 0; 
    ID_paket_gagal= 1;   
        //Jumlah paket data maksimal
    jml_paket= 0; 
             //Indek buffer konfirmasi
    indek_konfirmasi= 0;
    ID_paket_OK= 0;
        //Status konfirmasi
    //OK= 1, GAGAL= 0
    status_konfirmasi= 0;
    tunggu_konfirmasi= 0;
        //Status Paket Data Transmisi
    status_paket_perintah= 1;
    status_paket_data= 0;
    status_paket_akhir= 0;
                       //Untuk timer scheduling
    time1= 68 ;
	    //Status paket data yang diterima
    status_paket= 0; 
	    //Indek untuk pengalamatan SRAM eksternal
    idxBufExtSRAM= 0;
	        //IndekData untuk pengindekan buffer_paket_data
    indek_data= 0;  
    akhir_transmisi= 0; 
        //Set bit status penulisan data
    status_tulis_bertahap= 0;  
        //Set alamat akhir memori devais yang diakses
    alamat_awal= 0;
    alamat_akhir= 0;
        //Set kondisi akses devais  
    awal_akses_devais= 1;
    akhir_akses_devais= 0;
        //Set jumlah data byte yang diterima
    jml_byte_data= 0; 
        //Set LED indikator;
    PORTE |= (1<<0)  ;
    PORTD |= (1<<3)    ;
        //Cek eksistensi devais yang terpasang pada sisi target
    eksistensi_devais= cekEksistensiDevais();  
    if(eksistensi_devais) PORTD &= ~(1<<3);  
	    //Baca Devais Signature untuk dibandingkan dengan kode devais dari pemrogram
    ISPReadDevaisSignature(); 
        //Aktifkan Global Interrupt Enable
    #asm("sei");
}  
/************************************************************************************************************/
// Fungsi pengiriman data ke pemrogram                   
void kirimDataKePemrogram(void)
{        
    TCCR1B= 0x00;
    UCSR0B&= ~(1<<7  ); 
    UCSR0B|= (1<<4); 
        //Inisialisasi ID Paket mulai nol
    //ID Paket yang dikirim mulai dari satu
    ID_paket= 0;
	    //Status Paket Data Transmisi
    status_paket_perintah= 1;
    status_paket_data= 0;
    status_paket_akhir= 0;  
        //Status konfirmasi
    //OK= 1, GAGAL= 0
    status_konfirmasi= 0;
    tunggu_konfirmasi= 0;
        //Pengiriman Paket Perintah
    while(status_paket_perintah==1)
    {
        kirimPaketPerintah();
        tunggu_konfirmasi= 1; 
        UCSR0B|= (1<<4); 
        UCSR0B|= (1<<7  );
        olahKonfirmasiPaketPerintahDanAkhir();
        UCSR0B&= ~(1<<7  ); 
        UCSR0B&= ~(1<<4);
    };
    //Pengiriman Paket Data  
    while(status_paket_data==1)
    {      
        //debugging program
        kirimDataSerial(ID_paket);  //Debug log serial USB
        kirimPaketData();
        tunggu_konfirmasi= 1;
        UCSR0B|= (1<<4);
        UCSR0B|= (1<<7  ); 
        olahKonfirmasiPaketData(); 
        UCSR0B&= ~(1<<7  );
        UCSR0B&= ~(1<<4);
        if(ID_paket==jml_paket)
        { 
            status_paket_data= 0;
            status_paket_akhir= 1; 
        }   
    }; 
        //Reset indek buffer interrupt RXC
    rx_wr_index0= 0; 
    rx_rd_index0= 0;
    rx_counter0= 0;
        //Pengiriman Paket Akhir  
    while(status_paket_akhir==1)
    {       
        kirimPaketAkhir(); 
        tunggu_konfirmasi= 1;
        UCSR0B|= (1<<4); 
        UCSR0B|= (1<<7  );    
        olahKonfirmasiPaketPerintahDanAkhir(); 
        UCSR0B&= ~(1<<7  ); 
        UCSR0B&= ~(1<<4);
    };  
            TCCR1B= 0x02;
}   
/************************************************************************************************************/
// Fungsi pengiriman data berukuran besar ke pemrogram                   
void kirimDataFlashBesarKePemrogram(void)
{        
    unsigned char counter= 1;
        TCCR1B= 0x00;
    UCSR0B&= ~(1<<7  ); 
    UCSR0B|= (1<<4); 
        //Inisialisasi ID Paket mulai nol
    //ID Paket yang dikirim mulai dari satu
    ID_paket= 0;
	    //Status Paket Data Transmisi
    status_paket_perintah= 1;
    status_paket_data= 0;
    status_paket_akhir= 0;  
        //Status konfirmasi
    //OK= 1, GAGAL= 0
    status_konfirmasi= 0;
    tunggu_konfirmasi= 0;  
        status_baca_bertahap= 1;
    alamat_awal= 0;
        //Pengiriman Paket Perintah
    while(status_paket_perintah==1)
    {
        kirimPaketPerintah();
        tunggu_konfirmasi= 1; 
        UCSR0B|= (1<<4); 
        UCSR0B|= (1<<7  );
        olahKonfirmasiPaketPerintahDanAkhir();
        UCSR0B&= ~(1<<7  ); 
        UCSR0B&= ~(1<<4);
    };
    //Pengiriman Paket Data  
    while(status_paket_data==1)
    {      
        if(status_baca_bertahap)  
        {
            PORTD &= ~(1<<3);
            alamat_akhir= ISPReadHugeFlashMemory(1024*8                        , alamat_awal);
            alamat_awal= alamat_akhir;
            status_data_baru_sram= 1;
            PORTD |= (1<<3)    ;
        }
        while(status_data_baru_sram)
        {       
            //debugging program
            //kirimDataSerial(counter++);  //Debug log serial USB
            kirimPaketData();
            tunggu_konfirmasi= 1;
            UCSR0B|= (1<<4);
            UCSR0B|= (1<<7  ); 
            olahKonfirmasiPaketData(); 
            UCSR0B&= ~(1<<7  );
            UCSR0B&= ~(1<<4);
            if(ID_paket==jml_paket+1)
            { 
                status_baca_bertahap= 0;
                status_paket_data= 0;
                status_paket_akhir= 1; 
                status_data_baru_sram= 0;
            } 
        };  
    }; 
        //Reset indek buffer interrupt RXC
    rx_wr_index0= 0; 
    rx_rd_index0= 0;
    rx_counter0= 0;
        //Pengiriman Paket Akhir  
    while(status_paket_akhir==1)
    {       
        kirimPaketAkhir(); 
        tunggu_konfirmasi= 1;
        UCSR0B|= (1<<4); 
        UCSR0B|= (1<<7  );    
        olahKonfirmasiPaketPerintahDanAkhir(); 
        UCSR0B&= ~(1<<7  ); 
        UCSR0B&= ~(1<<4);
    };  
            TCCR1B= 0x02;
}
/************************************************************************************************************/
// Fungsi Pembacaan Ukuran Flash untuk mengetahui ukuran flash dan EEPROM maksimal 
unsigned int bacaUkuranFlash(void)
{
    // Ukuran memori flash berdasarkan devais signature
    // 0x90 -->  1024 byte
    // 0x91 -->  2048 byte
    // 0x92 -->  4096 byte
    // 0x93 -->  8192 byte
    // 0x94 --> 16384 byte
    // 0x95 --> 32768 byte
    // 0x96 --> 65536 byte 
        unsigned char kode_devais;
    unsigned int ukuran_flash;
        kode_devais= devais_signature[1];
    #asm("cli") ;    
    switch(kode_devais)
    {
        case 0x90:
             ukuran_flash= 1024;
             break;
        case 0x91:
             ukuran_flash= 2048;
             break;    
        case 0x92:
             ukuran_flash= 4096;
             break;
        case 0x93:
             ukuran_flash= 8192;
             break;
        case 0x94:
             ukuran_flash= 16384;
             break; 
        case 0x95:
             ukuran_flash= 32768;
             break;
        case 0x96:
             ukuran_flash= 65536;
             break; 
        default : ukuran_flash= 0;                                      
    } 
    return ukuran_flash;
}     
/************************************************************************************************************/  
// Fungsi Tulis Flash, data dari kiriman pemrogram
void tulisMemoriFlash(void)
{
    unsigned int jml_page_data;
    jml_page_data= 0;
        #asm("cli") ;    
    jml_page_data= (unsigned int) (idxBufExtSRAM/2); //Benar bisa jalan OK  
    PORTD &= ~(1<<3); 
    enterProgrammingMode();   
    chipErase();     
    ISPWriteFlashMemory(jml_page_data);
    leaveProgrammingMode();  
    PORTD |= (1<<3)    ;
    #asm("sei");
} 
/************************************************************************************************************/
// Fungsi Tulis EEPROM, data dari kiriman pemrogram
void tulisEEPROM(void)
{
    unsigned int jml_byte_data;
    jml_byte_data= 0;
         #asm("cli") ;    
    jml_byte_data= idxBufExtSRAM;
    PORTD &= ~(1<<3); 
    enterProgrammingMode();      
    ISPWriteEEPROM(jml_byte_data);
    leaveProgrammingMode();
    PORTD |= (1<<3)    ;
    #asm("sei");
}
/************************************************************************************************************/
// Fungsi Baca Flash, hasil pembacaan kirim ke pemrogram
void bacaMemoriFlash(void)
{
    unsigned int flash_page_max;
    unsigned int ukuran_flash_max; 
    #asm("cli") ;    
    //Baca flash memory kemudian data disimpan dalam SRAM Eksternal 
    ukuran_flash_max= bacaUkuranFlash();
    flash_page_max= ukuran_flash_max/2; //OK sudah bisa 
    jml_paket= (unsigned char) (ukuran_flash_max/128   );
    if(ukuran_flash_max>1024*8                        ) status_tulis_bertahap= 1; 
    //Pengiriman data pembacaan flash ke sisi pemrogram
    if(status_tulis_bertahap) 
    {
        //data flash melebihi kapasitas SRAM (ukuran flash besar)
        //debugging program  
        kirimDataSerial(0x11); //Debug log serial USB
        kirimDataFlashBesarKePemrogram(); 
    }
    else 
    { 
        //Kapasitas SRAM cukup untuk sekali tampung data flash (ukuran flash kecil)
        PORTD &= ~(1<<3);
        enterProgrammingMode();   
        ISPReadFlashMemory(flash_page_max);   
        leaveProgrammingMode();
        PORTD |= (1<<3)    ;
        //debugging program  
        kirimDataSerial(0x22); //Debug log serial USB
        kirimDataKePemrogram();
    } 
    #asm("sei");  
}
/************************************************************************************************************/
// Fungsi Baca EEPROM, hasil pembacaan kirim ke pemrogram
void bacaEEPROM(void)
{
    unsigned int ukuran_flash_max;
    unsigned int EEPROM_size_max;
    #asm("cli") ;  
    //Baca EEPROM memory kemudian data disimpan dalam SRAM Eksternal   
    ukuran_flash_max= bacaUkuranFlash();
    EEPROM_size_max= (unsigned int)(ukuran_flash_max/16); //OK sudah bisa   
    PORTD &= ~(1<<3);
    enterProgrammingMode();   
    ISPReadEEPROM(EEPROM_size_max); 
    leaveProgrammingMode();
    PORTD |= (1<<3)    ;
    #asm("sei");
        //Pengiriman data pembacaan EEPROM ke sisi pemrogram
    jml_paket= (unsigned int)(EEPROM_size_max/128   );
    kirimDataKePemrogram();    
}
/************************************************************************************************************/ 
// Fungsi Baca Fuse Bits
void bacaFuseBits(void)
{
    #asm("cli") ; 
    PORTD &= ~(1<<3);
    /*** Set SCK dengan frekuensi rendah ***/ 
    inisialisasiSPILowSpeed(); 
    enterProgrammingMode(); 
    fuse_bits[0]= ISPReadLowFuseBits();  
    fuse_bits[1]= ISPReadHighFuseBits();   
    fuse_bits[2]= ISPReadExtendedFuseBits();
    leaveProgrammingMode();
    PORTD |= (1<<3)    ;
    #asm("sei");  
        // Pengiriman data fuse bits sesuai dengan format protokol dengan pemrogram
    tulisExtSRAM(0, 0xFF);           //ID Fuse Bits (Diabaikan Pemrogram)
    tulisExtSRAM(1, '|');           
    tulisExtSRAM(2, fuse_bits[0]);   //Low Fuse
    tulisExtSRAM(3, '|');
    tulisExtSRAM(4, fuse_bits[1]);   //High Fuse
    tulisExtSRAM(5, '|');
    tulisExtSRAM(6, fuse_bits[2]);   //Extended Fuse
    //Pengiriman data pembacaan flash ke sisi pemrogram
    jml_paket= 1;  
    kirimDataKePemrogram();
}     
/************************************************************************************************************/  
// Fungsi Tulis Fuse Bits
void tulisFuseBits(void)
{
    unsigned char data_fuse[7];
    unsigned char informasi_fuse;
    unsigned char i;
    bit status_data_fuse;
    bit status_low_fuse;
    bit status_high_fuse;
    bit status_extended_fuse;
        #asm("cli") ; 
    status_data_fuse= 0;
    status_low_fuse= 0;
    status_high_fuse= 0;        
    status_extended_fuse= 0;
    for(i=0;i<7;i++) data_fuse[i]= bacaExtSRAM(i); 
    informasi_fuse= data_fuse[0];
    switch(informasi_fuse)
    {
        case 0x00:
             {
                status_low_fuse= 1;
                status_data_fuse= 1;
             }
             break;
        case 0x01:
             {
                status_low_fuse= 1;
                status_high_fuse= 1;
                status_data_fuse= 1;
             }
             break;
        case 0x02:
             {
                status_low_fuse= 1;
                status_high_fuse= 1;        
                status_extended_fuse= 1;
                status_data_fuse= 1;
             }
             break; 
        default : status_data_fuse= 0;   
    }
        if(status_data_fuse)   
    { 
        PORTD &= ~(1<<3);
        /*** Set SCK dengan frekuensi rendah ***/ 
        inisialisasiSPILowSpeed(); 
        enterProgrammingMode();
        if(status_low_fuse) ISPWriteLowFuseBits(data_fuse[2]);
        if(status_high_fuse) ISPWriteHighFuseBits(data_fuse[4]);
        if(status_extended_fuse) ISPWriteExtendedBits(data_fuse[6]);
        leaveProgrammingMode();
        PORTD |= (1<<3)    ; 
    }
    #asm("sei");  
} 
/************************************************************************************************************/   
// Fungsi Hapus Chip
void hapusMemoriDevais(void)
{
    #asm("cli") ; 
    enterProgrammingMode();   
    chipErase();     
    leaveProgrammingMode();
    #asm("sei");
}
/************************************************************************************************************/
// Fungsi Menu ISP (Baca dan Tulis Memori)
void menuISP(unsigned char menu_pilihan)
{
	status_paket= 0;                                                                              
	switch(menu_pilihan)
	{
		case 0x66  :
			bacaMemoriFlash();	
			break;
		case 0x46  : 
			tulisMemoriFlash();
			break;  
		case 0x65  :
			bacaEEPROM();	
			break;
		case 0x45  : 
			tulisEEPROM();
			break;
		case 0x62  :
		     bacaFuseBits();
		     break;  
		case 0x42  :
		     tulisFuseBits();
		     break;
		case 0x48  :
		     hapusMemoriDevais();
		     break;
	}
} 
/************************************************************************************************************/ 
void olahPaketPerintah(void)
{
    bit selesai_terima_perintah;
    unsigned char data;
    unsigned char buffer_paket_data[11];   // Ukuran buffer adalah [HEADER(7) + Data(kode devais/jumlah paket) + CRC(2)] 
        while(!selesai_terima_perintah)
    {
        data= bacaDataSerialIR();
        //kirimDataSerial(data); //Debug log serial USB
        buffer_paket_data[indek_data++]= data; 
                if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
            indek_data= 0;      
        else if(indek_data == 5 && data != '|')
        	indek_data= 0;
        else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
            time1= 68 ; 
        else if(indek_data == 11)  // Deteksi awal kiriman paket
        {
              CRC= hitungCRC16(buffer_paket_data, 9);
              if(((unsigned char) CRC == buffer_paket_data[9]) && ((unsigned char) (CRC>>8) == buffer_paket_data[10]))
              {
                  instruksi_pemrogram= buffer_paket_data[3];
                  //Operasi Tulis dan Hapus
                  if(instruksi_pemrogram==0x46 || instruksi_pemrogram==0x45 || instruksi_pemrogram==0x42 || instruksi_pemrogram==0x48)
                  {
                      PORTD |= (1<<3)    ;
                      PORTE &= ~(1<<0);
        			                          	  if(instruksi_pemrogram!=0x48)    // Bukan operasi hapus
                      {
                          jml_paket= buffer_paket_data[7];
                          status_paket= 1;
                      	  kirimKonfirmasiOK();  
                          ID_paket= 1;
                          status_perintah_tulis= 1;
                          selesai_terima_perintah= 1;  // Status perlu simpan data yang akan ditulis
                          jml_byte_data= (unsigned int) jml_paket*128   ;  
                          if(jml_byte_data>1024*8                        ) status_tulis_bertahap= 1; //Deteksi data ukuran besar   
                      } 
                      else                             // Operasi hapus tidak perlu data transmisi lagi
        			  {
                          status_paket= 1;
                          kirimKonfirmasiOK();      
                          selesai_terima_perintah= 1;
                          status_perintah_tulis= 0;
        			  }
    			 }  
    			     			 // Operasi Baca
    			 else if (instruksi_pemrogram==0x66 || instruksi_pemrogram==0x65 || instruksi_pemrogram==0x62)
    			 {
                     PORTD |= (1<<3)    ;
                     PORTE &= ~(1<<0);
                                          kode_devais[0]= buffer_paket_data[5];  
                     kode_devais[1]= buffer_paket_data[6];
                     kode_devais[2]= buffer_paket_data[7];
                     ID_paket= 1;
                     selesai_terima_perintah= 1; 
                                          if(kode_devais[0]==devais_signature[0] && kode_devais[1]==devais_signature[1] && kode_devais[2]==devais_signature[2])
                     {
                         ID_perintah= instruksi_pemrogram;
                         status_paket= 1;
                         kirimKonfirmasiOK();
                         status_perintah_tulis= 0;
                     }
                     else
                     {
                         selesai_terima_perintah= 0;
                         status_paket= 2;
                         kirimKonfirmasiGAGAL();
                         inisialisasi(); 
                     }
                }
			}
			indek_data= 0;
		}
    };
}     
/************************************************************************************************************/   
void olahPaketData(void)
{
    bit selesai_terima_data;
    unsigned char data;  
    unsigned char i; 
    unsigned char buffer_paket_data[128   +9];   //Ukuran buffer adalah [HEADER(7)+ DATA(PANJANG_PAKET)+CRC(2)] 
    unsigned int jml_data_page;                         //Jumlah data page yang akan ditulis ke flash
    unsigned char jml_paket_sram_tertulis;              //Counter paket sram yang tertulis pada flash  
        if(!status_tulis_bertahap) //Data yang di tulis antara 1 sampai dengan 8 kbyte (KAPASITAS_SRAM)
    {
        while(!selesai_terima_data)
        {
            data= bacaDataSerialIR();
            //kirimDataSerial(data); //Debug log serial USB
            buffer_paket_data[indek_data++]= data; 
                        if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
                indek_data= 0;      
            else if(indek_data == 5 && data != '|')
            	indek_data= 0;
            else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
                time1= 68 ;  
            else if((indek_data == 128   +9) && ID_paket <= jml_paket) //Penerimaan Data 
    		{
    			CRC= hitungCRC16(buffer_paket_data, 128   +7);
        			if(((unsigned char) CRC == buffer_paket_data[128   +7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[128   +8]))
    			{
    				if(ID_paket == buffer_paket_data[5])
    				{
    					ID_paket_OK= ID_paket;     
    					status_paket= 3;
    					//Tampung data yang diterima ke Eksternal SRAM mulai dari indek ke-7
    					for(i= 7; i<(128   +7); i++)
    						tulisExtSRAM(idxBufExtSRAM++, buffer_paket_data[i]);     
    					    					ID_paket++;
    					kirimKonfirmasiPaketOK();	
    					if(ID_paket == (jml_paket+1))
    						selesai_terima_data= 1;
    				}
    				else
    				{
    					status_paket= 2;
    					kirimKonfirmasiGAGAL(); 
    				}
    			}
    			else
    			{
    				status_paket= 2;
    				kirimKonfirmasiGAGAL();  
    			}
    			indek_data= 0;
    		}   
        }; 
    }
        else if(status_tulis_bertahap) //Data yang di tulis melebihi KAPASITAS_SRAM
    {
        while(!selesai_terima_data)
        {
            PORTB.7=0;          //Jaga kondisi SCK tanpa Pulsa
            data= bacaDataSerialIR();
            //kirimDataSerial(data); //Debug log serial USB
            buffer_paket_data[indek_data++]= data; 
                        if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
                indek_data= 0;      
            else if(indek_data == 5 && data != '|')
            	indek_data= 0;
            else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
                time1= 68 ;  
            else if((indek_data == 128   +9) && ID_paket <= jml_paket) //Penerimaan Data 
    		{
    			CRC= hitungCRC16(buffer_paket_data, 128   +7);
        			if(((unsigned char) CRC == buffer_paket_data[128   +7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[128   +8]))
    			{
    				if(ID_paket == buffer_paket_data[5])
    				{
    					ID_paket_OK= ID_paket;     
    					status_paket= 3;
    					//Tampung data yang diterima ke Eksternal SRAM mulai dari indek ke-7
    					for(i= 7; i<(128   +7); i++)
    						    tulisExtSRAM(idxBufExtSRAM++, buffer_paket_data[i]); 
    						    					jml_data_page= (unsigned int) idxBufExtSRAM/2;	
    					//Tulis jika isi buffer SRAM penuh maka segera tulis ke Flash
    					if(idxBufExtSRAM>1024*8                        )
    					{
        					idxBufExtSRAM= 0;      //Reset indek buffer
        					status_paket= 2;
        					kirimKonfirmasiGAGAL(); 
        					PORTD &= ~(1<<3); 
        					alamat_akhir= ISPWriteHugeFlashMemory(jml_data_page, alamat_awal);	 
        					alamat_awal= alamat_akhir;
        					PORTD |= (1<<3)    ;
    					} 
    					    					    					ID_paket++;
    					kirimKonfirmasiPaketOK();	
    					if(ID_paket == (jml_paket+1))
    						selesai_terima_data= 1;
    				}
    				else
    				{
    					status_paket= 2;
    					kirimKonfirmasiGAGAL(); 
    				}
    			}
    			else
    			{
    				status_paket= 2;
    				kirimKonfirmasiGAGAL();  
    			}
    			indek_data= 0;
    		}   
        };
    }
}
/************************************************************************************************************/
void olahPaketAkhir(void)
{
    bit selesai_terima_akhir;
    unsigned char data;
    unsigned char buffer_paket_data[11];   // Ukuran buffer adalah [HEADER(7) + Data + CRC(2)] 
                                           // Data dapat berupa kode devais atau jumlah paket 
        while(!selesai_terima_akhir)
    {
        data= bacaDataSerialIR();
        //kirimDataSerial(data); //Debug log serial USB
        buffer_paket_data[indek_data++]= data; 
                if((indek_data == 1 && data != '<') || (indek_data == 2 && data != 0x45) || (indek_data == 3 && data != '>'))
            indek_data= 0;      
        else if(indek_data == 5 && data != '|')
        	indek_data= 0;
        else if(indek_data == 1)  // Tanda  paket mulai valid, set nilai timeout
            time1= 68 ; 
        else if(status_perintah_tulis && indek_data == 9)// Deteksi akhir kiriman
		{
			CRC= hitungCRC16(buffer_paket_data, 7);
						if(((unsigned char) CRC == buffer_paket_data[7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[8]))
			{
				if((buffer_paket_data[3] == '\n') && (buffer_paket_data[5] == '\r')) 
      				selesai_terima_akhir= 1;      
				else
				{
					status_paket= 2;
					kirimKonfirmasiGAGAL();
				}
			}
			indek_data= 0;
		}
    };
}
/************************************************************************************************************/  
void terimaDataDariPemrogram(void)
{
    olahPaketPerintah();
    if(status_perintah_tulis)
    { 
        olahPaketData();
        olahPaketAkhir();  
    }
} 
/************************************************************************************************************/
// Fungsi Main
void main (void)
{
    unsigned char i; 
        // Crystal Oscillator division factor: 1
    #pragma optsize-
    (*(unsigned char *) 0x61)=0x80;
    (*(unsigned char *) 0x61)=0x00;
                    // Input/Output Ports initialization
    // Port A initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTA=0x00;
    DDRA=0x00;
        // Port B initialization
    // Func7=Out Func6=In Func5=Out Func4=Out Func3=In Func2=In Func1=Out Func0=Out 
    // State7=0 State6=T State5=0 State4=0 State3=T State2=T State1=0 State0=0 
    PORTB=0x10;
    DDRB=0xB3;
        // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTC=0x00;
    DDRC=0x00;
        // Port D initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTD=0b11110011;
    DDRD=0b11001110;
        // Port E initialization
    // Func2=In Func1=In Func0=In 
    // State2=T State1=T State0=T 
    PORTE=0x01;
    DDRE=0x01;
        // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR=0x80;  
        //Inisialisasi Hardware
    inisialisasiUSART();    
    inisialisasiSPILowSpeed();
    inisialisasiSPIHighSpeed(); 
    inisialisasiTimer();
    inisialisasiExtSRAM();
    PORTD &= ~(1<<2);  
        //Pengkondisian awal transmisi
    UCSR0B|= (1<<3); 
    TCCR2= 0x19;                                                  
    UCSR0B|= (1<<4);
    UCSR0B|= (1<<7  ); 
        for(;;) 
    {
        inisialisasi();
        terimaDataDariPemrogram();
        if(!status_tulis_bertahap) menuISP(instruksi_pemrogram); 
        //Penulisan bertahap untuk flash berukuran besar melebihi kapasitas SRAM
        else if(status_tulis_bertahap)
        {
    	    status_paket= 0;
        	jml_page_data= (unsigned int) (idxBufExtSRAM/2); //Benar bisa jalan OK  
        	PORTD &= ~(1<<3); 
        	alamat_akhir= ISPWriteHugeFlashMemory(jml_page_data, alamat_awal);	
        	PORTD |= (1<<3)    ;
    	}  
        status_paket= 5    ; //Set penanda akhir proses  
        kirimKonfirmasiIDLE();  
        for(i=0;i<4;i++)
        {
            PORTE &= ~(1<<0);
            PORTD |= (1<<3)    ;
            delay_ms(200);
            PORTE |= (1<<0)  ;
            PORTD &= ~(1<<3); 
            delay_ms(200);
        };
    };
}  
