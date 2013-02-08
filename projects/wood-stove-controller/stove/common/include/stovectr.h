/*
 * stovectr.h
 *
 * Created: 12/28/2012 9:08:45 PM
 *  Author: sasquach
 */ 


#ifndef STOVECTR_H_
#define STOVECTR_H_

#ifndef STOVECTR_CONFIG_S_ // defines below should be copied to config.h if need to to be modified
// START COPY HERE
// STOVE CONTROL DEFINITIONS
#define STOVECTR_CONFIG_S_
#define stove_run_hot_temp   0xa8
#define stove_run_med_temp   0x8c
#define stove_run_cool_temp  0x64
#define stove_start_temp     0xc8
#define stove_cool_half_temp 0x48
#define stove_shutdown_temp 0x00
#define stove_back_temp      0xc0
#define stove_mode_count_prescaler_value 0x017a ; this makes the count  1 sec
#define stove_mode_high_temp_lower_count 300
#define stove_mode_warmup_count          600
#define stove_mode_med_temp_lower_count  300
#define stove_mode_low_temp_lower_count  600
#define stove_mode_room_temp_over   1 // must be a whole number
#define stove_mode_room_temp_target 20
#define stove_mode_room_temp_under  1
#define damper_close_pos     0x2700
#define damper_shutdown_pos  0x2c00
#define damper_open_pos 	  0x1300
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
#define room_temp_sensor ds1620_temp
#define stove_control_led_PORT		PORTA
#define stove_control_led_DDR		DDRA
#define stove_control_led_PI		PA5
#define stove_control_buzzer_PORT	PORTA
#define stove_control_buzzer_DDR	DDRA
#define stove_control_buzzer_PI		PA6
#define stove_control_buzzer_short 2
#define stove_control_buzzer_long  300
//END COPY HERE
#endif 

// internal defines
#define stove_control_reset_normal 1
#define stove_control_reset_nite 2

// stove mode byte definitions

//upper 0bxxxx ----
#define MODE_SRT  1
#define MODE_HOT  2
#define MODE_MED  3
#define MODE_LOW  4
#define MODE_OFF  5
//lower 0b---- --xx
#define MODE_NORM 0
#define MODE_NITE 1
#define MODE_FEED 2
//bits in byte for led n beep
//		0b---- xx--
#define MODE_LED_bit  2 //bit shift
#define MODE_BEEP_bit 3 // bit shift

extern uint8_t stove_mode;
extern uint8_t stove_target_temp;
extern uint8_t stove_mode_room_temp_target_var;
extern uint8_t stove_target_temp_ref;
extern uint16_t stove_run_change_count_len;
extern uint16_t stove_run_change_count;
extern uint16_t stove_run_change_temp_count;
extern uint16_t stove_run_change_steps;
extern uint16_t stove_run_upper_limit;
extern uint16_t stove_run_lower_limit;
extern uint8_t stove_run_limit_result;
extern uint16_t stove_cooldown_count;
extern uint8_t stove_target_temp_lockout;
extern uint8_t stove_mode_start_first;

void stove_control(void);
void stove_control_init(void);
void stove_control_reset(uint8_t);
void stove_control_nite_mode(void);

#endif /* STOVECTR_H_ */

