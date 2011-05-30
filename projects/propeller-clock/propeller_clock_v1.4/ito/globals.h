/*
 * globals.h
 *
 * Created: 09-06-2011 18:28:24
 *  Author: limapapy
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <avr/pgmspace.h>

#define MAX_SIZE_WORD		20
#define TRANSLATE			12

//#define F_CPU				16000000UL 



#ifndef rgb_t
typedef struct rgb{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}rgb_t;
#endif


#ifndef rgb_t
typedef struct rgb_im{
	unsigned char r:2;
	unsigned char g:3;
	unsigned char b:3;
}rgb_im_t;
#endif


typedef struct {
	short int angle[7];
	rgb_t ptr_color;
	rgb_t nbr_color;
	rgb_t min_color;
	rgb_t hour_color;
}clk_analog_t;

typedef struct{
	short int sec;
	short int min;
	short int hour;
}timep_t;


typedef struct coord{
	char x;
	char y;
}coord_t;

typedef enum{
	INV=-1,
	ACLK,
	DCLK,
	PICT
}MODES;
/*
#ifndef bool
typedef enum{
	true,
	false
}bool;
#endif
*/
typedef enum{
	HELP_MSG0=0,
	HELP_MSG1,
	HELP_MSG2,
	HELP_MSG3,
	HELP_MSG4,
	SCLK_MSG,
	NSCLK_MSG,
	DISP_PIC_MSG,
	NDISP_PIC_MSG,
	MODE_MSG,
	NMODE_MSG,
	NUMBER_COLOR_MSG,
	NNUMBER_COLOR_MSG,
	POINTER_COLOR_MSG,
	NPOINTER_COLOR_MSG,
	PATTERN_MSG,
	NPATTERN_MSG	
}MSG;

extern const char posCOOR[91][32] PROGMEM;
extern timep_t time_clk;
extern clk_analog_t analog_clock;
extern rgb_t train[32];
extern unsigned char USART_WORD[];
extern unsigned int USART_WORD_INDEX;
extern MODES mode;
extern short int theta;
extern int sent;
extern PGM_P messages[];
extern short int cont_encoder;
extern char caract[185];
extern rgb_t rgb_table[57];
extern short int enable_send;
extern const int cosine[360] PROGMEM;
extern short int HELDER_FRANCISCO_FERREIRA_LOBATO[5];
extern char clock_digits[8];
extern unsigned char clock_window[7][40];
extern const char tab_chars[11] PROGMEM;
extern const unsigned char char_patterns[11][7] PROGMEM;
#endif /* GLOBALS_H_ */
