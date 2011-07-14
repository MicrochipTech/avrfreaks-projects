/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_meas.c
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   2011-05-09
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Measurement Finite State Machine (FSM) and distance calculation routines
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h> 
#include <stdlib.h>
#include "dle_typedef.h"
#include "dle_scan.h"
#include "dle_scan_lcd.h"
#include "dle_scan_meas.h"

#define MEAS_SAMPLES_P          5                           // 2^P Number of samples for measurement 
#define MEAS_DELAY_SAMPLES_NUM  16                          // Delay before measure start            
#define MEAS_ACC_SCALE          (MEAS_SAMPLES_P - 2)   
#define MEAS_RND_CONST          (1 << (MEAS_ACC_SCALE - 1))

#define CURTAIN_DELAY 2

#define LCD_PROGRESS_REFRESH_CNT 8
#define LCD_PROGRESS_TICKER_NUM 8
const uint8 gca_progress_ticker0[LCD_PROGRESS_TICKER_NUM] __attribute__ ((section (".text"))) = { 0x26,  0x25, 0x24, 0x20, 0x20, 0x20, 0x20, 0x21 };
const uint8 gca_progress_ticker1[LCD_PROGRESS_TICKER_NUM] __attribute__ ((section (".text"))) = { 0x20,  0x20, 0x20, 0x24, 0x23, 0x22, 0x21, 0x20 };
uint8 guc_lcd_meas_progress;
uint8 guc_lcd_meas_refresh_cnt;


#define PWR_MVTONUM(mv) (((mv/10)*(mv/10))*6/10)
// sqrt(range)*(2^SCALE)/2*3.22 -> mV
// ((mV*2/3.22)/8).^2 -> range
const uint16 gusa_pwr_range[E_PWR_RANGE_NUM-1] = { PWR_MVTONUM(100),   PWR_MVTONUM(135),  PWR_MVTONUM(350),  PWR_MVTONUM(600L),   PWR_MVTONUM(1300L)};

const uint8 guca_apd_range[APD_RANGE_NUM] = { 0x10, 0x20, 0x30, 0x40, 0x4C, 0x50 };

int8 gca_apd_auto_tune[E_PWR_RANGE_NUM][APD_RANGE_NUM] = {
    { +4,  +2, +2, +2, +1, -32},        //      < 100   Extremly low  - rapid rise
    { +2,  +2, +1, +1, +1, -32},        // 100..  135  Low           - slow rise
    { +0,  +0, +0, +0,  0,  -2},        // 135..  350  Normal        - Keep
    { -1,  -1, -1, -1, -2,  -2},        // 350..  600  high          - slow drop
    { -2,  -2, -2, -2, -2,  -4},        // 600.. 1300  Extremly high - rapid drop
    { -64, -64, -64, -64, -64,  -64},   // >1300       Overl Votage  - roll over
};

T_MEAS_INFO gt_meas_info;

T_FREQ_DATA gta_freq_table[] = {

    {0x37E4, 309, 39488, 0, 0 },    
    {0x1EE4, 311, 39782, 0, 0 },
    {0xC9D5, 319, 40781, 0, 0 },
    {0xA9D5, 321, 41139, 0, 0 },
    {0xD3B9, 371, 47539, 0, 0 },
    {0, 0, 0}
};


T_DUAL_FREQ_MEAS gta_dual_meas_s0[ROUGH_DIST_1_NUM] = 
{
                        // rought distance pairs stage 0                                  
                        // #  L1(mm)     L2(mm)     T(m)   1/T
    {1, 3,  9424, 0},   // 0 310.8(1)   321.4(3)    9.4       
    {1, 2, 12695, 0},   // 1 310.8(1)   318.6(2)	12.7
    {0, 3,  7686, 0},   // 2 308.5(0)   321.4(3)    7.7
    {0, 2,  9731, 0},   // 3 308.5(0)   318.6(2)    9.7
};

T_DUAL_FREQ_MEAS gta_dual_meas_s2[ROUGH_DIST_2_NUM] = 
{
                        // rought distance pairs stage 0                                  
                        // #  L1(mm)     L2(mm)     T0(m)   T1      T2      T3      T4
    {0, 4,  1822, 0},   //  308.5(0)	371.4(4)    1.8	    3.6	    5.5	    7.3     9.1
    {1, 4,  1905, 0},   //  310.8(1)	371.4(4)	1.9	    3.8	    5.7	    7.6	    9.5
    {2, 4,  2241, 0},   //  318.6(2)	371.4(4)	2.2	    4.5	    6.7	    9.0	    11.2
    {3, 4,  2387, 0},   //  321.4(3)	371.4(4)	2.4	    4.8	    7.2	    9.5	    11.9
};

const uint16 gusa_avg_coef[] __attribute__ ((section (".text"))) = { 0, 32768/1, 32768/2, 32768/3 +1, 32768/4, 32768/5 +1, 32768/6};

uint16 poly_atan2(int16 s_sin, int16 s_cos);
E_PWR_RANGE sinewave_pwr(int16 s_sin, int16 s_cos);
uint16 distance_estimation_stage01();
uint16 distance_estimation_stage23(uint16 us_prev_rough);
uint16 distance_estimation_stage45(uint16 us_prev_rough);

void calc_sincos(T_FREQ_DATA *pt_meas){

    uint16 us_sampl0, us_sampl1;

    us_sampl0 = (gt_meas_info.usa_acc[0] + MEAS_RND_CONST) >> MEAS_ACC_SCALE;
    us_sampl1 = (gt_meas_info.usa_acc[2] + MEAS_RND_CONST) >> MEAS_ACC_SCALE;
    pt_meas->s_cos = (us_sampl0 - us_sampl1);

    us_sampl0 = (gt_meas_info.usa_acc[1] + MEAS_RND_CONST) >> MEAS_ACC_SCALE;
    us_sampl1 = (gt_meas_info.usa_acc[3] + MEAS_RND_CONST) >> MEAS_ACC_SCALE;
    pt_meas->s_sin = (us_sampl0 - us_sampl1);
}

static void activate_measure(uint16 us_freq)
{
    spi_set_2bytes(0x02, us_freq >> 8, (uint8)us_freq);

    // Disable UART RX interrupt
    // Note: no full commande reception is possible during ADC sampling
    //       only short commands - start/stop/apd. Short commands fits
    //       to UART hw double buffer and thus can be processed whithout ISR
    UCSR0B &=  ~(1<<RXCIE0);

    gt_meas_info.uc_freq_sampl_delay = MEAS_DELAY_SAMPLES_NUM;
    gt_meas_info.uc_freq_sampl_cnt = 1 << MEAS_SAMPLES_P;

    // Activate ADC ISR
    gt_meas_info.e_state = MEAS_STATE_SYNC;
    ADCSRA |= (1<<ADEN ) | (1 << ADIF);         // Enable ADC 
    TIFR1 = (1 << OCF1B);                       // Clear interrupt flag (must be cleared for ADC autostart facility)
}

void init_measure()
{
    
    // init measure info 
    gt_meas_info.e_curt_state = CURT_STATE_UNKNOWN;
    gt_meas_info.e_state = MEAS_STATE_DISABLED;
    gt_meas_info.us_timestamp  = 0;

    guc_lcd_meas_progress = 0;
    guc_lcd_meas_refresh_cnt = 0;

}

void apd_auto_tune()
{
    // Modify APD control value according to APD auto tune table
    // where table row is minimal signal level range during last measurent,
    // table column is the APD control value range during last measurent.

    int8   c_apd_diff;
    uint8  uc_apd_low;
    uint8  uc_apd_range;
    uint8  *puc_apd_range_table;
    uint16 us_new_apd;

    if (gt_meas_info.uc_calibration)
    {
        us_new_apd = gt_meas_info.us_calib_bias_ctrl;
    }
    else
    {
        us_new_apd = gt_meas_info.us_meas_bias_ctrl;
    }

    puc_apd_range_table = (uint8*)&guca_apd_range[0];
    uc_apd_low = (uint8)us_new_apd;
    uc_apd_range = 0;

    for (uc_apd_range = 0; uc_apd_range < APD_RANGE_NUM; uc_apd_range++)
    {
        if ( uc_apd_low <= *puc_apd_range_table++)
        {
            break;
        }
    }

    // ignore min power if max greater than allowed
    if (gt_meas_info.e_pwr_range_max == E_PWR_RANGE_OVERVOLTAGE)
        gt_meas_info.e_pwr_range_min = E_PWR_RANGE_OVERVOLTAGE;

    c_apd_diff = gca_apd_auto_tune[gt_meas_info.e_pwr_range_min][uc_apd_range];
    us_new_apd += c_apd_diff;

    if (us_new_apd < APD_AUTO_MIN) us_new_apd = APD_AUTO_MIN;
    if (us_new_apd > APD_AUTO_MAX) us_new_apd = APD_AUTO_MAX;

    if (gt_meas_info.uc_calibration)
    {
        gt_meas_info.us_calib_bias_ctrl = us_new_apd;
    }
    else
    {
        gt_meas_info.us_meas_bias_ctrl = us_new_apd;
    }

}

void dle_scan_fsm_single_meas_proc(T_FREQ_DATA *pt_freq)
{
    E_PWR_RANGE e_pwr_range;
    uint16 us_phase;
    uint16 us_tmp;

    // ----------------------------------------------------
    // --- Phase processing
    // ----------------------------------------------------
    // calculte absolute phase value
    us_phase = poly_atan2(pt_freq->s_sin, pt_freq->s_cos);

    if (gt_meas_info.uc_calibration)
    { 
        pt_freq->us_calib_phase = us_phase; 
    }
    else
    {
        pt_freq->us_meas_phase = us_phase; 
    }

    // Get phase relative to calibration value if valid
    if ( (pt_freq->us_meas_phase != 0xFFFF) && (pt_freq->us_calib_phase != 0xFFFF))
    {
        us_phase = pt_freq->us_meas_phase - pt_freq->us_calib_phase;
        if (pt_freq->us_calib_phase > pt_freq->us_meas_phase) us_phase -= (65536 - PHASE_2PI);

        // convert from rad to 2*pi*rad. Mult by 1.273
        us_tmp = (((uint32)us_phase * COEF_Q13RAD_TO_2PIRAD) + 0x8000) >> 16;
        us_phase += us_tmp;
        pt_freq->us_phase = us_phase;
    }
    else
    {
        us_phase = 0xFFFF;
        gt_meas_info.e_err_code = E_MEAS_ERR_NOSIG;
    }


    // ----------------------------------------------------
    // --- Power processing
    // ----------------------------------------------------

    // classify input signal power ( E_PWR_RANGE_TOOLOW ... E_PWR_RANGE_OVERVOLTAGE )
    e_pwr_range = sinewave_pwr(pt_freq->s_sin, pt_freq->s_cos);

    // update MIN signal level within single measure (for APD auto tune & signal validation)
    if (e_pwr_range < gt_meas_info.e_pwr_range_min)
    {
        gt_meas_info.e_pwr_range_min = e_pwr_range;
    }

    // update MAX signal level within single measure (for APD auto tune & signal validation)
    if (e_pwr_range > gt_meas_info.e_pwr_range_max)
    {
        gt_meas_info.e_pwr_range_max = e_pwr_range;
    }

}

void dle_scan_fsm_finalize()
{
    uint16 us_apd_bias;

    // -------------------------------------------
    // --- Distance measurement
    // -------------------------------------------
    if ( // !gt_meas_info.uc_calibration && 
         gt_meas_info.e_err_code == E_MEAS_ERR_OK )
    { 
        // validate input signal power
        if ( gt_meas_info.e_pwr_range_min == E_PWR_RANGE_TOOLOW ||
             gt_meas_info.e_pwr_range_max == E_PWR_RANGE_OVERVOLTAGE )
        {
            gt_meas_info.e_err_code = E_MEAS_ERR_PWR;
        }
        else
        {
            uint16 us_distance; 

            us_distance = distance_estimation_stage01();                    // 2600 cycles
            us_distance = distance_estimation_stage23(us_distance);         // 2300 cycles
            us_distance = distance_estimation_stage45(us_distance);         // 3800 cycles
            gt_meas_info.us_distance = us_distance;
        }
    }

    // -------------------------------------------
    // --- Prepare and send out result
    // -------------------------------------------
    // Clear TX buffers
    guc_out_msg_wr_idx = 0;
    guc_out_msg_rd_idx = 0;

    // Compose packet
    guca_out_msg_buff[1] = 8;       // packet length
    guca_out_msg_buff[2] = (gt_meas_info.us_timestamp & 0xFF);
    guca_out_msg_buff[3] = (gt_meas_info.us_timestamp >> 8);

    if (gt_meas_info.uc_calibration)
    { // Calibration header
        guca_out_msg_buff[0] = 0x74;
        us_apd_bias = gt_meas_info.us_calib_bias_ctrl;
    }   
    else
    { // Regular measurement header
        guca_out_msg_buff[0] = 0x73;
        us_apd_bias = gt_meas_info.us_meas_bias_ctrl;
    }

    guca_out_msg_buff[4] = us_apd_bias & 0xFF;
    guca_out_msg_buff[5] = us_apd_bias >> 8;

    guca_out_msg_buff[6] = (uint8)gt_meas_info.us_distance;
    guca_out_msg_buff[7] = (uint8)(gt_meas_info.us_distance >> 8);
    guca_out_msg_buff[8] = gt_meas_info.e_err_code;
    guca_out_msg_buff[9] = gt_meas_info.e_pwr_range_min;

    guc_out_msg_wr_idx = 10;


    // -------------------------------------------
    // --- Send debug info if enabled
    // -------------------------------------------
    // ...


    // -------------------------------------------
    // --- APD auto tune
    // -------------------------------------------
    if (gt_meas_info.e_apd_mode == APD_MODE_AUTO)
    {
        // Disable calibration if APD AUTO enabled and signal level is ok
        if (
            gt_meas_info.uc_calibration && 
            gt_meas_info.e_pwr_range_min == E_PWR_RANGE_OK && 
            gt_meas_info.e_pwr_range_max != E_PWR_RANGE_OVERVOLTAGE
            )
        {
            gt_meas_info.uc_calibration = 0;
        }
        else
        {
            apd_auto_tune();
        }
    } 


    // -------------------------------------------
    // --- Referesh LCD info
    // -------------------------------------------
    // Refresh progress ticker
    if (guc_lcd_meas_refresh_cnt == LCD_PROGRESS_REFRESH_CNT)
    { 
        guc_lcd_meas_refresh_cnt = 0;

        guc_lcd_meas_progress++;
        if (guc_lcd_meas_progress == LCD_PROGRESS_TICKER_NUM) guc_lcd_meas_progress = 0;

        guca_lcd_digits_set1[0] = pgm_read_byte_near( &gca_progress_ticker0[guc_lcd_meas_progress]);    
        guca_lcd_digits_set1[1] = pgm_read_byte_near( &gca_progress_ticker1[guc_lcd_meas_progress]);    

        // Refresh APD info
        guca_lcd_digits_set1[2] = 0x20;
        guca_lcd_digits_set1[3] = ((uint8)us_apd_bias) >> 4 & 0x0F;
        guca_lcd_digits_set1[4] = ((uint8)us_apd_bias) & 0x0F;

        // Refresh Distance data
        if (gt_meas_info.e_err_code == 0)
        {
            dec2bcd(guca_lcd_digits_set0, gt_meas_info.us_distance);
        }
        else
        {   uint8 uc_i;
            guca_lcd_digits_set0[0] = gt_meas_info.e_err_code;
            for(uc_i = 1; uc_i < LCD_SET_DIGITS_NUM; uc_i++) guca_lcd_digits_set0[uc_i] = 0x1E; // "-----"
        }

        guc_lcd_redraw = 1; 
    }
    guc_lcd_meas_refresh_cnt++;




} // End of finalize measurement


void dle_scan_fsm_curtain_ctrl()
{
    // Is calibration routine activated ?
    if (gt_meas_info.uc_calibration)
    { // Calibration. Close the curtain

        if (gt_meas_info.e_curt_state != CURT_STATE_DOWN)
        {
#if CURTAIN_ENABLED
            // Drive curtain to close
            CURTAIN_PORT &= ~(1 << CURTAIN_DOWN_PIN);
#endif
            if (gt_meas_info.uc_delay < CURTAIN_DELAY) gt_meas_info.uc_delay = CURTAIN_DELAY;
            gt_meas_info.e_curt_state = CURT_STATE_DOWN;
        }

    }
    else
    { // Regular measurement. Open the curtain

        if (gt_meas_info.e_curt_state != CURT_STATE_UP)
        {
#if CURTAIN_ENABLED
            // Drive the curtain to open back in case of calibration
            CURTAIN_PORT &= ~(1 << CURTAIN_UP_PIN);
#endif
            if (gt_meas_info.uc_delay < CURTAIN_DELAY) gt_meas_info.uc_delay = CURTAIN_DELAY;
            gt_meas_info.e_curt_state = CURT_STATE_UP;
        }

    }
}


void dle_scan_fsm()
{
    //gt_meas_info.e_state = MEAS_STATE_READY;
    //gt_meas_info.pt_freq = &gta_freq_table[4];
    //gt_meas_info.pt_freq->s_cos = 100;
    //gt_meas_info.pt_freq->s_sin = 100;
    //gt_meas_info.uc_calibration = 1;
    //gt_meas_info.e_pwr_range_min = E_PWR_RANGE_NUM;

    // ----------------------------------------------------------------------
    // --- Idle state
    // ----------------------------------------------------------------------

    // Initiate distance measurement if 
    // in idle state                AND
    // it is measurement time       AND
    // no data for transfer in TX buffer (previous results are sent out)
    if (gt_meas_info.e_state == MEAS_STATE_IDLE &&
        gus_meas_timer >= MEASUREMENT_PERIOD &&
        guc_out_msg_wr_idx == guc_out_msg_rd_idx)
    {
        gt_meas_info.us_timestamp += gus_meas_timer;

        // Reset timer
        gus_meas_timer = 0;

        // Moves curtain UP or DOWN if necessary
        dle_scan_fsm_curtain_ctrl();

        // Modify APD bias value if necessary
        apd_control();

        // Check measuremnent delay (required in case of curtain state change or APD value modification)
        if (gt_meas_info.uc_delay != 0)
        { // Measure delay active. Delay for yet another MEASUREMENT_PERIOD
            gt_meas_info.uc_delay --;
            return;
        }

        // Init measurement statics
        gt_meas_info.e_pwr_range_min = E_PWR_RANGE_NUM;
        gt_meas_info.e_pwr_range_max = E_PWR_RANGE_TOOLOW;
        gt_meas_info.e_err_code = E_MEAS_ERR_OK;

#if CURTAIN_ENABLED
        // Remove curtain drive signal
        CURTAIN_PORT |= 
            (1 << CURTAIN_UP_PIN  )|
            (1 << CURTAIN_DOWN_PIN);
#endif

        // init pointer to table and activate ADC ISR
        gt_meas_info.pt_freq = &gta_freq_table[0];
        activate_measure(gt_meas_info.pt_freq->us_freq);


    }// End of idle state && measurement time


    // ----------------------------------------------------------------------
    // --- Ready state.
    // ----------------------------------------------------------------------
    if (gt_meas_info.e_state == MEAS_STATE_READY)
    { // Single frequency measurement is ready. Advance pt_freq pointer and continue


        T_FREQ_DATA *pt_freq;
        T_FREQ_DATA *pt_next_freq;

        pt_freq = gt_meas_info.pt_freq;

        // Convert accumulated samples to SIN and COS components
        // Att: Must be called before next measurement activation
        calc_sincos(pt_freq);

        // Advance table pointer
        pt_next_freq = pt_freq+1;
        gt_meas_info.pt_freq = pt_next_freq;

        // Continue measurement until end of table, disable otherwise
        if (pt_next_freq->us_freq)
        { 
            activate_measure(pt_next_freq->us_freq);
        }
        else
        { // Disable state machine 
            gt_meas_info.e_state = MEAS_STATE_IDLE;

            // Enable RX interrupt for further command processing
            UCSR0B |=  (1<<RXCIE0);
        } 

        // signal postprocessng
        dle_scan_fsm_single_meas_proc(pt_freq);

        // finalize measurement and get the distance at the end of measure table.
        // As well as APD auto tune control
        if (pt_next_freq->us_freq == 0)
        { 
            dle_scan_fsm_finalize();
        }

    } // End of MEAS_STATE_READY


} // End of dle_scan_fsm()


// polynomial atan2() ~1000 cycles per call (worst case)
uint16 poly_atan2(int16 s_sin, int16 s_cos)
{

    short s_abs_sin, s_abs_cos;
    int16 s_norm, s_norm_p3;
    int16 s_tmp1, s_tmp2;

    unsigned short us_atan_rad;

    s_abs_cos = abs(s_cos);
    s_abs_sin = abs(s_sin);

    
    {  // Scale Down with rounding. 
       // 7 bits is enough for normal operation

        uint8 uc_carry_sin, uc_carry_cos;

        s_tmp1 = s_abs_cos >  s_abs_sin  ? s_abs_cos : s_abs_sin;   // max
        uc_carry_cos = 0;
        uc_carry_sin = 0;

        while ( s_tmp1 > 128 )
        {
            s_tmp1 >>= 1;

            uc_carry_sin = s_abs_sin & 1;
            uc_carry_cos = s_abs_cos & 1;

            s_abs_cos >>= 1;
            s_abs_sin >>= 1;
        }

        s_abs_cos += uc_carry_cos ? 1 : 0;
        s_abs_sin += uc_carry_sin ? 1 : 0;
    }

    { // Normalization (cos-sin)/(cos+sin) (~520 cycles)
        div_t t_norm;

        s_tmp1 =  (int16) ( s_abs_cos - s_abs_sin ) << 7;  // Q7
        s_tmp2 =  (int16) ( s_abs_cos + s_abs_sin );       // Q8 unsigned

        if (s_tmp2 == 0)
        {
            return 0xFFFF;
        }

        // Q7/Q8 = Q7.8
        t_norm = div( s_tmp1, s_tmp2);
        s_norm = t_norm.quot << 7;

        t_norm = div( t_norm.rem << 7, s_tmp2);
        s_norm += t_norm.quot;
    }

    // Atan2 polynom calculation k0 + k1*norm^3 - k2*norm (~350cycles)

    // norm^3 
    s_norm_p3 = ((((int32)s_norm    * s_norm ) << 2) + 0x8000) >> 16;
    s_norm_p3 = ((((int32)s_norm_p3 * s_norm ) << 2) + 0x8000) >> 16;

    s_tmp1 = (((6432  * (int32)s_norm_p3) << 2) + 0x8000) >> 16;
    s_tmp2 = (((32168 * (int32)s_norm) << 2) + 0x8000) >> 16;

    us_atan_rad = (s_tmp1 - s_tmp2 + 25736);                   // Q16 unsigned!    sign(s_tmp1) == sign(s_tmp2)
    us_atan_rad = (us_atan_rad + 2) >> 2;                      // Q3.13

    if (s_cos < 0)
    {
        us_atan_rad = 25736 - us_atan_rad;                     // pi-atan Q3.13
    }

    if (s_sin < 0)
    {
        us_atan_rad = 51472 - us_atan_rad;                     // 2*pi-atan Q3.13
    }

    
    return us_atan_rad;

}

E_PWR_RANGE sinewave_pwr(int16 s_sin, int16 s_cos)
{
    int8 c_sin, c_cos;
    uint16 us_pwr;
    E_PWR_RANGE e_pwr_range;

    c_sin = (int8)(s_sin >> PWR_RANGE_SCALE_EXP);
    c_cos = (int8)(s_cos >> PWR_RANGE_SCALE_EXP);
    us_pwr = (c_sin*c_sin) + (c_cos*c_cos);
 
    for (e_pwr_range = 0; e_pwr_range < (E_PWR_RANGE_NUM-1); e_pwr_range++)
    {
        if (us_pwr <  gusa_pwr_range[e_pwr_range])
        {
             break;
        }
    }

    return e_pwr_range;
}

uint16 meas_distr_s0(uint16 us_center, uint16 *pus_data)
{
    uint8 uc_i;
    uint16 us_distr;

    us_distr = 0;
    for(uc_i = 0; uc_i < 4; uc_i++)
    {
        if (pus_data[uc_i] != 0)
        {
            // us_distr += abs(pus_data[n_i] - us_center);
            if (pus_data[uc_i] > us_center)
            {
                us_distr += pus_data[uc_i] - us_center;
            }
            else
            {
                us_distr += us_center - pus_data[uc_i];
            }
        }
    }

    return (us_distr >> 2);


}


uint16 measure_single_freq(T_FREQ_DATA *pt_freq, uint16 us_rough_dist){

    uint32 ul_dist_scaled;
    uint16 us_dist;
    uint16 us_wavelen;
    uint16 us_rem;
    uint8 uc_periods;

    // Calculate number of periods in rough distance estimation
    ul_dist_scaled = 0;
    uc_periods = 0;
    us_rem = us_rough_dist;

    us_wavelen = pt_freq->us_wavelen << 2;
    while(us_rem >= us_wavelen)
    {
        us_rem -= us_wavelen;
        uc_periods += 4;
    }


    us_wavelen = pt_freq->us_wavelen;
    while(us_rem >= us_wavelen)
    {
        us_rem -= us_wavelen;
        uc_periods += 1;
    }

    ul_dist_scaled = (uint32)pt_freq->us_wavelen_scaled * uc_periods;
    ul_dist_scaled += ((uint32)pt_freq->us_phase * pt_freq->us_wavelen_scaled) >> 16;

    us_dist = (uint16)((ul_dist_scaled * (65536/128) + 0x8000) >> 16);
    return us_dist;
}

uint16 dual_freq_distance(T_DUAL_FREQ_MEAS *pt_meas, uint16 us_rough_dist)
{
    uint16 us_distance;
    uint16 us_delta_phase;

    // Distance = floor((rough/period)) * period + (phase1 - phase2) * period
    us_delta_phase = pt_meas->us_phase1 - pt_meas->us_phase2;

    us_distance = 0;

    // Calculate number of periods in rough distance estimation
    if (us_rough_dist != 0)
    {
        uint16 us_rem;

        us_rem = us_rough_dist;

        while(us_rem >= pt_meas->us_period)         
        {
            us_rem -= pt_meas->us_period;
            us_distance += pt_meas->us_period;
        }
    }

    us_distance += ((uint32)us_delta_phase * pt_meas->us_period) >> 16;

    return us_distance;
}

int8 exclude_most_friendless(uint16 *pus_data, uint8 uc_num, uint16 us_max_dist)
{
    // "friendlessness" - is the sum of the distances till the neighbours :)
    // us_max_dist - is the maximum allowed "friendlessness" 

    int8   c_max_of_sum_num;
    uint16 us_max_of_sum;
    uint16 us_sum_dist;

    c_max_of_sum_num = -1;    
    us_max_of_sum = 0;

    // Get sum of distance to anothers
    for(int8 c_j = 0; c_j < uc_num; c_j++)
    {
        us_sum_dist = 0; 

        if (pus_data[c_j] == 0) continue;

        for(int8 c_i = 0; c_i < uc_num; c_i++)
        {

            if (pus_data[c_i] == 0) continue;

            // us_sum_dist += abs(pus_data[c_j] - pus_data[c_i]);
            if (pus_data[c_j] > pus_data[c_i])
            {
                us_sum_dist += pus_data[c_j] - pus_data[c_i];
            }
            else
            {
                us_sum_dist += pus_data[c_i] - pus_data[c_j];
            }
        }

        // Find maximum
        if (us_sum_dist > us_max_of_sum)
        {
            us_max_of_sum = us_sum_dist ;
            c_max_of_sum_num = c_j;
        }

    }

    // Is the sum of the distances till the neighbours within allowed limit
    if (us_max_of_sum > us_max_dist)
    {
        return c_max_of_sum_num;
    }

    // Nothing to exclude
    return -1;

}



uint16 meas_avg(uint16 *pus_data, uint8 uc_num)
{
    uint8 uc_i, uc_cnt;
    uint16 us_sum, us_avg, us_coef;

    uc_cnt = 0;
    us_sum = 0;
    for (uc_i = 0; uc_i < uc_num; uc_i++)
    {
        if (pus_data[uc_i] == 0) continue;

        us_sum += pus_data[uc_i];
        uc_cnt++;
    }

    us_coef  = pgm_read_word_near( &gusa_avg_coef[uc_cnt]);

    if (uc_cnt == 0)
    {
        gt_meas_info.e_err_code = E_MEAS_ERR_AVG0;
    }

    us_avg = ((((uint32)us_sum * us_coef) << 1)+ 0x8000) >> 16;

    return us_avg;
}


uint16 distance_estimation_stage01()        // ~2600 cycles
{
#if ROUGH_DIST_1_NUM != 4
#error Rework required!
#endif

    uint16 usa_rough_1[ROUGH_DIST_1_NUM];
    uint16 usa_rough_2[ROUGH_DIST_1_NUM];
    uint16 us_rought_avg_1;
    uint16 us_rought_avg_2;
    uint16 *pus_rough;
    uint16 us_rough;
    uint8  uc_i;

    // Fill the dual measurement table
    for(uc_i = 0; uc_i < ROUGH_DIST_1_NUM; uc_i ++)
    {
        uint8 uc_idx;

        uc_idx = gta_dual_meas_s0[uc_i].uc_f1_idx;
        gta_dual_meas_s0[uc_i].us_phase1 = gta_freq_table[uc_idx].us_phase;

        uc_idx = gta_dual_meas_s0[uc_i].uc_f2_idx;
        gta_dual_meas_s0[uc_i].us_phase2 = gta_freq_table[uc_idx].us_phase;
    }

    // ----------------------------------------------------
    // --- Stage 0
    // --- Find rough distance estimation according 
    // --- to the gta_dual_meas_s0 table, with special 
    // --- care around period boundary
    // ----------------------------------------------------
    us_rought_avg_1 = 0;
    us_rought_avg_2 = 0;
    for(uc_i = 0; uc_i < ROUGH_DIST_1_NUM; uc_i ++)
    {
        us_rough = dual_freq_distance(&gta_dual_meas_s0[uc_i], 0);      // ~100 cycles

        if ((gta_dual_meas_s0[uc_i].us_period - us_rough) < 700)
        { // too close to the period boundary, try 0
            usa_rough_2[uc_i] = us_rough;
            usa_rough_1[uc_i] = 1;       // not 0 because 0 is used for invalid/discarded values
        }
        else if (us_rough < 500)
        { // too close to 0 try T
            usa_rough_2[uc_i] = gta_dual_meas_s0[uc_i].us_period;
            usa_rough_1[uc_i] = us_rough;
        }
        else
        { 
            usa_rough_2[uc_i] = us_rough;
            usa_rough_1[uc_i] = us_rough;
        }

        // -----------------------------------------
        // --- Accumulate both rough estimation 
        // -----------------------------------------
        us_rought_avg_1 += usa_rough_1[uc_i];
        us_rought_avg_2 += usa_rough_2[uc_i];

    }

    us_rought_avg_1 >>= 2;
    us_rought_avg_2 >>= 2;

    {   // choose less distributed rough average
        uint16 us_distr_1, us_distr_2;

        // get distribution around average
        us_distr_1 = meas_distr_s0(us_rought_avg_1, &usa_rough_1[0]);   // ~130 cycles
        us_distr_2 = meas_distr_s0(us_rought_avg_2, &usa_rough_2[0]);
        
        pus_rough = (us_distr_1 < us_distr_2) ? &usa_rough_1[0] : &usa_rough_2[0];
    }


    // ---------------------------------------------------------------
    // --- Stage 1
    // --- Exclude most friendless (but not more than two members)
    // ---------------------------------------------------------------
    {
        uint8 uc_exclude_cnt;
        int8  c_exclude_num;

        uc_exclude_cnt = 0;
        do
        {
            c_exclude_num  = exclude_most_friendless(pus_rough, ROUGH_DIST_1_NUM, 700); // ~550 cycles
            if (c_exclude_num >= 0)
            {
                pus_rough[c_exclude_num] = 0;
                uc_exclude_cnt++;
            }

        }while(c_exclude_num >= 0 && uc_exclude_cnt < 2 ); // exit if nothing was excluded OR too many exclusions
    }

    us_rough = meas_avg(pus_rough, ROUGH_DIST_1_NUM);

    if (us_rough > 8000)
    {
        gt_meas_info.e_err_code = E_MEAS_ERR_MAX0;
    }

    return us_rough;    

}

uint16 distance_estimation_stage23(uint16 us_prev_rough)    // ~2300 cycles
{
#if ROUGH_DIST_2_NUM != 4
#error Rework required!
#endif
    uint16 usa_dist[ROUGH_DIST_2_NUM];   
    uint16 us_dist;
    uint8 uc_i;

    // Fill the dual measurement table ~300 cycles
    for(uc_i = 0; uc_i < ROUGH_DIST_2_NUM; uc_i ++)
    {
        uint8 uc_idx;

        uc_idx = gta_dual_meas_s2[uc_i].uc_f1_idx;
        gta_dual_meas_s2[uc_i].us_phase1 = gta_freq_table[uc_idx].us_phase;

        uc_idx = gta_dual_meas_s2[uc_i].uc_f2_idx;
        gta_dual_meas_s2[uc_i].us_phase2 = gta_freq_table[uc_idx].us_phase;
    }


    // ------------------------------------------------------------------------    
    // --- Stage 2
    // --- Correction for +/- one double period. Exclude too far from avg
    // ------------------------------------------------------------------------    
    for(uc_i = 0; uc_i < ROUGH_DIST_2_NUM; uc_i++)
    {
        uint8  uc_neg_corr;
        uint16 us_delta;    // distance beetween previous and current measurement

        us_dist = dual_freq_distance(&gta_dual_meas_s2[uc_i], us_prev_rough);    // ~150cycles

        // try to correct distance +/- 1 period if fine distance is too far from average
        // us_delta = abs(us_dist - us_prev_rough);
        if (us_dist > us_prev_rough)
        {
            us_delta = us_dist - us_prev_rough;
            uc_neg_corr = 1;
        }
        else
        {
            us_delta = us_prev_rough - us_dist;
            uc_neg_corr = 0;
        }

        if (us_delta > 500)
        { // current measurement is too far from the previous stage rough

            uint16 us_corr_dist, us_correction;

            us_correction = gta_dual_meas_s2[uc_i].us_period;

            // us_corr_dist = abs(us_delta - us_correction);
            if  ( us_delta > us_correction )
            {   
                us_corr_dist = us_delta - us_correction;
            }
            else
            {
                us_corr_dist = us_correction - us_delta;
            }

            // Check correction effect
            if (us_corr_dist > 500)
            { // discard measurement if doesn't help
                usa_dist[uc_i] = 0;
            }
            else
            { // get better, apply correction
                if (uc_neg_corr)
                { // -T, with zero limiting
                    if (us_dist < us_correction)
                    {
                        usa_dist[uc_i] = 0;
                    }
                    else
                    {
                        usa_dist[uc_i] = us_dist - us_correction;
                    }
                }
                else
                { // +T
                    usa_dist[uc_i] = us_dist + us_correction;
                }

            } // End of coorection apply

        } // End of IF too far from prvious rough
        else
        {
            usa_dist[uc_i] = us_dist;
        }
       
    } // End of stage 2 rough FOR


    // ---------------------------------------------------------------
    // --- Stage 3
    // --- Exclude most friendless (but not more than two members)
    // ---------------------------------------------------------------
    {
        uint8 uc_exclude_cnt;
        int8  c_exclude_num;

        uc_exclude_cnt = 0;
        do
        {
            c_exclude_num  = exclude_most_friendless(&usa_dist[0], ROUGH_DIST_2_NUM, 100);  // ~600 cycles
            if (c_exclude_num >= 0)
            {
                usa_dist[c_exclude_num] = 0;
                uc_exclude_cnt++;
            }

        }while(c_exclude_num >= 0 && uc_exclude_cnt < 2 ); // exit if nothing was excluded OR too many exclusions
    }

    us_dist = meas_avg(usa_dist, ROUGH_DIST_2_NUM); // ~150 cycles

    return us_dist;
}



uint16 distance_estimation_stage45(uint16 us_prev_rough)    // ~3800cycles
{
    uint16 usa_dist[FINE_DIST_NUM];   
    uint16 us_dist;
    uint8 uc_i;

    // ------------------------------------------------------------------------    
    // --- Stage 4
    // --- Correction for +/- one double period. Exclude too far from avg
    // ------------------------------------------------------------------------    
    for(uc_i = 0; uc_i < FINE_DIST_NUM; uc_i++)
    {
        uint8  uc_neg_corr;
        uint16 us_delta;         // distance beetween previous and current measurement

        us_dist = measure_single_freq(&gta_freq_table[uc_i], us_prev_rough);    // ~350 cycles

        // try to correct distance +/- 1 period if fine distance is too far from average
        // us_delta = abs(us_rough - us_prev_rough);
        if (us_dist > us_prev_rough)
        {
            us_delta = us_dist - us_prev_rough;
            uc_neg_corr = 1;
        }
        else
        {
            us_delta = us_prev_rough - us_dist;
            uc_neg_corr = 0;
        }

        if (us_delta > 200)
        { // current measurement is too far from the previous stage rough

            uint16 us_corr_dist, us_correction;

            us_correction = gta_freq_table[uc_i].us_wavelen;

            // us_corr_dist = abs(us_delta - us_correction);
            if  ( us_delta > us_correction )
            {   
                us_corr_dist = us_delta - us_correction;
            }
            else
            {
                us_corr_dist = us_correction - us_delta;
            }

            // Check correction effect
            if (us_corr_dist > 60)
            { // discard measurement if doesn't help
                usa_dist[uc_i] = 0;
            }
            else
            { // get better, apply correction
                if (uc_neg_corr)
                { // -T, with zero limiting
                    if (us_dist < us_correction)
                    {
                        usa_dist[uc_i] = 0;
                    }
                    else
                    {
                        usa_dist[uc_i] = us_dist - us_correction;
                    }
                }
                else
                { // +T
                    usa_dist[uc_i] = us_dist + us_correction;
                }

            } // End of coorection apply

        } // End of IF too far from prvious rough
        else
        {
            usa_dist[uc_i] = us_dist;
        }
       
    } // End of stage 4 fine FOR


    // ---------------------------------------------------------------
    // --- Stage 5
    // --- Exclude most friendless (but not more than two members)
    // ---------------------------------------------------------------
    {
        uint8 uc_exclude_cnt;
        int8  c_exclude_num;

        uc_exclude_cnt = 0;
        do
        {
            c_exclude_num  = exclude_most_friendless(&usa_dist[0], FINE_DIST_NUM, 15);  // ~800 cycles
            if (c_exclude_num >= 0)
            {
                usa_dist[c_exclude_num] = 0;
                uc_exclude_cnt++;
            }

        }while(c_exclude_num >= 0 && uc_exclude_cnt < 2 ); // exit if nothing was excluded OR too many exclusions
    }

    us_dist = meas_avg(usa_dist, FINE_DIST_NUM);    // ~170cycles

    return us_dist;
}

#if 0
// Prepare debug packet
        uc_wr_idx = guc_out_msg_wr_idx;

        guca_out_msg_buff[uc_wr_idx+0] = (uint8)pt_freq->s_cos;
        guca_out_msg_buff[uc_wr_idx+1] = (uint8)(pt_freq->s_cos >> 8);

        guca_out_msg_buff[uc_wr_idx+2] = (uint8)pt_freq->s_sin;
        guca_out_msg_buff[uc_wr_idx+3] = (uint8)(pt_freq->s_sin >> 8);

        guca_out_msg_buff[uc_wr_idx+4] = (uint8)pt_freq->us_calib_phase;
        guca_out_msg_buff[uc_wr_idx+5] = (uint8)(pt_freq->us_calib_phase >> 8);

        guca_out_msg_buff[uc_wr_idx+6] = (uint8)pt_freq->us_meas_phase;
        guca_out_msg_buff[uc_wr_idx+7] = (uint8)(pt_freq->us_meas_phase >> 8);

        guca_out_msg_buff[uc_wr_idx+8] = (uint8)us_phase;
        guca_out_msg_buff[uc_wr_idx+9] = (uint8)(us_phase >> 8);

        guca_out_msg_buff[uc_wr_idx+10] = e_pwr_range;
        guca_out_msg_buff[uc_wr_idx+11] = 0;

        guc_out_msg_wr_idx += MEAS_FREQ_RESULT_LEN;
#endif
