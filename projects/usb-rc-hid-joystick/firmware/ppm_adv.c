//=============================================================================
// Interrupt-driven PPM decoder (advanced).
// It reads up to MAX_CHANNELS pulses from PPM stream and stores their lengths
// into data buffer available for output interface.
// Additional options allow verification of input packets to reduce jitter.
//
// $Id: in_ppm_adv.c,v 1.1.2.4 2006/08/10 16:17:08 SOY Exp $
//=============================================================================

#include "common.h"

#if IN_PPM_ADV

//=============================================================================
// Servo tester option.
// Define this for channel number used for servo tester.
// No servo tester option supported if this is defined to 0.
#ifndef IN_PPM_ADV_SERVO_TEST_CHANNEL
#define IN_PPM_ADV_SERVO_TEST_CHANNEL   0   // range is 0 or 1..MAX_CHANNELS
#endif

// PPM polarity option.
// It should not be changed. But for some rare cases it may be useful
// if there is no PPM detected.
#ifndef IN_PPM_ADV_POLARITY
#define IN_PPM_ADV_POLARITY             0   // 0 for direct, 1 for inverted
#endif

// Advanced data verification options.
// You may set one of the following options to choose PPM data verification mode.
// By default all options are set to 0 that means no additional data verification.
// Up to MAX_CHANNELS will be read in that case.

// If this option is set to non-zero then ONLY first N_CHANNELS_ONLY channels
// will be read and made available for output. This option may be useful for
// Walkera transmitters which have some PCM data after standard PPM stream.
#ifndef IN_PPM_ADV_N_CHANNELS_ONLY
#define IN_PPM_ADV_N_CHANNELS_ONLY      0   // range is 0 or [1..MAX_CHANNELS]
#endif

// If this option is set to non-zero then input PPM stream will be checked to
// see if it consists of exactly N_CHANNELS_EXACT channels. Any deviation will
// result in refusal of whole PPM data packet.
#ifndef IN_PPM_ADV_N_CHANNELS_EXACT
#define IN_PPM_ADV_N_CHANNELS_EXACT     0   // range is 0 or [1..MAX_CHANNELS]
#endif

// If this option is set to non-zero then input PPM stream vill be evaluated
// to find the number of PPM channels. If last N_CHANNELS_AUTO PPM packets
// consist of equal number of pulses, then PPM data packet will be treated
// as valid. Otherwise it will be rejected. This is the recomended option
// with number of packets to check 2-10.
#ifndef IN_PPM_ADV_N_CHANNELS_AUTO
#define IN_PPM_ADV_N_CHANNELS_AUTO      0   // range is 0 or 2-10 (recommended option)
#endif

//=============================================================================
// Global variables

// Channel data variables
int  channelData[MAX_CHANNELS]; // 16 bit per channel value in uS (732-2268)
char newDataFound;              // new data packet found flag

//-----------------------------------------------------------------------------
// Decoder timing
//
// XTAL = 12,000,000 Hz (timer input frequency), prescaler = 8
// ppmLength (uS) = Timer / TCNT_CYCLES, where TCNT_CYCLES=1.5 (12/8)
//
// Timer = 30000 -> 20000 uS (PPM packet period)
//          4500 ->  3000 uS (syncro pause)
//          1050 ->   700 uS (minimum pulse)
//          2250 ->  1500 uS (middle pulse)
//          3300 ->  2200 uS (maximum value)

// Hardware configuration
#define TIMER           TCNT1       // 16-bit timer register
#define ICR             ICR1        // input capture register
#define TCCRA           TCCR1A
#define TCCRB           TCCR1B
#define OCRA            OCR1A
#define ICF             ICF1
#define OCF             OCF1A
#define TICIE           TICIE1
#define OCIE            OCIE1A
#define ICES            ICES1
#define TIMER_CAPT_vect TIMER1_CAPT_vect

// Capture port settings (required for turning on the internal pull-up resistor)
#define ICP_PORT_IN     IO_INPORT(ICP_PORT_NAME)
#define ICP_PORT_OUT    IO_OUTPORT(ICP_PORT_NAME)
#define ICP_PORT_DD     IO_DDRPORT(ICP_PORT_NAME)
#define ICP_BIT         ICP_PORT_BIT

// Servo tester port settings (if enabled then servo test output goes here)
#define ST_PORT_OUT     IO_OUTPORT(ST_PORT_NAME)
#define ST_PORT_DD      IO_DDRPORT(ST_PORT_NAME)
#define ST_BIT          ST_PORT_BIT

// Software configuration
#define PRESCALER_BITS  ((0<<CS12)|(1<<CS11)|(0<<CS10))                     // prescaler = 8
#define TCCRA_INIT      ((0<<WGM11)|(0<<WGM10))                             // normal mode
#define TCCRB_INIT      ((0<<WGM13)|(0<<WGM12))|(IN_PPM_ADV_POLARITY<<ICES) // input capture edge select
#define TIFR_INIT       (1<<ICF)                                            // reset ICF flag
#define TIMSK_INIT      (1<<TICIE)                                          // enable ICF interrupt

//-----------------------------------------------------------------------------
//
// Initialize PPM decoder
//
void inDecoderInit(void)
{
    ICP_PORT_DD &= ~(1<<ICP_BIT);               // input
    ICP_PORT_OUT |= (1<<ICP_BIT);               // enable pullup

    TCCRA  = TCCRA_INIT;                        // initialize timer
    TCCRB  = TCCRB_INIT | PRESCALER_BITS;
    TIFR  |= TIFR_INIT;                         // reset pending interrupt flags if any
    TIMSK |= TIMSK_INIT;                        // enable timer interrupts

#if IN_PPM_ADV_SERVO_TEST_CHANNEL
    ST_PORT_DD |= (1<<ST_BIT);                  // prepare output pin for servo tester
    ST_PORT_OUT &= ~(1<<ST_BIT);
#endif
}

//
// Stop PPM decoder interrupts
//
void inDecoderStop(void)
{
    TIMSK &= ~(TIMSK_INIT);                     // disable timer interrupts
    TIFR  |= TIFR_INIT;                         // reset pending interrupt flags if any
}

//
// Timer capture ISR.
//

ISR(TIMER_CAPT_vect)
{
    static unsigned ppmData[MAX_CHANNELS];  // temporary data buffer
    static unsigned char ppmChannel;        // PPM channel to receive
    static unsigned timerLast;              // last timer captured value
    unsigned timer;                         // captured value
    unsigned timerDiff;                     // timer difference
    unsigned ppmLength;                     // PPM pulse length in uS

    // calculate the length of pulse in uS (dt / 1.5)
    timer     = ICR;
    timerDiff = timer - timerLast;
    timerLast = timer;
    ppmLength = (timerDiff << 1) / 3;

    // if sycnro pause is found then process whole packet,
    // else simply store the pulse length into temp buffer
    if (ppmLength < 3000)
    {
        // next PPM pulse is found
        if (ppmChannel < MAX_CHANNELS) ppmData[ppmChannel] = ppmLength;
        ppmChannel++;

        // servo tester support
#if IN_PPM_ADV_SERVO_TEST_CHANNEL
        if (ppmChannel == (IN_PPM_ADV_SERVO_TEST_CHANNEL-1))
        {
            // start of required channel pulse found
            ST_PORT_OUT |= (1<<ST_BIT);
        }
        else if (ppmChannel == IN_PPM_ADV_SERVO_TEST_CHANNEL)
        {
            // end of required channel pulse found
            ST_PORT_OUT &= ~(1<<ST_BIT);
        }
#endif
    }
    else
    {
        // end of packet is found

#if IN_PPM_ADV_N_CHANNELS_ONLY
        // use only first N_CHANNELS_ONLY channels, the rest is ignored
        if (ppmChannel > IN_PPM_ADV_N_CHANNELS_ONLY) ppmChannel = IN_PPM_ADV_N_CHANNELS_ONLY;

#elif IN_PPM_ADV_N_CHANNELS_EXACT
        // use packet only if exact N_CHANNELS_EXACT channels are there
        if (ppmChannel != IN_PPM_ADV_N_CHANNELS_EXACT) ppmChannel = 0;

#elif IN_PPM_ADV_N_CHANNELS_AUTO
        // check for N_CHANNELS_AUTO valid packets
        static char ppmChannelLast;                         // last number of channels
        static char ppmChannelCount;                        // good packet counter

        if (ppmChannel != ppmChannelLast)
        {
            // number of channels has changed
            ppmChannelLast = ppmChannel;                    // new number of channels
            ppmChannelCount = IN_PPM_ADV_N_CHANNELS_AUTO;   // preset counter
            ppmChannel = 0;                                 // do not use this packet
        }
        else
        {
            // number of channels is the same
            if (ppmChannelCount)
            {
                // but not N_CHANNELS_AUTO times yet, still counting
                ppmChannelCount--;                          // count as good
                ppmChannel = 0;                             // but do not use yet
            }
        }

#else
        // do nothing, no special processing
#endif

        // copy data into output buffer if there are new data
        if (ppmChannel)
        {
            // use up to MAX_CHANNELS
            if (ppmChannel > MAX_CHANNELS) ppmChannel = MAX_CHANNELS;

            // copy data up to ppmChannel channels
			int i;
            for ( i = 0; i < ppmChannel; i++)
            {
                // ensure the atomic operation
                cli();
                channelData[i] = ppmData[i];
                sei();
            }

            // set new data flag to non-zero (actually the number of channels)
            newDataFound = ppmChannel;

            // prepare for next PPM data packet
            ppmChannel = 0;
        }

        // servo tester support
#if IN_PPM_ADV_SERVO_TEST_CHANNEL
        if (IN_PPM_ADV_SERVO_TEST_CHANNEL == 1)
        {
            // start of the first channel pulse found
            ST_PORT_OUT |= (1<<ST_BIT);
        }
#endif
    }
}

void inDecoderPoll(void){}

#endif  // IN_PPM_ADV
