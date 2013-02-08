/*
 * stepper.h
 *
 * Created: 12/27/2012 3:01:50 PM
 *  Author: sasquach
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_


#ifndef STEPPER_CONFIG_S_ // defines below should be copied to config.h if need to to be modified
// START COPY HERE
// STEPPER DEFINITIONS

#define STEPPER_CONFIG_S_
#define STEPPER_PORT PORTC
#define STEPPER_DDR  DDRC
#define STEPPER_STOP_PORT PORTA
#define STEPPER_STOP_DDR DDRA
#define STEPPER_STOP_PIN PINA
#define STEPPER_STOP PD7
#define STEPPER_SHIFT 0
#define STEPPER_SPEED 6
#define STEPPER_CYCLE1 ( ( uint8_t ) 0b1100 )
#define STEPPER_CYCLE2 ( ( uint8_t ) 0b1001 )
#define STEPPER_CYCLE3 ( ( uint8_t ) 0b0011 )
#define STEPPER_CYCLE4 ( ( uint8_t ) 0b0110 )
#define STEPPER_POS_START 0x4000
#define STEPPER_POS_CURRENT_START 0x0000
#define STEPPER_POS_STOP 0x2fff

// END COPY HERE

#endif /* STEPPER_CONFIG_S_ */

extern uint16_t STEPPER_POS;
extern uint16_t STEPPER_POS_CURRENT;
extern uint8_t STEPPER_DISPLAY;

void stepper(void);
void stepper_init(void);

#endif /* STEPPER_H_ */
