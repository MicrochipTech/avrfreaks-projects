/*---------------------------------------------------- I2C Configuration -----------------------------------------------------*/


/* Speed definition:

F_CPU= 16000000UL for Arduino Nano




F_CPU	16000000			
	Prescaler			

Bus	1	4	16	64
50000	152	38	9,5	2,375
100000	72	18	4,5	1,125
150000	45	11	2	0
200000	32	8	2	0,5
250000	24	6	1,5	0
300000	18	4	1	0
350000	14	3	0	0
400000	12	3	0	0

*/

#define ATmega168_I2C_Prescaler_1	TWSR &= ~_BV( TWPS1) & ~_BV( TWPS0)
#define ATmega168_I2C_Prescaler_4	TWSR |=  _BV( TWPS0)
#define ATmega168_I2C_Prescaler_16	TWSR |=  _BV( TWPS1)
#define ATmega168_I2C_Prescaler_64	TWSR |=  _BV( TWPS1) | _BV( TWPS0)

#define ATmega168_I2C_Prescaler_1_Mask  0xF8 // Se usa esta por defecto
#define ATmega168_I2C_Prescaler_4_Mask  0xF9
#define ATmega168_I2C_Prescaler_16_Mask 0xFA
#define ATmega168_I2C_Prescaler_64_Mask 0xFB

#define ATmega168_I2C_Bit_Rate__50K     ATmega168_I2C_Prescaler_1; TWBR= 152
#define ATmega168_I2C_Bit_Rate__100K    ATmega168_I2C_Prescaler_1; TWBR=  72
#define ATmega168_I2C_Bit_Rate__150K    ATmega168_I2C_Prescaler_1; TWBR=  45
#define ATmega168_I2C_Bit_Rate__200K    ATmega168_I2C_Prescaler_1; TWBR=  32
#define ATmega168_I2C_Bit_Rate__250K	ATmega168_I2C_Prescaler_1; TWBR=  24
#define ATmega168_I2C_Bit_Rate__300K	ATmega168_I2C_Prescaler_1; TWBR=  18
#define ATmega168_I2C_Bit_Rate__350K	ATmega168_I2C_Prescaler_1; TWBR=  14
#define ATmega168_I2C_Bit_Rate__400K	ATmega168_I2C_Prescaler_1; TWBR=  12

//#define ATmega168_I2C_Set_Power_Reduction_TWI PRR &= ~_BV( PRTWI);

#define ATmega168_I2C_TWIN_bit 0x80
/*----------------------------------------------------------------------------------------------------------------------------*/

/* Set SDA and SCL to out */

#define ATmega168_I2C_Direction_Port DDRC
#define ATmega168_I2C_Data_Port      PORTC
#define ATmega168_I2C_Pins_Port      PINC

#define ATmega168_I2C_SDA_Pin  4 
#define ATmega168_I2C_SCL_Pin  5 

#define ATmega168_I2C_Enabled_Port    ATmega168_I2C_Direction_Port |= _BV( ATmega168_I2C_SDA_Pin) | _BV( ATmega168_I2C_SCL_Pin)
#define ATmega168_I2C_Set_Ack_Bit                             TWCR |= _BV( TWEA)
#define ATmega168_I2C_Enable_Interrupts			      TWCR |= _BV( TWIE)
#define ATmega168_I2C_Enable_I2C_System			      TWCR |= _BV( TWEN)
/*----------------------------------------------------------------------------------------------------------------------------*/

#define I2C_Write 0
#define I2C_Read	1

void ATmega32_I2C_Init()
{
ATmega168_I2C_Bit_Rate__400K;
//ATmega168_I2C_Set_Power_Reduction_TWI;
ATmega168_I2C_Enabled_Port;
ATmega168_I2C_Enable_I2C_System;
//ATmega168_I2C_Enable_Interrupts;
}









