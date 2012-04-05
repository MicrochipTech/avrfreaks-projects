/*
 * Tone.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_TONE_H__
#define __AVRUTILPP_TONE_H__

#include <avrutil/avrutil.h>

namespace avrutil {

class Tone {
	protected:
	uint16_t freq;
	public:
	Tone(uint16_t freq);

	void start();
	void stop();
};

};

#endif /* __AVRUTILPP_TONE_H__ */

