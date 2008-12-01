#ifndef __ATA_H__
#define __ATA_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\ata.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.5 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:39:24 $
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
 *  Description:  Hardware independent ATA functions
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\include\ata.i $
 *  
 *    MicroLIB - ata function prototypes
 *  
 *  Revision 1.5  by: SH  Rev date: 04.05.2002 14:39:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.4  by: SH  Rev date: 04.05.2002 12:38:06
 *    - Changed version string 
 *    
 *  
 *  Revision 1.3  by: SH  Rev date: 20.09.2000 22:34:28
 *    - Updated to version 0.2.0
 *  
 *  Revision 1.2  by: SH  Rev date: 20.09.2000 22:20:16
 *    - Updated to version 0.2.0
 *  
 *  Revision 1.1  by: SH  Rev date: 20.09.2000 22:12:30
 *    - Reduced memory usage
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:40:40
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <common.h>
 #include <protocol.h>

/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 #define ATA_OK      '0' 
 #define ATA_ERROR   '1' 
 
 #define INFO        "MicroLIB, Serial IDE interface - "
 #define VERSION     "Version: 0.3.0"                
                                                                               
/****************************************************************************
 *                 Typedef Definitions
 ****************************************************************************/
 typedef void (*ata_function)(byte control, byte data);

/****************************************************************************
 * Function Prototypes
 ****************************************************************************/

/* Initialize ATA functions
 */
 void ata_init( port* io );

/* Get ATA command handler function
 */
 ata_function ata_command  ( byte command );

/* ATA command handler functions
 */
 void getVersion           ( byte control, byte data );
 void initialiseHardware   ( byte control, byte data );
 void getStatus            ( byte control, byte data );
 void getError             ( byte control, byte data );
 void sleepDrive           ( byte control, byte data );
 void wakeupDrive          ( byte control, byte data );
 void writeRegister        ( byte control, byte data );
 void readRegister         ( byte control, byte data );
 void writeSector          ( byte control, byte data );
 void readSector           ( byte control, byte data );
 void runDiagnostics       ( byte control, byte data );
 void identifyDrive        ( byte control, byte data );
                          
 #endif /* __ATA_H__ */    
                           



