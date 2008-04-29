#define pwm_record_factor (u08)F_CPU/500.0/256.0
#define lower_pwm_bound pwm_record_factor*4.25	//valid pwm limits
#define upper_pwm_bound pwm_record_factor*1.75
#define GROUND

void enable_ground_control();
void disable_ground_control();
