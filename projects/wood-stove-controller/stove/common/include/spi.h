/*
 * spi.h
 *
 * Created: 1/1/2013 4:23:47 PM
 *  Author: sasquach
 */ 


#ifndef SPI_H_
#define SPI_H_

#define SPI_SLAVE_SELECT_PORT PORTB
#define SPI_SLAVE_SELECT_DDR DDRB
#define SPI_SLAVE_SELECT_SS PB4
#define SPI_SLAVE_SELECT_PIN PINB
#define SPI_MASTER_DELAY_SHORT 10
#define SPI_MASTER_DELAY_LONG 100
#define SPI_SLAVE_DELAY (SPI_MASTER_DELAY_SHORT/2)
#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define DD_MOSI PB5
#define DD_MISO PB6
#define DD_SCK  PB7
#define SPI_DATA_BUFFER_SIZE 16
#define SPI_DATA_BUFFER_MASK 0x0f

#define SPI_LOOPBACK					1
#define SPI_ADC_VALUE1					2
#define SPI_ADC_VALUE2					3
#define SPI_STOVE_MODE					4
#define SPI_HIGH_STEPPER_POS			5
#define SPI_LOW_STEPPER_POS				6
#define SPI_HIGH_STEPPER_POS_CURRENT	7
#define SPI_LOW_STEPPER_POS_CURRENT		8
#define SPI_stove_target_temp			9
#define SPI_stove_target_temp_lockout	10
#define SPI_HIGH_stove_run_change_count	11
#define SPI_LOW_stove_run_change_count	12
#define SPI_HIGH_stove_cooldown_count	13
#define SPI_LOW_stove_cooldown_count	14
#define SPI_STEPPER_DISPLAY             15
#define SPI_ds1620_temp					2
#define SPI_stove_mode_target_temp_var  3
#define SPI_basement_control			4

extern uint8_t SPI_DATA_MISO[SPI_DATA_BUFFER_SIZE];
extern uint8_t SPI_DATA_MOSI[SPI_DATA_BUFFER_SIZE];


#endif /* SPI_H_ */

void spi_master_init(void);
void spi_slave_init(void);
void spi_master(void);
void spi_slave(void);
