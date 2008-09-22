#include "config.h"

#define printP(s) printstr_p(PSTR(s))
#define print_keyP_val32(k,v) print_keyp_val32(PSTR(k), v)

#define OUT_UART 0
#define OUT_GLCD 1
extern uint8_t output_device;

void init_serial();
uint8_t peekchr();
char getchr();
void putchr(char c);
void clear_screen();
#if GLCD_OVERPAINT
void cursor_to_top();
void clear_to_screen_end();
#endif
uint8_t readhex();
void printstr(const char *s);
void printstr_p(const char *s);
void zeropad(char *s, int target_len);
void print_int32(int32_t val);
void print_keyp_val32(const char *key, int32_t val);
void print_decimal(int32_t val, uint8_t dp);
void print_hex8(uint8_t val);
int32_t read_int32();
