/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\slip.c $
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
 *  Description:  Implementation of protocol slip
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\slip.d $
 *  
 *    MicroLIB - SLIP implementation
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:42:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:45:04
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <slip.h>

/****************************************************************************
 *
 * Function Name: slip_create( slip* ptr )
 *
 * Description:   Constructor
 *
 * Parameters:    slip* ptr
 *
 * Return:        void
 *
 ***************************************************************************/
 void slip_create( slip* ptr )
 {
    /* Check ptr first
     */
     if (ptr != NULL)
     {
        ptr->mRxState  = ST_SLIP_END;
        ptr->mBlocking = 1;
     }
 
 } /* slip_create */

/****************************************************************************
 *
 * Function Name: slip_destroy( slip* ptr )
 *
 * Description:   Destructor
 *
 * Parameters:    slip* ptr
 *
 * Return:        void
 *
 ***************************************************************************/
 void slip_destroy( slip* ptr )
 {
 } /* slip_destroy */

/****************************************************************************
 *
 * Function Name: slip_open( void )
 *
 * Description:   Open the protocol
 *
 * Parameters:    slip* ptr
 *
 * Return:        word
 *
 ***************************************************************************/
 word slip_open( slip* ptr )
 {
     word result = PROTOCOL_ERROR;

    /* Check ptr first
     */
     if (ptr == NULL) return result;
    
    /* Set state
     */
     ptr->mRxState = ST_SLIP_END;
     return PROTOCOL_OK;

 } /* slip_open */

/****************************************************************************
 *
 * Function Name: slip_close( void )
 *
 * Description:   Close the protocol
 *
 * Parameters:    slip* ptr
 *
 * Return:        word
 *
 ***************************************************************************/
 word slip_close( slip* ptr )
 {
     word result = PROTOCOL_ERROR;

    /* Check ptr first
     */
     if (ptr == NULL) return result;
    
    /* Set state
     */
     ptr->mRxState = ST_SLIP_END;
     return PROTOCOL_OK;

 } /* slip_close */
  
/****************************************************************************
 *
 * Function Name: slip_ioctl( void )
 *
 * Description:   Set protocol parameters etc.
 *
 * Parameters:    void* data, word length, byte control, slip* ptr
 *
 * Return:        word
 *
 ***************************************************************************/
 word slip_ioctl( void* data, word length, byte control, slip* ptr )
 {
    word result = PROTOCOL_ERROR;

   /* Check ptr first
    */
    if (ptr == NULL) return result;

    switch(control)
    {
        case PROTOCOL_PORT:
        {
            ptr->mPort  = (port*) data;
            result = PROTOCOL_OK;
        }

        case ASYNC:  
        {
            ptr->mBlocking = 0;
			result = PROTOCOL_OK;
        }
        break;

        case SYNC:
        {
            ptr->mBlocking = 1;
			result = PROTOCOL_OK;
        }
        break;

    } /* switch(control) */

    return result;

 } /* slip_ioctl */


/****************************************************************************
 *
 * Function Name: slip_read( byte* data, word length, slip* ptr )
 *
 * Description:   Read the protocol
 *
 * Parameters:    byte* data, word length, slip* ptr
 *
 * Return:        word
 *
 ***************************************************************************/
 word slip_read( byte* data, word length, slip* ptr )
 {
     word result = PROTOCOL_ERROR;
     word count;

    /* Check ptr first
     */
     if (ptr == NULL) return result;

    /* Check for new frame
     */ 
     if ( ptr->mRxState == ST_SLIP_END)
     {
        ptr->mRxState = ST_SLIP_DATA;
        return PROTOCOL_BEGIN;
     }

    /* check word length
     */
     if ( length > 0 && length < PROTOCOL_CONTROL )
     {
        do
        {
          count = ptr->mPort->read(data, 1, ptr->mPort);
          if ( ptr->mBlocking == 0 && count == 0) return 0;

        } while ( count == 0 );
        
        switch(data[0])
        {
           case SLIP_END:
           {
              result = PROTOCOL_END;
              ptr->mRxState = ST_SLIP_END;
           }
           break;
               
           case SLIP_ESC:
           {
              while ( ptr->mPort->read(data, 1, ptr->mPort) == 0);
              
              if ( data[0] != SLIP_ESC_END && data[0] != SLIP_ESC_ESC)
              {
                 result = PROTOCOL_ERROR;
              }
              else
              {
                 result = 1;
              }
           }
           break;
          
           default:        
           {
              result = 1;
           }
        }
     }
     return result;

 } /* slip_read */


/****************************************************************************
 *
 * Function Name: slip_write( byte* data, word length, slip* ptr )
 *
 * Description:   Write the protocol
 *
 * Parameters:    byte* data, word length, slip* ptr
 *
 * Return:        word
 *
 ***************************************************************************/
 word slip_write( byte* data, word length, slip* ptr )
 {
     word i;
     word count;
     byte out;
     word result = PROTOCOL_ERROR;
     
    /* Check ptr first
     */
     if (ptr == NULL) return result;
     
    /* Send data
     */
     if ( length < PROTOCOL_CONTROL)
     {
        for ( i = 0, count = 0; i< length; i++, count++)
        {
            switch(data[i])
            {
                case SLIP_ESC_END:
                {
                    out = SLIP_ESC;
                    ptr->mPort->write(&out, 1, ptr->mPort);
                    ptr->mPort->write(&(data[i]), 1, ptr->mPort);
                    ++count;
                }
                break;

                case SLIP_ESC_ESC:
                {
                    out = SLIP_ESC;
                    ptr->mPort->write(&out, 1, ptr->mPort);
                    ptr->mPort->write(&(data[i]), 1, ptr->mPort); 
                    ++count;
                }
                break;

                default:
                {
                    ptr->mPort->write(&(data[i]), 1, ptr->mPort);
                }

            } /* switch(data[i]) */

        } /* for */

        result = PROTOCOL_OK;
     }
     else
     {
        switch (length)
        {
            case PROTOCOL_BEGIN:
            {
                result = OK;
            }
            break;

            case PROTOCOL_END:
            {
                out = SLIP_END;
                ptr->mPort->write(&out, 1, ptr->mPort);
                result = OK;
            }
            break;
        }
     }
     return result;
 
 } /* slip_write */

 



