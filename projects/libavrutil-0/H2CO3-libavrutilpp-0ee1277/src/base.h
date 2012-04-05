/*
 * base.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_BASE_H__
#define __AVRUTILPP_BASE_H__

#include <avrutil/avrutil.h>

void *operator new(size_t size);
void operator delete(void *ptr);

namespace avrutil {
};

#endif /* __AVRUTILPP_BASE_H__ */

