// 8 channel soft pwm based on avr136 for tiny2313
#define F_CPU 20000000UL

  // redefine some macros
  #define __C_task int
  #define __watchdog_reset()    wdt_reset()


//! Pin mappings
#define CHMAX       8    // maximum number of PWM channels
//#define PWMDEFAULT  0x80  // default PWM value at start up for all channels





#define CH0_CLEAR (pinlevelB &= ~(1 << PB6)) // map CH0 to PB7
#define CH1_CLEAR (pinlevelB &= ~(1 << PB4)) // map CH1 to PB5
#define CH2_CLEAR (pinlevelB &= ~(1 << PB2)) // map CH2 to PB3
#define CH3_CLEAR (pinlevelB &= ~(1 << PB0)) // map CH3 to PB1
#define CH4_CLEAR (pinlevelB &= ~(1 << PB7)) // map CH4 to PB6
#define CH5_CLEAR (pinlevelB &= ~(1 << PB5)) // map CH5 to PB4
#define CH6_CLEAR (pinlevelB &= ~(1 << PB3)) // map CH6 to PB2
#define CH7_CLEAR (pinlevelB &= ~(1 << PB1)) // map CH7 to PB0


/* 10 pin IDC connector

---------------------
|	|	|	|	|	|
| A | 1	| 3 | 5 | 7 |
|	| 	|	|	|	|
---------------------
|	|	|	|	|	|
| A | 0 | 2 | 4 | 6 |
|	|	|	|	|	|
---------------------

*/


/*
#define CH0_CLEAR (pinlevelB &= ~(1 << PB0)) // map CH0 to PB0
#define CH1_CLEAR (pinlevelB &= ~(1 << PB1)) // map CH1 to PB1
#define CH2_CLEAR (pinlevelB &= ~(1 << PB2)) // map CH2 to PB2
#define CH3_CLEAR (pinlevelB &= ~(1 << PB3)) // map CH3 to PB3
#define CH4_CLEAR (pinlevelB &= ~(1 << PB4)) // map CH4 to PB4
#define CH5_CLEAR (pinlevelB &= ~(1 << PB5)) // map CH5 to PB5
#define CH6_CLEAR (pinlevelB &= ~(1 << PB6)) // map CH6 to PB6
#define CH7_CLEAR (pinlevelB &= ~(1 << PB7)) // map CH7 to PB7
*/



//! prototypes
void Init(void);
void random_function(void);
void dmx_function (void);
