/*
 * config.h  ******************THIS IS FOR UPSTAIRS STOVE
 *
 * Created: 1/1/2013 3:19:15 PM
 *  Author: sasquach
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_


#define SPI_MASTER_


#define DS1620_H_ENABLE

#define STOVECTR_CONFIG_S_
#define stove_run_hot_temp   0xa8
#define stove_run_med_temp   0x8c
#define stove_run_cool_temp  0x64
#define stove_start_temp     0xc8
#define stove_cool_half_temp 0x48
#define stove_shutdown_temp 0x00
#define stove_back_temp      0xc0
#define stove_mode_count_prescaler_value 0x017a ; this makes the count  1 sec
#define stove_mode_high_temp_lower_count 900
#define stove_mode_warmup_count          900
#define stove_mode_med_temp_lower_count  450
#define stove_mode_low_temp_lower_count  900
#define stove_mode_room_temp_over   1 // must be a whole number
#define stove_mode_room_temp_target 20
#define stove_mode_room_temp_under  1
#define damper_close_pos     0x2700
#define damper_shutdown_pos  0x2c00
#define damper_open_pos 	  0x0E50
#define bottem_open_pos     0x0300
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
#define stove_control_led_PORT		PORTA
#define stove_control_led_DDR		DDRA
#define stove_control_led_PI		PA5
#define stove_control_buzzer_PORT	PORTA
#define stove_control_buzzer_DDR	DDRA
#define stove_control_buzzer_PI		PA6
#define stove_control_buzzer_short 1
#define stove_control_buzzer_long  3000

// #define room_temp_sensor ds1620_temp


#endif /* CONFIG_H_ */

