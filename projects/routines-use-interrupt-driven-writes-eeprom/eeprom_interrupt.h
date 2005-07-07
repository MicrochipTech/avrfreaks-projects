#include <avr/signal.h>
#include <avr/interrupt.h>
extern volatile char eeprom_busy;

//prototypes
//designed for atmega128, but is either directly portable to others, or with minimal modification.
char	eeprom_interrupt_write_block (void* source, void* dest, char num_bytes);
SIGNAL(SIG_EEPROM_READY);


