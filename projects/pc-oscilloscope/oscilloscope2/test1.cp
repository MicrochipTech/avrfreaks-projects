#line 1 "C:/Documents and Settings/Administrator/Desktop/oscilloscope2/test1.c"
#line 1 "c:/program files/mikroelektronika/mikroc pro for avr/include/built_in.h"
#line 2 "C:/Documents and Settings/Administrator/Desktop/oscilloscope2/test1.c"
unsigned int adc_rd,setting;
char current_duty;
float adc;
char adcr[10];

void main() {
 ADC_Init();
 ADCSRA= (ADCSRA && 0xF8);
 setting = ADMUX;

 UART1_Init(115200);

 Delay_ms(100);
 UART1_Write_Text("Init");
 UART1_Write(13);UART1_Write(10);

 adc = ADC_Read(0);
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

 adc = ADC_Read(0);
 adc = adc * 5 /1024;
 FloatToStr(adc, adcr);
 UART1_Write_Text(adcr);
 UART1_Write(13);UART1_Write(10);


}
}
