#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "AMiR_Init.h"


//*****************************************************************
//                 -----------(  Light  )-------------
//-----------------------------------------------------------------
void led_red(uint8_t flag){
	if (flag)
		PORTB |= (1<<Red_LED);
	else
		PORTB &= ~(1<<Red_LED);
}
void led_green(uint8_t flag){
	if (flag)
		PORTB |= (1<<Green_LED);
	else
		PORTB &= ~(1<<Green_LED);
}
//*****************************************************************
//                 -----------(  UART  )-------------
//-----------------------------------------------------------------
void uart_putchar(char c)
{
/*  if (c == '\a'){
      fputs("*ring*\n", stderr);
      return 0;
  }
  if (c == '\n')
    uart_putchar('\r', stream);
  */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  //return 0;
}
//           -(  Print Uart )-
void write_rs232(char *s){
	for (;*s;s++)
		uart_putchar(*s);
}
int	uart_getchar(FILE *stream);

//*****************************************************************
//                 -----------( ADC UNIT )-------------
//-----------------------------------------------------------------
uint16_t read_adc(uint8_t channel)
{
	uint16_t adc_value;
//	uint8_t ch;
//	ch=channel;
	//set ADC channel
	ADMUX=(ADMUX&0xF0)|channel;
	//Start conversionio with Interupt after conversion
	_delay_us(10);
	//ADCSRA |= (1<<ADSC)|(1<<ADIE);
	ADCSRA|=0x40;
	
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	
	adc_value = ADCL;  
	adc_value += (ADCH<<8);
	return adc_value;
}
void send_adc_samples(){
        uint8_t i,buf[10];
		for (i=0;i<6;i++){
            sprintf(buf," %d=%d ",i,read_adc(i));
			write_rs232(buf);
		}
		uart_putchar(10);
		uart_putchar(13);
}
uint8_t battery_level(){
		uint8_t i;
        uint16_t bat;    
		float x=0;
		for (i=0;i<5;i++){
			x+=read_adc(battery_sample);
			_delay_ms(10);
		}
		x/=5;
        bat= x/17;
        return bat;
}

//*****************************************************************
//                 -----------( Kinematic )-------------
//-----------------------------------------------------------------

//              - Motors Activate -
void motor_left_active(uint8_t enable){
	if (enable)
		PORTB &= ~(1<<MotorL_en);   // 0=Enable
	else
		PORTB |= (1<<MotorL_en);    // 1=Disabe
}
void motor_right_active(uint8_t enable){
	if (enable)
		PORTB &= ~(1<<MotorR_en);
	else
		PORTB |= (1<<MotorR_en);
}
//           - PWM Output Setting -
void pwm_left(uint8_t Direction,uint16_t spd){
		if (Direction){
                OCR1AH=spd/256;
                OCR1AL=spd%256;
        }else{
                spd=0x3ff-spd;
                OCR1AH=spd/256;
                OCR1AL=spd%256;
        }                
}
void pwm_right(uint8_t Direction,uint16_t spd){
		if (Direction){
                OCR1BH=spd/256;
                OCR1BL=spd%256;
        }else{
                spd=0x3ff-spd;
                OCR1BH=spd/256;
                OCR1BL=spd%256;
        }                
}
//           - Motors Speed Setting -
void motor_left_speed(uint8_t Direction,uint8_t speed){
	uint16_t spd=0;
	switch (speed){
    	case 0: spd= 0x1FF ; break;   // Stop
        case 1: spd= 0x100 ; break;   // Half   //E0
        case 2: spd= 0x0F ; break;    // Max
    }
	pwm_left(Direction,spd);
}
void motor_right_speed(uint8_t Direction,uint8_t speed){
	uint16_t spd=0;
	switch (speed){
    	case 0: spd= 0x1FF ; break;   // Stop
        case 1: spd= 0x100 ; break;   // Half   //E0
        case 2: spd= 0x0F ; break;    // Max
    }
	pwm_right(Direction,spd);
}
//           -  Rotation -
void turn_in_place(uint16_t degree,uint8_t direction){
	float N;
	N=(float)degree/360;
	led_red(1);
	uint16_t rotate_time;
	uint8_t bat;
	// Speed (N)= V(bat)*12.68;
	bat=battery_level(); // speed is directly depend on battery level

	rotate_time= N*96116/bat;
	if (direction){
		motor_left_speed(0,2);
		motor_right_speed(1,2);
	} else{
		motor_left_speed(1,2);
		motor_right_speed(0,2);
	}
	_delay_ms(rotate_time);
	led_red(0);
}
void rotate_large_axis(float Radius,uint8_t degree){
	float N,area;
	led_red(1);
	area=Radius*6.28318;
	uint16_t rotate_time,spd;
	uint8_t bat;
	// Speed (N)= V(bat)*12.68;
	bat=battery_level(); // speed is directly depend on battery level
	float max_speed;
	max_speed=bat*1.268;
	N=max_speed*(Radius-half_wheelbase)/(Radius+half_wheelbase);
	spd=N*512/max_speed;
	spd=512-spd;
	motor_right_speed(0,2);
	pwm_left(0,spd);
	_delay_ms(3000);
}
//*****************************************************************
//                 -----------( Communication )-------------
//-----------------------------------------------------------------
void send_message(uint8_t element){
	int8_t	i;
	uint8_t arr[8];
	for (i=0;i<8;i++){
		arr[i]=element%2;
        element=element/2;
	}
	PORTD |=0xFE;
	_delay_ms(pulse_width); // Start Bit
	for (i=7;i>=0;i--){
		if (arr[i])
			PORTD |=0xFE;
		else
			PORTD &=0x03;
		_delay_ms(pulse_width);
	}
	PORTD &=0x03;
	_delay_ms(pulse_width); // Stop Bit
}
//-----------------------


ISR(TIMER0_OVF_vect) {
	TCNT0=0x63;
// ---- insert code here
}

