#ifndef __isp_h_included__
#define __isp_h_included__ 

#include <mega162.h>
#include <spi.h>   

#define SPIF 7   
#define SCK PORTB.7 
#define RESET_SLAVE_AKTIF()     PORTB&=(0<<4)
#define RESET_SLAVE_NONAKTIF()  PORTB|=(1<<4)

//Deklarasi Prototipe Fungsi Operasi-Operasi SPI-ISP
void inisialisasiSPILowSpeed(void);
void inisialisasiSPIHighSpeed(void);
unsigned char SPIMasterKirimData(unsigned char data);
void enterProgrammingMode(void);  
void chipErase(void);  
void leaveProgrammingMode(void);  
unsigned char cekEksistensiDevais(void);

#endif