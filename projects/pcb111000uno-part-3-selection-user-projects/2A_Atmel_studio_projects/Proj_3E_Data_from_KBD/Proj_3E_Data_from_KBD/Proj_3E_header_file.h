

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_2.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/Subroutines/Integers_to_from_PC.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"

long Num_from_KBD_Local(char*);
void Num_to_PC_local(char, long);
void SBtoAL_local(char*, long, char);
void NumericString_to_PC_local(char*);

