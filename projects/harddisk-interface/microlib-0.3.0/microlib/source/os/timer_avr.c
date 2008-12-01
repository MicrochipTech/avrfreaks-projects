/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\timer_avr.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:45:42 $
 *
 *  Copyright:    Copyright (C) 2000-2002 Stefan Heesch (heesch@cybervillage.de)
 *
 *  This software/hardware is free software/hardware; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or (at your
 *  option) any later version.
 *
 *  This software is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Description:  Timer implementation for ATMEL AVR controller 90S8515
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\timer_avr.d $
 *  
 *    MicroLIB - timer functions for ATMEL AVR 90S8515
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:51:54
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <common.h>
 #include <os/timer.h>


/****************************************************************************
 *
 *  ATMEL AVR specific implementation for GNU C
 *  -------------------------------------------
 *
 ****************************************************************************/
 #ifdef __ATMEL__

 /* #include <io.h> */
 #include <interrupt.h>
 #include <signal.h>
 #include <timer.h>
                   
/****************************************************************************
 *                Global variables
 ****************************************************************************/
 long _timer = 0;
 int  _tick  = 0;

/****************************************************************************
 *
 * Function Name: SIGNAL( SIG_OVERFLOW0 )
 *
 * Description:   Timer interrupt handler for Timer 0
 *                Increments variable timer by one each millisecond
 *
 * Parameters:    SIG_OVERFLOW0
 *
 * Return:        void
 *
 ***************************************************************************/
 SIGNAL( SIG_OVERFLOW0 )
 {
    ++_timer;
     
   /* reset counter to get this interrupt again 
    */  
    outp(131, TCNT0);          
  
 } /* SIGNAL */
 

/****************************************************************************
 *
 * Function Name: startTimer( void )
 *
 * Description:   Start Timer 0
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void startTimer( void )
 {
     _timer = 0;
     
     outp((1<<TOIE0), TIMSK); /* enable TCNT0 overflow */
     outp(0,   TCNT0);        /* reset TCNT0 */
     outp(3,   TCCR0);        /* count with cpu clock/1024 */

     sei();                   /* enable interrupts */
 
 } /* startTimer */


/****************************************************************************
 *
 * Function Name: stopTimer( void )
 *
 * Description:   Stop timer 0
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void stopTimer( void )
 {
    timer0_stop();  

 } /* stopTimer */


/****************************************************************************
 *
 * Function Name: getTimer( void )
 *
 * Description:   Get actual value of the millisecond timer
 *
 * Parameters:    void
 *
 * Return:        long
 *
 ***************************************************************************/
 long getTimer( void )
 {
   return _timer;

 } /* getTimer */


#endif /* __ATMEL__ */
