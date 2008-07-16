#ifndef _MCA25_H_
	#define _MCA25_H_
	
#include "../config.h"
#include "../util/string.h"
#include "../util/crc8.h"
#include "../net/mynic.h"
#include "clock.h"
#include "mpf10.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
	
#define MCA25_PIC_STATE_IDLE          0
#define MCA25_PIC_STATE_PREVIEW_DONE  1
#define MCA25_PIC_STATE_JPG_DATA      2
#define MCA25_PIC_STATE_LAST_DATA     3
#define MCA25_PIC_STATE_CAM_ERROR   128

#define MCA25_COMM_BUFFER_LEN 40

//reset control signal
#define MCA25_RESET_HI() MCA25_RESET_PORT|=(1<<MCA25_RESET_PIN);
#define MCA25_RESET_LO() MCA25_RESET_PORT&=~(1<<MCA25_RESET_PIN);

extern unsigned char mca25_pic_state;
extern unsigned char mca25_allocated_for_socket;
extern unsigned char mca25_allocated_timeout;
extern unsigned char mca25_buffer[MCA25_COMM_BUFFER_LEN];
extern unsigned char mca25_ready;
extern unsigned char mca25_mux_buffer[6];
extern unsigned char mca25_brightness;
extern unsigned long mca25_datapos;
extern unsigned char mca25_jpg_state;


void mca25_estimate_brightness(unsigned char val);
void mca25_send_data_ack(void);
void mca25_handle_channel_x23(void);
void mca25_handle_channel_x03(void);
void mca25_cleanup(void);
unsigned int mca25_grab_data(char *buffer, unsigned int datalen, unsigned long pos, unsigned char socketnum);
unsigned char mca25_start_image_grab(unsigned char socketnum);
unsigned char mca25_configure(void);
void mca25_init(void);
unsigned char mca25_init_cam(void);
void mca25_send_ok(void);
unsigned char mca25_read_at_command(unsigned char *buffer);
unsigned char mca25_read_mux_packet(unsigned char *buffer, unsigned char skipcount);

void mca25_puts_progmem2(PGM_P pointer, unsigned int len);
#define mca25_puts_progmem(s) mca25_puts_progmem2(PSTR((s)),sizeof((s))-1)

void mca25_uart_init_460800(void);
void mca25_uart_init_9600(void);
void mca25_uart_init_19200(void);
#define mca25_uart_putc(c) { while (!(UCSRA & (1<<UDRE))){}; UDR=(c); }
	
#endif
