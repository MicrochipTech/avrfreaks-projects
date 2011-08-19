/*****************************************************************************
Title  :   Accurate Delay Library
Author :   SA Develpoment
Version:   1.02
*****************************************************************************/

#ifndef ADELAY_H
#define ADELAY_H

#include "math.h"
#include "adelay_settings.h"

//disable inline error on -O0 and give a warning that delays will be longer than expected
#ifdef __NO_INLINE__
#warning Delay functions will take longer than expected when inlining is disabled (no optimizations).
static void DelayCycles(unsigned long __count);
static void VariableDelayCycles(unsigned long __count);
#else
static __inline__ void DelayCycles(unsigned long __count) __attribute__((always_inline));
static __inline__ void VariableDelayCycles(unsigned long __count) __attribute__((always_inline));
#endif

extern void DelayCyclesUI(unsigned int AVariable);
extern void DelayCyclesUL(unsigned long AVariable);

//make sure F_CPU is defined
#ifndef F_CPU
#error F_CPU not defined, use "#define F_CPU 8000000" (an 8mhz example) at the top of your source
#endif


//1, 2, and 3 cycle defines
#define ASM_ONECYCLE __asm__ __volatile__ ("nop");
#define ASM_TWOCYCLES __asm__ __volatile__ ("rjmp 1f\n 1:");
#define ASM_THREECYCLES __asm__ __volatile__ ("lpm");

//delaycycles will delay from 0 to 4294967295 cycles with 100% accuracy
//it EXPECTS a constant value so it can be fully optimized by GCC
//the maximum amount of time that can be delayed is 4294967295/F_CPU - 8mhz would be a maximum of 536.87091 seconds
#ifdef __NO_INLINE__
void DelayCycles(unsigned long __count)
#else
static __inline__ void DelayCycles(unsigned long __count)
#endif
  {
    if (__count>65535)
      {
        switch(__count%6)
          {
            case 1 : ASM_ONECYCLE; break;
            case 2 : ASM_TWOCYCLES; break;
            case 3 : ASM_THREECYCLES; break;
            case 4 : ASM_THREECYCLES; ASM_ONECYCLE; break;
            case 5 : ASM_THREECYCLES; ASM_TWOCYCLES; break;
          }
        __count=(__count-18)/6;
        DelayCyclesUL(__count);
      }
    else
      {
        switch(__count)
          {
            case 0  : break;
            case 1  : ASM_ONECYCLE; break;
            case 2  : ASM_TWOCYCLES; break;
            case 3  : ASM_THREECYCLES; break;
            case 4  : ASM_THREECYCLES; ASM_ONECYCLE; break;
            case 5  : ASM_THREECYCLES; ASM_TWOCYCLES; break;
            case 6  : ASM_THREECYCLES; ASM_THREECYCLES; break;
            case 7  : ASM_THREECYCLES; ASM_THREECYCLES; ASM_ONECYCLE; break;
            case 8  : ASM_THREECYCLES; ASM_THREECYCLES; ASM_TWOCYCLES; break;
            case 9  : ASM_THREECYCLES; ASM_THREECYCLES; ASM_THREECYCLES; break;
            case 10 : ASM_THREECYCLES; ASM_THREECYCLES; ASM_THREECYCLES; ASM_ONECYCLE; break;
            case 11 : ASM_THREECYCLES; ASM_THREECYCLES; ASM_THREECYCLES; ASM_TWOCYCLES; break;
            case 12 : ASM_THREECYCLES; ASM_THREECYCLES; ASM_THREECYCLES; ASM_THREECYCLES; break;
            default : switch(__count%4)
                        {
                          case 0 : ASM_THREECYCLES; break;
                          case 2 : ASM_ONECYCLE; break;
                          case 3 : ASM_TWOCYCLES; break;
                        }
                      __count=(__count-9)/4;
                      DelayCyclesUI(__count);
          }
      }
  }

//variabledelaycycles is designed for when you need to specify a non-constant value for the count
//such as specifying a variable or some other value not known at compile time
//it may overshoot the request number of cycles by as much as 12, but typically no more than
//3 extra cycles for <65536 and 5 extra cycles for >=65536
#ifdef __NO_INLINE__
void VariableDelayCycles(unsigned long __count)
#else
static inline void VariableDelayCycles(unsigned long __count)
#endif
  {
    if (__count>65535)
      {
        __count=(__count-13)/6;
        DelayCyclesUL(__count);
      }
    else
      {
        if (!__count)
          return;
        if (__count<10)
          __count=1;
        else __count=(__count-6)/4;
        DelayCyclesUI(__count);
      }
  }


//defines to handle Delay_ns, Delay_us, Delay_ms, and Delay_s
#define Delay_ns(_ns)     DelayCycles(ceil(((double)(F_CPU)*((double)_ns))/1.0e9))
#define Delay_us(_us)     DelayCycles(ceil(((double)(F_CPU)*((double)_us))/1.0e6))
#define Delay_ms(_ms)     DelayCycles(ceil(((double)(F_CPU)*((double)_ms))/1.0e3))
#define Delay_s(_s)       DelayCycles(ceil(((double)(F_CPU)*((double)_s))/1.0e0))

//defines to handle Delay_ns, Delay_us, Delay_ms, and Delay_s
//we are unable to take advantage of floating point to make this easy because it will link in the floating point library
//it is more critical that you think about how big a number can be for these functions can be specified
//for example - lets say F_CPU is divisible by 1000, look at the top section of defines - if you are specifying nanoseconds
//take 4294967295000/F_CPU and that is the maximum number you can specify.  For an 8mhz, this is 536870 ns/us/ms.
//for the bottom section (or the seconds function), use 4294967295/F_CPU, so for a 32768hz you can specify 131071 ns/us/ms.
//note that 32000hz would be the TOP section
#if (F_CPU%1000==0)
  #define VariableDelay_ns(_ns)     VariableDelayCycles(((F_CPU/1000UL)*_ns)/1000000UL+1)
  #define VariableDelay_us(_us)     VariableDelayCycles(((F_CPU/1000UL)*_us)/1000UL+1)
  #define VariableDelay_ms(_ms)     VariableDelayCycles(((F_CPU/1000UL)*_ms)+1)
#else
  #define VariableDelay_ns(_ns)     VariableDelayCycles((((unsigned long)F_CPU)*_ns)/1000000000UL+1)
  #define VariableDelay_us(_us)     VariableDelayCycles((((unsigned long)F_CPU)*_us)/1000000UL+1)
  #define VariableDelay_ms(_ms)     VariableDelayCycles((((unsigned long)F_CPU)*_ms)/1000+1)
#endif
#define VariableDelay_s(  _s)     VariableDelayCycles(((unsigned long)F_CPU)*_s)

#endif //DELAY_H

