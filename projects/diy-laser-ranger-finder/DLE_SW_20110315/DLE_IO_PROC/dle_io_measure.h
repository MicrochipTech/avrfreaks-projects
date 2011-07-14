
typedef struct freq_table_tag{

    UINT16  us_freq;
    double  db_wave_len;

    INT16  s_sin;
    INT16  s_cos;

//    UINT16  us_atan_rad_3q13;

    double db_power;
    double db_angle_grad;
    int    n_periods;
    double db_distance;

} T_FREQ_TABLE;

extern T_FREQ_TABLE  gta_calib_table[];
extern T_FREQ_TABLE  gta_meas_table[];

void measure_stream2sincos(T_FREQ_TABLE *pt_freq_table,  int n_burst_size, unsigned short *pus_burst);
void measure_sincos2phase(T_FREQ_TABLE *pt_freq_table);
double measure_phases2distance(T_FREQ_TABLE *pt_freq_table, int n_freq_num, int *pn_error);



