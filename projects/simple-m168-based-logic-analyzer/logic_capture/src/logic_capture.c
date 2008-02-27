// Simple logic capture / logic analyzer tool
//
//  (c) 2008 Tymm Twillman.  
//  Released under Creative Commons Share-Alike license.
//
//  For monitoring of logic levels on pins.  Up to 128 samples of 8 bits each
//   taken, along with a (CPU clock-relative) timestamp.  The 8 bits come from
//   PD2-7 and PB0-1 respectively (Arduino digital pins 2-10).
//
//  Two modes are implemented; clocked and state-change.
//   In clocked mode, PB2 (Arduino digital pin 10) is used as the clock line; 
//   on every transition of the pin, 8 bits are stored along with a timestamp.
//   In state-change mode, any change of any of the input pins causes
//   the state of all 8 pins to be stored along with a timestamp.
//
//  This is mainly made for use debugging SPI, I2C, UART, etc. traffic, but also
//   will work as a general logic analysis tool.  Each "tick" takes 32 clock
//   cycles, therefore maximum sample freq @ 8mHz is 250K (500K @ 16mHz).
//
//  Compilation (set F_CPU to clock speed of your '168):
//   avr-gcc -mmcu=atmega168 -O2 -DF_CPU=16000000 -Wall -Werror
//          logic_capture.c -o logic_capture.elf
//   avr-objcopy -j .text -j .data -O srec logic_capture.elf logic_capture.srec
//
//  The necessary circuit:
//   I've got my setup based on an Arduino compatible board, though the
//   application will compile fine in an AVR-GCC environment without the
//   Arduino libraries, etc.  In general you just need a generic ATMEGA168
//   board with a serial connection to a PC.  To simplify matters, a reset
//   button (or a circuit to auto-reset the chip when a serial port is opened)
//   is helpful.
//
//  Use:
//   Attach the board to your PC using whatever serial mechanism you're set up
//   for.  Start a serial terminal application on the PC, 19200 baud N81.  
//   Hook up pins to be monitored to PD2-7, PB0-1 (bit 0 to be monitored to
//   PD2, bit 1 to PD3, etc).  If you have the auto-reset setup, you should
//   see "ok" printed to the terminal.  By default the device is in "clocked"
//   capture mode (it will capture the states of the data lines every time
//   the clock line, PB2 (Digital Pin 10) has a transition.
//   You can send 'ms' (no enter/cr necessary for any commands) to change the
//   mode to "state change" mode (state of lines are captured every time any
//   of them changes).  Send 't' to begin tracing; tracing will continue until
//   128 samples are captured or another key is received (I usually use the
//   space key).  Send 'd' to dump the current trace buffer.
//
//   Dumps are sent as two colon-separated hex values on each line; the first
//   (a 24-bit hex number) is the timestamp for that entry relative to the 
//   time the trace was started; the unit is 32 clock cycles (e.g. on a '168 
//   clocked at 16 mHz, each unit is 2 uS).  The second hex value (an 8-bit
//   hex number) is the state of the 8 digital lines, with PD2 being the LSB.
//
//   I have put some work into a GUI frontend for the logic analyzer; it's
//   currently very pre-alpha and only runs on OSX (tested only with Leopard).
//   I will probably not put a lot of time into it, since it does what I
//   need for now, but I'm open to others working on it if they find it
//   useful/worthwhile.  It is based on OpenFrameworks 0.05.  That code/
//   OSX executable can be found at http://timothytwillman.com/?page_id=162
//

#define BAUD 19200

#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#define CLOCKED      0x00
#define STATE_CHANGE 0x01


uint8_t trace_mask       = 0xff;  // For state change capture
uint8_t trace_mode       = CLOCKED;
uint8_t max_trace_length = 128;
uint8_t trace_length     = 0;
uint32_t trace_buffer[128];


void serial_init(int baud)
{
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0A = 0;
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
  UBRR0 = ((((F_CPU / 16L) + (((uint32_t)baud) >> 1))
            / ((uint32_t)baud)) - 1L);
}

int serial_getc()
{
  if (!(UCSR0A & _BV(RXC0)))
    return -1;
  return UDR0;
}


void serial_putc(char c)
{
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = c;
}


void serial_write(char *buf, int buflen)
{
  int i;
  for (i = 0; i < buflen; i++) {
    serial_putc(buf[i]);
  }
}


void serial_putstr(char *str)
{
  while (*str) {
    serial_putc(*str++);
  }
}


void serial_puthex(uint8_t num)
{
  int i;
  char c;
  
  for (i = 0; i < 2; i++) {
    c = (num & 0xf0) >> 4;
    num <<= 4;
    c += (c > 9) ? ('A' - 10):'0';
    serial_putc(c);
  }
}


uint8_t state_change_capture(uint32_t *bufptr, uint8_t max_trace_len, uint8_t mask)
{  
  // Output munges low 2 bits of PINB with high 6 of PIND, to speed up capture
  
  uint8_t count = max_trace_len;
  uint8_t xmask = (mask << 2) | (mask >> 6);
  
    
  asm volatile (
    "    in   r20, 0x3f     \n"
    "    sei                \n"

    // Clear time counter
    "    clr  r0            \n"
    "    clr  r1            \n"
    "    clr  r2            \n"

    // Read in initial state
    "    in   r16, 0x03     \n"  // PINB
    "    in   r17, 0x09     \n"  // PIND
    "    andi r16, 0x03     \n"
    "    andi r17, 0xfc     \n"
    "    or   r16, r17      \n"
    "    mov  r17, r16      \n"
    "    and  r17, %4       \n"
    "    nop                \n"
    "    nop                \n"
  
    // HERE r16 = combined value of PINB/D, r17 = masked PB/D,
    //  R0 = 0, R1 = TCNT1H, R2 = TCNT1L
    "1:  st   Z+, r16       \n" // 2
    "    st   Z+, r0        \n" // 2
    "    st   Z+, r1        \n" // 2
    "    st   Z+, r2        \n" // 2
    "    mov  r3, r17       \n" // 1 -- keep last state for comparison
    "    dec  %3            \n" // 1
    "    breq 5f            \n" // 1 or 2 (1 when it counts)

    // 32 cycles

    "2:  lds  r16,  0xc0    \n" // 2 -- UCSR0A
    "    andi r16,  0x80    \n" // 1 -- RXC (receive complete)
    "    brne       5f      \n" // 1 or 2 (1 when it counts)

    // Increment time counter
    "    inc  r0            \n" // 1
    "    breq 3f            \n" // 1 or 2
    "    nop                \n" // 1
    "    jmp  4f            \n" // 3
    // Always 3 cycles
    "3:  inc  r1            \n" // 1
    "    brne 4f            \n" // 1 or 2
    "    inc  r2            \n" // 1

    "4:  nop                \n" // 1  (make even 32 cycles)
    "    in   r16,  0x03    \n" // 1 -- PINB
    "    in   r17,  0x09    \n" // 1 -- PIND
    "    andi r16,  0x03    \n" // 1
    "    andi r17,  0xfc    \n" // 1
    "    or   r16,  r17     \n" // 1
    "    mov  r17,  r16     \n" // 1
    "    and  r17,  %4      \n" // 1 
    "    cp   r17,  r3      \n" // 1
    "    brne 1b            \n" // 1 or 2
    
    // Delays to keep timing constant even if don't capture
    
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // mov
    "    nop                \n" // dec
    "    breq 2b            \n" // always -- 2 -- makes up for branch not taken 
                                //  above + breq in capture

    "5:  out  0x3f, r20     \n"
  
 : "=z" (bufptr), "=r" (count)
 : "0"  (bufptr), "1"  (count), "r" (xmask)
 : "r0", "r1", "r2", "r3", "r16", "r17", "r20");  

 return max_trace_len - count;
}


// Edge clocked
uint8_t clocked_capture(uint32_t *bufptr, uint8_t max_trace_len)
{
  uint8_t count = max_trace_len;
  
  
  // Output munges low 2 bits of PINB with high 6 of PIND, to speed up capture
  
  asm volatile (
    "    in   r20, 0x3f     \n"
    "    sei                \n"
    
    // Clear time counter
    "    clr  r0            \n"
    "    clr  r1            \n"
    "    clr  r2            \n"

    // Read in initial state
    "    in   r16, 0x03     \n"  // PINB
    "    in   r17, 0x09     \n"  // PIND
    "    mov  r18, r16      \n"  // make copy of PINB
    "    andi r16, 0x03     \n"
    "    andi r17, 0xfc     \n"
    "    or   r16, r17      \n"
    "    andi r18, 0x04     \n"  // mask off PB2 in copy
    "    nop                \n"
    "    nop                \n"
  
    // HERE r16 = combined value of PINB/D, r17 = masked PB/D,
    //  R0 = 0, R1 = TCNT1H, R2 = TCNT1L
    "1:  st   Z+, r16       \n" // 2
    "    st   Z+, r0        \n" // 2
    "    st   Z+, r1        \n" // 2
    "    st   Z+, r2        \n" // 2
    "    mov  r3, r18       \n" // 1 -- keep last state for comparison

    "    dec  %3            \n" // 1
    "    breq 5f            \n" // 1 or 2 (1 when it counts)
        
    // 32 cycles

    "2:  lds  r16,  0xc0    \n" // 2 -- UCSR0A
    "    andi r16,  0x80    \n" // 1 -- RXC (receive complete)
    "    brne       5f      \n" // 1 or 2 (1 when it counts)

    // Increment time counter
    "    inc  r0            \n" // 1
    "    breq 3f            \n" // 1 or 2
    "    nop                \n" // 1
    "    jmp  4f            \n" // 3
    // Always 3 cycles
    "3:  inc  r1            \n" // 1
    "    brne 4f            \n" // 1 or 2
    "    inc  r2            \n" // 1


    "4:  nop                \n" // 1  (make even 32 cycles)
    "    in   r16,  0x03    \n" // 1 -- PINB
    "    in   r17,  0x09    \n" // 1 -- PIND
    "    mov  r18,  r16     \n" // 1 (make copy of PINB in r18 to mask off clk pin)
    "    andi r16,  0x03    \n" // 1
    "    andi r17,  0xfc    \n" // 1
    "    or   r16,  r17     \n" // 1
    "    andi r18,  0x04    \n" // 1  (mask off PB2's bit in r18)
    "    cp   r18,  r3      \n" // 1  (see if it's changed from last time)
    "    brne 1b            \n" // 1 or 2
    
    // Delays to keep timing constant even if don't capture
    
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // st Z+ (2)
    "    nop                \n"
    "    nop                \n" // mov
    "    nop                \n" // dec
    "    breq 2b            \n" // always -- 2 -- makes up for branch not taken 
                                //  above + breq in capture

    "5:  out  0x3f, r20     \n"
 : "=z" (bufptr), "=r" (count)
 : "0" (bufptr), "1" (count)
 : "r0", "r1", "r2", "r3", "r16", "r17", "r18", "r20");
 
 return max_trace_len - count;
}


void setup()
{
  DDRB = _BV(PB4);
  DDRD = 0;
  serial_init(BAUD);
  serial_putstr("ok.\n");
}


void start_trace()
{
  serial_putstr("Tracing...\n");
  
  memset(trace_buffer, 0, sizeof(trace_buffer));
  
  if (trace_mode == STATE_CHANGE) {
    trace_length = state_change_capture(trace_buffer, max_trace_length, trace_mask);
  } else {
    trace_length = clocked_capture(trace_buffer, max_trace_length);
  }
  
  serial_putstr("Tracing done (");
  serial_puthex(trace_length);
  serial_putstr(").\n");
}


void dump_trace()
{
  int i;


  serial_putstr("Trace dump:\n");
  for (i = 0; i < trace_length; i++) {
    serial_puthex(trace_buffer[i] >> 24);
    serial_puthex(trace_buffer[i] >> 16);
    serial_puthex(trace_buffer[i] >> 8);
    serial_putstr(":");
    // Straighten out the pin munging
    serial_puthex((trace_buffer[i] & 0xff) >> 2 | (trace_buffer[i] & 0x03) << 6);
    serial_putc('\n');
  }
  serial_putstr("done.\n");
}


void loop()
{
  int c = serial_getc();
  
  switch(c) {
    case 't':  // Trace
      start_trace();
      break;

    case 'd':  // Dump Trace
      dump_trace();
      break;
      
    case 'm':  // Set trace mode
      do {
        c = serial_getc();
      } while (c == -1);
      
      if (c == 'c') {
        trace_mode = CLOCKED;
        serial_putstr("Mode now clocked.\n");
      } else if (c == 's') {
        trace_mode = STATE_CHANGE;
        serial_putstr("Mode now state change.\n");
      }
      break;    
  }  
}


#ifndef WIRING
#include <avr/interrupt.h>

int main()
{
  sei();
  setup();

  while(1) loop();

  return 0;
}
#endif

