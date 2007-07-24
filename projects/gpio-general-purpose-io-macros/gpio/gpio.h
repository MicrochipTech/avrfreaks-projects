#ifndef _gpio_h_
#define _gpio_h_

/*
 *   Copyright (c) 2006
 *   Dipl.-Ing. Hans-Juergen Heinrichs
 *   33184 Altenbeken, Germany
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 *   * Neither the name of the copyright holders nor the names of
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * ---------------------------------------------------------------------------
 * Change log:
 *
 *      Apr, 30th 2007 : added gpio_tst() to test a PIN value
 *      May, 1st 2007  : added gpio_rd_inv() which reads inverted
 *                       added gpio_set() and gpio_clr()
 * ---------------------------------------------------------------------------
 */


/*
 * ---------------------------------------------------------------------------
 * -------------------------    g p i o . h  ---------------------------------
 * ---------------------------------------------------------------------------
 *  Very intuitive and efficient (G)eneral (P)urpose IO handling.
 *
 *  !! To be efficient, one of optimization levels 1,2,3,s shall be used !!
 *  This include file provides symbolic access to GPIO pins through a few
 *  generic macros and a configuration table. The configuration table holds
 *  information about the GPIO layout and mapping to the hardware ports of
 *  the AVR chip. Using a table to describe the GPIOs in a symbolic way has
 *  several advantages:
 *  1. All bit mappings can be made at a central project place, in one file.
 *  2. Changes to the layout of the GPIOs do not enforce any changes to other
 *     source code places - simply change the config table.
 *  3. In the program itself there is no need to worry about bit positions
 *     or bit masks - bits are handled in a symbolic way.
 *  4. No more worry about anding/oring bits and bitfields.
 *  5. ...
 *
 *  The configuration table is made up from preprocessor #defines. The syntax
 *  is as below:
 *
 *      #define SYMBOL  PORT, MSB, LSB
 *
 *      // SYMBOL  : A unique symbolic name for the GPIO (or GPIO group).
 *      // PORT    : AVR Hardware Port (A,B,C,D,...).
 *      // MSB     : Bit# of the (M)ost  (S)ignificant (B)it in this GPIO group.
 *      / /LSB     : Bit# of the (L)east (S)ignificant (B)it in this GPIO group.
 *
 *  Once the table is created, the GPIOs can be accessed by several macros
 *  (please find the exact calling conventions in the source code below):
 *
 *      gpio_conf(cfg,io,p_da) - Configures the GPIOs (set PORT/DDR reg.)
 *      gpio_wr(px,cfg,da)     - Writes to GPIO registers
 *      gpio_rd(px,cfg)        - Reads GPIO registers (right aligned)
 *      gpio_rd8(px,cfg)       - Reads GPIO registers (unaligned)
 *      gpio_toggle(cfg)       - Toggles GPIOs (write 1's to PIN reg.)
 *      gpio_rd_inv(cfg)       - Reads GPIO regs. right aligned, and inversed
 *      gpio_set(cfg)          - Sets GPIO registers (write 1's to PORT reg.)
 *      gpio_clr(cfg)          - Clears GPIO registers (write 0's to PORT reg.)
 *
 *
 *  Sample code snippet:
 *  ====================
 *
 *      // -------------- begin example -----------------------------
 *
 *      #include <avr/io.h>
 *      #include "gpio.h"
 *
 *      //         S         P    M    L
 *      //         Y         O    S    S
 *      //         M         R    B    B
 *      //         B         T
 *      //         O
 *      //         L
 *      //-------------------------------
 *      #define    LED_cfg   B,   5,   5
 *      #define    SEL_cfg   A,   4,   2
 *      #define    ID_cfg    D,   7,   4
 *
 *      int main(void)
 *      {
 *          volatile unsigned char dummy;
 *          //
 *          //  The simple case -
 *          //  a single output bit.
 *          //  E.g. using LED_cfg.
 *          //
 *          gpio_conf( LED_cfg, OUTPUT, 0 );
 *          gpio_wr( PORT, LED_cfg, 1 );
 *          gpio_set( LED_cfg );
 *          gpio_clr( LED_cfg );
 *          gpio_toggle( LED_cfg );
 *
 *          //
 *          //  A more complex case -
 *          //  Multiple output bits.
 *          //  E.g. using SEL_cfg.
 *          //
 *          gpio_conf( SEL_cfg, OUTPUT, 0x05 );
 *          gpio_wr( PORT, SEL_cfg, 0x07 );
 *          gpio_set( SEL_cfg );
 *          gpio_clr( SEL_cfg );
 *          gpio_toggle( SEL_cfg );
 *
 *          //
 *          //  The complex case -
 *          //  mutliple input bits.
 *          //  E.g. using gpio ID_cfg.
 *          //
 *          gpio_conf( ID_cfg, INPUT, NOPULLUP );
 *          gpio_conf( ID_cfg, INPUT, PULLUP );
 *          if( gpio_rd( PIN, ID_cfg ) == 0x04 )
 *              dummy = 0x01;
 *          if( gpio_rd8( PIN, ID_cfg ) == 0 )
 *              dummy = 0x02;
 *          if( gpio_tst( ID_cfg ) )
 *             dummy = 0x03;
 *      }
 *      // -------------- end example --------------------------------
 *
 *
 *  Description of sample code:
 *  ===========================
 *
 *  The code above creates three symbolic GPIOs named LED_cfg, ID_cfg
 *  and SEL_cfg.
 *
 *  LED_cfg is the symbolic name for port B5. Macro gpio_conf() makes
 *  LED an output with initial value set to '0'. The output can be
 *  changed with calls to gpio_wr(), gpio_set(), gpio_clr() and
 *  gpio_toggle.
 *
 *  SEL_cfg is a symbolic GPIO name for bits4..2 of port A. It is
 *  configured to be an output with an initial value of 0x05, meaning
 *  PA4..2 = "101". After that SEL_cfg is written to 0x07 with
 *  gpio_wr() resulting in PA4..2 being "111". Finally, gpio_set(),
 *  gpio_clr() and gpio_toggle() are used to set, clear and toggle
 *  the bits PA4..2.
 *
 *  ID_cfg is a symbolic GPIO name for bits7..4 of port D. It is set
 *  to input without internal pullups enabled using the macro gpio_conf().
 *  The following line shows how to configure with internal pullups.
 *  The input can be accessed with the macro gpio_rd() which returns
 *  the four bits right justified. The function gpio_rd8() returns
 *  the four bits as they are read from the hardware PIN register and
 *  masked appropriately - this saves code space if only 'equal zero'
 *  or 'non equal zero' is to be checked. The macro gpio_tst() delivers
 *  the same result by implicitly reading the PIN register.
 *
 */


/* ------------------------------------------------------------------
 *
 *  g p i o _ c o n f (cfg, io, p_da)
 *
 *  Configures the GPIO as either output, input with pullup or
 *  input without pullup.
 *
 *  Return      : None
 *  Parameters  : cfg  - Symbolic name of entry in config. table
 *                io   - one of INPUT or OUTPUT
 *                p_da - if io==INPUT: one of PULLUP or NOPULLUP
 *                       if io==OUTPUT: right aligned initial
 *                                      output data
 * ------------------------------------------------------------------
 */
#define INPUT       0
#define OUTPUT      1
#define PULLUP      1
#define NOPULLUP    0

#define gpio_conf(cfg,io,p_da)                                      \
({                                                                  \
    if((io)==INPUT) {                                               \
        ARG_ddr(cfg) =ARG_ddr(cfg) &~ARG_mask(cfg);                 \
        if((p_da)==PULLUP)                                          \
            ARG_port(cfg)=ARG_port(cfg)| ARG_mask(cfg);             \
        else if((p_da)==NOPULLUP)                                   \
            ARG_port(cfg)=ARG_port(cfg)&~ARG_mask(cfg);             \
    } else if((io)==OUTPUT) {                                       \
        ARG_ddr(cfg) = ARG_ddr(cfg)| ARG_mask(cfg);                 \
        ARG_port(cfg)=(ARG_port(cfg)&~ARG_mask(cfg))                \
                     |((p_da)<<ARG_lsb(cfg));                       \
    }                                                               \
})


/* ------------------------------------------------------------------
 *
 *  g p i o _ w r (reg, cfg, da)
 *
 *  Writes to a GPIO register.
 *
 *  Return      : None
 *  Parameters  : reg - Register (one of PORT, DDR or PIN)
 *                cfg - Symbolic name of entry in config. table
 *                da  - right aligned write data
 * ------------------------------------------------------------------
 */
#define gpio_wr(reg,cfg,da)                                         \
({                                                                  \
    if(ARG_nbits(cfg)==1) {                                         \
        if((da)==0)ARG_reg(reg,cfg)&=~(1<<ARG_lsb(cfg));            \
        else       ARG_reg(reg,cfg)|= (1<<ARG_lsb(cfg));            \
    } else {                                                        \
        ARG_reg(reg,cfg)=(ARG_reg(reg,cfg)&~ARG_mask(cfg))|         \
                       (((da)<<ARG_lsb(cfg))&ARG_mask(cfg));        \
    }                                                               \
})


/* ------------------------------------------------------------------
 *
 *  g p i o _ r d (reg, cfg)
 *
 *  Reads from an GPIO register.
 *
 *  Return      : Right aligned data read from GPIO register
 *  Parameters  : reg - Register (one of PORT, DDR or PIN)
 *                cfg - Symbolic name of entry in config. table
 * ------------------------------------------------------------------
 */
#define gpio_rd(reg,cfg)                                            \
({                                                                  \
    typeof(ARG_reg(reg,cfg)) f=ARG_reg(reg,cfg);                    \
    f&=ARG_mask(cfg);                                               \
    f>>=ARG_lsb(cfg);                                               \
})


/* ------------------------------------------------------------------
 *
 *  g p i o _ r d _ i n v (reg, cfg)
 *
 *  Reads from an GPIO register.
 *
 *  Return      : Right aligned data read from GPIO register
 *  Parameters  : reg - Register (one of PORT, DDR or PIN)
 *                cfg - Symbolic name of entry in config. table
 * ------------------------------------------------------------------
 */
#define gpio_rd_inv(reg,cfg)                                        \
({                                                                  \
    typeof(ARG_reg(reg,cfg)) f=ARG_reg(reg,cfg);                    \
    f&=ARG_mask(cfg);                                               \
    f^=ARG_mask(cfg);                                               \
    f>>=ARG_lsb(cfg);                                               \
})


/* ------------------------------------------------------------------
 *
 *  g p i o _ r d 8 (reg, cfg)
 *
 *  This is the most efficient way to check for zero or non zero
 *  of GPIOs.
 *  Reads a complete 8 bit vector (not right aligned but masked
 *  appropriately) from an GPIO register. This results in smaller
 *  code size because return data needs not to be right aligned.
 *
 *  Return      : Unaligned (but masked) data read from GPIO register
 *  Parameters  : reg - Register (one of PORT, DDR or PIN)
 *                cfg - Symbolic name of entry in config. table
 * ------------------------------------------------------------------
 */
#define gpio_rd8(reg,cfg)                                           \
({                                                                  \
    typeof(ARG_reg(reg,cfg)) f=ARG_reg(reg,cfg);                    \
    f&=ARG_mask(cfg);                                               \
})


/* ------------------------------------------------------------------
 *
 *  g p i o _ t o g g l e (cfg)
 *
 *  Toggles this GPIO by writing a '1' to all corresponding bits
 *  in the PIN register.
 *
 *  Return      : None
 *  Parameters  : cfg - Symbolic name of entry in config. table
 * ------------------------------------------------------------------
 */
#define gpio_toggle(cfg)                                            \
({                                                                  \
    if( ARG_nbits(cfg)==1 )                                         \
        ARG_pin(cfg)|=ARG_mask(cfg);                                \
    else                                                            \
        ARG_pin(cfg)=ARG_mask(cfg);                                 \
})


/* ------------------------------------------------------------------
 *
 *  g p i o _ s e t (cfg)
 *  g p i o _ c l r (cfg)
 *
 *  Set/Clears this GPIO by writing to the PORT register. or
 *
 *  Return      : None
 *  Parameters  : cfg - Symbolic name of entry in config. table
 * ------------------------------------------------------------------
 */
#define gpio_set(cfg)  ({ ARG_port(cfg) |=  ARG_mask(cfg); })
#define gpio_clr(cfg)  ({ ARG_port(cfg) &= ~ARG_mask(cfg); })


/* ------------------------------------------------------------------
 *
 *  g p i o _ t s t (cfg)
 *
 *  Tests PIN values for zero.
 *
 *  Return      : 0 if PIN is/are 0
 *              : non equal 0 if PIN is/are non equal 0
 *  Parameters  : cfg - Symbolic name of entry in config. table
 * ------------------------------------------------------------------
 */
#define gpio_tst(cfg)  (ARG_pin(cfg)&ARG_mask(cfg))



/* =============================================================== */
/*  Internally required macros - not for user access.              */
/* =============================================================== */
#define ARG_port(P,...)         (PORT ## P)
#define ARG_ddr(P,...)          (DDR  ## P)
#define ARG_pin(P,...)          (PIN  ## P)
#define ARG_lsb(P,msb,lsb)      (lsb)
#define ARG_reg(reg,P,... )     (reg ## P)
#define ARG_nbits(P,msb,lsb)    ((msb)-(lsb)+1)

/*
 *  The macro below creates a bitmask with '1' bits, beginnning from
 *  msb downto lsb. E.g. ARG_mask(D,4,1) creates the value 0x1E.
 */
#define ARG_mask(P,msb,lsb)     (((1<<((msb)+1))-1)&~((1<<(lsb))-1))

#endif /* _gpio_h_ */
