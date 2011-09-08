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
 * @file dstherm.cpp
 */

#include <math.h>

#include <savr/dstherm.h>


// Documented commands from DS18B20 spec

static const uint8_t DS_CONVERT         = 0x44;
static const uint8_t DS_READ_SCRATCH    = 0xBE;
static const uint8_t DS_WRITE_SCRATCH   = 0x4E;
static const uint8_t DS_COPY_SCRATCH    = 0x48;
static const uint8_t DS_RECALL_EEPROM   = 0xB8;
static const uint8_t DS_READ_SUPPLY     = 0xB4;



/**
 * @par Implementation notes:
 */
DSTherm::DSTherm(W1 wire, W1::Address address) :
    _wire(wire),
    _address(address)
{
    // Empty
}


/**
 * @par Implementation notes:
 */
double
DSTherm::GetTemp(bool ferinheit)
{
    uint16_t    temp;   // Raw
    double      dtemp;  // Converted

    // Bus setup
    if(!WaitForConversion()) return NAN;
    if(!_wire.Reset()) return NAN;

    // Select our device
    _wire.MatchROM(_address);
    _wire.WriteByte(DS_READ_SCRATCH);

    // Read only the first 2 bytes
    temp  = _wire.ReadByte();                   // LSB
    temp |= ((uint16_t)_wire.ReadByte() << 8);  // MSB

    // Stop transmission
    _wire.Reset();

    dtemp = temp;
    dtemp /= 16;

    // Do a conversion to F if necessary
    if(!ferinheit)
        return dtemp;
    return 1.8*dtemp + 32;
}


/**
 * @par Implementation notes:
 */
bool
DSTherm::WaitForConversion(void)
{
    size_t count = 0;
    static const size_t MAX_COUNT = 15000;  // 750ms max for a conversion
                                            // (70us min in pure delay per bit read)
    // Wait for the therm to release the DQ line
    while(_wire.ReadBit() == 0) {
        if(count++ > MAX_COUNT) {
            return false;
        }
    }

    return true;
}

/**
 * @par Implementation notes:
 */
bool
DSTherm::StartConversion(void)
{
    if(!_wire.Reset()) return false;

    _wire.MatchROM(_address);
    _wire.WriteByte(DS_CONVERT);
    return true;
}


/**
 * @par Implementation notes:
 */
bool
DSTherm::StartConversionAll(void)
{
    if(!_wire.Reset()) return false;

    _wire.SkipROM();
    _wire.WriteByte(DS_CONVERT);
    return true;
}
