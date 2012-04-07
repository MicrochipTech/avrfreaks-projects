/*
 * UART1.h
 *
 * Created: 22/12/2011 16:29:51
 *  Author: Giacky98
 */ 

#ifndef UART1_H_
#define UART1_H_
	
	#define F_CPU 16000000
	
	/* UART Baud Rate Register */
	#define UBRR _SFR_IO8(0x09)
	
	/* UART Control Register */
	#define UCR	_SFR_IO8(0x0A)
	
	/* UART Status Register */
	#define USR	_SFR_IO8(0x0B)
	
	/* UART I/O Data Register */
	#define UDR	_SFR_IO8(0x0C)
	
	/* Macros */
	#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 8UL)) / ((uint32_t)(baudRate) * 16UL) - 1)
	
	/* Prototypes */
	void InitUART(int baudrate);
	
	// Basic
	unsigned char ReceiveByte(void);
	void TransmitByte(unsigned char data);
	
	// Advanced
	void SPrint(char *data);
	void SPrintN(long n);

#endif /* UART1_H_ */