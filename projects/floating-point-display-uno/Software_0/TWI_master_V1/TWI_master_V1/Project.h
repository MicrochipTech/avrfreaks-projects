


#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


#include "../Resources/Display_header.h"
#include "../Resources/TWI_master_subroutines.c"
#include "../Resources/Floating_point_subroutines.c"


