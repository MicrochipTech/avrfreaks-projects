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
#include <avr/io.h>
#include "M_BUS_Modules.h"
#include "M_BUS_data.h"

/****************************Ack_Wait***********************************************/
/*********************************************************************************/
void  Ack_Wait(void)
{
Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=ACK_WAIT ;
Trame_Locale[2]=0x00;
Trame_Locale[3]=0x00;
Trame_Locale[4]=0x00;
Trame_Locale[5]=0x00;
Trame_Locale[6]=0x00;

send_frame(&Trame_Locale[0],ACK_WAIT_LENGTH);
}
