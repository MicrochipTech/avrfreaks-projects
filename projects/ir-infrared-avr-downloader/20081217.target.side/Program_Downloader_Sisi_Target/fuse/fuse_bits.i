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
//Deklarasi Prototipe Fungsi Operasi-Operasi Memori EEPROM
void ISPWriteEEPROM(unsigned int jml_byte_data);
void ISPReadEEPROM(unsigned int addr_EEPROM_max);
void ISPPollingEEPROM(unsigned int addr_EEPROM_memory);   
      /*** Operasi Baca Kode Devais ***/
void ISPReadDevaisSignature(void)
{ 
    unsigned char i;
          /*** Instruksi Read Signature Byte pada Serial Programming ***/  
    //Devais Signature Byte ATmega8535
    //0x00 0x1E
    //0x01 0x93
    //0x02 0x08
            enterProgrammingMode();    
    for(i=0;i<3;i++)
    {
        SPIMasterKirimData(0x30);    
        SPIMasterKirimData(0x00);   
        SPIMasterKirimData(i);  
        devais_signature[i]=SPIMasterKirimData(0x00);
    }
    leaveProgammingMode(); 
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
