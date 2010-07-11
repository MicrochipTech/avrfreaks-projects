/*------------------ Tarjeta Arduino Nano ---------  Configuración del Led L--------------------------------------------------*/


#define ATmega32_Led_L_Direction    DDRB
#define ATmega32_Led_L_Data         PORTB
#define ATmega32_Led_L_Pins_Pin     PINB
#define ATmega32_Led_L_Pin          0

#define ATmega32_Led_2_Direction    DDRB
#define ATmega32_Led_2_Data         PORTB
#define ATmega32_Led_2_Pins_Pin     PINB
#define ATmega32_Led_2_Pin          1



#define ATmega32_Led_L_Set_Out ATmega32_Led_L_Direction |= _BV( ATmega32_Led_L_Pin)
#define ATmega32_Led_L_Set_On  ATmega32_Led_L_Data      |=  _BV( ATmega32_Led_L_Pin)
#define ATmega32_Led_L_Set_Off ATmega32_Led_L_Data      &= ~_BV( ATmega32_Led_L_Pin)

#define ATmega32_Led_2_Set_Out ATmega32_Led_2_Direction |=  _BV( ATmega32_Led_2_Pin)
#define ATmega32_Led_2_Set_On  ATmega32_Led_2_Data      |=  _BV( ATmega32_Led_2_Pin)
#define ATmega32_Led_2_Set_Off ATmega32_Led_2_Data      &= ~_BV( ATmega32_Led_2_Pin)

volatile uint8_t *Puntero_OCR0;
volatile uint8_t *Puntero_OCR2;
volatile char *Temp[ 6];

void ATmega32_Set_Led_Out()
{
ATmega32_Led_L_Set_Out;
ATmega32_Led_2_Set_Out;

}

void ATmega32_Init_Setup()
{
Puntero_OCR0= &OCR0;
Puntero_OCR2= &OCR2;
}
