//debugging
void tulis_EEPROM(unsigned char data);  
//debugging program 
unsigned int alamat_EEPROM;
void tulis_EEPROM(unsigned char data) 
{ 
  /* Tunggu sampai syarat kondisi terpenuhi  */
  if(alamat_EEPROM<512 ) 
    {
      /* Tunggu sampai proses penulisan EEPROM sebelumnya selesai*/
      while(EECR & (1<<1)); 
        /* Setting alamat dan data register EEPROM */
      EEAR = alamat_EEPROM;
      EEDR = data; 
               /* Mengaktifkan EEPROM Master Write Enable (EEMWE) */
      EECR |= (1<<2);  
       /* Mulai menuliskan data dengan mengaktifkan EEPROM Write Enable (EEWE) */
      EECR |= (1<<1); 
          /* Penambahan (increment) alamat EEPROM */ 
      alamat_EEPROM++;  
    }                        
}
