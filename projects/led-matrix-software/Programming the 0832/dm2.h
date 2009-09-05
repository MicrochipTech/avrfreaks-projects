//      dm2.h - header file for horizontal multiboard LED display matrix


extern void dm_blink(char);                     // set blink on/off
extern void dm_enable(char);                    // set enable on/off
extern void dm_setpwm(char);                    // set PWM duty cycle

extern char dm_rcol(char);                      // read column of entire display
extern void dm_wcol(char, char);                // write column of entire display
extern int dm_rbytes(char, char *, int);        // read bytes from display memory
extern int dm_wbytes(char, char *, int);        // write bytes to display memory

extern void dm_lscroll(char);                   // scroll display left 1 column
extern void dm_rscroll(char);                   // scroll display right 1 column

extern char dm_rbit(char, char);                // read one bit from display
extern void dm_wbit(char, char, char);          // write one bit to display

extern void dm_blankall(void);                  // blank all boards
extern int dm_blankcols(char, int);             // blank columns
extern int dm_fill(char, int, char);            // write pattern to multiple columns

extern void dm_init(char);                      // initialization

//      global variables
extern char dm_nboards;                         // number of active boards (1 - 4)
extern unsigned char dm_cmax;                   // rightmost column (31, 63, 95, or 127)


#define COMMONS 0x28            // PMOS open, 8
#define SET_MASTER 0x14         // master mode
#define SET_SLAVE 0x10          // slave mode
#define LED_ON 0x03             // display LEDs on
#define LED_OFF 0x02            // display LEDs off
#define SYS_DIS 0x00            // system disable
#define SYS_EN 0x01             // system enable
#define BLINK_ON 0x09           // blink on
#define BLINK_OFF 0x08          // blink off
#define SET_PWM 0xa0            // set PWM level (OR 0000 - 1111)

#define DM_PORT PORTA           // port to use
#define DM_DDR  DDRA
#define DM_PIN  PINA
#define DM_CS0  (1 << 0)        // CS for master
#define DM_CS1  (1 << 4)        // CS for first slave
#define DM_CS2  (1 << 5)        // CS for second slave
#define DM_CS3  (1 << 6)        // CS for third slave
#define DM_RD   (1 << 1)        // read
#define DM_WR   (1 << 2)        // write
#define DM_DATA (1 << 3)        // data
