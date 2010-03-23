extern "C" void __cxa_pure_virtual() {}

#include <avr/pgmspace.h>

#define BUFLEN 15

prog_char thestr[] PROGMEM = {"Hello World!\r\n"};

char buffer[ BUFLEN ];

void setup() {
  Serial.begin(9600);
  strncpy_P(buffer, thestr, BUFLEN);
}

void loop() {
  Serial.println(buffer);
}


