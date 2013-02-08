/*
 * ds1620.h
 *
 * Created: 12/23/2012 9:14:00 AM
 *  Author: sasquach
 */ 


#ifndef DS1620_H_
#define DS1620_H_

#define ds1620_DDR		DDRC
#define ds1620_PORT		PORTC
#define ds1620_PIN		PINC
#define ds1620_clk		PC4
#define ds1620_data		PC6
#define ds1620_rst		PC7
#define ds1620_clk_delay 5




#endif /* DS1620_H_ */


extern uint16_t ds1620_temp;


void ds1620_read (void);
void ds1620_init(void);