
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>


#include <stdbool.h>
#define F_CPU 16000000UL
#include <util/delay.h>
extern  volatile unsigned int displmod;         //counter for display mode
extern  volatile char modus;
extern char sample_rate;      // at start up 5 min sample interval

extern bool NBM;           //NoBusMode
extern volatile bool read;          //triggers ds1820 reading
extern volatile bool update;        // update display
extern volatile bool keyin;
// menu modus
extern void read_deviceT(void);
extern bool resetL(void);
extern void generate(void);
extern void compute_E(void);
extern void compute_P(void);
extern void compute_Trend (void);
extern void displ_energy(void);
extern void displ_Trend (void);
extern void transfer(void);
extern void ds_read_id(void) ;
extern void displ_calendar(void);
extern void display(void);
extern void Init(void);
extern bool LCD (void);
extern void keyp_exe();
extern void clear (void);
extern bool resetL();
extern void readSdeviceT(void);
extern volatile char base;
extern char display1[25];
extern char display2[25];

