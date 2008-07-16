#ifndef _DEBUG_H_
#define _DEBUG_H_

//if we need debug support -> load software uart
#include "io/softuart.h"

#define FILESYSTEM_DEBUG  0
#define DATAFLASH_DEBUG   0
#define PLOT_DEBUG        0
#define LM75_DEBUG        0
#define CLOCK_DEBUG       0

#define ENC_DEBUG         0
#define NIC_DEBUG         0
#define NETHELP_DEBUG     0
#define IP_DEBUG          0
#define TCP_DEBUG         0
#define UDP_DEBUG         0
#define ARP_DEBUG         0
#define ICMP_DEBUG        0
#define HTTPD_DEBUG       0
//#define HTTPD_DEBUG_GET   1
#define HTTPD_DEBUG_AUTH  0
#define HTTPD_DEBUG2      0 //verbose!
#define NTP_CLIENT_DEBUG  0

#define LOGGER_DEBUG 0
#define MCA25_DEBUG  0


#endif
