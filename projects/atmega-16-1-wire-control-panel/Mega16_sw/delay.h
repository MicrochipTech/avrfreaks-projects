#ifndef DELAY_H
#define DELAY_H

#ifndef F_CPU
#error F_CPU not defined
#endif

inline void DelayCycles(unsigned int __count)
  {
    if ( __count&1)
      {
        __asm__ __volatile__ ( "nop" ); //1 cycle

      }

    if ( __count&2)
      {
        __asm__ __volatile__ ( "rjmp 1f\n 1:" );   // 2 cycles
      }

    if ( __count >=4)
      {
        __count/=4;
      __asm__ __volatile__ ( "1: sbiw %0,1" "\n\t"
                             "brne 1b"           // 4 cycles/loop
                             : "=w" (__count)
                             : "0" (__count));
      }
  }

inline void Delay64KCycles(unsigned int __count)
  {
    while (__count)
      {
        DelayCycles(65535);
        __count--;
      }
  }

//1-65535 ns 
#define Delay_ns(ns) \
if (F_CPU<=65535) \
  DelayCycles((unsigned int)(1+((((unsigned long)F_CPU)*ns)/1000000000))); \
else DelayCycles((unsigned int)(1+(((F_CPU/1000LU)*ns)/1000000)));


//1-65535 us 
#define Delay_us(us) \
if (F_CPU<=65535) \
  DelayCycles((unsigned int)(1+((((unsigned long)F_CPU)*us)/1000000))); \
else \
  { \
    if (1+(((F_CPU/1000LU)*us)/1000)>65535) \
      { \
        Delay64KCycles((unsigned int)((1+(((F_CPU/1000LU)*us)/1000))>>16)); \
        DelayCycles((unsigned int)((1+(((F_CPU/1000LU)*us)/1000))&65535)); \
      } \
    else DelayCycles((unsigned int)(1+(((F_CPU/1000LU)*us)/1000))); \
  }

//1-65535 ms 
#define Delay_ms(ms) \
if (F_CPU<=65535) \
  { \
    if (1+((((unsigned long)F_CPU)*ms)/1000)>65535) \
      { \
        Delay64KCycles((unsigned int)((1+((((unsigned long)F_CPU)*ms)/1000))>>16)); \
        DelayCycles((unsigned int)((1+((((unsigned long)F_CPU)*ms)/1000))&65535)); \
      } \
    else DelayCycles((unsigned int)(1+((((unsigned long)F_CPU)*ms)/1000))); \
  } \
else \
  { \
    if (1+((F_CPU/1000LU)*ms)>65535) \
      { \
        Delay64KCycles((unsigned int)((1+((F_CPU/1000LU)*ms))>>16)); \
        DelayCycles((unsigned int)((1+((F_CPU/1000LU)*ms))&65535)); \
      } \
    else DelayCycles((unsigned int)(1+((F_CPU/1000LU)*ms))); \
  }

#endif //DELAY_H
