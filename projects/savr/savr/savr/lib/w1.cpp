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

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay_basic.h>

#include <stdio.h>

#include <savr/w1.h>


/**
 * After examining the code, each call to _Delay() requires this many cycles of setup
 * 10 for call setup and return, 11 for function setup...
 */
#define DELAY_CYCLE_ADJ     21


//! Initial us delay values used to calculate clock delays based on sysClock
static const uint16_t CPP_PROGMEM usDelay[DelayIndex::NUM_DELAY_INDEXES] = {
//  A,  B,  C,  D, E,  F, G,   H,  I,   J
    6, 64, 60, 10, 9, 55, 0, 480, 70, 410,
};


// One instance for all class objects
uint16_t    W1::_delayCount[DelayIndex::NUM_DELAY_INDEXES];
bool        W1::_delaySet = false;



/**
 * @par Implementation notes:
 * GCC creates two copies of the constructor and destructors. This is the 'real'
 * constructor code in a separate function to reduce code size.
 *
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=3187
 */
void
W1::__W1(uint32_t sysClock, GPIO::Pin pin)
{
    _pin  = pin;

    // Set to tristate
    GPIO::Low(_pin);
    GPIO::In(_pin);

    // Pre-compute for more accurate delays (calc how many 4-cycle periods to delay)
    if(!_delaySet) {
        _delaySet = true;
        double delayFactor = 0.000001*sysClock/4;

        memcpy_P(_delayCount, usDelay, sizeof(usDelay));
        for(uint8_t i=0; i<DelayIndex::NUM_DELAY_INDEXES; ++i) {
            _delayCount[i] = _delayCount[i]*delayFactor;

            if(_delayCount[i] > DELAY_CYCLE_ADJ) {
                _delayCount[i] -= DELAY_CYCLE_ADJ;
            }else{
                _delayCount[i] = 0;
            }
        }
    }
}


/**
 * @par Implementation notes:
 */
W1::~W1()
{
    _Release();
}


/**
 * @par Implementation notes:
 */
bool
W1::Reset()
{
    bool presence = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _Delay(DelayIndex::G);
        _DriveLow();
        _Delay(DelayIndex::H);
        _Release();
        _Delay(DelayIndex::I);
        presence = (_ReadState() == 0);
        _Delay(DelayIndex::J);
    }
    return presence;
}


/**
 * @par Implementation notes:
 */
void
W1::MatchROM(Address &address)
{
    WriteByte(0x55);
    WriteBytes(address.array, 8);
}


/**
 * @par Implementation notes:
 */
void
W1::SkipROM(void)
{
    WriteByte(0xCC);
}


/**
 * @par Implementation notes:
 */
bool
W1::SearchROM(Address &address, Token &token)
{
    return _Searcher(0xF0, address, token);
}


/**
 * @par Implementation notes:
 */
bool
W1::AlarmSearch(Address &address, Token &token)
{
    return _Searcher(0xEC, address, token);
}


/**
 * @par Implementation notes:
 */
bool
W1::_Searcher(uint8_t command, Address &address, Token &token)
{

    // Note: 1-based bit counting
    uint8_t  last_zero_path = 0;    // Last zero taken at a discrepancy
    uint8_t  current_bit    = 0;    // Current bit counter
    uint8_t  search_dir     = 0;    // 1 or 0 for next bit?
    uint8_t  bits;


    // Did we finish last time? Reset state for next call.
    if(token == 0xFF) {
        token = 0;
        return false;
    }

    // Always reset when beginning a new search
    if(!Reset()) {
        return false;
    }

    // Begin search
    last_zero_path = 0;
    WriteByte(command);


    // Scan down 64 bits of the ROM...
    while(++current_bit <= 64) {

        bits  = ReadBit();       // Next bit value
        bits |= ReadBit() << 1;  // Complement

        switch(bits) {
        case 0: // Discrepancy
            if(current_bit == token) {

                // We took the 0 path, now take the 1 path
                search_dir = 1;

            }else if(current_bit > token) {

                // New discrepancy... take the 0 path
                search_dir      = 0;
                last_zero_path  = current_bit;

            }else{

                // Old discrepancy... take the old path from the address
                if(GetBit(address, current_bit-1) == 0) {
                    search_dir      = 0;
                    last_zero_path  = current_bit;
                }else{
                    search_dir      = 1;
                }

            }
            break;

        case 1: // Only a 1
            search_dir = 1;
            break;

        case 2: // Only a 0
            search_dir = 0;
            break;

        case 3: // No devices!
            // Fallthrough
        default:
            return false;
        }
        SetBit(address, current_bit-1, search_dir);
        WriteBit(search_dir);
    }

    token = last_zero_path;

    // Check if we're done (no more 0 paths for which we still need to search the 1's path)
    if(token == 0) {
        token = 0xFF;
    }

    return true;
}


/**
 * @par Implementation notes:
 */
uint8_t
W1::ReadBit()
{
    bool state = false;

    // These operations are time sensitive...
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _DriveLow();
        _Delay(DelayIndex::A);
        _Release();
        _Delay(DelayIndex::E);
        state = _ReadState();
        _Delay(DelayIndex::F);
    }
    return (uint8_t)state;
}


/**
 * @par Implementation notes:
 */
void
W1::WriteBit(bool bit)
{
    // These operations are time sensitive...
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(bit) {
            _DriveLow();
            _Delay(DelayIndex::A);
            _Release();
            _Delay(DelayIndex::B);
        }else{
            _DriveLow();
            _Delay(DelayIndex::C);
            _Release();
            _Delay(DelayIndex::D);
        }
    }
}


/**
 * @par Implementation notes:
 * Read LSB first.
 */
uint8_t
W1::ReadByte(void)
{
    uint8_t byte = 0;
    for(uint8_t i=0; i<8; ++i) {
        byte >>= 1;
        if(ReadBit()) {
            byte |= 0x80;
        }
    }
    return byte;
}


/**
 * @par Implementation notes:
 * Write LSB first.
 */
void
W1::WriteByte(uint8_t byte)
{
    for(uint8_t i=0; i<8; ++i) {
        WriteBit(byte & 0x01);
        byte >>= 1;
    }
}


/**
 * @par Implementation notes:
 */
void
W1::ReadBytes(uint8_t *byte, size_t size)
{
    for(size_t i=0; i<size; ++i) {
        byte[i] = ReadByte();
    }
}


/**
 * @par Implementation notes:
 */
void
W1::WriteBytes(uint8_t *byte, size_t size)
{
    for(size_t i=0; i<size; ++i) {
        WriteByte(byte[i]);
    }
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) void
W1::_Delay(DelayIndex::Index index)
{
    // How many clock cycles do we need to delay?
    // 4 clock cycles per count given
    uint16_t count = _delayCount[index];
    __asm__ volatile (
        "loop: sbiw %0,1" "\n\t"
        "brne loop"
        : "=w" (count)
        : "0" (count)
    );
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) void
W1::_DriveLow()
{
    // Tri-state to low, DDR to 1
    GPIO::Out(_pin);
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) void
W1::_Release()
{
    // Low to tri-state, DDR to 0
    GPIO::In(_pin);
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) bool
W1::_ReadState()
{
    return (bool)GPIO::Get(_pin);
}


/**
 * @par Implementation notes:
 */
void
W1::SetBit(Address &address, uint8_t bitNum, bool set)
{
    uint8_t bit     = _BV(bitNum%8);
    uint8_t byte    = bitNum / 8;

    if(set) {
        address.array[byte] |= bit;
    }else{
        address.array[byte] &= ~bit;
    }
}


/**
 * @par Implementation notes:
 */
uint8_t
W1::GetBit(Address &address, uint8_t bitNum)
{
    return !!(address.array[bitNum/8] & _BV(bitNum%8));
}


/**
 * @par Implementation notes:
 */
void
W1::PrintAddress(const Address &address)
{
    for(uint8_t i=0; i<8; ++i) {
        printf_P(PSTR("%02x"), address.array[i]);
    }
}

