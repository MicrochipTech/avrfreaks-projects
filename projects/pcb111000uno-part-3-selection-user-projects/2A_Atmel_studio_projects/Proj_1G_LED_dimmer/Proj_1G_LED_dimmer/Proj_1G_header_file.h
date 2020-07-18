

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"

void initialise_display(void);
void Inc_Display(void);
void I2C_Tx_local(char, char, char*);
