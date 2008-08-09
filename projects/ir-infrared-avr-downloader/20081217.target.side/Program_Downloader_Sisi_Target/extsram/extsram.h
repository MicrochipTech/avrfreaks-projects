#ifndef __extsram_h_included__
#define __extsram_h_included__
 
#define OFFSET_ADDR_EXT_SRAM    0x0500    //Alamat awal buffer Ext SRAM 
#define ADDR_EXT_SRAM_MAX       0x1FFF    //Ext SRAM 8 kbyte

void inisialisasiExtSRAM(void);
unsigned char bacaExtSRAM(unsigned int addr_read_ext_SRAM);
void tulisExtSRAM(unsigned int addr_write_ext_SRAM, unsigned char data);

#endif 