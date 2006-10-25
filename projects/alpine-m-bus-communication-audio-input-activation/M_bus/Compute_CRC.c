/*CAVR********************************************
Projet : M BUS
Commentaires :
Version : 1
Date    : 16/12/2005
Heure : 15:45
Auteur  : Nicolas LE FEVRE
Compagnie: Nycos Corporation

Micro Contrôleur     : AT TINY 26
Clock frequency       : 16,000000 MHz
*********************************************/

#include <avr/io.h>
#include "M_BUS_Modules.h"

char Compute_CRC (char *Buffer_Ptr, unsigned char number_chars)									//count CRC, and put it
{												//in the last send location
	unsigned char temp =0;
	unsigned char n ;
	for (n=0;n<number_chars;n++)
	{
		temp = Buffer_Ptr[n] ^ temp;								//XOR all bytes
	}
	temp++;											                //and increment
	if (temp>15) temp=0;									//reset if too big

    return temp;
}
