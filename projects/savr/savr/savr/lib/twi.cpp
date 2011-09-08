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
#include <savr/twi.h>


static const char CPP_PROGMEM SentData[]    = "Send data, got";
static const char CPP_PROGMEM RcvdData[]    = "Rcvd data and";
static const char CPP_PROGMEM SLAW[]        = "SLA+W";
static const char CPP_PROGMEM SLAR[]        = "SLA+R";
static const char CPP_PROGMEM eACK[]        = " ACK";
static const char CPP_PROGMEM eNACK[]       = " NACK";


/**
 * @par Implementation notes:
 */
void
TWI::Init(uint32_t sysClock, uint32_t outputFreq)
{

    // SCL Freq = CPU Freq / ( 16 + 2(TWBR)(PrescalerValue) ) 
    // TWBR =  ((CPU Freq / SCL Freq) - 16) / 2 / (PrescalerValue)
    TWBR = ((sysClock/outputFreq) - 16) / 2;

    // Enable interrupt, enable the TWI
    TWCR = _BV(TWINT) | _BV(TWEN);
}


/**
 * @par Implementation notes:
 */
void
TWI::PrintState(void)
{
    char temp[5];
    switch (TWI::State()) {
        case TW_MT_DATA_ACK:
            puts_P(SentData); puts_P(eACK);
            break;
        case TW_MT_DATA_NACK:
            puts_P(SentData); puts_P(eNACK);
            break;

        case TW_MT_SLA_ACK:
            puts_P(SLAW); puts_P(eACK);
            break;
        case TW_MT_SLA_NACK:
            puts_P(SLAW); puts_P(eNACK);
            break;

        case TW_MR_ARB_LOST:
            puts_P(PSTR("Arb Lost"));
            break;

        case TW_MR_SLA_ACK:
            puts_P(SLAR); puts_P(eACK);
            break;
        case TW_MR_SLA_NACK:
            puts_P(SLAR); puts_P(eNACK);
            break;

        case TW_REP_START:
            puts_P(PSTR("Rep Start"));
            break;
        case TW_START:
            puts_P(PSTR("Initial Start"));
            break;

        case TW_MR_DATA_ACK:
            puts_P(RcvdData); puts_P(eACK);
            break;
        case TW_MR_DATA_NACK:
            puts_P(RcvdData); puts_P(eNACK);
            break;

        case TW_NO_INFO:
            puts_P(PSTR("No Info"));
            break;

        case TW_BUS_ERROR:
            puts_P(PSTR("Bus Error"));
            break;

        default:
            puts_P(PSTR("Status: "));
            printf(itoa((TWSR & TW_STATUS_MASK), temp, 16));
            break;
    }
}


/**
 * @par Implementation notes:
 */
void
TWI::Send(uint8_t b)
{
    TWI::Wait();
    TWDR = b;
    TWCR = _BV(TWINT) | _BV(TWEN);
    TWI::Wait();
}


/**
 * @par Implementation notes:
 */
uint8_t
TWI::GetAck(void)
{
    TWI::Wait();
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); // Enable ACK 
    TWI::Wait();
    return TWDR;
}


/**
 * @par Implementation notes:
 */
uint8_t
TWI::Get(void)
{
    TWI::Wait();
    TWCR = _BV(TWINT) | _BV(TWEN); // No ACK 
    TWI::Wait();
    return TWDR;
}


/**
 * @par Implementation notes:
 */
uint8_t
TWI::Address(uint8_t address, uint8_t rw)
{
    // Create start condition
    uint8_t state = 0;
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    TWI::Wait();
    state = TWI::State();
    if(state != TW_START && state != TW_REP_START) {
        return 1;
    }

    // Send address | RW (has waits...)
    TWI::Send((address<<1) | (!!rw));

    state = TWI::State();
    if(state != TW_MR_SLA_ACK && state != TW_MT_SLA_ACK) {
        return 1;
    }
    
    return 0;
}


/**
 * @par Implementation notes:
 */
void
TWI::Stop(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}


/**
 * @par Implementation notes:
 */
void
TWI::Start(void)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
}


/**
 * @par Implementation notes:
 */
uint8_t
TWI::State(void)
{
    return (TWSR & TW_STATUS_MASK);
}


