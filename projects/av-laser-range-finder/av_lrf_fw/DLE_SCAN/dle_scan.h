/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan.h
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#ifndef __DLE_SCAN_H__
#define __DLE_SCAN_H__

#define CMD_MODE_SHORT  1
#define CMD_MODE_FULL   2

#define CURTAIN_ENABLED 1

#ifndef CURTAIN_ENABLED
#define CURTAIN_ENABLED 0
#endif

#define TRACE_SIZE  16
extern uint16 gusa_trace[TRACE_SIZE];

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
#define CURTAIN_DOWN_PIN    PF5
#define CURTAIN_UP_PIN      PF4

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


#define APD_DELAY 3


// ----------------- Global variable ---------------------

// --- IO buffers
#define MSG_BUFF_SIZE 128   // in bytes

extern volatile uint8 guc_in_msg_idx;
extern uint8 guca_in_msg_buff[MSG_BUFF_SIZE];

extern uint8 guca_out_msg_buff[MSG_BUFF_SIZE];
extern volatile uint8 guc_out_msg_wr_idx; 
extern volatile uint8 guc_out_msg_rd_idx;

// --- timer/counters
extern uint8 guc_30usec_cnt;
extern uint16 gus_meas_timer;
extern volatile uint8 guc_240usec_timer;

// --- other
extern uint8 guc_lcd_meas_progress;

#define SPI_MAX_LEN 14
extern uint8 guca_spi_curr_state[SPI_MAX_LEN];

// ------------------------ Global functions -----------------

extern void FATAL_TRAP(uint16 us_line_num);

extern void spi_set_byte(uint8 uc_addr, uint8 uc_data);    
extern void spi_set_3bytes(uint8 uc_addr, uint8 uc_data0, uint8 uc_data1, uint8 uc_data2);
extern void spi_set_2bytes(uint8 uc_addr, uint8 uc_data0, uint8 uc_data1);
extern void spi_read_curr();

extern void apd_control();

#endif /* __DLE_SCAN_H__ */

