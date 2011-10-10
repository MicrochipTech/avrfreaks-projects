#ifndef USER_BOARD_H
#define USER_BOARD_H

#include "ioport.h"

#define LED0_GPIO   IOPORT_CREATE_PIN(PORTJ,0)

void board_init(void);

#endif /* USER_BOARD_H */

