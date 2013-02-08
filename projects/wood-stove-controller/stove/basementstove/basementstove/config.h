/*
 * config.h ******************THIS IS FOR BASEMENT STOVE
 *
 * Created: 1/1/2013 3:17:43 PM
 *  Author: sasquach
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

// #define ds1620_temp (20 * 2)

// STEPPER DEFINITIONS

#define DOWN_STAIRS_
#define STEPPER_CONFIG_S_
#define STEPPER_PORT PORTC
#define STEPPER_DDR  DDRC
#define STEPPER_STOP_PORT PORTA
#define STEPPER_STOP_DDR DDRA
#define STEPPER_STOP_PIN PINA
#define STEPPER_STOP PD6
#define STEPPER_SHIFT 4
#define STEPPER_SPEED 6
#define STEPPER_CYCLE1 ( ( uint8_t ) 0b1100 )
#define STEPPER_CYCLE2 ( ( uint8_t ) 0b1001 )
#define STEPPER_CYCLE3 ( ( uint8_t ) 0b0011 )
#define STEPPER_CYCLE4 ( ( uint8_t ) 0b0110 )
#define STEPPER_POS_START 0x4000
#define STEPPER_POS_CURRENT_START 0x0000
#define STEPPER_POS_STOP 0x3fff

#define STOVECTR_CONFIG_S_
#define stove_run_hot_temp   0xb8
#define stove_run_med_temp   0x8e
#define stove_run_cool_temp  0x64
#define stove_start_temp     0xd0
#define stove_cool_half_temp 0x48
#define stove_shutdown_temp 0x00
#define stove_back_temp      0xc0
#define stove_mode_count_prescaler_value 0x017a ; this makes the count  1 sec
#define stove_mode_high_temp_lower_count 450
#define stove_mode_warmup_count          900
#define stove_mode_med_temp_lower_count  450
#define stove_mode_low_temp_lower_count  900
#define stove_mode_room_temp_over   1 // must be a whole number
#define stove_mode_room_temp_target 20
#define stove_mode_room_temp_under  1
#define damper_close_pos     0x3c00
#define damper_shutdown_pos  0x3d80
#define damper_open_pos 	  0x1c00
#define bottem_open_pos     0x0a00
#define bottom_close_pos damper_close_pos
#define stove_run_change_count_prescaler  0xff
#define stove_run_change_count_len_stage4 0x0003
#define stove_run_change_count_len_stage3 0x0006
#define stove_run_change_count_len_stage2 0x001f
#define stove_run_change_count_len_stage1 0x004f
#define stove_run_change_steps_stage1     0x0030
#define stove_run_change_steps_stage2     0x0050
#define stove_run_change_steps_stage3     0x0080
#define stove_run_change_steps_stage4     0x00c0
#define stove_run_change_stage1_mask      0x0000
#define stove_run_change_stage2_mask      0x0003
#define stove_run_change_stage3_mask      0x000f
#define stove_run_change_stage4_mask      0x003F
#define open_damper 1
#define close_damper 0
#define room_temp_sensor ds1620_temp
#define stove_control_led_PORT		PORTA
#define stove_control_led_DDR		DDRA
#define stove_control_led_PI		PA2
#define stove_control_buzzer_PORT	PORTA
#define stove_control_buzzer_DDR	DDRA
#define stove_control_buzzer_PI		PA6
#


#define SPI_SLAVE_



#endif /* CONFIG_H_ */
extern uint8_t ds1620_temp;

