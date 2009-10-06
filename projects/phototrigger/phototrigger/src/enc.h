/*
 * enc.h
 *
 *  Created on: 2009-08-30
 *      Author: liku
 *
 *      Header file for encoder implementation. Contains some constant which
 *      determine encoder behavior.
 */

#ifndef ENC_H_
#define ENC_H_

#include <stdbool.h>

#define KEYDEFENCSKIP 3     // how many encoder events skip before counting it as an event
#define KEYREPEATRATE 30    // delay for keyboard in ms
#define KEYWAITFORKEY true  // make a value human readable
#define KEYNOWAIT     false // make a value human readable

typedef enum _key_t
{
	KEYLEFT,
	KEYRIGHT,
	KEYSWITCH,
	KEYNONE
} key_t;

key_t encGetKey(bool waitforkey);
void encClrKeyBuf(void);

#endif /* ENC_H_ */
