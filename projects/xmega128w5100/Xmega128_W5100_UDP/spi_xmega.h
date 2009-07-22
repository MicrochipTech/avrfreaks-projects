
#ifndef	_SPI_H_
#define	_SPI_H_
#endif


/* Hardware defines */
//PIN
#define SPID_SS_bm             0x10 //pin4
#define SPID_MOSI_bm           0x20 //pin5
#define SPID_MISO_bm           0x40 //pin6
#define SPID_SCK_bm            0x80 //pin7

#define SPIC_SS_bm             0x10 //pin4
#define SPIC_MOSI_bm           0x20 //pin5
#define SPIC_MISO_bm           0x40 //pin6
#define SPIC_SCK_bm            0x80 //pin7 


void SPIC_Init();
void SPID_Init(); //need to be call at startup
void IINCHIP_SpiSendData(uint8_t data);
uint8_t IINCHIP_SpiRecvData();

#define IINCHIP_CSon()					(PORTD.OUTSET = SPID_SS_bm)
#define IINCHIP_CSoff()					(PORTD.OUTCLR = SPID_SS_bm)
