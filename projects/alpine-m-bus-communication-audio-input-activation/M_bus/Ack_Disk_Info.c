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
void Ack_Disk_Info(void)
{
Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=GIVE_DISK_INFO ;
// Disk
Trame_Locale[2]=0x01;
// ??
Trame_Locale[3]=0x00;
Trame_Locale[4]=0x01;
// Tracks
Trame_Locale[5]=0x09;
Trame_Locale[6]=0x09;
// Min
Trame_Locale[7]=0x05;
Trame_Locale[8]=0x05;
// Sec
Trame_Locale[9]=0x05;
Trame_Locale[10]=0x05;
// Flag 1
Trame_Locale[11]=0x00;
send_frame(&Trame_Locale[0],DISK_INFO_LENGTH);
}
/************************************ the end ************************************/
