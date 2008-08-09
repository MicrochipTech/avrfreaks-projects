#ifndef __debug_h_included__
#define __debug_h_included__

//debugging
#define EERE                    0
#define EEWE                    1
#define EEMWE                   2
#define EERIE                   3

#define EEPROM_Read_Enable             (1<<EERE)
#define EEPROM_Write_Enable            (1<<EEWE)
#define EEPROM_Master_Write_Enable     (1<<EEMWE)
#define EEPROM_Ready_Interrupt_Enable  (1<<EERIE) 

#define EEPROM_ADDRESS_MAX      512 

void tulis_EEPROM(unsigned char data);  

#endif 