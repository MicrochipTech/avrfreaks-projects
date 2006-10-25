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
#include "M_BUS_Modules.h"
#include "M_BUS_data.h"
//*********************************************************************************
void Ack_Change_Disk(void)
{
Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=GIVE_CHANGING_INFO ;
Trame_Locale[2]=0x09;  // CHANGING OK
//Disk
Trame_Locale[3]=0x01;
// Track
Trame_Locale[4]=0x00;
Trame_Locale[5]=0x01;
// Flag 1
Trame_Locale[6]=0x00;
// Flag 2
Trame_Locale[7]=0x00;
// Flag 3
Trame_Locale[8]=0x00;
// Flag 4
Trame_Locale[9]=0x00;
// Flag 5
Trame_Locale[10]=0x01; // Done

send_frame(&Trame_Locale[0],CHANGING_INFO_LENGTH);
}
/************************************ the end ************************************/
