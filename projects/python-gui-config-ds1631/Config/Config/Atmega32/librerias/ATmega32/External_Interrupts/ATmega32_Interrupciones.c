#include "./ATmega32_Interrupciones.h"
#include "./ATmega32_Manejo_Interrupciones_Externas.c"


void ATmega32_Set_INT0_Low_Level()
{
ATmega32_Interrupts_Int0_Set_Disabled;
ATmega32_Interrupts_Set_Int0_Request_Low_Level;
ATmega32_Interrupts_Int0_Set_Enabled;

}


void ATmega32_Set_INT0_Rising_Edge()
{
ATmega32_Interrupts_Int0_Set_Disabled;
ATmega32_Interrupts_Set_Int0_Rising_Edge;
ATmega32_Interrupts_Int0_Set_Enabled;
}

void ATmega32_Set_INT0_Falling_Edge()
{
ATmega32_Interrupts_Int0_Set_Disabled;
ATmega32_Interrupts_Set_Int0_Falling_Edge;
ATmega32_Interrupts_Int0_Set_Enabled;
}


