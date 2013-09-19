#define FREQ_BASE       50U     // 50Hz
#define SPEED_BASE      (FREQ_BASE * 10U -5U)
#define SPEED_BOOST     1U
#define A_BOOST_MIN     25U     // 9.8 percent      

#define KEY_PORT        PORTE
#define KEY_DDR         DDRE
#define KEY_PIN         PINE
#define RUN_KEY_bp      2       // bit position
#define STOP_KEY_bp     3
#define DIR_KEY_bp      4

#define RUN_gc          (1<<DIR_KEY_bp)|(1<<STOP_KEY_bp)
#define STOP_gc         (1<<DIR_KEY_bp)|(1<<RUN_KEY_bp)
#define DIR_gc          (1<<STOP_KEY_bp)|(1<<RUN_KEY_bp)


#define ENABLE_PORT     PORTD
#define ENABLE_DDR      DDRD
#define ENABLE_bp       0

#define FAULT_PORT      PORTD
#define FAULT_DDR       DDRD
#define FAULT_PIN       PIND
#define FAULT_bp        1

#define RUN_LED_PORT    PORTD
#define RUN_LED_DDR     DDRD
#define RUN_LED_bp      2
                   
#define DIR_LED1_PORT   PORTD
#define DIR_LED1_DDR    DDRD
#define DIR_LED1_bp     3

#define DIR_LED2_PORT   PORTD
#define DIR_LED2_DDR    DDRD
#define DIR_LED2_bp     4

#define ENABLE_MOTOR    ENABLE_PORT|=(1<<ENABLE_bp)
#define DISABLE_MOTOR   ENABLE_PORT&=~(1<<ENABLE_bp)

#define RUN_LED_ON      RUN_LED_PORT&=~(1<<RUN_LED_bp)
#define RUN_LED_OFF     RUN_LED_PORT|=(1<<RUN_LED_bp)
#define TOGGLE_RUN_LED  RUN_LED_PORT^=(1<<RUN_LED_bp)

#define DIR_LED1_ON     RUN_LED_PORT&=~(1<<DIR_LED1_bp)
#define DIR_LED1_OFF    RUN_LED_PORT|=(1<<DIR_LED1_bp)

#define DIR_LED2_ON     RUN_LED_PORT&=~(1<<DIR_LED2_bp)
#define DIR_LED2_OFF    RUN_LED_PORT|=(1<<DIR_LED2_bp)


