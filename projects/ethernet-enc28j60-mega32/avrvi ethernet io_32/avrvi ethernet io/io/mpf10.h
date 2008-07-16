#ifndef _MPF10_H_
#define _MPF10_H_
#include "../config.h"

#define MPF10_ENABLE()   MPF10_ENABLE_PORT |= (1<<MPF10_ENABLE_PIN)
#define MPF10_DISABLE()  MPF10_ENABLE_PORT &= ~(1<<MPF10_ENABLE_PIN)

#define MPF10_FADE_IN   1
#define MPF10_FADE_OUT -1

void mpf10_init(void);
void mpf10_fade(signed char fade);

#endif

