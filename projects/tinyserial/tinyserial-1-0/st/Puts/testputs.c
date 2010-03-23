
#include <avr/io.h>

#include "tinyserial.h"

int
main(void) {

  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
  
  while (1) {
    ts_puts("Hello world!\r\n");
  }

  return 0;
}
