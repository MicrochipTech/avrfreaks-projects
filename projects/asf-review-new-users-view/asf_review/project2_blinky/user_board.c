
#include "user_board.h"

void board_init(void)
{
    ioport_configure_pin(LED0_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
}

