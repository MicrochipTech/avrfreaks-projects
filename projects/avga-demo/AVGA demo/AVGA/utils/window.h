/*

	file:	window.h
	desc:	tool heder.

	note:	Window provides interface between monitor and driver's screen memory (reference table).
			All windows are aligned horizontally, in order they were added with window_add(...).
			Each window can have following special parameters: vertical scroll (max 7 pixels) and 4bit color mask,
			which affects DDR of VIDEO_PORT.
			Window can either be invisible. In such case, it's videolines are not scanned - each line 
			means 1000 more free clock cycles.

	note: 	Sum of window's heights should always be greater than vertical resolution. This is handled automatically until 
			number of used windows reaches MAX_WINDOWS.

	todo:	implement WINDOW_SCROLL version which would take blocks from memory somehow.

	author: Jaromir Dvorak (md@unicode.cz)

*/


#ifndef _WINDOW_H_
#define _WINDOW_H_


/* ########## BEGIN OF USER CONFIGURABLE SECTION ########## */

//
//Maximum count of windows on the screen.
//
#define MAX_WINDOWS 3

//
//define which types of scrolling you want to compile.
//
#define WINDOW_ABS_SCROLL
//#define WINDOW_SCROLL
//#define WINDOW_SCROLL_FULL


/*  ########### END OF USER CONFIGURABLE SECTION ########### */


/*
 *
 * Window handling.
 *
 */

struct window
{
	unsigned char line;
	unsigned char height;
	unsigned char params;
} __attribute__((packed));

//
// Pseudo-type for high-level.
//
#define WINDOW  struct window*

//
// Returns handle to the first window.
//
#define window_get_default()	({ extern struct window windows[]; windows; })



/*
 *
 * Generic window set-up.
 *
 */

//
// Creates a window.
//
WINDOW window_add(unsigned char line, unsigned char height, unsigned char params);

//
// Theese macros should be used instead of window_add.
//
#define window_add_space(height)  		window_add(0, height, 0)
#define window_add_standard(row, rows)  window_add(row<<3, rows<<3, 0xf8) 
#define window_add_clone(wnd)  			window_add(wnd->line, wnd->height, wnd->params) 

//
// Clears all visible window and restores original state (whole screen mapped from the beginning)
//
void window_reset();



/*
 *
 * Basic window input/output macros.
 *
 */

#define awindow_get_row(wnd)			  ((wnd->line)>>3)
#define awindow_get_startline(wnd)		  ((wnd->line))
#define awindow_get_height(wnd)			  ((wnd->height)>>3)
#define awindow_get_num_lines(wnd)		  ((wnd->height))
#define awindow_get_scrollX(wnd)		  ((wnd->params & 7))
#define awindow_get_scrollY(wnd)		  ((wnd->params & 7))
#define awindow_get_ptr(wnd)		 	  &driver_get_block(0, awindow_get_row(wnd))
#define awindow_get_block(wnd, x, y)      driver_get_block(x, y + awindow_get_row(wnd))
#define awindow_set_block(wnd, x, y, blk) driver_set_block(x, y + awindow_get_row(wnd), blk)
#define awindow_fill(wnd, blk) 		  	  memset(awindow_get_ptr(wnd), blk, (DRIVER_COLUMNS) * awindow_get_height(wnd))
#define awindow_fill_scroll(wnd, blk) 	  memset(awindow_get_ptr(wnd), blk, (DRIVER_COLUMNS) * (awindow_get_height(wnd)+1))
#define awindow_print(wnd, x, y, txt)	  driver_print(x, y + awindow_get_row(wnd), txt)
#define awindow_print_P(wnd, x, y, txt)	  driver_print_P(x, y + awindow_get_row(wnd), txt)

//
// Window effects.
//
void awindow_set_colormask(struct window* wnd, unsigned char mask);
#define awindow_show(wnd)  ({ wnd->params |= 0x08;  })
#define awindow_hide(wnd)  ({ wnd->params &= ~0x08; })
#define awindow_adjust_for_background(wnd, x, y) awindow_set_scroll(wnd, ~(x), ~(y))

//
// Scrolling effects.
// note: for fully functional scrolling, a standard window with one row space  below must be used.
//
void awindow_set_scroll(struct window* wnd, unsigned char x, unsigned char y);
void awindow_scroll(WINDOW wnd, signed char x, signed char y);
void awindow_scroll_full(struct window* wnd, signed char x, signed char y, unsigned char default_block);

//
// Tansition effects.
// note: For scrolling, wnd->line and wnd->height must be multiply of 8.
//	 	 When using following macros, user must handle this!
//
#define awindow_move(wnd, n)   ({ wnd->line += n; })
#define awindow_shrink(wnd, n) ({ wnd->height += n; }) 





/*
 *
 * Optimized window input/output macros.
 *
 */

WINDOW selwnd;
unsigned char *selwndptr;

#define window_select(wnd)		     ({ selwnd = wnd; selwndptr = awindow_get_ptr(wnd); })
#define window_get_row()		 	 awindow_get_row(selwnd)
#define window_get_startline()		 awindow_get_startline(selwnd)
#define window_get_height()			 awindow_get_height(selwnd)
#define window_get_num_lines()		 awindow_get_num_lines(selwnd)
#define window_get_scrollX()		 awindow_get_scrollX(selwnd)
#define window_get_scrollY()		 awindow_get_scrollX(selwnd)
#define window_get_ptr()			 (selwndptr)
#define window_get_block(x, y)       (selwndptr[(x)+(y)*DRIVER_COLUMNS])
#define window_set_block(x, y, blk)  ({selwndptr[(x)+(y)*DRIVER_COLUMNS] = (blk);})
#define window_fill(blk) 		  	 memset(window_get_ptr(), blk, (DRIVER_COLUMNS) * window_get_height())
#define window_fill_scroll(blk) 	 memset(window_get_ptr(), blk, (DRIVER_COLUMNS) * (window_get_height()+1))
#define window_print(x, y, txt)      strdump(&window_get_block((x),(y)), (txt)); 
#define window_print_P(x, y, txt) 	 strdump_P(&window_get_block((x), (y)), (txt));  

#define window_set_colormask(mask)		  	awindow_set_colormask(selwnd, mask)
#define window_show(wnd)  				  	awindow_show(selwnd)
#define window_hide(wnd) 				  	awindow_hide(selwnd) 
#define window_adjust_for_background(x, y) 	awindow_adjust_for_background(selwnd, x, y) 

#define window_set_scroll(x,y)				awindow_set_scroll(selwnd, x, y)
#define window_scroll(x,y,default_block)	awindow_scroll(selwnd,x, y, default_block)
#define window_move(n)						awindow_move(selwnd, n) 
#define window_shrink(n)					awindow_shrink(selwnd, n)


#endif
