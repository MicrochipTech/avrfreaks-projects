#include <ctype.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include <util/atomic.h>


#include "fifo.h"
#include "uart1281.h"
#include "main.h"
#include "misc.h"
#include "console.h"
#include "sd.h"
#include "fat32.h"
#include "audio.h"



