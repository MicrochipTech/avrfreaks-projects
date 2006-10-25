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

/*    DEFINITION DES CONSTANTES   */
// RECEPTION
/* Commande */
#define SETTTING                      0x01
#define PING                                 0x08
#define POWER_UP                    0x09

/* Command Type */
#define SET_PLAY_STATE      0x01
#define SET_MEDIA                  0x03
#define SET_PLAY_MODE       0x04
// EMISSION
#define RESPONSE_START                0x09
#define PING_OK                                   0x08
#define GIVE_PLAY_STATE               0x09
#define GIVE_CHANGING_INFO        0x0B
#define GIVE_DISK_INFO				   0x0C
#define ACK_WAIT                              0x0F
// Frame LENGTH (without CRC)
#define PLAY_STATE_LENGTH    			15
#define PING_OK_LENGTH              		2
#define ACK_WAIT_LENGTH         		7
#define  CHANGING_INFO_LENGTH	11
#define DISK_INFO_LENGTH                  12

/*    SECTION DEFIINITION   */
/* Reception Bus Buffer */
extern char R_BUS[];

/* Sending Bus Buffer */
extern char S_BUS[];


volatile unsigned char received;
volatile unsigned char bus_byte;
volatile unsigned char n_bit;
volatile unsigned char n_byte;
volatile unsigned char bus_busy;
volatile unsigned char s_flag;
volatile unsigned char Actual_Play_State;
volatile unsigned char Last_Play_State;
volatile unsigned char Actual_Repeat_Mode;
volatile unsigned char Actual_Prog_Mode;

extern char Request_Id;
extern char Commande_Type;
extern char Trame_Locale[];
