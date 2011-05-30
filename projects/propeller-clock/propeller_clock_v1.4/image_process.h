/*
 * image_process.h
 *
 * Created: 01-06-2011 23:44:02
 *  Author: limapapy
 */ 


#ifndef IMAGE_PROCESS_H_
#define IMAGE_PROCESS_H_
#include "globals.h"
#include <math.h>

#define DEGREE_TO_RADIAN(val) (val*M_PI/180)


//****************** DIGITAL CLOCK DEFINES**************************************


#define NUM_CHARS 11
#define X_CLK_W_START  8
#define X_CLK_W_END  55
#define Y_CLK_W_START 10
#define Y_CLK_W_END  16
#define WINDOW_SIZE 48

#define MSG_W_START 17
#define MSG_W_END 46
#define MSG_W_SIZE 34
#define MSG_ARRAY_SIZE 150
#define Y_MSG_W_START 44
#define Y_MSG_W_END 50
#define SHIFT_N	500
#define W_STATE0 0
#define W_STATE1 1
#define W_STATE2 2
// END DIGITAL DEFINES**************************************************************
coord_t polar_to_rectangular(float theta,int radius);
void update_train(short int angle,rgb_t *leds);
void  next_train(rgb_t *leds);
void clear_train(void);
void spiral(rgb_t color);
rgb_t rand_color(void);
void propeller_simul(void);

//PATRICIO SÃO AS MINHAS FUNÇÕES
char *getCHAR_PIXEIS(char chr, char *caract);
int getPIXEL_CHAR(char chr,int posx, int posy, char *caract);
int isBlack(rgb_t COR);//verifica se é preto

void nop(void);
float convert_to_radian(short int degree);
float convert_encod_2_radian();
short int convert_encod_2_degree();
void convert_int_to_2digits(char *dois_digitos,short int digito);
unsigned char my_rand(void);
void square_prop(unsigned char d,short int theta,rgb_t *leds);
void mod_div(short int *divd,short int *divr,short int *q,short int *r);
coord_t polar_2_rect(short int angulo,short int raio);


//**********************************DIGITAL HEADERS*****************************************


unsigned char get_pixel(unsigned char j, unsigned char i, unsigned char clock_window[7][48]);
void  clear_clock_window(unsigned char clock_window[7][48]);
void update_clk_window(unsigned char clock_digits[8], unsigned char clock_window[7][48]);
int get_chr_index(char ascii_value);
void get_pattern_digit(unsigned char index, unsigned char pattern_digit[7]);
void store_clock_digit(unsigned char start,unsigned char clock_window[7][48], unsigned char digit[7]);
window_coord get_clock_window_index(int x, int y);
window_coord get_mensg_window_index(int x, int y );
void digital_clock_generator(void);
void shift_msg_window(void);
int sin_calc(int angle);
int cos_calc(int angle);
//*********************************END DIGITAL HEADERS********************************************
#ifdef RAM
void copy_rgb(rgb_im_t *lo,rgb_im_t *ld,short int size);
void update_train_im(short int angle,rgb_im_t *leds);
void  next_train_im(rgb_im_t *leds);
void full_fill(rgb_t color,rgb_t *leds);
#endif // RAM

#endif /* IMAGE_PROCESS_H_ */