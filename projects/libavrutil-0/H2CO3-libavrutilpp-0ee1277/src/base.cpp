/*
 * base.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <stdlib.h>
#include "base.h"

/* 
 * These have to be implemented manually due to a bug in the toolchain.
 * Without these implementations, one gets an
 * "Undefined reference to operator new" linker error.
 */

void *operator new(size_t size)
{
	return malloc(size);
}

void operator delete(void *ptr)
{
	free(ptr);
}

