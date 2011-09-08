#ifndef _eth_h_Included_
#define _eth_h_Included_
/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

/**
 * \file eth.h
 *
 * W5100 interface header
 *
 * A simple ethernet/network interface.
 */

#include <stdint.h>
#include <stddef.h>

#include <savr/gpio.h>

namespace Eth {
typedef uint32_t IP4Address;

/**
 * Initialize the W5100 with the given params
 *
 * @param ss      The GPIO::Pin used for the slave select
 * @param address IPv4 address
 * @param mask    IPv4 subnet mask
 * @param gateway Default IPv4 gateway
 *
 * @return true on success, false otherwise
 */
bool Init(GPIO::Pin ss, IP4Address address, IP4Address mask, IP4Address gateway);


/**
 * Change the address
 *
 * @param address IPv4 address
 *
 * @return true on success, false otherwise
 */
bool SetIP(IP4Address address);

/**
 * Change the subnet mask
 *
 * @param mask    IPv4 subnet mask
 *
 * @return true on success, false otherwise
 */
bool SetSubnetMask(IP4Address mask);

/**
 * Change the gateway address
 *
 * @param gateway Default IPv4 gateway
 *
 * @return true on success, false otherwise
 */
bool SetGateway(IP4Address gateway);

/**
 * Change the MAC address
 *
 * @param mac The 6-byte MAC address
 *
 * @return true on success, false otherwise
 */
bool SetMAC(uint8_t* mac);


// Read/write routines for the physical device
namespace W5100 {
bool Read(uint16_t address, uint8_t *byte);
bool Write(uint16_t address, uint8_t byte);
};

};

#endif

