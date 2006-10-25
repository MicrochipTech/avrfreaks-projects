/*CAVR********************************************
Projet : M BUS
Commentaires :
Version : 1
Date    : 02/03/2006
Auteur  : Nicolas LE FEVRE
Compagnie: Nycos Corporation

Micro Contrôleur     : AT TINY 26
Clock frequency       : 16,000000 MHz
*********************************************/

//************************************* #include **********************************
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "M_BUS_Modules.h"
#include "M_BUS_data.h"
//************************************ main program *******************************
void Decode_Received(void)
{
	Request_Id=R_BUS[1];
	switch (Request_Id)
	{
		case SETTTING:
				Commande_Type=R_BUS[2];

				switch (Commande_Type)
				{
					case SET_PLAY_STATE:
						Actual_Play_State	=  ( (R_BUS[3] & 0x04)*2 ) | ( R_BUS[4] & 0x03 );
						Ack_Play_State (Last_Play_State, Actual_Repeat_Mode, Actual_Prog_Mode, Actual_Play_State);
						// Mise à jour de l'état de lecture actuel - Pour le moment on cinsidère qu'on est toujours en mode Play
						//Actual_Play_State = ;
					break;

					case SET_MEDIA:
						// Changement de Disque OK
						Ack_Change_Disk();
						// Attendre 100 ms
						loop_ms(100);
						// Status Disque
						Ack_Disk_Info();
						// Attendre 100 ms
						loop_ms(100);
						// Play State
						Ack_Play_State (Last_Play_State, Actual_Repeat_Mode, Actual_Prog_Mode, Actual_Play_State);
					break;

					case SET_PLAY_MODE :
						// Il n'y a pas de réponse particulière à cette requête.
						Actual_Repeat_Mode = R_BUS[3];
						Actual_Prog_Mode = R_BUS[4];
					break;

					default:
						Ack_Wait();
					break;
				}
		break;
		case PING:                                                // PING
				ping_ok();
		break;
		case POWER_UP:                                  // POWER_UP
				Ack_Power_Up();
		break;
		default:
				Ack_Wait();
		break;
	}
    received = 0;                               //reset M-Bus receive flag
}
/************************************ the end ************************************/
