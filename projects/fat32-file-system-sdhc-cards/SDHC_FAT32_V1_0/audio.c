
#include <includes.h>

/*
Audio module
*/

// Public variables

	//when swapflag gets set, clear it and write to audio_buffer
	char* 			audio_buffer;
	volatile char 	audio_swapflag;
	

// Local variables

	//double buffer for audio data
	static char buffer_a[AUDIO_BUFFER_SIZE];
	static char buffer_b[AUDIO_BUFFER_SIZE];
	static char *buffer;

void audio_init(void)
{
	//Setup timer 0
	TCCR0A |=_BV(WGM00);	// WGM=001 phase correct PWM
	TCCR0A |=_BV(COM0A1);	// non-inverted PWM
	TCCR0B |=_BV(CS00);		// = Fosc (gives 510cy/sample & 15.686kHz sample rate @ 8MHz)

	memset(buffer_a, 0x80, AUDIO_BUFFER_SIZE);
	memset(buffer_b, 0x80, AUDIO_BUFFER_SIZE);

	audio_buffer = buffer_b;
	audio_swapflag = TRUE;
	buffer = buffer_a;

	TIMSK0 |=_BV(TOIE0);

	DDRB |=_BV(PB7);	//OC0A
}

ISR(TIMER0_OVF_vect)
{
	
	OCR0A = *buffer;
	buffer++;

	if(buffer== &buffer_a[AUDIO_BUFFER_SIZE])
	{
		buffer=buffer_b;
		audio_buffer=buffer_a;
		audio_swapflag=TRUE;
	}
	else if(buffer== &buffer_b[AUDIO_BUFFER_SIZE])
	{
		buffer=buffer_a;
		audio_buffer=buffer_b;
		audio_swapflag=TRUE;
	};
}
