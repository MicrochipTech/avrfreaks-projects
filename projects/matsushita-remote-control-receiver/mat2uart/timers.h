// ****TIMERS.H****


//#define TIMER_0_INIT_VALUE 0x9F
//#define TIMER_1_TICK 0xC350		//50ms @8MHz
//#define TIMER_2_TICK 125		//1ms @ 8MHz
#define TIMER_2_TICK 115		//1ms @ 7,3728MHz

#define NUM_TIMERS 12
#define PWM_STEPS 1500


void main_timer_init (void);
void pwm_timer_init (void);
byte start_timer (word ticks);
void restart_timer (byte t_number,word ticks);
byte timer_stopped (byte t_number,byte release);
void stop_timer (byte t_number);
void delay (word ticks);
byte set_pwm_cycle (byte pwm_num, word value);



/*
struct {	byte sec;
					byte min;
					byte hour;
					byte day;
					byte month;
					byte year;
					} time;
*/
