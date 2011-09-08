#ifndef _w1_h_Included_
#define _w1_h_Included_
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
 * @file w1.h
 *
 * @brief 1-Wire interface using a single GPIO pin.
 */

#include <stdint.h>
#include <stdlib.h>

#include <savr/cpp_pgmspace.h>
#include <savr/gpio.h>


#define W1_EMPTY_TOKEN 0

namespace DelayIndex {
typedef enum {
    A=0, B, C, D, E, F, G, H, I, J, NUM_DELAY_INDEXES
} Index;
};



class W1 {

public:
    /**
     * @brief 64-bit 1-Wire device address type
     *
     * Internally, the address is stored with the MSB at byte 0
     *
     * The 8-byte address looks like:
     *  [ CRC | 48-bit serial | Fam Code ]
     *     0    1 2 3 4 5 6 7    8
     *
     * This is going along with AN187.
     */
    typedef union {
        uint64_t raw;
        uint8_t  array[8];
    }Address;

    // Token for reentrant bus searches
    typedef uint8_t     Token;


    /**
     * Create a 1-Wire interface on the given port and pin.
     *
     * Note that 'sysClock' is used to compute necessary bus delays. The delay values are shared
     * between W1 objects. This is set only once by the first object created.
     *
     * @param sysClock  System clock speed, in Hz.
     * @param pin       GPIO Pin to use for the bus.
     */
    W1(uint32_t sysClock, GPIO::Pin pin) {
        __W1(sysClock, pin);
    }


    /**
     * Cleans up and sets bit to high-impedance.
     */
    ~W1();


    /**
     * Reset the 1-Wire bus and detect presence.
     *
     * @return true if presence found, false otherwise.
     */
    bool    Reset(void);


    /**
     * Select the device with the given address using a Match ROM.
     *
     * @param address   Pointer to an address source
     */
    void    MatchROM(Address &address);


    /**
     * Select all devices on the bus.
     */
    void    SkipROM(void);


    /**
     * Scan the bus for devices (0xF0 Search)
     *
     * This can be called multiple times while it returns True. The same address must be
     * passed back in, as it is used for state between calls. For instance:
     *
     * @code
     *  W1::Address address;
     *  W1::Token   token = W1_EMPTY_TOKEN;
     *  while(wire->SearchROM(address, token)) {
     *      W1::PrintAddress(address);
     *      putchar('\n');
     *  }
     * @endcode
     *
     * @param address   Pointer to address destination
     * @param token     Token that keeps state between calls. This must be 0 on the first call. And
     *                  must never be modified by the caller.
     *
     * @return true if we found a device, false otherwise.
     */
    bool    SearchROM(Address &address, Token &token);


    /**
     * Scan the bus for devices that are in an Alarm state (0xEC Search)
     *
     * This can be called multiple times while it returns True. The same address must be
     * passed back in, as it is used for state between calls. For instance:
     *
     * @code
     *  W1::Address address;
     *  W1::Token   token = W1_EMPTY_TOKEN;
     *  while(wire->AlarmSearch(address, token)) {
     *      W1::PrintAddress(address);
     *      putchar('\n');
     *  }
     * @endcode
     *
     * @param address   The address destination
     * @param token     Token that keeps state between calls. This must be 0 on the first call. And
     *                  must never be modified by the caller.
     *
     * @return true if we found a device, false otherwise.
     */
    bool    AlarmSearch(Address &address, Token &token);


    /**
     * If there is only a single device on the bus, this can be used to retrieve its address.
     *
     * @param address   The address destination
     *
     * @return true if we successfully found a device, false otherwise.
     */
    bool    ReadROM(Address &address);


    /**
     * Read a bit from the bus
     *
     * @return 1 if bit is high, 0 otherwise
     */
    uint8_t ReadBit(void);


    /**
     * Write a bit to the bus
     *
     * Writes a 1 if 'bit' is non-zero
     *
     * @param bit   Bit to write
     */
    void    WriteBit(bool bit);


    /**
     * Read a full byte from the bus
     * @return The byte read
     */
    uint8_t ReadByte(void);


    /**
     * Write a full byte to the bus
     * @param byte  The byte write
     */
    void    WriteByte(uint8_t byte);


    /**
     * Read a series of bytes from the bus
     *
     * @param byte  Pointer to a destination byte array
     * @param size  Number of bytes to read
     */
    void    ReadBytes(uint8_t *byte, size_t size);


    /**
     * Write a series of bytes to the bus
     *
     * @param byte  Pointer to a source byte array
     * @param size  Number of bytes to write
     *
     * Writes bytes starting at 0, to size-1
     */
    void    WriteBytes(uint8_t *byte, size_t size);


    /**
     * Set a bit in the address
     *
     * @param address   The address
     * @param bitNum    Bit number to set (0 is LSB)
     * @param set       Set the bit to 1 if true, 0 otherwise
     */
    static void     SetBit(Address &address, uint8_t bitNum, bool set);


    /**
     * Get a bit in the address
     *
     * @param address   The address
     * @param bitNum    Bit number to get (0 is LSB)
     *
     * @return 0 if the bit is 0, 1 if the bit is 1...
     */
    static uint8_t  GetBit(Address &address, uint8_t bitNum);


    /**
     * Print the address to standard out
     *
     * @param address  The address to print
     */
    static void     PrintAddress(const Address &address);

protected:

    bool _Searcher(uint8_t command, Address &address, Token &token);
    void _Delay(DelayIndex::Index index);
    void _DriveLow();
    void _Release();
    bool _ReadState();

private:
    void __W1(uint32_t sysClock, GPIO::Pin pin);

    GPIO::Pin           _pin;   ///< GPIO Pin to control for this bus

    // 4-cycle delay count conversion, one per system.
    static uint16_t     _delayCount[DelayIndex::NUM_DELAY_INDEXES];
    static bool         _delaySet;
};


#endif /* W1_H_ */
