
#define FALSE 0
#define TRUE 1

#define PDATA(type, ...) (__extension__({static type __nonamed[] PROGMEM = __VA_ARGS__ ; &__nonamed[0];}))
char* hex2ascii(char* textbuf, void* startadd, unsigned char length, unsigned char option);


unsigned int read_bige_uint(unsigned int bige);
