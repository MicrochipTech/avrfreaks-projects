#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "command.h"
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&definitions
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&definitions
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&definitions
#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PINB
#define LED      2

#define reset_DDR  DDRB
#define reset_PORT PORTB
#define reset_PIN  PINB
#define reset      PORTB1

#define spi_ddr DDRB

#define os_ddr DDRD
#define os 	   PORTD5

#define sck_ddr  DDRB
#define sck_port PORTB
#define sck	     PORTB7
//Set values as required supress popup messages in AVR-Studio 
#define HARDWARE_VERSION      0x02
#define SOFTWARE_MAJOR        0x02							//softwer ver. 2.04
#define SOFTWARE_MINOR        0x07
/* value for vtarget: always return 5.0V */
#define VTARGET		0x32
/* value for vadjust: always return 5.0V */
#define VADJUST		0x32
#define MAX_BUF_SIZE 275

#define messagesize1 2 
#define messagesize0 3
//888888887777777444444444444444444444444
#define STK501		0xAA
#define STK502		0x55
#define STK503		0xFA
#define STK504		0xEE
#define STK505		0xE4
#define STK520		0xDD	
/* value for vtarget: always return 5.0V */
#define VTARGET		0x32
/* value for vadjust: always return 5.0V */
#define VADJUST		0x32
/* prescalled clock frequesncy equal to system clock */
#define PSCALE_FSYS 0x01
#define CMATCH_DEF	0x00
#define SCK_DURATION_DEF 0x02
//***********************************functions
//***********************************functions
//***********************************functions
static void avr_init(void);
void putch(char ch);
char getch(void);
void flash_led(uint8_t count);
void get_message(void);
void get_message_body(void);
void sendResponse(void);
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void cmdSignOn(void);
void cmdGetParameter(void);
void cmdSetParameter(void);
void cmdEnterProgmodeIsp(void);
void cmdLeaveProgmodeIsp(void);
void cmdReadSignatureIsp(void);
void cmdreadosccalisp(void);
void cmdreadlockisp(void);
void cmdprogramlockisp(void);
void cmdreadfuseisp(void);
void cmdprogramfuseisp(void);
void cmdspimulti(void);
void cmdchiperaseisp(void);
void cmdloadaddress(void);
void cmdprogramflashisp(void);
void cmdreadflashisp(void);
void cmdprogrameepromisp(void);
void cmdreadeepromisp(void);
void cmdosccal(void);
//***********************************program functions
void delay_ms(int time);
char spi(char data);
void EnterProgmodeIsp(void);
void ReadfusebitIsp(void);
void programfuseisp(void);
void SetParameter(void);
//***********************************variables
unsigned char rx_buf[MAX_BUF_SIZE];						// RX buffer
unsigned char tx_buf[MAX_BUF_SIZE];						// TX buffer
unsigned int rx_ptr = 0;  									// pointer to rx buffer
unsigned int tx_ptr = 0;  									// pointer to rx buffer
unsigned char rx_chm=0;									// calculated recived message checksum
unsigned char tx_chm=0;									// calculated transmit message checksum
unsigned long address_flash = 0; 							// actual address in flash memory
unsigned int address_eeprom = 0; 						// actual address in eeprom memory
char extended_address=0;									//for larger flashes
char sckdurationdef=SCK_DURATION_DEF;						//spi frequency division
char cmatchdef=CMATCH_DEF;									//stk500 output frequency
char pscalefsys=PSCALE_FSYS;
//***********************************
int main(void)
{
    avr_init();
    
	flash_led(5);
    while(1)
    {
        // Tasks here.
		LED_PORT &= ~_BV(LED);
		get_message();
		LED_PORT |= _BV(LED);
		switch(rx_buf[5])
		{
			case CMD_SIGN_ON:
				cmdSignOn();
			break;	
			
			case CMD_GET_PARAMETER:
        		cmdGetParameter();
        	break;
			
			case CMD_SET_PARAMETER:
				cmdSetParameter();
			break;
			
			case CMD_ENTER_PROGMODE_ISP:
				cmdEnterProgmodeIsp();
			break;
			
			case CMD_LEAVE_PROGMODE_ISP:
				cmdLeaveProgmodeIsp();
			break;
			case CMD_READ_SIGNATURE_ISP:
				cmdReadSignatureIsp();
			break;
			
			case CMD_READ_OSCCAL_ISP:
				cmdreadosccalisp();
			break;
			
			case CMD_READ_FUSE_ISP:
				cmdreadfuseisp();
			break;
			
			case CMD_PROGRAM_FUSE_ISP:
				cmdprogramfuseisp();
			break;
			
			case CMD_READ_LOCK_ISP:
				cmdreadlockisp();
			break;
			
			case CMD_PROGRAM_LOCK_ISP:
				cmdprogramlockisp();
			break;
			
			case CMD_SPI_MULTI:
				cmdspimulti();
			break;
			
			case CMD_CHIP_ERASE_ISP:
				cmdchiperaseisp();
			break;
			
			case CMD_PROGRAM_FLASH_ISP:
				cmdprogramflashisp();
			break;
			
			case CMD_LOAD_ADDRESS:
				cmdloadaddress();
			break;
			
			case CMD_READ_FLASH_ISP:
				cmdreadflashisp();
			break;
			
			case CMD_PROGRAM_EEPROM_ISP:
				cmdprogrameepromisp();
			break;
			
			case CMD_READ_EEPROM_ISP:
				cmdreadeepromisp();
			break;
			
			case CMD_OSCCAL:
				cmdosccal();
			break;
			
		}
		sendResponse();
    }
    
    return(0);
}



static void avr_init(void)
{
    // Initialize device here.
    
	LED_DDR|=_BV(LED);										//set led out put
	os_ddr|=_BV(os);										//set os out put
	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	// USART Baud rate: 115200
	UCSRA=0x00;
	UCSRB=0x18;
	UCSRC=0x86;
	UBRRH=0x00;
	UBRRL=0x03;
	// SPI initialization
	// SPI Type: Master
	// SPI Clock Rate: 57.600 kHz
	// SPI Clock Phase: Cycle Half
	// SPI Clock Polarity: Low
	// SPI Data Order: MSB First
	
	SPCR=0x50;
	SPCR|=(0x3&SCK_DURATION_DEF);	
	SPSR=0x00;

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 7372.800 kHz
	// Mode: CTC top=OCR1A
	// OC1A output: Toggle
	// OC1B output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	TCCR1A=0x40;
	TCCR1B=0x08|PSCALE_FSYS;
	TCNT1H=0xFF;
	TCNT1L=0xFF;
	OCR1AH=0x00;
	OCR1AL=CMATCH_DEF;
	OCR1BH=0x00;
	OCR1BL=0x00;
    return;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void putch(char ch)
{
    while (!(UCSRA & _BV(UDRE)));
    UDR = ch;
}

char getch(void)
{
    while(!(UCSRA & _BV(RXC)));
    return UDR;
}


void flash_led(uint8_t count)
{
    uint32_t ll;
	uint8_t ii;
    if (count == 0) {
      count = 3;
    }
    
    for (ii = 0; ii < count; ++ii) {
	LED_PORT &= ~_BV(LED);
	for(ll = 0; ll < (F_CPU / 5); ++ll);
	LED_PORT |= _BV(LED);
	for(ll = 0; ll < (2 * F_CPU); ++ll);
    }	
}

void get_message(void)
{
		
		rx_ptr=0;
		rx_buf[rx_ptr]=getch();									//get MESSAGE_START
		rx_chm^=rx_buf[rx_ptr];
		rx_ptr++;
		
		//////////////////LED_PORT &= ~_BV(LED);										//turn led on
		
		rx_buf[rx_ptr]=getch();									//get SEQUENCE_NUMBER
		rx_chm^=rx_buf[rx_ptr];
		rx_ptr++;

		rx_buf[rx_ptr]=getch();									//get MESSAGE_SIZE[1]
		rx_chm^=rx_buf[rx_ptr];
		rx_ptr++;

		rx_buf[rx_ptr]=getch();									//get MESSAGE_SIZE[0]
		rx_chm^=rx_buf[rx_ptr];
		rx_ptr++;
		
		rx_buf[rx_ptr]=getch();									//get TOKEN
		rx_chm^=rx_buf[rx_ptr];
		rx_ptr++;

		get_message_body();												//get message body
		
		rx_buf[rx_ptr]=getch();									//get CHECKSUM
		rx_chm^=rx_buf[rx_ptr];
		
		if(rx_chm==0)													//message recived Ok!
		{
			rx_chm=0;
			rx_ptr=0;
			//go to start
		}
	////////////////////////////LED_PORT |= _BV(LED);									//turn led off
	
	
}
void get_message_body(void)
{
	int j,k;
	j=rx_buf[2]*256+rx_buf[3];								//calculate message size
	
	for(k=0;k<j;k++)
	{
		rx_buf[rx_ptr]=getch();
		rx_chm^=rx_buf[rx_ptr];
		rx_ptr++;
	}
	
}


void sendResponse(void)
{
	int j,k;
	tx_chm=0;
		
	tx_buf[0]=MESSAGE_START;								//MESSAGE_START
		
	tx_buf[1]=rx_buf[1];									//SEQUENCE_NUMBER
		
	tx_buf[4]=TOKEN;										//token
	
	for(j=0;j<5;j++)
	{	
		tx_chm^=tx_buf[j];
		putch(tx_buf[j]);
	}
	
	j=tx_buf[messagesize1]*256+tx_buf[messagesize0];
	for(k=0;k<j;k++)
	{
		tx_chm^=tx_buf[k+5];								//start from message body
		putch(tx_buf[k+5]);
	}
	
	putch(tx_chm);

}

void cmdSignOn(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=11;
	
	tx_buf[5]= CMD_SIGN_ON;
	tx_buf[6]= STATUS_CMD_OK;
	
	tx_buf[7]= 0x08;  	// send signature length
	tx_buf[8]= 'S';		// send identifier
	tx_buf[9] = 'T';
	tx_buf[10] = 'K';
	tx_buf[11] = '5';
	tx_buf[12] = '0';
	tx_buf[13] = '0';
	tx_buf[14] = '_';
	tx_buf[15] = '2';

}
void cmdGetParameter(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=3;
	
	tx_buf[5] = CMD_GET_PARAMETER;
	tx_buf[6] = STATUS_CMD_OK;

	switch( rx_buf[6] )  {
		case PARAM_HW_VER: 
            tx_buf[7] = HARDWARE_VERSION;  // send hardware version          				
			break;
		case PARAM_SW_MAJOR: 
            tx_buf[7] = SOFTWARE_MAJOR; // send software major version         				
			break;        			
		case PARAM_SW_MINOR: 
            tx_buf[7] = SOFTWARE_MINOR;  // send software minor version          				
			break;			
		case PARAM_VTARGET: 
            tx_buf[7] = VTARGET; // target supply voltage         				
			break;        			
		case PARAM_VADJUST: 
            tx_buf[7] = VADJUST; // target VREF voltage          				
			break;  
		case PARAM_OSC_PSCALE: 
            tx_buf[7] = pscalefsys; // oscilator prescaler value         				
			break;
		case PARAM_OSC_CMATCH: 
            tx_buf[7] = cmatchdef; // oscilator compare value         				
			break;			
		case PARAM_SCK_DURATION: 
            tx_buf[7] = sckdurationdef; // oscilator compare value         				
			break;        			       			        			        			

		case PARAM_TOPCARD_DETECT: 
            tx_buf[7] =  STK501; // STK501 is expected          				
			break;		

		default: 	
            tx_buf[7] = 0x00; // send dummy value for not supported parameters   
        break;      			
	}			
}
void cmdSetParameter(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;
	
	SetParameter();											//set selected parameter
	
	tx_buf[5] = CMD_SET_PARAMETER;
	tx_buf[6] = STATUS_CMD_OK; 

}
void cmdReadSignatureIsp(void) 
{

	tx_buf[5] = CMD_READ_SIGNATURE_ISP;
	ReadfusebitIsp();

}

void cmdEnterProgmodeIsp(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;
	
	EnterProgmodeIsp();
}

void cmdLeaveProgmodeIsp(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;
	
	DDRB&=31;												//leave spi pins High ampedance
	reset_DDR&=~_BV(reset);
	PORTB&=31;												//leave spi pins High ampedance
	reset_PORT&=~_BV(reset);
	
	tx_buf[5] = CMD_LEAVE_PROGMODE_ISP;
	tx_buf[6] = STATUS_CMD_OK;
	
}

void cmdreadosccalisp(void)
{
	tx_buf[5] = CMD_READ_OSCCAL_ISP;						//read osccal byte
	ReadfusebitIsp();
}
void cmdreadlockisp(void)
{
	tx_buf[5] = CMD_READ_LOCK_ISP;							//read lock bits
	ReadfusebitIsp();
}
void cmdreadfuseisp(void)
{
	tx_buf[5] = CMD_READ_FUSE_ISP;							//read fuse bits
	ReadfusebitIsp();
}
void cmdprogramlockisp(void)
{
	programfuseisp();										//program lock bits
	tx_buf[5] =CMD_PROGRAM_LOCK_ISP;
}
void cmdprogramfuseisp(void)
{
	programfuseisp();
	tx_buf[5] = CMD_PROGRAM_FUSE_ISP;						//program fuse bits
}
void cmdspimulti(void)
{
	int j;
	tx_buf[messagesize1]=rx_buf[7]/256;  		// set message length
	tx_buf[messagesize0]=rx_buf[7]+3;

	
	tx_buf[5] = CMD_SPI_MULTI;
	tx_buf[6] = STATUS_CMD_OK;
	for(j=0;j<rx_buf[7];j++)
	{
		tx_buf[j+7]=0x22;		//data send back
		
	}
	
	tx_buf[rx_buf[7]+7] = STATUS_CMD_OK;
	
}
void cmdchiperaseisp(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;

	tx_buf[5] = CMD_CHIP_ERASE_ISP;
	tx_buf[6] = STATUS_CMD_OK;
	
	spi(rx_buf[8]);											//erase dvice
	spi(rx_buf[9]);
	spi(rx_buf[10]);
	spi(rx_buf[11]);
	
	delay_ms(rx_buf[6]);
	
}
void cmdprogramflashisp(void)
{
	int a,b;
	long add;
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;

	tx_buf[5] = CMD_PROGRAM_FLASH_ISP;
	tx_buf[6] = STATUS_CMD_OK;

	if(extended_address)									//flash with large memory
	{
		spi(0x4d);
		spi(0);
		spi(address_flash>>16);
		spi(0);
	}
	
	b=rx_buf[6]*256+rx_buf[7];
	add=address_flash;
	for(a=0;a<b;a+=2)
	{
		spi(rx_buf[10]);									//load Program Memory
		spi(address_flash>>8);
		spi(address_flash);
		spi(rx_buf[a+15]);

		spi(rx_buf[10]|8);									//load Program Memory
		spi(0);
		spi(address_flash);
		spi(rx_buf[a+16]);

		address_flash++;
	}

	if(rx_buf[8]&0x80)
		{
			spi(rx_buf[11]);									//write program memory
			spi(add>>8);
			spi(add);
			spi(0);
		
			delay_ms(rx_buf[9]);
		}
}
void cmdloadaddress(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;

    address_eeprom = rx_buf[8]*256+rx_buf[9];				//set eeprom address
    address_flash  = (rx_buf[7]*65536+rx_buf[8]*256+rx_buf[9])*2;			//set flash word address
	if(rx_buf[6]&0x80)
		extended_address=1;									//bit 31 is true so extended read should executed
	
	tx_buf[5] = CMD_LOAD_ADDRESS;
	tx_buf[6] = STATUS_CMD_OK;
	
}
void cmdreadflashisp(void)
{
	int j,k;
	k=rx_buf[6]*256+rx_buf[7];
	
	j=k+3;	
	tx_buf[messagesize1]=(unsigned char) (j >> 8);  		// set message length
	tx_buf[messagesize0]=(unsigned char) (j & 0xFF);

	tx_buf[5] = CMD_READ_FLASH_ISP;
	tx_buf[6] = STATUS_CMD_OK;
	
	if(extended_address)									//flash with large memory
	{
		spi(0x4d);
		spi(0);
		spi(address_flash>>16);
		spi(0);
	}

	
	for(j=0;j<k;j+=2)
	{
		spi(rx_buf[8]);
		spi(address_flash>>8);
		spi(address_flash);
		tx_buf[j+7]=spi(0xff);
	
		spi(rx_buf[8]|8);
		spi(address_flash>>8);
		spi(address_flash);
		tx_buf[j+8]=spi(0xff);
		
		address_flash++;
	}
	
	tx_buf[k+7] = STATUS_CMD_OK;
}

void cmdprogrameepromisp(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;

	tx_buf[5] = CMD_PROGRAM_EEPROM_ISP;
	tx_buf[6] = STATUS_CMD_OK;

	int a,b;
	unsigned char c,d;
	
	b=rx_buf[6]*256+rx_buf[7];								//eeprom nData bytes

	for(a=0;a<b;a++)
	{
		spi(0xa0);									//read eeprom if differ then write
		spi(address_eeprom>>8);
		spi(address_eeprom);
		c=spi(0xff);
		
		d=rx_buf[a+15]^c;
		if(d)
		{
			spi(rx_buf[10]);									//write Program Memory
			spi(address_eeprom>>8);
			spi(address_eeprom);
			spi(rx_buf[a+15]);

			delay_ms(rx_buf[9]);
		}
		address_eeprom++;
	}

}

void cmdreadeepromisp(void)
{
	int j,k;
	k=rx_buf[6]*256+rx_buf[7];
	
	j=k+3;	
	tx_buf[messagesize1]=(unsigned char) (j >> 8);  		// set message length
	tx_buf[messagesize0]=(unsigned char) (j & 0xFF);

	tx_buf[5] = CMD_READ_EEPROM_ISP;
	tx_buf[6] = STATUS_CMD_OK;
	
	k=rx_buf[6]*256+rx_buf[7];
	for(j=0;j<k;j++)
	{
		spi(rx_buf[8]);
		spi(address_eeprom>>8);
		spi(address_eeprom);
		tx_buf[j+7]=spi(0xff);

		address_eeprom++;
	}
	tx_buf[k+7] = STATUS_CMD_OK;
}

void cmdosccal(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=2;

	tx_buf[5] = CMD_OSCCAL;
	tx_buf[6] = STATUS_CMD_OK;
}
void delay_ms(int time)
{
	while(time--)
		_delay_ms(1);
}

char spi(char data)
{

	SPDR = data;
	/* Wait for transmission complete */
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	/* Return data register */
	return SPDR;
}

void EnterProgmodeIsp(void)
{
	char tmp;
	
	
	tx_buf[5] = CMD_ENTER_PROGMODE_ISP;
	
	
	DDRB|=0xB0;												//set spi pins out put
	reset_DDR|=_BV(reset);

	sck_port &=~_BV(sck);								//clear sck
	
	delay_ms(rx_buf[7]+rx_buf[8]);	
	if(rx_buf[11]==0x53)										//this is avr set reset
	{
		
		reset_PORT &= ~_BV(reset);									//reset 0
		
		delay_ms(rx_buf[8]);
		delay_ms(10);
	
		spi(rx_buf[13]);
		delay_ms(rx_buf[10]);
		spi(rx_buf[14]);
		delay_ms(rx_buf[10]);
		tmp=spi(rx_buf[15]);
		delay_ms(rx_buf[10]);
		spi(rx_buf[16]);
		
		if(tmp==0x53)
		{
			tx_buf[6] = STATUS_CMD_OK; 
		}
			
		else tx_buf[6] = STATUS_CMD_FAILED; 
	}
	
	if(rx_buf[11]==0x69)										//this is 89s51 set reset
	{
		reset_PORT |= _BV(reset);									//reset 1
		delay_ms(rx_buf[8]);
		delay_ms(10);
		
		
		spi(rx_buf[13]);
		delay_ms(rx_buf[10]);
		spi(rx_buf[14]);
		delay_ms(rx_buf[10]);
		spi(rx_buf[15]);
		delay_ms(rx_buf[10]);
		tmp=spi(rx_buf[16]);
		
		if(tmp==0x69)
		{
			tx_buf[6] = STATUS_CMD_OK; 
		}
		
		else tx_buf[6] = STATUS_CMD_FAILED; 
	}
}
void ReadfusebitIsp(void)
{
	char k[4];
	
	
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=4;
	
	tx_buf[6] = STATUS_CMD_OK;
	
	
	
	k[0]=spi(rx_buf[7]);
	k[1]=spi(rx_buf[8]);
	k[2]=spi(rx_buf[9]);
	k[3]=spi(rx_buf[10]);
	
	tx_buf[7]=k[rx_buf[6]-1];
	tx_buf[8] = STATUS_CMD_OK;	

}

void programfuseisp(void)
{
	tx_buf[messagesize1]=0;  		// set message length
	tx_buf[messagesize0]=3;

	spi(rx_buf[6]);
	spi(rx_buf[7]);
	spi(rx_buf[8]);
	spi(rx_buf[9]);

	tx_buf[6] = STATUS_CMD_OK;
	tx_buf[7] = STATUS_CMD_OK;

}

void SetParameter(void)
{
	switch(rx_buf[6])
	{
		case PARAM_OSC_PSCALE:
		{
			TCCR1B=0x08|(0x03&rx_buf[7]);
			pscalefsys=rx_buf[7];							// Set timer operation mode and prescaler					
		}
		break;
		
		case PARAM_OSC_CMATCH:
		{
			OCR1AL=rx_buf[7];
			cmatchdef=rx_buf[7];							//Set compare match value
		}
		break;
		
		case PARAM_SCK_DURATION:
		{
			SPCR=0x50;
			SPCR|=(0x3&rx_buf[7]);								//set spi clock
			sckdurationdef=(0x3&rx_buf[7]);
		}
		break;
	}
}
