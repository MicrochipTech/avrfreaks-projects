#ifndef _HTTP_CLIENT_H
#define _HTTP_CLIENT_H_
#include "tcp.h"
#include "mynic.h"

void http_client_init(void);
void http_client_connect(unsigned char *buffer);
unsigned int http_client_data_in(unsigned char *buffer, unsigned int datapos, unsigned int datalen, unsigned char socketnum, unsigned char *appstate);

#endif

