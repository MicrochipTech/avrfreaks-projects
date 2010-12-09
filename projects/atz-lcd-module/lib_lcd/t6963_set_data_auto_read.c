#include "lcd_t6963.h"

void t6963_set_data_auto_read(void) {
    t6963_send_command(T6963_SET_DATA_AUTO_READ);
    t6963_status_check_mask=T6963_STATUS_CHECK_AUTO_READ;
}
