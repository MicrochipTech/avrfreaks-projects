#include "config.h"

void init_stackcheck(void);
uint16_t get_free_stack(void);
void show_free_stack();
void trap_stack_overflow();
void dump_ram();
