//Tankless Waterheater Controler
// by Joseph Bumstead
// no rights reserved



// For AVR Butterfly
// which uses atmega169



#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "LCD_Driver.c"
//#include <avr/sleep.h>

// Utility Macros
#define MIN(a,b) ((a)<(b)) ? a:b
#define MAX(a,b) ((a)>(b)) ? a:b
#define LIMIT(lower,m,upper) MAX(MIN((upper),(m)),(lower))

// Application defines ////////////////////////////////////
#define SERVO_POS OCR1A
#define SERVO_ON        (DDRB |= (1<<5))
#define SERVO_OFF       (DDRB &= ~(1<<5))
#define SERVO_IS_ON     (DDRB & (1<<5))
#define SERVO_IS_OFF    ((DDRB & (1<<5)) ==0)

#define CONTROL_ON   (TIMSK1 |=(1<<OCIE1A))   // allow interrupt
#define CONTROL_OFF  (TIMSK1 &=~(1<<OCIE1A))  // stop interrupt

#define PARK_SPOT 1800 // moderate gas
#define RUNNING SERVO_IS_ON
#define STOP SERVO_OFF; inv_flow = 9999
#define PARK SERVO_POS = PARK_SPOT

#define PARK_COUNT 33
#define UNPARK_COUNT 33
#define FLOW_WATCH_SET 7


#define KEY_UP 6
#define KEY_DN 7
#define KEY_LF 2
#define KEY_RT 3
#define KEY_CNTR 4
#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))


#define SERVO_REVERSE // Comment-out if increased servo-pulse
                      // causes increased gas.


#define DEFAULT_INLET_TEMP 50
#define DEFAULT_TEMP 115


#define FLOW_NUMERATOR 9800 // For reading in tenths of GPM
#define FLOW_TOO_LOW 3333

#ifdef SERVO_REVERSE
#define SERVO_ZERO 2777 // higher shifts temperature range down
#else
#define SERVO_ZERO 66 // higher shifts temperature range up
#endif

#define K_FEED_FOWRD 15000 // higher expands range

       // P I D,  see http://en.wikipedia.org/wiki/PID_controller, etc.
#define WIND_UP 77.0
#define KP 1.5
#define KI 0.1
#define KD 0.0

#define SERVO_MIN 840
#define SERVO_MAX 2280


#define FAHR_SHFT 4
#define DEBOUNCE_CTR  5

static volatile uint8_t timer2HiCntr = 0;

static volatile uint16_t time=0;
static volatile uint16_t prevTime=0;

static volatile int8_t flow_watch_dog = FLOW_WATCH_SET;
static volatile int8_t parkCTR = 0;
static volatile int8_t unParkCTR = UNPARK_COUNT;
static volatile uint16_t inv_flow = FLOW_TOO_LOW;
static volatile uint16_t flow = 0;
static volatile uint16_t outTemp = 111;
static volatile uint16_t inTemp = DEFAULT_INLET_TEMP;
static volatile uint16_t setting=DEFAULT_TEMP;



       // P I D
static volatile int16_t prev_err = 0;
static volatile float Kp=KP,Ki=KI,Kd=KD;
static volatile int16_t err, deriv;
static volatile int16_t integ = 0;
static volatile int16_t pre_servo;






// NTC thermistor adc values table
 // -40 to 212 F,
const int16_t TEMP_LU[] PROGMEM = {
1001,992,979,963,942,916,884,845,800,749,693,634,573,512,453,396,344,
297,255,218,186,159,135,115,98,83,71,61,52
};
#define FhrnheitTableIncr 9

// For calibrating temperature sensors:
#define ADC_CH0_OFFSET 50
#define ADC_CH2_OFFSET 110
#define FAHR_TUNE_NUM 114
#define FAHR_TUNE_DEN 100
int16_t getFahrenheit(uint16_t sense) {
	int16_t t1,a1, a2 = 11111; // some num greater than table entries
	uint8_t i=1; // Start at second index
	while ((a2 > sense)&& (i<30)) {
	       a2 = pgm_read_word(&TEMP_LU[i++]);
	}
	i -=2;
	a1 = pgm_read_word(&TEMP_LU[i]);
	t1 = i*FhrnheitTableIncr-40;//interpolate
	return (t1+FhrnheitTableIncr*(a1-sense)/(a1-a2))*FAHR_TUNE_NUM/FAHR_TUNE_DEN;
//interpolate
}





//adc_init
#define N_ADC_CHANS 3
static volatile uint8_t adc_chan_ndx = 0;
static volatile uint16_t ADC_Results[N_ADC_CHANS];// ={800,0,800};

void ADC_init() {
	PORTF |= (1<< PF3);// turn on sensors
	DDRF |= (1<< DDF3);
	ADMUX = adc_chan_ndx;// start on chan x
	ADCSRA = (1<<ADPS1)|(1<<ADPS0);//pre scale
	ADCSRA |= (1<< ADEN)|(1<<ADIE);//turn on adc, enable interrupt
	ADCSRA |= (1<<ADSC);// start first read
}



void Setup_flow_sense(){
	// setup flow sensor pulse interrupt on pin B3;
	PCMSK1 = 1<<PINB3; //
	EIMSK= 0x80;
	SREG |= 0x80;
	// the ISR will read the period of these pulse interrupts
	// from timer2 and its overflow byte: timer2HiCntr.

	//Let timer 2 free-run for measuring intervals.
	// Overflow interrupt will incrfement timer2HiCntr.
	TCCR2A = 0x07;// Normal mode, prescale: /1024
	TIMSK2 = 0x01;// ovrflw int16_t enable
}

void Setup_servo_driver(){
	//set Timer1 in fast pwm mode (14), TOP=ICR1
	//clear output on compare match, set at top (COM1A1=1)
	TCCR1A = (1<<COM1A1) | (1<<WGM11);
	TCCR1B = (1<<WGM12)  | (1<<WGM13) | (1<<CS11); //prescaler 8?
	ICR1 = 20000;    //period = 20 ms   ICR1 = (time[us] * F_CPU[MHz] /prescaler) - 1

	CONTROL_OFF;
	SERVO_OFF;
}

void init(){
	LCD_Init();
	ADC_init();

	Setup_flow_sense();
	Setup_servo_driver();

	DDRB &= ~PINB_MASK;  // joystick pins
	PORTB |= PINB_MASK;  // joystick pins
	DDRE &= ~PINE_MASK;  // joystick pins
	PORTE |= PINE_MASK;  // joystick pins
}





#define NUM_MENU_ITEMS 6
static volatile uint8_t menuItm = 0;

struct {
       uint16_t *data;         // points at menu item data.
       char legend[4];         // menu text.
       uint8_t view_scale;     // to scale menu item data.
       uint8_t adj_scale;      // menu item data adjustment increment.
}  menu[NUM_MENU_ITEMS]=
{
{&setting,   "T ",  1, 1},
{&flow,      "F",   1, 0},
{&inTemp,    "IN ", 1, 0},
{&outTemp,   "OUT", 1, 0},
{&err,      "E",   1, 0},// feedback
{&SERVO_POS, "V",   1, 0},// servo position

};



void show() {
	char ct[7];
	char ctt[11]="";
	itoa(((int16_t)(*(menu[menuItm].data)))/(menu[menuItm].view_scale), ct, 10);
	strcpy(ctt,menu[menuItm].legend);
	strcat(ctt, ct);
	LCD_puts(ctt);
}

void joystick() {
	int8_t menuItmDir = 0;
	uint8_t button = ((~PINB) & PINB_MASK) | ((~PINE) & PINE_MASK);
	if (button & (1<<KEY_UP)) {     menuItmDir = 1;}// bump
	else if (button & (1<<KEY_DN)) { menuItmDir = -1;}// drop
	else {// show next menu item
		if (button & (1<<KEY_RT)) { menuItm++;}//
		else if (button & (1<<KEY_LF)) {
			if (menuItm<=0) menuItm=NUM_MENU_ITEMS;
			menuItm--;
		};
		menuItm %= NUM_MENU_ITEMS;
	}
	if (menu[menuItm].adj_scale) {
		*(menu[menuItm].data) += menuItmDir*menu[menuItm].adj_scale;
	}
}




uint8_t buttonPressed =0;
uint8_t showSig = 0;
int main(void) {
	sei();
	init();
	for(;;) {
		if (buttonPressed >= DEBOUNCE_CTR) {
			joystick(); // check buttons
			buttonPressed = 0; // acknowledge
		}
		if (showSig) {
			// ADC not working for inTemp so stay with DEFAULT_INLET_TEMP.
			//inTemp = getFahrenheit((ADC_Results[0]>>FAHR_SHFT)+ADC_CH0_OFFSET);
			outTemp = getFahrenheit((ADC_Results[2]>>FAHR_SHFT)+ADC_CH2_OFFSET);
			flow = FLOW_NUMERATOR/inv_flow;
			show();
			showSig = 0; // acknowledge
		}
	}
	//sleep_mode();
}





ISR(SIG_OVERFLOW2) { // Timer2 Ovrflow
	if (timer2HiCntr++ %4 == 0)     { // do this every 2^nth time..
		showSig = 1;// time to refresh display.

		// This decision tree turns off gas valve servo pulses
		// when no flow is detected. But first moves servo to PARK_SPOT,
		// allowing time to PARK.
		// When flow is first detected, a count is started before turning on gas
		// to ignore possible brief false flow sense.
		flow_watch_dog = MAX(0, flow_watch_dog-1);

		if (RUNNING) {
			if (flow_watch_dog <=0) {//no flow: start parking
				if (parkCTR > 0) {// parking
					CONTROL_OFF;
					SERVO_POS = PARK_SPOT;
					parkCTR--;
				}
				else {// parked
					SERVO_OFF;
				}
			}
		}
		else {// not running
			if (flow_watch_dog > 0) {// if flow sensed:
				if (unParkCTR > 0) {// unparking -- delay startup in case bogus flow sense.
					unParkCTR--;
				}
				else {// done unparking
					SERVO_ON;
					CONTROL_ON;
					parkCTR = PARK_COUNT; // for later, when flow stops
				}
			}
			else unParkCTR = UNPARK_COUNT; //no flow sensed -- (re)start unParkCTR.
		}
	}
	// debounce joystick buttons
	if      (((~PINB) & PINB_MASK) | ((~PINE) & PINE_MASK)) buttonPressed++;
	else buttonPressed--;
	buttonPressed = LIMIT(0, buttonPressed, DEBOUNCE_CTR);
}


ISR(SIG_PIN_CHANGE1) { // flow sensor interrupt
       time = (timer2HiCntr<<8)|TCNT2;
       inv_flow += time-prevTime;// add reading
       inv_flow -= inv_flow>>3;// subtract average so inv_flow is running average * 2^3.

       prevTime = time;
       flow_watch_dog = FLOW_WATCH_SET;// flow watch-dog.
}


ISR(ADC_vect) { // adc interrupt
	ADC_Results[adc_chan_ndx] += ADCL; // add reading lo byte
	ADC_Results[adc_chan_ndx] += (ADCH<<8); // add reading hi byte
	ADC_Results[adc_chan_ndx] -= ADC_Results[adc_chan_ndx]>>FAHR_SHFT; // running average
	ADMUX = (++adc_chan_ndx)%N_ADC_CHANS;// point to next channel
	ADCSRA |= (1<<ADSC);//start next
}


ISR(SIG_OUTPUT_COMPARE1A) {
	// P I D,  see http://en.wikipedia.org/wiki/PID_controller
	err -= err>>4;// for error averaged over last 16 readings, * 16
	err += setting-outTemp;
	integ += err;//
	integ = LIMIT(-WIND_UP, integ, WIND_UP);
	deriv -= deriv>>4;// for deriv averaged over last 16 readings, *  16
	deriv += err-prev_err;
	prev_err = err;

	// make servo position be proportional to flow times required temperature rise.
	// add in feedback correction.
	pre_servo = (int16_t)((((float)(setting-inTemp)*K_FEED_FOWRD) /inv_flow)
	                       +(Kp*(float)err + Ki*(float)integ + Kd*(float)deriv));

#ifdef SERVO_REVERSE
	SERVO_POS = (int16_t)(LIMIT(SERVO_MIN, (SERVO_ZERO - pre_servo), SERVO_MAX));
#else
	SERVO_POS = (int16_t)(LIMIT(SERVO_MIN, (SERVO_ZERO + pre_servo), SERVO_MAX));
#endif
}


