#include "flash.h"                   
#include "extsram/extsram.h" 
#include "isp/isp.h"

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
    ulang_polling_flash=ULANG_POLLING_FLASH_MAX;  
      
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
    ulang_polling_flash=ULANG_POLLING_FLASH_MAX;
    
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