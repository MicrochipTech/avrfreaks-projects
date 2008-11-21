#include "WiSHABI_TX.h"

unsigned char Global_TX_Bytes[4] = {0,0,0,0};

void hardware_Init (void)
{
	//All inputs
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;

	//Internal pull-ups active
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;

	//ADC Hardware Initialisation
	//ADSCRA - ADC Control and Status Register A
	//ADEN - ADC Enable
	//ADPS2:0 - ADC Prescaler Select Bits - Set to Prescaler = 128, with 12MHz XTal -> ADC Frequency = 93750Hz
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	
	//ADUMX - ADC Multiplexer Selection Register
	//REFS1:0 - Voltage Reference Selection for ADC - Set to use AVcc
	//ADLAR - ADC Left Adjust Bit - Set to use 8MSB (From 10 bit ADC)
	ADMUX |= (1 << REFS0) | (1 << ADLAR);

	//Global Interrupt Enable
	sei();

	//Initialise the USART
	USART_Init(BAUDRATE);
}

void Send_Packet (void)
{
	unsigned int i;
	for (i=0;i<TX_ITERATIONS;i++)
	{
		USART_Transmit(NUL);
		USART_Transmit(NUL);
		USART_Transmit(NUL);
		USART_Transmit(SOH);
		USART_Transmit(STX);
		USART_Transmit(Global_TX_Bytes[0]);//data - The actual data byte to be sent
		USART_Transmit(Global_TX_Bytes[1]);
		USART_Transmit(Global_TX_Bytes[2]);
		USART_Transmit(Global_TX_Bytes[3]);
		USART_Transmit(ETX);
		USART_Transmit(EOT);
		USART_Transmit(NUL);
	}
}

//Returns the ADC value for the specified channel.
unsigned char Get_ADC(unsigned char channel)
{
	//ADMUX - ADC Multiplexer Selection Register
	//MUX3..0 - Analog Channel Selection Bits
	ADMUX |= channel;
	ADMUX &= (0xF8 + channel);//0xF8 = 0b11111000, required to clear any of the last 3 bits that require clearing.
	// Start new ADC conversion
	ADCSRA |= (1 << ADSC);
	//Wait until any pending A to D conversions are completed.
	while ((ADCSRA & _BV(ADSC)) != 0){}

	return(ADCH);
}

//Set the values of the global byte array to be transmitted.
//Global_TX_Bytes[0] : Button Status
//Global_TX_Bytes[1] : ADXL330 Accelerometer X-Axis Status
//Global_TX_Bytes[2] : ADXL330 Accelerometer Y-Axis Status
//Global_TX_Bytes[3] : ADXL330 Accelerometer Z-Axis Status
void Poll_Status (void)
{
	Global_TX_Bytes[0] = (~PINB) & 0b00011111;//Only want 5 LSB for buttons, inverted due to pull-ups on inputs.
	Global_TX_Bytes[1] = Get_ADC(CHANNEL_X);
	Global_TX_Bytes[2] = Get_ADC(CHANNEL_Y);
	Global_TX_Bytes[3] = Get_ADC(CHANNEL_Z);
}

int main(void)
{
	unsigned char Last_TX_Bytes[4] = {0,0,0,0};
	unsigned char i;

	//Initialise hardware
	hardware_Init();

	//Main Event Loop
	while (1)
	{
		Poll_Status();
		
		//If the value of any of the data bytes has changed,
		//send the new data.
		if (	(Last_TX_Bytes[3] == Global_TX_Bytes[3]) | 
				(Last_TX_Bytes[2] == Global_TX_Bytes[2]) |
				(Last_TX_Bytes[1] == Global_TX_Bytes[1]) |
				(Last_TX_Bytes[0] == Global_TX_Bytes[0]) )
		{
			Send_Packet();
			for(i=0;i<4;i++)
			{
				Last_TX_Bytes[i] = Global_TX_Bytes[i];
			}
		}
	}
	return 0;
}
