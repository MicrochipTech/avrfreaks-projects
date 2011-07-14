void dle_cmd_io_dev_sign();
void dle_cmd_io_init();
void dle_cmd_io_spi_wr();
void dle_cmd_io_spi_rd();
void dle_cmd_io_apd_on();
void dle_cmd_io_apd_off();
void dle_cmd_io_aim();
void dle_cmd_io_curtain(int n_curtain_act);
void dle_cmd_io_measure();
void dle_cmd_io_lcd();

#define MEAS_TYPE_STREAM    1
#define MEAS_TYPE_SINCOS    2
#define MEAS_TYPE_DISTANCE  4
