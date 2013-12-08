# Reading The Data Light Sensor

Uploaded by danasorani on 2013-12-08 14:31:00 (rating 0 out of 5)

## Summary

We Have Light Sensor To detect the illumination and i write program that read the value of the light and send to the Computer by Usart 


#include   

#include   

#include   

#define F\_CPU 3686400L  

void ADC\_init();  

void uartinit(unsigned long baudrate);  

void putChar(unsigned char data);


ISR(ADC\_vect) // this is if the ADC intrrupted to the micro controller the program run that function  

{  

 unsigned char dumyL = 0, dumyH = 0; // we use to varibale to hold the data low and hight digital output  

 dumyL = ADCL; // the digital out put in the ADCL  

 dumyH = ADCH; // the digital output in the ADCH  

 putChar(dumyH); // and we send the data to the computer  

 \_delay\_ms(500); // we call the delay  

 putChar(dumyL); // and we send the data to the computer  

 \_delay\_ms(500); // we call the delay  

 ADCSRA |= 0b01000000; // adc start convertion again  

}


int main()  

{  

 DDRC = 0x00; // make port C input  

 PORTC = 0xff; // acctive pull up resistor  

 uartinit(115200); // call the function that intilaze the Serial Comunication  

 ADC\_init(); // call the ADC Function that intilaze the ADC  

 sei(); // turn on intrrupt generally  

 ADCSRA |= 0b01001101; // the adc start convertion  

 while(1)  

 {  

 //putChar('f');  

 }  

 return 0;


}  

void uartinit(unsigned long baudrate)  

{  

 unsigned long ubrr = (F\_CPU/(baudrate*16))-1; // calculate to determine baudrate  

 UBRRL = ubrr;  

 UBRRH = ubrr >> 8;  

 UCSRB |=0b00001000; // acctive sender 


}


void putChar(unsigned char data) // define the function Put char to send data to the buffer  

{  

 while( !(UCSRA &(0b00100000)));  

 UDR = data;  

}


void ADC\_init() // this function to inilaize ADC  

{  

 ADMUX = 0b01000101;  

 // voltage refrence = Vcc  

 // Select Channel 5 PORTC Pin 5  

 // adler = 0  

 ADCSRA = 0b10001101;


 // ADC turn on  

 // acctive ADC intrrupt  

 // chosse prescelar 32 


}

## Compilers

- AtmanAvr C/C++ IDE

## Tags

- Complete code
- AtmanAvr C/C++ IDE
