#ifndef _LCD_BARGRAPH_
#define _LCD_BARGRAPH_

// Initialize the bar graph routine
void lcd_bar_init(void);

// Print a one-character vertical bar graph corresponding to an
// unsigned 8-bit number
void lcd_bar(unsigned char value);

#endif //_LCD_BARGRAPH_
