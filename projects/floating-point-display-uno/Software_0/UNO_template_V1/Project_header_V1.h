
#include <avr/wdt.h>

#define switch_1_down  ((PIND & 0x04)^0x04)
#define switch_1_up    (PIND & 0x04)
#define switch_3_down  ((PINB & 0x04)^0x04)

#define IO        '1'
#define FPN       '2'
#define int_num   '3'

#define OVF_cntl_reg     (uint8_t*)0x3FB                      //EEPROM address. Set bits to zero and reset them to one
#define OVF_test          0x0                                 //Bit 0: Set for OVF test and reset if OK
#define OVF_active        0x01                                //Bit 1: Set to flash display if OVF test fails
#define Restore_OVF       0x02                                //Bit 2: Set on recovery from OVF
#define Recovery_active   0x03                                //Bit 3: Recovery from glitch

#define OVF_recovery      0x3FF                               //Address in EEPROM of number displayed before OVF ocurred
#define IO_recovery       0x3FA                               //Address in IO of number displayed before error ocurred



/***************************************************************************************************/
#define ongoing_OVF \
!(eeprom_read_byte(OVF_cntl_reg) & (1 << Restore_OVF))

#define normal_data_entry \
eeprom_read_byte(OVF_cntl_reg) & (1 << Restore_OVF)

#define clear_OVF_flag \
eeprom_write_byte(OVF_cntl_reg,\
eeprom_read_byte(OVF_cntl_reg) | (1 << Restore_OVF));

#define clear_all_recovery_flags \
eeprom_write_byte(OVF_cntl_reg, 0xFF);



/****************************************************************************************************/
#define flash_on_FPN_overflow \
\
if(!(eeprom_read_byte(OVF_cntl_reg) & (1 << OVF_test))){\
eeprom_write_byte(OVF_cntl_reg,(eeprom_read_byte(OVF_cntl_reg)) & (~(1 << OVF_active)));\
float_num_1 = float_num_from_eepom(0);\
float_num_to_display(float_num_1);\
wdt_enable(WDTO_500MS);\
while(1){if(switch_3_down)\
{eeprom_write_byte(OVF_cntl_reg,(eeprom_read_byte(OVF_cntl_reg) | ((1<<OVF_test)|(1<<OVF_active))));\
eeprom_write_byte(OVF_cntl_reg,(eeprom_read_byte(OVF_cntl_reg) & (~(1 << Restore_OVF))));\
break;}}}



/***************************************************************************************************/
#define setup_328_HW \
setup_watchdog;\
ADMUX |= (1 << REFS0);\
initialise_IO;\
Reset_L;\
_delay_ms(10);\
Reset_H;\
TWAR = 0x06;\
flash_on_FPN_overflow;\
Serial.begin(38400);\
while (!Serial);\
if(switch_3_down)\
eeprom_write_byte(OVF_cntl_reg,\
(eeprom_read_byte(OVF_cntl_reg) & \
(~(1 << Restore_OVF))));



/***************************************************************************************************/
#define initialise_IO \
MCUCR &= (~(1 << PUD));\
DDRB = 0;\
DDRC = 0;\
DDRD = 0;\
PORTB = 0xFF;\
PORTC = 0xFF;\
PORTD = 0xFF;\
DDRC |= 0x08;

#define Reset_L PORTC &= ~(0x08);
#define Reset_H PORTC |= 0x08;


/***************************************************************************************************/
#define wdr()  __asm__ __volatile__("wdr")

#define setup_watchdog \
wdr();\
MCUSR &= ~(1<<WDRF);\
WDTCSR |= (1 <<WDCE) | (1<< WDE);\
WDTCSR = 0;

#define SW_reset {wdt_enable(WDTO_30MS);while(1);}



/******************************************************************************************************/
#include "Resources_UNO_template\UNO_template_header_V1.h"
#include "Resources_UNO_template\UNO_TWI_subroutines_V1.c"
#include "Resources_UNO_template\ASKII_subroutines_V1.c"
#include "Resources_UNO_template\PCI_subroutines_V1.c"
