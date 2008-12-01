/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\ide_tc.c $
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
 *  Description:  IDE hardware access for IBM-PC / Turbo C
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\ide_tc.d $
 *  
 *    MicroLIB - IDE functions for MS-DOS / Turbo C++
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:48:30
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/
 
/****************************************************************************
 *
 * Address Assignments for IDE-Interface
 * 
 ****************************************************************************
 *
 *    Adress        |   IDE-Interface
 *   ------------------------------------------
 *   0x1F0 - 0x1F7  |   Command register block
 *   0x3F0 - 0x3F7  |   Control register block
 */

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <ide.h>
 #include <dos.h>
                              
/****************************************************************************
 *
 * Function Name: ide_ioctl( void* data, word length, byte control )
 *
 * Description:   Device control
 *
 * Parameters:    void* data, word length, byte control
 *
 * Return:        word
 *
 ***************************************************************************/
 word ide_ioctl( void* data, word length, byte control )
 {
    word result = 0;

    switch(control)
    {
        case IDE_IOCTL_SOFTRESET:  
        {
           ide_outbyte(IDE_BLOCK_CONTROL, 6, 0x0E);
           delay(300);
           ide_outbyte(IDE_BLOCK_CONTROL, 6, 0x0A);
        }
        break;
    }
    return result;

 } /* ide_ioctl */


/****************************************************************************
 *
 * Function Name: ide_outword()
 *
 * Description:   Write a word to the IDE controller
 *
 * Parameters:    byte block, byte address, word data
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_outword( byte block, byte address, word data )
 {
    switch (block)
    {
       case IDE_BLOCK_COMMAND:
       {
          outport( 0x1F0 + address, data);
       }
       break;

       case IDE_BLOCK_CONTROL:
       {
          outport( 0x3F0 + address, data);
       }
       break;
    }

 } /* ide_outword */

/****************************************************************************
 *
 * Function Name: ide_outbyte()
 *
 * Description:   Write a byte to the IDE controller
 *
 * Parameters:    byte block, byte address, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_outbyte( byte block, byte address, byte data )
 {
    switch (block)
    {
       case IDE_BLOCK_COMMAND:
       {
          outportb( 0x1F0 + address, data);
       }
       break;

       case IDE_BLOCK_CONTROL:
       {
          outportb( 0x3F0 + address, data);
       }
       break;
    }

 } /* ide_outbyte */

/****************************************************************************
 *
 * Function Name: ide_inbyte()
 *
 * Description:   Read a byte from the IDE controller
 *
 * Parameters:    byte block, byte address
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte ide_inbyte( byte block, byte address )
 {
    byte result = 0;

    switch (block)
    {
       case IDE_BLOCK_COMMAND:
       {
          result = inportb( 0x1F0 + address );
       }
       break;

       case IDE_BLOCK_CONTROL:
       {
          result = inportb( 0x3F0 + address );
       }
       break;
    }
    return result;

 } /* ide_inbyte */

 /****************************************************************************
  *
  * Function Name: ide_inword()
  *
  * Description:   Read a word from the IDE controller
  *
  * Parameters:    byte block, byte address
  *
  * Return:        word
  *
  ***************************************************************************/
  word ide_inword( byte block, byte address )
  {
     word result;

     switch (block)
     {
        case IDE_BLOCK_COMMAND:
        {
           result = inport( 0x1F0 + address );
        }
        break;

        case IDE_BLOCK_CONTROL:
        {
           result = inport( 0x3F0 + address );
        }
        break;
     }
     return result;

  } /* ide_inword */




