#include "extsram.h"
                         
/************************************************************************************************************/
// Fungsi Inisialisasi SRAM Eksternal
void inisialisasiExtSRAM(void)
{  
    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // INT2: Off
    // Interrupt on any change on pins PCINT0-7: Off
    // Interrupt on any change on pins PCINT8-15: Off
    // External SRAM page configuration: 
    //              -              / 0000h - 1FFFh
    // Lower page wait state(s): None
    // Upper page wait state(s): None
    MCUCR=0x80;
    EMCUCR=0x00; 
} 
/************************************************************************************************************/  
// Fungsi Pembacaan SRAM Eksternal
unsigned char bacaExtSRAM(unsigned int addr_read_ext_SRAM)
{
    unsigned char *ptr_data = (unsigned char *) (addr_read_ext_SRAM + OFFSET_ADDR_EXT_SRAM);
    unsigned char data;
    DDRC = 0xFF;
    PORTC = 0x00; 
    data= *ptr_data;
    return data;
} 
/************************************************************************************************************/ 
// Fungsi Penulisan SRAM Eksternal
void tulisExtSRAM(unsigned int addr_write_ext_SRAM, unsigned char data)
{
    unsigned char *ptr_data = (unsigned char *) (addr_write_ext_SRAM + OFFSET_ADDR_EXT_SRAM);
    DDRC = 0xFF;
    PORTC = 0x00; 
    *ptr_data= data;
}