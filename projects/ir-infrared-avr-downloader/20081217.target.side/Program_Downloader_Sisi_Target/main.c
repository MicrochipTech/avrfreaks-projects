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

#include <mega162.h>
#include <delay.h>
#include "crc/crc.h"  
#include "debug/debug.h"
#include "eeprom/eeprom.h" 
#include "extsram/extsram.h"
#include "flash/flash.h"  
#include "fuse/fuse.h" 
#include "isp/isp.h" 
#include "timer/timer.h" 
#include "usart/usart.h"  

#define TDK_PERLU_KONFIRMASI	0
#define KONFIRMASI_OK			1
#define KONFIRMASI_GAGAL		2
#define KONFIRMASI_PAKET_OK		3
#define KONFIRMASI_IDLE			4 
#define AKHIR_PROSES            5    

#define TULIS_FLASH_MEMORY		0x46  //'F'   
#define BACA_FLASH_MEMORY		0x66  //'f'  
#define TULIS_EEPROM     		0x45  //'E'   
#define BACA_EEPROM     		0x65  //'e'  
#define TULIS_FUSE_BITS    		0x42  //'B'   
#define BACA_FUSE_BITS     		0x62  //'b'
#define HAPUS_CHIP              0x48  //'H'

#define PANJANG_PAKET           128   //64  //32  

//Kapasitas SRAM maksimal yang digunakan pada perangkat
//sisi target adalah 8 kbyte
#define FAKTOR_PENGALI          8                     //8 --> 8 KByte
#define KAPASITAS_SRAM          1024*FAKTOR_PENGALI   //Dalam orde kbyte

#define SEI()                   #asm("sei")
#define CLI()                   #asm("cli") 

//D-Latch
#define AKTIFKAN_LATCH()        PORTD &= ~(1<<2)
#define NON_AKTIFKAN_LATCH()    PORTD |= (1<<2)

#define LED_BIRU_ON()           PORTD &= ~(1<<3)
#define LED_BIRU_OFF()          PORTD |= (1<<3)    
#define LED_MERAH_ON()          PORTE &= ~(1<<0)
#define LED_MERAH_OFF()         PORTE |= (1<<0)  

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
unsigned char data_load[PANJANG_PAKET];                  //Menyimpan data yang akan diload ke bufferPaketData 
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
#define RX_BUFFER_SIZE0 256
char rx_buffer0[RX_BUFFER_SIZE0];
unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
/************************************************************************************************************/
// USART0 Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
    unsigned char status,data;  
    status=UCSR0A;
    data=UDR0;
    if ((status & ((FRAMING_ERROR)|(DATA_OVERRUN)))==0)
       {
           rx_buffer0[rx_wr_index0]= data;
           if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0= 0;
           if (++rx_counter0 == RX_BUFFER_SIZE0) rx_counter0= 0;
       }
}
/************************************************************************************************************/
// Fungsi Pembacaan Data Serial pada Buffer Interrupt RXC
unsigned char bacaDataSerialIR(void)
{
    unsigned char data;
    
    while (rx_counter0==0);
    data= rx_buffer0[rx_rd_index0];
    if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0= 0;
    CLI();
    --rx_counter0;
    SEI();                                                                            
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
    	indek_data= (unsigned int)indek_paket*PANJANG_PAKET;
    	
    	for(i=0;i<PANJANG_PAKET;i++)
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
    	indek_data= (unsigned int)indek_paket*PANJANG_PAKET;
    	
    	for(i=0;i<PANJANG_PAKET;i++)
        	data_load[i]= bacaExtSRAM(indek_data++);     //Ambil data dari SRAM Eksternal
    }
    if(jml_byte_data>KAPASITAS_SRAM) 
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
      	
    CLI();
    
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
      	
    SEI();
} 
/************************************************************************************************************/  
// Fungsi Pengiriman Paket Data Transmisi 
void kirimPaketData(void)
{
    unsigned char i, j;
    unsigned char buffer_paket_data[PANJANG_PAKET+9];      // Ukuran buffer adalah [HEADER(7)+ DATA(PANJANG_PAKET)+CRC(2)]
	
    CLI();
	
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
	for(i=0; i<PANJANG_PAKET; i++)
	{
		j= i+7;
		buffer_paket_data[j]= data_load[i];
	};

    CRC= hitungCRC16(buffer_paket_data, (PANJANG_PAKET+7));           //HEADER(7)+DATA(PANJANG_PAKET)
    buffer_paket_data[PANJANG_PAKET+7]= (unsigned char)  CRC;         //HEADER(7)+DATA(PANJANG_PAKET)+CRC(low byte)
    buffer_paket_data[PANJANG_PAKET+8]= (unsigned char) (CRC>>8);     //HEADER(7)+DATA(PANJANG_PAKET)+CRC(high byte)
	
    //Persiapan pengiriman data 
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    kirimDataSerialIR(0x00);
    
    for(i= 0; i <= (PANJANG_PAKET+8) ; i++)
        kirimDataSerialIR(buffer_paket_data[i]);
	
	SEI(); 
}
/************************************************************************************************************/  
// Fungsi Pengiriman Paket Akhir Transmisi 
void kirimPaketAkhir(void)
{
	unsigned char i; 
	unsigned char buffer_paket_data[9];
	
	CLI();  
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
	
	SEI(); 
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
	MATIKAN_RECEIVER_INTERRUPT_IR();
	MATIKAN_RECEIVER_IR();
	CLI();
	
	kirimDataSerialIR(0x00);  
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	
	kirimDataSerialIR('|');
	kirimDataSerialIR('\n');
	kirimDataSerialIR('|');
	kirimDataSerialIR('\r');
	
	SEI();       
	AKTIFKAN_RECEIVER_IR();                   
	AKTIFKAN_RECEIVER_INTERRUPT_IR();
}
/************************************************************************************************************/
// Fungsi Pengiriman Konfirmasi Paket OK 
void kirimKonfirmasiPaketOK(void)
{
	MATIKAN_RECEIVER_INTERRUPT_IR();
	MATIKAN_RECEIVER_IR();
	CLI();
	
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
	
	SEI();               
	AKTIFKAN_RECEIVER_IR();           
	AKTIFKAN_RECEIVER_INTERRUPT_IR();
}
/************************************************************************************************************/
// Fungsi Pengiriman Konfirmasi OK 
void kirimKonfirmasiOK(void)
{	
	MATIKAN_RECEIVER_INTERRUPT_IR();
	MATIKAN_RECEIVER_IR();
	CLI();
	
	kirimDataSerialIR(0x00);  
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	kirimDataSerialIR(0x00);
	
	kirimDataSerialIR('|');
	kirimDataSerialIR(0xAA);
	kirimDataSerialIR('|');
	kirimDataSerialIR(0xAA);
	
	SEI();               
	AKTIFKAN_RECEIVER_IR();           
	AKTIFKAN_RECEIVER_INTERRUPT_IR(); 
}
/************************************************************************************************************/
// Fungsi Pengiriman Konfirmasi Paket Gagal
void kirimKonfirmasiGAGAL(void)
{
	MATIKAN_RECEIVER_INTERRUPT_IR();
	MATIKAN_RECEIVER_IR();
	CLI();
	
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
	
	SEI();               
	AKTIFKAN_RECEIVER_IR();           
	AKTIFKAN_RECEIVER_INTERRUPT_IR();
}
/************************************************************************************************************/
// Timer 1 output compare A interrupt service routine  
// Interrupt setiap 20 ms
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
      TCNT1L= 0x00;
      TCNT1H= 0x00;
      	
      if(time1 > 0)
      --time1;
      	
      if(time1 == 0)
      {
            time1= USART_TIMEOUT;
            		
            if(status_paket == KONFIRMASI_GAGAL)
            {   
                  //Reset indek data pada buffer data sementara
                  indek_data= 0;
                  kirimKonfirmasiGAGAL();
            }
            else if(status_paket == KONFIRMASI_OK)
                  kirimKonfirmasiOK();
            else if(status_paket == KONFIRMASI_PAKET_OK)
                  kirimKonfirmasiPaketOK();
            else if(status_paket == AKHIR_PROSES)
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
    time1= USART_TIMEOUT;
	
    //Status paket data yang diterima
    status_paket= TDK_PERLU_KONFIRMASI; 
	
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
    LED_MERAH_OFF();
    LED_BIRU_OFF();
    
    //Cek eksistensi devais yang terpasang pada sisi target
    eksistensi_devais= cekEksistensiDevais();  
    if(eksistensi_devais) LED_BIRU_ON();  
	
    //Baca Devais Signature untuk dibandingkan dengan kode devais dari pemrogram
    ISPReadDevaisSignature(); 
    
    //Aktifkan Global Interrupt Enable
    SEI();
}  
/************************************************************************************************************/
// Fungsi pengiriman data ke pemrogram                   
void kirimDataKePemrogram(void)
{        
    MATIKAN_SCHEDULLING();
    MATIKAN_RECEIVER_INTERRUPT_IR(); 
    AKTIFKAN_RECEIVER_IR(); 
    
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
        AKTIFKAN_RECEIVER_IR(); 
        AKTIFKAN_RECEIVER_INTERRUPT_IR();
        olahKonfirmasiPaketPerintahDanAkhir();
        MATIKAN_RECEIVER_INTERRUPT_IR(); 
        MATIKAN_RECEIVER_IR();
    };
    //Pengiriman Paket Data  
    while(status_paket_data==1)
    {      
        //debugging program
        kirimDataSerial(ID_paket);  //Debug log serial USB
        kirimPaketData();
        tunggu_konfirmasi= 1;
        AKTIFKAN_RECEIVER_IR();
        AKTIFKAN_RECEIVER_INTERRUPT_IR(); 
        olahKonfirmasiPaketData(); 
        MATIKAN_RECEIVER_INTERRUPT_IR();
        MATIKAN_RECEIVER_IR();
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
        AKTIFKAN_RECEIVER_IR(); 
        AKTIFKAN_RECEIVER_INTERRUPT_IR();    
        olahKonfirmasiPaketPerintahDanAkhir(); 
        MATIKAN_RECEIVER_INTERRUPT_IR(); 
        MATIKAN_RECEIVER_IR();
    };  
        
    AKTIFKAN_SCHEDULLING();
}   
/************************************************************************************************************/
// Fungsi pengiriman data berukuran besar ke pemrogram                   
void kirimDataFlashBesarKePemrogram(void)
{        
    unsigned char counter= 1;
    
    MATIKAN_SCHEDULLING();
    MATIKAN_RECEIVER_INTERRUPT_IR(); 
    AKTIFKAN_RECEIVER_IR(); 
    
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
        AKTIFKAN_RECEIVER_IR(); 
        AKTIFKAN_RECEIVER_INTERRUPT_IR();
        olahKonfirmasiPaketPerintahDanAkhir();
        MATIKAN_RECEIVER_INTERRUPT_IR(); 
        MATIKAN_RECEIVER_IR();
    };
    //Pengiriman Paket Data  
    while(status_paket_data==1)
    {      
        if(status_baca_bertahap)  
        {
            LED_BIRU_ON();
            alamat_akhir= ISPReadHugeFlashMemory(KAPASITAS_SRAM, alamat_awal);
            alamat_awal= alamat_akhir;
            status_data_baru_sram= 1;
            LED_BIRU_OFF();
        }
        while(status_data_baru_sram)
        {       
            //debugging program
            //kirimDataSerial(counter++);  //Debug log serial USB
            kirimPaketData();
            tunggu_konfirmasi= 1;
            AKTIFKAN_RECEIVER_IR();
            AKTIFKAN_RECEIVER_INTERRUPT_IR(); 
            olahKonfirmasiPaketData(); 
            MATIKAN_RECEIVER_INTERRUPT_IR();
            MATIKAN_RECEIVER_IR();
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
        AKTIFKAN_RECEIVER_IR(); 
        AKTIFKAN_RECEIVER_INTERRUPT_IR();    
        olahKonfirmasiPaketPerintahDanAkhir(); 
        MATIKAN_RECEIVER_INTERRUPT_IR(); 
        MATIKAN_RECEIVER_IR();
    };  
        
    AKTIFKAN_SCHEDULLING();
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
    CLI();    
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
    
    CLI();    
    jml_page_data= (unsigned int) (idxBufExtSRAM/2); //Benar bisa jalan OK  
    LED_BIRU_ON(); 
    enterProgrammingMode();   
    chipErase();     
    ISPWriteFlashMemory(jml_page_data);
    leaveProgrammingMode();  
    LED_BIRU_OFF();
    SEI();
} 
/************************************************************************************************************/
// Fungsi Tulis EEPROM, data dari kiriman pemrogram
void tulisEEPROM(void)
{
    unsigned int jml_byte_data;
    jml_byte_data= 0;
     
    CLI();    
    jml_byte_data= idxBufExtSRAM;
    LED_BIRU_ON(); 
    enterProgrammingMode();      
    ISPWriteEEPROM(jml_byte_data);
    leaveProgrammingMode();
    LED_BIRU_OFF();
    SEI();
}
/************************************************************************************************************/
// Fungsi Baca Flash, hasil pembacaan kirim ke pemrogram
void bacaMemoriFlash(void)
{
    unsigned int flash_page_max;
    unsigned int ukuran_flash_max; 

    CLI();    
    //Baca flash memory kemudian data disimpan dalam SRAM Eksternal 
    ukuran_flash_max= bacaUkuranFlash();
    flash_page_max= ukuran_flash_max/2; //OK sudah bisa 
    jml_paket= (unsigned char) (ukuran_flash_max/PANJANG_PAKET);
    if(ukuran_flash_max>KAPASITAS_SRAM) status_tulis_bertahap= 1; 
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
        LED_BIRU_ON();
        enterProgrammingMode();   
        ISPReadFlashMemory(flash_page_max);   
        leaveProgrammingMode();
        LED_BIRU_OFF();
        //debugging program  
        kirimDataSerial(0x22); //Debug log serial USB
        kirimDataKePemrogram();
    } 
    SEI();  
}
/************************************************************************************************************/
// Fungsi Baca EEPROM, hasil pembacaan kirim ke pemrogram
void bacaEEPROM(void)
{
    unsigned int ukuran_flash_max;
    unsigned int EEPROM_size_max;

    CLI();  
    //Baca EEPROM memory kemudian data disimpan dalam SRAM Eksternal   
    ukuran_flash_max= bacaUkuranFlash();
    EEPROM_size_max= (unsigned int)(ukuran_flash_max/16); //OK sudah bisa   
    LED_BIRU_ON();
    enterProgrammingMode();   
    ISPReadEEPROM(EEPROM_size_max); 
    leaveProgrammingMode();
    LED_BIRU_OFF();
    SEI();
    
    //Pengiriman data pembacaan EEPROM ke sisi pemrogram
    jml_paket= (unsigned int)(EEPROM_size_max/PANJANG_PAKET);
    kirimDataKePemrogram();    
}
/************************************************************************************************************/ 
// Fungsi Baca Fuse Bits
void bacaFuseBits(void)
{
    CLI(); 
    LED_BIRU_ON();
    /*** Set SCK dengan frekuensi rendah ***/ 
    inisialisasiSPILowSpeed(); 
    enterProgrammingMode(); 
    fuse_bits[0]= ISPReadLowFuseBits();  
    fuse_bits[1]= ISPReadHighFuseBits();   
    fuse_bits[2]= ISPReadExtendedFuseBits();
    leaveProgrammingMode();
    LED_BIRU_OFF();
    SEI();  
    
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
    
    CLI(); 
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
        LED_BIRU_ON();
        /*** Set SCK dengan frekuensi rendah ***/ 
        inisialisasiSPILowSpeed(); 
        enterProgrammingMode();
        if(status_low_fuse) ISPWriteLowFuseBits(data_fuse[2]);
        if(status_high_fuse) ISPWriteHighFuseBits(data_fuse[4]);
        if(status_extended_fuse) ISPWriteExtendedBits(data_fuse[6]);
        leaveProgrammingMode();
        LED_BIRU_OFF(); 
    }
    SEI();  
} 
/************************************************************************************************************/   
// Fungsi Hapus Chip
void hapusMemoriDevais(void)
{
    CLI(); 
    enterProgrammingMode();   
    chipErase();     
    leaveProgrammingMode();
    SEI();
}
/************************************************************************************************************/
// Fungsi Menu ISP (Baca dan Tulis Memori)
void menuISP(unsigned char menu_pilihan)
{
	status_paket= TDK_PERLU_KONFIRMASI;                                                                              
	switch(menu_pilihan)
	{
		case BACA_FLASH_MEMORY:
			bacaMemoriFlash();	
			break;
		case TULIS_FLASH_MEMORY: 
			tulisMemoriFlash();
			break;  
		case BACA_EEPROM:
			bacaEEPROM();	
			break;
		case TULIS_EEPROM: 
			tulisEEPROM();
			break;
		case BACA_FUSE_BITS:
		     bacaFuseBits();
		     break;  
		case TULIS_FUSE_BITS:
		     tulisFuseBits();
		     break;
		case HAPUS_CHIP:
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
            time1= USART_TIMEOUT; 
        else if(indek_data == 11)  // Deteksi awal kiriman paket
        {
              CRC= hitungCRC16(buffer_paket_data, 9);
              if(((unsigned char) CRC == buffer_paket_data[9]) && ((unsigned char) (CRC>>8) == buffer_paket_data[10]))
              {
                  instruksi_pemrogram= buffer_paket_data[3];
                  //Operasi Tulis dan Hapus
                  if(instruksi_pemrogram==0x46 || instruksi_pemrogram==0x45 || instruksi_pemrogram==0x42 || instruksi_pemrogram==0x48)
                  {
                      LED_BIRU_OFF();
                      LED_MERAH_ON();
        			        
                  	  if(instruksi_pemrogram!=0x48)    // Bukan operasi hapus
                      {
                          jml_paket= buffer_paket_data[7];
                          status_paket= KONFIRMASI_OK;
                      	  kirimKonfirmasiOK();  
                          ID_paket= 1;
                          status_perintah_tulis= 1;
                          selesai_terima_perintah= 1;  // Status perlu simpan data yang akan ditulis
                          jml_byte_data= (unsigned int) jml_paket*PANJANG_PAKET;  
                          if(jml_byte_data>KAPASITAS_SRAM) status_tulis_bertahap= 1; //Deteksi data ukuran besar   
                      } 
                      else                             // Operasi hapus tidak perlu data transmisi lagi
        			  {
                          status_paket= KONFIRMASI_OK;
                          kirimKonfirmasiOK();      
                          selesai_terima_perintah= 1;
                          status_perintah_tulis= 0;
        			  }
    			 }  
    			 
    			 // Operasi Baca
    			 else if (instruksi_pemrogram==0x66 || instruksi_pemrogram==0x65 || instruksi_pemrogram==0x62)
    			 {
                     LED_BIRU_OFF();
                     LED_MERAH_ON();
                     
                     kode_devais[0]= buffer_paket_data[5];  
                     kode_devais[1]= buffer_paket_data[6];
                     kode_devais[2]= buffer_paket_data[7];
                     ID_paket= 1;
                     selesai_terima_perintah= 1; 
                     
                     if(kode_devais[0]==devais_signature[0] && kode_devais[1]==devais_signature[1] && kode_devais[2]==devais_signature[2])
                     {
                         ID_perintah= instruksi_pemrogram;
                         status_paket= KONFIRMASI_OK;
                         kirimKonfirmasiOK();
                         status_perintah_tulis= 0;
                     }
                     else
                     {
                         selesai_terima_perintah= 0;
                         status_paket= KONFIRMASI_GAGAL;
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
    unsigned char buffer_paket_data[PANJANG_PAKET+9];   //Ukuran buffer adalah [HEADER(7)+ DATA(PANJANG_PAKET)+CRC(2)] 
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
                time1= USART_TIMEOUT;  
            else if((indek_data == PANJANG_PAKET+9) && ID_paket <= jml_paket) //Penerimaan Data 
    		{
    			CRC= hitungCRC16(buffer_paket_data, PANJANG_PAKET+7);
    
    			if(((unsigned char) CRC == buffer_paket_data[PANJANG_PAKET+7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[PANJANG_PAKET+8]))
    			{
    				if(ID_paket == buffer_paket_data[5])
    				{
    					ID_paket_OK= ID_paket;     
    					status_paket= KONFIRMASI_PAKET_OK;
    					//Tampung data yang diterima ke Eksternal SRAM mulai dari indek ke-7
    					for(i= 7; i<(PANJANG_PAKET+7); i++)
    						tulisExtSRAM(idxBufExtSRAM++, buffer_paket_data[i]);     
    					
    					ID_paket++;
    					kirimKonfirmasiPaketOK();	
    					if(ID_paket == (jml_paket+1))
    						selesai_terima_data= 1;
    				}
    				else
    				{
    					status_paket= KONFIRMASI_GAGAL;
    					kirimKonfirmasiGAGAL(); 
    				}
    			}
    			else
    			{
    				status_paket= KONFIRMASI_GAGAL;
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
                time1= USART_TIMEOUT;  
            else if((indek_data == PANJANG_PAKET+9) && ID_paket <= jml_paket) //Penerimaan Data 
    		{
    			CRC= hitungCRC16(buffer_paket_data, PANJANG_PAKET+7);
    
    			if(((unsigned char) CRC == buffer_paket_data[PANJANG_PAKET+7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[PANJANG_PAKET+8]))
    			{
    				if(ID_paket == buffer_paket_data[5])
    				{
    					ID_paket_OK= ID_paket;     
    					status_paket= KONFIRMASI_PAKET_OK;
    					//Tampung data yang diterima ke Eksternal SRAM mulai dari indek ke-7
    					for(i= 7; i<(PANJANG_PAKET+7); i++)
    						    tulisExtSRAM(idxBufExtSRAM++, buffer_paket_data[i]); 
    						
    					jml_data_page= (unsigned int) idxBufExtSRAM/2;	
    					//Tulis jika isi buffer SRAM penuh maka segera tulis ke Flash
    					if(idxBufExtSRAM>KAPASITAS_SRAM)
    					{
        					idxBufExtSRAM= 0;      //Reset indek buffer
        					status_paket= KONFIRMASI_GAGAL;
        					kirimKonfirmasiGAGAL(); 
        					LED_BIRU_ON(); 
        					alamat_akhir= ISPWriteHugeFlashMemory(jml_data_page, alamat_awal);	 
        					alamat_awal= alamat_akhir;
        					LED_BIRU_OFF();
    					} 
    					    					
    					ID_paket++;
    					kirimKonfirmasiPaketOK();	
    					if(ID_paket == (jml_paket+1))
    						selesai_terima_data= 1;
    				}
    				else
    				{
    					status_paket= KONFIRMASI_GAGAL;
    					kirimKonfirmasiGAGAL(); 
    				}
    			}
    			else
    			{
    				status_paket= KONFIRMASI_GAGAL;
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
            time1= USART_TIMEOUT; 
        else if(status_perintah_tulis && indek_data == 9)// Deteksi akhir kiriman
		{
			CRC= hitungCRC16(buffer_paket_data, 7);
			
			if(((unsigned char) CRC == buffer_paket_data[7]) && ((unsigned char) (CRC>>8) == buffer_paket_data[8]))
			{
				if((buffer_paket_data[3] == '\n') && (buffer_paket_data[5] == '\r')) 
      				selesai_terima_akhir= 1;      
				else
				{
					status_paket= KONFIRMASI_GAGAL;
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
    CLKPR=0x80;
    CLKPR=0x00;
    #ifdef _OPTIMIZE_SIZE_
    #pragma optsize+
    #endif
    
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
    AKTIFKAN_LATCH();  
    
    //Pengkondisian awal transmisi
    AKTIFKAN_TRANSMITTER_IR(); 
    AKTIFKAN_SINYAL_CARRIER();                                                  
    AKTIFKAN_RECEIVER_IR();
    AKTIFKAN_RECEIVER_INTERRUPT_IR(); 
    
    for(;;) 
    {
        inisialisasi();
        terimaDataDariPemrogram();
        if(!status_tulis_bertahap) menuISP(instruksi_pemrogram); 
        //Penulisan bertahap untuk flash berukuran besar melebihi kapasitas SRAM
        else if(status_tulis_bertahap)
        {
    	    status_paket= TDK_PERLU_KONFIRMASI;
        	jml_page_data= (unsigned int) (idxBufExtSRAM/2); //Benar bisa jalan OK  
        	LED_BIRU_ON(); 
        	alamat_akhir= ISPWriteHugeFlashMemory(jml_page_data, alamat_awal);	
        	LED_BIRU_OFF();
    	}  
        status_paket= AKHIR_PROSES; //Set penanda akhir proses  
        kirimKonfirmasiIDLE();  
        for(i=0;i<4;i++)
        {
            LED_MERAH_ON();
            LED_BIRU_OFF();
            delay_ms(200);
            LED_MERAH_OFF();
            LED_BIRU_ON(); 
            delay_ms(200);
        };
    };
}  