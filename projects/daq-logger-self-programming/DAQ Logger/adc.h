/***********************************************

		ADC.H
					
	ADC  routunes for the AVR device
	
************************************************/

/*		DEFINITIONS		*/

#define ADC_DDR DDRA

#define REF_AREF 0
#define REF_AVCC (1<<REFS0)
#define REF_INTERNAL ((1<<REFS1)|(1<<REFS0))
#define CHANNEL_MASK (0b00011111)
#define PRESCALAR_4 (2<<ADPS0)
#define PRESCALAR_8 (3<<ADPS0) 
#define PRESCALAR_16 (4<<ADPS0) 
#define PRESCALAR_32 (5<<ADPS0) 
#define PRESCALAR_64 (6<<ADPS0) 
#define PRESCALAR_128 (7<<ADPS0) 

/*		MACROS			*/


/*		FUNCTIONS		*/


/* Initialise ADC */
void adc_init(uint8_t REF);

/*  change reference source and channel	*/
void adc_configure(uint8_t admux); 
 

/* Do a single converion and returns result in 8-bits */
uint8_t adc_read_8bit(void);

/* Do a single converion and returns result in 10-bits */
uint16_t adc_read_10bit(void);
