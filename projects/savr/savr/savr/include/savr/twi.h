#ifndef _twi_h_Included_
#define _twi_h_Included_
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

#include <inttypes.h>
#include <util/twi.h>


namespace TWI {

    /**
     * Inltialize the TWI subsystem
     *
     * @param sysClock      The system clock frequency in Hz
     * @param outputFreq    The desired TWI bus frequency in Hz
     */
    void    Init(uint32_t sysClock, uint32_t outputFreq);


    /**
     * Prints a textual description of the bus state
     */
    void    PrintState(void);


    /**
     * Addresses the given endpoint for read or write
     *
     * @param address   The address of the endpoint
     * @param rw        The RW bit
     * @return 0 on success, non-zero on error
     */
    uint8_t Address(uint8_t address, uint8_t rw);


    /**
     * Read a byte with acknowledgment
     *
     * @return The byte read
     */
    uint8_t GetAck(void);


    /**
     * Read a byte with no acknowledgment
     *
     * @return The byte read
     */
    uint8_t Get(void);


    /**
     * Send a byte
     *
     * @param b     The byte to send
     */
    void    Send(uint8_t b);


    /**
     * Send bus Start
     */
    void    Start(void);


    /**
     * Send bus stop
     */
    void    Stop(void);


    /**
     * Get the state of the bus
     *
     * @return The bus state byte
     *
     * See util/twi.h from the avr-libc for a state enumeration
     */
    uint8_t State(void);


    /**
     * Polling wait on the TWI bus
     */
    static inline void
    Wait(void) {
        while(!(TWCR & _BV(TWINT))) ;
    }

};

#endif /*_twi_h_Included_ */

