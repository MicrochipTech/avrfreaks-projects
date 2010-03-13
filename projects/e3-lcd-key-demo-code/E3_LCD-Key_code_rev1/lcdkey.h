/*
 * This file contains hardware constants for E3 LCD-Key Part SB6432 configurable
 * LCD display/switches.  See <http://www.e3-keys.com/>.
 * This code was developed on the Atmel mega168.
 *
 * This code is not maintained and is free to use with no rights reserved, and
 * no guarantees made.
 *
 * Code is based on ScreenKey code developed by Tom Blough at AVR Freaks <http://www.avrfreaks.net>
 *
 * Uses - SPI
 *
 * Jim Minihane 3/7/2010
 */

#ifndef E3_LCDKEY_H
#define E3_LCDKEY_H


/******** USER CONFIGURATION OPTIONS ************************/

// uncomment the following 2 lines to include support for dynamic text.
// this adds approximately 1K program and 480 data bytes to the code footprint.
#define E3_TEXT_SUPPORT

#define E3_TEXT_CHAR 96 // Set the number of characters to use in the charMap. 96 or 45

// uncomment the following 2 lines to add support for bar graphs
#define E3_GRAPH_SUPPORT

#define E3_BAR_HEIGHT 7 // Set the height of a single line horizontal bar graph (7 or 8 pixels)

// uncomment the following line to add support for image display
#define E3_IMAGE_SUPPORT

#define E3_IMAGE_COUNT 2 // The number of images in the imageMap array. You will need to make sure the array
                         // is populated with the correct amount of data as defined

/******** END OF CONFIGURATION OPTIONS ************************/


/*
 * RGB Background colors (3 colors [RGB], each with variable intensities)
 * Byte format is Red byte, Green byte, Blue byte. A maximum value of 0x7F is allowed.
 */
#define E3_BLACK 0x00,0x00,0x00
#define E3_GRAY 0x32,0x32,0x32
#define E3_WHITE 0x7F,0x7F,0x7F

#define E3_DARK_BLUE 0x00,0x00,0x32
#define E3_BRIGHT_BLUE 0x00,0x00,0x7F
#define E3_PALE_BLUE 0x32,0x32,0x7F

#define E3_DARK_RED 0x32,0x00,0x00
#define E3_BRIGHT_RED 0x7F,0x00,0x00
#define E3_PALE_RED 0x7F,0x32,0x32

#define E3_DARK_MAGENTA 0x32,0x00,0x32
#define E3_BRIGHT_MAGENTA 0x7F,0x00,0x7F
#define E3_PALE_MAGENTA 0x7F,0x32,0x7F

#define E3_DARK_GREEN 0x00,0x32,0x00
#define E3_BRIGHT_GREEN 0x00,0x7F,0x00
#define E3_PALE_GREEN 0x40,0x7F,0x40

#define E3_DARK_CYAN 0x00,0x32,0x32
#define E3_BRIGHT_CYAN 0x00,0x7F,0x7F
#define E3_PALE_CYAN 0x32,0x7F,0x7F

#define E3_DARK_YELLOW 0x32,0x32,0x00
#define E3_BRIGHT_YELLOW 0x7F,0x7F,0x00
#define E3_PALE_YELLOW 0x7F,0x7F,0x32

// LCD-Key SB6432 Commands and constants
#define E3_SET_ADDR 0x40		// Set Display Address & Write Display Data
#define E3_SET_COLOR 0x41		// Set Color - The code in these files uses the SET_RGB_COLOR command
								// But this is here since the E3 LCD-Key does support this command as well.
#define E3_SET_RGB_COLOR 0x42	// Set RGB Color - This is the extended color command. It allows for more and brighter colors.
#define E3_END_BYTE 0x43		// End Transmission - This command tells the LCD-Key that the command and following data is finished.
								// Please read the documentation because it is not always needed.
#define E3_IMAGE_BYTES 256  	// maximum bytes in pixel image
#define E3_IMAGE_WIDTH 64  		// display width in pixels/columns
#define E3_MAX_TEXT 10  		// maximum of 6x8 (5x7 font) characters across LCD
#define E3_LINE_1 0x01FC  		// byte address of upper left corner of Line 1
#define E3_LINE_2 0x01FE  		// byte address of upper left corner of Line 2
#define E3_LINE_3 0x01F8  		// byte address of upper left corner of Line 3
#define E3_LINE_4 0x01FA  		// byte address of upper left corner of Line 4



// text support
#define E3_TEXT_CENTER 0
#define E3_TEXT_RIGHT 1
#define E3_TEXT_LEFT 2
#define E3_TEXT_FULL 0
#define E3_TEXT_EXACT 1

// public functions
extern void e3_init( void);

/* e3_set_pixels
 * Used by e3_clear_line, e3_set_text_line, and e3_set_text_loc
 * 1st param "data[]" = The byte array to be sent to the LCD
 * 2nd param "count" = the number of characters to be displayed
 * 3rd param "line_address" = The line address of the LCD.
 */
extern void e3_set_pixels( uint8_t data[], uint8_t count, uint16_t line_address);

/* e3_set_color
 * Sets the color of the LCD back light
 * 1st param "red_byte" = Red LED value, values 0x00 - 0x7F allowed
 * 2st param "green_byte" = Green LED value, values 0x00 - 0x7F allowed
 * 3st param "blue_byte" = Blue LED value, values 0x00 - 0x7F allowed
 * Generally, however, the color Macros above are used to send all 3 bytes
 */
extern void e3_set_color( uint8_t red_byte, uint8_t green_byte, uint8_t blue_byte);







#ifdef E3_GRAPH_SUPPORT
/* e3_bar_graph
 * Creates a single line high (7 or 8 pixels) displaying a bar width 0-60 pixels wide.
 * 1st param "bar_value" = numeric value of the bar. Values = 1 - 60.
 * 2nd param "line" = The line on which to display the bar. Values = E3_LINE_1, E3_LINE_2, E3_LINE_3, E3_LINE_4
 */
extern void e3_bar_graph( uint8_t bar_value, uint16_t line);

// Bar graph
 #ifndef E3_BAR_HEIGHT
  #define E3_BAR_HEIGHT 7
 #endif // E3_BAR_HEIGHT

 #if E3_BAR_HEIGHT == 7  // 7 Bit high bar graph
	#define E3_BG_VERT 0x7F
	#define E3_BG_BAR 0x5D
	#define E3_BG_GAP 0x41
 #else // 8 Bit high bar graph
	#define E3_BG_VERT 0xFF
	#define E3_BG_BAR 0xBD
	#define E3_BG_GAP 0x81
 #endif // E3_BAR_HEIGHT

 #ifndef E3_CLEAR_PIXELS // If you have graphs, you'll need to be able to clear them if necessary
   #define E3_CLEAR_PIXELS
 #endif // E3_CLEAR_PIXELS

#endif // E3_GRAPH_SUPPORT





#ifdef E3_IMAGE_SUPPORT
/* e3_set_image
 * 1st param "image_index" = The array column position of the image to be displayed.
 * Overwrites entire display.
 */
extern void e3_set_image( uint8_t image_index);

/* imageMap array sizing
 * 1st param ALWAYS has to be 256. The image is ALWAYS 256 bytes. So the E3_IMAGE_BYTES macro is used as defined above.
 * 2nd param is the number if images in the array. The E3_IMAGE_COUNT macro is used as defined above.
 */
static const uint8_t imageMap[E3_IMAGE_BYTES][E3_IMAGE_COUNT];  // image array size

 #ifndef E3_CLEAR_PIXELS // If you have images, you'll need to be able to clear them if necessary
   #define E3_CLEAR_PIXELS
 #endif // E3_CLEAR_PIXELS

#endif // E3_IMAGE_SUPPORT






#ifdef E3_TEXT_SUPPORT

 #ifndef E3_TEXT_CHAR
  #define E3_TEXT_CHAR 96
 #endif

 #if E3_TEXT_CHAR == 96
  static const uint8_t charMap[96][5];  // 5x8 bitmap font. Full 96 characters
 #else
  static const uint8_t charMap[45][5];  // 5x8 bitmap font, Subset of 45 characters to save memory. Upper Case and numbers mostly.
 #endif //E3_TEXT_CHAR


/* Choose one of the below text generators. You don't really need both. If you want the compiled code smaller, use
 * the e3_set_text_line. For extended functionality, use the e3_set_text_loc.
 */

/* e3_set_text_line - Simple, smaller footprint text generator
 * 1st param "text" = The text string to display. Enclosed in Double Quotes.
 * 2nd param "justify" = character justification on the line. Values = E3_TEXT_LEFT, E3_TEXT_CENTER, E3_TEXT_RIGHT
 * 3rd param "line" =  The line on which to display the text. Values = E3_LINE_1, E3_LINE_2, E3_LINE_3, E3_LINE_4
 */
extern void e3_set_text_line( char* text, uint8_t justify, uint16_t line);

/* e3_set_text_loc - More functional, larger footprint text generator
 * 1st param "text" = The text string to display. Enclosed in Double Quotes.
 * 2nd param "mode_option" = Write the full line of text or an exact part of the line. Values = E3_TEXT_FULL, E3_TEXT_EXACT
 * 3rd param "exact_length_char" = The number of exact characters. Only functions with E3_TEXT_EXACT option. Values = 1 - 10
 *                                 Should never be less than the number if characters sent.
 * 4th param "justify" = character justification on the line/part. Values = E3_TEXT_LEFT, E3_TEXT_CENTER, E3_TEXT_RIGHT
 * 5th param "line" =  The line on which to display the text. Values = E3_LINE_1, E3_LINE_2, E3_LINE_3, E3_LINE_4
 * 6th param "line_loc" = The column within the line to write text. Only functions with E3_TEXT_EXACT option. Values = 1 - 64
 *
 * If you dont need all the functionality of e3_set_text_loc, you should comment the below line out as well
 * as the associated routine in the e3_lcdkey.c file to save space.
 */
extern void e3_set_text_loc( char* text, uint8_t mode_option, uint8_t exact_length_char, uint8_t justify, uint16_t line, uint8_t text_loc);

 #ifndef E3_CLEAR_PIXELS // If you have text, you'll need to be able to clear them if necessary
   #define E3_CLEAR_PIXELS
 #endif // E3_CLEAR_PIXELS

#endif // E3_TEXT_SUPPORT






#ifdef E3_CLEAR_PIXELS
/* e3_clear_line
 * Clears a single line of text. Each cleared line requires 320 bytes to be sent over SPI.
 */
extern void e3_clear_line( uint16_t line);

/* e3_clear_scr
 * Clears the entire screen. This works faster than clearing even a single line
 * lines of text, but does add to the code footpront. Requires 259 bytes to be sent over SPI.
 */
extern void e3_set_scr( uint8_t data_byte);

#endif // E3_CLEAR_PIXELS


// private helper functions
void write_byte( uint8_t data); // Sends byte by SPI as a whole byte
void write_data( uint8_t data); // Sends byte by SPI as 2 4-bit nibbles.
void write_address( uint16_t addr_data); // Sends 2 bytes, least significant 4 nibbles.

#endif // E3_LCDKEY_H
