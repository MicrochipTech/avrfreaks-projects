/***C*********************************************************************************************
**
** SRC-FILE     :   dle_io_measure.cpp
**                                        
** PROJECT      :   DLE
**                                                                
** SRC-VERSION  :   
**              
** DATE         :   22/01/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   Measurement routines for DLE device.
**                  
** FUNCTIONS    :   measure_stream2sincos()     -   converts samples stream to sin & cos values
**                  measure_sincos2phase()      -   converts sin & cos to phase (trig atan())
**                  measure_phase2distance()    -   converts measured & calibration phases to distance
**                  pol_atan()                  -   fixed point polynomial atan()
**
** COPYRIGHT    :   (c) 2011 Andrejs Vasiljevs. All rights reserved.
**
****C*E******************************************************************************************/
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "dle_io_measure.h"

#define MEAS_LOG_STR_LEN 1024
WCHAR gca_meas_log[MEAS_LOG_STR_LEN];
WCHAR gca_meas_log1[MEAS_LOG_STR_LEN];

void log_meas(WCHAR *pc_str);
extern HANDLE gh_meas_log_file;


T_FREQ_INFO gta_meas_info[] = {

    {0x37E4, 308.5}, 
    {0x1EE4, 310.8}, 
    {0xC9D5, 318.6}, 
    {0xA9D5, 321.4}, 
    {0xD3B9, 371.4}, 
    {0}
};

T_APD_AUTO gt_apd_auto = {
    0, 0 , 0,
    {{L"V.Low ",  50,     {{0x9010, +4},  {0x9020,  +2}, {0x9030, +2}, {0x9040, +2}, {0x904C, +1}, {0x9050, -32} }},        //      < 50   Extremly low  - rapid rise
     {L"Low   ", 150,     {{0x9010, +2},  {0x9020,  +2}, {0x9030, +1}, {0x9040, +1}, {0x904C, +1}, {0x9050, -32} }},        //  50.. 150   Low           - slow rise
     {L"Norm. ", 350,     {{0x9010, +0},  {0x9020,  +0}, {0x9030, +0}, {0x9040, +0}, {0x904C,  0}, {0x9050,  -2} }},        // 150.. 300   Normal        - Keep
     {L"High  ", 600,     {{0x9010, -1},  {0x9020,  -1}, {0x9030, -1}, {0x9040, -1}, {0x904C, -2}, {0x9050,  -2} }},        // 300.. 600   high          - slow drop
     {L"V.High", DBL_MAX, {{0x9010, -2},  {0x9020,  -2}, {0x9030, -2}, {0x9040, -2}, {0x904C, -2}, {0x9050,  -4} }}},       // >600        Extremly high - rapid drop
};
WORD gus_apd_calib;
WORD gus_apd_meas;
WORD gus_apd;
WORD gus_distance;
WORD gus_timestamp;
BYTE guc_pwr_range;
BYTE guc_err_code;

signed short rshift_rnd(signed short s_val, unsigned int un_shift)
{

    int n_carry;
    n_carry = s_val & (1 << (un_shift - 1));

    s_val >>= un_shift;

    s_val += n_carry ? 1 : 0;

    return s_val;
}

unsigned short poly_atan2(short s_sampl_sin, short s_sampl_cos){

    short s_norm, s_norm_p3;
    short s_abs_sin, s_abs_cos, s_tmp1, s_tmp2;
    div_t   t_norm;
    unsigned short us_atan_rad;

    int n_carry_sin, n_carry_cos;

    s_abs_cos = abs(s_sampl_cos);
    s_abs_sin = abs(s_sampl_sin);

    // Scale Down. 7 bits is enough for normal operation
    s_tmp1 = max(s_abs_cos, s_abs_sin);
    n_carry_cos = 0;
    n_carry_sin = 0;

    while ( s_tmp1 > 128 )
    {
        s_tmp1 >>= 1;

        n_carry_sin = s_abs_sin & 1;
        n_carry_cos = s_abs_cos & 1;

        s_abs_cos >>= 1;
        s_abs_sin >>= 1;
    }

    s_abs_cos += n_carry_cos ? 1 : 0;
    s_abs_sin += n_carry_sin ? 1 : 0;

    s_tmp1 = (s_abs_cos - s_abs_sin) << 7;  // Q7
    s_tmp2 = (s_abs_cos + s_abs_sin);       // Q8 unsigned
    
    if (s_tmp2 == 0)
    {
        return 0xFFFF;
    }

    // Q7/Q8 = Q7.8
    t_norm = div( s_tmp1, s_tmp2);
    s_norm = t_norm.quot << 7;

    t_norm = div( 128 * t_norm.rem, s_tmp2);
    s_norm += t_norm.quot;

    s_norm_p3 = ((s_norm * s_norm) + 0x2000) >> (15 - 1);           
    s_norm_p3 = ((s_norm_p3 * s_norm) + 0x2000) >> (15 - 1);         

    s_tmp1 = ((6432 * s_norm_p3) + 0x2000) >> (15-1);              
    s_tmp2 = ((32168 * s_norm) + 0x2000) >> (15-1);            

    us_atan_rad = (s_tmp1 - s_tmp2 + 25736);                   // Q16 unsigned!    sign(s_tmp1) == sign(s_tmp2)

    us_atan_rad = (us_atan_rad + 2) >> 2;                      // Q3.13

    if (s_sampl_cos < 0)
    {
        us_atan_rad = 25736 - us_atan_rad;                     // pi-atan Q3.13
    }

    if (s_sampl_sin < 0)
    {
        us_atan_rad = 51472 - us_atan_rad;                     // 2*pi-atan Q3.13
    }
    
    return us_atan_rad;

}


// --------------------------------------------------------------------------------------------------------
// ---------
// --------------------------------------------------------------------------------------------------------

void measure_sincos2param(T_SIG_PARAM *pt_sig){


    unsigned long  ul_sampl_pwr;

    ul_sampl_pwr = pt_sig->s_sin * pt_sig->s_sin + pt_sig->s_cos * pt_sig->s_cos;
    pt_sig->db_power_mV = sqrt((double)ul_sampl_pwr)/2*3.22;

    // phase shift calculated by stdlib 
    //pt_sig->db_angle_deg = atan2((double)pt_sig->s_sin, (double)pt_sig->s_cos) / (2*3.141593) * 360;
    //if (pt_sig->db_angle_deg < 0) pt_sig->db_angle_deg += 360;

    pt_sig->us_phase_int = poly_atan2(pt_sig->s_sin, pt_sig->s_cos);
    pt_sig->db_angle_deg = ((double)pt_sig->us_phase_int) /8192 / (2*3.141593) * 360;

    if (pt_sig->us_phase_int == 0xFFFF) pt_sig->uc_pwr_range_int = 0;       // siganl not valid
    else if (pt_sig->db_power_mV <  100) pt_sig->uc_pwr_range_int = 0;
    else if (pt_sig->db_power_mV <  150) pt_sig->uc_pwr_range_int = 1;
    else if (pt_sig->db_power_mV <  350) pt_sig->uc_pwr_range_int = 2;
    else if (pt_sig->db_power_mV <  600) pt_sig->uc_pwr_range_int = 3;
    else if (pt_sig->db_power_mV < 1300) pt_sig->uc_pwr_range_int = 4;
    else pt_sig->uc_pwr_range_int = 5;

    return;
}


double measure_dual_freq(T_FREQ_INFO *pt_freq_1, T_FREQ_INFO *pt_freq_2, double db_rough_dist){

    double db_dist;
    double db_dph;
    double db_n;

    double db_period;

    db_period = measure_dual_maxd(pt_freq_1, pt_freq_2);
    db_n = floor(db_rough_dist/db_period);

    db_dph = (pt_freq_1->db_phase - pt_freq_2->db_phase);

    if (db_dph > 360) db_dph -= 360;    // remove rounding error
    if (db_dph < 0) db_dph += 360;

    db_dist = (db_dph/360 + db_n) * db_period;
   
    return db_dist;
}

double measure_single_freq(T_FREQ_INFO *pt_freq, double db_dist_rough){

    double db_dist;
    int n_periods;

    n_periods = (int)floor(db_dist_rough/pt_freq->db_wave_len);
    pt_freq->n_periods = n_periods;

    db_dist = (n_periods + pt_freq->db_phase/360) * pt_freq->db_wave_len;
    pt_freq->db_distance = db_dist;

    return db_dist;
}

double measure_dual_maxd(T_FREQ_INFO *pt_freq_1, T_FREQ_INFO *pt_freq_2){

    double db_max_dist;

    // D = L1*L2/abs(L2-L1)
    db_max_dist = (pt_freq_1->db_wave_len * pt_freq_2->db_wave_len) / 
                  abs(pt_freq_2->db_wave_len - pt_freq_1->db_wave_len);
   
    return db_max_dist;
}

double meas_avg(double *pdb_inp, int n_num)
{

    double db_avg;
    int n_avg_cnt;

    db_avg = 0;
    n_avg_cnt = 0;

    for(int n_i = 0; n_i < n_num; n_i++)
    {
        if (pdb_inp[n_i] != 0)
        {
            db_avg += pdb_inp[n_i];
            n_avg_cnt ++;
        }
    }

    if (n_avg_cnt)
    {
        db_avg = db_avg / n_avg_cnt;
    }

    return db_avg;

}
double meas_distr(double *pdb_inp, int n_num)
{

    int n_avg_cnt;
    double db_distr;
    double db_avg;

    db_distr = 0;
    n_avg_cnt = 0;

    db_avg = meas_avg(pdb_inp, n_num);

    for(int n_i = 0; n_i < n_num; n_i++)
    {
        if (pdb_inp[n_i] != 0)
        {
            db_distr += abs(pdb_inp[n_i] - db_avg);
            n_avg_cnt ++;
        }
    }

    if (n_avg_cnt)
    {
        db_distr = db_distr / n_avg_cnt;
    }

    return db_distr;

}

int exclude_far_from_avg(double *pdb_inp, int n_num, double db_dist)
{
    
    double db_avg;
    int n_avg_cnt;

    db_avg = 0;
    n_avg_cnt = 0;

    // Get rought average
    for(int n_i = 0; n_i < n_num; n_i++)
    {
        if (pdb_inp[n_i] != 0)
        {
            db_avg += pdb_inp[n_i];
            n_avg_cnt ++;
        }
    }
    db_avg = db_avg / n_avg_cnt;

    // Exclude rough distances which are too far from average

    for(int n_i = 0; n_i < n_num; n_i++)
    {
        if (pdb_inp[n_i] != 0)
        {
            if ( (abs(db_avg - pdb_inp[n_i])) > db_dist)
            {
                return n_i;
            }
        }
    }

    return -1;

}

int exclude_most_friendless(double *pdb_inp, int n_num, double db_max_dist)
{
 
    int n_max_of_sum_num;
    double db_sum_dist[16]; 
    double db_dist, db_max_of_sum;

    // Get sum of distance to anothers
    for(int n_j = 0; n_j < n_num; n_j++)
    {
        db_sum_dist[n_j] = 0; 

        if (pdb_inp[n_j] == 0)
            continue;

        for(int n_i = 0; n_i < n_num; n_i++)
        {

            if (pdb_inp[n_i] == 0)
                continue;

            db_dist = abs(pdb_inp[n_j] - pdb_inp[n_i]);
            db_sum_dist[n_j] += db_dist;
        }
    }


    // Exclude max of min distances (most_friendless)
    db_max_of_sum = 0;
    n_max_of_sum_num = -1;
    for(int n_i = 0; n_i < n_num; n_i++)
    {
        if (db_sum_dist[n_i] > db_max_of_sum)
        {
            db_max_of_sum = db_sum_dist[n_i];
            n_max_of_sum_num = n_i;
        }
    }

    if (db_max_of_sum > db_max_dist)
    {
        return n_max_of_sum_num;
    }
    return -1;

}


int scan_measure(double *pdb_distance)
{

    // Validate incoming signal
    int n_is_valid;
    T_FREQ_INFO *pt_freq;

    pt_freq = &gta_meas_info[0];
    n_is_valid = TRUE;
    while(pt_freq->us_freq)
    {
        n_is_valid &= pt_freq->n_valid;
        pt_freq++;
    }

    if (!n_is_valid)
    {
        swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n    Not valid            ");
        log_meas(gca_meas_log);
        return FALSE;
    }

    // -------------------------------------------------------------
    // --- Rough disatnce estimation stage 1
    // -------------------------------------------------------------
#define ROUGH_DIST_1_NUM    4

    double dba_dist_rough_1[ROUGH_DIST_1_NUM];
    double dba_dist_rough_1_1[ROUGH_DIST_1_NUM];
    double dba_dist_rough_1_2[ROUGH_DIST_1_NUM];
    double dba_period_1[ROUGH_DIST_1_NUM];

    double db_rough_avg_1;
    double db_rough_avg_1_1;
    double db_rough_avg_1_2;
    int n_exclude_cnt, n_exclude_num;

    // rought distance pairs stage 0
    // #  L1(mm)     L2(mm)     T(m)
    // 0 310.8(1)   321.4(3)    9.4
    // 1 310.8(1)   318.6(2)	12.7
    // 2 308.5(0)   321.4(3)    7.7
    // 3 308.5(0)   318.6(2)    9.7

    // Choose closer to 0 or closer to T
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 0 --- ");
    log_meas(gca_meas_log);

    dba_dist_rough_1_1[0] = measure_dual_freq(&gta_meas_info[1], &gta_meas_info[3], 0);
    dba_dist_rough_1_1[1] = measure_dual_freq(&gta_meas_info[1], &gta_meas_info[2], 0);
    dba_dist_rough_1_1[2] = measure_dual_freq(&gta_meas_info[0], &gta_meas_info[3], 0);
    dba_dist_rough_1_1[3] = measure_dual_freq(&gta_meas_info[0], &gta_meas_info[2], 0);

    dba_period_1[0] = measure_dual_maxd(&gta_meas_info[1], &gta_meas_info[3]);
    dba_period_1[1] = measure_dual_maxd(&gta_meas_info[1], &gta_meas_info[2]);
    dba_period_1[2] = measure_dual_maxd(&gta_meas_info[0], &gta_meas_info[3]);
    dba_period_1[3] = measure_dual_maxd(&gta_meas_info[0], &gta_meas_info[2]);

    swprintf(gca_meas_log , MEAS_LOG_STR_LEN, L"\n                             Dist @0 (mm)     : ");
    swprintf(gca_meas_log1, MEAS_LOG_STR_LEN, L"\n                             Dist @T (mm)     : ");

    for(int n_i = 0; n_i < ROUGH_DIST_1_NUM; n_i++)
    {
        // temp
        if (dba_period_1[n_i] < dba_dist_rough_1_1[n_i])
            while(1);

        if ((dba_period_1[n_i] - dba_dist_rough_1_1[n_i]) < 700)
        { // to close to period boundary, try -T
            dba_dist_rough_1_2[n_i] = dba_dist_rough_1_1[n_i];
            dba_dist_rough_1_1[n_i] = 1;
        }
        else if (dba_dist_rough_1_1[n_i] < 500)
        { // to close to 0 try +T
            dba_dist_rough_1_2[n_i] = dba_period_1[n_i];
        }
        else
        { 
            dba_dist_rough_1_2[n_i] = dba_dist_rough_1_1[n_i];
        }
        
        swprintf(gca_meas_log , MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, dba_dist_rough_1_1[n_i]);
        swprintf(gca_meas_log1, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log1, dba_dist_rough_1_2[n_i]);
    }

    log_meas(gca_meas_log);
    log_meas(gca_meas_log1);

    // 1_1 closer to 0, 1_2 closer to period boundary T
    // get distribution for 1_1 and 1_2
    double *pdb_winner;
    db_rough_avg_1_1 = meas_distr(dba_dist_rough_1_1, ROUGH_DIST_1_NUM);
    db_rough_avg_1_2 = meas_distr(dba_dist_rough_1_2, ROUGH_DIST_1_NUM);
    if (db_rough_avg_1_1 > db_rough_avg_1_2)
    { // closer to T wins
        pdb_winner = &dba_dist_rough_1_2[0];
        swprintf(gca_meas_log1 , MEAS_LOG_STR_LEN, L"%s% !", gca_meas_log);
    }
    else
    { // closer to 0 wins
        pdb_winner = &dba_dist_rough_1_1[0];
        swprintf(gca_meas_log , MEAS_LOG_STR_LEN, L"%s% !", gca_meas_log);
    }

    for(int n_i = 0; n_i < ROUGH_DIST_1_NUM; n_i++) 
    { // Pass the winners to the next stage
        dba_dist_rough_1[n_i] = pdb_winner[n_i];
    }

    // ------------------------- end of stage 0 ------------------------------------


    // Exclude most friendless
    n_exclude_cnt = 0;

    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 1 --- ");
    do
    {
        swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s    Rough Dist (mm) : ", gca_meas_log);
            
        for(int n_i = 0; n_i < ROUGH_DIST_1_NUM; n_i++)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, dba_dist_rough_1[n_i]);
        }

//        n_exclude_num = exclude_far_from_avg(dba_dist_rough_1, ROUGH_DIST_1_NUM, 200);
        n_exclude_num  = exclude_most_friendless(dba_dist_rough_1, ROUGH_DIST_1_NUM, 700);
        if (n_exclude_num >= 0)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s excl.(%-07.1f),  ", gca_meas_log, dba_dist_rough_1[n_exclude_num]);
            dba_dist_rough_1[n_exclude_num] = 0;
            n_exclude_cnt++;
        }

        log_meas(gca_meas_log);

        if (n_exclude_num >= 0 && n_exclude_cnt < 2 )
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n                         ");
        }

    }while(n_exclude_num >= 0 && n_exclude_cnt < 2 ); // exit if no more exclusions OR too many exclusions

    db_rough_avg_1 = meas_avg(dba_dist_rough_1, ROUGH_DIST_1_NUM);
    if (db_rough_avg_1 < 0) db_rough_avg_1 = 50;
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L" AVG = %-07.1f", db_rough_avg_1);
    log_meas(gca_meas_log);

    if (db_rough_avg_1 > 8000)
    {
        log_meas(L" >8M");
        return FALSE;
    }

    // -------------------------------------------------------------
    // --- Rough disatnce estimation stage 2
    // -------------------------------------------------------------
#define ROUGH_DIST_2_NUM    4

    double dba_period_2[ROUGH_DIST_2_NUM];
    double db_rough_avg_2;

    double dba_dist_rough_2[ROUGH_DIST_2_NUM];
    // rought distance pairs stage 2
    //                          1	    2	    3	    4	    5
    //  308.5(0)	371.4(4)    1.8	    3.6	    5.5	    7.3     9.1
    //  310.8(1)	371.4(4)	1.9	    3.8	    5.7	    7.6	    9.5
    //  318.6(2)	371.4(4)	2.2	    4.5	    6.7	    9.0	    11.2
    //  321.4(3)	371.4(4)	2.4	    4.8	    7.2	    9.5	    11.9

    dba_period_2[0] = measure_dual_maxd(&gta_meas_info[0], &gta_meas_info[4]);
    dba_period_2[1] = measure_dual_maxd(&gta_meas_info[1], &gta_meas_info[4]);
    dba_period_2[2] = measure_dual_maxd(&gta_meas_info[2], &gta_meas_info[4]);
    dba_period_2[3] = measure_dual_maxd(&gta_meas_info[3], &gta_meas_info[4]);

    dba_dist_rough_2[0] = measure_dual_freq(&gta_meas_info[0], &gta_meas_info[4], db_rough_avg_1);
    dba_dist_rough_2[1] = measure_dual_freq(&gta_meas_info[1], &gta_meas_info[4], db_rough_avg_1);
    dba_dist_rough_2[2] = measure_dual_freq(&gta_meas_info[2], &gta_meas_info[4], db_rough_avg_1);
    dba_dist_rough_2[3] = measure_dual_freq(&gta_meas_info[3], &gta_meas_info[4], db_rough_avg_1);

    // Corr +/- double period. Exclude too far from avg
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 2 --- " );
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s    Rough Dist (mm) : ", gca_meas_log);

    
    for(int n_i = 0; n_i < ROUGH_DIST_2_NUM; n_i++)
    {
        double db_correction;

        db_correction = 0;
        // try to correct distance +/- 1 period if fine distance is too far from average
        if  (  abs(dba_dist_rough_2[n_i] - db_rough_avg_1) > 500 )
        {   
            
            db_correction = dba_period_2[n_i];    // try +1
            if (dba_dist_rough_2[n_i] > db_rough_avg_1)
            { // try -1
                db_correction = -db_correction;
            }
            
            if  (  abs(dba_dist_rough_2[n_i] + db_correction - db_rough_avg_1) > 500 )
            { // Discard measure if correction didn't help
                swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s(%-07.1f),  ", gca_meas_log, dba_dist_rough_2[n_i]);
                dba_dist_rough_2[n_i] = 0;
            }

            if (dba_dist_rough_2[n_i] != 0)
            {
                dba_dist_rough_2[n_i] += db_correction;
                if (db_correction > 0)
                {
                    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f(+),  ", gca_meas_log, dba_dist_rough_2[n_i]);
                }
                else
                {
                    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f(-),  ", gca_meas_log, dba_dist_rough_2[n_i]);
                }
            }

        }

        if ((db_correction == 0) && (dba_dist_rough_2[n_i] != 0))
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, dba_dist_rough_2[n_i]);
        

    } // End of stage 2 rough FOR

    db_rough_avg_2 = meas_avg(dba_dist_rough_2, ROUGH_DIST_2_NUM);
    if (db_rough_avg_2 < 0) db_rough_avg_2 = 50;

    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s AVG = %-07.1f,  ", gca_meas_log, db_rough_avg_2);
    log_meas(gca_meas_log);

    if (db_rough_avg_2 == 0)
    { // No more valid measures
        return FALSE;
    }

#if 0
    double dba_period_2[ROUGH_DIST_2_NUM];
    double db_rough_avg_2;

    dba_period_2[0] = measure_dual_maxd(&gta_meas_info[0], &gta_meas_info[4]);
    dba_period_2[1] = measure_dual_maxd(&gta_meas_info[1], &gta_meas_info[4]);
    dba_period_2[2] = measure_dual_maxd(&gta_meas_info[2], &gta_meas_info[4]);
    dba_period_2[3] = measure_dual_maxd(&gta_meas_info[3], &gta_meas_info[4]);

    // Exclude rough estimation where average is too close to the period boundary
    db_rough_avg_2 = 0;

    // Exclude near dual period
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 2 --- " );
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s    Rough Dist (mm) : ", gca_meas_log);

    n_exclude_cnt = 0;
    for(int n_i = 0; n_i < ROUGH_DIST_2_NUM; n_i++)
    {
        int n_j;

        swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, dba_dist_rough_2[n_i]);

        if (n_exclude_cnt < 1) 
        { // single exclusion only

            for (n_j = 1; n_j < 5; n_j ++)
            {
                if (abs((dba_period_2[n_i] * n_j) - db_rough_avg_1) < 100)
                { // too close to period boundary - exclude! ??? or try +/- double freq period correction ???
                    n_exclude_cnt ++; 
                    n_exclude_num = n_i;

                    break;
                }
            }
        } 
    }

    if (n_exclude_cnt)
    {
        swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s excl.(%-07.1f),  ", gca_meas_log, dba_dist_rough_2[n_exclude_num]);
        dba_dist_rough_2[n_i] = 0;
    }

    db_rough_avg_2 = meas_avg(dba_dist_rough_2, ROUGH_DIST_2_NUM);

    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s AVG = %-07.1f,  ", gca_meas_log, db_rough_avg_2);
    log_meas(gca_meas_log);

#endif  // disable stage 2

    // -------------------------------------------------------------
    // --- Rough disatnce estimation stage 3
    // -------------------------------------------------------------
    double db_rough_avg_3;
    
    // Exclude most friendless
    n_exclude_cnt = 0;
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 3 --- " );
    do
    {
        swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s    Rough Dist (mm) : ", gca_meas_log);
            
        for(int n_i = 0; n_i < ROUGH_DIST_2_NUM; n_i++)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, dba_dist_rough_2[n_i]);
        }

//        n_exclude_num = exclude_far_from_avg(dba_dist_rough_2, ROUGH_DIST_2_NUM, 200);
        n_exclude_num  = exclude_most_friendless(dba_dist_rough_2, ROUGH_DIST_2_NUM, 100);
        if (n_exclude_num >= 0)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s excl.(%-07.1f),  ", gca_meas_log, dba_dist_rough_2[n_exclude_num]);
            dba_dist_rough_2[n_exclude_num] = 0;
            n_exclude_cnt++;
        }

        log_meas(gca_meas_log);

        if (n_exclude_num >= 0 && n_exclude_cnt < 2 )
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n                         ");
        }

    }while(n_exclude_num >= 0 && n_exclude_cnt < 2 ); // exit if no more exclusions OR too many exclusions

    db_rough_avg_3 = meas_avg(dba_dist_rough_2, ROUGH_DIST_2_NUM);
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"AVG = %-07.1f,  ", db_rough_avg_3);
    log_meas(gca_meas_log);

    // -------------------------------------------------------------
    // --- Fine disatnce estimation stage 1
    // -------------------------------------------------------------
#define FINE_DIST_NUM 5

    double db_fine_avg;
    double dba_dist_fine[FINE_DIST_NUM];

    // Corr +/- period. Exclude too far from avg
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 4 --- " );
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s    Fine Dist (mm) : ", gca_meas_log);

    for(int n_i = 0; n_i < FINE_DIST_NUM; n_i++)
    {
        double db_correction;

        measure_single_freq(&gta_meas_info[n_i], db_rough_avg_3);

        db_correction = 0;
        // try to correct distance +/- 1 period if fine distance is too far from average
        if  (  abs(gta_meas_info[n_i].db_distance - db_rough_avg_3) > 200 )
        {   
            
            db_correction = gta_meas_info[n_i].db_wave_len;    // try +1
            if (gta_meas_info[n_i].db_distance > db_rough_avg_3)
            { // try -1
                db_correction = -db_correction;
            }
            
            // remeasure with correction
            measure_single_freq(&gta_meas_info[n_i], db_rough_avg_3 + db_correction);
            if  (  abs(gta_meas_info[n_i].db_distance - db_rough_avg_3) > 60 )
            { // Ignore fine distance if correction didn't help
                double db_corrected;
                db_corrected = gta_meas_info[n_i].db_distance;
                measure_single_freq(&gta_meas_info[n_i], db_rough_avg_3);   // restore original value
                swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s(%7.1f, %7.1f),  ", gca_meas_log, gta_meas_info[n_i].db_distance, db_corrected );
                gta_meas_info[n_i].db_distance = 0;
            }

            if (gta_meas_info[n_i].db_distance != 0)
            {
                if (db_correction > 0)
                {
                    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f(+),  ", gca_meas_log, gta_meas_info[n_i].db_distance);
                }
                else
                {
                    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f(-),  ", gca_meas_log, gta_meas_info[n_i].db_distance);
                }
            }

        }

        if ((db_correction == 0) && (gta_meas_info[n_i].db_distance != 0))
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, gta_meas_info[n_i].db_distance);

        dba_dist_fine[n_i] = gta_meas_info[n_i].db_distance;
        
    } // End of fine FOR

    log_meas(gca_meas_log);


    // Exclude most friendless
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n         --- Stage 5 --- " );
    n_exclude_cnt = 0;
    do
    {
        swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s    Fine Dist (mm) : ", gca_meas_log);
            
        for(int n_i = 0; n_i < FINE_DIST_NUM; n_i++)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s%-07.1f,  ", gca_meas_log, dba_dist_fine[n_i]);
        }

        n_exclude_num  = exclude_most_friendless(dba_dist_fine, FINE_DIST_NUM, 15);
        if (n_exclude_num >= 0)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"%s excl.(%-07.1f),  ", gca_meas_log, dba_dist_fine[n_exclude_num]);
            dba_dist_fine[n_exclude_num] = 0;
            n_exclude_cnt++;
        }

        log_meas(gca_meas_log);

        if (n_exclude_num >= 0)
        {
            swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n                         ");
        }

    }while(n_exclude_num >= 0 && n_exclude_cnt < 2 ); // exit if no more exclusions OR too many exclusions

    db_fine_avg = meas_avg(dba_dist_fine, FINE_DIST_NUM);


    // --- Final result in  db_fine_avg
    swprintf(gca_meas_log, MEAS_LOG_STR_LEN, L"\n    int = %-07.1f    ext = %d    delata = %-07.1f", db_fine_avg, gus_distance, (double)(db_fine_avg - gus_distance));
    log_meas(gca_meas_log);

    *pdb_distance = db_fine_avg;

    return TRUE;
    
}


void measure_init_scan(WORD us_apd_meas)
{
    gt_apd_auto.us_apd_value = us_apd_meas; 
    gt_apd_auto.n_curr_apd_idx = -1;

    { // Init measure info structure
        T_FREQ_INFO *pt_freq;

        pt_freq = &gta_meas_info[0];
        while(pt_freq->us_freq)
        {
            pt_freq->t_meas_sig.db_angle_deg  = 0;
            pt_freq->t_calib_sig.db_angle_deg = 0;
            pt_freq++;
        }
    }

}

void log_meas(WCHAR *pc_str)
{
    DWORD dw_temp;
    int n_rc;

    n_rc = WriteFile( gh_meas_log_file, (void*)pc_str, (wcslen(pc_str)+1)*2, &dw_temp, NULL);
}










