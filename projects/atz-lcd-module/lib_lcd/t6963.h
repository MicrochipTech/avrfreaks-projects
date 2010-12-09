#ifndef __T6963_H__
#define __T6963_H__

#include <inttypes.h>

#define T6963_TEXT_HOME_ADDRESS			0x0000
#define T6963_GRAPHIC_HOME_ADDRESS		0x0200
#define T6963_EXTERNAL_CG_HOME_ADDRESS		0x1400

#define T6963_SET_CURSOR_POINTER       		0x21
#define T6963_SET_OFFSET_REGISTER      		0x22
#define T6963_SET_ADDRESS_POINTER      		0x24
#define T6963_SET_TEXT_HOME_ADDRESS    		0x40
#define T6963_SET_TEXT_AREA            		0x41
#define T6963_SET_GRAPHIC_HOME_ADDRESS 		0x42
#define T6963_SET_GRAPHIC_AREA         		0x43
#define T6963_OR_MODE			 	0x80
#define T6963_EXOR_MODE		 		0x81
#define T6963_AND_MODE		 		0x83
#define T6963_TEXT_ATTRIBUTE_MODE	 	0x84
#define T6963_INTERNAL_CG_ROM_MODE	 	0x80
#define T6963_EXTERNAL_CG_RAM_MODE	 	0x88
#define T6963_BLINK_OFF                		0x90
#define T6963_CURSOR_OFF               		0x90
#define T6963_TEXT_OFF                 		0x90
#define T6963_GRAPHIC_OFF              		0x90
#define T6963_BLINK_ON                 		0x91
#define T6963_CURSOR_ON                		0x92
#define T6963_TEXT_ON                  		0x94
#define T6963_GRAPHIC_ON               		0x98
#define T6963_SET_DATA_AUTO_WRITE	 	0xB0
#define T6963_SET_DATA_AUTO_READ	 	0xB1
#define T6963_AUTO_RESET	 	 	0xB2
#define T6963_DATA_WRITE_AND_INCREMENT_AP	0xC0
#define T6963_DATA_READ_AND_INCREMENT_AP	0xC1
#define T6963_DATA_WRITE_AND_DECREMENT_AP	0xC2
#define T6963_DATA_READ_AND_DECREMENT_AP	0xC3
#define T6963_DATA_WRITE		 	0xC4
#define T6963_DATA_READ		 		0xC5

#define T6963_STATUS_CHECK_COMMAND_DATA 	0x03
#define T6963_STATUS_CHECK_AUTO_READ    	0x04
#define T6963_STATUS_CHECK_AUTO_WRITE   	0x08

extern uint8_t	t6963_status_check_mask;

void	t6963_auto_reset(void);
void	t6963_data_write_and_increment_ap(uint8_t data);
void	t6963_erase_line(void);
void	t6963_home(void);
void	t6963_init(void);
uint8_t	t6963_read(void);
uint8_t t6963_receive_data(void);
void	t6963_scroll_up(void);
void	t6963_send(uint8_t data);
void	t6963_send_command(uint8_t command);
void	t6963_send_command_columns(uint8_t command);
void	t6963_send_command_uint16(uint8_t command, uint16_t data);
void	t6963_send_command_uint8(uint8_t command,uint8_t data);
void	t6963_send_command_uint8_nul(uint8_t command, uint8_t data);
void	t6963_send_command_uint8_uint8(uint8_t command,uint8_t data1,uint8_t data2);
void	t6963_send_data(uint8_t data);
void	t6963_set_address_pointer(uint16_t address);
void	t6963_set_cursor_pointer(uint8_t x_address,uint8_t y_address);
void	t6963_set_data_auto_read(void);
void	t6963_set_data_auto_write(void);
void	t6963_set_position(uint8_t x,uint8_t y);
uint8_t	t6963_status(void);
void	t6963_status_check(void);

#endif
