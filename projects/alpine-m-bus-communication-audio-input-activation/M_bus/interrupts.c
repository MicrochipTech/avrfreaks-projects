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
#include <avr/interrupt.h>
#include "M_BUS_Modules.h"
#include "M_BUS_data.h"

/*********************************************************************************/
/* Ce handler se déclenche sur front du signal d'entrée                                                                     */
/*********************************************************************************/
SIGNAL (SIG_INTERRUPT0)                                     //INT0 interrupt handler
{
    bus_busy = 1;                                                                   //set flag "bus is busy"
	/* -----------------------------------------------------------------------------------------------------------------  */
	/* Le front montant désigne le début d'un bit.                                                                                 */
    /*  Sur un front montant du signal (Entrée lue à 1) :                                                                       */
	/*  1 - On inhibe l'interruption INTO                                                                                              */
	/*  2 - On paramètre le déclenchement de l'interruption sur front descendant du signal           */
	/*  3 - On réactive l'interruption                                                                                                     */
	/*  4 - On démarre le timer T0 en lui donnant Commentaires valeur 186.                                    */
	/*  5 - On paramètre le prescaler à CLK/64, ce qui redémarre le compteur                                */
	/* ----------------------------------------------------------------------------------------------------------------- */
    if (PINB & 0x40)                                        //if INT0 = 1
    {
        GIMSK &= 0xbf;                                  //INT0 interrupt disable
        MCUCR = 0x02;                                   //INT0 falling edge sensitive
        GIMSK |= 0x40;                                  //INT0 interrupt enable
        TCNT0 = 186;                                    //start T0 with the value
        TCCR0 = 0x0b;                                   //and prescaler CLK/64 (reset prescaler)
    }
	/* -----------------------------------------------------------------------------------------------------------------  */
	/* Le front descendant désigne la fin d'un bit.                                                                               */
    /*  Sur un front descendant du signal (Entrée lue à 0) :                                                                 */
	/*  1 - On évalue la durée du bit pour déterminer si c'est un bit 0 ou un bit 1                              */
	/*  2 - On enregistre le bit dans le queue de reception                                                                  */
	/* ----------------------------------------------------------------------------------------------------------------- */
    if (!(PINB & 0x40))                                     //if INT0 = 0
    {
        if (TCNT0 > 208)                                   // Si la durée du front > 1.3 ms alors c'est un 1
			{
			bus_byte ++;
			}
        bus_byte = (bus_byte << 1);                             //shift byte left
        n_bit++;                                        //inc bit number
        if (n_bit == 4)                                 //if bit number is 4
        {
            n_bit = 0;                                  //reset bit number
            bus_byte = (bus_byte >> 1);                         //correction of shifting
            *(R_BUS + n_byte)=bus_byte;                     //put the byte in table
            bus_byte = 0;                               //reset byte
            n_byte++;                               //inc byte number

			// robustesse : La taille de R_BUS est de 16
            if (n_byte == 16)                               //if byte number is 16
            {
                n_byte = 0;                         //reset byte number
                received = 1;                           //set M-Bus receive flag
                bus_busy = 0;                           //reset flag "bus is busy"
            }
        }
    GIMSK &= 0xbf;                                      //INT0 interrupt disable
    MCUCR = 0x03;                                       //INT0 rising edge sensitive
    GIMSK |= 0x40;                                      //INT0 interrupt enable
    }
}

/* -----------------------------------------------------------------------------------------------------------------  */
/* Le SIGNAL_OVERFLOW indique que le compteur a dépassé sa valeur maximale                 */
/* A rappeler que l'overflow n'arrive que si un front montant à été vu, donc quand un bit        */
/*  a été reçu (un bit commençant par un front montant.                                                                */
/* Normalement quand une trame est reçue, les bit s'enchainent donc l'overflow n'est en        */
/* en principe possible que lorque l'autoradio à terminé sa trame                                                */
/* ------------------------------------------------------------------------------------------------------------------ */
SIGNAL (SIG_OVERFLOW0)                                  //T0 overflow interrupt handler
{
    TCCR0 = 0x00;                                       //stop T0
    n_bit = 0;                                          //reset bit number
    n_byte = 0;                                     //reset byte number
    bus_byte = 0;                                       //reset byte
    received = 1;                                       //set M-Bus receive flag
    bus_busy = 0;                                       //reset flag "bus is busy"
}

/*********************************************************************************/
SIGNAL (SIG_OUTPUT_COMPARE1B)                                           //T1 compare 1B interrupt handler
{
    s_flag = 1;                                     //set flag
}
