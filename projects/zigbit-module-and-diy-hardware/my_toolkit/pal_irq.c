/**
 * @file pal_irq.c
 *
 * @brief PAL IRQ functionality
 *
 * This file contains functions to initialize, enable, disable and install
 * handler for the transceiver interrupts.
 *
 * $Id: pal_irq.c 22917 2010-08-13 10:15:57Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================ */

#include <stdint.h>
#include "pal.h"
#include "pal_boardtypes.h"

#if (BOARD_TYPE == MY_BOARD)



/* === Globals ============================================================= */

/*
 * Function pointers to store the callback function of
 * the transceiver interrupt
 */
static irq_handler_t irq_hdl_trx;

/* === Prototypes ========================================================== */


/* === Implementation ====================================================== */

/**
 * @brief Initializes the main transceiver interrupt
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the main transceiver interrupt
 *
 * @param trx_irq_cb Callback function for the main transceiver interrupt
 */
void pal_trx_irq_init(FUNC_PTR trx_irq_cb)
{
    /*
     * Set the handler function.
     * The handler is set before enabling the interrupt to prepare for spurious
     * interrupts, that can pop up the moment they are enabled
     */
    irq_hdl_trx = (irq_handler_t)trx_irq_cb;

    /* Init Main TRX interrupt */
    /* Select rising edge */
    EICRB |= _BV(ISC50) | _BV(ISC51);
    /* clear pending interrupt */
    EIFR = _BV(INTF5);
}


#if defined(DOXYGEN)
/**
 * @brief ISR for transceiver's main interrupt
 */
void TRX_IRQ_vect(void);
#else  /* !DOXYGEN */
ISR(TRX_IRQ)
{
    irq_hdl_trx();
}
#endif /* defined(DOXYGEN) */

#endif /* ATZB_24_MN2 */

/* EOF */
