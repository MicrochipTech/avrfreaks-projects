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

/****************************Ping_Ok***********************************************/
/*********************************************************************************/
void ping_ok(void)
{
Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=PING_OK;

send_frame(&Trame_Locale[0],PING_OK_LENGTH);
}
