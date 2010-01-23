//unsigned char segTblReturn(unsigned char uLookMeUp);
void uCinit(void);		//initialization routine for the ATMEGA8535
//void clearRecStr(void);


//This takes the 7-segment LED code from the Eagle counting unit and converts it into a
// number from 0 to 9.
/*
unsigned char segTblReturn(unsigned char uLookMeUp)
{
	u08 x;
	
	for(x=0;x<10;x++)
	{
		if(PRG_RDB(&strSegTbl[x])==uLookMeUp)	
		{
			return(x);	//we found the number in the 7 segment LED lookup table
		}
	}
	return(0);	//we did not find the number in the lookup table
}
*/

//----------------------------------------------------------------------------
//Routine:     uCinit  (initialization of the microcontroller)
//
//Function:    initialize the I/O lines, timers, interrupts, etc.
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void uCinit(void)
{
   outp(0xF0,DDRA);           //PortA, Pin 5 is an ADC input
   outp(0x0F,PORTA);			//enable the pull up resistor for the POrtA, Pin 5
   outp(0x1F,DDRB);           //only the push buttons are input pins
   outp(0xE0,PORTB);          //enable pull-up resistors
   outp(0xFF,DDRC);           //use all pins on PortA for output
   //outp(0xFC,PORTC);          //the pullup resistors are on all pins except the LCD control lines
   outp(0xF1,DDRD);           //Portd is mostly inputs, except for the backlight enable, and the audio alarm output
   outp(0x0E,PORTD);          //PortD has allmost all the pullup resistors enabled
   
   cbi(defLCDContPort,defLcdRES);       //keep the display in reset
   //8 bit timer 0 initialization, used for the 20mS tock.
    outp((1<<TOIE0) | (1<<TOIE2), TIMSK);   //enable timer 0 and timer 2 overflow
    outp(0,TCNT0);             // reset TCNT0
    outp(1,TCCR0);             // count with cpu clock/1024
	
	//ADC input
	ADMUX=0x00;
	ADCSRA=0x86;
	SFIOR&=0x00; 	


	//8 bit timer 1 initialization, used for the 32.768KHz crystal (Real Time Clock)
	outp((1<<CS20) | (1<<CS22), TCCR2);	//divide by 128 for timer counter 2
    ASSR = (1<<AS2);        // select asynchronous operation of Timer2
    while((ASSR & 0x01) | (ASSR & 0x04));       // wait for TCN2UB and TCR2UB to be cleared
	
    //external interrupt 0 initialization, for pulse from the eagle counting unit
	/*
	outp((1<<INT0), GIMSK);    // enable external int0
	outp((1<<ISC00)|(1<<ISC01), MCUCR);    // rising egde on int0 generates the interrupt
	outp((1<<ISC00), MCUCR);    //a change on int0 generates an interrupt
	*/
    //set OC1A pin as output, required for output toggling
    //DDRD = _BV(PD5);

    //enable 8 bit PWM, select inverted PWM
    //TCCR1A =  _BV(PWM10) | _BV(COM1A1) | _BV(COM1A0);
    
    // timer1 running on 1/8 MCU clock with clear timer/counter1 on Compare Match
    // PWM frequency will be MCU clock / 8 / 512, e.g. with 4Mhz Crystal 977 Hz.
//    TCCR1B = _BV(CS11) | _BV(CTC1);
	//if (eeprom_rb(EEKey1)!=defKey1 || eeprom_rb(EEKey2)!=defKey2 || eeprom_rb(EEKey3)!=defKey3)
	//{


/*
	intAlarmMin=eeprom_read_word(&MyEepVar);
	if(intAlarmMin>59)
	{
		intAlarmMin=1;
	}
	intAlarmHour=eeprom_read_word(&defAlarmHour);
	if(intAlarmHour>23)
	{
		intAlarmHour=6;
	}
*/
//	OCR1AL = 50;

//Get values stored in EEPROM
	if(eeprom_read_word((uint16_t*)&EEKey) != 789)		//have we written to the EEPROM before?
	{
		eeprom_write_word((uint16_t*)&EEKey,789);	//set the key value to let us know that we have written to EEPROM before
		eeprom_write_word((uint16_t*)&EEMinute,6);	//set all of the default EEPROM values
		eeprom_write_word((uint16_t*)&EEHour,6);
		eeprom_write_word((uint16_t*)&EEPhotoTriggerValue,600);
		eeprom_write_byte((u08*)&EEBacklightOnHour,6);
		eeprom_write_byte((u08*)&EEBacklightOffHour,22);	
	}
	intAlarmMin = eeprom_read_word((uint16_t*)&EEMinute);	//Get the Alarm minute out of EERPOM
	if(intAlarmMin>59)		//Is the Alarm Minute an illegal value?
	{
		intAlarmMin=5;	//yes, then reset the alarm minute to a legal value
	}
	intAlarmHour = eeprom_read_word((uint16_t*)&EEHour);	//get the alarm hour out of EEPROM
	if(intAlarmHour>23)		//is the alarm hour an illegal value?
	{
		intAlarmHour=5;		//yes, then reset the alarm hour to a legal value
	}
	intUserPhotoTriggerValue = eeprom_read_word((uint16_t*)&EEPhotoTriggerValue);	//get the alarm hour out of EEPROM
	if(intUserPhotoTriggerValue>1000)		//is the intUserPhotoTriggerValue an illegal value?
	{
		intUserPhotoTriggerValue=500;		//yes, then reset the intUserPhotoTriggerValue to a legal value
	}
	uBacklightOnHour = eeprom_read_byte((u08*)&EEBacklightOnHour);		//get the backlight on time out of EERPOM
	if(uBacklightOnHour>23)		//is the backlight on hour an illegal value?
	{
		uBacklightOnHour=6;		//yes, then reset the ram value to a legal value
	}
	uBacklightOffHour = eeprom_read_byte((u08*)&EEBacklightOffHour);	//get the backlight off time out of EERPOM
	if(uBacklightOffHour>23)	//is the backlight off hour an illegal value?
	{
		uBacklightOffHour=22;	//yes then reset the ram value to a legal value
	}


}


//clear out the data received from the eagle counting unit
/*
void clearRecStr(void)
{
	u08 uClearCount;
	
	for(uClearCount=0;uClearCount<15;uClearCount++)
	{
		strFromEagle[uClearCount]=0;
	}	
}
*/

