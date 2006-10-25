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
#include "M_BUS_data.h"

/* Reception Bus Buffer */
char R_BUS[16]={};

/* Sending Bus Buffer */
char S_BUS[16]={};


volatile unsigned char received;
volatile unsigned char bus_byte = 0;
volatile unsigned char n_bit = 0;
volatile unsigned char n_byte = 0;
volatile unsigned char bus_busy = 0;
volatile unsigned char s_flag = 0;
volatile unsigned char Last_Play_State = 4; // Playing par défaut
volatile unsigned char Actual_Play_State = 0x01; // Playing par défaut
volatile unsigned char Actual_Repeat_Mode = 0; // Mode de répétition (Aucune, 1 fois la piste, répéter tout)
volatile unsigned char Actual_Prog_Mode = 0; // Mode de programme (Aléatoire, Intros)
/*  */

char Request_Id=0;
char Commande_Type=0;
char Trame_Locale[16]={};
