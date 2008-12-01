#ifndef __MONITOR_H__
#define __MONITOR_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\apps\monitor\ui.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.3 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:35:40 $
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
 *  Description:  Header for user interface of ATMEL harddisk controller
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\apps\monitor\ui.i $
 *  
 *    Simple IDE Monitor - header for user interface
 *  
 *  Revision 1.3  by: SH  Rev date: 04.05.2002 14:35:40
 *    Updated copyright to LGPL
 *  
 *  Revision 1.2  by: SH  Rev date: 04.05.2002 12:37:26
 *    - Changed version string 
 *    - Modifed e-mail address in copyright information
 *  
 *  Revision 1.1  by: SH  Rev date: 17.09.2000 20:41:10
 *    - Bugfix for reading/writing 16 bit values from/to ide controller
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:56:10
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <protocol.h>
 #include <ata.h>
                            
/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 #define BLACK      0 
 #define RED        1
 #define GREEN      2
 #define YELLOW     3
 #define BLUE       4
 #define MAGNETA    5
 #define CYAN       6
 #define WHITE      7
                  
 #define TITLE      "*** Mini IDE harddisk monitor - Version 0.3.0 ***"
 #define COPYRIGHT  "Written by Stefan Heesch, Germany, heesch@web.de"
                   
 #define BUSY       IDE_STATUS_BSY
 #define DRQ        IDE_STATUS_DRQ
 #define DRDY       IDE_STATUS_DRDY
 
 #define TIMER(X)   (X < getTimer())
 #define STATUS(X)  (ide_status() & X)
                                                     
/****************************************************************************
 *                 Typedef Definitions
 ****************************************************************************/

/****************************************************************************
 *                 Function prototypes
 ****************************************************************************/
 void ansi_clrscr( void );
 void ansi_curpos( byte x, byte y );
 void ansi_fg( byte color );
 void ansi_bg( byte color );
 
 void printn( byte character, byte n );
 void print( byte* string );

 void startup( void );
 void show_menu( void );
 void menu_main( void );

 void DeviceDiagnostics( void );
 void IdentifyHarddisk( byte Drive );
 void Sleep( void );
 void Wakeup( void );
 void SurfaceTest(byte Drive);
 
 #endif /* __MONITOR_H__ */
 




