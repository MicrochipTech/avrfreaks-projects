#ifndef __IDE_H__
#define __IDE_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\ide.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
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
 *  Description:  
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\include\ide.i $
 *  
 *    MicroLIB - ide function prototypes
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:39:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:41:18
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <common.h>

 #ifdef __ATMEL__
 #include <progmem.h>
 #endif

 #ifdef __TURBOC__
 #endif



/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 
/* IDE ioctl commands
 */ 
 #define  IDE_IOCTL_INIT          0x01
 #define  IDE_IOCTL_RESET         0x02
 #define  IDE_IOCTL_SOFTRESET     0x03
 
/* IDE register blocks
 */ 
 #define  IDE_BLOCK_COMMAND       0x00
 #define  IDE_BLOCK_CONTROL       0x01

/* Symbolic register names
 */
 #define  DATA_REGISTER           0 /* Cmd  block: data register (16 bit)   */
 #define  ERROR_REGISTER          1 /* Cmd  block: error register           */
 #define  FEATURE_REGISTER        2 /* Cmd  block: feature register         */
 #define  SECTOR_REGISTER         3 /* Cmd  block: sector register          */
 #define  SECTOR_NUMBER_REGISTER  4 /* Cmd  block: sector number register   */
 #define  LOW_CYLINDER_REGISTER   5 /* Cmd  block: cylinder register (low)  */
 #define  HIGH_CYLINDER_REGISTER  6 /* Cmd  block: cylinder register (high) */
 #define  DRIVE_REGISTER          7 /* Cmd  block: drive/head register      */
 #define  STATUS_REGISTER         8 /* Cmd  block: stats register           */
 #define  COMMAND_REGISTER        9 /* Cmd  block: command register         */
 #define  ALT_STATUS_REGISTER    10 /* Ctrl block: alt. status register     */
 #define  ADDRESS_REGISTER       11 /* Ctrl block: address register (ATA-1) */
 
/* IDE status bit definitions
 */ 
 #define  IDE_STATUS_BSY       0x80 /* IDE status: Busy                     */
 #define  IDE_STATUS_DRDY      0x40 /* IDE status: Drive ready              */      
 #define  IDE_STATUS_DF        0x20 /* IDE status: Drive fault              */
 #define  IDE_STATUS_DSC       0x10 /* IDE status: Drive seek complete      */
 #define  IDE_STATUS_DRQ       0x08 /* IDE status: Data request             */
 #define  IDE_STATUS_CORR      0x04 /* IDE status: Corrected data           */
 #define  IDE_STATUS_IDX       0x02 /* IDE status: Index                    */
 #define  IDE_STATUS_ERR       0x01 /* IDE status: Error                    */
 
/* IDE error bit definitions
 */ 
 #define  IDE_ERROR_BBK        0x80 /* IDE error: bad block detected (ATA1) */
 #define  IDE_ERROR_UNC        0x40 /* IDE Error: uncorrectable data error  */
 #define  IDE_ERROR_MC         0x20 /* IDE Error: Media change              */
 #define  IDE_ERROR_IDNF       0x10 /* IDE Error: ID not found              */
 #define  IDE_ERROR_MCR        0x08 /* IDE Error: Media change request      */
 #define  IDE_ERROR_ABRT       0x04 /* IDE Error: Aborted command           */
 #define  IDE_ERROR_TK0NF      0x02 /* IDE Error: Track 0 not found         */
 #define  IDE_ERROR_AMNF       0x01 /* IDE Error: Address mark not found    */
 
 
/****************************************************************************
 *                 Typedef Definitions
 ****************************************************************************/

 typedef struct IDE_ConfigurationSector
 {
   word		Configuration;
   word     LogicalCylinders;
   word     __reserved_1__;
   word     LogicalHeads;
   word     BytesPerTrack;
   word     BytesPerSector;
   word     LogicalSectorsPerTrack;
   word     __reserved_2__[3];
   word     SerialNumber[10];
   word     BufferType;
   word     BufferSize;
   word     EccBytes;
   word     FirmwareRevision[4];
   char     Model[40];
   word     SectorsPerInterrupt;
   word     DoubleWordIO;
   word     Capabilities;
   word     __reserved_3__;
   word     PIOTimingMode;
   word     DMATimingMode;
   word     ValidData;
   word     Cylinders;
   word     Heads;
   word     Sectors;
   word     Capacity[2];
   word     CurrentSectorsPerInterrupt;
   word     LBASectors[2];
   word     SingleDMA;
   word     MultipleDMA;
   word     AdvancedPIO;
   word     CycleTime[4];
   word     __reserved_4__[2];
   word     __reserved_5__[4];
   word     QueueLength;                                         
   word     __reserved_6__[4];
   word     MajorRevsion;
   word     MinorRevision;
   word     SupportedFeatures[3];
   word     ActiveFeatures[3];
   word     UltraDMA;
   word     DeletionTime;
   word     ExtendedDeletionTime;
   word     PowerManagement;
   word     __reserved_7__[34];
   word     MediaChangeNotification;
   word     SecurityState;
   word     __reserved_8__[31];
   word     __reserved_9__[96];

 } ConfigurationSector;
 
                
/****************************************************************************
 * Function Prototypes
 ****************************************************************************/
 
/* Low level ide hardware access
 */ 
 word ide_ioctl   (void* data, word length, byte control);

 void ide_outword (byte block, byte address, word data);
 void ide_outbyte (byte block, byte address, byte data);
 word ide_inword  (byte block, byte address);
 byte ide_inbyte  (byte block, byte address);

/* Low level ide access with symbolic names
 */
 void ide_hardreset   ( void );
 void ide_softreset   ( void );
 
 byte ide_status      ( void );
 byte ide_error       ( void );
 
 byte ide_read        ( byte name );
 void ide_write       ( byte name, byte data);
 word ide_read_data   ( void );
 void ide_write_data  ( word data );

/*************************************************************************/
 
#endif /* __IDE_H__ */



