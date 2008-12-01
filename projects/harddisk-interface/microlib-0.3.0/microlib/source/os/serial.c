/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\serial.c $
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
 *  Description:  Implementation of serial port API
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\serial.d $
 *  
 *    MicroLIB - serial port functions
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:48:48
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <os/serial.h>


/****************************************************************************
 *
 *  Template implementation
 *  -----------------------
 *
 ****************************************************************************/
 #if 0


 void serial_create(serial* ptr)
 {
     if (ptr != NULL)
	 {
        ptr->mBlocking = TRUE;
	    ptr->mOpen     = FALSE;
	    ptr->mTimeout  = 0;
	 }
 }

 void serial_destroy(serial* ptr)
 {
     serial_close(ptr);
 }


 word serial_open  (serial* ptr)
 {
     word result = OK;
     return result;
 }

 
 word serial_close (serial* ptr)
 {
    /* Check parameter
	 */
	 if (ptr == NULL) return ERROR;

	/* Close port
	 */
	 if (ptr->mOpen) 
	 {
	     ptr->mOpen = FALSE;
	 }
     return OK;
 }

 
 word serial_ioctl (void* data, word length, byte control, serial* ptr)
 {
     word result = ERROR;   
     
    /* Check parameter
	 */
     if (ptr == NULL) return result;

    /* Handle command
	 */
     switch(control)
     {
	     case TIMEOUT: ptr->mTimeout = length;
			           result = OK;
			           break;

         case ASYNC:   ptr->mBlocking = 0;
			           result = OK;
                       break;

         case SYNC:    ptr->mBlocking = 1;
			           result = OK;
                       break;
	 }
     return result;
 }
 
 word serial_read  (byte* data, word length, serial* ptr)
 {
	 word count = 0;

    /* Check parameter
	 */
	 if (ptr == NULL || data == NULL) return 0;
     	 
     return count;
 }
 word serial_write (byte* data, word length, serial* ptr)
 {
     word count = 0;

	/* Check parameter
	 */
	 if (ptr == NULL) return 0;
 	 
     return count;
 }

 #endif


/****************************************************************************
 *
 *  WIN32 specific implementation (Windows 95,98, Windows NT, Windows 2000)
 *  -----------------------------------------------------------------------
 *
 ****************************************************************************/
 #ifdef WIN32

 byte serial_setup(serial* ptr)
 {
	COMMTIMEOUTS tcb;
    DCB dcb;

   /* Check parameter first
    */
    if (ptr == NULL) return ERROR;
    if (ptr->mHandle == INVALID_HANDLE_VALUE) return ERROR;

   /* Set buffer sizes
    */
	if (!SetupComm(ptr->mHandle, SERIAL_BUFFER_SIZE_RX, SERIAL_BUFFER_SIZE_TX)) return ERROR;


   /* Build standard device control block first
    */
    if (!GetCommState(ptr->mHandle, &dcb)) return ERROR;

   /* General settings
    */
	dcb.DCBlength = sizeof(dcb);
    dcb.fBinary       = 1;
    dcb.fNull         = 0;
    dcb.fAbortOnError = 0;

   /* Baudrate, data bits, stop bits, parity
    */
    dcb.BaudRate = (ULONG) ptr->mBaud;
    dcb.ByteSize = ptr->mData;
    dcb.StopBits = ptr->mStop;

    dcb.Parity            = ptr->mParity;
    dcb.fErrorChar        = 0;
    if (ptr->mParity > 0)
          dcb.fParity     = 1;
    else  dcb.fParity     = 0;

   /* No hardware handshake
    */
    dcb.fOutxCtsFlow      = 0;
    dcb.fOutxDsrFlow      = 0;
    dcb.fDsrSensitivity   = 0;
    dcb.fDtrControl       = DTR_CONTROL_DISABLE;
    dcb.fRtsControl       = RTS_CONTROL_DISABLE;

   /* No software flow control
    */
    dcb.fInX              = 0;
    dcb.fOutX             = 0;
    dcb.XoffChar          = 19;
    dcb.XonChar           = 17;


   /* Configure com port
    */
    if(!SetCommState(ptr->mHandle, &dcb)) return ERROR;

   /* Set timeouts
    */
    tcb.ReadIntervalTimeout         = 0;
    tcb.ReadTotalTimeoutConstant    = 50;
    tcb.ReadTotalTimeoutMultiplier  = 0;
    tcb.WriteTotalTimeoutConstant   = 0;
    tcb.WriteTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(ptr->mHandle, &tcb)) return ERROR;

	return OK;
 }


 void serial_create(serial* ptr)
 {
     if (ptr != NULL)
	 {
        ptr->mBlocking = TRUE;
        ptr->mPort     = 1;
	    ptr->mBaud     = 9600;
	    ptr->mParity   = 0;
        ptr->mData     = 8;
	    ptr->mStop     = 0;
	    ptr->mOpen     = FALSE;
	    ptr->mTimeout  = 0;
		ptr->mHandle   = INVALID_HANDLE_VALUE;
	 }
 }

 void serial_destroy(serial* ptr)
 {
     if (ptr == NULL) return;
     serial_close(ptr);
 }


 word serial_open  (serial* ptr)
 {
     word result = ERROR;
     char name[8];
     
    /* Check parameter
	 */
	 if (ptr == NULL) return result;
   
    /* Create port name
	 */
	 sprintf(name,"COM%d", ptr->mPort);

    /* Open the port
     */
	 if ( ptr->mOpen == FALSE)
     {
         
         ptr->mHandle = CreateFile( name, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,OPEN_EXISTING, 0, 0);

         if (ptr->mHandle != INVALID_HANDLE_VALUE)
         {
            result = serial_setup(ptr);
            if (result == OK) 
			{
				ptr->mOpen = TRUE;
			}
			else
			{
				CloseHandle(ptr->mHandle);
				ptr->mHandle = INVALID_HANDLE_VALUE;
			}
         }
     }
     return result;
 }

 
 word serial_close (serial* ptr)
 {
    /* Check parameter
	 */
	 if (ptr == NULL) return ERROR;

	/* Close port
	 */
	 if (ptr->mOpen) 
	 {
	     ptr->mOpen = FALSE;
		 CloseHandle(ptr->mHandle);
		 ptr->mHandle = INVALID_HANDLE_VALUE;
	 }
     return OK;
 }

 
 word serial_ioctl (void* data, word length, byte control, serial* ptr)
 {
     word result = ERROR;   
     
    /* Check parameter
	 */
     if (ptr == NULL) return result;

    /* Handle command
	 */
     switch(control)
     {
	     case TIMEOUT: ptr->mTimeout = length;
			           result = OK;
			           break;

         case ASYNC:   ptr->mBlocking = 0;
			           result = OK;
                       break;

         case SYNC:    ptr->mBlocking = 1;
			           result = OK;
                       break;


         case SERIAL_PORT:
         {
           if ( length == 1 ) ptr->mPort = *((byte*) (data));
         }
         break;

         case SERIAL_BAUD:
         {
           if ( length == 2 ) ptr->mBaud = *((word*) (data));
         }
         break;

	 }
     return result;
 }
 
 word serial_read  (byte* data, word length, serial* ptr)
 {
	 ULONG count;
     ULONG result;
     ULONG start;

    /* Check parameter
	 */
	 if (ptr == NULL || data == NULL) return 0;

    /* get the tickcount 
     */
     start  = GetTickCount();
     count  = 0;
	 result = 0;

    /* read the serial port
	 */
     while ( ptr->mOpen ) {

	  /* Try to read the COM port
       */
       if (!ReadFile( ptr->mHandle, data, length, &count, NULL )) result=GetLastError();

      /* if an error return
       */
       if ( result != 0 && result!= ERROR_MORE_DATA ) return ERROR;

      /* break from the loop 
       */
	   if ( !(ptr->mBlocking) || count> 0)  break;

      /* check for timeout 
       */
       if ( ( ptr->mTimeout != 0 ) && ( GetTickCount() - start > ptr->mTimeout ) )
       {
         count = 0;
         break;
       }

     } /* while (mOpen) */

     return (word) count;
 }

 
 
 word serial_write (byte* data, word length, serial* ptr)
 {
     ULONG count = 0;

	/* Check parameter
	 */
	 if (ptr == NULL) return 0;

	/* Write data
	 */
     if (!WriteFile( ptr->mHandle, data, length, &count, 0 )) 
	 {
		 return 0;
	 }
     else
	 {
         return (word) count;
	 }
 }
 #endif /* WIN32 */
 
/****************************************************************************
 *
 *  MS-DOS specific implementation for TURBO C++
 *  --------------------------------------------
 *
 *  This implementation is based on communications routines in turbo C by
 *  by Peter Ibbotson / Borland
 *
 *  COM Port: Default port is COM1, can also be set to COM2
 *
 *  Sources:  serial_tc.c (was originally named serial.c)
 *            serial_tc.h (was originally named serial.h)
 *
 ****************************************************************************/
 #ifndef __WIN32__
 #ifdef __TURBOC__
 #include <serial_tc.h>

 void serial_create(serial* ptr)
 {
     if (ptr != NULL)
	 {
        ptr->mBlocking = TRUE;
        ptr->mPort     = COM1;
	    ptr->mBaud     = 19200;
	    ptr->mParity   = NO_PAR;
        ptr->mData     = 8;
	    ptr->mStop     = 1;
	    ptr->mOpen     = FALSE;
	    ptr->mTimeout  = 0;
	 }
 }

 void serial_destroy(serial* ptr)
 {
     serial_close(ptr);
 }

 word serial_open  (serial* ptr)
 {
	 word result = OK;

    /* Check parameter and whether port is already open
	 */
	 if (ptr == NULL) return ERROR;
     if (ptr->mOpen)  return OK;

	/* Set parameters ...
	 */
     if ( setserial (ptr->mPort,ptr->mBaud,ptr->mParity,ptr->mData,ptr->mStop) == -1)
     {
        result = ERROR;
     } 
     else
     {  
       /* open the port
	    */
        initserial();
		ptr->mOpen = TRUE;
     }
	 return result;
 }
 
 
 word serial_close (serial* ptr)
 {
    /* Check parameter
	 */
	 if (ptr == NULL) return ERROR;

	/* Close port
	 */
	 if (ptr->mOpen) 
	 {
	     ptr->mOpen = FALSE;
		 closeserial();
	 }
         return OK;
 }

  
 word serial_ioctl (void* data, word length, byte control, serial* ptr)
 {
     word result = ERROR;   
     
    /* Check parameter
	 */
     if (ptr == NULL) return result;

    /* Handle command
	 */
     switch(control)
     {
	     case TIMEOUT: ptr->mTimeout = length;
			           result = OK;
			           break;

         case ASYNC:   ptr->mBlocking = 0;
			           result = OK;
                       break;

         case SYNC:    ptr->mBlocking = 1;
			           result = OK;
                       break;

         case SERIAL_PORT:
         {
           if ( length == 1 ) ptr->mPort = *((byte*) (data));
         }
         break;

         case SERIAL_BAUD:
         {
           if ( length == 2 ) ptr->mBaud = *((word*) (data));
         }
         break;

	 }
     return result;
 }

 word serial_read  (byte* data, word length, serial* ptr)
 {
     word count = 0;
     int  in;

    /* Check parameter
	 */
	 if (ptr == NULL || data == NULL) return 0;

     do
     {
        in = getccb();
        if ( in != -1)
		{
			data[count] = in;
			++count;
		}

     }  while ( ptr->mBlocking && count < length);

	 return count;
 }


 word serial_write (byte* data, word length, serial* ptr)
 {
	 word count = 0;

    /* Check parameter
	 */
	 if (ptr == NULL || data == NULL || length >= PROTOCOL_CONTROL) return 0;

 	 for (count = 0; count < length; count++)
	 {
        SerialOut(data[count]);
	 }
	 return count;
 }

 #endif
 #endif
/****************************************************************************
 *
 *  ATMEL AVR specific implementation for GNU C
 *  -------------------------------------------
 *
 *  This implementation is based on communications routines from Volker Roth
 *  (Files uart.h, uart.c from GCC examples)
 *
 ****************************************************************************/
 #ifdef __ATMEL__
 #include <os/serial_avr.h>

 void serial_create(serial* ptr)
 {
     if (ptr != NULL)
	 {
        ptr->mBlocking = TRUE;
	    ptr->mOpen     = FALSE;
	    ptr->mTimeout  = 0;
	 }
 }

 void serial_destroy(serial* ptr)
 {
     serial_close(ptr);
 }


 word serial_open  (serial* ptr)
 {
     word result = OK;

    /* Check parameter and whether port is already open
	 */
	 if (ptr == NULL) return ERROR;
     if (ptr->mOpen)  return OK;

    /* open the port
     */
     uart_init(ptr->mBaud);
     ptr->mOpen = TRUE;

    /* exit
     */
     return result;
 }


 word serial_close (serial* ptr)
 {
    /* Check parameter
	 */
	 if (ptr == NULL) return ERROR;

	/* Close port
	 */
	 if (ptr->mOpen)
	 {
	     ptr->mOpen = FALSE;
	 }
     return OK;
 }


 word serial_ioctl (void* data, word length, byte control, serial* ptr)
 {
     word result = ERROR;

    /* Check parameter
	 */
     if (ptr == NULL) return result;

    /* Handle command
	 */
     switch(control)
     {
	     case TIMEOUT: ptr->mTimeout = length;
			           result = OK;
			           break;

         case ASYNC:   ptr->mBlocking = 0;
			           result = OK;
                       break;

         case SYNC:    ptr->mBlocking = 1;
			           result = OK;
                       break;

         case SERIAL_BAUD:
         {
           if ( data != NULL ) ptr->mBaud = *( (unsigned long*) (data) );
         }

	 }
     return result;
 }

 word serial_read  (byte* data, word length, serial* ptr)
 {
     word count = 0;
     int  in;

    /* Check parameter
	 */
	 if (ptr == NULL || data == NULL) return 0;

     do
     {
        in = uart_receiveByte();
        if ( in != -1)
		{
			data[count] = in;
			++count;
		}

     }  while ( ptr->mBlocking && count < length);
	 return count;
 }



 word serial_write (byte* data, word length, serial* ptr)
 {
	 word count = 0;

    /* Check parameter
	 */
	 if (ptr == NULL || data == NULL || length >= PROTOCOL_CONTROL) return 0;

 	 for (count = 0; count < length; count++)
	 {
        uart_sendByte(data[count]);
	 }
	 return count;
     
 }

 #endif



