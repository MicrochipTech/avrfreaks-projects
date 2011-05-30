/*
 * spi_com.h
 *
 * Created: 29-05-2011 00:59:20
 *  Author: limapapy
 */ 


#ifndef SPI_COM_H_
#define SPI_COM_H_

void SPI_MasterConfig(void);
void SPI_send(unsigned char cData);
void set_xlat(void);
void clear_xlat(void);
void set_blank(void);
void clear_blank(void);
void togle_blank(void);



#endif /* SPI_COM_H_ */