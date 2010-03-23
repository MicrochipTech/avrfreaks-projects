
#include <avr/io.h>

#include "tinyserial.h"

int
main(void) {

  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
  
  while (1) {
    ts_putc('H');
    ts_putc('e');
    ts_putc('l');
    ts_putc('l');
    ts_putc('o');
    ts_putc('!');
    ts_putc('\r');
    ts_putc('\n');

  }

  return 0;
}
