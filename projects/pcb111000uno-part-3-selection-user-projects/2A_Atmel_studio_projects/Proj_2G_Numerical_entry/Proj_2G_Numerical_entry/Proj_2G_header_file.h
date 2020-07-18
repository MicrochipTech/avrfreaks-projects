

#include "../../Resources_V15/Header_files/Basic_HW_setup.h"
#include "../../Resources_V15/Subroutines/Basic_HW_plus_Timer.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_subroutines_1.c"
#include "../../Resources_V15/Subroutines/Basic_PC_comms.c"
#include "../../Resources_V15/I2C_Subroutines/I2C_slave_Rx_Tx.c"
#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"

void display_num_string (const char*, int);


                                           //Define 10 strings the elements of each of which are stored in an array
#define zero "abcdef"                       //chars a,b,c,d,e and f are stored in an array named "zero"
#define one "bc"                            //chars b and c are stored in an array named "one"
#define two "abged"                         //Note: the compiler stores each array in SRAM and terminates it in zero
#define three "abcdg"                       //Whenever the compiler sees the term zero it substitutes the address of the letter a 
#define four "fgbc"                         //Whenever the compiler sees the term one it substitutes the address of the letter b
#define five "afgcd"
#define six "fegdc"
#define seven "abc"
#define eight "abcdefg"
#define nine "abcfg"