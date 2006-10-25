/*CAVR********************************************
Projet : M BUS
Commentaires :
Version : 1
Date    : 16/12/2005
Heure : 15:21
Auteur  : Nicolas LE FEVRE
Compagnie: Nycos Corporation

Micro Contrôleur     : AT Tiny 26
Clock frequency       : 8,000000 MHz
*********************************************/

#include <avr/io.h>

/*  EXTERNAL DECLARATION OF FUNCTIONS  */

/*                                                                          Init */
extern void init(void);

/*                                                        Compute_CRC  */
extern char Compute_CRC(char *Buffer_Ptr, unsigned char number_chars);

/*                                                                       send */
extern void send(unsigned char nmbr) ;


/*                                                          send_frame */
extern void send_frame(char *Frame_Char_Ptr,int frame_lenght);

/*                                                                ping_ok */
extern void ping_ok(void);

/*                                                            Ack_Wait */
 extern void Ack_Wait(void);

/*                                                 Ack_Play_State */
extern void Ack_Play_State(char  Play_State, char Flag_1, char Flag_2, char Flag_3);

/*                                                 Ack_Change_Disk */
extern void Ack_Change_Disk(void);

/*                                                 Ack_Disk_Info */
extern void Ack_Disk_Info(void);

/*                                                 Decode_Received */
extern void Decode_Received(void);

/*                                                 loop_ms */
extern void loop_ms(int ms);

/*                                                 time_loop */
extern void time_loop(int tenmicros);

/*                                                 Ack_Power_Up */
extern void Ack_Power_Up(void);
