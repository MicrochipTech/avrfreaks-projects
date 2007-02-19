
#ifndef __hegr_h__
#define __hegr_h__


// character generator
#define CHAR8	   // CHAR14 oder CHAR8


#ifdef CHAR14
#define GRCHARHEIGHT 14
#define GRLINEHEIGHT 14
#endif

#ifdef CHAR8
#define GRCHARHEIGHT 8		   // pixels Heigh of a chracter
#define GRLINEHEIGHT 9		// Line to Line distance
#endif

typedef unsigned long rgbdef ;

// from gropen - basics integer (int) graphics.

// signed to detect negative overflows
extern int xmax ;	  // maybe 640, filled after gropen
extern int ymax ;	  // maybe 480, filled after gropen
extern int xold, yold ;	  // where grfrom has put the pen
extern rgbdef fcol, bcol ;	   // what grfcol / grbcolhas selected

extern int grcharsize ;	   // size multiplier of characters
extern int ngrzeilen ;	   // pixel height of characters
extern int uiGrLineHeight ;	   // pixel distance of characters
extern uint8_t grbitmap[256][GRCHARHEIGHT] ;

void gropen(void);			// start graphics capability
void grclear(void);
void grrefresh(void);		// bring picture to display device
void grwaitesc(void);
void grclose(void);

void grfcol(rgbdef selected_color);
void grbcol(rgbdef selected_color);
void grset(int x,int y);
void grfrom(int x,int y);
void grto(int x,int y);
void grputs(const char *string_pointer);
void grchar(char code_of_character);
void grsbyte(uint8_t pattern);


// #define mixrgb(r,g,b) ( (uint32_t) ( ((uint32_t)r << 16L)| (uint32_t)( (uint32_t)(g)<<8)|((b)  ) ))
#define mixrgb(r,g,b) ( (uint32_t) ( ((uint32_t)b << 16L)| (uint32_t)( (uint32_t)(g)<<8)|((r)  ) ))

#define GR_BLACK 0x000000L
#define GR_WHITE 0xffffffL
#define GR_GREEN 0x00ff00L
#define GR_RED	 0xff0000L
#define GR_BLUE	 0x0000ffL
#define GR_GREY	 0x808080L
#define GR_BROWN 0x808000L

// Extract a colour component
#define comp_r(col) ((uint8_t)(	( ((uint32_t)col) >> 16L ) & 0xffL ))
#define comp_g(col) ((uint8_t)(	((col) >>  8 ) & 0xff ))
#define comp_b(col) ((uint8_t)(	((col)		 ) & 0xff ))

#ifndef PRINTF					// don't make prototypes
extern "C" {
void guprintf(char const *fmt, ... );
void grprintf(char const *fmt, ... );
}

#endif	// PRINTF

#ifdef DOSGNU
#include <dos.h>
int int10( union REGS *preg);	// used for graphics selection
#endif

#ifdef MDOS40
int int10(union REGS *inregs);
#endif

#ifdef __TURBOC__
int int10(union REGS *inregs);
#endif

/*
#ifdef __TURBOC__
#include <graphics.h>

#define gropen() initgraph( EGAVGA_driver, VGAHI, "\\TC\\BGI\\" )
#define grto(x,y) lineto(x,y)
#define grfrom(x,y) moveto(x,y)
#endif		// __TURBOC__
*/

bool	griskey( void );
uint	grkey( void );


#endif			// #ifdef __hegr_h__
