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

/****************************Time_Loop ***********************************************/
void time_loop(int tenmicros)                                       //time loop => one loop = 10 us
{
    for(;tenmicros>0;tenmicros--)
    {
        asm volatile ("push r1""\n\t"
                "pop r1""\n\t");
    }
}
