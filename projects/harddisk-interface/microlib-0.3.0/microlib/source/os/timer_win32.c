/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\timer_win32.c $
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
 *  Description:  Timer implementation
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\timer_win32.d $
 *  
 *    MicroLIB - timer functions for Windows 95/98/NT/2000
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:51:32
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
 *  WIN32 specific implementation (Windows 95,98, Windows NT, Windows 2000)
 *  -----------------------------------------------------------------------
 *
 ****************************************************************************/
 #ifdef __WIN32__
 
/****************************************************************************
 *                Global variables
 ****************************************************************************/
 long _timer = 0;

/****************************************************************************
 *
 * Function Name: startTimer( void )
 *
 * Description:   Start Timer
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void startTimer( void )
 {
    _timer = GetTickCount();
 
 } /* startTimer */


/****************************************************************************
 *
 * Function Name: stopTimer( void )
 *
 * Description:   Stop timer
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void stopTimer( void )
 {
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
   return (GetTickCount() - _timer);

 } /* getTimer */
 
 
 #endif /* __WIN32__ */


