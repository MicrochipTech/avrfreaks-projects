

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/Header_files/clock_stop_watch.h"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"		

void Inc_display(void);
void set_time(void);

#define User_instructions \
String_to_PC(message_1);\
String_to_PC(message_2);


#define message_1 \
"\r\nSTOP WATCH: sw2 selects 100ms clock, sw1 a 10ms clock\r\n\
Press sw1 to pause SW and save time\r\n\
Press sw2 the read back saved times\r\n"

#define message_2 \
"While pressing sw2 press sw1 to restore SW\r\n\
Press sw3 to re-initialise SW\r\n"
