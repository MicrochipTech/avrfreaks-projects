
#include <avr/io.h>

#include "tinyserial.h"

#define INPUT_LENGTH 16

int
main(void) {

  char buffer[INPUT_LENGTH + 1];
  unsigned int length;

  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_READWRITE);
  
  while (1) {
    ts_puts("input with echo: ");
    length = ts_gets(buffer, INPUT_LENGTH, TS_ECHO_ON);
    buffer[length] = '\0';
    ts_puts("\r\noutput: ");
    ts_puts(buffer);
    ts_puts("\r\n");

    ts_puts("input without echo: ");
    length = ts_gets(buffer, INPUT_LENGTH, TS_ECHO_OFF);
    buffer[length] = '\0';
    ts_puts("\r\noutput: ");
    ts_puts(buffer);
    ts_puts("\r\n");
  }

  return 0;
}
