#ifndef __flash_h_included__
#define __flash_h_included__

#define ULANG_POLLING_FLASH_MAX     2 

//Deklarasi Prototipe Fungsi Operasi-Operasi Memori Flash
void ISPWriteFlashMemory(unsigned int jml_page_data); 
unsigned int ISPWriteHugeFlashMemory(unsigned int jml_page_data, unsigned int addr_page_flash_terakhir);
void ISPReadFlashMemory(unsigned int addr_flash_max);
unsigned int ISPReadHugeFlashMemory(unsigned int ukuran_halaman_flash, unsigned int addr_page_flash_terakhir);
void ISPPollingFlashMemory(unsigned int addr_rd_prog_memory);  

#endif