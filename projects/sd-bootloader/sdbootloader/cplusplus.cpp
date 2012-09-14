#include "cplusplus.h"

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}

void __cxa_guard_release (__guard *g) {*(char *)g = 1;}

void __cxa_guard_abort (__guard *) {}

void __cxa_pure_virtual(void) { while(1); }
