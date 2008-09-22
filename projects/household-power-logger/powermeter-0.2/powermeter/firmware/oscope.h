#include <stdint.h>

#include "config.h"

#if USE_OSCOPE

#define OSCOPE_STRIDE 2
#define OSCOPE_LEN 128
extern uint8_t oscope_buf[OSCOPE_LEN];
extern uint8_t oscope_full;

void print_scope_glcd();

#endif
