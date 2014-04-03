//=============================================================================
// Interrupt-driven PPM decoder.
// It reads up to MAX_CHANNELS pulses from PPM stream and stores their lengths
// into data buffer available for output interface.
//
// $Id: in_ppm.c,v 1.8.2.4 2006/08/10 16:04:29 SOY Exp $
//=============================================================================

#include "common.h"


//=============================================================================
// Global variables

// Channel data variables
volatile int  channelData[MAX_CHANNELS]; // 16 bit per channel value in uS (732-2268)

char newDataFound;              // new data packet found flag

//-----------------------------------------------------------------------------
// Decoder timing
//
// XTAL = 8,000,000 Hz (timer input frequency)
// We do not use any prescaler (divider is 1)
// One timer tick is (1 / XTAL) = 0.083 uS
// 1 uS is TCNT_CYCLES=12 timer ticks.
// T_uS = TIMER * tick = TIMER * 0.083 = TIMER / TCNT_CYCLES
// TIMER = T_uS * TCNT_CYCLES.
//
// Timer TOP value = 36000 -> 3000 uS (overflow = syncro pause)
//                    8400 ->  700 uS (minimum pulse length)
//                   18000 -> 1500 uS (middle pulse length)
//                   26400 -> 2200 uS (maximum pulse length)

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


// Capture port settings (required for turning on the internal pull-up resistor)
/*#define ICP_PORT_IN     IO_INPORT(ICP_PORT_NAME)
#define ICP_PORT_OUT    IO_OUTPORT(ICP_PORT_NAME)
#define ICP_PORT_DD     IO_DDRPORT(ICP_PORT_NAME)
#define ICP_BIT         ICP_PORT_BIT
*/

#define ICP_PORT_DD DDRB // timer input capture ports
#define ICP_PORT_IN PINB
#define ICP_PORT_OUT PORTB
#define ICP_BIT 0 // timer input capture pin



// Software configuration
#define TCNT_CYCLES     (XTAL / 1000000L)
#define PRESCALER_BITS  ((0<<CS12)|(0<<CS11)|(1<<CS10))     // no prescaler
#define TCCRA_INIT      ((0<<WGM11)|(0<<WGM10))             // CTC mode
#define TCCRB_INIT      ((0<<WGM13)|(1<<WGM12))             // TOP=OCRA
#define TIFR_INIT       ((1<<ICF)|(1<<OCF))
#define TIMSK_INIT      ((1<<TICIE)|(1<<OCIE))

// PPM decoder FSM states. The states have special numbering
// to provide minimum delay for timer clearing after capture
// (compiler-dependent optimization feature).
typedef enum
{
    PPM_ST_WAIT_START = 0,  // waiting for start of first channel pulse
    PPM_ST_WAIT_NEXT,       // waiting for start of next channel pulse
    PPM_ST_WAIT_PAUSE,      // waiting for syncro pause
    PPM_ST_WAIT_SYNC,       // waiting for the 1st falling edge
    PPM_ST_WAIT_END,        // waiting for end of channel pulse
} ppmState_t;

// PPM decoder variables
static ppmState_t ppmState; // PPM decoder state
int ppmChannel;     // PPM channel to receive

//-----------------------------------------------------------------------------
//
// Initialize PPM decoder
//
void inDecoderInit(void)
{
    ICP_PORT_DD &= ~(1<<ICP_BIT);               // input
    ICP_PORT_OUT |= (1<<ICP_BIT);               // enable pullup

    TCCRA  = TCCRA_INIT;
    TCCRB  = TCCRB_INIT | PRESCALER_BITS;
    OCRA   = 3000 * TCNT_CYCLES;                // (3000 uS -> overflow = syncro pause)
	OCRA 	= 30000;

    newDataFound = 0;                           // no PPM data packet yet
    ppmState = PPM_ST_WAIT_PAUSE;               // set initial decoder state

    TIMER = 0;                                  // start counting
    TIFR  |= TIFR_INIT;                         // reset pending interrupt flags if any
    TIMSK |= TIMSK_INIT;                        // enable compare and capture interrupts

}

//
// Stop PPM decoder interrupts
//
void inDecoderStop(void)
{
    TIMSK &= ~(TIMSK_INIT);                     // disable compare and capture interrupts
    TIFR  |= TIFR_INIT;                         // reset pending interrupt flags if any
}

//
// Timer compare with TOP ISR.
// Used for syncing with syncro pause and for input polarity detection.
//
//#pragma =TIMER_COMP_vect



ISR(TIMER1_COMPA_vect)
{
    // syncro pause is found, set capture edge
    if (ICP_PORT_IN & (1<<ICP_BIT))
        TCCRB &= ~(1<<ICES);            // wait for falling edge
    else
        TCCRB |= (1<<ICES);             // wait for rising edge

    // and now wait for the starting edge
    ppmState = PPM_ST_WAIT_SYNC;
}

//
// Timer capture ISR.
// Uses FSM for decoding the PPM stream.
//
//#pragma vector=TIMER_CAPT_vect
ISR(TIMER1_CAPT_vect)
{
    // PPM decoder state machine processing
    switch (ppmState)
    {
        case PPM_ST_WAIT_PAUSE:
            // still waiting for syncro pause
            TIMER = 0;
            break;

        case PPM_ST_WAIT_SYNC:
            // syncro falling edge found, waiting for start
            ppmChannel = 0;
            ppmState = PPM_ST_WAIT_START;
            TCCRB ^= (1<<ICES);             // change edge polarity
            break;

        case PPM_ST_WAIT_START:
            // starting edge is found, waiting for ending one

            TIMER = 0;
            ppmState = PPM_ST_WAIT_END;
            TCCRB ^= (1<<ICES);             // change edge polarity
            break;

        case PPM_ST_WAIT_END:
            // ending edge is found, waiting for next channel
            ppmState = PPM_ST_WAIT_NEXT;

            TCCRB ^= (1<<ICES);             // change edge polarity
            break;

        case PPM_ST_WAIT_NEXT:
            // pulse length is captured, reset timer for next channel
            // and save measured value

            TIMER = 0;
            ppmState = PPM_ST_WAIT_END;

            // save channel data
            if (ppmChannel < MAX_CHANNELS)
            {

                // data captured divided by TCNT_CYCLES gives the length in uS
                int data = (ICR / TCNT_CYCLES);



                // ensure the atomic operation
                cli();
                channelData[ppmChannel] = data;
                sei();

                // proceed to next channel
                ppmChannel++;

                // set new data flag to non-zero (actually the number of channels read)
                newDataFound = ppmChannel;
            }

            TCCRB ^= (1<<ICES);             // change edge polarity
            break;
    }
}

