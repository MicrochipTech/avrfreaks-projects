#define ADC_ENABLE 					ADCSRA |= (1<<ADEN)
#define ADC_DISABLE 				ADCSRA &= 0x7F
#define ADC_START_CONVERSION		ADCSRA |= (1<<ADSC)


void ADC_init(void);
int ADC_read(void);
float ADC_calculateTemp(int);
unsigned char* updateTempDisplay(float);
unsigned char* temporary(void);
void ADC_transmitValue(void);