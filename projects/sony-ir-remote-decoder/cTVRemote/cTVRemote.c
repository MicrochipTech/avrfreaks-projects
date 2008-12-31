/*--------------------------------------------------------------------
  cTVRemote.c - Decode Sony IR remote control

  Operation:  Measure counts between falling and rising edges on 
              TIMER1 input capture pin connected to IR demodulator.
              Reads 12 bit Sony IR remote control codes.

  Hardware:  ATtiny2313 on breadboard with 8 MHz external xtal.

             Port B connected to command bit LEDs (PB0:PB6).
             Port D connected to device bit LEDs (PD0:PD4).
             Pulse input from IR demodulator connected to ICP/PD6.
               This line has external 10 Kohm pullup resistor.

  Notes:  The Sony code consists of a start pulse of length 2.4 ms
          followed by 12 pulses of length either 0.6 ms for zero or 
          1.2 ms for one.  There is 0.6 ms between pulses.  The first
          seven bits give a command code, LSB sent first.  The last
          five bits give a device code, also LSB sent first.

          The pulse line from the 38 kHz IR demodulator (Vishay Part
          TSOP2238) is normally high at Vdd and the line is pulled
          low for a pulse.  

  Andrew Palm
  2008.12.28
--------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>

// Helpful general defines
#define clear_bit( reg, bitnum )  ((reg) &= ~(1<<(bitnum)))
#define set_bit( reg, bitnum )    ((reg) |= (1<<(bitnum)))
#define test_bit( reg, bitnum )   ((reg) & (1<<(bitnum)))
#define toggle_bit( reg, bitnum ) ((reg) ^= (1<<(bitnum)))

// Clock counts per ms, assuming timer clock divide-by-8
// COUNT_PER_US = (F_CPU / 1000000UL) / 8UL;

// Counts between successive falling and rising edges for three 
// pulse types:
#define START_COUNT 2400  // =(2400*COUNT_PER_US) for 2.4 ms start pulse
#define ONE_COUNT 1200    // =(1200*COUNT_PER_US) for 1.2 ms one pulse
#define ZERO_COUNT 600    // =(600*COUNT_PER_US) for 0.6 ms zero pulse
// Interpulse time is 0.6 ms
// Tolerance for pulse count error:
#define TOL_COUNT 200     // =(200*COUNT_PER_US)

// LED output ports
#define COMMAND_LED_PORT PORTB
#define COMMAND_LED_DDR  DDRB
#define DEVICE_LED_PORT  PORTD
#define DEVICE_LED_DDR   DDRD

// Global variable declarations
unsigned int falling_edge;   // Clock count on falling edge
unsigned int rising_edge;     // Clock count on rising edge
unsigned int pulse_count;     // Clock counts inside pulse
unsigned char bit_pos;

volatile unsigned int ovfl_count;    // Counter for TIMER1 overflow
volatile unsigned char pulse_state;  // ISR state machine state
volatile unsigned char command;   // Pulse sequence 7-bit command code
volatile unsigned char device;    // Pulse sequence 5-bit device code

//--------------------------------------------------------------------
ISR(TIMER1_OVF_vect)
// TIMER1 overflow ISR
{
  ovfl_count++;     // Increment counter when overflow occurs
}
//--------------------------------------------------------------------
ISR(TIMER1_CAPT_vect)
// TIMER1 capture ISR
{
  if (!test_bit(TCCR1B, ICES1)) // This interrupt is falling edge
  {
    falling_edge = ICR1L;
    falling_edge += 256*ICR1H; // Get start count for pulse
    ovfl_count = 0;   // Clear overflow counter for this measurement
    set_bit(TCCR1B, ICES1);   // Next interrupt is rising edge
  }
  else  // This interrupt is rising edge.  Get pulse count.
  {
    rising_edge = ICR1L;
    rising_edge += 256*ICR1H;  // Get end count for pulse
    pulse_count = (unsigned long) rising_edge
                  + ((unsigned long) ovfl_count * 65536)
                  - (unsigned long) falling_edge;
    clear_bit(TCCR1B, ICES1);  // Next interrupt is falling edge

    switch (pulse_state)
    {
      case 0:   // Pulse sequence read complete, wait to start next read
        break;

      case 1:   // Wait for 2.4 ms start pulse
        if ((pulse_count > (START_COUNT - TOL_COUNT)) &&
            (pulse_count < (START_COUNT + TOL_COUNT)))
        {
          pulse_state = 2;    // Initialize for data pulse capture
          bit_pos = 0;
          command = 0;
          device = 0;
        }
        break;

      case 2:   // Read data bit pulses
        if (pulse_count > (ONE_COUNT - TOL_COUNT))    // 1 pulse
        {
          if (bit_pos < 7)
            command |= (1<<bit_pos);
          else
            device |= (1<<(bit_pos - 7));
        }
        bit_pos++;
        if (bit_pos > 11)
          pulse_state = 0;  // Data read done
        break;

      default:
        break;
    }
  }
}
//--------------------------------------------------------------------
void main(void)
{
// Initialize LED outputs
  COMMAND_LED_DDR = 0b01111111;   // Seven command bits output
  COMMAND_LED_PORT = 0;
  DEVICE_LED_DDR = 0b00011111;    // Five device bits output
  DEVICE_LED_PORT = 0;

// Initialize TIMER1
  TCCR1A = 0;     // Disable all waveform functions
  TCCR1B = (1<<ICNC1)|(1<<CS11);  // Noise canceller enabled, clk/8,
                                  // (first capture on falling edge)
  TIMSK = (1<<TOIE1)|(1<<ICIE1);  // Unmask TIMER1 overflow and 
                                  // capture interrupts

  pulse_state = 0;   // Start doing nothing
  
  command = 0;       // Initialize codes
  device = 0;

  sei();             // Global interrupt enable

  while(1)
  {
    if (pulse_state == 0)   // Pulse sequence read complete
    {
      COMMAND_LED_PORT = command;
      DEVICE_LED_PORT = device;

      pulse_state = 1; // Signal ISR to look for next pulse sequence
    }
  }

}

 
