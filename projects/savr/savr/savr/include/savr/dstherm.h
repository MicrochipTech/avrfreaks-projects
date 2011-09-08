#ifndef _dstherm_h_Included_
#define _dstherm_h_Included_
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
 * @file dstherm.h
 *
 * Dallas 1-Wire thermometer interface
 */

#include <savr/w1.h>


/**
 * Dallas Semiconductor thermometer interface
 *
 * This should work for many of the 1-Wire devices in the DS182x and DS18B2x line.
 *
 * Conversion note:
 *  You either need to connect Vdd to a power source (non-parasitic mode),
 *  or ground it (parasitic power mode) and supply a strong pullup on DQ
 *  after calling StartConversion*(). For some models, the Vdd pin should
 *  not be connected at all!
 *
 *  In parasitic power mode, WaitForConversion() will not work as the function
 *  relies on the DS thermometer to pull down the DQ line during the conversion.
 *  If you are operating in parasitic power mode, you must do you own handling
 *  of the strong pullup, and delay between starting a conversion and calling GetTemp()
 */
class DSTherm {
public:

    /**
     * Create a Dallas Semiconductor thermometer interface
     *
     * @param wire      A 1-Wire interface
     * @param address   Address of the thermometer in question
     */
    DSTherm(W1 wire, W1::Address address);


    /**
     * Get the temperature in Celcius or Ferinheit
     *
     * @param ferinheit true to convert to F, false to leave in C (default)
     *
     * @return the temperature in degrees of the selected units, or NAN (math.h) on error
     *
     * This should be called after StartConversion() or StartConversionAll(). This
     * will call WaitForConversion() for you.
     */
    double GetTemp(bool ferinheit=false);


    /**
     * Start the conversion for this particular temp sensor
     *
     * @return false if the 1-Wire reset failed
     */
    bool StartConversion(void);


    /**
     * Start a conversion for all temp sensors on this 1-Wire bus
     *
     * @return false if the 1-Wire reset failed
     */
    bool StartConversionAll(void);


    /**
     * Wait for conversions to complete (non parisitic power mode)
     *
     * This polls the DQ line while it is held low until it is
     * released by the thermometer. This will not work in parasitic-power mode.
     *
     * @return true if conversion completed within 750-ish ms. false if conversion timed out.
     */
    bool WaitForConversion(void);


private:
    W1              _wire;      ///< Copy of a 1-wire interface (it's only a byte in size...)
    W1::Address     _address;   ///< Copy of a given address
};


#endif /* _dstherm_h_Included_ */


