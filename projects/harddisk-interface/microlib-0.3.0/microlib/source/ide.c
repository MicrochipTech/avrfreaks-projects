/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\ide.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:42:24 $
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
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\ide.d $
 *  
 *    MicroLIB - ide functions
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:42:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:44:46
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <ide.h>

/****************************************************************************
 *
 * Function Name: ide_hardreset()
 *
 * Description:   Execute hardware reset
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_hardreset(void)
 {
    /* Execute result
     */
     ide_ioctl(NULL, 0, IDE_IOCTL_RESET);

 } /* reset */


/****************************************************************************
 *
 * Function Name: ide_softreset()
 *
 * Description:   Execute software reset
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_softreset(void)
 {
   /* Execute software reset
    */
    ide_ioctl(NULL, 0, IDE_IOCTL_SOFTRESET);

 } /* reset */


/****************************************************************************
 *
 * Function Name: ide_status()
 *
 * Description:   Get IDE status
 *
 * Parameters:    void
 *
 * Return:        byte - IDE staus
 *
 ***************************************************************************/
 byte ide_status( void )
 {
    return ide_inbyte(IDE_BLOCK_COMMAND,7);

 } /* ide_status */


/****************************************************************************
 *
 * Function Name: ide_error()
 *
 * Description:   Get IDE error staus
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte ide_error( void )
 {
    return ide_inbyte(IDE_BLOCK_COMMAND,1);
 
 } /* ide_error */


/****************************************************************************
 *
 * Function Name: ide_read( byte name )
 *
 * Description:   Read an ide controller register
 *
 * Parameters:    byte name
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte ide_read( byte name )
 {
    byte result = 0;

    switch (name)
    {
      case ERROR_REGISTER:         
      {
        /* Command register #1: 'Error register'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,1);
      }
      break;

      case SECTOR_REGISTER:
      {
        /* Command register #2: 'Sector register'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,2);
      }
      break;

      case SECTOR_NUMBER_REGISTER:
      {
        /* Command register #3: 'Sector number register'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,3);

      }
      break;

      case LOW_CYLINDER_REGISTER:
      {
        /* Command register #4: 'Cylinder register, low byte'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,4);
      }
      break;

      case HIGH_CYLINDER_REGISTER:
      {
        /* Command register #5: 'Cylinder register, high byte'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,5);
      }
      break;

      case DRIVE_REGISTER:
      {
        /* Command register #6: 'Drive / Head register'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,6);

      }
      break;

      case STATUS_REGISTER:
      {
        /* Command register #7: 'Status register'
         */
         result = ide_inbyte(IDE_BLOCK_COMMAND,7);

      }
      break;

      case ALT_STATUS_REGISTER:
      {
        /* Control register #6: 'Alternative status register'
         */
         result = ide_inbyte(IDE_BLOCK_CONTROL,6);
      }
      break;

      case ADDRESS_REGISTER:
      {
        /* Control register #7: 'Address register'
         */
         result = ide_inbyte(IDE_BLOCK_CONTROL,7);

      }
    }
    return result;

 } /* ide_read */

/****************************************************************************
 *
 * Function Name: ide_write( byte name, byte data )
 *
 * Description:   Write into an ide controller register
 *
 * Parameters:    byte name, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_write( byte name, byte data )
 {
    switch (name)
    {
      case FEATURE_REGISTER:        /* Command register 1
                                     */
                                     ide_outbyte(0,1,data);
                                     break;

      case SECTOR_REGISTER:         /* Command register 2
                                     */
                                     ide_outbyte(0,2,data);
                                     break;

      case SECTOR_NUMBER_REGISTER:  /* Command register 3
                                     */
                                     ide_outbyte(0,3,data);
                                     break;

      case LOW_CYLINDER_REGISTER:   /* Command register 4
                                     */
                                     ide_outbyte(0,4,data);
                                     break;
                                       
      case HIGH_CYLINDER_REGISTER:  /* Command register 5
                                     */
                                     ide_outbyte(0,5,data);
                                     break;

      case DRIVE_REGISTER:          /* Command register 6
                                     */
                                     ide_outbyte(0,6,data);
                                     break;

    case COMMAND_REGISTER:          /* Command register 7
                                     */
                                     ide_outbyte(0,7,data);
                                     break;
    }
 
 } /* ide_write */

/****************************************************************************
 *
 * Function Name: ide_read_data()
 *
 * Description:   Read ide data register
 *
 * Parameters:    void
 *
 * Return:        word
 *
 ***************************************************************************/
 word ide_read_data( void )
 {
    return ide_inword(0,0);

 } /* ide_read_data */

/****************************************************************************
 *
 * Function Name: ide_write_data()
 *
 * Description:   Write into ide data register
 *
 * Parameters:    word data
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_write_data( word data )
 {
   ide_outword(0,0,data);

 } /* ide_write_data */



 

