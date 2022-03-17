/*CAVR********************************************
Projet : M BUS
Commentaires :
Version : 1
Date    : 02/03/2006
Auteur  : Nicolas LE FEVRE
Compagnie: Nycos Corporation

Micro Contr�leur     : AT TINY 26
Clock frequency       : 16,000000 MHz
*********************************************/

//************************************* #include **********************************
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "M_BUS_Modules.h"
#include "M_BUS_data.h"
//************************************ main program *******************************
int main (void)
{
    int Compteur_1 =0;
    int Compteur_2 =0;
    init();                                         //initialization

    // Tache de fond (Boucle infinie)
    for(;;)
    {
    if (received == 1)                                    // Si on a re�u une donn�e et que 1er caract�re est "1" alors on teste la commande re�ue
        {
		if ((*R_BUS)==1)                              // Si la requ�te vient de l'autoradio (robustesse)
			{
	        Decode_Received();
			}
        received = 0;                                      //reset M-Bus receive flag
        }
		// en t�che de fond quand on a rien re�u et qu'on est en mode lecture, on envoi les info � l'autoradio
	if ( (Actual_Play_State	== 0x01) && ( Compteur_2 >= 25) )    // 25 -> (25 * 16 000) * (18i * 0.0625�s) = 450 ms
		{
			Ack_Play_State (Last_Play_State, Actual_Repeat_Mode, Actual_Prog_Mode, Actual_Play_State);
			Compteur_1 = 0;
			Compteur_2 = 0;
		}
	Compteur_1 ++;
	if (	Compteur_1 >= 16000 )   // 16 000 -> 18ms pour 18 instructions
		{
			Compteur_1 = 0;
			Compteur_2 ++;
		}
	}
}

/************************************ the end ************************************/
