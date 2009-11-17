// modbus.h
#ifndef __modbus__

#define __modbus__ 

typedef unsigned char uchar;
typedef unsigned int uint;
#define MB_ADDR    1
/* commun functions */
uint Mb_test_crc(uchar* ,uchar );   /* check crc16 */
uint Mb_calcul_crc(uchar* ,uchar );      /* compute crc16 */
#endif


