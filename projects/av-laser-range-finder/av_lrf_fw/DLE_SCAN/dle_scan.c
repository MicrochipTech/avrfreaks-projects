/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan.c
**                                        
** PROJECT      :   DLE Scan
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
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h> 
#include "dle_typedef.h"
#include "dle_scan_meas.h"
#include "dle_scan.h"
#include "dle_scan_lcd.h"
#include "dle_scan_act.h"
#include "dle_scan_keyb.h"

// -------------------------------------------------
// --- Fuse settings:
// ---  1111 1011 0001 0001 1010 0000 = 0xFD118E
// ---       ^^^^ ^^^^ ^^^^ ^^^^ ^^^^   
// ---         || |||| |||| || |    |            
// ---         || |||| |||| || |    |            
// ---         || |||| |||| || |    +-- CKSEL3..0  (0000) Full swing 16Mhz, 
// ---         || |||| |||| || +------- SUT1..0      (10)
// ---         || |||| |||| |+--------- CKOUT         (0) PORTE7 as clock output
// ---         || |||| |||| +---------- CKDIV8        (1)
// ---         || |||| ||||                                        
// ---         || |||| |||+------------ BOOTRST       (1)
// ---         || |||| ||+------------- BOOTSZ0       (0)
// ---         || |||| |+-------------- BOOTSZ1       (0)
// ---         || |||| +--------------- EESAVE        (0)
// ---         || ||||
// ---         || |||+----------------- WDTON         (1)
// ---         || ||+------------------ Enable SPI    (0) Enabled
// ---         || |+------------------- Enable JTAG   (0) Enabled
// ---         || +-------------------- Enable OCD    (0) Enabled
// ---         ||
// ---         |+-----------------------RSTDISBL      (1) Reset enable
// ---         +------------------------BODLEVEL    (101) 2.5V..2.7V..2.9V
// ---

//------ Global variables ---------------------
HW_INFO gt_hw_info = {0, 0, "AV DLE Scan 09/05/2011"};
                                
volatile uint16 gus_trap_line;
uint16 gusa_trace[TRACE_SIZE];

uint8 guc_30usec_cnt;
volatile uint8 guc_240usec_timer;

uint16 gus_meas_timer;
uint16 gus_keyb_timer;

uint8 guca_spi_curr_state[SPI_MAX_LEN]; 

volatile uint8 guc_in_msg_idx;                                                    
uint8 guca_in_msg_buff[MSG_BUFF_SIZE];

volatile uint8 guc_out_msg_wr_idx; 
volatile uint8 guc_out_msg_rd_idx;
uint8 guca_out_msg_buff[MSG_BUFF_SIZE];

void command_lookup();
void short_cmd_lookup();

#define INT_FATAL_TRAP(us_line_num)     \
    {                                   \
        gus_trap_line = us_line_num;    \
        /* Clear hardware hazard */     \
        /* ... */                       \
        while(1);                       \
    }

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Initializes timer2 in order to generate PWM signal on 
**                  PB7/OC2A pin. T = 30us, D ~= 3.76% (1.128us)
**                  CTC mode T=30us, pulse generation & width control 
**                  is hardcoded into ISR
**
***C*F*E**********************************************************************/
void init_apd_bias_pwm(){
 

    OCR2A = (30*8 - 1);     // Timer period = 30usec @ 8MHz

    TIMSK2 = (1<<OCIE2A);   // Compare match interrupt enable

    TIFR2 = (1<<OCF2A);     // Clear compare match interrupt flag
         
    TCCR2A = 
        (1<<WGM21 )|        // CTC operation mode
        (0<<WGM20 )|
        (0<<COM2A0)|        // OC2A disconnected
        (1<<CS20  );        // No prescaller

    PORTB |= (1<<APD_BIAS_PWM_PIN); 
    DDRB  |= (1<<APD_BIAS_PWM_PIN);

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Init UART to 76800 8N1
**
***C*F*E**********************************************************************/
void init_uart(){

    // Power Enable. Just in case. Enabled at startup
    PRR &= ~(1<<PRUSART0);


    // Set baud rate. UBRR = Fosc/16/BAUD - 1

    // 0.5Mbps
    // Fosc = 8MHz, UBRR = 8*10^6/16/500000 - 1 = 0
    // UBRR0H = 0;
    // UBRR0L = 0;

    // 76800
    UBRR0H = 0;
    UBRR0L = 12;
    UCSR0A |= (1<<U2X0);

    // Set frame format: 8data, 1stop bit */
    UCSR0C = 
        (0<<UPM00) |      // No parity
        (0<<USBS0) |      // 1 stop bit
        (3<<UCSZ00);      // 8 data bits

    // Enable receiver and transmitter
    UCSR0B =
        (1<<RXCIE0)|     // RX Interrupt enable
        (0<<TXCIE0)|     // TX Interrupt disable
        (1<<RXEN0) |     // Receiver enabled
        (1<<TXEN0);      // Transmitter enabled
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Init USI in SPI 0 Master Mode
**                  The USI interface is used to implement SPI communication
**                  with onboard modulator
** 
***C*F*E**********************************************************************/
void init_spi(){

    // Set DO, SCK, CS pins as outputs
    SPI_DIR |= 
        _BV(SPI_DO_PIN) |
        _BV(SPI_CS_PIN) |
        _BV(SPI_CLK_PIN);

    // Set DI pin as input 
    SPI_DIR &= 
        ~_BV(SPI_DI_PIN);

    // Pull up DI pin, CS set to inactive (high)
    SPI_PORT |= 
        _BV(SPI_DI_PIN) |
        _BV(SPI_CS_PIN);

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :  ADC always running at fixed frequency 250kHz. ADC sample  
**                 rate, however, is set by timer1 period (60usec or 16.6kHz).
**                 ADC continously generates ADC complete interrupt and send 
**                 ADC data to UART.
**
***C*F*E**********************************************************************/
void init_adc(){

    // ------------------------------------------------------
    // --- Configure ADC 
    // ------------------------------------------------------
    ADCSRA = 0;         // Disable ADC. Just in case.

    // Power Enable. Just in case. Enabled at startup
    PRR &= ~(1<<PRADC);

    // Selects auto triger source
    ADCSRB =
        (5<<ADTS0 ) |   // 0->Free running mode; 5->compare match 1B
        (0<<ACME  );    // Comparator disabled

    // Disable digital input circuit on ADC pins
    DIDR0 = 
        (1<<ADC0D) |    // APD signal
        (1<<ADC1D) |    // Battery
        (1<<ADC2D) |    // APD BIAS mon
        (1<<ADC3D);     // APD preampl mon

    ADMUX = 
        (0 << MUX0 ) |  // 0x1E -> 1.1V Vbg; 0 -> ADC0
        (0 << REFS0);   // 0 - External AREF (AVCC 3.3V)

    ADCSRA = 
        (0<<ADEN ) |    // ADC Disabled
        (1<<ADSC ) |    // Sart very first conversion
        (1<<ADATE) |    // Auto trigger enabled
        (1<<ADIE ) |    // ADC interrupt enabled
        (1<<ADIF ) |    // Clear ADC complete interrupt
        (5<<ADPS0);     // ADC clock  5->250kHz @ 8MHz

    // ------------------------------------------------------
    // --- Configure Timer 1 
    // ------------------------------------------------------

    // Timer 1 set ADC sample rate (OCR1A) and phase (OCR1B)
    // CTC mode, clear TCNT on OCR1A match, full throttle, no interrupt

    // WGM1 3:0 = 0100 => CTC
    TCCR1A = (0 << WGM10);

    TCCR1B = (1 << CS10) |  // clock prescaler. 1=>1/1, 
             (1 << WGM12);  // CTC mode

    // 60usec@8MHz no perscaler
    OCR1AH = 1;     // 60*8>>8
    OCR1AL = 223;   // 60*8 mod 256  - 1

    // 50% phase shift relative to OCR1A = 239
    // ADC starts conversion at OCR1B interrupt
    OCR1BH = 0;    // 
    OCR1BL = 8*8;  //  Assuming timers are synchronized.

    // Enable OCR1B interrupt
    TIMSK1 = (1 << OCIE1B);

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   The routine sends predefined SPI commands to modulator
**                  in order to asure the same phase shift on each start
**
***C*F*E**********************************************************************/
static void resync_synthesizer()
{
    uint8 uc_tmp;

    SPI_PORT &= ~_BV(SPI_CS_PIN);
    SEND_SPI_BYTE(uc_tmp, 0x06);
    SEND_SPI_BYTE(uc_tmp, 0xF5);
    SPI_PORT |= _BV(SPI_CS_PIN);

    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);

    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);

    SPI_PORT &= ~_BV(SPI_CS_PIN);
    SEND_SPI_BYTE(uc_tmp, 0x06);
    SEND_SPI_BYTE(uc_tmp, 0xF4);
    SPI_PORT |= _BV(SPI_CS_PIN);
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   The functions checks communication input & output buffers
**                  Send out command responses when available, and parse input 
**                  command upon arrival
**
***C*F*E**********************************************************************/
 void dle_scan_io()
 {
    // Get number of bytes received (2 bytes at least)
    if (guc_in_msg_idx >= 2)
    {
        // short commands during scan only (state idle)
        if (gt_meas_info.e_state == MEAS_STATE_IDLE)
        {
            short_cmd_lookup();
        }
        else if (gt_meas_info.e_state == MEAS_STATE_DISABLED)
        { // full commands allowed only if scan disabled
            // Init transmitter's buffer
            guc_out_msg_wr_idx = 0;
            guc_out_msg_rd_idx = 0;

            // Find and execute command from the ACTIONS TABLE
            command_lookup();
        }

        // Clear RX buffer index in order to receive next message
        guc_in_msg_idx = 0;
    }

    // Is something to send
    if (guc_out_msg_wr_idx > guc_out_msg_rd_idx)
    {
        // check is current byte is send completely
        if (UCSR0A & (1 << UDRE0))
        { // TX FIFIO is empty. 

            UDR0 = guca_out_msg_buff[guc_out_msg_rd_idx];
            UCSR0A  |= (1 << TXC0); // Clear transmit complete flag
            guc_out_msg_rd_idx ++;
        }
    }
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Entry point
**
***C*F*E**********************************************************************/
int main() {

    uint8 uc_prev_30usec_cnt, uc_curr_30usec_cnt;

    // --------------------------------------------------
    // --- Init perephirial devices
    // --------------------------------------------------
    { uint8 uc_i;
        // Wait a little just in case
        for(uc_i = 0; uc_i < 255U; uc_i++){

            __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"\
                                  "    nop\n    nop\n    nop\n    nop\n"\
                                  "    nop\n    nop\n    nop\n    nop\n"\
                                  "    nop\n    nop\n    nop\n    nop\n"
                                    ::);
        }
    }

    // Hold the power switch (drive 1) (wired on PCB. #POWER_HOLD (TP2) connected directly to GND)
    // uncomment to control power_on/power_off by MCU
    //PORTB |= _BV(POWER_HOLD_PIN);
    //DDRB  |= _BV(POWER_HOLD_PIN);

    // pull up laser feedback pd
    PORTE |= _BV(LASER_FB_PD_PIN);
    DDRE  &= ~_BV(LASER_FB_PD_PIN);

    // Normally these pins are used by LCD display
    DBG_PORT |= 
        (1 << DBG0_PIN  )|
        (1 << DBG1_PIN  );

    DBG_DIR |= 
        (1 << DBG0_PIN)|
        (1 << DBG1_PIN);

    // Init global timer counter
    guc_30usec_cnt = 0;

    init_apd_bias_pwm();

    init_uart();

    init_spi();

    init_lcd();

    init_adc();

    init_keyb();

    init_measure();

    // Syncronize timers
    GTCCR = (1 << TSM) | (1 << PSR2 ) | (1 << PSR10);
    TCNT1H = 0;
    TCNT1L = 0;
    TCNT2 = 0;
    GTCCR = 0;

    gus_trap_line = 0;

    uc_prev_30usec_cnt = guc_30usec_cnt;

    gus_keyb_timer = 0;
    gus_meas_timer = 0;

    // Enable Interrupts
    sei();

    while(1)
    {
        // Update time since last measurements in 30usec ticks
        uint16 us_time_delta;
        uc_curr_30usec_cnt = guc_30usec_cnt;
        us_time_delta = (uc_curr_30usec_cnt - uc_prev_30usec_cnt);
        if (uc_curr_30usec_cnt < uc_prev_30usec_cnt)
        {
            us_time_delta += 0x100;
        }
        uc_prev_30usec_cnt = uc_curr_30usec_cnt;

        gus_meas_timer += us_time_delta;
        gus_keyb_timer += us_time_delta;

        // Measurement state machine
        dle_scan_fsm();

        // Command & response processing routine
        dle_scan_io();

        // keyboard processing
        if (gus_keyb_timer >= KEYB_SCAN_PERIOD)
        {
            keyb_scan();
            gus_keyb_timer = 0;
        }

        // LCD processing 
        if (guc_lcd_redraw)
        {
            lcd_render();
            guc_lcd_redraw = 0;
        }

    }

    return 0;
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Just a trap. Normally code should never hit here
**
***C*F*E**********************************************************************/
void FATAL_TRAP(uint16 us_line_num){

    uint8 uc_tmp;
    gus_trap_line = us_line_num;

    // clear all hazards on external pins
    init_keyb();

    // remove curtain driver signal
    CURTAIN_PORT |= 
        (1 << CURTAIN_UP_PIN  )|
        (1 << CURTAIN_DOWN_PIN);

    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);    
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);    

    // Disable curtain IO pins by enabling JTAG
    uc_tmp = MCUCR & ~(1 << JTD);
    MCUCR = uc_tmp;
    MCUCR = uc_tmp;

    while(1)
    {
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);    
    }

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Short commands processing routine
**                  The shorts commands are the commands executed during scaning.
**                      MEAS_APD    - modifies APD value used while measurements
**                      CALIB_APD   - modifies APD value used while calibration
**                      CALIBRATION - forces calibration. 
**                      MEAS_STOP   - Stop scanning
**
***C*F*E**********************************************************************/
void short_cmd_lookup()
{
    uint8 uc_cmd;
    uint16 us_apd;

    uc_cmd = guca_in_msg_buff[0] & 0x0F;
    us_apd = ((guca_in_msg_buff[0] & 0xF0) << 8) | guca_in_msg_buff[1];

    if (uc_cmd == 0x01)
    { // Measure APD
        gt_meas_info.us_meas_bias_ctrl = us_apd;
    }
    else if (uc_cmd == 0x02)
    { // calibration ADP
        gt_meas_info.us_calib_bias_ctrl = us_apd;
    }
    else if (uc_cmd == 0x03)
    { // Calibrate

        if (us_apd != 0)
        {
            gt_meas_info.uc_calibration = 1;
            gt_meas_info.us_calib_bias_ctrl = us_apd;
        }
        else
        {
            gt_meas_info.uc_calibration = 1 - gt_meas_info.uc_calibration;
        }
    }
    else if (uc_cmd == 0x04)
    { // Stop
#if CURTAIN_ENABLED
        uint8 uc_tmp;
        // ACT=2. Disable curtain IO pins by enabling JTAG
        uc_tmp = MCUCR & ~(1 << JTD);
        MCUCR = uc_tmp;
        MCUCR = uc_tmp;

        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
        __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n"::);
#endif

        gt_meas_info.e_state = MEAS_STATE_DISABLED;

        disable_modulator();

        guca_lcd_digits_set1[0] = 0x1E;    
        guca_lcd_digits_set1[1] = 0x1E;    
        guc_lcd_redraw = 1;

        // ??? send stop response ???
        // ...
    }
    else
    { // Invlid short command
        FATAL_TRAP(__LINE__);
    }
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Lookup incoming command in actions table and
**                  execute it if found.
**
***C*F*E**********************************************************************/
void command_lookup(){

    T_ACTION    *pt_action;
    PF_PVOID    pf_func;

    uint8 uc_act_cmd;
    uint8 uc_msg_cmd;
    
    // check is command valid
    // 1st byte must be 0xD5, read pointer must be set on buffer start
    if ( guca_in_msg_buff[0] != 0xD5)
        FATAL_TRAP(__LINE__);

    uc_msg_cmd = guca_in_msg_buff[1];

    // --------------------------------------------------
    // --- Find received command in actions' table
    // --------------------------------------------------
    pt_action = gta_action_table;

    // Check functions table signature
    pf_func = (PF_PVOID)pgm_read_word_near(&pt_action->pf_func);
    if (pf_func != (PF_PVOID)0xFEED)
        FATAL_TRAP(__LINE__);

    // Find action in table
    do{
        pt_action ++;
        uc_act_cmd = pgm_read_byte_near(&pt_action->uc_cmd);

        // TRAP if unknown command received (end of table reached)
        if (uc_act_cmd == 0xFF) 
            FATAL_TRAP(__LINE__);

        if (uc_msg_cmd == uc_act_cmd){
            // get functor from table
            pf_func = (PF_PVOID)pgm_read_word_near(&pt_action->pf_func);
            break;
        }
    }while(1); // End of action table scan

    pf_func();

    return;

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Interrupt Service routine
**                  save data incoming through UART interface
**
***C*F*E**********************************************************************/
ISR(USART0_RX_vect) {

    uint8 uc_status;
    uint8 uc_data;

    // get RX status, trap if something wrong
    uc_status = UCSR0A;
    uc_data = UDR0;

    // Check Frame Error (FE0) and DATA overflow (DOR0)
    if ( uc_status & (_BV(FE0) | _BV(DOR0)) )
        FATAL_TRAP(__LINE__);

    if ( guc_in_msg_idx == MSG_BUFF_SIZE )
        FATAL_TRAP(__LINE__);

    guca_in_msg_buff[guc_in_msg_idx++] = uc_data;
  
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Timer2 Interrupt Service routine
**                  Generates PWM signal for APD booster. 
**                  Generate negative 1.128 usec pulse (9cycles)
**
***C*F*E**********************************************************************/
ISR(TIMER2_COMP_vect){

    PORTB &= ~(1<<APD_BIAS_PWM_PIN); 
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n    nop\n" ::);
    __asm__ __volatile__ ("    nop\n    nop\n    nop\n" ::);
    PORTB |= (1<<APD_BIAS_PWM_PIN); 

    guc_30usec_cnt++;
    if ((guc_30usec_cnt & 0x7) == 0)
    {
        guc_240usec_timer--;
    }

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   ADC Interrupt Service routine
**                  accumulate samples during measurement    
**
***C*F*E**********************************************************************/
ISR(ADC_vect){

    uint8 uc_data_h, uc_data_l;
    uint8 uc_sampl_cnt;

    // get samlpes ASAP. 6usec to get samplpes from ADC (best case @250kHz)
    uc_data_l = ADCL;
    uc_data_h = ADCH;

    TIFR1 = (1 << OCF1B);   // Clear OCR1B interrupt flag. Requried by ADC auto triger mode

    if (gt_meas_info.e_state == MEAS_STATE_SYNC)
    {
        gt_meas_info.e_state = MEAS_STATE_SYNC1;
        return;
    }

    if (gt_meas_info.e_state == MEAS_STATE_SYNC1)
    {
        gt_meas_info.e_state = MEAS_STATE_SAMPLING;

        resync_synthesizer();

        gt_meas_info.usa_acc[0] = 0;
        gt_meas_info.usa_acc[1] = 0;
        gt_meas_info.usa_acc[2] = 0;
        gt_meas_info.usa_acc[3] = 0;

        return;
    } // End of SYNC state
    
    if (gt_meas_info.e_state == MEAS_STATE_SAMPLING)
    {
        uc_sampl_cnt = gt_meas_info.uc_freq_sampl_cnt;

        // Is measurement in progress
        if (uc_sampl_cnt == 0)
        { // End of measure
             ADCSRA &= ~(1<<ADEN);                       // Disable ADC
             gt_meas_info.e_state = MEAS_STATE_READY;
             return;
        }

        if (gt_meas_info.uc_freq_sampl_delay != 0)
        {
            gt_meas_info.uc_freq_sampl_delay --;
            return;
        }

        // Continue ADC sampling
        gt_meas_info.usa_acc[(4 - uc_sampl_cnt) & 0x03] += (uc_data_h << 8) | uc_data_l;  // -4 can be omitted by swaping samples while SIN calculation

        gt_meas_info.uc_freq_sampl_cnt = uc_sampl_cnt-1;
   
    } // End of SAMPLING state

} // End of ADC interrupt


ISR(TIMER1_COMPB_vect){

//    DBG_PORT ^= (1 << DBG1_PIN);

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   SPI helper functions
**
***C*F*E**********************************************************************/
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

void spi_read_curr(){

    uint8 uc_feedback, uc_i;

    SPI_PORT &= ~_BV(SPI_CS_PIN);

    SEND_SPI_BYTE(uc_feedback, 0x80);

    // Send raw data, get feedback and write to message
    for (uc_i = 0; uc_i < SPI_MAX_LEN; uc_i++){
        SEND_SPI_BYTE(uc_feedback, 0);
        guca_spi_curr_state[uc_i] = uc_feedback;
    }

    SPI_PORT |= _BV(SPI_CS_PIN); 
}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Rise APD bias value 1 by 1 till the specified value
**                  Single SPI transaction is sent for every modify iteration
**                  
***C*F*E**********************************************************************/
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
/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Down APD bias value 1 by 1 till the specified value
**                  Single SPI transaction is sent for every modify iteration
**                  
***C*F*E**********************************************************************/
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

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Compare current state of APD BIAS control value with one 
**                  from global measurement info structure and if not equal 
**                  riseup or pulldown current bias value step-by-step.
**
***C*F*E**********************************************************************/
void apd_control()
{ 
    uint16 us_bias_ctrl_curr;
    uint16 us_bias_ctrl;

    if (gt_meas_info.uc_calibration)
    {
        us_bias_ctrl = gt_meas_info.us_calib_bias_ctrl;
    }
    else
    {
        us_bias_ctrl = gt_meas_info.us_meas_bias_ctrl;
    }

    // Read SPI current state
    spi_read_curr();

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


