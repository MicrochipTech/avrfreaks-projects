//
// File: errorhandlers.h
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
// Error codes for the demo application
//
#ifndef _ERRORHANDLERS_H_
#define _ERRORHANDLERS_H_

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
#define ERR_LOW_HEAP_SPACE            52 // not enought rtos heap available
#define ERR_BYTE_LOST_ON_SEND		  53
#define ERR_BYTE_LOST_ON_RECEIVE      54
#define ERR_STRING_OVERFLOW           55
#define ERR_INVALID_STRING_ADDRESS    56
#define ERR_CHANNEL_QUEUE_FULL        57
#define ERR_HEAP_TOO_LARGE			  58 // decrease configTOTAL_HEAP_SIZE = decrease rtos heap !
#define ERR_LOW_GLOBAL_STACK_SPACE    59 

// error reporting function
//
void errorNonFatal(u8 errCode);
void error(u8 errCode);
void checkAllStacks(void);
void software_reset(void);

#endif // _ERRORHANDLERS_H_
