/*
 * TouchHelper.c
 *
 * Created: 4/09/2011 5:01:45 PM
 *  Author: Jim
 */ 

#include "TouchHelper.h"
#include "udi_hid_kbd.h"

volatile bool touch_detect = false;
volatile uint8_t keys;

static pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = FCPU_HZ,
  .pba_f        = FPBA_HZ,
  .osc0_f       = FOSC0,
  .osc0_startup = OSC0_STARTUP
};

void touch_init()
{
	init_sys_clocks();
	twi_init();
	cpu_delay_ms(230, FCPU_HZ);
	at42qt1060_init(FCPU_HZ);
	at42qt1060_register_int(&touch_detect_callback);
	RingBuffer_InitBuffer(&pressQueue);
}

static void init_sys_clocks(void)
{
  // Configure system clocks.
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
    while(1);
}

static void twi_init(void)
{
  const gpio_map_t AT42QT1060_TWI_GPIO_MAP =
  {
	  {AT42QT1060_TWI_SCL_PIN, AT42QT1060_TWI_SCL_FUNCTION},
	  {AT42QT1060_TWI_SDA_PIN, AT42QT1060_TWI_SDA_FUNCTION}
  };

  const twi_options_t AT42QT1060_TWI_OPTIONS =
  {
    .pba_hz = FPBA_HZ,
    .speed = AT42QT1060_TWI_MASTER_SPEED,
    .chip = AT42QT1060_TWI_ADDRESS
  };

  // Assign I/Os to SPI.
  gpio_enable_module(AT42QT1060_TWI_GPIO_MAP,
    sizeof(AT42QT1060_TWI_GPIO_MAP) / sizeof(AT42QT1060_TWI_GPIO_MAP[0]));
  // Initialize as master.
  twi_master_init(AT42QT1060_TWI, &AT42QT1060_TWI_OPTIONS);

}
void get_key_signal_values(at42qt1060_data *touch_data)
{
	LSB(touch_data->key_signal[0]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_0_SIGNAL_LSB);
	MSB(touch_data->key_signal[0]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_0_SIGNAL_MSB);
	LSB(touch_data->key_signal[1]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_1_SIGNAL_LSB);
	MSB(touch_data->key_signal[1]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_1_SIGNAL_MSB);
	LSB(touch_data->key_signal[2]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_2_SIGNAL_LSB);
	MSB(touch_data->key_signal[2]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_2_SIGNAL_MSB);
	LSB(touch_data->key_signal[3]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_3_SIGNAL_LSB);
	MSB(touch_data->key_signal[3]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_3_SIGNAL_MSB);
	LSB(touch_data->key_signal[4]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_4_SIGNAL_LSB);
	MSB(touch_data->key_signal[4]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_4_SIGNAL_MSB);
	LSB(touch_data->key_signal[5]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_5_SIGNAL_LSB);
	MSB(touch_data->key_signal[5]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_5_SIGNAL_MSB);
}

void get_key_ref_values(at42qt1060_data *touch_data)
{
	LSB(touch_data->key_ref_value[0]) =
		at42qt1060_read_reg(AT42QT1060_KEY_0_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[0]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_0_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[1]) =
		at42qt1060_read_reg(AT42QT1060_KEY_1_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[1]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_1_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[2]) =
		at42qt1060_read_reg(AT42QT1060_KEY_2_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[2]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_2_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[3]) =
		at42qt1060_read_reg(AT42QT1060_KEY_3_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[3]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_3_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[4]) =
		at42qt1060_read_reg(AT42QT1060_KEY_4_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[4]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_4_REF_DATA_MSB);
	LSB(touch_data->key_ref_value[5]) =
		at42qt1060_read_reg(AT42QT1060_KEY_5_REF_DATA_LSB);
	MSB(touch_data->key_ref_value[5]) = 
		at42qt1060_read_reg(AT42QT1060_KEY_5_REF_DATA_MSB);
}

void touch_detect_callback(void)
{
	keys = at42qt1060_read_reg(AT42QT1060_DETECTION_STATUS);
    // need to read input port status too to reset CHG line
    at42qt1060_read_reg(AT42QT1060_INPUT_PORT_STATUS);
	gpio_tgl_gpio_pin(LED3_GPIO);
}

uint8_t touch_get_keys(void)
{
	return keys;
}