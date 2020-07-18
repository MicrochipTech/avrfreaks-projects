
#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"
#include "../../Resources_V15/Subroutines/Random_and_prime_nos.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"



#define flash_leds \
\
{I2C_Tx_2_integers(PORT_1, 0);\
Timer_T0_10mS_delay_x_m(16);\
I2C_Tx_2_integers(0, PORT_2);\
Timer_T0_10mS_delay_x_m(16);}



#define Rotate_Port_1 \
\
if (PORT_1 == 0b1000000000000000)\
PORT_1 = 0b0000000000000001;\
else PORT_1 = PORT_1 << 1; 



#define Rotate_Port_2 \
\
if (PORT_2 == 0b1000000000000000)\
PORT_2 = 0b0000000000000001;\
else PORT_2 = PORT_2 << 1; 

