/*
 * globals.h
 *
 * Created: 09-06-2011 18:28:24
 *  Author: limapapy
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <avr/pgmspace.h>

#ifndef NULL
#define NULL				0
#endif

#define ROM
//#define RAM

#define MAX_SIZE_WORD		20
#define TRANSLATE			7

//#define F_CPU				16000000UL 



#ifndef rgb_t
typedef struct rgb{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}rgb_t;
#endif


#ifdef RAM
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
	unsigned char x;
	unsigned char y;
}coord_t;

typedef enum emode{
	INV=-1,
	ACLK,
	DCLK,
	PICT
}MODES;

#ifndef bool
typedef enum{
	bfalse,
	btrue
}bool;
#endif

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
extern short int encoder360;
extern const char posCOOR[91][32] PROGMEM;
extern short int image_index;
extern bool ens;

// *******************************DIGITAL VARS******************************


extern const char tab_chars[11] PROGMEM;
extern const unsigned char shift_message[7][150] PROGMEM;
extern const unsigned char char_patterns[11][7] PROGMEM;

typedef struct  {
        int x;
        int y;
		int found;
} window_coord;

typedef struct {
	short int start;
	short int end;
	short int shift_counter;
	short int state;
	short int array_offset;
} msg_window_t;

msg_window_t msg_window;

unsigned char clock_digits[8];
unsigned char clock_window[7][48];
rgb_t rgb_array[32];
coord_t p_1;
window_coord p_2;
window_coord  pos; 


//END DIGITAL VARS
#ifdef RAM
	extern rgb_im_t train_im[32];
#endif // RAM

#endif /* GLOBALS_H_ */