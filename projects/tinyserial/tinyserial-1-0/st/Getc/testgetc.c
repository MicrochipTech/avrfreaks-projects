
#include <avr/io.h>

#include "tinyserial.h"


int
main(void) {

  char c;

  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_READWRITE);
  
  while (1) {
    ts_putc('i');
    ts_putc('n');
    ts_putc(' ');
    ts_putc('w');
    ts_putc('i');
    ts_putc('t');
    ts_putc('h');
    ts_putc(' ');
    ts_putc('e');
    ts_putc('c');
    ts_putc('h');
    ts_putc('o');
    ts_putc(':');
    ts_putc(' ');
    c = ts_getc(TS_ECHO_ON);
    ts_putc('\r');
    ts_putc('\n');
    ts_putc('o');
    ts_putc('u');
    ts_putc('t');
    ts_putc(':');
    ts_putc(' ');
    ts_putc(c);
    ts_putc('\r');
    ts_putc('\n');

    ts_putc('i');
    ts_putc('n');
    ts_putc(' ');
    ts_putc('n');
    ts_putc('o');
    ts_putc(' ');
    ts_putc('e');
    ts_putc('c');
    ts_putc('h');
    ts_putc('o');
    ts_putc(':');
    ts_putc(' ');
    c = ts_getc(TS_ECHO_OFF);
    ts_putc('\r');
    ts_putc('\n');
    ts_putc('o');
    ts_putc('u');
    ts_putc('t');
    ts_putc(':');
    ts_putc(' ');
    ts_putc(c);
    ts_putc('\r');
    ts_putc('\n');
  }

  return 0;
}
