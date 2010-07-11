#include </home/chus/avr/AVR/Revisor_De_Flectores/librerias/Timer0.h>

void Lectura_Potenciometros();
int Get_sample( int Canal);
void Ajuste_Valores_Ton_Toff();
int Leer_Tecla();

int Lectura_Potenciometros_Contador=0;
int Pasos=0 ;

int Tecla_Anterior= 0;
int Pulsada= 0;
int Ejecuta_Accion= 0;

ISR(TIMER0_COMP_vect)
{
//Lectura_Potenciometros();
Ajuste_Valores_Ton_Toff();
}


void Init_Timer0_Normal_Mode(void)
{
/*
F_CPU= 1 MHz
Timer0 is a 8 bit counter ( max. 255)
	1	 8	  32	    64	     128       256       1024
Tmin	1E-06	 8E-06	  3,2E-05   6,4E-05  0,000128  0,000256	 0,001024
tmax	0,000256 0,002048 0,008192  0,016384 0,032768  0,065536	 0,262144
*/
Timer0_Stop_Timer;
Timer0_WaveForm_Generation_Mode_CTC;
Timer0_Set_Clock_Preescaling_1024;
Timer0_Interrupt_Enable_Output_Compare_Match;
OCR0= 2; 
}


void Lectura_Potenciometros()
{

if(Contador==0)
 {
  Ton= (( 0.048828125 * Get_sample( 2)) +0) * 0.125;
  Lectura_Potenciometros_Contador=1;
 }
else
 {
  Toff= Get_sample( 1) * 0.125;
  Lectura_Potenciometros_Contador=0;

 }
}


int Get_sample( int Canal)
{


	 ADMUX= Canal; 
	 ADCSRA=0xC0;     //init conversion
	 loop_until_bit_is_clear(ADCSRA, ADSC);

	return ADC; 
}


void Ajuste_Valores_Ton_Toff()
/*
 Según la tecla que esté pulsada se ajustan los valores de Ton y Toff
  Boton_1 -> Toff -
  Boton_2 -> Toff +
  Boton_3 -> Ton  -
  Boton_4 -> Ton  +
  
  Los límites son:
    Ton 
      Mínimo= 10
      Máximo= 50
    Toff
      Mínimo= 0
      Máximo= 255
      
 En caso de que llegue a mínimo o máximo se queda ahí, no se hace ciclo de mínimo a máximo ni viceversa
*/
{
int Tecla_Leida;

Tecla_Leida= Leer_Tecla();

if ( !Tecla_Leida== 0)
{
switch ( Tecla_Leida)
{

case Boton_1:
	 if ( Toff> 1)
	  {
	   Toff = Toff- 1;
	  }
	break;
case Boton_2:
	if ( Toff< 254)
	 {
	  Toff = Toff+ 1;
	 }
	break;
case Boton_3:
	if ( Ton > 1)
	 {
	  Ton= Ton -1 ;
	 }
	break;
case Boton_4:
	if ( Ton< 50)
	 {
	  Ton= Ton+ 1 ;
	 }
	break;
} // switch Tecla_Leida
Tarjeta_Externa_Led_01_Off;
} // if Tecla_Leida> 0

}


int Leer_Tecla()
{

/*
Se asegura de leer el valor de una pulsación de tecla, y evita el rebote.

Boton_x
   |
   |             Tecla On
   |            --------------
   |            |            |
   |            |            |
   |            |            |
   | Tecla Off  |            | Tecla Off
   |------------|            |------------
   ----------------------------------------->
                                         Tiempo 
*/

Tecla= KeyBoard_Code();

if ( !Tecla== 0 && Pulsada== 0 && Tecla_Anterior== 0)
 {
  Pulsada= 1;
  Tecla_Anterior= Tecla;
/*
  Init_Timer1(); // Inicia el timer1 para ver que acción se toma sobre la pulsación de una tecla.
*/
 }

if ( Tecla== 0 && Pulsada== 1 && Ejecuta_Accion== 0)
 {
  /*Aquí se puede meter un manejador de tiempo através del timer1. De tal manera que en vez de poner a 1 Ejecuta_Accion de golpe
    se lance el timer1, con una duración determinada. Dicha duración nos permitirá seleccionar otras opciones.
    
    Por ejemplo:
     timer1_pasos= 1 -> Acción de aumentar la velocidad de contaje para las teclas
     timer1_pasos= 5 -> Indica reset de valores a valores por defecto
     timer1_pasos= 20 -> indica grabación del valor en eeprom y ajuste a valor por defecto
     etc...
  */
  
  Ejecuta_Accion= 1;  

 }

if ( Ejecuta_Accion== 1)
 {
  Tarjeta_Externa_Led_01_On;
  Pulsada= 0;
  Ejecuta_Accion= 0;
  Tecla= Tecla_Anterior;
  Tecla_Anterior= 0;
  return Tecla;
 }

}

