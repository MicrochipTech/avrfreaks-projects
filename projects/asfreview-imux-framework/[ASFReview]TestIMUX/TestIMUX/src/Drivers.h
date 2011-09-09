/*
 * Drivers.c
 *
 * Created: 21/08/2011 20:51:26
 *  Author: Guillem Planisi
 */
#ifndef _Driver_H
#define _Driver_H

//Command definition for SPI Flash memory handling
#define CmdREMs  0x90  //Read Electronic Manufacturer
#define CmdRES   0xAB  //Release Deep Powerdown and Read Electronic ID
#define CmdREAD  0x03  //Read Data
#define CmdFaRD  0x0B  //FastRead
#define CmdWREn  0x06  //Write Enabe
#define CmdWRDI  0x04  //Write Disable
#define CmdRDId  0x9F  //Read ID
#define CmdRDSR  0x05  //Read Status Register
#define CmdWRSR  0x01  //Write Status Register
#define CmdSE    0x20  //Sector Erase
#define CmdBE    0x52  //Block Erase
#define CmdCE    0x60  //Chip Erase
#define CmdPP    0x02  //Page Program.
#define CmdRDP   0xB9  //Deep Powerdown

char SPI_Sync(SPI_t *SPIPort, char *Buff, unsigned int Count);
unsigned int SPIFlash_ReadID(SPI_t * SPIPort);

#endif

