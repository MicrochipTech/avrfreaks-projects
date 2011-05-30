/*
 * analog_clk.c
 *
 * Created: 13-06-2011 20:36:30
 *  Author: Lito
 */ 

#include "globals.h"
#include "image_process.h"
#include "analog_clk.h"
#include <avr/pgmspace.h>



void analog_clk_SET_COR_PONTEIROS(clk_analog_t *analog_clock,rgb_t COR)
{
	analog_clock->ptr_color=COR;
}

void analog_clk_SET_COR_NUMEROS(clk_analog_t *analog_clock,rgb_t COR)
{
	analog_clock->nbr_color=COR;
}

void analog_clk_SET_COR_HOUR(clk_analog_t *analog_clock,rgb_t COR)
{
	analog_clock->hour_color=COR;
}

void analog_clk_SET_COR_MIN(clk_analog_t *analog_clock,rgb_t COR)
{
	analog_clock->min_color=COR;
}


void analog_clk_SET_ANGULOS_PONTEIROS(clk_analog_t *analog_clock,timep_t time)
{
	analog_clock->angle[0]=6*time.sec;//SEGUNDOS
	analog_clock->angle[1]=6*time.min;//MINUTOS_CENTRO
	analog_clock->angle[2]=(6*time.min+1)%360;//MINUTOS_ESQUERDA
	analog_clock->angle[3]=(6*time.min-1)%360;//MINUTOS_DIREITA;
	analog_clock->angle[4]=(30*(time.hour%12))+(time.min>>1);//HORAS_CENTRO
	analog_clock->angle[5]=((analog_clock->angle[4])+1)%360;//HORAS_ESQUERDA
	analog_clock->angle[6]=((analog_clock->angle[4])-1)%360;//HORAS_DIREITA
}

void analog_clk_GET_LEDS_PONTEIROS(clk_analog_t *analog_clock,rgb_t *train,short int theta)
{
	long int temp=0;
	short int i;
	
	if (theta==(analog_clock->angle[0]))
			temp= temp | 0x001FFFFF;
	else
		{		//Verifica se o angulo está contido nos minutos
					if (theta==(analog_clock->angle[1])) temp = temp | 0x0001FFFF; 
					if (theta==(analog_clock->angle[2])) temp = temp | 0x00004000; 
					if (theta==(analog_clock->angle[3])) temp = temp | 0x00004000;
				
				//Verifica se o angulo está contido nas horas
					if (theta==(analog_clock->angle[4])) temp = temp | 0x00000FFF; 
					if (theta==(analog_clock->angle[5])) temp = temp | 0x00000200; 
					if (theta==(analog_clock->angle[6])) temp = temp | 0x00000200; 
		}
	
	
	for (i=0;i<32;i++)
	{
		if (temp & 0x00000001)
			train[i]=(analog_clock->ptr_color);
		else
		{
			train[i].r=0;
			train[i].g=0;
			train[i].b=0;
		}
		temp=temp>>1;
	}
	
}

void analog_clk_GET_LEDS_BACKGROUND(clk_analog_t *analog_clock,rgb_t *train,short int theta,char *caract)
{
		coord_t temp,carc;
		short int raio;
		if ((theta%6)==0)
			train[31]=(analog_clock->min_color);
		
		if ((theta%30)==0)
			{
				train[30]=(analog_clock->hour_color);
				train[29]=(analog_clock->hour_color);
			}
		
		
		for (raio=22; raio<29; raio++)
			//if (isBlack(train[raio]))
				{
				
					temp=polar_2_rect(theta,raio);
					//HELDER_FRANCISCO_FERREIRA_LOBATO[0]=theta;
					//HELDER_FRANCISCO_FERREIRA_LOBATO[1]=raio;
					//HELDER_FRANCISCO_FERREIRA_LOBATO[2]++;
					//HELDER_FRANCISCO_FERREIRA_LOBATO[3]=temp.x;
					//HELDER_FRANCISCO_FERREIRA_LOBATO[4]=temp.y;
					if( (temp.y>29 && temp.y<37) && (temp.x>4 && temp.x<10) )
						{
							carc.x=temp.x-5;
							carc.y=temp.y-30;
							if (getPIXEL_CHAR('9',carc.x,carc.y,caract)  )
								train[raio]=(analog_clock->nbr_color);
							
						}
				
					if( (temp.x>55 && temp.x<62) && (temp.y>29 && temp.y<37) )
						{
							carc.x=temp.x-56;
							carc.y=temp.y-30;
							if (getPIXEL_CHAR('3',carc.x,carc.y,caract) )
								{train[raio]=(analog_clock->nbr_color);
								HELDER_FRANCISCO_FERREIRA_LOBATO[0]++;
								}
								HELDER_FRANCISCO_FERREIRA_LOBATO[1]++;
						}
						
					if( (temp.x>30 && temp.x<36) && (temp.y>54 && temp.y<62) )
						{
							carc.x=temp.x-31;
							carc.y=temp.y-55;
							if (getPIXEL_CHAR('6',carc.x,carc.y,caract) )
								train[raio]=(analog_clock->nbr_color);
						}
						
					if( (temp.x>27 && temp.x<33) && (temp.y>4 && temp.y<12) )
						{
							carc.x=temp.x-28;
							carc.y=temp.y-5;
							if (getPIXEL_CHAR('1',carc.x,carc.y,caract) )
								train[raio]=(analog_clock->nbr_color);
						}
						
					if( (temp.x>33 && temp.x<39) && (temp.y>4 && temp.y<12) )
						{
							carc.x=temp.x-34;
							carc.y=temp.y-5;
							if (getPIXEL_CHAR('2',carc.x,carc.y,caract) )
								train[raio]=(analog_clock->nbr_color);
						}				
				}
}


 

