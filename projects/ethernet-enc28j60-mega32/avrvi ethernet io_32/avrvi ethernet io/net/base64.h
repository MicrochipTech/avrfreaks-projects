#ifndef _BASE64_H_
#define _BASE64_H_
#include <stdint.h>

//decode helper
void base64_decode_4to3(unsigned char* buf, unsigned char *res);
//decode string given by *buf, return decoded string in *buf!
void base64_decode(unsigned char *buf, unsigned char len);

//void base64_encode_3to4(unsigned char *buf);
//void base64_encode(unsigned char *string);

#endif
