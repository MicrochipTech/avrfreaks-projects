


unsigned int alamat_EEPROM;

{ 

  if(alamat_EEPROM<512 ) 
    {
      /* Tunggu sampai proses penulisan EEPROM sebelumnya selesai*/
      while(EECR & (1<<1)); 
  
      EEAR = alamat_EEPROM;
      EEDR = data; 
          
      EECR |= (1<<2);  
  
      EECR |= (1<<1); 
     
      alamat_EEPROM++;  
    }                        
}