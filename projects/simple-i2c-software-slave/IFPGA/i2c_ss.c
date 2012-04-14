/*
	Very Simple I2C Software Slave. 
	
	NO assembler exzept "_delay_us" from <avr/delay.h>
	Tested on ATtiny84 running on internal 8MHz RC clock, WinAVR + AVR Studio 4.
	SCL speed to 50kHz (may be higher).
	Source code has no specific optimizations.
	
	Algorithm built around Finite State Machine. 
	
	Uses only one PinChange interrupt for START condition detection.
	
	Predefined:
	SDA pin: PORTB1 (PCINT8)
	SCL pin: PORTB0 (PCINT9)
	
	Hardcoded data processing (device/board specific), see text
	(this design controls small Rf PGA chip AD8369).
	
	(c) kinkajou, 2011
	kinkajou@mtu-net.ru
	
	Join the Dark Side!.. we have cookies.
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "defines.h"

#define DELAY() _delay_us(5)
//#define DELAY() asm("nop")

typedef enum
{
	I2C_IDLE,
	I2C_START,
	I2C_SAMPLE,
	I2C_SETUP,
	I2C_CHANGE,
	I2C_ACK,
	I2C_WACK,
	I2C_NACK,
	I2C_WSTOP,
	I2C_STOP
} Bus_State_t;

typedef enum
{
	HEADER = 0,
	DATA = 1
} What_t;

typedef enum
{
	MASTER_READ=1,
	MASTER_WRITE=0
} DataDirection_t;

#if 0
volatile Bus_State_t Bus_State = I2C_IDLE;
volatile What_t What;
volatile DataDirection_t DataDirection;
#else /* иначе дебагер тупит */
volatile uint8_t Bus_State = I2C_IDLE;
volatile uint8_t What;
volatile uint8_t DataDirection;
#endif

volatile uint8_t BitCount, BitSkip;
volatile uint8_t DataBit0, DataBit1, DataReg;

#define BOARD_ADDR	0x20

extern uint8_t ReceivedDataReady, ReceivedData;

void ProcessData (uint8_t Data);

void led_on(void);
void led_off(void);
void led_tgl(void);

void AD8369_SetGain(uint8_t Gain);
static uint8_t IF_Gain=0;

volatile struct
{
	uint8_t
		SDA, SCL,
		xSDA, xSCL;
	uint8_t ByteDone;
	uint8_t BitSkip;
	uint8_t DataDirection;
	uint8_t HeaderReceived;
	uint8_t AddrMatch;
} StateFlags;

uint8_t TestArray[4] = 
{
	0xBA, 0xAD, 0xF0, 0x0D
};
uint8_t TAIndex = 0;

ISR(PCINT1_vect)
{
	cli();
	uint8_t I2CState;
	StateFlags.SDA = ((PINB & (1<<SDApin))>>SDApin) & 0x01;

	if (StateFlags.SDA == 0x00) /* transition from HI to LO */
	/* START condition detected! */
	{
		I2CState = I2C_START;

		while (I2CState != I2C_STOP)
		{
			switch (I2CState)
			{
				case I2C_START:
				{
					if (((PINB & (1<<SCLpin))>>SCLpin) == 0)
					/* wait until SCL goes LO */
					{
						PULL_SCL();
						I2CState = I2C_CHANGE;
						BitCount = 0;
						DataReg = 0;
						StateFlags.HeaderReceived = 0;
						StateFlags.DataDirection = MASTER_WRITE;
						RELEASE_SCL();
					};
				};
				break;

				case I2C_CHANGE:
				{
					if (StateFlags.DataDirection == MASTER_WRITE)
					{						
						if (((PINB & (1<<SCLpin))>>SCLpin) == 1)
						{
							StateFlags.SDA = (PINB & (1<<SDApin))>>SDApin;
							I2CState = I2C_SAMPLE;
						}
					}
					else
					{
						if (((PINB & (1<<SCLpin))>>SCLpin) == 1)
						{
							I2CState = I2C_SETUP;
							//StateFlags.BitSkip = 0;
						};
					};
				};
				break;

				case I2C_SETUP:
				/* SCL is LO */
				{
					if (((PINB & (1<<SCLpin))>>SCLpin) == 0)
					{
						PULL_SCL();

						if (BitCount == 8)
						{
							RELEASE_SDA();
							BitCount = 0;
							DELAY();
							I2CState = I2C_WACK;
						}
						else
						{
							StateFlags.SDA = (DataReg & 0x80)>>7;
							OVR_BIT (I2Cport, SDApin, StateFlags.SDA);
							DataReg <<= 1;
							BitCount ++;
							I2CState = I2C_CHANGE;
						};
						RELEASE_SCL();
					}
				};
				break;

				case I2C_WACK:
				{
					if (!StateFlags.BitSkip)
					{
						if ((PINB & (1<<SCLpin))>>SCLpin)
						/* SCL transited to HI */
						{
							StateFlags.SDA = ((PINB & (1<<SDApin))>>SDApin);
							/* Save SDA state */
							StateFlags.BitSkip = 1;
						}
					}
					else
					{
						if (StateFlags.SDA != ((PINB & (1<<SDApin))>>SDApin))
						/* SDA changed */
						{
							if (StateFlags.SDA) /* was 1, became 0 -> [RE]START */
							{
								I2CState = I2C_START;
							}
							else /* was 0, became 1 -> STOP */
							{
								I2CState = I2C_STOP;
							}
						}
						else
						if (((PINB & (1<<SCLpin))>>SCLpin) == 0)
						/* SCL transited to LO */
						{
							PULL_SCL();
							StateFlags.BitSkip = 0;
							if (StateFlags.SDA)
							/* SDA is HI - > NACK from MASTER, end of transmission */
							{
								/* Something important to do */
								I2CState = I2C_WSTOP;
								//led_tgl();
							}
							else
							/* SDA was LO -> ACK from MASTER, next byte requested */
							{
								/* Get next data byte, etc. */
								DataReg = TestArray[TAIndex++];
								if (TAIndex > 3) TAIndex = 0;
								BitCount = 0;
								I2CState = I2C_SETUP;
								DRIVE_SDA();
								//I2CState = I2C_CHANGE;								
								//led_tgl();

								StateFlags.SDA = (DataReg & 0x80)>>7;
								OVR_BIT (I2Cport, SDApin, StateFlags.SDA);
								DataReg <<= 1;
								BitCount ++;

								DELAY();
							};
							//I2CState = I2C_CHANGE;
							//DataReg = 0xAD;
							RELEASE_SCL();
						}
					}
				};
				break;

				case I2C_WSTOP:
				{
					if ((PINB & (1<<SCLpin))>>SCLpin)
					{
						StateFlags.SDA = (PINB & (1<<SDApin))>>SDApin;
						if (StateFlags.SDA)
						{
							I2CState = I2C_STOP;

							RELEASE_SCL();
							RELEASE_SDA();
						};
					};
				};
				break;

				case I2C_SAMPLE:
				{
					if (StateFlags.SDA != ((PINB & (1<<SDApin))>>SDApin))
					/* SDA changed */
					{
						if (StateFlags.SDA) /* was 1, became 0 -> [RE]START */
						{
							I2CState = I2C_START;
						}
						else /* was 0, became 1 -> STOP */
						{
							I2CState = I2C_STOP;
						}
					}
					else if (((PINB & (1<<SCLpin))>>SCLpin) == 0)
					/* if SCL goes LO */
					{
						PULL_SCL(); /* clock stretchin ON */
						DataReg = ((DataReg << 1) + StateFlags.SDA);						
						if (BitCount == 7) /* all data bits are shifted in */
						{							
							if (StateFlags.HeaderReceived)
							{
								if (StateFlags.AddrMatch)
								{
									I2CState = I2C_ACK;
									PULL_SDA();

									/*************************************************************************/
									/* In Situ Data Processing (SCL "frozen", master waiting for completion) */
									/*************************************************************************/
									switch (DataReg)
									{
										case 0x08:
											if (IF_Gain > 15) 
											{
												IF_Gain = 15;
												//AD8369_SetGain (IF_Gain);
											}
											else
											{
												IF_Gain += 1;
												//AD8369_SetGain (IF_Gain);												
											};

											//led_on(); 
										break;

										case 0x09: 																					
											if (IF_Gain == 0) 
											{
												IF_Gain = 0;
												//AD8369_SetGain (IF_Gain);
											}
											else
											{
												IF_Gain -= 1;
												//AD8369_SetGain (IF_Gain);
											}

											//led_off(); 
										break;
										
										default: 
											led_off(); 
											/* Amplifiers Power OFF */
										break;
									}
									
								}
								else
								{
									I2CState = I2C_NACK;
								}
							}
							else
							{
								if ((DataReg & 0xFE) == BOARD_ADDR)
								{
									I2CState = I2C_ACK;
									PULL_SDA();
									DELAY();
									StateFlags.AddrMatch = 1;
									StateFlags.DataDirection = DataReg & 0x01;
								}
								else
								{
									I2CState = I2C_NACK;
									StateFlags.AddrMatch = 0;
								}
								StateFlags.HeaderReceived = 1;
							}
							BitCount = 0;
						}
						else
						{
							BitCount++;
							I2CState = I2C_CHANGE;
						};
						RELEASE_SCL(); /* clock stretchin OFF */
					};
				};
				break;

				case I2C_ACK:
				{
					if (!StateFlags.BitSkip)
					{
						if (((PINB & (1<<SCLpin))>>SCLpin) == 1)
						{
							StateFlags.BitSkip = 1;							
						}
					}
					else
					{
						if (((PINB & (1<<SCLpin))>>SCLpin) == 0)
						{
							PULL_SCL();
							StateFlags.BitSkip = 0;
							BitCount = 0;
							RELEASE_SDA();
							if (StateFlags.DataDirection == MASTER_READ)
							{
								//DataReg = 0xBA; /* Dummy data for testing */
								DataReg = IF_Gain + 0x80;
								I2CState = I2C_SETUP; /* will drive bus */
								DRIVE_SDA();
								DELAY();								

								StateFlags.SDA = (DataReg & 0x80)>>7;
								OVR_BIT (I2Cport, SDApin, StateFlags.SDA);
								DataReg <<= 1;
								BitCount ++;
							}
							else
							{
								I2CState = I2C_CHANGE; /* will sample bus */
								//I2CState = I2C_SAMPLE; /* will sample bus */
							};
							//I2CState = I2C_CHANGE;

							//BitCount = 0;

							
							RELEASE_SCL();
						}
					}
				};
				break;

				case I2C_NACK:
				{
					RELEASE_SDA();
					if (!StateFlags.BitSkip)
					{
						if (((PINB & (1<<SCLpin))>>SCLpin) == 1)
						{
							StateFlags.BitSkip = 1;							
						}
					}
					else
					{
						if (((PINB & (1<<SCLpin))>>SCLpin) == 0)
						{
							StateFlags.BitSkip = 0;
							I2CState = I2C_CHANGE;
						}
					}
				};
				break;

				default:
				break;
			}; /* switch (I2C_STATE) */
		}; /* while (I2CState != I2C_STOP) */
	}; /* START condition process stop, return from interrupt */
	//RELEASE_SCL();
	sei();
}

void I2C_SoftwareSlave_Init()
{
	CLR_MSK(I2Cddr, ((1<<SCLpin)|(1<<SDApin))); /* оба пина - входы */
	CLR_MSK(I2Cport, ((1<<SCLpin)|(1<<SDApin))); /* high impedance */

	StateFlags.xSDA = 0x03;
	StateFlags.xSCL = 0x03;

	//I2Cport |= 0x03;

	SDA_INT_ON();
	SCL_INT_OFF();

	GIMSK	|= (1<<PCIE1);

	Bus_State = I2C_IDLE;
}
