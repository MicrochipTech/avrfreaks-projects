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
#include "dle_io_measure.h"

unsigned short pol_atan(short s_sampl_cos, short s_sampl_sin);
double measure_sph(int n_f, double db_dist_rough);
double measure_dph(int n_f1, int n_f2, double db_rough_dist);

T_FREQ_TABLE  gta_meas_table[] = {

    {0x37E4, 308.5,     0, 0, 0, 0,     0, 0}, 
    {0x2BE4, 309.9,     0, 0, 0, 0,     0, 0}, 
    {0x1EE4, 310.8,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xC9D5, 318.6,     0, 0, 0, 0,     0, 0}, 
    {0xBDD5, 319.5,     0, 0, 0, 0,     0, 0}, 
    {0xB5D5, 320.2,     0, 0, 0, 0,     0, 0}, 
    {0xA9D5, 321.4,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xD3B9, 371.4,     0, 0, 0, 0,     0, 0}, 
    {0xC7B9, 372.6,     0, 0, 0, 0,     0, 0}, 
    {0xB4B9, 375  ,     0, 0, 0, 0,     0, 0}, 
    {0xA8B9, 376.5,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xDEAC, 404  ,     0, 0, 0, 0,     0, 0}, 
    {0xD2AC, 405.4,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xABAD, 411.5,     0, 0, 0, 0,     0, 0}, 
    {0x9FAD, 413.2,     0, 0, 0, 0,     0, 0}, 
};


T_FREQ_TABLE  gta_calib_table[] = {

    {0x37E4, 308.5,     0, 0, 0, 0,     0, 0}, 
    {0x2BE4, 309.9,     0, 0, 0, 0,     0, 0}, 
    {0x1EE4, 310.8,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xC9D5, 318.6,     0, 0, 0, 0,     0, 0}, 
    {0xBDD5, 319.5,     0, 0, 0, 0,     0, 0}, 
    {0xB5D5, 320.2,     0, 0, 0, 0,     0, 0}, 
    {0xA9D5, 321.4,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xD3B9, 371.4,     0, 0, 0, 0,     0, 0}, 
    {0xC7B9, 372.6,     0, 0, 0, 0,     0, 0}, 
    {0xB4B9, 375  ,     0, 0, 0, 0,     0, 0}, 
    {0xA8B9, 376.5,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xDEAC, 404  ,     0, 0, 0, 0,     0, 0}, 
    {0xD2AC, 405.4,     0, 0, 0, 0,     0, 0}, 
                                            
    {0xABAD, 411.5,     0, 0, 0, 0,     0, 0}, 
    {0x9FAD, 413.2,     0, 0, 0, 0,     0, 0}, 
};


void measure_stream2sincos(T_FREQ_TABLE *pt_freq_table,  int n_burst_size, unsigned short *pus_burst){

    int n_i;
    unsigned short us_sampl_1, us_sampl_2, us_sampl_3, us_sampl_4;
    int n_rnd_const;

    us_sampl_1 = us_sampl_2 = us_sampl_3 = us_sampl_4 = 0;
    for (n_i = 0; n_i < n_burst_size; n_i += 4)
    {
        us_sampl_1 += pus_burst[n_i + 0];
        us_sampl_2 += pus_burst[n_i + 1];
        us_sampl_3 += pus_burst[n_i + 2];
        us_sampl_4 += pus_burst[n_i + 3];
    }

    n_rnd_const = (n_burst_size >> 1) >> 2;
    us_sampl_1 = (us_sampl_1 + n_rnd_const) / (n_burst_size >> 2);
    us_sampl_2 = (us_sampl_2 + n_rnd_const) / (n_burst_size >> 2);
    us_sampl_3 = (us_sampl_3 + n_rnd_const) / (n_burst_size >> 2);
    us_sampl_4 = (us_sampl_4 + n_rnd_const) / (n_burst_size >> 2);

    pt_freq_table->s_cos = (INT16)(us_sampl_1 - us_sampl_3);
    pt_freq_table->s_sin = (INT16)(us_sampl_2 - us_sampl_4);

}

void measure_sincos2phase(T_FREQ_TABLE *pt_freq_table){

    unsigned long  ul_sampl_pwr;

    ul_sampl_pwr = pt_freq_table->s_sin * pt_freq_table->s_sin + pt_freq_table->s_cos * pt_freq_table->s_cos;
    pt_freq_table->db_power = sqrt((double)ul_sampl_pwr);

//    us_angle = pol_atan((s_sampl_cos + 16) >>5, (s_sampl_sin+16)>>5); 
//    pt_freq_table->us_atan_rad_3q13 = us_angle;
//    pt_freq_table->db_angle_grad = (double)us_angle/8192 / (2*3.141593) * 360;

    pt_freq_table->db_angle_grad = atan2((double)pt_freq_table->s_sin, (double)pt_freq_table->s_cos) / (2*3.141593) * 360;
    if (pt_freq_table->db_angle_grad < 0) pt_freq_table->db_angle_grad += 360;

    return;
}

double measure_phases2distance(T_FREQ_TABLE *pt_freq_table, int n_freq_num, int *pn_error){
    
    int n_f1, n_f2, uc_i;
    double db_dist_rough[5];

    if (pn_error) *pn_error = FALSE;

    if (n_freq_num < 14)
    {
        wprintf(L"Error in measure_phases2distance(). Rework required!");
        if (pn_error) *pn_error = TRUE;
        return 0;
    }

    // ------------------------------------
    // --- Calculate distance
    // ------------------------------------
    db_dist_rough[0] = 0;

    // F1, F3   ~56M
    n_f1 = 0; n_f2 = 2;
    db_dist_rough[1] = measure_dph(n_f1, n_f2, db_dist_rough[0]);

    // F1, F4   ~9.2M
    n_f1 = 0; n_f2 = 3;
    db_dist_rough[2] = measure_dph(n_f1, n_f2, db_dist_rough[1]);

    // F4, F8   ~1.9M
    n_f1 = 3; n_f2 = 7;
    db_dist_rough[3] = measure_dph(n_f1, n_f2, db_dist_rough[2]);

    // F3, F14  ~0.6M
    n_f1 = 2; n_f2 = 13;
    db_dist_rough[4] = measure_dph(n_f1, n_f2, db_dist_rough[3]);

    for(uc_i = 0; uc_i < n_freq_num; uc_i++)
    {
        measure_sph(uc_i, db_dist_rough[4]);
    }

    return 0;

}

unsigned short pol_atan(short s_sampl_cos, short s_sampl_sin){

    short s_norm, s_norm_p3;
    short s_abs_sin, s_abs_cos, s_tmp1, s_tmp2;
    div_t   t_norm;
    unsigned short us_atan_rad;

    s_abs_cos = abs(s_sampl_cos);
    s_abs_sin = abs(s_sampl_sin);

    // Scale Down. 7 bits enough for normal operation
    s_tmp1 = max(s_abs_cos, s_abs_sin);
    while ( s_tmp1 > 128 )
    {
        s_tmp1 >>= 1;
        s_abs_cos >>= 1;
        s_abs_sin >>= 1;
    }

    s_tmp1 = (s_abs_cos - s_abs_sin) << 7;
    s_tmp2 = (s_abs_cos + s_abs_sin);
    
    if (s_tmp2 == 0)
    {
        goto pol_atan_err;
    }

    t_norm = div( s_tmp1, s_tmp2);
    s_norm = t_norm.quot << 7;

    t_norm = div( 128 * t_norm.rem, s_tmp2);
    s_norm += t_norm.quot;
    s_norm <<= 1;

    // !Overflow control!
    // ...


    s_norm_p3 = ((s_norm * s_norm) + 0x8000) >> 15 ;           
    s_norm_p3 = ((s_norm_p3 * s_norm) + 0x8000) >> 15;         

    s_tmp1 = ((6432 * s_norm_p3) + 0x8000) >> 15;              
    s_tmp2 = ((32168 * s_norm) + 0x8000) >> 15;            

    us_atan_rad = (s_tmp1 - s_tmp2 + 25736);                   // Q16 unsigned!

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

pol_atan_err:

    return -1;
}

double measure_dph(int n_f1, int n_f2, double db_rough_dist){

    double db_dist;
    double db_dph, db_dph1, db_dph2;
    double db_n1, db_n2, db_dn12;

    db_n1 = db_rough_dist/gta_calib_table[n_f1].db_wave_len;
    db_n2 = db_rough_dist/gta_calib_table[n_f2].db_wave_len;
    db_dn12 = floor(db_n1 - db_n2);

    db_dph1 = ( gta_meas_table[n_f1].db_angle_grad -  gta_calib_table[n_f1].db_angle_grad);
    db_dph2 = ( gta_meas_table[n_f2].db_angle_grad -  gta_calib_table[n_f2].db_angle_grad);

    db_dph = (db_dph1 - db_dph2);
    if (db_dph < 0) db_dph += 360;

    db_dist = (db_dph/360 + db_dn12) *
              (gta_calib_table[n_f1].db_wave_len * gta_calib_table[n_f2].db_wave_len) /
              abs(gta_calib_table[n_f1].db_wave_len - gta_calib_table[n_f2].db_wave_len);
   
    return db_dist;
}


double measure_sph(int n_f, double db_dist_rough){

    double db_dist;
    double db_dph;
    int n_periods;

    db_dph = (gta_meas_table[n_f].db_angle_grad - gta_calib_table[n_f].db_angle_grad);
    if (db_dph < 0) db_dph += 360;

    n_periods = (int)floor(db_dist_rough/gta_calib_table[n_f].db_wave_len);
    gta_meas_table[n_f].n_periods = n_periods;

    db_dist = (n_periods + db_dph/360) * gta_calib_table[n_f].db_wave_len;
    gta_meas_table[n_f].db_distance = db_dist;

    return db_dist;
}
