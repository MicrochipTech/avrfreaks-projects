#include "debug.h"

void tulisEEPROM(uchar data) 
{ 
	if(alamat_EEPROM <= EEPROM_ADDRESS_MAX) 
	{
		/* Tunggu sampai proses penulisan EEPROM sebelumnya selesai*/
		while(EECR & (1<<EEWE)); 
		  
		/* Setting alamat dan data register EEPROM */
		EEAR= alamat_EEPROM;
		EEDR= data; 
		  
		/* Mengaktifkan EEPROM Master Write Enable (EEMWE) */
		EECR |= (1<<EEMWE);  
		  
		/* Mulai menuliskan data dengan mengaktifkan EEPROM Write Enable (EEWE) */
		EECR |= (1<<EEWE); 
			 
		/* Penambahan (increment) alamat EEPROM */ 
		alamat_EEPROM++;
	}
}
