#include <built_in.h>
unsigned int adc_rd,setting;
char current_duty;
float adc;
char adcr[10];

void main() {
  ADC_Init();
  ADCSRA= (ADCSRA && 0xF8);
  setting = ADMUX;

  UART1_Init(115200);               // Initialize UART module at 9600 bps
  //UART1_Init_Advanced(115200, _UART_ODDPARITY, _UART_ONE_STOPBIT);
  Delay_ms(100);                  // Wait for UART module to stabilize
  UART1_Write_Text("Init");
  UART1_Write(13);UART1_Write(10);

  adc = ADC_Read(0);    // get ADC value from 1 channel
  adc = adc * 5 /1024;
  FloatToStr(adc, adcr);
  UART1_Write_Text(adcr);
  UART1_Write(13);UART1_Write(10);

  UART1_Write_Text("ADCSRA");
  UART1_Write(13);UART1_Write(10);
  DDB3_bit = 1;
  DDD7_bit = 1;
  PWM1_Init(_PWM1_FAST_MODE, _PWM1_PRESCALER_1, _PWM1_NON_INVERTED, 127);
  PWM2_Init(_PWM1_FAST_MODE, _PWM1_PRESCALER_1, _PWM1_NON_INVERTED, 200);
  PWM1_start();
  PWM2_start();

while(1){

  adc = ADC_Read(0);    // get ADC value from 1 channel
  adc = adc * 5 /1024;
  FloatToStr(adc, adcr);
  UART1_Write_Text(adcr);
  UART1_Write(13);UART1_Write(10);


}
}