/*
 * TouchHelper.h
 *
 * Created: 4/09/2011 4:59:01 PM
 *  Author: Jim
 */ 


#ifndef TOUCHHELPER_H_
#define TOUCHHELPER_H_

#include "conf_at42qt1060.h"
#include "twim.h"
#include "power_clocks_lib.h"
#include "main.h"
#include "cycle_counter.h"
#include "at42qt1060.h"
#include "gpio.h"
#include "RingBuffer.h"

typedef struct 
{
  uint8_t detect_status;
  uint16_t key_signal[6];
  uint8_t key_threshold[6];
  uint16_t key_ref_value[6];
} at42qt1060_data;

void touch_init(void);
static void twi_init(void);
static void init_sys_clocks(void);
void get_key_signal_values(at42qt1060_data *touch_data);
void get_key_ref_values(at42qt1060_data *touch_data);
void touch_detect_callback(void);
uint8_t touch_get_keys(void);

RingBuff_t pressQueue;
volatile bool touch_detect;
static pcl_freq_param_t pcl_freq_param;
volatile at42qt1060_data touch_data;
volatile at42qt1060_data old_touch_data;

#endif /* TOUCHHELPER_H_ */