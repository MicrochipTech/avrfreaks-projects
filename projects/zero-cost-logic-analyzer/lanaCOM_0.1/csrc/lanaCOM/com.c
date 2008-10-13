/*
 * lanaCOM - 4 bit logic analyzer using COM-port interface
 * Copyright (c) 2008, Dipl.-Ing. Hans-Juergen Heinrichs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* ---------------------------------------------------------------------------
 *    c o m
 *
 *    Access functions for the COM port I/O registers.
 *
 * ---------------------------------------------------------------------------
 */

/*
 *        Device  I/O address range IRQ
 *        COM1    0x3F8 - 0x3FF     4
 *        COM2    0x2F8 - 0x2FF     3
 *        COM3    0x3E8 - 0x3EF     4
 *        COM4    0x2E8 - 0x2EF     3
 *
 *
 *        I/O port  Read/ Write   Description
 *        base      RBR / THR     RX buffer / TX buffer
 *        base+1    IER / IER     Interupt enable
 *        base+2    IIR / FCR     Interrupt Info / FIFO control
 *        base+3    LCR / LCR     Line control
 *        base+4    MCR / MCR     Modem control
 *        base+5    LSR / -       Line status
 *        base+6    MSR / -       Modem status
 *        base+7    SCR / SRC     Scratch
 *
 *
 *        IER : Interrupt enable register
 *        Bit Description
 *        0   Received data available
 *        1   Transmitter holding register empty
 *        2   Receiver line status register change
 *        3   Modem status register change
 *        4   Sleep mode (16750 only)
 *        5   Low power mode (16750 only)
 *        6   reserved
 *        7   reserved
 *
 *        MSR : Modem status register
 *        Bit Description
 *        0   change in Clear to send
 *        1   change in Data set ready
 *        2   trailing edge Ring indicator
 *        3   change in Carrier detect
 *        4   Clear to send
 *        5   Data set ready
 *        6   Ring indicator
 *        7   Carrier detect
 *
 *        MCR : Modem control register
 *        Bit Description
 *        0   Data terminal ready
 *        1   Request to send
 *        2   Auxiliary output 1
 *        3   Auxiliary output 2
 *        4   Loopback mode
 *        5   Autoflow control (16750 only)
 *        6   Reserved
 *        7   Reserved
 *
 * ===============================================================
 *
 *  DB-9 pin mapping
 *
 *           D               G
 *           C               N
 *           D               D
 *       _________________________
 *       \   1   2   3   4   5   /
 *        \    6   7   8   9    /
 *         \___________________/
 *             D       C   R
 *             S       T   I
 *             R       S
 *
 *  DB-25 pin mapping
 *                           C   D   G   D
 *                           T   S   N   C
 *                           S   R   D   D
 *       _________________________________________________________
 *       \   1   2   3   4   5   6   7   8   9  10  11  12  13   /
 *        \   14  15  16  17  17  19  20  21  22  23  24  25    /
 *         \___________________________________________________/
 *                                            R
 *                                            I
 *
 */

#define COM1_BASE   0x3F8
#define COM2_BASE   0x2F8
#define COM3_BASE   0x3E8
#define COM4_BASE   0x2F8

#define RBR_TBR_OFFSET      0
#define IER_OFFSET          1
#define IIR_FCR_OFFSET      2
#define LCR_OFFSET          3
#define MCR_OFFSET          4
#define LSR_OFFSET          5
#define MSR_OFFSET          6
#define SCR_OFFSET          7

#define DSR_MASK            0x02
#define CTS_MASK            0x01
#define RTS_MASK            0x02
#define DTR_MASK            0x01

#include <stdio.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <sys/io.h>

#include "com.h"
#include "inout.h"

/* -------------------------------------------------------------------------------- */
/*  Static variables                                                                */
/* -------------------------------------------------------------------------------- */
static unsigned short comMSR_addr;
static unsigned short comMCR_addr;
static unsigned short comIER_addr;
static unsigned char  comIER_data;

static int comFD = -1;


/* -------------------------------------------------------------------------------- */
/*  Function name:      C O M _ o p e n ()                                          */
/*                                                                                  */
/*  Returns:            File descriptor number of opened serial line                */
/*  Parameters:         Pointer to name of serial device                            */
/*  Description:        Opens a serial device                                       */
/* -------------------------------------------------------------------------------- */
int
COM_open( int comport )
{
    char ttyName[64];

    /* Create the name of ttyName.  */
    sprintf( ttyName, "%s%d", BASE_TTY_NAME, comport-1 );

    /* Open with O_NDELAY.  */
    comFD = open( ttyName, O_RDWR | O_NDELAY | O_EXCL );
    return comFD;
}


/* -------------------------------------------------------------------------------- */
/*  Function name:      c h e c k _ u a r t ()                                      */
/*                                                                                  */
/*  Returns:            0 = no UART found at base address                           */
/*                      1 = UART found at base address                              */
/*  Parameters:         UART base address to check                                  */
/*  Description:        Checks if given base address is a UART I/O                  */
/* -------------------------------------------------------------------------------- */
static int
check_uart( unsigned short base )
{
    unsigned short comSCR     = base + SCR_OFFSET;
    unsigned short comIIR_FCR = base + IIR_FCR_OFFSET;
    unsigned int data;
    int scratch_present, fifo_present;

    /* Check for scratch register through writing 0x55, 0xaa.   */
    scratch_present = 1;
    _outp( comSCR, 0x55 );
    if ( _inp( comSCR ) != 0x55 ) scratch_present = 0;
    _outp( comSCR, 0xaa );
    if ( _inp( comSCR ) != 0xaa ) scratch_present = 0;

    /* Check for FIFO.   */
    _outp( comIIR_FCR, 0x01 );
    data = _inp( comIIR_FCR );
    _outp( comIIR_FCR, 0x00 );

    fifo_present = data & 0x0080;

    return (scratch_present && fifo_present) ? 1 : 0;
}


/* -------------------------------------------------------------------------------- */
/*  Function name :     C O M _ i n i t ()                                          */
/*                                                                                  */
/*  Return:             <0 - no UART found (-1*IOaddress)                           */
/*                      >0 - is base IO adddress of COM port                        */
/*  Parameters:         1. com port number                                          */
/*                      2. IO address of com port                                   */
/*  Description:        Setup the I/O addresses and disable interrupts              */
/*                      from this com port.                                         */
/* -------------------------------------------------------------------------------- */
int
COM_init(int comport, int ioaddr)
{
    int basePortNr;
    switch( comport )  {
        case 1  : basePortNr = COM1_BASE; break;
        case 2  : basePortNr = COM2_BASE; break;
        case 3  : basePortNr = COM3_BASE; break;
        case 4  : basePortNr = COM4_BASE; break;
        default : basePortNr = ioaddr;    break;
    }


    comMCR_addr = basePortNr + MCR_OFFSET;      /* Set HW I/O addresses.            */
    comMSR_addr = basePortNr + MSR_OFFSET;
    comIER_addr = basePortNr + IER_OFFSET;

    if( check_uart(basePortNr) == 0 )           /* Check, whether we access a UART. */
        return -1 * basePortNr;

    /*
     * Disabling all interrupts from this com port
     * is required because flow control signals create
     * interrupts (and disturbing the sample cycles)
     * otherwise.
     */
    comIER_data = _inp( comIER_addr );  /* Save IER for restoring it later.             */
    _outp( comIER_addr, 0x00 );         /* Disable all interrupts from this COM port.   */

    return basePortNr;
}


/* -------------------------------------------------------------------------------- */
/*  Function name:      C O M _ g e t d a t a ()                                    */
/*                                                                                  */
/*  Return:             8 bit data from MSR                                         */
/*  Parameters:         -                                                           */
/*  Description:        Read Data from Serial COM port                              */
/* -------------------------------------------------------------------------------- */

unsigned char
COM_getdata(void)
{
    return (unsigned char)_inp( comMSR_addr );
}

/*
#include <sys/termios.h>
#include <sys/ioctl.h>

unsigned char
COM_getdata(void)
{
    int status;
    int data = 0;
    if( comFD )
        ioctl( comFD, TIOCMGET, &status );
    if( status & TIOCM_CTS ) data |= 0x10;
    if( status & TIOCM_DSR ) data |= 0x20;
    if( status & TIOCM_RI  ) data |= 0x40;
    if( status & TIOCM_CD  ) data |= 0x80;
    return data;
}
*/


/* -------------------------------------------------------------------------------- */
/*  Function name:      C O M _ c l o s e ()                                        */
/*                                                                                  */
/*  Return:             -                                                           */
/*  Parameters:         -                                                           */
/*  Description:        Closes the com port and restores IER                        */
/* -------------------------------------------------------------------------------- */
void
COM_close(void)
{
    if( comFD > 0 )
        close( comFD );

    _outp( comIER_addr, comIER_data );  /* Restore IER. */
}
