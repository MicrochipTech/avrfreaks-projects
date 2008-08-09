#ifndef __eeprom_h_included__
#define __eeprom_h_included__

#define ULANG_POLLING_EEPROM_MAX    2 

//Deklarasi Prototipe Fungsi Operasi-Operasi Memori EEPROM
void ISPWriteEEPROM(unsigned int jml_byte_data);
void ISPReadEEPROM(unsigned int addr_EEPROM_max);
void ISPPollingEEPROM(unsigned int addr_EEPROM_memory);   

#endif 