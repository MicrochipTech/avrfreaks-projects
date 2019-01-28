#ifndef _USART_FUNCS_// AVAILABLE FUNCTIONS
#define _USART_FUNCS_

// DEFINES
/*#ifndef F_CPU
#define F_CPU 8000000
#endif*/

#define usart_newline()      usart_transmit(0x0A);usart_transmit(0x0D);
char nibble_to_hex(uint8_t value); // Conversion tool to get hex values

// Set up the usart - 8-bits,no parity, 1 stop bit, no hardware control
void usart_init(uint16_t baud);
// FOR SETTING UP VERY FAST BAUDS. CAUSES TRUNCATION OTHERWISE! BUT VERY SLOW USING 32-bit VALUES!!!
void usart_init_Xlarge(uint32_t baud);
// Send data to the usart buffer
void usart_transmit(unsigned char data);
// Receives data and returns the character received
char usart_receive(void);
// Send a string to the usart buffer
void usart_transmit_str(const char* data);
// Send a string from PROGMEM the usart buffer
void usart_transmit_str_P(const char* data);
// Send a 16-bit integer to the usart buffer
void usart_transmit_int(int16_t num);
// Send a 32-bit integer to the usart buffer
void usart_transmit_int32(uint32_t num);
// Disable usart - just a test function. not sure if needed
void usart_rec_disable(void);
// Enable usart - just a test function. not sure if needed
void usart_rec_enable(void);
// Send decimal representation of a number --> TAKES MORE CPU FOR THIS ONE!!
void usart_transmit_dec( unsigned char value );
// returns true if there's data ready to be taken from UDR0
uint8_t usart_data_received(void);

int usart_int_enable(int(*func)(unsigned char));
int usart_int_disable(int(*func)(unsigned char));

///////////////////////////////////////////////////////////
/////// USART 1 FUNCTION PROTOTYPES////////////////////////
///////////////////////////////////////////////////////////
#ifdef __AVR_ATmega644P__
void usart1_init(uint16_t baud);
void usart1_init_Xlarge(uint32_t baud);
void usart1_transmit(unsigned char data);
char usart1_receive(void);
void usart1_transmit_str(const char* data);
void usart1_transmit_int32(uint32_t num);
#endif


// HYPERTERMINAL COMMANDS
// Text modes 
#define MODE_NONE         '0'
#define MODE_BOLD         '1'
#define MODE_DIM          '2'
#define MODE_UNDERLINE    '4'
#define MODE_BLINK        '5'
#define MODE_REVERSED     '7'
#define MODE_CONCEALED    '8'


// Text colours
#define COL_FOREGROUND    '3'
#define COL_BACKGROUND    '4'

#define COL_BLACK         '0'
#define COL_RED           '1'
#define COL_GREEN         '2'
#define COL_YELLOW        '3'
#define COL_BLUE          '4'
#define COL_MAGENTA       '5'
#define COL_CYAN          '6'
#define COL_WHITE         '7'

// HYPERTERMINAL HAS 24 LINES WITH 80 CHARACTERS PER LINE

// Cursor move direction 
#define MOVE_UP           'A'
#define MOVE_DOWN         'B'
#define MOVE_RIGHT        'C'
#define MOVE_LEFT         'D'

// Clear Hyperterminal Screen
void usart_clear_ht(void);
// Move cursor to position x,y on the hyperterminal screen (top left is 1,1)
void cursor_goto(int x, int y);
// Move cursor relative to position on screen. See above for info
void move_cursor(int distance, int direction);
// Saves current cursor position. NOTE: CAN ONLY SAVE 1 POSITION!! THEN IT'S OVERWRITTEN
void save_cursor_pos(void);
// Restore previous cursor position stored. I don't know what happens if you haven't previously saved a position...
void restore_cursor_pos(void);
// DUNNO!!! Would assume erases below cursor position
void erase_screen_bottom(void);
// DUNNO!!! Would assume erases above cursor position
void erase_screen_top(void);
// DUNNO!!! Would assume erases to end of line past cursor position
void erase_screen_endline(void);
// DUNNO!!! Would assume erases start of line before cursor position
void erase_screen_startline(void);
// DUNNO!!! Would assume erases current line
void erase_screen_line(void);
// DUNNO!!! Would assume select back/fore ground, then set the colour you want. SEE ABOVE DEFINES
void ht_set_display_colour( unsigned char fg_bg, unsigned char colour );
// DUNNO!!! Changes some display feature, but I don't know what... (maybe text colour)
void ht_set_display_attribute_mode( unsigned char mode );
// Enables scrolling on all the screen. Dunno what that means though...
void ht_scroll_all(void);
// Limit scrolling between a start and end line number. Dunno what it does though...
void ht_scroll_limit( unsigned char start, unsigned char end );
// Send hyperterminal the print screen command
void ht_print_screen(void);
// Draw a black line on the hyperterminal screen. IMPROVEMENT: Perhaps make the line colour available later on
void draw_horizontal_line(int y, int startx, int endx);

#endif
