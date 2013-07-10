#ifndef _OV7670_H
#define _OV7670_H

#include "avr_compiler_.h"
#include "VARIABLE.h"
#include "SCCB.h"
#include "delay.h"

#define CHANGE_REG_NUM 176

uchar wrOV7670Reg(uchar regID, uchar regDat);
uchar rdOV7670Reg(uchar regID, uchar *regDat);
//void OV7670_config_window(uint startx,uint starty,uint width, uint height);
uchar OV7670_init(void);

#endif



