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
void Ack_Power_Up(void)
{
Ack_Disk_Info();

// Attendre 100 ms
loop_ms(100);

Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=0x0A ;
Trame_Locale[2]=0x00;
Trame_Locale[3]=0x00;
Trame_Locale[4]=0x00;
Trame_Locale[5]=0x00;
Trame_Locale[6]=0x00;
Trame_Locale[7]=0x00;
Trame_Locale[8]=0x00;
Trame_Locale[9]=0x00;
Trame_Locale[10]=0x00;
Trame_Locale[11]=0x00;
send_frame(&Trame_Locale[0],12);

// Attendre 100 ms
loop_ms(100);

Trame_Locale[0]=RESPONSE_START;
Trame_Locale[1]=0x0A ;
Trame_Locale[2]=0x00;
Trame_Locale[3]=0x00;
Trame_Locale[4]=0x00;
Trame_Locale[5]=0x0D;
Trame_Locale[6]=0x05;
Trame_Locale[7]=0x05;
Trame_Locale[8]=0x05;
Trame_Locale[9]=0x05;
send_frame(&Trame_Locale[0],10);

// Attendre 100 ms
loop_ms(100);

Ack_Change_Disk();
}
/************************************ the end ************************************/
