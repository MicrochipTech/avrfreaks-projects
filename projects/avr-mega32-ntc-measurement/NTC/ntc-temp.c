
/****************************************************************************
* Title  	:  NTC-Temperature measurement
* Author	:  Bingo   avr1@luna.kyed.com	
* Date		:  15-Jul-2005
* Software	:  AVR-GCC with AVR-AS
* Target	:  ATmega xx (This one Mega128)
* Copyright	:  None
* Comments	:
* 		   No guarantees, warrantees, or promises, implied or otherwise.
* 		   May be used for hobby or commercial purposes provided copyright
* 		   notice remains intact.
*
*			Hardware : STK-500 + Mega32 , AVCC = VCC = 5V
*
*			System has a blinking LED connected to PD5
*
*			An EPCOS B57703M0103G040 10K NTC on PA0 (ADC0)
*			A 10K 1% Resistor from AVCC to AD0 , and NTC from ADC0 to GND
*			
*			An EPCOS B57164K0222+000 2.2K NTC on PA1 (ADC1)
*			A 2.2K 5% Resistor (Had no 1%)  from AVCC to AD1 , and NTC from ADC1 to GND
*
*****************************************************************************/

/**************************************************************************************
*				Include section
***************************************************************************************/
#include	<stdio.h>
#include	<inttypes.h>
#include	<stdlib.h>
#include	<string.h>
#include	<avr/io.h>
#include	<avr/eeprom.h>
#include	<avr/pgmspace.h>
#include 	<avr/interrupt.h>
#include 	<avr/signal.h>
#include 	<avr/delay.h>
#include	<uart.h>
#include	<ntc-temp.h>

#define		MIN_TEMPERATURE -55		
#define		MAX_TEMPERATURE 125

extern		uint16_t EPCOS_B57703M0103G040_table[] __attribute__((progmem));
extern		uint16_t EPCOS_B57164K0222_000_table[] __attribute__((progmem));

void 		init_hardware(void);
void 		initadc(void);
uint16_t 	read_adc(uint8_t adc_input);
int16_t		adc_to_temperature(uint16_t *temperature_table , uint16_t ntcdata);
uint16_t 	ntc0_average(uint16_t adcdata);
uint16_t 	ntc1_average(uint16_t adcdata);
void 		delay_ms(uint16_t ms);
void 		disp(uint8_t adc,uint16_t ntcdata);

#define	ADC_ENABLE 			(1<<ADEN)
#define	ADC_CONVERT			(1<<ADSC)
#define ADC_VREF_TYPE 		((0<<REFS1) | (1<<REFS0))
#define	ADC_PRESCALE_128	((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)) //150 us Conversion time @11.xxx Mhz


void initadc(void){

	ADCSRA = 0;
	ADMUX = ADC_VREF_TYPE; 	//AVCC voltage reference with external capacitor at AREF pin
	ADCSRA = ADC_ENABLE | ADC_PRESCALE_128 ; 		// ADC Enable and Set Prescale value 
	SFIOR &= 0x0F; 			// Make sure reserved bit 0x04 is set to zero

}



uint16_t read_adc(uint8_t adc_input)
{
	ADMUX = adc_input|ADC_VREF_TYPE; 	// Internal 2.56 VREF , use the specified input pin
	ADCSRA |= ADC_CONVERT; 				// ADC Start conversion (this bit is restored to zero when finished)
	while (ADCSRA & ADC_CONVERT);     // Check the complete flag and wait until done
	return ADCW;

}



int16_t main(void)
{
	uint8_t		i,j,led = 0;
	uint16_t	ntc0data;


	
	init_hardware();
	
	while(1)
	{
		bit_flip(PORTD,BIT(PD5));
		for(i=0; i < 2 ; i++)
		{
			ntc0data = read_adc(i);
			disp(i,ntc0data);
		}
		delay_ms(2000);
	}
	return(0);
}

void disp(uint8_t adc,uint16_t ntcdata)
{
	int16_t		ntctemp;
	uint16_t	*table;
	uint8_t		buf[100] , i,*ptr;
	
	ptr = buf;
	
	for(i=0;i<30;i++)
	{
		*ptr++ = '*';
	}
	*ptr++ = '\r';
	*ptr++ = '\n';
	*ptr++ = '\0';
	uart_puts(buf);
	
	sprintf(buf,"ADC(%d) = %d\n\r",adc,ntcdata);
	uart_puts(buf);

	if(adc == 0)
	{
		table = (uint16_t *)EPCOS_B57703M0103G040_table;
		ntcdata = ntc0_average(ntcdata);
	}
	else
	{
		table = (uint16_t *)EPCOS_B57164K0222_000_table;
		ntcdata = ntc1_average(ntcdata);
	}
	


	sprintf(buf,"ADC(%d) AVG = %d\n\r",adc,ntcdata);
	uart_puts(buf);
	ntctemp = 	adc_to_temperature(table,ntcdata);
	sprintf(buf,"Temp(%d) = %d\n\r",adc,ntctemp);
	uart_puts(buf);
}

void init_hardware(void)
{

	initadc();
	DDRD |= BIT(PD5);	//Set PD5 as output for blinking led

	/* UART WILL BE INITIALIZED ONLY ! */ 
	uart_init( UART_BAUD_SELECT(9600,XTAL)); 

	sei();
}

void delay_ms(uint16_t ms)
{
	while(ms--)
	{
		_delay_ms(1);
	}
}



int16_t	adc_to_temperature(uint16_t *temperature_table , uint16_t ntcdata)
{
	int16_t		roomtemp = 0;

	while ((ntcdata < pgm_read_word(temperature_table++) && (roomtemp < (MAX_TEMPERATURE - MIN_TEMPERATURE))))
	{
		roomtemp++;
	}

	return((roomtemp + MIN_TEMPERATURE));
}

uint16_t ntc0_average(uint16_t adcdata)
{ // performs five-point moving average

	static uint8_t		first = 1;
	static uint16_t	t1,t2,t3,t4,t5;
	
	if(first == 1)
	{
		first = 0;
		t1 = t2 = t3 = t4 = t5 = adcdata;
	}
	
    t5 = t4; 
    t4 = t3; 
    t3 = t2; 
    t2 = t1;
    t1 = adcdata; 
    adcdata = (t1+t2+t3+t4+t5)/5;
	
	return(adcdata);
} 

uint16_t ntc1_average(uint16_t adcdata)
{ // performs five-point moving average

	static uint8_t		first = 1;
	static uint16_t	t1,t2,t3,t4,t5;
	
	if(first == 1)
	{
		first = 0;
		t1 = t2 = t3 = t4 = t5 = adcdata;
	}
	
    t5 = t4; 
    t4 = t3; 
    t3 = t2; 
    t2 = t1;
    t1 = adcdata; 
    adcdata = (t1+t2+t3+t4+t5)/5;
	
	return(adcdata);
} 

/***************************************************************************************************************
*
*	ADC Value table for the Epcos B57703M0103G040 NTC (10K @ 25 degrees), extracted from EPCOS Tools program
*	and converted by a hint from R.Hilton
*
*  	SQL Code used for the EPCOS Access Database (pwd = omega)
*
*	SELECT Result.Temperatur, Result.Widerstand
*	FROM Result;
*
*	SELECT Result.Temperatur, Result.Widerstand, Int((([Widerstand]*10000)*1024)/(([Widerstand]*10000)+10000)+0.5) AS adcval INTO adcvals
*	FROM Result
*	ORDER BY Result.Temperatur;
*
*	The table contains ADC values from -55 degrees to +125 degrees , and a NULL value at the end.
*
****************************************************************************************************************/


uint16_t EPCOS_B57703M0103G040_table[(MAX_TEMPERATURE - MIN_TEMPERATURE) + 2] __attribute__((progmem)) = {
1013,
1013,
1012,
1011,
1010,
1009,
1008,
1007,
1005,
1004,
1003,
1001,
1000,
998,
996,
994,
992,
990,
988,
986,
983,
981,
978,
975,
972,
969,
966,
962,
959,
955,
951,
947,
943,
938,
933,
928,
923,
918,
912,
907,
901,
894,
888,
881,
874,
867,
860,
852,
845,
837,
828,
820,
811,
802,
793,
784,
774,
765,
755,
745,
735,
724,
714,
703,
692,
682,
671,
659,
648,
637,
626,
614,
603,
592,
580,
569,
557,
546,
535,
523,
512,
501,
490,
479,
468,
457,
446,
436,
425,
415,
405,
395,
385,
375,
365,
356,
347,
338,
329,
320,
311,
303,
295,
287,
279,
271,
264,
256,
249,
242,
235,
229,
222,
216,
210,
204,
198,
193,
187,
182,
177,
171,
167,
162,
157,
153,
148,
144,
140,
136,
132,
128,
125,
121,
118,
114,
111,
108,
105,
102,
99,
96,
94,
91,
89,
86,
84,
81,
79,
77,
75,
73,
71,
69,
67,
65,
63,
62,
60,
58,
57,
55,
54,
53,
51,
50,
48,
47,
46,
45,
44,
43,
41,
40,
39,
38,
37,
36,
36,
35,
34,
0
};

/***************************************************************************************************************
*
*	ADC Value table for the Epcos B57164K0222+000 NTC (2.2K @ 25 degrees) , extracted from EPCOS Tools program
*	and converted by a hint from R.Hilton
*
*  	SQL Code used for the EPCOS Access Database (pwd = omega)
*
*	SELECT Result.Temperatur, Result.Widerstand
*	FROM Result;
*
*	SELECT Result.Temperatur, Result.Widerstand, Int((([Widerstand]*2200)*1024)/(([Widerstand]*2200)+2200)+0.5) AS adcval INTO adcvals
*	FROM Result
*	ORDER BY Result.Temperatur;
*
*	The table contains ADC values from -55 degrees to +125 degrees , and a NULL value at the end.
*
****************************************************************************************************************/
uint16_t EPCOS_B57164K0222_000_table[(MAX_TEMPERATURE - MIN_TEMPERATURE) + 2] __attribute__((progmem)) = {
1011,
1010,
1009,
1008,
1007,
1006,
1004,
1003,
1002,
1000,
999,
997,
995,
994,
992,
990,
987,
985,
983,
980,
978,
975,
972,
969,
966,
963,
959,
955,
952,
948,
943,
939,
935,
930,
925,
920,
915,
909,
904,
898,
892,
885,
879,
872,
865,
858,
851,
843,
836,
828,
820,
811,
803,
794,
785,
776,
767,
757,
748,
738,
728,
718,
707,
697,
687,
676,
665,
655,
644,
633,
622,
611,
600,
589,
578,
567,
556,
545,
534,
523,
512,
501,
490,
480,
469,
459,
448,
438,
428,
418,
408,
398,
388,
379,
369,
360,
351,
342,
333,
325,
316,
308,
300,
292,
284,
277,
270,
262,
255,
248,
242,
235,
229,
222,
216,
210,
204,
199,
193,
188,
183,
178,
173,
168,
163,
159,
154,
150,
146,
142,
138,
134,
130,
127,
123,
120,
116,
113,
110,
107,
104,
101,
98,
96,
93,
91,
88,
86,
83,
81,
79,
77,
75,
73,
71,
69,
67,
65,
64,
62,
60,
59,
57,
56,
54,
53,
51,
50,
49,
48,
46,
45,
44,
43,
42,
41,
40,
39,
38,
37,
36,
00
};
