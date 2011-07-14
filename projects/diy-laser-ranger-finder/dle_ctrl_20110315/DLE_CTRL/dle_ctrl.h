#ifndef __DLE_CTRL_H__
#define __DLE_CTRL_H__

#define MSG_BUFF_SIZE 128   // in bytes

#define ADC_STATE_SYNC          1
#define ADC_STATE_SYNC1         3
#define ADC_STATE_SAMPLING      2


extern void FATAL_TRAP(uint16 us_line_num);
extern void wait_for_command(uint8 uc_msg_len);

extern void activate_adc_isr(uint8 uc_adc_stream_en);

extern void spi_set_byte(uint8 uc_addr, uint8 uc_data);    
extern void spi_set_3bytes(uint8 uc_addr, uint8 uc_data0, uint8 uc_data1, uint8 uc_data2);
extern void spi_set_2bytes(uint8 uc_addr, uint8 uc_data0, uint8 uc_data1);

#define SAMPLES_PER_PERIOD  4
#if SAMPLES_PER_PERIOD != 4
    #error SAMPLES_PER_PERIOD is redefined. Rework required!
#endif

// SPI 0 Master Mode, Internal clock by USICT => USIWM = b00 USICS = b00
#define SPI_STROB_ACT    (1<<USIWM0)|(0<<USICS0)|(1<<USITC)
#define SPI_STROB_INACT  (1<<USIWM0)|(0<<USICS0)|(1<<USITC)|(1<<USICLK)

#define SPI_PORT    PORTE
#define SPI_DIR     DDRE
#define SPI_DI_PIN  PE5
#define SPI_DO_PIN  PE6
#define SPI_CLK_PIN PE4
#define SPI_CS_PIN  PE3

#define LASER_FB_PD_PIN  PE2    // Laser feedback photodide pull up

#define CURTAIN_PORT        PORTF
#define CURTAIN_DIR         DDRF
#define CURTAIN_DOWN_PIN    PF4
#define CURTAIN_UP_PIN      PF5

#define DBG_PORT            PORTA
#define DBG_DIR             DDRA
#define DBG0_PIN            PA2
#define DBG1_PIN            PA3


#define POWER_HOLD_PIN      PB5
#define APD_BIAS_PWM_PIN    PB7


// SPI running on 2MHz
#define SEND_SPI_BYTE(spi_data_out, spi_data_in)     \
    {                                                \
        USIDR = spi_data_in;                         \
        USICR = SPI_STROB_ACT;   /* MSB */           \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;                       \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;                       \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;                       \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;                       \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;                       \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;                       \
        USICR = SPI_STROB_INACT;                     \
        USICR = SPI_STROB_ACT;  /* LSB */            \
        USICR = SPI_STROB_INACT;                     \
        spi_data_out = USIDR;                        \
    }


extern volatile uint8 guc_in_msg_idx;
extern uint8 guca_in_msg_buff[MSG_BUFF_SIZE];

extern volatile uint8 guc_out_msg_wr_idx; 
extern uint8 guca_out_msg_buff[MSG_BUFF_SIZE];

extern volatile uint8 guc_adc_mux_hdr;
extern volatile uint8 guc_out_msg_rd_idx;
extern volatile uint8 guc_out_msg_bit_idx;

extern volatile uint8 guc_measure_cnt;
extern volatile uint8 guc_adc_state;
extern volatile uint8 guc_adc_stream_en;

extern uint8 guc_30usec_cnt;
extern volatile uint8 guc_240usec_timer;

extern uint16 gus_acc[SAMPLES_PER_PERIOD];

#define DBG_BUFF_SIZE  256
extern uint8 guca_dbg_buff[DBG_BUFF_SIZE];
extern uint8 guc_dbg_buff_idx;

#endif /* __DLE_CTRL_H__ */

