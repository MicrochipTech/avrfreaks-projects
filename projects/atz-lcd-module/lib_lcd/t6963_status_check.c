#include "lcd_t6963.h"

void t6963_status_check(void) {
    while ((t6963_status()&t6963_status_check_mask)!=t6963_status_check_mask) ;
}
