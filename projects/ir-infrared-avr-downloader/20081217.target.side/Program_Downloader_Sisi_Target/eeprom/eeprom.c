#include "eeprom.h"
#include "extsram/extsram.h" 
#include "isp/isp.h"

void ISPWriteEEPROM(unsigned int jml_byte_data)
{
    //Variabel Tulis Data EEPROM Memory Byte Mode 
    unsigned char data;
    unsigned int i; 
    unsigned int addr_EEPROM_memory; 
    
    addr_EEPROM_memory= 0; 
    i=0;  
     
    while(jml_byte_data>0) 
    {  
        /** Instruksi Write EEPROM Memory pada Serial Programming **/  
        data= bacaExtSRAM(i++);
        if(data!=0xFF)
        {
            SPIMasterKirimData(0xC0);    
            SPIMasterKirimData(addr_EEPROM_memory>>8);   
            SPIMasterKirimData(addr_EEPROM_memory);  
            SPIMasterKirimData(data);  
            
            /*** Polling Data EEPROM Memory ***/
            ISPPollingEEPROM(addr_EEPROM_memory);
        }
          
        /* Autoincrement alamat untuk writing EEPROM memory dan parameter polling EEPROM memory */   
        addr_EEPROM_memory++; 
         
        jml_byte_data--; 
    }  
} 

void ISPReadEEPROM(unsigned int addr_EEPROM_memory_max)
{
    unsigned char data_EEPROM; 
    unsigned int addr_EEPROM_memory;   
    unsigned int i;       
    
    addr_EEPROM_memory= 0;
    i= 0;
    
    while(addr_EEPROM_memory<addr_EEPROM_memory_max) 
    {
        /*** Instruksi Read EEPROM Memory pada Serial Programming ***/
        //Read EEPROM Memory Low Byte
        SPIMasterKirimData(0xA0);    
        SPIMasterKirimData(addr_EEPROM_memory>>8);   
        SPIMasterKirimData(addr_EEPROM_memory);  
        data_EEPROM=SPIMasterKirimData(0x00); 
        tulisExtSRAM(i++, data_EEPROM);
        
        /* Autoincrement alamat EEPROM memory */ 
        addr_EEPROM_memory++; 
    }; 
} 

void ISPPollingEEPROM(unsigned int addr_EEPROM_memory)
{
    unsigned char ulang_polling_EEPROM;
    unsigned char data_EEPROM_memory; 
      
    //Reload jumlah pengulangan polling data EEPROM memory
    ulang_polling_EEPROM= ULANG_POLLING_EEPROM_MAX;  

    while(ulang_polling_EEPROM>0) 
    {
        /*** Instruksi Read EEPROM pada Serial Programming ***/
        SPIMasterKirimData(0xA0);  
        SPIMasterKirimData(addr_EEPROM_memory>>8);   
        SPIMasterKirimData(addr_EEPROM_memory);  
        data_EEPROM_memory=SPIMasterKirimData(0x00); 
        if(data_EEPROM_memory==0xFF) delay_ms(9);
        ulang_polling_EEPROM--;
    } 
}