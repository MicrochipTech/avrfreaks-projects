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

coord_t polar_to_rectangular(float theta,int radius);
void update_train(short int angle,rgb_im_t *leds);
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
coord_t polar_2_rect(short int angulo,short int raio);
void copy_rgb(rgb_im_t *lo,rgb_im_t *ld,short int size);
coord_t p_2_r(int theta,int radius);
#endif /* IMAGE_PROCESS_H_ */