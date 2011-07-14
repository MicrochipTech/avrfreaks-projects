/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_meas.h
**                                        
** PROJECT      :   DLE MEAS
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
#ifndef __DLE_SCAN_MEAS_H__
#define __DLE_SCAN_MEAS_H__

#define SAMPLES_PER_PERIOD  4
#if SAMPLES_PER_PERIOD != 4
    #error SAMPLES_PER_PERIOD is redefined. Rework required!
#endif

#define MEASUREMENT_PERIOD (25000/30)   // 30usec ticks

#define APD_AUTO_MAX    0x9050
#define APD_AUTO_MIN    0x9000

#define PHASE_2PI 51472
#define COEF_Q13RAD_TO_2PIRAD 17907       // (65536/51472)-1

typedef enum e_pwr_range_tag        // see gusa_pwr_range[] for range values in mV
{
    E_PWR_RANGE_TOOLOW = 0,
    E_PWR_RANGE_LOW,
    E_PWR_RANGE_OK,
    E_PWR_RANGE_HIGH,
    E_PWR_RANGE_TOOHIGH,
    E_PWR_RANGE_OVERVOLTAGE,
    E_PWR_RANGE_NUM 
} E_PWR_RANGE;

typedef struct {
    uint16 us_freq;
    uint16 us_wavelen;             // half wave length in mm
    uint16 us_wavelen_scaled;      // half wave length in 1/128mm

    int16 s_sin;
    int16 s_cos;
    
    uint16 us_meas_phase;          // Q3.13 in rad (2pi = 51472)
    uint16 us_calib_phase;         // Q3.13 in rad (2pi = 51472)

    uint16 us_phase;                // Q16 in 2pi*rad (2pi = 65536)

} T_FREQ_DATA;


typedef enum e_meas_state_tag
{
    MEAS_STATE_DISABLED = 0,
    MEAS_STATE_IDLE,
    MEAS_STATE_INIT,
    MEAS_STATE_SYNC,
    MEAS_STATE_SYNC1,
    MEAS_STATE_SAMPLING,
    MEAS_STATE_READY
} E_MEAS_STATE;

typedef enum e_curt_state_tag
{
    CURT_STATE_UNKNOWN = 0,
    CURT_STATE_UP,
    CURT_STATE_DOWN,
} E_CURT_STATE;

typedef enum e_apd_mode_tag
{
    APD_MODE_MANUAL = 0,
    APD_MODE_AUTO
} E_APD_MODE;

typedef enum e_meas_err_code_tag
{
    E_MEAS_ERR_OK = 0,
    E_MEAS_ERR_MAX0,
    E_MEAS_ERR_AVG0,
    E_MEAS_ERR_NOSIG,
    E_MEAS_ERR_PWR,
} E_MEAS_ERR;

typedef struct meas_info_tag
{
    uint8 uc_msg_len;
    uint16 us_timestamp;
    uint8 uc_delay;
    uint8 uc_freq_sampl_delay;
    uint8 uc_freq_sampl_cnt;
    uint8 uc_calibration;
    uint16 us_distance;

    // APD auto tune 
    E_APD_MODE   e_apd_mode;
    E_PWR_RANGE  e_pwr_range_min;
    E_PWR_RANGE  e_pwr_range_max;

    uint16 us_meas_bias_ctrl;
    uint16 us_calib_bias_ctrl;

    E_CURT_STATE e_curt_state;

    E_MEAS_STATE e_state;

    T_FREQ_DATA *pt_freq;

    uint16 usa_acc[SAMPLES_PER_PERIOD];

    E_MEAS_ERR e_err_code;
} T_MEAS_INFO;


typedef struct dual_freq_meas_tag {
    
    uint8 uc_f1_idx;
    uint8 uc_f2_idx;
    uint16 us_period;
    //uint16 us_period_recip;     // reciprocal value of period

    uint16 us_phase1;
    uint16 us_phase2;
} T_DUAL_FREQ_MEAS;

#define ROUGH_DIST_1_NUM    4
#define ROUGH_DIST_2_NUM    4
#define FINE_DIST_NUM       5

#define  PWR_RANGE_SCALE_EXP    3


#define APD_RANGE_NUM       6
// ------------ measurement related global variables -----------
extern T_MEAS_INFO gt_meas_info;

// ------------ measurement related global functions -----------
extern void dle_scan_fsm();
extern void init_measure();


#endif // __DLE_SCAN_MEAS_H__
