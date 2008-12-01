/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\apps\monitor\monitor.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:35:52 $
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
 *  Description:  Monitor program for IDE controller
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\apps\monitor\monitor.d $
 *  
 *    Simple IDE Monitor - main function
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:35:52
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:55:50
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/


/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <os/serial_avr.h>
 #include <os/timer.h>
 #include <ide.h>
 
 #include "ui.h"
 
/****************************************************************************
 *
 * Function Name: main( int argc, char** argv )
 *
 * Description:   Tiny IDE harddisk monitor
 *
 * Parameters:    int argc, char** argv
 *
 * Return:        int
 *
 ***************************************************************************/
 int main( int argc, char** argv )
 {
    uart_init(38400l);
    ide_hardreset();
    startTimer();

   /* Show startup message
    */
    startup();
    menu_main();

   /* Should never come to here ...
    */
    return 0;
 
 } /* main */
  
