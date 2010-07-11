#include  "../ATmega32/ATmega32_Timer2.h"

void ATmega32_Latido();
int Imprime_Valores( void);

uint16_t Tiempo;
int Contador=0;

uint16_t Pulsos= 1;


ISR( TIMER2_COMP_vect )
{
ATmega32_Latido();
}


void ATmega32_Latido()
{

if(Contador==0)
 {
	ATmega32_Led_L_Set_On;
      i2c_DS1631_Read_Temperature( __I2C_DS1631_TAD);
	Contador=1;
 }
else
 {
	ATmega32_Led_L_Set_Off;

	Contador=0;
 }
}

void ATmega32_Init_Diapason()
{
ATmega32_Init_Setup();
ATmega32_Set_Led_Out();
Init_RTC();
}


