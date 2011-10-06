#define TRUE 1
#define FALSE 0
#define PDATA(type, ...) (__extension__({static type __nonamed[] PROGMEM = __VA_ARGS__ ; &__nonamed[0];}))


// call to print memory to a text buffer as hex data
// option=0, ############
// option=1, ##:##:##:##:##:## (used for MAC)
char* hex2ascii(char* textbuf, void* startadd, unsigned char length, unsigned char option);

//write a number to a text buffer, base can be 1-41 (using alpha up to Z)
//number will be right justified within string of length, left padded with char pad
//if length is 0, or padding character is null, string length will be the minimum required
//if fixed length specefied is inadequate for value, buffer will contain '#' characters
//note that specefied length does not include terminator, or sign character, so buffer must be at least length+2
//if sign_pad is non0, positive values will begin with this character
char* ascii_long(long value, char *text, unsigned char base, unsigned char fixed_length, char digit_pad, char sign_pad);
char* ascii_ulong(unsigned long value, char *text, unsigned char base, unsigned char fixed_length, char pad);
