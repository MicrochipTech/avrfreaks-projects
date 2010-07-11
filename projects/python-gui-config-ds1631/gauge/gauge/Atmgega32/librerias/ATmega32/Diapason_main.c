void Arduino_Nano_Latido();
int Imprime_Valores( void);

int Contador=0;

uint16_t Pulsos= 1;

ISR( TIMER2_COMPA_vect )
{
switch ( Pulsos)
 {
	case 250 : // 250=> 0.5 seg
		Pulsos= 1;
		Arduino_Nano_Latido();
	break;

	default:
		Pulsos++;
	break;
 }


}

void Arduino_Nano_Latido()
{
if(Contador==0)
 {
  Arduino_Nano_Led_L_Set_On;
  Imprime_Datos();
  Contador=1;
 }
else
 {
  Arduino_Nano_Led_L_Set_Off;
  Contador=0;
 }
}

void Arduino_Nano_Init_Diapason_Main()
{
Arduino_Nano_Init_Setup();
Arduino_Nano_Set_Led_Out();
ATmega168_Timer2_Stop;
ATmega168_Timer2_Wave_Form_Generation_CTC;
ATmega168_Timer2_Compare_Match_Output__A__Mode_CTC__Normal;
ATmega168_Timer2_Interrupt_Set_On_Output_Compare_Match__A;
ATmega168_Timer2_Clock_Prescaling__256;
OCR2A= 125; // Tenemos una interrupción cada 2 msg.
}

int Imprime_Datos( void)
{
char ADC_String[ 32];
ATmega168_ADC_Start_Conversion;

 while( !( ADCSRA & ( 1<< ADIF))) {}
 sprintf( ADC_String, "%6d\n", (uint16_t) ADC);
 uart_puts( ADC_String);
}


