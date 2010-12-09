#include "lcd.h"

void t6963_auto_reset(void) {
    t6963_send_command(T6963_AUTO_RESET);
    t6963_status_check_mask=T6963_STATUS_CHECK_COMMAND_DATA;
}
