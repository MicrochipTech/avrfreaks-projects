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

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <savr/cpp_pgmspace.h>
#include <savr/spi.h>
#include <savr/eth.h>
#include <savr/gpio.h>


static GPIO::Pin _ss;



/**
 * @par Implementation notes:
 */
bool
Eth::Init(GPIO::Pin ss, Eth::IP4Address address, Eth::IP4Address mask, Eth::IP4Address gateway)
{
    bool ret = true;

    _ss = ss;
    GPIO::Out(_ss);
    GPIO::High(_ss);

    // Reset, early error termination
    ret &= Eth::W5100::Write(0, 0x80);
    if(!ret) return false;

    ret &= Eth::SetIP(address);
    ret &= Eth::SetSubnetMask(mask);
    ret &= Eth::SetGateway(gateway);

    // Default MAC
    ret &= Eth::W5100::Write(0x0009, 2);   // Unicast, locally administered
    ret &= Eth::W5100::Write(0x000E, 1);

    return ret;
}


/**
 * @par Implementation notes:
 */
bool
Eth::SetIP(Eth::IP4Address address)
{
    bool ret = true;
    uint8_t *temp;
    temp = (uint8_t*)&address;
    ret &= Eth::W5100::Write(0x000F, temp[3]);
    ret &= Eth::W5100::Write(0x0010, temp[2]);
    ret &= Eth::W5100::Write(0x0011, temp[1]);
    ret &= Eth::W5100::Write(0x0012, temp[0]);
    return ret;
}


/**
 * @par Implementation notes:
 */
bool
Eth::SetSubnetMask(Eth::IP4Address mask)
{
    bool ret = true;
    uint8_t *temp;
    temp = (uint8_t*)&mask;
    ret &= Eth::W5100::Write(0x0005, temp[3]);
    ret &= Eth::W5100::Write(0x0006, temp[2]);
    ret &= Eth::W5100::Write(0x0007, temp[1]);
    ret &= Eth::W5100::Write(0x0008, temp[0]);
    return ret;
}


/**
 * @par Implementation notes:
 */
bool
Eth::SetGateway(Eth::IP4Address gateway)
{
    bool ret = true;
    uint8_t *temp;
    temp = (uint8_t*)&gateway;
    ret &= Eth::W5100::Write(0x0001, temp[3]);
    ret &= Eth::W5100::Write(0x0002, temp[2]);
    ret &= Eth::W5100::Write(0x0003, temp[1]);
    ret &= Eth::W5100::Write(0x0004, temp[0]);
    return ret;
}


/**
 * @par Implementation notes:
 */
bool
Eth::SetMAC(uint8_t* mac)
{
    bool ret = true;
    // MAC
    ret &= Eth::W5100::Write(0x0009, mac[0]);
    ret &= Eth::W5100::Write(0x000A, mac[1]);
    ret &= Eth::W5100::Write(0x000B, mac[2]);
    ret &= Eth::W5100::Write(0x000C, mac[3]);
    ret &= Eth::W5100::Write(0x000D, mac[4]);
    ret &= Eth::W5100::Write(0x000E, mac[5]);
    return false;
}












#define READ_OP     0x0F
#define WRITE_OP    0xF0

bool
Eth::W5100::Read(uint16_t address, uint8_t* byte) {
    GPIO::Low(_ss);

    if(SPI::TrxByte(READ_OP) != 0) goto ethread_fail;
    if(SPI::TrxByte((address>>8) & 0xFF) != 1) goto ethread_fail;
    if(SPI::TrxByte((address)    & 0xFF) != 2) goto ethread_fail;
    *byte = SPI::TrxByte(0xFF);

    GPIO::High(_ss);

    return true;

ethread_fail:
    GPIO::High(_ss);
    printf_P(PSTR("Read Failed\n"));
    return false;
}



bool
Eth::W5100::Write(uint16_t address, uint8_t byte) {
    GPIO::Low(_ss);

    if(SPI::TrxByte(WRITE_OP) != 0) goto ethwrite_fail;
    if(SPI::TrxByte((address>>8) & 0xFF) != 1) goto ethwrite_fail;
    if(SPI::TrxByte((address)    & 0xFF) != 2) goto ethwrite_fail;
    if(SPI::TrxByte(byte) != 3) goto ethwrite_fail;

    GPIO::High(_ss);

    return true;

ethwrite_fail:
    GPIO::High(_ss);
    printf_P(PSTR("Write Failed\n"));
    return false;
}

