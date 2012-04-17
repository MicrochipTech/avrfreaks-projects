#include <avr/eeprom.h>

uint8_t mymac[6];

uint8_t gwip[4];
uint8_t myip[4];
uint8_t netmask[4];
uint8_t dnsip[4];

uint8_t _enable_dhcp;