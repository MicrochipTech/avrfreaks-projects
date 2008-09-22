#include "config.h"

int ee24xx_read_bytes(uint16_t eeaddr, int len, uint8_t *buf);
int ee24xx_write_page(uint16_t eeaddr, int len, uint8_t *buf);
