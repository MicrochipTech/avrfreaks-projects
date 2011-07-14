
#define APD_AUTO_MAX    0x9050
#define APD_AUTO_MIN    0x9000

#define APD_AUTO_SIG_PARAM_NUM 5
#define APD_AUTO_APD_PARAM_NUM 6

typedef struct meas_sig_param_tag{

    INT16  s_sin;
    INT16  s_cos;
    UINT16 us_phase_int;
    UINT16 us_phase_ext;
    UINT8  uc_pwr_range_int;

    double db_power_mV;
    double db_angle_deg;

} T_SIG_PARAM;

typedef struct freq_info_tag{

    UINT16  us_freq;
    double  db_wave_len;

    int n_valid_int;
    int n_valid_ext;
    int n_valid;

    T_SIG_PARAM t_meas_sig;
    T_SIG_PARAM t_calib_sig;

    UINT8   uc_pwr_range_ext;
    UINT8   uc_pwr_range_int;
    UINT8   uc_pwr_range;

    UINT16 us_phase_ext;

    double db_phase_int;    // calculated by host
    double db_phase_ext;    // calculated by MCU
    double db_phase;        // either db_phase_ext or db_phase_int

    int    n_periods;
    double db_distance;

} T_FREQ_INFO;


typedef struct apd_auto_apd_param_tag{
    WORD us_level;
    int  n_change;
} T_APD_AUTO_APD_PARAM;

typedef struct apd_auto_sig_param_tag{
    const WCHAR *pc_sig_level_str;
    double db_sig_level;
    const T_APD_AUTO_APD_PARAM t_apd_param[APD_AUTO_APD_PARAM_NUM];
} T_APD_AUTO_SIG_PARAM;

typedef struct apd_auto_tag{
    int n_curr_sig_idx;

    WORD us_apd_value;
    int n_curr_apd_idx;

    T_APD_AUTO_SIG_PARAM ta_sig_apd_table[APD_AUTO_SIG_PARAM_NUM];
} T_APD_AUTO;


extern T_APD_AUTO gt_apd_auto;
extern WORD gus_distance;
extern WORD gus_timestamp;
extern BYTE guc_pwr_range;
extern BYTE guc_err_code;


//extern T_FREQ_TABLE  gta_calib_table[];
//extern T_FREQ_TABLE  gta_meas_table[];

extern T_FREQ_INFO gta_meas_info[];

//void measure_stream2sincos(T_FREQ_TABLE *pt_freq_table,  int n_burst_size, unsigned short *pus_burst);
//void measure_sincos2phase(T_FREQ_TABLE *pt_freq_table);
//double measure_phases2distance(T_FREQ_TABLE *pt_freq_table, int n_freq_num, int *pn_error);

//void proceed_scan_meas(T_FREQ_TABLE *pt_freq_table);

void measure_sincos2param(T_SIG_PARAM *pt_sig);
double measure_dual_freq(T_FREQ_INFO *pt_freq_1, T_FREQ_INFO *pt_freq_2, double db_rough_dist);
double measure_dual_maxd(T_FREQ_INFO *pt_freq_1, T_FREQ_INFO *pt_freq_2);
double measure_single_freq(T_FREQ_INFO *pt_freq, double db_dist_rough);
int scan_measure(double *pdb_distance);
void measure_init_scan(WORD us_apd_meas);



