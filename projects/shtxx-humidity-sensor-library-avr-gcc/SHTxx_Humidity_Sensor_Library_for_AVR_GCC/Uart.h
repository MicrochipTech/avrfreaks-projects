#ifndef __uart_h
#define __uart_h

#define XTAL				8000000
#define UART_BAUD			9600
#define UART_BAUD_SELECT 	(XTAL/(UART_BAUD*16l)-1)

#define UART_RX_BUFFER_SIZE	50
#define UART_TX_BUFFER_SIZE	50


extern unsigned int RxOverflow;
extern unsigned int TxOverflow;


extern void UartInit(void);
extern void UartClearBuffers(void);

extern unsigned char IsTxSpaceAvailable(void);
extern unsigned char IsCharAvailable(void);
extern unsigned char IsLineAvailable(void);
extern unsigned char NumRxCharAvailable(void);

extern unsigned char RxChar(void);
extern unsigned char RxLine(char *, unsigned char);

extern void TxChar(unsigned char);
extern void TxString(unsigned char[]);
extern void TxLine(unsigned char[]);
extern void TxUByte(unsigned char, char);
extern void TxUInt(unsigned int, char);
extern void TxULong(unsigned long, char);
extern void TxFloat(float, unsigned char, unsigned char, unsigned char);

#endif

