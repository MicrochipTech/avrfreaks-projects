#ifndef INCLUDE_TextFormat
#define INCLUDE_TextFormat

#include <inttypes.h>

#define DigitsLong 11
#define convBufLen (DigitsLong+2)		//enough to hold a long	with '\'0	and a '.'
										//for fixed pt reals (eg long/lat)
extern char convBuf[convBufLen];		

#define GetConvBuffer() ((const char*)convBuf)

const char* const LtoaZ(int32_t val, uint8_t width);
const char* const LtoaN(int32_t val, uint8_t width);
const char* const LtoaS(int32_t val, uint8_t width);
//const void        PutDotInIt(uint8_t digitsBefore);
const char* const LtoaLL(int32_t angle, uint8_t totalChars);



#endif //TextFormat