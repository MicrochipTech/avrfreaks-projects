//------[ PPM_Lib.c ]---------------------------------------------------------------
//
//Written for  	: WinAVR 20070525 (C compiler for Atmel AVR)
//        when 	: 2007 MAY 28
//      where	: BRUSSELS - BELGIUM - EUROPE
//      who  	: Jan Huygh
//      E-mail	: jan.huygh@skynet.be
//		
//------[ ABOUT THIS VERSION ]------------------------------------------------------
//
//	KNOWN BUGS :
//		* None so far
//	OPPORTUNITIES FOR IMPROVEMENT
//		* Implement a variable number of channels that are transmitted.
//		In this version it is fixed to 8.
//
//------[ INTRODUCTION ]------------------------------------------------------------
//
//If you want to build your own RC transmitter then using a commercial HF module
//such as the Graupner/JR Sender-HF-Modul Part Nr 4057 or the Graupner/JR
//PLL-SYNTHESIZER-Sender HF-Modul Part Nr 4059 is a good idea.
//Using such a module you will not have to worry about a transmitter license nor
//will you have to worry about building the HF-portion of the transmitter.
//
//I can buy such a module for about 75 Euro. If somebody can tell me where I can buy
//a comparable module for less please send me a mail jan.huygh@skynet.be
//
//This program does generate the postitive Pulse Position Modulation (PPM) signal
//that you will need if you want to build your own RC transmitter using such a
//commercial High Frequency module. These Modules take a PPM signal as input.
//
//If you have a Graupner X-756 or comparable transmitter then the trainer signal is
//this PPM-signal. So you could use the trainer connection on that transmitter and
//avoid buying the above mentioned module.
//
//------[ DESCRIPTION OF THE POSITIVE PPM SIGNAL ]----------------------------------
//
//The PPM signal is transmitted every 20 ms. Between 2 PPM signals the line is high.
//The start of the PPM signal is a line low pulse of 0,4 ms.
//After this start-pulse each channel is transmitted by setting the line high for
//x ms followed by a low pulse of y ms with y at least 0,4 ms.
//On the receiver this will generate a pulse with a lenght of x + y ms.
//The receiver expects each channel pulse to be between 1 and 2 ms.
//
//To transmit 8 channels you need 8 X 2 + 0,4 ms = 16,4 ms
//
//--------[ CONNECTING TO A HF MODULE ]---------------------------------------------
//
//  |                    If you lay the HF module in front of you with the
//  |                    connector (5 small holes) in the lower left corner
//  |  ° PPM             then the connections are as indicated here on the left.
//  |  ° +5V
//  |  ° +8V                PPM is the Positive Pulse Position Signal
//  |  ° GND                + 5V is a stabelized 5V tention
//  |  ° To Antenna         + 8V to this you connect your main battery
//  |______________            8 * 1.5 = 12V is OK too.
//                          GND is the ground connection (the - from the battery)
//
//------[ EXPLAINING HOW THIS CODE WORKS ]------------------------------------------
//
//The transmitter has to transmit the PPM signal every 20 ms. The PPM signal can
//take up to 16.4 ms to transmit. This means that the microcontroller in the
//transmitter could spend up to 16.4/20*100% = 82% of his time on generating the
//PPM signal. Obviously that is NOT acceptable. The PPM signal will have to be
//generated using a timer and his interrupt.
//
//The most logical approach would be to use 2 timers : one to time the start of each
//PPM-pulse sequence and a 2nd timer to generate the pulses inside the PPM signal.
//Because we want to leave a maximum of resources available to the microcontroller
//this code will only use one 8 bit timer.
//To make sure this code can be used, even if the microcontroller has already an
//8 bit counter in use, you can set the timer counter that will be used in the
//beginning the code.
//
//Because we use just one timer we will have to set the timer at the end of the
//PPM-signal to that value that generates an interrupt 20ms after the start of the
//PPM signal.
//If all the channel pulses are just 1 ms long, the longest period that we will have
//to set here is 20 - 8X1 - 0,4 = 11,6 ms.
//To time 11.6 ms with the highest precision the best time base would be 11,6/255 =
//0,04549 ms. We can change the time base of the timer using the pre scaler (PS).
//The ideal PS with our microcontroller running at 16 Mhz would be PS = 0,04549 X 16
//X 10^3 = 727,8. The only PS-values that are available are 8, 64, 256 or 1024. So
//we have to use 1024. Using PS = 1024 will set the time base for our counter to
//1024/16 = 64 µs.
//
//The longest pulse we need to generate inside a PPM signal is the channel pulse of
//maximum 2 ms. If we start the high pulse with a fixed 0,5 ms pulse and we end the
//pulse with a 0,5 ms low pulse, the longest pulse we need to transmit is 1 ms.
//Analog to what has been calculated here above : 
//Ideal time base = 1/255 = 0.003921 ms. Ideal PS = 0.003921 X 16 X 10^3 = 62,7.
//That means we will use PS= 64 and so the time base will be 64/16 = 4 µs.
//To generate a 1 ms delay we will have to set the timer to 1000/4 = 250.

//To use this program you need to load PPM_Channel_n_PulseLength [n] with a value
//between 0 and 255. Loading 0 => 1/0ms, Loading 125 => 1.5ms Loading 250 => 2.0ms.
//
//After the initial load of PPM_Channel_n_PulseLength [n] you need to start the PPM-
//pulse-train with the function PPM_Start().
//Your program can now do whatever it needs to do including changing the values for
//PPM_Channel_n_PulseLength [n].
//
//You can stop the PPM signal with the function PPM_Stop().

#include <avr/io.h>
#include <avr/interrupt.h>
#include <sbi_cbi.c>

#define PPM_PORT 	PORTC 	//The port that wil be used to generate the PPM signal
#define PPM_DDR		DDRC	//The data direction register for the PPM_PORT
#define PPM_BIT  	0		//The bit that will be used to generate the PPM signal
#define PPM_TCCR	TCCR0	//The timer counter control register.
							//The alternative is TCCR2
#define PPM_TCNT	TCNT0	//The timer counter. The alternative is TCNT2
#define PPM_INTERRUPT TIMER0_OVF_vect//The alternative is TIMER2_OVF_vect
#define PPM_INT_BIT 0		//Enable timer overflow interrupt bit.
							//The alternative is 6

uint8_t PPM_Progress; //Used to tell the interrupt routine where we are in the pulse
uint8_t PPM_Channel_n_PulseLength [8]; //These needs to be set by the main program
//PPM_Channel_n_PulseLength[n] = 0   will result in a 1.0ms pulse from the receiver
//PPM_Channel_n_PulseLength[n] = 125 will result in a 1.5ms pulse from the receiver
//PPM_Channel_n_PulseLength[n] = 250 will result in a 2.0ms pulse from the receiver

void PPM_Start (void){
	sbi (PPM_DDR,PPM_BIT); 	//Set PPM_BIT in PPM_DDR to output
	PPM_Progress = 0;
	PPM_TCCR = 0x05;		//Normal mode. Prescaler 1024 = 64µs (with CPU@16MHz)
	PPM_TCNT = 256-132;		//20 ms to next timer counter overflow interrupt
	sbi (TIMSK,PPM_INT_BIT);//Enable PPM_TCNT interrupt
	sbi(SREG,7); 			//Global Interrupt Enable
}
void PPM_Stop (void){
	PPM_TCCR = 0x00;		//Disconnects the clock source so the PPM counter stops
}
ISR(PPM_INTERRUPT){
	switch (PPM_Progress){
	case 0:
		//Included just to have a decent trigger. Remove before production use.
		//sbi (PPM_PORT, 7);
		//cbi (PPM_PORT, 7);

		PPM_TCCR = 0x03;//Set time base to 4 µs. (If CPU@16Mhz) = Pre Scaler = 64.
		cbi (PPM_PORT,PPM_BIT);//Clear PPM_BIT in PPM_PORT
		PPM_TCNT = 255 - 98; //Set next interrupt to occure after 100 X 4 µs = 0,4ms
		PPM_Progress ++;
		break;
	case 1: case 4: case 7: case 10: case 13: case 16: case 19: case 22:
		sbi  (PPM_PORT,PPM_BIT);//Set PPM_BIT in PPM_PORT
		PPM_TCNT = 255 - 125;//set next interrupt in 1ms
		PPM_Progress ++;
		break;
	case 2: case 5: case 8: case 11: case 14: case 17: case 20: case 23:
		PPM_TCNT = 255 - (PPM_Channel_n_PulseLength [(PPM_Progress/3)]);
		PPM_Progress ++;
		break;
	case 3: case 6: case 9: case 12: case 15: case 18: case 21: case 24:
		cbi (PPM_PORT,PPM_BIT);//Clear PPM_BIT in PPM_PORT
		PPM_TCNT = 255 - 125;//Set next interrupt to occure after 100 X 4 µs = 0,4ms
		PPM_Progress ++;
		break;
	case 25:
		sbi (PPM_PORT,PPM_BIT);//Set PPM_BIT in PPM_PORT
		PPM_TCCR = 0x05;		//Normal mode. Prescaler 1024 = 64µs with CPU@16MHz
		int PPM_Time = 0;
		for(int i=0; i<8 ;i++){PPM_Time += PPM_Channel_n_PulseLength[i];}
		PPM_TCNT = 255 - ((20000-(1000 * 8)-400-(PPM_Time * 4))/64);
		PPM_Progress = 0;
	}
}
