

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"	
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"



#define message_1 \
"\r\n\
Press sw2 to populate digits[0]\r\n\
sw1 to shift display left\r\n\
sw3 to enter the number\r\n\
sw2 to multiply the number by 10\r\n\
sw1 to divide it by 10\r\n\
press sw3 before sw1 or 2 to repeat\r\n"
