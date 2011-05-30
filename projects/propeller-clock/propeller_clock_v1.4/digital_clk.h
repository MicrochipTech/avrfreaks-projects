#ifndef DIGITAL_CLK_H
#define DIGITAL_CLK_H

coord_t get_clock_window_index(coord_t p1, char *found);
void digital_clock_generator(rgb_t *train, short int theta);
void store_clock_digit(unsigned char start,unsigned char clock_window[7][40],unsigned char digit[7]);
void get_pattern_digit(unsigned char index,unsigned char pattern_digit[7]);
int get_chr_index(char ascii_value);
void update_clk_window(unsigned char clock_window[7][40]);
void  clear_clock_window(unsigned char clock_window[7][48]);
unsigned char get_pixel(unsigned char j, unsigned char i, unsigned char clock_window[7][48]);
void shift_msg_window(void);
void init_DCLK();

#endif 
