#ifndef _SLAVE_SERVICE_INCLUDED_
#define _SLAVE_SERVICE_INCLUDED_

#include <stdint.h>
#include <stdbool.h>  

#include "packetSlave.h"
#include "encoder.h"

#define ECHO 0
#define MTR_FWD 1
#define GET_ENCTICK 2
#define GET_STATUS 3
#define GET_TRACK_DISP_MM 4
#define MTR_BWD 5

#pragma used+
uint16_t make_16_from_2_8 (uint8_t* a);
bool echo(Packet* p);
bool motorForward(uint16_t mmValue);
bool motorBackward(uint16_t mmValue);
bool _getEncoderTick();
bool sendStatus();
bool sendTrackDispMM();
bool masterPacketProcess (Packet* p);
#pragma used-

#endif