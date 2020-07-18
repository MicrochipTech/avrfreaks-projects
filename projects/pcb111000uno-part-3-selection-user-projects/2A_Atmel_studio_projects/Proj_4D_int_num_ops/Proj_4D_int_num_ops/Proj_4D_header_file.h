


#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"

void int_plus_short_from_IO(void);
void int_arithmetic_sw3_isr(void);
void enter_number(void);

#define Op_1 'P'
#define Op_2 'R'
#define inc_op  if (Op=='P') Op='R'; else Op='P';


#define message_1 \
"\r\n\r\n\
INTEGER NUMBER Ops\r\n\r\n\
Press sw1 for roots or sw2 for powers.\r\n\
Hold down sw3 to populate the first digit of the display."


#define message_2 \
"\r\nRelease sw3 when the correct number is being displayed to move on to the next digit.\r\n\r\n"

#define message_3 \
"When final digit is being entered press sw1 before releasing sw3 then release sw1.\r\n\
Press sw3 for the next number (only one digit is allowed this time).\r\n"

#define message_4 \
"Release sw3 and root or power will be calculated.\r\n\
Pulse sw3 to repeat calculation.\r\n\r\n\
Hold sw1 or sw2 down while pulsing sw3 then press it again to reset the device.\r\n"



#define User_instructions \
String_to_PC(message_1);\
String_to_PC(message_2);\
String_to_PC(message_3);\
String_to_PC(message_4);

