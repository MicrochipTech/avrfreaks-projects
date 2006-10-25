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
void send_frame(char *Frame_Char_Ptr,int Frame_Lenght)
{
int i;
for (i =0; i < Frame_Lenght; i++)
    {
    S_BUS[i]=Frame_Char_Ptr[i];
    }
// Compute the CRC
S_BUS[Frame_Lenght]=Compute_CRC (&(S_BUS[0]), Frame_Lenght);
Frame_Lenght++;
// Send the frame
send(Frame_Lenght);
}
