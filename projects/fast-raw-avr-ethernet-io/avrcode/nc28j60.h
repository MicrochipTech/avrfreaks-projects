#ifndef ENC28J60_H
#define ENC28J60_H

#include <inttypes.h>

//------------------------------------------------------------------------------
// Functions for using an ENC28J60 ethernet controller, optimized for 
// communication speed.
//
// Written by Rogier Schouten http://www.rogiershikes.tk
// Based on code by Guido Socher http://www.tuxgraphics.org
// License: GPL V2
//
// Assumptions:
// - AVR pin connections are like the "AVR microcontroller based Ethernet device"
//   from tuxgraphics.org
//        AVR <--> ENC28J60:
//      XTAL1 <-- CLKout
//   PD2 INT0 <-- /INT
//   PB4 MISO <-- SO
//   PB3 MOSI --> SI
//   PB5 SCK  --> SCK
//   PB2      --> /CS
//   
// - uP wants to operate at 12.5Mhz (note: after power-on-reset this is 6.25Mhz
//   until enc28j60Init() has been called)
// - Max. payload data: 255 bytes per packet
// - The network consists of a master PC and one or more slave devices. 
//   No others, no switches or routers. Hubs are allowed.
// 
// Based on these assumptions, we can optimize the protocol for faster communication:
// - Master sends unicast packets to slaves, which send broadcasts back.
//   (This prevents us from ever having to copy MAC addresses in the slaves. 
//    As we have the ENC28J60s filter out broadcast packets, other slaves are 
//    not bothered by them.)
// - We use raw ethernet and no higher-level protocol such as UDP or TCP/IP
// - We use the EtherType field of a packet as a length field. Actually, we only
//   use one byte of it since we have max 255 length packets.
//  
// Furthermore, there are a few code optimizations wrt Guido's code
// - Only payload data is communicated to the application, no MAC addresses
// - No unnecessary memory bank checks
// - All the constant bytes of the transmitted packets are initialized 
//   only in enc28j60Init()
// 
//------------------------------------------------------------------------------

#ifndef F_CPU
#define F_CPU 12500000UL  // 12.5 MHz
#endif


// Define this macro if the AVR might call enc28j60TransmitPacket()
// when there is still a transmit in progress. Otherwise comment out completely.
#define ENC28J60_HAS_PENDING_TRANSMIT_ON_TRANSMIT 1

/// Initialize the controller (must be called first).
extern void enc28j60Init(
	uint8_t* macaddr); // Our own 6-byte mac address. Note: it CANNOT have the multicast bit set!!!!

/// Cable connected?
extern uint8_t enc28j60IsLinkUp(void);

/// Poll for a received packet. The payload of the packet is copied
/// to buf and the actual payload length is returned (always <= maxlen).
/// NOTE: to be interrupt-driven iso polling, you can enable the atmel's INT0 
/// interrupt on a high-to-low transition and call this function in the ISR.
///
/// Only packets with valid CRC, unicast MAC, and EtherType/length <= 255 are 
/// accepted. 
extern uint8_t enc28j60PollPacket(
	uint8_t maxlen, // maximum payload length
	uint8_t* buf);  // buffer to receive payload data in

/// Start transmitting a packet, waiting for any previous transmission to end.
/// The transmission is a broadcast.
/// Ret: 0: success 
///     -1: failed
extern void enc28j60TransmitPacket(
	uint8_t len,   // length of payload data
	uint8_t* buf); // payload data 


#endif
