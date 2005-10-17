/// uart.h ///





#define UART_TX_ENABLE 0x08
#define UART_RX_ENABLE 0x10
#define UART_RX_INT_ENABLE  0x80
#define UART_TX_INT_ENABLE 0x40
#define UART_DRE_INT_ENABLE 0c20
#define UART_2STOPBITS 0x08

#define UART_RX_COMPLETE 0x80
#define UART_TX_COMPLETE 0x40
#define UART_DR_EMPTY 0x20
#define UART_FRAME_ERROR 0x10
#define UART_OVERRUN 0x08
#define UART_MPCM 0x01

#define UART_TIMEOUT  5



volatile byte bufferstatus;
word transmit_counter,transmit_size;

char *ringpointer;
volatile byte uart_complete;
volatile word receive_counter;
byte  uart_receive_init (byte interrupt, byte data9bit,char *start,word buffersize);

byte  uart_transmit_init (byte interrupt,byte doublestop);
byte uart_transmit (byte *data,word size);



