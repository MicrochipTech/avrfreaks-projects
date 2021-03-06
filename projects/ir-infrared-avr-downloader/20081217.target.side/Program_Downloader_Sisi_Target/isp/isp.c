#include "isp.h"

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
    while(!(SPSR & (1<<SPIF)));      
    return SPDR;
} 

void enterProgrammingMode(void)
{
    /*** Instruksi Programming Enable pada Serial Programming ***/
    //Reset harus diberi pulsa positif minimal selama 2 siklus clock CPU setelah pin SCK diset 0 
    SCK=0;  
    RESET_SLAVE_NONAKTIF();
    delay_ms(1);
    RESET_SLAVE_AKTIF(); 
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
    RESET_SLAVE_NONAKTIF();
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