#ifndef _CONFIG_H
#define _CONFIG_H

/******************************************************************************
 * Configuration for ZG2100 module as defined in config.h file from asynclabs
 ******************************************************************************/
 #include <avr/pgmspace.h>
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "net.h"

/******************************************************************************
 * Configuration for ZG2100 module as defined in config.h file from asynclabs
 *
 * These values are set within the "wlan.c" source file
 *
 ******************************************************************************/
 extern const prog_char ssid[];
 extern unsigned char ssid_len;

 extern const prog_char security_passphrase[];
 extern unsigned char security_passphrase_len;
 extern unsigned char security_type;
 extern unsigned char wireless_mode;

 extern prog_uchar wep_keys[];

 #define NETWORK_BUFSIZE     400
 extern  unsigned char network_buffer[NETWORK_BUFSIZE+2];


/******************************************************************************
 * SPI setup for ZG2100 module originating from asynclabs file "spi.h" 
 ******************************************************************************/
 #define ZG2100_ISR_DISABLE()	( GICR  &= ~(1<<INT0)  );

 #define ZG2100_ISR_ENABLE()	( MCUCR |=  (1<<ISC01) );\
                              ( MCUCR &= ~(1<<ISC00) );\
                              ( GICR  |=  (1<<INT0)  );


 #define SPI0_SS_BIT				0x04
 #define SPI0_SS_DDR				DDRB
 #define SPI0_SS_PORT			PORTB

 #define SPI0_SCLK_BIT			0x20
 #define SPI0_SCLK_DDR			DDRB
 #define SPI0_SCLK_PORT			PORTB

 #define SPI0_MOSI_BIT			0x08
 #define SPI0_MOSI_DDR			DDRB
 #define SPI0_MOSI_PORT			PORTB

 #define SPI0_MISO_BIT			0x10
 #define SPI0_MISO_DDR			DDRB
 #define SPI0_MISO_PORT			PORTB


 #define SPI0_WaitForReceive()
 #define SPI0_RxData()	 		(SPDR)

 #define SPI0_TxData(Data)		(SPDR = Data)
 #define SPI0_WaitForSend()	while( (SPSR & 0x80)==0x00 )

 #define SPI0_SendByte(Data)	SPI0_TxData(Data);SPI0_WaitForSend()
 #define SPI0_RecvBute()		SPI0_RxData()

 #define SPI0_Init()				DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT|LEDConn_BIT;\
										DDRB  &= ~(SPI0_MISO_BIT);\
										PORTB = SPI0_SS_BIT;\
										SPCR  = 0x50;\
										SPSR  = 0x01

 #define ZG2100_SpiInit			SPI0_Init
 #define ZG2100_SpiSendData	SPI0_SendByte
 #define ZG2100_SpiRecvData	SPI0_RxData

 #define ZG2100_CS_BIT			SPI0_SS_BIT
 #define ZG2100_CS_DDR			SPI0_SS_DDR
 #define ZG2100_CS_PORT			SPI0_SS_PORT 

 #define ZG2100_CSInit()		(ZG2100_CS_DDR  |= ZG2100_CS_BIT)
 #define ZG2100_CSon()			(ZG2100_CS_PORT |= ZG2100_CS_BIT)
 #define ZG2100_CSoff()			(ZG2100_CS_PORT &= ~ZG2100_CS_BIT)

 #define LEDConn_BIT				0x02
 #define LEDConn_DDR				DDRB
 #define LEDConn_PORT			PORTB

 #define LEDConn_on()	      (LEDConn_PORT |= LEDConn_BIT)
 #define LEDConn_off()	      (LEDConn_PORT &= ~LEDConn_BIT)

/******************************************************************************
 * UART baudrate
 ******************************************************************************/	
 #define BAUDRATE       38400

/******************************************************************************
 * Logging
 ******************************************************************************/	
 #define LOGGING         1

/******************************************************************************
 * Defines for LEDs
 ******************************************************************************/	
 #define LED_OUT         PORTD
 #define LED_DDR         DDRD
 #define RED             PD5
 #define GREEN           PD6
 #define BLUE            PD7

/******************************************************************************
 * Defines for network
 ******************************************************************************/	
 #define AMBICOLOR_PORT  MyPort


#endif 
