#ifndef _DALI_LIB_H_
#define _DALI_LIB_H_

//_____ M A C R O S ________________________________________________________

  /*** EUSART Configuration Macros. See "dali_config.h" to change parameters ***/

#define Dali_set_eucsra()     ( EUCSRA = 0,\
                              EUCSRA = (EUSART_TX_SIZE<<UTxS0   \
                                      | EUSART_RX_SIZE<<URxS0)  )
#define Dali_set_eucsrb()     ( EUCSRB = 0,\
                              EUCSRB = (EUSART_ENABLE<<EUSART   \
                                      | NB_STOP_BITS_RX<<EUSBS  \
                                      | MANCHESTER_MODE<<EMCH   \
                                        | BIT_ORDER<<BODR)      )
#define Dali_set_mubrrh()     ( MUBRRH = (U8)((FOSC/BAUD_RATE)>>8))
#define Dali_set_mubrrl()     ( MUBRRL = (U8)((FOSC/BAUD_RATE)&0x00ff))
#define Dali_set_ubrrh()      ( UBRRH = (U8)((FOSC/(16*BAUD_RATE)-1)>>8))
#define Dali_set_ubrrl()      ( UBRRL = (U8)((FOSC/(16*BAUD_RATE))&0x00ff))

#define Dali_set_ucsra()      ( UCSRA = 0,\
                              UCSRA = (TX_SPEED_X2<<U2X) )
#define Dali_set_ucsrb()      ( UCSRB = 0,\
                              UCSRB = (RX_COMPLETE_INTERRUPT<<RXCIE         \
                                     | TX_COMPLETE_INTERRUPT<<TXCIE         \
                                     | DATA_REGISTER_EMPTY_INTERRUPT<<UDRIE \
                                     | RX_ENABLE<<RXEN | TX_ENABLE<<TXEN) )
#define Dali_set_ucsrc()      ( UCSRC = 0,\
                              UCSRC = (SYNCHRONOUS<<UMSEL     \
                                     | PARITY_CHECK<<UPM0     \
                                     | NB_STOP_BITS_TX<<USBS  \
                                     | CLOCK_POLARITY<<UCPOL) )

#define Dali_enable_rx()      ( UCSRB |= 1<<RXEN )    
#define Dali_disable_rx()     ( UCSRB &= ~(1<<RXEN) )
 

//_____ D E F I N I T I O N S ______________________________________________

#define DALI_BROADCAST  0xfe
#define NO              0
#define YES             0xff
#define DOWN            0
#define UP              1
#define FW_FW_DELAY     84    //(84ms + FW frame duration) = 100ms

#define PHYSICAL_SELECTION_DISABLED   0
#define PHYSICAL_SELECTION_REQUESTED  1 
#define PHYSICAL_SELECTION_ENABLED    2


//_____ D E C L A R A T I O N S ____________________________________________

  /*** Type of DALI Command received ***/

typedef enum{
  NONE=0,
  DIRECT_ARC_POWER=1,
  CONFIG_CMD=2,
  QUERY_CMD=3,
  SPECIAL_CMD=4,
  INDIRECT_ARC_POWER=5,
} Cmd_type;

  /*** 'STATUS REGISTER' bits ***/
typedef union
{
  struct{
    unsigned ballast_failure    :1;
    unsigned lamp_failure       :1;    
    unsigned lamp_on            :1;    
    unsigned limit_error        :1;    
    unsigned fade_running       :1;    
    unsigned reset_state        :1;
    unsigned missing_short_add  :1;
    unsigned power_failure      :1;
  };
  char status_information;
} Dali_status;
    
    /*** DALI Registers ***/

typedef struct{
    Cmd_type      cmd_type;               
    U8            add_byte;                  //1st byte of received frame
    U8            data_byte;                 //2nd byte of received frame
    U8            dtr;                      //Data Transfer Register
    U8            actual_dim_level;           //See DALI Standard
    U8            power_on_level;
    U8            system_failure_level;
    U8            min_level;
    U8            max_level;
    U8            fade_rate;
    U8            fade_time;
    U8            short_add;                 //format 0AAA AAA1
    U8            search_add_H;
    U8            search_add_M;
    U8            search_add_L;
    U8            random_add_H;
    U8            random_add_M;
    U8            random_add_L;
    U16           group;                //MSB : group 15    LSB : group 0. If set, device belongs to group x
    U8            scene[16];
    Dali_status   status;
} St_dali_registers;

  /*** EEPROM Addresses of stored DALI Registers ***/

#define ADD_EEPROM_STATUS           0
#define ADD_POWER_ON_LEVEL          1
#define ADD_SYSTEM_FAILURE_LEVEL    2
#define ADD_MIN_LEVEL               3
#define ADD_MAX_LEVEL               4
#define ADD_FADE_RATE               5
#define ADD_FADE_TIME               6
#define ADD_SHORT_ADD               7
#define ADD_RANDOM_ADDH             8
#define ADD_RANDOM_ADDM             9
#define ADD_RANDOM_ADDL             10
#define ADD_GROUPH                  11
#define ADD_GROUPL                  12
#define ADD_SCENE_0                 13

#define EEPROM_INITIALIZED          0xAA      //if eeprom(0) == 0xAA : eeprom has been written at least once

#ifndef DALI_PHYSICAL_LEVEL
  #define DALI_PHYSICAL_LEVEL 0
#endif
#ifndef DALI_VERSION_NUMBER 
  #define DALI_VERSION_NUMBER 0x00
#endif

U8 new_dali_add =0;
U8 new_dali_data = 0;
volatile bit new_rx = 0;

volatile U8 tick_1ms_counter = 0;     //incremented every ms. 
volatile U8 time_out = 0;     //used to generate a delay between 1 and 255 milliseconds 
U16 time_out_2 = 0;  //long time delay (1-65535), in 1/4th seconds.
//when this timer is decounting, dali is in "special mode" (for 15 minutes).
//when time_out_2 is equal to 0, all special modes shall be terminated

U16 fade_period;       //period between each fade step. = fade_time/(actualLevel - requested_level) or = 1/fade_rate.
U8 requested_level = 0;     //counter, decremented every 1 ms, used for fading...
bit dim_sense = DOWN;  //this bit indicates wether the power must increase or decrease.
bit compare_mode = 0;
U8 physical_selection_mode = 0;

St_dali_registers dali;

//_____ P R O T O T Y P E S ________________________________________________

void dali_init_eusart(void);
void dali_init(void);
void dali_tick(void);
void dali_rx_state_machine(void);
void dali_step_output_power(bit sense);
void dali_change_output_w_fade_time(void);
void dali_up_output_w_fade_rate(void);
void dali_down_output_w_fade_rate(void);
void dali_answer(U8 answer);
void dali_execute(void);
U8 dali_output_power(void);
U8 dali_control_gear(U8);

#endif // _DALI_LIB_H_
