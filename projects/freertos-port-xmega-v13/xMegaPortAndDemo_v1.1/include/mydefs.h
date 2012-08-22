
// file: mydefs.h

#ifndef _mydefs_h_
#define _mydefs_h_


#define SERIAL_SIMULATION


#define SERIAL_PORT_BUFFER_LEN	 82  // each serial port has a buffer queue
#define CHANNEL_QUEUE_LENGTH	164  // each channel has output queue

#define TRANSMITTER_INTERRUPT_LEVEL 	USART_DREINTLVL_HI_gc  // serial transmitter
#define RECEIVER_INTERRUPT_LEVEL 		USART_DREINTLVL_MED_gc // serial receiver


// for easier type writing:

typedef unsigned char  u8;
typedef   signed char  s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;

// error code defs
//
#define ERR_QUEUE_CREATE_HANDLE_NULL  42
#define ERR_STACK_OVERFLOW            43
#define ERR_QUEUE_SEND_FAILED         44
#define ERR_QUEUE_RECEIVE_FAILED      45
#define ERR_INVALID_CHANNELINDEX      46
#define ERR_BYTES_SHOULD_BE_AVAILABLE 47
#define ERR_BAD_ISR					  48
#define ERR_DISPATCH_QUEUE_FULL       49
#define ERR_LOW_STACK_SPACE           50
#define ERR_INTERRUPT_YIELD           51


// error reporting function
//
void error(u8 errCode, xTaskHandle *pxTask);

#endif // _mydefs_h_ 
