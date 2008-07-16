#ifndef _HTTPD_H_
#define _HTTPD_H_
#include <avr/pgmspace.h>
#include "../main.h"
#include "mynic.h"
#include "ip.h"
#include "tcp.h"
#include "../io/filesystem.h"
#include "../io/plot.h"
#include "../io/mca25.h"
#include "../io/servo.h"
#include "base64.h"
#include "../io/port.h"
#include "../io/led.h"
#include "../util/string.h"

//fixme: we need this to read enc revision...
#include "../io/enc28j60.h" 


#define HTTPD_STATE_IDLE             0
#define HTTPD_STATE_FILE_INDEX       1
#define HTTPD_STATE_FILE_SITE_CAM    2
#define HTTPD_STATE_FILE_SITE_TEMP   3
#define HTTPD_STATE_FILE_SITE_IO    11
#define HTTPD_STATE_FILE_GRAPH0      4
#define HTTPD_STATE_FILE_UP          5
#define HTTPD_STATE_FILE_FS_LS       6
#define HTTPD_STATE_FILE_REMOVED    14
#define HTTPD_STATE_FILE_UP_DONE    15
#define HTTPD_STATE_FILE_UP_ERR     18
#define HTTPD_STATE_FILE_MKFS       16
#define HTTPD_STATE_FILE_MKFS2      19
#define HTTPD_STATE_FILE_SERVO_OK   20
#define HTTPD_STATE_FILE_SERVO_POS  21

#define HTTPD_STATE_FILE_CAM_PIC    7
#define HTTPD_STATE_FILE_CAM_BUSY   8
#define HTTPD_STATE_FILE_SET_IO_1_PIC 12
#define HTTPD_STATE_FILE_SET_IO_0_PIC 13
#define HTTPD_STATE_FILE_SITE_FILE_ICON_PIC 17
#define HTTPD_STATE_FILE_FS_Fxxx    9
#define HTTPD_STATE_FILE_FS_xxx    10
#define HTTPD_STATE_FILE_POST    0xFE

#define HTTPD_STATE_ACTIVE    110
#define HTTPD_STATE_FINISHED  111
#define HTTPD_STATE_POST      112

//we use the lower 3 bits for rnrn detection
//the 5 higher bits can be used for misc status info (see post below)
#define HTTPD_SUBSTATE_NONE    0
#define HTTPD_SUBSTATE_R       1
#define HTTPD_SUBSTATE_RN      2
#define HTTPD_SUBSTATE_RNR     3
#define HTTPD_SUBSTATE_RNRN    4
//POST subsubstate:
#define HTTPD_SUBSTATE_POST_FN_OK 8


void httpd_init(void);
void httpd_cleanup_conn(unsigned char i);
void httpd_handle_packet (unsigned char* buffer,unsigned int *len);
unsigned int httpd_data_in(unsigned char *buffer, unsigned int datapos, unsigned int datalen, unsigned char socketnum, unsigned char *appstate);
//unsigned int httpd_add_progmem_data_offset(PGM_P pointer,unsigned char *buffer, unsigned int pos, unsigned int offset, unsigned int len);
unsigned int httpd_add_progmem_data(PGM_P pointer, PGM_P include,unsigned char *buffer, unsigned int pos, unsigned int offset, unsigned int len);
unsigned int httpd_add_progmem_data_bin(PGM_P pointer, unsigned char *buffer, unsigned int pos, unsigned int offset, unsigned int len, unsigned int pgm_datalen);
unsigned int httpd_add_io_icon(unsigned char *buffer, unsigned int datalen, unsigned int offset, unsigned char socketnum);

#endif
