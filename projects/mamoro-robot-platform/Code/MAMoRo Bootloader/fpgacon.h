/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	9.4.2007
*
*Slave serial configuration of Xilinx Spartan-III FPGA's
*Tested with Spartan 3 XC3S400
*
****************************************************************************/

/* input pins */
#define INIT_PIN PC5
#define PROG_PIN PC3 //can be set as output to start re-configurtion of the FPGA
#define DONE_PIN PC0

/* output pins */
#define CCLK_PIN PC2
#define DIN_PIN PC1 // input & output

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

void configure_fpga(unsigned int DF_START_ADR, unsigned long FILE_SIZE);
void initialize(void);
void restart_fpga(void);
void upload_data(unsigned int DF_START_ADR, unsigned long FILE_SIZE);
void read_done(void);
void wait_ms(unsigned int ms);
