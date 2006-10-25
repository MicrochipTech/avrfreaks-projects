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
void Ack_Play_State(char  Play_State, char Flag_1, char Flag_2, char Flag_3)
{
Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=GIVE_PLAY_STATE ;
Trame_Locale[2]=Play_State;
//Track
Trame_Locale[3]=0x00;
Trame_Locale[4]=0x01;
// Index
Trame_Locale[5]=0x00;
Trame_Locale[6]=0x01;
// Min
Trame_Locale[7]=0x00;
Trame_Locale[8]=0x00;
// Sec
Trame_Locale[9]=0x00;
Trame_Locale[10]=0x01;
// Flag_1
Trame_Locale[11]=Flag_1;
// Flag_2
Trame_Locale[12]=Flag_2;
// Constante a 0x00
Trame_Locale[13]=0x00;
// Flag_3
Trame_Locale[14]=Flag_3;

send_frame(&Trame_Locale[0],PLAY_STATE_LENGTH);
}
/************************************ the end ************************************/
