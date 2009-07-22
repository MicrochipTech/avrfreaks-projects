#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#ifndef F_CPU  
#define F_CPU 32000000
#endif
 
#include "clksys_driver.h"
#include "usart.h"
#include "port_driver.h"
#include "wiznetw5100.h"
#include "spi_xmega.h"
#include "wdt_driver.h"
 



#define sbi(port,pinno) port.OUTSET=1<<pinno
#define cbi(port,pinno) port.OUTCLR=1<<pinno 
 

uint8_t STATUS = 1;

static void clock_init(void)
{   
    CLKSYS_Enable( OSC_RC32MEN_bm );
	CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
	do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
	CLKSYS_Disable(OSC_RC2MCREF_bm);
	CLKSYS_AutoCalibration_Enable(OSC_RC32MCREF_bm,1);
}

static void UsartC_init(void)
{
	usart_init(&PORTC, &USARTC0, 256000);
}

static void IoInit()
{
/* 		
	Ports set as output by other functions

	PORTC.DIRSET = PIN3_bm; // set under UsartC_Init() TXC0

	PORTH.DIRSET = PIN7_bm; // set under ETH_Init() W_SPIEN

	PORTD.DIRSET = PIN4_bm; // set under ETH_Init() !SS SPID_Init() 

	PORTD.DIRSET = PIN5_bm; // set under ETH_Init() MOSI SPID_Init() 

	PORTD.DIRSET = PIN7_bm; // set under ETH_Init() SCLK SPID_Init()  
	
	// status led
	PORTA.DIRSET=PIN1_bm;
	PORTA.OUTSET=PIN1_bm;	// at the end of init

*/


	PORTJ.DIRSET = 0xFF;	// DA Array
	PORTK.DIRSET = 0xFF;	// DA Array



//SS_DFL, same SPID as Wiznet
	PORTQ.DIRSET = PIN3_bm;
	PORTQ.OUTSET = PIN3_bm;

// Enable SPI on Wiznet W5100
	PORTH.DIRSET=PIN7_bm;  
 	PORTH.OUTSET=PIN7_bm;

// Enable power to Wiznet W5100
	PORTA.DIRSET=PIN0_bm;  
 	PORTA.OUTSET=PIN0_bm;

// reset w5100
	PORTF.DIRSET=PIN6_bm; // pin5 on revB
	_delay_us(2);
	PORTF.OUTSET=PIN6_bm;





	/* Configure PA2 as input, triggered on falling edge. */
	PORT_ConfigurePins( &PORTA,
	                    PIN2_bm,
	                    false,
	                    false,
	                    PORT_OPC_TOTEM_gc,
	                    PORT_ISC_FALLING_gc );

	PORT_SetPinsAsInput( &PORTA, PIN2_bm );

 
	/* Configure Interrupt0 to have low interrupt level, triggered by pin 0. */
	PORT_ConfigureInterrupt0( &PORTA, PORT_INT0LVL_MED_gc, PIN2_bm );

	/* Enable low level interrupts in the PMIC. */
	//PMIC.CTRL |= PMIC_LOLVLEN_bm;

}

static void ETH_Init(void)
{     
    //Enabling SPI on wiznet chip
    PORTH.DIRSET = PIN7_bm;
    PORTH.OUTSET = PIN7_bm;
    // setting up SPI
    SPID_Init();

    printf("Reseting iinchip\n");
    _delay_ms(100); // as done in dns example from wiznet

	iinchip_init(); // reset iinchip. This might not be necessary for SPI communication

    
	uint8_t ip[6]; //6 because it is also used for the mac address
	printf("Setting mac, subnet and gateway\n");
	// MAC address
	ip[0] = 0x00; ip[1] = 0x08; ip[2] = 0xDC; ip[3] = 0x00; ip[4] = 0x00; ip[5] = 0x4F;
	setMacAddress( ip );

	// subnet mask
	ip[0] = 255;ip[1] = 255;ip[2] = 255;ip[3] = 0;
	setSubnetMask( ip );

	// gateway address
	ip[0] =192 ;ip[1] = 168;ip[2] = 0;ip[3] = 1;
	setGatewayAddress( ip );

	// ip address
	ip[0] = 192;ip[1] = 168;ip[2] = 0;ip[3] = 100;
	printf("Setting IP address %i.%i.%i.%i \n",ip[0], ip[1], ip[2], ip[3]);
	setIPAddress( ip );

    //printf("Setting mask\n");
    //setIMR( 0xEF); // set interrupt mask == do we generate interrupt for IP conflict, Dest unreachable, ...
    //
	
	printf("sysinit\n");
	sysinit(0x11, 0x11); // setting tx/rx buf size 0x11 is 8 kbytes ans is he maximum for the chip

}

static void UDP_Init(void) 
{
	uint16_t port = 1776;
    SOCKET sock = 0x01;
	setIMR(0b00000010); // Enable interrupt on socket 1
	printf("Opening socket on port : %d \n", port);
 
    while ( socket(sock,  Sn_MR_UDP, port , 0x00) != 1 ) { // create socket.
        printf("socket creation failed");
        _delay_ms(1000);
        } 
}


static void init_status_tick(void){
	/* Set up Timer/Counter E0 with OVF interrupt. */
	TCD1.INTCTRLA = (TCD1.INTCTRLA & ~TC1_OVFINTLVL_gm) | TC_OVFINTLVL_LO_gc;	//enable overflow interrupt: OFF test
 	
	switch (STATUS)
	{
		case 1:	TCD1.PER =31250; break;
		case 2: TCD1.PER =31250/2; break;
		case 3: TCD1.PER =31250/10; break;
 		default:TCD1.PER =31250; break;
	}		
	
	//TCD1.PER =31250;  
 	TCD1.CTRLA = TC_CLKSEL_DIV1024_gc; // 1 Hz
}


///////////////////		START PRINTF

#include <stdio.h>

static int uart_putchar(char c, FILE *stream)
{
   if (c == '\n') uart_putchar('\r', stream);
   usart_send_byte(c);
   return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

///////////////////		END PRINTF

//--------------------------------------------- INTERRUPTS ---------------------------------------
 
ISR(PORTA_INT0_vect) // Interrupt from W5100 
{
	 PacketInBuf = 1;
	 //PORTF.OUTSET=PIN1_bm; // test com time
}

ISR(USARTC0_RXC_vect) //Reception Complete interrupt
{
    uint8_t Temp;
    Temp = USARTC0.DATA ; 
    usart_rx_buf_put_byte(Temp);
    Temp++;
}

ISR(TCD1_OVF_vect) // Default 1 Hz
{
 	PORTA.OUTTGL=PIN1_bm;
	WDT_Reset();
}

 
//-------------------------------------------- END INTERRUPTS -------------------------------------
void Read_Packet_eth(void)
{
	SOCKET sock = 0x01;
 	packetSize= getSn_RX_RSR(sock); // get received size
	DataSize = recvfrom(sock,packetBuffer,packetSize,remoteIp,&remotePort);
 	PacketInBuf = 0;
	Packet_Pointer = DataSize; 
} 

uint8_t Next_Byte_eth(void)
{
	uint8_t newbyte;
	newbyte=packetBuffer[DataSize-Packet_Pointer];
	Packet_Pointer--;
			
	if (Packet_Pointer == 0)
	{
			IINCHIP_WRITE(Sn_IR(1), 0xff);
			//PacketRespTo = 1;
	}
	
	return(newbyte);
}

void acknowledge_usa(void)
{
	usart_send_byte('a');
}

void error_usa(void)
{
	usart_send_byte('e');
}

void acknowledge_eth(void)
{
	SOCKET sock = 0x01;
	RespLen = 1;
	Response[0] = 'a';
	sendto(sock,Response,RespLen,remoteIp,remotePort);
}

void error_eth(void)
{
	SOCKET sock = 0x01;
	RespLen = 1;
	Response[0] = 'e';
	sendto(sock,Response,RespLen,remoteIp,remotePort);
}


int main(void)
{

	clock_init();
	UsartC_init();

	stdout = &mystdout; //printf 
	usart_rx_buf_flush(); //remove bad data we got at startup
    
	printf("\n******* Booting up Xmegaboard ********\n");
	
	IoInit();
	
	ETH_Init();
	
	UDP_Init(); 

	WDT_EnableAndSetTimeout( WDT_PER_2KCLK_gc );
	
	PMIC.CTRL |= PMIC_LOLVLEN_bm; //Enable low level Interrupts 
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;//Enable Medium level Interrupts
	PMIC.CTRL |= PMIC_HILVLEN_bm; //Enable High level Interrupts
	sei();

		// status led
	PORTA.DIRSET=PIN1_bm;
	//PORTA.OUTSET=PIN1_bm;
	init_status_tick();

 
	while(1)
	{
		if (PacketInBuf == 1 && (0x04 & getSn_IR(0x01)))		// falling edge and bit Res (bit 2) in Socket 01 set.
		{	
			Read_Packet_eth();
		}		
//////////////// Respond to command from UDP on port 1776		

		if (Packet_Pointer > 0)
		{
 			
		
		switch (Next_Byte_eth())

			{
/************************************* Test Com ******************************************************/
				case 'A':
				case 'a':
					acknowledge_eth();
					break;
/************************************* Turn off power to eth chip W5100 *****************************/
				case 'w':
					acknowledge_eth();
					PORTA.OUTCLR=PIN0_bm;
					break;
 /**************************************** I/O Connected to Darlington Array's ***********************************/
				case 'J':
				case 'j':
					if (Packet_Pointer > 0)
					{
						PORTJ.OUT = Next_Byte_eth();
				 		acknowledge_eth();
					}
					else	
					{
						error_eth();
					}
					break;
				case 'K':
				case 'k':
					if (Packet_Pointer > 0)
					{
						PORTK.OUT = Next_Byte_eth();
				 		acknowledge_eth();
					}
					else	
					{
						error_eth();
					}
					break;
 				default:
				error_eth(); 
			}
		}	
			 
		
//////////////// Respond to command from USART
		
		if ( usart_rx_buf_has_data() ) 
		{
			uint8_t newbyte;
			cli();
			newbyte = usart_rx_buf_get_byte();
			sei();
 
			
			switch (newbyte)

			{
				case 'A':
				case 'a':
					acknowledge_usa();
					break;
				case 'W':
					PORTA.OUTSET=PIN0_bm;
					acknowledge_usa();
					_delay_ms(2000);
					acknowledge_eth();
				 	break;
				case 'R':
				 	acknowledge_usa();
				 	cli();
					_delay_ms(1500);	// Force reset by not resetting WDT on time
				 	break;
				default:
				error_eth();
 
			}
		}
	}

}



