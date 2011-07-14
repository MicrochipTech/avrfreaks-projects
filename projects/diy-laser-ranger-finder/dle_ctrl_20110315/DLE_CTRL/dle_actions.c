/***C*********************************************************************************************
**
** SRC-FILE     :   dle_actions.c
**                                        
** PROJECT      :   DLE Control
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Set of primitive actions such as laser on/off, APD BIAS control, etc 
**                  to control DLE behavior and perform measuremnts
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2010
**
****C*E******************************************************************************************/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "dle_actions.h"
#include "dle_ctrl.h"

//------ Global variables ---------------------
volatile uint8 guc_in_msg_idx;
uint8 guca_in_msg_buff[MSG_BUFF_SIZE];

volatile uint8 guc_out_msg_wr_idx; 
volatile uint8 guc_out_msg_rd_idx;

uint8 guca_out_msg_buff[MSG_BUFF_SIZE];

uint8 guca_spi_curr_state[14+2+10]; // +10 just in case. Remove after debug

extern volatile uint8 guc_timer_cnt;
extern volatile uint8 guc_resync_trig;

volatile uint8 guc_adc_mux_hdr;

extern uint16 gusa_trace[16];

T_FREQ_DATA gta_freq_table[] = {

    {0x37E4, 0, 0, 0 },    //
    {0x2BE4, 0, 0, 0 },
    {0x1EE4, 0, 0, 0 },
             
    {0xC9D5, 0, 0, 0 },
    {0xBDD5, 0, 0, 0 },
    {0xB5D5, 0, 0, 0 },
    {0xA9D5, 0, 0, 0 },
             
    {0xD3B9, 0, 0, 0 },
    {0xC7B9, 0, 0, 0 },
    {0xB4B9, 0, 0, 0 },
    {0xA8B9, 0, 0, 0 },
             
    {0xDEAC, 0, 0, 0 },
    {0xD2AC, 0, 0, 0 },
             
    {0xABAD, 0, 0, 0 },
    {0x9FAD, 0, 0, 0 },

    {0, 0, 0}
};

//------ Local functions ---------------------
void spi_set_byte(uint8 uc_addr, uint8 uc_data){
    uint8 uc_feedback;

    SPI_PORT &= ~_BV(SPI_CS_PIN);
    SEND_SPI_BYTE(uc_feedback, uc_addr);
    SEND_SPI_BYTE(uc_feedback, uc_data);
    SPI_PORT |= _BV(SPI_CS_PIN);
    return;
}

void spi_set_2bytes(uint8 uc_addr, uint8 uc_data0, uint8 uc_data1){
    uint8 uc_feedback;

    SPI_PORT &= ~_BV(SPI_CS_PIN);
    SEND_SPI_BYTE(uc_feedback, uc_addr);
    SEND_SPI_BYTE(uc_feedback, uc_data0);
    SEND_SPI_BYTE(uc_feedback, uc_data1);
    SPI_PORT |= _BV(SPI_CS_PIN);
    return;
}

void spi_set_3bytes(uint8 uc_addr, uint8 uc_data0, uint8 uc_data1, uint8 uc_data2){
    uint8 uc_feedback;

    SPI_PORT &= ~_BV(SPI_CS_PIN);
    SEND_SPI_BYTE(uc_feedback, uc_addr);
    SEND_SPI_BYTE(uc_feedback, uc_data0);
    SEND_SPI_BYTE(uc_feedback, uc_data1);
    SEND_SPI_BYTE(uc_feedback, uc_data2);
    SPI_PORT |= _BV(SPI_CS_PIN);
    return;
}

void calc_sincos(T_FREQ_DATA *pt_meas){

    uint16 us_sampl0, us_sampl1;
    uint8  uc_scale, uc_rnd_const;

    // 2^(APD_MEAS_SAMPLES_P - 2) number of samples in each ACC
    uc_scale = (MEAS_SAMPLES_P - 2); 
    uc_rnd_const = 1 << (uc_scale - 1);

    us_sampl0 = (gus_acc[0] + uc_rnd_const) >> uc_scale;
    us_sampl1 = (gus_acc[2] + uc_rnd_const) >> uc_scale;
    pt_meas->s_cos = (us_sampl0 - us_sampl1);

    us_sampl0 = (gus_acc[1] + uc_rnd_const) >> uc_scale;
    us_sampl1 = (gus_acc[3] + uc_rnd_const) >> uc_scale;
    pt_meas->s_sin = (us_sampl0 - us_sampl1);
}

void send_out_msg(uint8 uc_len){

    uint8 uc_i;

    for (uc_i = 0; uc_i < uc_len; uc_i++)
    {
        while((UCSR0A & (1 << UDRE0)) == 0);
        UDR0 = guca_out_msg_buff[uc_i];
    }
}

void spi_read_curr(){

    uint8 uc_feedback, uc_i;

    SPI_PORT &= ~_BV(SPI_CS_PIN);                \

    SEND_SPI_BYTE(uc_feedback, 0x80);

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < 14; uc_i++){
        SEND_SPI_BYTE(uc_feedback, 0);
        guca_spi_curr_state[uc_i] = uc_feedback;
    }

    SPI_PORT |= _BV(SPI_CS_PIN); 
}

//------ Command functions -------------------
void action_spi_rd(){

    uint8 uc_spi_len, uc_i;
    uint8 uc_feedback;

    // --- Play out raw SPI sequence ---

    // 0xLL   0xSS
    // Length Read reques len

    // response 
    // 0x16 0xSS   0xDD ...
    //      CMD  Length SPI raw data 0xLL bytes
    
    // wait until full message received
    while(guc_in_msg_idx < 4);

    // --- complete message received here ---
    uc_spi_len = guca_in_msg_buff[3];

    // Write message header
    guca_out_msg_buff[0] = 0x16;
    guca_out_msg_buff[1] = uc_spi_len+1;

    SPI_PORT &= ~_BV(SPI_CS_PIN);                \

    SEND_SPI_BYTE(uc_feedback, 0x80);
    guca_out_msg_buff[2] = uc_feedback;

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < uc_spi_len; uc_i++){
        SEND_SPI_BYTE(uc_feedback, 0x00);
        guca_out_msg_buff[uc_i+3] = uc_feedback;
    }

    SPI_PORT |= _BV(SPI_CS_PIN);                 \

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = uc_i + 3;

    // End of action_spi_rd
    return;
}

void action_spi_wr(){

    uint8 uc_spi_len, uc_i;
    uint8 uc_feedback;

    // --- Play out raw SPI sequence ---

    // 0xLL   0xAA    0xDD ...
    // Length Address SPI raw data 0xLL bytes

    // response 
    // 0xD5 0x15 0xLL   0xDD ...
    //      CMD  Length SPI raw data 0xLL bytes
    
    // Check is message length available, wait if not yet
    while(guc_in_msg_idx < 3);

    uc_spi_len = guca_in_msg_buff[2];

    // Check is complete message received
    while(guc_in_msg_idx < (uint8)(uc_spi_len+3));

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x15;
    guca_out_msg_buff[1] = uc_spi_len;

    SPI_PORT &= ~_BV(SPI_CS_PIN);                \

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < uc_spi_len; uc_i++){
        SEND_SPI_BYTE(uc_feedback, guca_in_msg_buff[uc_i+3]);
        guca_out_msg_buff[uc_i+2] = uc_feedback;
    }

    SPI_PORT |= _BV(SPI_CS_PIN);                 \

    // Check no more input data received
    if (guc_in_msg_idx != (uint8)(uc_spi_len+3))
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = uc_i + 2;

    // End of action_spi_wr
    return;
}


void action_signature(){

    uint8 uc_i;

    // Response format
    // 0x11 0xLL    0xMM    0xMM    0xCC
    // CMD  Length  Major   Minor   String name
    gusa_trace[1]++;

    // Write header & length
    guca_out_msg_buff[0] = 0x11;
    guca_out_msg_buff[1] = SIGN_LEN+2;

    // Write version number
    guca_out_msg_buff[2] = gt_hw_info.uc_ver_maj;
    guca_out_msg_buff[3] = gt_hw_info.uc_ver_min;

    // Write signature
    for(uc_i = 0; uc_i < SIGN_LEN; uc_i++){
        guca_out_msg_buff[uc_i+4] = gt_hw_info.ca_signature[uc_i];
    }

    // Check no more input data received
    if (guc_in_msg_idx != 2)
        FATAL_TRAP(__LINE__);

    guc_out_msg_wr_idx = SIGN_LEN + 4;


    // End of action_signature
    return;
}

void action_adc_mux(){

    uint8 uc_mux, uc_tmp;

    // wait until full message received
    while(guc_in_msg_idx < 2);

    uc_mux = guca_in_msg_buff[2] & 0x03;   // Read ADC source from input buffer

    // Write Packet type and ADC source to ADC message header
    guc_adc_mux_hdr = (uint8)(((uint16)uc_mux << ADC_MSG_MUX_BIT)>>8);

    // Modify ADC input selector
    uc_tmp = ADMUX;
    uc_tmp &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3) | (1 << MUX4));     // clear previous MUX bits
    uc_tmp |= (uc_mux << MUX0);
    ADMUX = uc_tmp;
                     
    // Send command response to host
    guca_out_msg_buff[0] = 0x14;
    guca_out_msg_buff[1] = uc_tmp;
    guc_out_msg_wr_idx = 2;

}

void action_curtain(){

    uint8 uc_act;
    uint8 uc_tmp;

    // 0x41 0x01   0xDD 
    // CMD  Length ACT

    // response 
    // 0x41 0x01   0xDD ...
    // CMD  Length ACT

    // ACT=0 -> laser curtain open (up)
    // ACT=1 -> laser curtain close (down)
    // ACT=2 -> laser curtain disabled (IO pin disconnected)

    // wait until full message received
    while(guc_in_msg_idx < 4);
    
    uc_act = guca_in_msg_buff[3];

    if (uc_act != 2)
    { // either close or open curtain

        // Prepare IO port and dir registers
        CURTAIN_PORT |= 
             ((1 << CURTAIN_UP_PIN  )|
              (1 << CURTAIN_DOWN_PIN));

        CURTAIN_DIR |= 
             ((1 << CURTAIN_UP_PIN  )|
              (1 << CURTAIN_DOWN_PIN));

        // Disable JTAG (enabled at reset), in order to override IO pins
        uc_tmp = MCUCR | (1 << JTD);
        MCUCR = uc_tmp;
        MCUCR = uc_tmp;

        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
    
    
        if (uc_act)
        {
            CURTAIN_PORT &= ~(1 << CURTAIN_DOWN_PIN);
        }
        else
        {
            CURTAIN_PORT &= ~(1 << CURTAIN_UP_PIN);
        }

        // wait 50ms
        guc_240usec_timer = (50000/240);
        while(guc_240usec_timer);

        // remove driver signal
        CURTAIN_PORT |= 
            (1 << CURTAIN_UP_PIN  )|
            (1 << CURTAIN_DOWN_PIN);

        // Note: Curtain IO pins remain active, normally should override signal from mkII
        //       In order to return JTAG control IO pins must be disabled
    }
    else
    { // ACT=2. Disable curtain IO pins by enabling JTAG
        uc_tmp = MCUCR & ~(1 << JTD);
        MCUCR = uc_tmp;
        MCUCR = uc_tmp;
    }

    // Send command response to host
    guca_out_msg_buff[0] = 0x41;
    guca_out_msg_buff[1] = 1;
    guca_out_msg_buff[2] = uc_act;
    guc_out_msg_wr_idx = 3;

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // End of action_curtain
    return;

}

void action_laser_aim(){

    uint8 uc_action;

    // 0x1    0xDD    
    // Length Action

    // response 
    // 0x32 0x01   0xDD 
    // CMD  Length Action
    
    // Wait until complete message received
    while(guc_in_msg_idx < 4);

    uc_action = guca_in_msg_buff[3];

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x32;
    guca_out_msg_buff[1] = 0x01;


    // Read SPI current state
    spi_read_curr();

    // Get current laser state. Consider 0x10, 0xAB as Enabled, disabled otherwise
    if (uc_action == 2)     // AIM toggle
    {
        if (guca_spi_curr_state[8] == 0xAB)      // Is AIM enabled
        { // Toggle to disable
            uc_action = 0;
        }
        else
        { // Toggle to enable
            uc_action = 1;
        }
    }

    guca_out_msg_buff[2] = uc_action;

    if (uc_action == 1)
    { // AIM enable
        // Enable laser if disabled (0x06 & 0x10) == 0
        if ((guca_spi_curr_state[3] & 0x10) == 0)
        {   // 0x06 |= 0x10;
            spi_set_byte(0x06, guca_spi_curr_state[3] | 0x10);
        }

        // Enable AIM mode if not set
        if (guca_spi_curr_state[8] != 0xAB)
        {   // 0x10 = 0xAB;
            spi_set_byte(0x16, 0x76);
            spi_set_byte(0x10, 0xAB);
            spi_set_byte(0x16, 0xF6);
        }

    }
    else
    { // AIM disable
        // Clear AIM mode if not zero
        if (guca_spi_curr_state[8] != 0x00)
        {   // 0x10 = 0x00;
            spi_set_byte(0x16, 0x76);
            spi_set_byte(0x10, 0x00);
            spi_set_byte(0x16, 0xF6);
        }

        // Disable laser if enable (0x06 & 0x10) != 0
        if ((guca_spi_curr_state[3] & 0x10) != 0)
        {   // 0x06 &= ~0x10;
            spi_set_byte(0x06, (guca_spi_curr_state[3] & ~0x10));
        }

    }

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = 3;

    // End of action_laser_aim
    return;

}

void action_laser_off(){

    // 0x00   
    // Length 

    // response 
    // 0x33 0x00  
    // CMD  Length 
    
    // Wait until complete message received
    while(guc_in_msg_idx < 3);

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x33;
    guca_out_msg_buff[1] = 0x00;


    // Read SPI current state
    spi_read_curr();

    // Clear AIM mode if not zero
    if (guca_spi_curr_state[8] != 0x00)
    {   // 0x10 = 0x00;
        spi_set_byte(0x16, 0x76);
        spi_set_byte(0x10, 0x00);
        spi_set_byte(0x16, 0xF6);
    }

    // Disable X3 if enabled
    if ((guca_spi_curr_state[3] & 0xC0) != 0)
    {   // 0x06 &= ~0xC0;
        spi_set_byte(0x06, (guca_spi_curr_state[3] & ~0xC0));  // F4 -> 34
    }

    // Disable laser if enable (0x06 & 0x10) != 0
    if ((guca_spi_curr_state[3] & 0x10) != 0)
    {   // 0x06 &= ~0x10;
        spi_set_byte(0x06, (guca_spi_curr_state[3] & ~0x10));  // 34 -> 24
    }

    // Check no more input data received
    if (guc_in_msg_idx != 3)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = 2;

    // End of action_laser_off
    return;

}

void rise_bias_ctrl(uint16 us_curr_bias, uint16 us_final_bias){

    while (us_final_bias != us_curr_bias)
    {
        us_curr_bias++;
        if (us_curr_bias & 0x0100)
        {
            us_curr_bias &= ~0x0100;
            us_curr_bias |= 0x9000;
        }

        spi_set_3bytes(0x12, (us_curr_bias >> 8) & 0x90, us_curr_bias & 0xFF, 0xF6);
    }
}

void down_bias_ctrl(uint16 us_curr_bias, uint16 us_final_bias){

    while (us_final_bias != us_curr_bias)
    {
        us_curr_bias--;
        if ((us_curr_bias >> 8) == 0x8F)
        {
            us_curr_bias &= ~0xFF00;
            us_curr_bias |= 0x1000;
        }

        spi_set_3bytes(0x12, (us_curr_bias >> 8) & 0x90, us_curr_bias & 0xFF, 0xF6);
    }
}

void action_apd_on(){

    T_FREQ_DATA t_apd_res;
    uint16 us_bias_ctrl, us_bias_ctrl_curr;

    // 0x21 0x02   0xDDDD             
    // CMD  Length BIAS CNTRL Level   

    // response 
    // 0x21 0x06    0xDDDD        0xDDDD     0xDDDD
    // CMD  Length  CNTRL Level   ADC0 COS   ADC0 SIN
    
    // Wait until complete message received
    while(guc_in_msg_idx < 5);

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x21;
    guca_out_msg_buff[1] = 0x06;

    // Read SPI current state
    spi_read_curr();

    // ----------------------------------------------------------
    // --- Prepare for BIAS shift
    // ----------------------------------------------------------
    // Enable APD bias if not enabled (0x06 & 0x20) == 0
    if ((guca_spi_curr_state[3] & 0x20) == 0)
    {   // 0x06 |= 0x20;    
        guca_spi_curr_state[3] |= 0x20;
        spi_set_byte(0x06, guca_spi_curr_state[3]);            // 04 -> 24

        // Wait a little after APD power ON (50ms)
        guc_240usec_timer = (50000/240);
        while(guc_240usec_timer);

    } 
    
    // Enable laser if disabled (0x06 & 0x10) == 0
    if ((guca_spi_curr_state[3] & 0x10) == 0)
    {   // 0x06 |= 0x10;
        guca_spi_curr_state[3] |= 0x10;                         // 24->34
        spi_set_byte(0x06, guca_spi_curr_state[3]);   

        guc_240usec_timer = (20000/240);    // wait 20ms
        while(guc_240usec_timer);

    }

    // Enable modulator if disabled                
    if (guca_spi_curr_state[8] != 0x66)
    {   // 0x10 = 0x66;
        spi_set_byte(0x16, 0x76);                               // 0x00->0x66
        spi_set_byte(0x10, 0x66);
        spi_set_byte(0x16, 0xF6);
    }

    // Enable X3 if not set
    if ((guca_spi_curr_state[3] & 0xC0) != 0xC0)
    {   // 0x06 |= 0xC0;
        guca_spi_curr_state[3] |= 0xC0;
        spi_set_byte(0x06, guca_spi_curr_state[3]);             // 34->F4
    }


    // ----------------------------------------------------------
    // --- Shift BIAS
    // ----------------------------------------------------------
    us_bias_ctrl = guca_in_msg_buff[3] | (guca_in_msg_buff[4] << 8);

    if (us_bias_ctrl != 0)
    { // BIAS Control level is specified. Change level until specified value.

        // verify BIAS control value
        if ((us_bias_ctrl & 0x1000) && ((us_bias_ctrl & ~0x90FF) == 0))
        {   // Start BIAS rise/down from BIAS current value.
            us_bias_ctrl_curr = (guca_spi_curr_state[9] << 8) | guca_spi_curr_state[10];
            if (us_bias_ctrl > us_bias_ctrl_curr)
            {
                rise_bias_ctrl(us_bias_ctrl_curr, us_bias_ctrl);
            }
            else
            {
                down_bias_ctrl(us_bias_ctrl_curr, us_bias_ctrl);
            }
        }
    }

    // ----------------------------------------------------------
    // --- Get final BIAS control value and send it back
    // ----------------------------------------------------------

    // Read SPI current state
    spi_read_curr();

    // Write final BIAS_CTRL to response
    guca_out_msg_buff[2] = guca_spi_curr_state[10];
    guca_out_msg_buff[3] = guca_spi_curr_state[9];

    // ----------------------------------------------------------
    // --- Collect samples from ADC0 and send average to HOST
    // ----------------------------------------------------------
    
    // wait 20ms until APD level become stabilised
    guc_240usec_timer = (20000/240);
    while(guc_240usec_timer);

    DBG_PORT &= ~(1 << DBG0_PIN  );

    activate_adc_isr(0);      // ADC stream disabled

    // Wait until requested samples num are collected (ADC disabled)
    while (ADCSRA & (1<<ADEN));

    DBG_PORT |= (1 << DBG0_PIN  );

    calc_sincos(&t_apd_res);

    guca_out_msg_buff[4] = t_apd_res.s_cos & 0xFF;
    guca_out_msg_buff[5] = (uint8)(t_apd_res.s_cos >> 8);

    guca_out_msg_buff[6] = t_apd_res.s_sin & 0xFF;
    guca_out_msg_buff[7] = (uint8)(t_apd_res.s_sin >> 8);

    // Check no more input data received
    if (guc_in_msg_idx != 5)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = 8;

    // End of action_apd_on
    return;
}

void action_apd_off(){

    // 0x00   
    // Length 

    // response 
    // 0x22 0x00  
    // CMD  Length 
    
    // Wait until complete message received
    while(guc_in_msg_idx < 3);

    // --- complete message received here ---

    // Write message header
    guca_out_msg_buff[0] = 0x22;
    guca_out_msg_buff[1] = 0x00;

    // Read SPI current state
    spi_read_curr();

    spi_set_byte(0x16, 0x76);

    // Init APD bias if not zero 0x12 != 0x10 0x00
    if ( (guca_spi_curr_state[9]  != 0x10) ||
         (guca_spi_curr_state[10] != 0x00) )
    {  
        spi_set_2bytes(0x12, 0x10, 0);
    }

    // Disable X3 if set
    if ((guca_spi_curr_state[3] & 0xC0) == 0xC0)
    {   // 0x06 &= ~0xC0;
        guca_spi_curr_state[3] &= ~0xC0;
        spi_set_byte(0x06, guca_spi_curr_state[3]);          // F4->34
    }

    // Disable modulator & laser if enabled
    if (guca_spi_curr_state[8] != 0x00)
    {   // 0x10 = 0x00;
        spi_set_byte(0x10, 0x00);
        guca_spi_curr_state[3] &= ~0x10;
        spi_set_byte(0x06, guca_spi_curr_state[3]);          // 34->24
    }


    // Disable APD bias if not disabled (0x06 & 0x20) != 0
    if ((guca_spi_curr_state[3] & 0x20) != 0)
    {   // 0x06 &= ~0x20;       
        spi_set_byte(0x06, (guca_spi_curr_state[3] & ~0x20));  // 24 -> 04
    }


    spi_set_byte(0x16, 0xF6);

    // Check no more input data received
    if (guc_in_msg_idx != 3)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---
    guc_out_msg_wr_idx = 2;

    // End of action_laser_off
    return;

}



void action_measure(){

    T_FREQ_DATA *pt_freq;

    // Command
    // [1]  [2]     [3]
    // 0x51 0x01    0xDD           
    // CMD  Length  Measure Type 

    // response 
    // [0]  [1]       [2]         [3]         [4]     [5]            [6]
    // 0x51 0xDD      0xDD        0xDD        0xDD    0xDD           0xDDDD...
    // CMD  H.Length  Burst_num   Burst_size  Delay   Measure Type   Measure results

    // Wait until complete message received
    while(guc_in_msg_idx < 4);

    // --- complete message received here ---

    // -----------------------------------------------------------------
    // --- Compose & send response header
    // -----------------------------------------------------------------

    // Write message header
    guca_out_msg_buff[0] = 0x51;
    guca_out_msg_buff[1] = 6;                       // Header length only. Full response length depent on measure type
    guca_out_msg_buff[2] = 0;

    // Count number of bursts
    pt_freq = &gta_freq_table[0];
    while(pt_freq->us_freq)
    {
        pt_freq++;
        guca_out_msg_buff[2]++;
    } 

    guca_out_msg_buff[3] = 1 << MEAS_SAMPLES_P;     // Burst size
    guca_out_msg_buff[4] = DELAY_SAMPLES_NUM;       // Delay
    guca_out_msg_buff[5] = guca_in_msg_buff[3];     // Measure type

    // Send response header
    send_out_msg(guca_out_msg_buff[1]);

    // -----------------------------------------------------------------
    // --- Prepare for measurement
    // -----------------------------------------------------------------

    // Read SPI current state
    spi_read_curr();
        
    // Enable laser if disabled (0x06 & 0x10) == 0
    if ((guca_spi_curr_state[3] & 0x10) == 0)
    {   // 0x06 |= 0x10;
        guca_spi_curr_state[3] |= 0x10;                      // 24->34
        spi_set_byte(0x06, guca_spi_curr_state[3]);   
    }

    // wait a little
    guc_240usec_timer = (10000/240);
    while(guc_240usec_timer);

    // Enable modulator if disabled                
    if (guca_spi_curr_state[8] != 0x66)
    {   // 0x10 = 0x66;
        spi_set_byte(0x16, 0x76);                           // 0x00->0x66
        spi_set_byte(0x10, 0x66);
        spi_set_byte(0x16, 0xF6);
    }

    // wait a little
    guc_240usec_timer = (10000/240);
    while(guc_240usec_timer);


    // Enable X3 if not set
    if ((guca_spi_curr_state[3] & 0xC0) != 0xC0)
    {   // 0x06 |= 0xC0;
        guca_spi_curr_state[3] |= 0xC0;
        spi_set_byte(0x06, guca_spi_curr_state[3]);          // 34->F4
    }

    // wait a little
    guc_240usec_timer = (10000/240);
    while(guc_240usec_timer);

    DBG_PORT &= ~(1 << DBG0_PIN  );

    // -----------------------------------------------------------------
    // --- Enable ADC interrupt
    // --- ADC interrupt sends ADC samples according to the FREQ TABLE
    // --- and autodisabled after whoe table processed
    // -----------------------------------------------------------------
    // init FREQ TABLE
    pt_freq = &gta_freq_table[0];

    while(pt_freq->us_freq)
    {
        // Init measurement if FREQ TABLE pointer != END
        // Set FREQ from table
        spi_set_2bytes(0x02, pt_freq->us_freq >> 8, (uint8)pt_freq->us_freq);

        activate_adc_isr(guca_in_msg_buff[3] & MEAS_TYPE_STREAM );

        // Wait until requested samples are collected (ADC disabled)
        while (ADCSRA & (1<<ADEN));

        // Send measured SIN, COS if requested
        if (guca_in_msg_buff[3] & MEAS_TYPE_SINCOS)
        {
            calc_sincos(pt_freq);

            guca_out_msg_buff[0] = (uint8)pt_freq->s_cos & 0xFF;
            guca_out_msg_buff[1] = (uint8)(pt_freq->s_cos >> 8);

            guca_out_msg_buff[2] = (uint8)pt_freq->s_sin & 0xFF;
            guca_out_msg_buff[3] = (uint8)(pt_freq->s_sin >> 8);

            send_out_msg(4);
        }

        // Advance table pointer
        pt_freq++;

    } // end of measure init

    // Calculate distance
    // ...
    DBG_PORT |= (1 << DBG0_PIN  );

    // Check no more input data received
    if (guc_in_msg_idx != 4)
        FATAL_TRAP(__LINE__);

    // --- SPI transaction finished. Send feedback upward ---

    // End of action_measure
    return;

}

void action_lcd(){

    uint8 uc_i;
    uint8 uc_digit_mask;
    uint8 uc_digit, uc_set;

    // Command                                 
    // [1]  [2]     [3]           [8]           [12]
    // 0x61 0x01    0xDDDDDDDDDD  0xDDDDDDDDDD  0xDDDDDDDD
    // CMD  Length  Digits set 0  Digits set 1  Signs bitmask see gta_signs_map table for details

    // response 
    // [0]  [1]       
    // 0x61 0xDD      
    // CMD  Length  

    // Wait until complete message received
    while(guc_in_msg_idx < 17);

    // --- complete message received here ---

    // Write response message header
    guca_out_msg_buff[0] = 0x61;
    guca_out_msg_buff[1] = 0x00;

    // Clear all segments
    for(uc_i = 0; uc_i < (uint8)(&LCDDR18 - &LCDDR0 + 1); uc_i ++)
    {
        (&LCDDR0)[uc_i] = 0;
    }

    // -----------------------------------------------------------------
    // --- Decomposite Digits set 0/1
    // -----------------------------------------------------------------
    T_DIGIT_MAP *pt_digit_map;
    uint8   *puc_inp;

    for (uc_set = 0; uc_set < 2; uc_set ++)   
    {
        // init pointer to input data and decoding table
        if (uc_set == 0)
        {
            pt_digit_map = &gta_digit_set0_map[0];
            puc_inp = &guca_in_msg_buff[LCD_DIGITS0_INP_IDX];
        }
        else
        {
            pt_digit_map = &gta_digit_set1_map[0];
            puc_inp = &guca_in_msg_buff[LCD_DIGITS1_INP_IDX];
        }

        for (uc_digit = 0; uc_digit < LCD_SET_DIGITS_NUM; uc_digit ++)
        {               // 5530 remove uc_digit ???
            T_SEG_MAP *pt_seg_map;

            // validate puc_inp[uc_digit]
            // ...

            // Convert digit to 7segment bitmask        
            uc_digit_mask = pgm_read_byte_near( &gta_digit_decode[*puc_inp++]);

            pt_seg_map = &(pt_digit_map->ta_seg_map[0]);

            // decode segments from mask to LCD reg bits
            while(uc_digit_mask)
            {
                if (uc_digit_mask & 1)
                {
                    uint16 us_map;
                    us_map  = pgm_read_word_near( &pt_seg_map->uc_addr);
                    *(volatile uint8 *)(us_map & 0xFF) |= (us_map >> 8);           // AV Note: LCD register address is 8bits long
                }          
                uc_digit_mask >>= 1;
                pt_seg_map++;
            }
            pt_digit_map++;

        } // End if digits for
    } // End of sets for

    //
    // -----------------------------------------------------------------
    // --- Decomposite Other signs according to received bit mask
    // -----------------------------------------------------------------
    uint8 uc_sign, uc_mask;
    T_SEG_MAP *pt_sign_map;

    pt_sign_map = &gta_signs_map[0];
    puc_inp = &guca_in_msg_buff[LCD_SIGNS_INP_IDX];

    uc_mask = 0; // AV: warning suppression

    for (uc_sign = 0; uc_sign < LCD_SIGNS_NUM; uc_sign++)
    {
        if ((uc_sign & 0x7) == 0)
            uc_mask = *puc_inp++;

        if (uc_mask & 1)
        {
            uint16 us_map;
            us_map  = pgm_read_word_near( &pt_sign_map->uc_addr);
            *(volatile uint8 *)(us_map & 0xFF) |= (us_map >> 8);           // AV Note: LCD register address is 8bits long
        }
        pt_sign_map++;
    }

    // ----------------------------------------------------------------

    // Check no more input data received
    if (guc_in_msg_idx != 17)
        FATAL_TRAP(__LINE__);

    // --- Action finished. Send feedback upward ---
    guc_out_msg_wr_idx = 2;


}

//
T_ACTION gta_action_table[64] __attribute__ ((section (".text"))) = {
    {"mark", 0x00, (void*)0xFEED        },    // Table signature
    {"sign", 0x11, action_signature     },    
    {"amux", 0x14, action_adc_mux       },    // ADC multiplexor control
    {"spiw", 0x15, action_spi_wr        },    // SPI raw write
    {"spir", 0x16, action_spi_rd        },    // SPI raw read
    {"curt", 0x41, action_curtain       },    // Laser curatain UP/DOWN
    {"laim", 0x32, action_laser_aim     },    // Laser AIM
    {"loff", 0x33, action_laser_off     },    // Laser OFF
    {"apde", 0x21, action_apd_on        },    // APD ON
    {"apdd", 0x22, action_apd_off       },    // APD OFF
    {"meas", 0x51, action_measure       },    // Start measurement
    {"lcd ", 0x61, action_lcd           },    // LCD control
    {""    , 0xFF, NULL                 }     // End of table
};

uint8 gta_digit_decode[] __attribute__ ((section (".text"))) = {

    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5)          ),      // 0x0,   0,
    (          _BV(1) | _BV(2)                                     ),      // 0x1,   1,    //      0
    ( _BV(0) | _BV(1) |          _BV(3) | _BV(4) |          _BV(6) ),      // 0x2,   2,    //    ----
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) |                   _BV(6) ),      // 0x3,   3,    //   |    |1
    (          _BV(1) | _BV(2) |                   _BV(5) | _BV(6) ),      // 0x4,   4,    //  5| 6  |
    ( _BV(0) |          _BV(2) | _BV(3) |          _BV(5) | _BV(6) ),      // 0x6,   6,    //    ----
    ( _BV(0) |          _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0x5,   5,    //   |    |2
    ( _BV(0) | _BV(1) | _BV(2)                                     ),      // 0x7,   7,    //  4|  3 |
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0x8,   8,    //    ----
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) |          _BV(5) | _BV(6) ),      // 0x9,   9,
    ( _BV(0) | _BV(1) | _BV(2) |          _BV(4) | _BV(5) | _BV(6) ),      // 0xA,   A,
    (                   _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0xB,   B,
    ( _BV(0) |                   _BV(3) | _BV(4) | _BV(5)          ),      // 0xC,   C,
    (          _BV(1) | _BV(2) | _BV(3) | _BV(4)          | _BV(6) ),      // 0xD,   D,
    ( _BV(0) |                   _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0xE,   E,
    ( _BV(0) |                            _BV(4) | _BV(5) | _BV(6) ),      // 0xF,   F,
    ( 0                                                            ),      // 0x10,  blank,
    ( _BV(0) | _BV(1) |                   _BV(4) | _BV(5) | _BV(6) ),      // 0x11,   P,
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5)          ),      // 0x12,   O,
    (          _BV(1) | _BV(2) |          _BV(4) | _BV(5) | _BV(6) ),      // 0x13,   H
    (                            _BV(3) | _BV(4) | _BV(5)          ),      // 0x14    L,
    (          _BV(1) | _BV(2)                                     ),      // 0x15    I,    
    (          _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5)          ),      // 0x16,   U,
    (                   _BV(2) |          _BV(4) |          _BV(6) ),      // 0x17,   n
    (                   _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0x18,   b
    (          _BV(1) | _BV(2) | _BV(3) | _BV(4)          | _BV(6) ),      // 0x19,   D,
    (                                     _BV(4) |          _BV(6) ),      // 0x1A,   r
    (                   _BV(2) | _BV(3) | _BV(4) |          _BV(6) ),      // 0x1B,   o
    (                   _BV(2)                                     ),      // 0x1C,   i
    (                   _BV(2) |          _BV(4) | _BV(5) | _BV(6) ),      // 0x1D,   h
    (                                                       _BV(6) ),      // 0x1E,   -
    (                            _BV(3)                            ),      // 0x1F,   _

};


#define _LA(byte, bit) {(uint8)(uint16)(&LCDDR0+byte), _BV(bit)}      

T_DIGIT_MAP gta_digit_set0_map[LCD_SET_DIGITS_NUM] __attribute__ ((section (".text"))) = 
{
    // D.pos ,    Seg0,      Seg1,      Seg2,      Seg3,      Seg4,      Seg5,       Seg6
    {4,    { _LA( 1,2), _LA( 6,1), _LA(16,1), _LA(16,2), _LA(11,2), _LA( 6,2), _LA(11,1) }},
    {3,    { _LA( 1,0), _LA( 0,7), _LA(10,7), _LA(16,0), _LA(11,0), _LA( 6,0), _LA( 5,7) }},
    {2,    { _LA( 0,6), _LA( 0,5), _LA(10,5), _LA(15,6), _LA(10,6), _LA( 5,6), _LA( 5,5) }},
    {1,    { _LA( 0,4), _LA( 5,3), _LA(15,3), _LA(15,4), _LA(10,4), _LA( 5,4), _LA(10,3) }},
    {0,    { _LA( 0,2), _LA( 5,1), _LA(15,1), _LA(15,2), _LA(10,2), _LA( 5,2), _LA(10,1) }},
};

T_DIGIT_MAP gta_digit_set1_map[LCD_SET_DIGITS_NUM] __attribute__ ((section (".text"))) = 
{  // D.pos ,    Seg0,      Seg1,      Seg2,      Seg3,      Seg4,      Seg5,       Seg6
    {4,    { _LA(16,5), _LA(16,6), _LA( 6,6), _LA( 1,5), _LA( 6,5), _LA(11,5), _LA(11,6) }},
    {3,    { _LA(16,7), _LA(17,0), _LA( 7,0), _LA( 1,7), _LA( 6,7), _LA(11,7), _LA(12,0) }},
    {2,    { _LA(17,1), _LA(17,2), _LA( 7,2), _LA( 2,1), _LA( 7,1), _LA(12,1), _LA(12,2) }},
    {1,    { _LA(17,3), _LA(17,4), _LA( 7,4), _LA( 2,3), _LA( 7,3), _LA(12,3), _LA(12,4) }},
    {0,    { _LA(17,5), _LA(17,6), _LA( 7,6), _LA( 2,5), _LA( 7,5), _LA(12,5), _LA(12,6) }},
};

T_SEG_MAP gta_signs_map[LCD_SIGNS_NUM] __attribute__ ((section (".text"))) = {
   _LA(15,5),  // dec. point 0 set 0      //     0 
   _LA(15,7),  // dec. point 1 set 0      //     1 
   _LA(15,0),  // set 0 in meters         //     2 
   _LA(10,0),  // set 0 in foots          //     3 
                                               
   _LA( 2,4),  // dec. point 0 set 1      //     4 
   _LA( 2,2),  // dec. point 1 set 1      //     5 
   _LA( 2,0),  // dec. point 2 set 1      //     6 
   _LA(12,7),  // set 1 in meters         //     7 
   _LA(13,0),  // set 1 in foots          //     8 
                                             
   _LA( 5,0),  // ref point 0             //     9 
   _LA( 0,0),  // ref point 1             //    10
   _LA( 3,0),  // ref point 2             //    11
   _LA( 8,0),  // ref point 3             //    12
                                               
   _LA( 2,7),  // case                    //    13
   _LA( 7,7),  // Laser enable            //    14
   _LA( 2,6),  // temperature             //    15
   _LA( 0,1),  // Baterry                 //    16
   _LA( 0,3),  // continous measure       //    17
                                               
   _LA( 1,1),  // cube front bott         //    18
   _LA( 1,4),  // cube front right        //    19
   _LA( 1,3),  // cube front diag         //    20
   _LA( 6,3),  // cube front top left     //    21
   _LA( 1,6),  // cube bott right         //    22
   _LA( 6,4),  // cube back other         //    23
                                             
   _LA(16,3),  // M                       //    24
   _LA(16,4),  // equal top               //    25
   _LA(11,3),  // equal bott              //    26
   _LA(11,4),  // plus                    //    27
   _LA(17,7),  // "2"                     //    28
   _LA(18,0),  // "3"                     //    29
   _LA(18,0),  // not in use              //    30
   _LA(18,0),  // not in use              //    31
};

