/*****************************************************
Project : 2008 Nissan Tiida steering wheel buttons to Pioneer SPH-DA120
Version : 1,2
Date    : 31.1.2015
Author  : Elias Paloniemi  FINLAND
Comments: Goal of this program is to read the steering wheel buttons with ADC and then ouput control signal to W/R input of Pioneer SPH-DA120.

Note: Pins are sinking current trough a resistor and diode to generate control signal for Pioneer W/R input. Pioneer has internal pull up resistors on W/R input.
Ouput from pins are therfore reversed. output on = 0, output off = 1;
Because of the diode forward bias voltage the resistor values for each control function are one step lower than normal.
i.e. the first control function is 0 ohm rather than 1,2k ohm, the second control function is 1,2k ohm rather than 3,3k ohm and so on... 
However the last function must be at least 30k ohm rather than 22k ohm.

Here is a link to some good info on the Pioneer W/R control input:
http://jvde.net/node/7

Steering wheel buttons & functions table:
Button:     Function:         Function:
            Quick tap:        Hold down:

Source      Hang up phone     Source
Phone       Answer phone      Voice control
Seek Up     Seek Up           none(continously scrolls seek forward)
Seek Down   Seek Down         back/exit/radio band(will not continously scroll seek back)
Vol Up      Vol Up            none(continously scrolls volume up)
Vol Down    Vol Down          none(continously scrolls volume down)


Chip type               : ATmega8535
Program type            : Application
AVR Core Clock frequency: 14,318180 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 128
*****************************************************/

#include <mega8535.h>

#include <delay.h>

#define ADC_VREF_TYPE 0x20 // AREF pin, Internal Vref turned off. ADLAR set(result left adjusted).

// Declare your global variables here
unsigned int ADC0result = 255; // 5V on ADC input pin = no buttons pressed(390 ohm pull up resistors). Start condition = no buttons pressed.
unsigned int ADC1result = 255;
unsigned int backexitCHECK = 0; // This variable is used to "see" if the seek down button was held down and the back/exit function was activated, 
// and to make sure that after the back/eaxit functions has been activated that the seek down function is not activated. Also it is used to "see" 
// that if the back/exit function was not activated(i.e. quick push of button), then the seek down function is to be activated.

// ADC function. Read the 8 most significant bits of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
      ADMUX = adc_input | (ADC_VREF_TYPE & 0xff);
      // Delay needed for the stabilization of the ADC input voltage
      delay_us(10);
      // Start the AD conversion
      ADCSRA = ADCSRA | 0x40;
      // Wait for the AD conversion to complete
      while ((ADCSRA & 0x10)==0);
      ADCSRA = ADCSRA | 0x10;
      return ADCH;
}

void readADCfunction(void)
{
      // Read both ADC0 and ADC1 pins. Then copy the conversion results into global variables ADC0result and ADC1result.
      read_adc(0);
      delay_ms(10);
      ADC0result = ADCH;
      read_adc(1);
      delay_ms(10);
      ADC1result = ADCH;    
}

void backexit(void)
{
      // Seek down button hold down function.
      // Set backexitCHECK to 1.
      backexitCHECK = 1;
      // Back/Exit. Turn on output PORTC pin(33k ohm).
      PORTC.0 = 0;
      delay_ms(100);
      PORTC.0 = 1;
      // Wait a while to slow down loop if button is held down.
      delay_ms(400); 
}

void voice(void)
{
      // Voice control. Turn on output PORTC pin0(33K ohm) and output PORT D pin7(0 ohm).
      PORTC.0 = 0;
      PORTD.7 = 0;
      delay_ms(3000);
      // Turn off outputs.
      PORTC.0 = 1;
      PORTD.7 = 1;
      // Wait a while to slow down loop if button is held down. <- Not required for the voice control because 
      // of the 3 sec. needed to activate.(Button doen't need to held for more than 500ms)
      //delay_ms(400);      
}

void sourceFunction(void)
{
      // Source. Turn on output PORTC pin7(0 ohm).
      PORTC.7 = 0;
      delay_ms(100);
      // Turn off outputs.
      PORTC.7 = 1;
      // Wait a while to slow down loop if button is held down.
      delay_ms(400);
}

void phone(void)
{
      // Phone button.
      // This button has dual functions, quickly pressing the button and holding it down have different functions.
      // Holding down the button activates the quick function before activating the hold down function, but this has no foreseeable conflict.
      if(ADC0result < 5)
      {
            // Answer the phone. Turn on output PORT C pin6(1k ohm) and output PORT D pin7(0 ohm).
            PORTC.6 = 0;
            PORTD.7 = 0;
            delay_ms(100);
            // Turn off outputs.
            PORTC.6 = 1;
            PORTD.7 = 1;
            // Wait a while then see if button is held down.
            delay_ms(400);
            // Check if the button is being held down.
            readADCfunction();
            if(ADC0result < 5)
            {
                  // Go to voice control function.
                  voice();
            }
      }
}

void seekup(void)
{
      // Seek Up button.
      if(ADC0result < 90 && ADC0result > 60)
      {
            // Seek Up. Turn on output PORT C pin4(5,6k ohm).
            PORTC.4 = 0;
            delay_ms(100);
            // Turn off outputs.
            PORTC.4 = 1;
            // Wait a while to slow down loop if button is held down.
            delay_ms(700);
      }
}

void volumeup(void)
{
      // Volume Up button.
      if(ADC0result < 170 && ADC0result > 150)
      {
            // Volume Up. Turn on output PORT C pin2(12k ohm).
            PORTC.2 = 0;
            delay_ms(100);
            // Turn off outputs.
            PORTC.2 = 1;
            // Wait a while to slow down loop if button is held down. <- Not used for volume control.
            // Holding down the volume button didn't change the volume fast enough in practice so I commented out the following delay.
            // delay_ms(200);
      }
}

void source(void)
{
      // Source button.
      // This button has dual functions, quickly pressing the button and holding it down have different functions.
      // Holding down the button activates the quick function before activating the hold down function, but this has no foreseeable conflict.
      if(ADC1result < 5)
      {
            // Hang up the phone. Turn on output PORT C pin7(0 ohm) and output PORT D pin7(0 ohm).
            PORTC.5 = 0;
            PORTD.7 = 0;
            delay_ms(100);
            // Turn off outputs.
            PORTC.5 = 1;
            PORTD.7 = 1;
            // Wait a while then see if button is held down.
            delay_ms(400);
            // Check if the button is being held down.
            readADCfunction();
            if(ADC1result < 5)
            {
                  // Go to source function.
                  sourceFunction();
            }
      }
}

void seekdown(void)
{
      // Seek Down button.
      // This button has dual functions, quickly pressing the button and holding it down have different functions.
      // Holding down the button DOES NOT activate the quick function before activating the hold down function, this avoids conflicts in some situations.
      // This means that holding down the button will not continously seek through items. Going through items must be done by quickly tapping the button repeatedly.
      if(ADC1result < 90 && ADC1result > 60)
      {
            // Wait to see if the button has been held down, if YES then goto backexit(), if NO send seek down conrtol signal to Pioneer.
            delay_ms(400); 
            // Check if the button is being held down.
            readADCfunction();
            if(ADC1result < 90 && ADC1result > 60)
            {
                  // Go to back/exit function.
                  backexit();
            }
            if(backexitCHECK == 0)
            {
                  // Seek Down. Turn on output, if the button is not being held down, PORT C pin4(8,2k ohm).
                  PORTC.3 = 0;
                  delay_ms(100);
                  // Turn off outputs.
                  PORTC.3 = 1;
                  // Wait a while to slow down loop if button is held down.
                  delay_ms(400);
            }
            backexitCHECK = 0; // Reset backexitCHECK.
      }
}

void volumedown(void)
{
      // Volume Down button.
      if(ADC1result < 170 && ADC1result > 150)
      {
            // Volume Down. Turn on output PORT C pin1(15k ohm).
            PORTC.1 = 0;
            delay_ms(100);
            // Turn off outputs.
            PORTC.1 = 1;
            // Wait a while to slow down loop if button is held down. <- Not used for volume control.
            // Holding down the volume button didn't change the volume fast enough in practice so I commented out the following delay.
            // delay_ms(200);
      }
}

void main(void)
{
      // Declare your local variables here

      // Input/Output Ports initialization 
      PORTA = 0x00;
      DDRA = 0x00;

      PORTB = 0x00;
      DDRB = 0x00;

      PORTC = 0xFF; // Set outputs high = off.
      DDRC = 0xFF;

      PORTD = 0x80; // Set output high = off.
      DDRD = 0x80;

      // Timer/Counter 0 initialization
      TCCR0 = 0x00;
      TCNT0 = 0x00;
      OCR0 = 0x00;

      // Timer/Counter 1 initialization
      TCCR1A = 0x00;
      TCCR1B = 0x00;
      TCNT1H = 0x00;
      TCNT1L = 0x00;
      ICR1H = 0x00;
      ICR1L = 0x00;
      OCR1AH = 0x00;
      OCR1AL = 0x00;
      OCR1BH = 0x00;
      OCR1BL = 0x00;

      // Timer/Counter 2 initialization
      ASSR = 0x00;
      TCCR2 = 0x00;
      TCNT2 = 0x00;
      OCR2 = 0x00;

      // External Interrupt(s) initialization
      MCUCR = 0x00;
      MCUCSR = 0x00;

      // Timer(s)/Counter(s) Interrupt(s) initialization
      TIMSK = 0x00;

      // USART initialization
      UCSRB = 0x00;

      // Analog Comparator initialization
      ACSR = 0x80;
      SFIOR = 0x00;
      
      // ADC initialization
      // ADC Clock frequency: 223,722 kHz
      // ADC Voltage Reference: AREF pin
      // ADC High Speed Mode: Off
      // ADC Auto Trigger Source: ADC Stopped
      // Only the 8 most significant bits of the AD conversion result are used, result left shifted.
      ADMUX = ADC_VREF_TYPE & 0xff;
      ADCSRA = 0x86;
      // SFIOR = SFIOR & 0xEF;  //Redundant? Was written by CodeVisionAVR code generator.

      // SPI initialization
      SPCR = 0x00;

      // TWI initialization
      TWCR = 0x00;

      while (1)
      {
            // Go to readADCfunction(). Read ADC inputs 0 and 1, steering wheel buttons signals(390 ohm pull up resistors on ADC pins 0 and 1).
            readADCfunction();
            phone();
            seekup();
            volumeup();
            source();
            seekdown();
            volumedown();     
      }
}