//_____ I N C L U D E S ____________________________________________________

#include "eeprom.h"

//_____ D E F I N I T I O N S ______________________________________________

void ee_write (U8 address, U8 value){
    
  /* Wait for completion of previous write */
  while(EECR & (1<<EEWE));
  /* Set up address and data registers */
  EEAR = address;
  EEDR = value;
  Disable_interrupt();
  /* Write logical one to EEMWE */
  EECR |= (1<<EEMWE);
  /* Start eeprom write by setting EEWE */
  EECR |= (1<<EEWE);
  Enable_interrupt();
}

U8 ee_read(U8 address){
  
  /* Wait for completion of previous write */
  while(EECR & (1<<EEWE));
  /* Set up address register */
  EEAR = address;
  /* Start eeprom read by writing EERE */
  EECR |= (1<<EERE);
  /* Return data from data register */
  return EEDR;
}
