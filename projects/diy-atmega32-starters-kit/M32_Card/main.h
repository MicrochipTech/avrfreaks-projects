
#define pushButton1  0x1c
#define pushButton2	 0x1a
#define pushButton3  0x16
#define pushButton4  0x0e

void port_init(void);
void uart0_init(void);
void uart0_rx_isr(void);
void twi_init(void);
void init_devices(void);
void checkKeys(void);
void checkLED(void);
