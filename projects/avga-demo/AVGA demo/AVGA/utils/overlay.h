/*

	file:	overlay.h
	desc:	OVERLAY tool heder.

	note:	The overlay tool uses RAM block capability of the driver to show floating images, primitives, etc.
			overlay_draw(x, y, ...) function behaves just like a putimage(x, y, ...)  
	 		in a full-graphics environment.

	author: Jaromir Dvorak (md@unicode.cz)

*/

#ifndef _OVERLAY_H_
#define _OVERLAY_H_



/* ########## BEGIN OF USER CONFIGURABLE SECTION ########## */

//
// define maximum block count for overlay graphics here.
// the system assumes that appropriate amount of allocated memory was given to driver_mmap(...)
//
#define OVERLAY_BLOCK_COUNT   DRIVER_RAM_BLOCK_COUNT  //use all available blocks

//
// Define this if you want the system to be able to remove drawn entities.
//
#define OVERLAY_CLEARABLE

//
// If this is defined, and there's not enough RAM to finish a drawing, eariler allocated blocks will be released.
//
//#define OVERLAY_INVERSE_PRIORITY

//
// define if you want to draw simple overlay primitives
//
#define OVERLAY_PRIMITIVES

//
// define if using overlay_draw.
//
#define OVERLAY_DRAW

//
// choose drawing area. (selected window or whole screen).
//
#define OVERLAY_DRAW_WINDOW 

//
// cut the image if it exceeds drawing area somewhere?
//
//#define OVERLAY_DRAW_SAFE

//
// EOR all valid pixels of the image with lower 4 bits of param.
//
#define OVERLAY_DRAW_COLORMODIFY

//
// allow simple image transformations (horizontal & vertical flipping)
//
//#define OVERLAY_DRAW_TRANSFORMATIONS

//
// transparent color for overlay_draw, pixels in the drawn image with this color will not change underlay pixels.
//
#define OVERLAY_DRAW_ALPHA    0


/* ########### END OF USER CONFIGURABLE SECTION ########### */


/*
 * 
 * Defined options combination.
 *
 */

#if defined(OVERLAY_CLEARABLE) || defined(OVERLAY_INVERSE_PRIORITY)
#define OVERLAY_REMEMBER_BLOCKS
#endif

#if defined(OVERLAY_DRAW_COLORMODIFY) || defined(OVERLAY_DRAW_TRANSFORMATIONS)
#define OVERLAY_DRAW_PARAM
#endif

#if defined(OVERLAY_DRAW_COLORMODIFY) || defined(OVERLAY_DRAW_SAFE)
#define OVERLAY_DRAW_IMGADDR
#endif


/*
 * 
 * Function prototypes / macros
 *
 */

//
// Removes all drawn overlay objects.
//
void overlay_clear(void);

//
// Releases all drawn overlay entities.
// Use this macro instead of overlay_clear() if all underlay blocks were restored or repalaced.
//
#ifdef OVERLAY_INVERSE_PRIORITY
#define overlay_release() 					({ extern unsigned char overlay_block_id; extern unsigned char *s_addr[OVERLAY_BLOCK_COUNT]; memset(s_addr,0,sizeof(s_addr)); overlay_block_id=0; })
#else
#define overlay_release() 					({ extern unsigned char overlay_block_id; overlay_block_id = 0; })

//
// Returns number of currently used  blocks.
//
#define overlay_get_used_block_count()		({ extern unsigned char overlay_block_id; overlay_block_id; })
#endif


//
// Puts an overlay image at pointer img stored in flash with dimensions width, height to selected window at x, y.
// Returned value: 0 if OK, else if not.
// note: configuration - dependent function arguments are used to allow optimizer to always do best job,
//		 even in high-level usage of this function.
//
#ifdef OVERLAY_DRAW_PARAM
#define overlay_draw( x, y, img, width, height) overlay_draw_param(x,y,img,width,height,0)  //overloading.

unsigned char overlay_draw_param
(
#ifdef OVERLAY_DRAW_SAFE
	signed int x, signed int y, 
#else
	unsigned char x, unsigned char y,
#endif
	PGM_P img, 
	unsigned char width, unsigned char height,
	unsigned char param
);
#else
unsigned char overlay_draw
(
#ifdef OVERLAY_DRAW_SAFE
	signed int x, signed int y, 
#else
	unsigned char x, unsigned char y,
#endif
	PGM_P img, 
	unsigned char width, unsigned char height
);
#endif


//
// Primitives
//
unsigned char overlay_putpixel (unsigned char x, unsigned char y, unsigned char color);
unsigned char overlay_line     (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
unsigned char overlay_bar      (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
unsigned char overlay_circle   (unsigned char x, unsigned char y,  unsigned char r, unsigned char color);



/*
 * 
 * Some high-level useful macros.
 *
 */

//
// draws an overlay sprite from tileset to window wnd.
//
#define overlay_draw_sprite( x, y, sprite ) overlay_draw( x, y,(uint8_t *)&_pgmp[(sprite) * DRIVER_BLOCK_SIZE], 8, DRIVER_BLOCK_HEIGHT);
#define overlay_draw_sprite_param( x, y, sprite, param ) overlay_draw_param( x, y,(uint8_t *)&_pgmp[(sprite) * DRIVER_BLOCK_SIZE], 8, DRIVER_BLOCK_HEIGHT,param);


/*
 * 
 * Backward compatibility.
 *
 */

#define putimage(x,y,img,width,height)	 	 overlay_draw(x, y, img, width, height);
#define sprite_put(id,x,y,sprite)            overlay_draw_sprite(x, y, sprite)
#define sprite_init()
#define sprite_clear()                       overlay_clear()
#define SPRITE_COUNT                         OVERLAY_BLOCK_COUNT/2


#endif  //!_OVERLAY_H_
