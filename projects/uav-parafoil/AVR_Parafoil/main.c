//UAV parafoil main code
#include "main.h"
#include "ground.h"
//globals
unsigned char EEMEM windeast[255];
unsigned char EEMEM windnorth[255];
 
unsigned char EEMEM flight_status;
 
volatile kalman_state our_state;
volatile kalman_model our_model;
 
volatile float Heading;
volatile float Target;
volatile u08 Heading_flag;
 
volatile u08 gyro_error=0x00;
 
gps_type gps;
//ground control globals
#ifdef GROUND
extern volatile u08 pwm_counter;
extern volatile u08 pwm_period;
#else
#define pwm_counter (u08)0x00
#endif
 
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
 
    void get_mcusr(void) \
      __attribute__((naked)) \
      __attribute__((section(".init3")));
    void get_mcusr(void)
    {
      mcusr_mirror = MCUSR;
      MCUSR = 0;
      wdt_disable();
    }															//avr-libc wdt code
 
void setup()
{
	outb(UCSR0B, BV(RXCIE0)|BV(RXEN0)|BV(TXEN0));	//mega xx8 registers - enable tx and rx, with interrupts on RX only
	outb(UBRR0L, bauddiv);
	outb(UBRR0H, bauddiv>>8);									//end of UART setup
	//ADC setup
	sbi(ADCSR, ADEN);               // enable ADC (turn on ADC power)
	cbi(ADCSR, ADFR);               // default to single sample convert mode
	outb(ADCSR, ((inb(ADCSR) & ~0x07) | 0x06));  // set prescaler 64
	outb(ADMUX, ((inb(ADMUX) & ~0xC0) | (0x03<<6))); // set reference 2.56V
	cbi(ADMUX, ADLAR);              // set to right-adjusted result
 
	//sbi(ADCSR, ADIE);               // enable ADC interrupts
 
	//Timer 1 setup
	TCCR1B = ((TCCR1B & ~0x07) | 0x02);			//clock at F_CPU/8
 
	TCNT1 = 0;		//reset timer1  
 
	// set PWM mode with ICR top-count
	cbi(TCCR1A,WGM10);
	sbi(TCCR1A,WGM11);
	sbi(TCCR1B,WGM12);
	sbi(TCCR1B,WGM13);
 
	// set top count value to give correct period PWM
	ICR1 = (u16)F_CPU*delta_time/8.0;
 
	// clear output compare value A
	OCR1A = 0;
	// clear output compare value B
	OCR1B = 0;
 
	// turn on channel A (OC1A) PWM output
	// set OC1A as non-inverted PWM for direct servo connection
	sbi(TCCR1A,COM1A1);
	cbi(TCCR1A,COM1A0);
 
	//Timer0 setup
	#ifdef GROUND
	TCCR0B = ((TCCR0B & ~0x07) | 0x04);			//clock at F_CPU/256
 
	TCNT0=0x00;
	#endif
	//External interrupts setup
 
	outb(PCMSK1,(1<<PCINT8)|(1<<PCINT9));
 
	//SPI setup
	outb( SPCR, (1<<MSTR)|(1<<SPE));		//F_CPU/4, master, CPOL=CPHA=0
 
	//IO setup
	gyro_off;													//SS to the gyro - pullups
	PORTD|=( (1<<3) | (1<<4) );									//pullups to radio cts and dead man switch			
	DDRB=0b00101111;											//talk to servo and gyro
	DDRC=0b00011000;											//indicator leds
	DDRD=0b10100000;											//cutdown line and "heartbeat" led	
 
	//Kalman setup
	our_model.F.tl=1.0;											//kalman model setup
	our_model.F.tr=delta_time;
	our_model.F.bl=0.0;
	our_model.F.br=1.0-(damping_constant*delta_time);
 
	our_model.B.tl=0.0;
	our_model.B.tr=(delta_time*delta_time*control_gain)/2.0;
	our_model.B.bl=0.0;
	our_model.B.br=delta_time*control_gain;
 
	our_model.H.tl=0.0;											//set to 1 for a gps update
	our_model.H.tr=0.0;
	our_model.H.bl=0.0;
	our_model.H.br=1.0;											//this is set for a gyro update
 
	our_model.Q.tl=0.1/sqrt_delta_time;				//estimated - corresponds to heading change
	our_model.Q.tr=0.0;
	our_model.Q.bl=0.0;
	our_model.Q.br=1.0/sqrt_delta_time;				//estimated - corresponds to rate change
 
	our_model.R.tl=20.0;								//or around 20 in the case of a GPD update
	our_model.R.tr=0.0;
	our_model.R.bl=0.0;
	our_model.R.br=1.0;								//approximate for an MLX90609
 
	our_state.state.t=0.0;
	our_state.state.b=0.0;
 
	our_state.P.tl=150.0;								//we might be pointing in any direction
	our_state.P.tr=0.0;
	our_state.P.bl=0.0;
	our_state.P.br=20.0;								//rough estimate for turn rate error at release
 
}
 
float * read_gyro()
{
	static float turn_rate;
	u16 a;
	gyro_on;									//SS = 0
	SPDR=read_rate;
	while(!(inb(SPSR) & (1<<SPIF)));
	SPDR=0x00;
	while(!(inb(SPSR) & (1<<SPIF)));
	SPDR=0x00;
	while(!(inb(SPSR) & (1<<SPIF)));
	gyro_off;
	_delay_us(150);
	gyro_on;
	SPDR=read_melexis;
	while(!(inb(SPSR) & (1<<SPIF)));
	SPDR=0x00;
	while(!(inb(SPSR) & (1<<SPIF)));
	a=(u16)SPDR<<8;
	gyro_error=SPDR&0b01000000;						//internal selftest NOTE, not on latest datasheet, obsolete?
	SPDR=0x00;
	while(!(inb(SPSR) & (1<<SPIF)));
	a|=(u16)SPDR;
	gyro_off;
	a=(a>>1)&mask_word;								//all based on the rogallo code
	turn_rate=(125.0/1024.0)*(float)(a-gyro_null);			//+-125 degree/sec gyro, 11 bit resolution
	return &turn_rate;
}
 
ISR(TIMER1_COMPA_vect)							//fires at the end of the PWM pulse
{
	sei();										//we need to be able to nest UART interrupts in here
	vector measurement_vector;
	static u08 timer;
	static vector control_vector;
	static float integral;
	float a;
	control_vector.t=0.0;						//we control one servo, bottom
	if(Heading_flag)
	{
		our_model.H.tl=1.0;
		measurement_vector.t=Heading;			//from the main loop nav code
		Heading_flag=FALSE;
	}
	else
	{
		our_model.H.tl=0.0;						//setup the model to reflect the data
		measurement_vector.t=0.0;
	}
	measurement_vector.b=*read_gyro();
	if(gyro_error)
	{
		our_model.H.br=0.0;
		measurement_vector.b=0.0;					//faulty gyro, limp home mode
	}
	else
	{
		our_model.H.br=1.0;
	}
	our_state=predict_and_update(&our_state,&our_model,&control_vector,&measurement_vector);
	if(our_state.state.t>180)					//keeps us in the =-180 degree range
	{
		our_state.state.t-=360;
	}
	if(our_state.state.t<-180)
	{
		our_state.state.t+=360;
	}
	a=our_state.state.t-Target;
	if(a>180)									//heading offset
	{
		a-=360;
	}
	if(a<-180)
	{
		a+=360;
	}
	integral+=a;								//integral term limits
	if(integral>integral_limit)
	{
		integral=integral_limit;
	}
	if(integral<-integral_limit)
	{
		integral=-integral_limit;
	}
	if(timer>4)
	{	
		control_vector.b=P_C*a+I_C*integral+D_C*our_state.state.b;		//PID control
		if(control_vector.b>1)							//servo limits
		{
			control_vector.b=1.0;
		}
		if(control_vector.b<-1)
		{
			control_vector.b=-1.0;
		}
		OCR1A=(u16)(((float)F_CPU/(500.0*8.0))*(3.0+control_vector.b));	//PWM=1.5+-0.5 uS
	}
	else
	{
		timer++;				//we use timer to turn on control after a few seconds
	}
	#ifdef GROUND
	if(pwm_counter==0x0A)		//if under ground control, use the recorded pwm as a control input
	{
		control_vector.b=(float)(pwm_period-3*pwm_record_factor);	//1500us -> 0, 2000 -> +2, 1000 -> -1
	}
	#endif
}
 
u08 cutdownrulecheck(int time)
{
	if(gps.altitude>altitudelimit)
	{
		printf("CUTDOWN, Altitude limit\r\n");
  		return 1;
	}
	if(time>timelimit)
	{
		printf("CUTDOWN, Time limit\r\n");
		return 2;
	}
	return 0;
}
 
u08 checksum(char *c_buf)
{
    u08 i;
    u08 sum=0;
    u08 size=strlen(c_buf);
    for(i=0 ; i<size ; i++,c_buf++)
    {
    	sum += *c_buf;
    }
    return sum;
}
 
void cutdown(char time)
{
	cutter_on;
	for(;time;time--)
	{
		_delay_ms(1000);
		wdt_reset();
	}
	cutter_off;
}
 
void wiggleservo()
{
	OCR1A =(u16)(((float)F_CPU/500.0)*4.0);
	_delay_ms(150);
	OCR1A =(u16)(((float)F_CPU/500.0)*2.0);
	_delay_ms(150);
	OCR1A =(u16)(((float)F_CPU/500.0)*3.0);
}
 
float getbatteryvoltage()
{
	//a2dCompleteFlag = FALSE;                // clear conversion complete flag
	outb(ADMUX, (inb(ADMUX) & ~0x1F) | (battery_chan & 0x1F));    // set channel
	sbi(ADCSR, ADIF);                       // clear hardware "conversion complete" flag 
	sbi(ADCSR, ADSC);                       // start conversion
	//while(!a2dCompleteFlag);              // wait until conversion complete
	//while( bit_is_clear(ADCSR, ADIF) );       // wait until conversion complete
	while( bit_is_set(ADCSR, ADSC) );       // wait until conversion complete
 	// CAUTION: MUST READ ADCL BEFORE ADCH!!!
	return battery_factor*((float)(inb(ADCL) | (inb(ADCH)<<8)));    // read ADC (full 10 bits);
}
 
char *datastring()
{
	static char outputbuffer[80];
	char longitude;
	char latitude;
 
	if (gps.longitude>0)
	{
		longitude='E';
	}
	else
	{
		longitude='W';
	}
	if (gps.latitude>0)
	{
		latitude='N';
	}
	else
	{
		latitude='S';
	}
	sprintf(outputbuffer,"%.6f,%c,%.6f,%c,%.1f,M,%.1f,%.1f,%.1f,%.1f,%.1f,%X",(double)gps.latitude,latitude,
	(double)gps.longitude,longitude,(double)gps.altitude,(double)Heading,(double)Target,(double)our_state.state.t,
	(double)our_state.state.b,(double)getbatteryvoltage(),pwm_counter);
	sprintf(outputbuffer,"%s%X",outputbuffer,checksum(outputbuffer));		//we generate a string 
	return outputbuffer;								//with checksum, containing position and filter info
}
 
int put_char(char c, FILE * stream)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);         //unbuffered tx comms
	UDR0 = c;
 	return 0;
}
 
int get_char(FILE * stream)
{
	return -1;
}
 
int main()
{
	wdt_enable(WDTO_4S);				//watchdog set to 4 seconds
	int time=0;
	int L;
	int Heightupto=0;
	float K;
	float Windx_total=0;
	float Windy_total=0;
	float Wind_x;
	float Wind_y;
	FILE mystdio = FDEV_SETUP_STREAM(put_char, get_char, _FDEV_SETUP_RW);  //so we can printf to the radio
	setup();
	stdout = &mystdio;
	sei();
	if(undead_man)							//dead man switch to gnd D.4
	{
		while(!gps.status)							//wait for a gps lock
		{
			printf("Waiting for GPS\r\n%.5f\r\n%.5f",(double)gps.latitude,(double)gps.longitude);
			wdt_reset();
			while(!gps.packetflag)					//wait for some new gps
			gps.packetflag=FALSE;
		}
		for (L=0;L<255;L++)
		{
			printf("Record number %d: East,%d",L,(int)eeprom_read_byte(&windeast[L]));
			printf(", West,%d\r\n",(int)eeprom_read_byte(&windnorth[L]));
			wdt_reset();
		}
		if(eeprom_read_byte(&flight_status)!=0x00)	//set eeprom status to ascending
		{
			eeprom_write_byte(&flight_status,0x00);
		}
		printf("Test ground control function\r\n");
		#ifdef GROUND
		enable_ground_control();
		for(;L>250;L--)								//5 second delay
		{
			_delay_ms(1000);						//time to test ground control
			wdt_reset();							//reset watchdog
		}
		disable_ground_control();
		#endif
		L=0;										//need it =0 for next stage
		printf("Ok, ready for launch\r\n");
	}
	if(eeprom_read_byte(&flight_status)==0x00)		//0x00=ascent
	{
		while(!cutdownrulecheck(time))
		{
 			time++;
 			Windx_total += gps.speed*sin(Deg2rad*gps.heading);            //add it to our total wind
 			Windy_total += gps.speed*cos(Deg2rad*gps.heading);
 			L++;                              //incrament the denominator for our averaging
        	        //find the altitude in 50m incraments
			if ((int)(gps.altitude/50.0) > Heightupto)    //do we now fall into another (higher) 50m incrament ?
 			{
				Heightupto++;                           
				eeprom_write_byte(&windeast[Heightupto],(u08)(Windx_total/L + 127));         //we are storing as a signed byte
				eeprom_write_byte(&windnorth[Heightupto],(u08)(Windy_total/L + 127));         //we find the average and shove it in the eeprom
				L = 0;                                                   
			}
			wiggleservo();
			if (radio_cts)                    //CTS from radio
			{
				printf("UKHAS>,%s\r\n",datastring());
			}
			wdt_reset();
			while(!gps.packetflag)					//wait for some new gps
			gps.packetflag=FALSE;
		}
		cutdown(12);
		eeprom_write_byte(&flight_status,0x01);		//we have cutdown=decent
	}
	sbi(TIMSK1, OCIE1A);						//enable output compare interrupt - turns on guidance code
	for(;;)
	{
		L=(int)gps.altitude/50.0;
		Wind_x=(float)(eeprom_read_byte(&windeast[L])-127);
		Wind_y=(float)(eeprom_read_byte(&windnorth[L])-127);	//load valid wind data
		//direction to target (uses equatorial degree units)
		Target = Rad2deg*atan2((targeteast - gps.longitude)*cos(Deg2rad*gps.latitude),targetnorth - gps.latitude);
	        //wind compensation  in ENU frame
		Heading =Rad2deg*atan2(gps.speed*sin(gps.heading)-Wind_x,gps.speed*cos(gps.heading)-Wind_y);
		Heading_flag=TRUE;
		K=Heading-Target;				//k is now our heading offset (from now on is just for leds)
		if (K >180)
		{
			K -= 360;
		}                                                       //all to keep us in +-180 degree range
		if(K < -180)
		{
			K += 360;
		}  
		if (K > 0) 					            
		{
			led_left;						//left/right indictor: bicolour LED between 3 and 4
		}
		else
		{
			led_right;   
		}
 		if (radio_cts)                    //CTS from radio
		{
			printf("UKHAS<,%s\r\n",datastring());
		}
		#ifdef GROUND
		if(gps.altitude<200.0)					//near the ground, ground control
		{
			enable_ground_control();
		}
		#endif
		wdt_reset();
		while(!gps.packetflag)					//wait for some new gps
		gps.packetflag=FALSE;
	}
}
