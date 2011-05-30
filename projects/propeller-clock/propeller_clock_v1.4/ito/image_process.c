/*
 * image_process.c
 *
 * Created: 01-06-2011 23:44:26
 *  Author: limapapy
 */ 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
//#include <common.h>
//#include <time.h>
#include "globals.h"
#include "image_process.h"
#include "images/image_01.h"
#include "spi_com.h"
#define PI 3.141592654
//#define PROTEUS_DEBUG
#ifdef PROTEUS_DEBUG
	short int xg,yg,ang,cosg,sing;
	rgb_im_t ledsg[32];
#endif // PROTEUS_DEBUG

#define X_PIXEL				65
#define Y_PIXEL				65
#define RADIUS				32
#define CURRENT_IMAGE		image_01
#define SHIFT_RGB			4
#define SHIFT_RGB_IMR		10
#define SHIFT_RGB_IMG		9
#define SHIFT_RGB_IMB		9
#define INV360				0.0027777777778				
//rgb_t IMAGE[X_PIXEL][Y_PIXEL];
unsigned int CURRENT_ANGLE;
short int t=0;
//convert the rectangular to polar
coord_t polar_to_rectangular(float theta,int radius)
{
	coord_t pos;
	pos.x=(int)((float)radius*cos(theta)+32-TRANSLATE);
	pos.y=(int)(-(float)radius*sin(theta)+32-TRANSLATE);
	return pos;
}

//get the next state of the leds from the matrix 

void update_train(short int angle,rgb_im_t *leds){
/*	short int i=0;
	short int x,y;
	rgb_im_t col_tmp;
	short int t_sin,t_cos;
	rgb_im_t col={0,0,0};
	short int sin_ang;sin_ang= (angle>=90) ? angle-90 : 0-(angle-90);
	t_sin= pgm_read_word(&cosine[sin_ang]);
	t_cos=pgm_read_word(&cosine[angle]);
#ifdef PROTEUS_DEBUG
	cosg=t_cos;
	sing=t_sin;
	ang=angle;
#endif // _DEBUG
	for (i=1;i<RADIUS+1;){
		
		x=(((i*t_cos)>>7)+32-TRANSLATE);
		y=(-((i*t_sin)>>7)+32-TRANSLATE);
#ifdef PROTEUS_DEBUG
		serial_send((unsigned char)x);serial_send((unsigned char)y);serial_send(0);
		xg=x;yg=y;
#endif // PROTEUS_DEBUG
	
		if (((x>40 || y>40) || (x<0 || y<0 )))
		{
			i++;
			continue;
		}
		leds[i-1]=CURRENT_IMAGE[y][x];
		i++;
		
		x=(((i*t_cos)>>7)+32-TRANSLATE);
		y=(-((i*t_sin)>>7)+32-TRANSLATE);
#ifdef PROTEUS_DEBUG
		serial_send((unsigned char)x);serial_send((unsigned char)y);serial_send(0);
		xg=x;yg=y;
#endif // PROTEUS_DEBUG
			if (((x>40 || y>40) || (x<0 || y<0 )))
		{
			i++;
			continue;
		}
		leds[i-1]=CURRENT_IMAGE[y][x];
		i++;
		
		x=(((i*t_cos)>>7)+32-TRANSLATE);
		y=(-((i*t_sin)>>7)+32-TRANSLATE);
#ifdef PROTEUS_DEBUG
		serial_send((unsigned char)x);serial_send((unsigned char)y);serial_send(0);
		xg=x;yg=y;
#endif // PROTEUS_DEBUG
			if (((x>40 || y>40) || (x<0 || y<0 )))
		{
			i++;
			continue;
		}
		leds[i-1]=CURRENT_IMAGE[y][x];
		i++;
		
	}
#ifdef PROTEUS_DEBUG
	copy_rgb(leds,ledsg,32);
#endif // PROTEUS_DEBUG
	
*/
}


void update_and_send(float theta){
	rgb_t col_tmp={0,220,0};
		
	rgb_t col_tmp2={0,0,230};
//	rgb_t col_tmp3;
	short int i;
	long int to_send;
	float t_sin,t_cos;
	short int x,y;
	t_sin=sin(theta);
	t_cos=cos(theta);
	for (i=RADIUS-1;i>0;i-=2){
		//send first 3 byte
		x=(short int)((float)i*t_cos+32);
		y=(short int)(-(float)i*t_sin+32);
		/*col_tmp.r=pgm_read_byte(&(CURRENT_IMAGE[y][x].r));
		col_tmp.g=pgm_read_byte(&(CURRENT_IMAGE[y][x].g));
		col_tmp.b=pgm_read_byte(&(CURRENT_IMAGE[y][x].b));*/
		
		x=(short int)((float)(i-1)*t_cos+32);
		y=(short int)(-(float)(i-1)*t_sin+32);
		/*col_tmp2.r=pgm_read_byte(&(CURRENT_IMAGE[y][x].r));
		col_tmp2.g=pgm_read_byte(&(CURRENT_IMAGE[y][x].g));
		col_tmp2.b=pgm_read_byte(&(CURRENT_IMAGE[y][x].b));
		*/
		
		to_send=((col_tmp.b<<SHIFT_RGB));
		to_send=(to_send<<12);
		to_send|=(col_tmp.g<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		
		//send second 3 byte
		to_send=(col_tmp.r<<SHIFT_RGB);
		to_send=(to_send<<12);
		to_send|=((col_tmp2.b)<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		//send third 3 byte
		to_send=(col_tmp2.g<<SHIFT_RGB);
		to_send=(to_send<<12);
		to_send|=(col_tmp2.r<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		/*col_tmp3=col_tmp;
		col_tmp=col_tmp2;
		col_tmp2=col_tmp3;*/
		//_delay_ms(1000);
	}
	
}
//send the data to the leds drivers
void  next_train(rgb_t *leds){
	short int i;
	long int to_send=0;
	for (i=RADIUS-1;i>0;i-=2){
		//send first 3 byte
		to_send=((leds[i].b<<SHIFT_RGB));
		to_send=(to_send<<12);
		to_send|=(leds[i].g<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		
		//send second 3 byte
		to_send=(leds[i].r<<SHIFT_RGB);
		to_send=(to_send<<12);
		to_send|=((leds[i-1].b)<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		//send third 3 byte
		to_send=(leds[i-1].g<<SHIFT_RGB);
		to_send=(to_send<<12);
		to_send|=(leds[i-1].r<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
	}
	set_xlat();
	clear_xlat();
}

//send the data to the leds drivers
void  next_train_im(rgb_im_t *leds){
	short int i;
	long int to_send=0;
	for (i=RADIUS-1;i>0;i-=2){
		//send first 3 byte
		to_send=((leds[i].b<<SHIFT_RGB_IMB));
		to_send=(to_send<<12);
		to_send|=(leds[i].g<<SHIFT_RGB_IMG);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		
		//send second 3 byte
		to_send=(leds[i].r<<SHIFT_RGB_IMR);
		to_send=(to_send<<12);
		to_send|=((leds[i-1].b)<<SHIFT_RGB_IMB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		//send third 3 byte
		to_send=(leds[i-1].g<<SHIFT_RGB_IMG);
		to_send=(to_send<<12);
		to_send|=(leds[i-1].r<<SHIFT_RGB_IMR);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
	}
}

void clear_train(void){
	unsigned char i;
	for (i=0;i<RADIUS;i++){
		train[i].r=0;
		train[i].g=0;
		train[i].b=0;
	}
	
}


void spiral(rgb_t color){
		long int leds;
		unsigned char i;
		rgb_t off={0,0,0};
		leds =0x01;
		//clear_train();
		train[0]=color;
		next_train(train);
		set_xlat();
		_delay_us(10);
		clear_xlat();
		for (i=0;i<RADIUS;i++){
			if(i<31){
				train[i]=off;
				train[i+1]=color;
			}	
			next_train(train);
			set_xlat();	
			
			clear_xlat();
			_delay_ms(10);	
			
		}
		train[RADIUS-1]=off;
		next_train(train);
}	

unsigned char my_rand(void) {
  static unsigned int seed;
  seed = 181 * seed + 359 ;
  return (seed >> 8);
}
rgb_t rand_color(void){
	rgb_t tmp_color;
	tmp_color.r=my_rand()%256;
	tmp_color.g=my_rand()%256;
	tmp_color.b=my_rand()%256;
	
	return tmp_color;
	
}


void propeller_simul(void){
	unsigned short int i=0;
	rgb_im_t *leds;
	
	leds=(rgb_im_t*)malloc(32*sizeof(rgb_im_t));
	while(1){
		//update_and_send(angle);
		update_train(i%360,leds);
		//square_prop(10,i%360,train);
		next_train_im(leds);
		set_xlat();
		//_delay_us(10);
		clear_xlat();
		//_delay_ms(3000);
		i++;
	}
	
}


void nop(void){
	unsigned char i=0,j;
	
	for (i=0;i<5;i++)
	{
		j=i;
	}
	j++;
}


//PATRICIO SÃO AS MINHAS FUNÇÕES

int getPIXEL_CHAR(char chr,int posx, int posy, char *caract)
{		
		char pos=255;
		char pixel;
		if (chr>= 48 && chr <= 58)
			pos=chr-48;
		if (chr>= 65 && chr <= 90)
			pos=chr-54;

		if (pos==255 || posx>4 || posy>6)
			return 0;
		else{
			pixel=pgm_read_byte(&caract[pos*5+posx]);
			return  (pixel >> (7 - posy )  & 0x01);
		}		
		
}




char *getCHAR_PIXEIS(char chr, char *caract)
{
		char pos=255;

		if (chr>= 48 && chr <= 58)
			pos=chr-48;
		if (chr>= 65 && chr <= 90)
			pos=chr-54;

		if (pos==255)
			return NULL;
		else
			return &caract[pos*5];
}

unsigned int read_TCNT0(void){
	
	
	return 0;
}



float convert_encod_2_radian()
{
	return cont_encoder*0.0174532925;
}

short int convert_encod_2_degree()
{
	return ( (int)cont_encoder*0.72);
}

float convert_to_radian(short int degree){
	float rad=degree;
	rad=rad*INV360*M_PI*2;
	return rad;
}

void convert_int_to_2digits(char *dois_digitos,short int digito)
{
	dois_digitos[0]=48+(int)(digito*0.1);
	dois_digitos[1]=48+(int)(digito%10);
}

int isBlack(rgb_t COR)
{
	return !(COR.r+COR.g+COR.b);
}




void copy_rgb(rgb_im_t *lo,rgb_im_t *ld,short int size){
	short i=0;
	for (i=0;i<size;i++)
	{
		ld[i]=lo[i];
	}
	
}


coord_t polar_2_rect(short int angulo,short int raio)
{
	coord_t coord;
	//short int sinalx, sinaly;
	if (angulo>=270)
	{
		angulo=360-angulo;
		coord.y=-1;
		coord.x=1;
	}
	else 	if (angulo>=180)
			{
				angulo=angulo-180;
				coord.y=-1;
				coord.x=-1;
			}
			else 	if (angulo>=90)
					{
						angulo=180-angulo;
						coord.y=1;
						coord.x=-1;
					}
					else
					{
						coord.x=1;
						coord.y=1;
					}
	
	
	coord.x=coord.x*pgm_read_byte(&posCOOR[angulo][raio-1])+33;//o raio é só de 0 a 31
	coord.y=(-1)*coord.y*pgm_read_byte(&posCOOR[90-angulo][raio-1])+33; // o angulo é de 0 a 90
	
	return coord;
}


coord_t p_2_r(int theta,int radius){
	coord_t pos;
	float ta=theta*PI/180;
	
	pos.x=(int)round((float)radius*(float)cos(ta)+33);
	pos.y=(int)round(-(float)radius*(float)sin(ta)+33);
	return pos;
}
