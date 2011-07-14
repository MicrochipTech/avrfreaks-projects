/***C*********************************************************************************************
**
** SRC-FILE     :   dle_ctrl.c
**                                        
** PROJECT      :   DLE Control
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Initialization code for DLE device, interrupts & communication routines
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2010
**
****C*E******************************************************************************************/
/*
    TODO: double check PWM duty cycle impact on APD BIAS
*/

/*
Add building to VS project
set PATH=%PATH%;%WinAVR_HOME%\utils\bin\
set PATH=%PATH%;%WinAVR_HOME%\bin\
set WinAVR_HOME=C:\Program Files\Atmel\WinAVR-20071221
*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h> 
#include "dle_actions.h"
#include "dle_ctrl.h"


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
HW_INFO gt_hw_info = {0, 2, "AV DLE Control"};

uint16 gus_trap_line;

#define TRACE_SIZE  16
uint16 gusa_trace[TRACE_SIZE];

uint8 guca_dbg_buff[DBG_BUFF_SIZE];
uint8 guc_dbg_buff_idx;

uint8 guc_30usec_cnt;
volatile uint8 guc_240usec_timer;

uint16 gus_acc[SAMPLES_PER_PERIOD];

PF_PVOID gpf_action_func;

void command_lookup();
void wait_for_command(uint8 uc_msg_len);

#define INT_FATAL_TRAP(us_line_num)     \
    {                                   \
        gus_trap_line = us_line_num;    \
        /* Clear hardware hazard */     \
        /* ... */                       \
        while(1);                       \
    }


volatile uint8 guc_delay_cnt;
volatile uint8 guc_measure_cnt;
volatile uint8 guc_adc_state;
volatile uint8 guc_adc_stream_en;

void activate_adc_isr(uint8 uc_adc_stream_en){

    // init measurement & delay counter
    guc_delay_cnt = DELAY_SAMPLES_NUM;
    guc_measure_cnt = 1 << MEAS_SAMPLES_P;
    guc_adc_state = ADC_STATE_SYNC;
    guc_adc_stream_en = uc_adc_stream_en;

    // Enable ADC 
    ADCSRA |= (1<<ADEN ) | (1 << ADIF);

    // Clear interrupt flag (must be cleared for ADC autostart facility)
    TIFR1 = (1 << OCF1B);
}

//------- Internal function declaration -------
void init_apd_bias_pwm(){
    
    // Generate PWM signal on PB7/OC2A pin. T = 30us, D ~= 3.76% (1.128us)
    // CTC mode T=30us, pulse generation & width control is hardcoded into ISR, 

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

void init_uart(){

    // Init UART to 0.5Mbps 8N1

    // Power Enable. Just in case. Enabled at startup
    PRR &= ~(1<<PRUSART0);

    // Set baud rate. UBRR = Fosc/16/BAUD - 1
    // Fosc = 8MHz, UBRR = 8*10^6/16/500000 - 1 = 0
    UBRR0H = 0;
    UBRR0L = 0;

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

void init_lcd(){

//LCDFFR 0x65
//LCDCCR 0x17
//LCDCRB 0x37
//LCDCRA 0x80

    // Disable LCD. Just in case...
    LCDCRA = 0;

    // 
    LCDFRR = 
        (5 << LCDCD0) |         // LCD Clock Divide (N+1)
        (6 << LCDPS0);          // 6 -> clkLCD/2048

    // Contrast = 100%
    LCDCCR = 
        (1 << LCDMDT);

    // 
    LCDCRB = 
        (3 << LCDMUX0) |     // DUTY = 1/4; BIAS = 1/2 or 1/3; COM0:3
        (7 << LCDPM0);       // LCD port mask SEG0:24 25

    // Enable LCD
    LCDCRA = 
        (1 << LCDEN);

    // Set initial picture
    LCDDR2  = 0x80;
    LCDDR5  = 0xA0;
    LCDDR10 = 0x0A;
    LCDDR11 = 0x02;

}

void init_spi(){

    // The USI interface is used to implement SPI communication
    // Init USI in SPI 0 Master Mode

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
void init_adc(){

    // Note: ADC always running at fixed frequency 250kHz. ADC sample rate, however, 
    //       is set by timer1 period (60usec or 16.6kHz).
    //       ADC continously generates ADC complete interrupt and send ADC data to UART.

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
    OCR1BH = 0;     // 
    OCR1BL = 8*8;  //  Assuming timers are synchronized.

    // Enable OCR1B interrupt
    TIMSK1 = (1 << OCIE1B);

}

//---------------------------------------------
int main() {

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

    gusa_trace[0] = 0;
    gusa_trace[1] = 0;
    gusa_trace[2] = 0;
    gusa_trace[3] = 0;
    gusa_trace[4] = 0;

    guc_dbg_buff_idx = 0;

    init_apd_bias_pwm();

    init_uart();

    init_spi();

    init_lcd();

    init_adc();

    // Syncronize timers
    GTCCR = (1 << TSM) | (1 << PSR2 ) | (1 << PSR10);
    TCNT1H = 0;
    TCNT1L = 0;
    TCNT2 = 0;
    GTCCR = 0;

    gus_trap_line = 0;

    // Enable Interrupts
    sei();

    while(1)
    {

        // Get number of bytes received (2 bytes at least)
        if (guc_in_msg_idx >= 2)
        {
            // Init transmitter's buffer
            guc_out_msg_wr_idx = 0;
            guc_out_msg_rd_idx = 0;

            // Find and execute command from the ACTIONS TABLE
            command_lookup();
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

        // main FSM 

        // check error condition
        // ...

        // keyboard processing
        // ...

        // LCD processing ??? or move directly to the dedicated action ???
        // ...
    }

    return 0;
}

void FATAL_TRAP(uint16 us_line_num){

    gus_trap_line = us_line_num;

    // clear all hazards on external pins
    // ...

    while(1);

}

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
            gpf_action_func = (PF_PVOID)pgm_read_word_near(&pt_action->pf_func);
            break;
        }
    }while(1); // End of action table scan

    gpf_action_func();

    // Clear RX buffer index in order to receive next message
    guc_in_msg_idx = 0;

    return;

}

ISR(USART0_RX_vect) {

    uint8 uc_status;
    uint8 uc_data;

    // get RX status, trap if something wrong
    uc_status = UCSR0A;
    uc_data = UDR0;

    // Check Frame Error (FE0) and DATA overflow (DOR0)
    if ( uc_status & (_BV(FE0) | _BV(DOR0)) )
        INT_FATAL_TRAP(__LINE__);

    if ( guc_in_msg_idx == MSG_BUFF_SIZE )
        INT_FATAL_TRAP(__LINE__);

    guca_in_msg_buff[guc_in_msg_idx++] = uc_data;
  
}

ISR(TIMER2_COMP_vect){

    // Generates PWM signal for APD booster. 
    // Generate negative 1.128 usec pulse (9cycles)
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


ISR(ADC_vect){


    uint8 uc_data_h, uc_data_l;

    // get samlpes ASAP. 6usec to get samplpes from ADC (best case @250kHz)
    uc_data_l = ADCL;
    uc_data_h = ADCH;

    TIFR1 = (1 << OCF1B);   // Clear OCR1B interrupt flag. Requried by ADC auto triger mode

    if (guc_adc_state == ADC_STATE_SYNC)
    {
        guc_adc_state = ADC_STATE_SYNC1;
        return;
    }

    if (guc_adc_state == ADC_STATE_SYNC1)
    {
        guc_adc_state = ADC_STATE_SAMPLING;

        // Resync modulator
        SPI_PORT &= ~_BV(SPI_CS_PIN);
        SEND_SPI_BYTE(uc_data_l, 0x06);
        SEND_SPI_BYTE(uc_data_l, 0xF5);
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
        SEND_SPI_BYTE(uc_data_l, 0x06);
        SEND_SPI_BYTE(uc_data_l, 0xF4);
        SPI_PORT |= _BV(SPI_CS_PIN);

        gus_acc[0] = 0;
        gus_acc[1] = 0;
        gus_acc[2] = 0;
        gus_acc[3] = 0;

        gusa_trace[2]++;

        return;
    } // End of SYNC state
    

    if (guc_adc_state == ADC_STATE_SAMPLING)
    {
        // Is measurement is in progress
        if (guc_measure_cnt == 0)
        {
            // Disable ADC
             ADCSRA &= ~(1<<ADEN);
             return;
        }

        if (guc_delay_cnt != 0)
        {
            guc_delay_cnt --;
            return;
        }

        // Continue ADC sampling
        gus_acc[(4 - guc_measure_cnt) & 0x03] += (uc_data_h << 8) | uc_data_l;  // -4 can be omitted by swaping samples while SIN calculation

        // Write ADC sample to UART if ADC stream is enabled
        if (guc_adc_stream_en)
        {
            // Check previous transaction is completed (TXC: USART Transmit Complete)
            if (UCSR0A & (1 << TXC0))
            { // TX FIFIO is empty. Write 16bit ADC sample to UART TX FIFO
                UDR0 = uc_data_l;
                UDR0 = uc_data_h;
                UCSR0A  |= (1 << TXC0); // Clear transmit complete flag
            }
            else
            {
                // if USART TX is busy, then increment overload counter
                gusa_trace[1]++;
            }
        }// End of ADC stream enable            

        if ((guc_measure_cnt & 0x03) == 0)
        {
            DBG_PORT ^= (1 << DBG1_PIN);
        }
        guc_measure_cnt --;
    

    } // End of SAMPLING state


} // End of ADC interrupt



ISR(TIMER1_COMPB_vect){


//    DBG_PORT ^= (1 << DBG1_PIN);

}

