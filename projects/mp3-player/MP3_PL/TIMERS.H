// ****TIMERS.H****


#define TIMER_0_INIT_VALUE 0x9F
//#define TIMER_1_TICK 0xC350		//50ms @8MHz
#define TIMER_2_TICK 0x07D0		//1ms @ 16MHz
#define NUM_TIMERS 12


void main_timer_init (void);
void pwm_timer_init (void);
//void delay (word ticks); 
byte start_timer (word ticks);
byte timer_stopped (byte t_number);
void stop_timer (byte t_number);
void delay (word ticks);


volatile byte adc_timer;
volatile byte sys_tick;
volatile word tick1ms;
volatile word fast_timer;
byte sample_time;	//time in ms between adc samples

/*
struct {	byte sec;
					byte min;
					byte hour;
					byte day;
					byte month;
					byte year;
					} time;
*/
