/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef ISPCB_H
#define ISPCB_H

	// DEFINES:
	#define AICB_MESSAGE_START             0x1B
	#define AICB_TOKEN                     0x0E
	
	#define AICB_CMD_SIGN_ON               0x01
	#define AICB_CMD_SET_PARAMETER         0x02
	#define AICB_CMD_GET_PARAMETER         0x03
	#define AICB_CMD_OSCCAL                0x05
	#define AICB_CMD_LOAD_ADDRESS          0x06
	#define AICB_CMD_FIRMWARE_UPGRADE      0x07
	#define AICB_CMD_ENTER_PROGMODE_ISP    0x10
	#define AICB_CMD_LEAVE_PROGMODE_ISP    0x11
	#define AICB_CMD_CHIP_ERASE_ISP        0x12
	#define AICB_CMD_PROGRAM_FLASH_ISP     0x13
	#define AICB_CMD_READ_FLASH_ISP        0x14
	#define AICB_CMD_PROGRAM_EEPROM_ISP    0x15
	#define AICB_CMD_READ_EEPROM_ISP       0x16
	#define AICB_CMD_PROGRAM_FUSE_ISP      0x17
	#define AICB_CMD_READ_FUSE_ISP         0x18
	#define AICB_CMD_PROGRAM_LOCK_ISP      0x19
	#define AICB_CMD_READ_LOCK_ISP         0x1A
	#define AICB_CMD_READ_SIGNATURE_ISP    0x1B
	#define AICB_CMD_READ_OSCCAL_ISP       0x1C
	#define AICB_CMD_SPI_MULTI             0x1D
	
	#define AICB_PARAM_BUILD_NUMBER_LOW    0x80
	#define AICB_PARAM_BUILD_NUMBER_HIGH   0x81
	#define AICB_PARAM_HARDWARE_VERSION    0x90
	#define AICB_PARAM_SW_MAJOR            0x91
	#define AICB_PARAM_SW_MINOR            0x92
	#define AICB_PARAM_VTARGET             0x94
	#define AICB_PARAM_VADJUST             0x95
	#define AICB_PARAM_OSC_PSCALE          0x96
	#define AICB_PARAM_OSC_CMATCH          0x97
	#define AICB_PARAM_SCK_DURATION        0x98
	#define AICB_PARAM_TOPCARD_DETECT      0x9A
	#define AICB_PARAM_STATUS              0x9C
	#define AICB_PARAM_DATA                0x9D
	#define AICB_PARAM_RESET_POLARITY      0x9E
	#define AICB_PARAM_CONTROLLER_INIT     0x9F
	
	#define AICB_STATUS_CMD_OK             0x00
	#define AICB_STATUS_CMD_TOUT           0x80
	#define AICB_STATUS_CMD_FAILED         0xC0
	#define AICB_STATUS_CKSUM_ERROR        0xC1
	#define AICB_STATUS_CMD_UNKNOWN        0xC9
	
#endif
