/*
 * spin_delay.h:
 *  Static inline functions for relatively precise timing
 *
 *          (c) 2004-2007 Tymm Twillman <tymm@booyaka.com>
 *
 */

#ifndef SPIN_DELAY_H
#define SPIN_DELAY_H

#ifndef MCU_CLK
# error
# error
# error Please define MCU_CLK on compile line (in Hz)
# error when including spin_delay.h
# error
# error Example: use -DMCU_CLK=4000000
# error for 4MHz clock rate
# error
# error
#endif

#ifdef AVR

/*
 * Basic spin delay functions
 */

# define LOOPS_PER_MS ((MCU_CLK/1000)/4)

# if (MCU_CLK >= 4000000)

# define LOOPS_PER_US ((MCU_CLK/1000000)/4)

/* Spin for about (us) microseconds
 * The inner loop takes 4 cycles per iteration
 */

static inline void us_spin(unsigned short us)
{
    if (us) {
        __asm__ __volatile__ (
            "1: ldi r26, %2     \n"
            "2: dec r26         \n"
            "   brne 2b         \n"
            "   sbiw %0, 1      \n"
            "   brne 1b         \n"
             : "=w" (us)
            : "w" (us), "i" (LOOPS_PER_US)
            : "r26"
        );
    }
}

# else /* MCU_CLK < 4000000 */

# define LOOPS_PER_8US ((MCU_CLK / 125000) / 4)

/* Spin for about (us) microseconds (slow CPU clock version)
 *    -- This actually does it in 8 us increments
 * the inner loop takes 4 cycles per iteration
 *
 * The us divide will actually add a few cycles extra...
 */

static inline void us_spin(unsigned short us) {
    if (us /= 8) {
        __asm__ __volatile__ (
            "1: ldi r26, %2     \n"
            "2: dec r26         \n"
            "   brne 2b         \n"
            "   sbiw %0, 1      \n"
            "   brne 1b         \n"
            : "=w" (us)
            : "w" (us), "i" (LOOPS_PER_8US)
            : "r26"
        );
    } else {
        __asm__ __volatile__ (
            "  nop  \n"
        );
    }
}

# endif /* if (MCU_CLK >= 4000000) */

/*
 * Spin for about (ms) milliseconds
 *
 */

static inline void ms_spin(unsigned short ms) {
    if (ms) {
        __asm__ __volatile__ (
            "1: ldi r26, %3     \n"
            "    ldi r27, %2     \n"
            "2: sbiw r26, 1     \n"
            "    brne 2b         \n"
            "    sbiw %0, 1      \n"
            "    brne 1b         \n"
            : "=w" (ms)
            : "w" (ms),
              "i" (LOOPS_PER_MS >> 8),
              "i" (0xff & LOOPS_PER_MS)
            : "r26", "r27"
        );
    }
}

#elif defined(MSP430)

# define LOOPS_PER_MS ((MCU_CLK / 1000) / 5)

# if (MCU_CLK >= 4000000)
#  define LOOPS_PER_US ((MCU_CLK / 1000000) / 5)   /* 5 cycles/loop */

/*
 * Spin for about (us) microseconds -- faster clock version
 */

static inline void us_spin(unsigned short us)
{
    if (us) {
        __asm__ __volatile__ (
            "1: mov %1, r4 \n"
            "2: dec r4     \n"
            "   jnz 2b     \n"
            "   dec %0     \n"
            "   jnz 1b     \n"
            :: "r" (us), "i" (LOOPS_PER_US) : "r4"
        );
    }
}

# else

/*
 * Spin for about (us) microseconds (for lower speed clocks,
 *  works in multiples of 8 us)
 *
 */

#  define LOOPS_PER_8US ((MCU_CLK / 125000) / 5)

static inline void us_spin(unsigned short us)
{
    if (us /= 8) {
        __asm__ __volatile__ (
            "1: mov %1, r4 \n"
            "2: dec r4     \n"
            "   jnz 2b     \n"
            "   dec %0     \n"
            "   jnz 1b     \n"
            :: "r" (us), "i" (LOOPS_PER_8US) : "r4"
        );
    }
}

#  endif

/*
 * Spin for about (ms) milliseconds
 *
 */

static inline void ms_spin(unsigned short ms)
{
    if (ms) {
        __asm__ __volatile__ (
            "1: mov %1, r4 \n"
            "2: dec r4     \n"
            "   jnz 2b     \n"
            "   dec %0     \n"
            "   jnz 1b     \n"
            :: "r" (ms), "i" (LOOPS_PER_MS) : "r4"
        );
    }
}

# elif defined(__arm__)

static inline void us_spin(unsigned long us)
{
    us *= (MCU_CLK / 125000) / 3;
    us >>= 5;

    if (us) {

        __asm__ __volatile__ (
            "1: nop              \r\n"
            "   subs  %0, %0, #1 \r\n"
            "   bne  1b          \r\n"
            :: "r" (us));
    }
}


/* Should be good up to about 70s at 60 Mhz */

static inline void ms_spin(unsigned long ms)
{
    ms *= (MCU_CLK / 1000) / 3;
    ms >>= 2;

    if (ms) {

        __asm__ __volatile__ (
            "1: nop              \r\n"
            "   subs  %0, %0, #1 \r\n"
            "   bne  1b          \r\n"
            :: "r" (ms));
    }
}

#endif /* #ifdef AVR ... elif defined(__arm__) ... */

#endif /* #ifndef SPIN_DELAY_H ... */
