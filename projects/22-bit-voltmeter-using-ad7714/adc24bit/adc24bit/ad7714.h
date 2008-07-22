typedef struct {
	uint8_t gain_param;
	uint8_t chan;
	uint16_t rate;
} ad7714params_t;

void ad7714init();
void ad7714setup(ad7714params_t new_params);
uint8_t ad7714have_capture();
uint32_t ad7714get_capture();
uint8_t ad7714accum_count();
uint32_t ad7714pull_average();
uint8_t ad7714can_set_params();
void ad7714set_buffer(uint8_t state);
char *ad7714chan_name(uint8_t chan);
double ad7714rate_to_hertz(uint16_t rate);
uint16_t ad7714standard_rate(uint8_t idx);
