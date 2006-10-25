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

/*************************** Loop_Ms ***********************************************/
void loop_ms(int ms)                                        //time loop => one loop = 1 ms
{
/*      Local data          */
    int boucle_ms;
/***********************/
        for(boucle_ms=0;boucle_ms<ms;boucle_ms++)
    {
    time_loop(100);
    }
}
