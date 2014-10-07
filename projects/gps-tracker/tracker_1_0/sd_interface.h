/*
 * sd_interface.h
 *
 * Created: 19.3.2014 16:14:00
 *  Author: PjV
 *
 * Version: 1.0 
 */ 


#ifndef SD_INTERFACE_H_
#define SD_INTERFACE_H_

unsigned char SD_Initialize(void);
unsigned char SD_CreateOpenedFile(void);
unsigned char SD_WriteToFile(const char* buf);
unsigned char SD_Close(void);
unsigned char SD_CloseKmlFile(void);

#endif /* SD_INTERFACE_H_ */