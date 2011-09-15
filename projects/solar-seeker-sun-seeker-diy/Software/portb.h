/*
	Libreria para AVRStudio que simplifica la asignación de bits
	en puertos y registros.
	
	Autor: J.D. Hdez Hdez
	E-mail: cuninganreset@gmail.com
	Web:	www.simplificate.es
*/

#ifndef fportb_h
#include <stdbool.h>
	#define fportb_h
	#define OUTPUT true	
	#define HIGH true
	#define INPUT false
	#define LOW false
	// Funcion para controlar el registro PORTB
	void setportb(uint8_t pin,bool s)
	{
		if(pin<8)
		{
			if(s==HIGH)
			{
				PORTB=(PORTB | 1<<pin);
			}
			else
			{
				PORTB=(PORTB & ~(1<<pin));
			}
		}
	}
	// Funcion para controlar el registro DDRB
	void setddrb(uint8_t pin,bool s)
	{
		if(pin<8)
		{
			if(s==OUTPUT)
			{
				DDRB=(DDRB | 1<<pin);
			}
			else
			{
				DDRB=(DDRB & ~(1<<pin));
			}
		}
	}
	// Establece a 0 todos los registros.
	void clearb()
	{
		PORTB = 0x00;
		DDRB = 0x00;
	}
#endif