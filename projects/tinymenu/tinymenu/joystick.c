#include <inttypes.h>

#include "joystick_hw.h"
#include "joystick.h"

void joystick_init(void)
{
	joystick_hw_init();
}


uint8_t joystick_read(void)
{
	return joystick_hw_read();
}
