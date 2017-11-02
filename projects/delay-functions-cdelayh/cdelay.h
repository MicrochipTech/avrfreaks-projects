#ifndef CDELAY_H
#define CDELAY_H

//approx cycles will select the next larger number of cycles that avoids the waste
//of the nop or rjmp +0 commands which use 2 bytes of flash for scenarios where one
//does not care if the delay overshoots by up to 5 cycles.

//single begins at 6                    6, 9, 12 ... 765                    (x3)
//double begins at 769                  769, 773, 777, 262141               (x4)
//triple begins at 262145               262147, 262152, 83886077            (x5)
//quad begins at 83886082               83886087, 83886093                  (x6)

#define approxcycles1(x)  ((x>=6)?(((x+2)/3)*3):x)
#define approxcycles15(x) ((x>765)?769:approxcycles1(x))
#define approxcycles2(x)  ((x>=769)?(((x+2)/4)*4+1):approxcycles15(x))
#define approxcycles25(x) ((x>262141)?262147:approxcycles2(x))
#define approxcycles3(x)  ((x>=262147)?(((x+2)/5)*5+2):approxcycles25(x))
#define approxcycles35(x) ((x>83886077)?83886087:approxcycles3(x))
#define approxcycles(x)   ((x>=83886087)?(((x+2)/6)*6+3):approxcycles35(x))

#define nscycles(x) (F_CPU/1000000000.0*x)
#define uscycles(x) (F_CPU/1000000.0*x)
#define mscycles(x) (F_CPU/1000.0*x)
#define scycles(x)  (F_CPU/1.0*x)

#define ceiling(x)  (((unsigned long)x!=x)?((unsigned long)x+1):(unsigned long)x)

#define Delay_ns(x) __builtin_avr_delay_cycles(ceiling(nscycles(x)))
#define Delay_us(x) __builtin_avr_delay_cycles(ceiling(uscycles(x)))
#define Delay_ms(x) __builtin_avr_delay_cycles(ceiling(mscycles(x)))
#define Delay_s(x)  __builtin_avr_delay_cycles(ceiling(scycles(x)))

#define Delay_ns_approx(x) __builtin_avr_delay_cycles(approxcycles(ceiling(nscycles(x))))
#define Delay_us_approx(x) __builtin_avr_delay_cycles(approxcycles(ceiling(uscycles(x))))
#define Delay_ms_approx(x) __builtin_avr_delay_cycles(approxcycles(ceiling(mscycles(x))))
#define Delay_s_approx(x)  __builtin_avr_delay_cycles(approxcycles(ceiling(scycles(x))))

#endif

