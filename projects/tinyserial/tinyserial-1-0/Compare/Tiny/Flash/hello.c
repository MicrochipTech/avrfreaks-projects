
#include <avr/io.h>

#include "tinyserial.h"

char thestr[] __attribute((__progmem__)) = "Hello world!\r\n";

int
main(void) {

  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
  
  while (1) {
    ts_putsf(thestr);
  }

  return 0;
}
