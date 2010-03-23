
#include <avr/io.h>

#include "tinyserial.h"
#include "data.h"

// This string is larger than can fit in SRAM, but we can store it in flash
// and print it one character at a time.
char thestr[] __attribute((__progmem__)) = THEDATA;

int
main(void) {

  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
  
  while (1) {
    ts_putsf(thestr);
  }

  return 0;
}
