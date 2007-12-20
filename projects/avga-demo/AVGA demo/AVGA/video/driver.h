/*

	file:	driver.h
	desc:	VIDEO DRIVER header.

	note:	graphics memories are mapped with driver_mmap
			driver_throw_scanline(...) shifts out one video line to VIDEO_PORT, 5 clock cycles per one pixel.

	todo:	implement double buffering.

	author: Jaromir Dvorak (md@unicode.cz)

*/ 


#ifndef _DRIVER_H_
#define _DRIVER_H_


/* ########## BEGIN OF USER CONFIGURABLE SECTION ########## */

////
//Reference table size.
////
#define DRIVER_WIDTH			 24
#define DRIVER_HEIGHT			 18

//////
//Define how many upper blocks driver should look for in RAM.
//////
#define DRIVER_RAM_BLOCK_COUNT   12

//////
//Define height of block in pixels.
//note that block width is always 8, fixed.
//////
#define DRIVER_BLOCK_HEIGHT      8

///
//Define if you want to enable horizontal scrolling.
//note: if scrolling is not needed at all, a simple version of the driver should be used.
////
#define HSCROLL

/////
//Define how many extra rows you want to use for vertical scrolling.
//e.g. how many windows can be independetly vertically scrollable.
/////
#define VSCROLL					 1


/* ########### END OF USER CONFIGURABLE SECTION ########### */


/*
 *
 * Low-level defined configuration combinations.
 *
 */

#ifdef  HSCROLL
#define DRIVER_COLUMNS 			 (DRIVER_WIDTH + 1)
#else
#define DRIVER_COLUMNS           (DRIVER_WIDTH)
#endif

#ifdef  VSCROLL
#define DRIVER_ROWS              (DRIVER_HEIGHT + VSCROLL)
#else
#define DRIVER_ROWS              (DRIVER_HEIGHT)
#endif

#define DRIVER_PGM_BLOCK_COUNT   (0xFF-DRIVER_RAM_BLOCK_COUNT)
#define DRIVER_BLOCK_SIZE        (DRIVER_BLOCK_HEIGHT * 4)

#ifndef __ASM__
#include <avr/pgmspace.h>



/*
 *
 * Basic drawing constants
 *
 */

//screen parameters
#define DRIVER_RESX				 (DRIVER_WIDTH * 8)
#define DRIVER_MAXX				 (DRIVER_COLUMNS * 8)
#define DRIVER_RESY				 (DRIVER_HEIGHT * DRIVER_BLOCK_HEIGHT)
#define DRIVER_MAXY				 (DRIVER_ROWS  * DRIVER_BLOCK_HEIGHT)
#define DRIVER_LAST_COLUMN		 (DRIVER_WIDTH-1)
#define DRIVER_LAST_ROW			 (DRIVER_HEIGHT-1)

//page size
#define DRIVER_SCREEN_SIZE		 (DRIVER_ROWS * DRIVER_COLUMNS)
#define DRIVER_RAMMAP_SIZE		 (DRIVER_BLOCK_SIZE * DRIVER_RAM_BLOCK_COUNT)




/*
 *
 * Generic I/O macros.
 *
 */

//map the given block to ram
#define driver_rammap_block(x)       (DRIVER_PGM_BLOCK_COUNT + (x))

//is the given block in ram?
#define driver_is_rammap(x)    ((x) >= DRIVER_PGM_BLOCK_COUNT)

//gets the block from x,y
#define driver_get_block(x, y) (_scrp[(x)+(y)*DRIVER_COLUMNS])

//sets the block x,y to blk
#define driver_set_block(x,y, blk) ({_scrp[(x)+(y)*DRIVER_COLUMNS] = (blk);})

//fill the whole screen with block blk
#define driver_fill(blk)  memset(&driver_get_block(0,0), blk, DRIVER_SCREEN_SIZE)

//clear the whole screen
#define driver_clear()  driver_fill(0)

//prints PGM string
#define driver_print_P(x, y, txt) ({ strdump_P(&driver_get_block((x),(y)), (txt)); })

//prints RAM string
#define driver_print(x, y, txt) ({ strdump(&driver_get_block((x),(y)), (txt)); })




/*
 *
 * Video pointers. 
 *
 */

uint8_t *_scrp;
uint8_t *_ramp;
uint8_t *__ramp;
PGM_P _pgmp;



/*
 *
 * Function prototypes 
 *
 */

//
//map memory.
//
static inline void driver_mmap(uint8_t *screen, PGM_P pgm, uint8_t *ram)
{
	_scrp = screen;
	_ramp = ram;
	_pgmp = pgm;
	__ramp = ram - DRIVER_PGM_BLOCK_COUNT * DRIVER_BLOCK_SIZE;
}

//
//shifts out a scanline.
//
void driver_throw_scanline(uint8_t line, uint8_t scroll);


#endif // !__ASM__
#endif // !_DRIVER_H_
