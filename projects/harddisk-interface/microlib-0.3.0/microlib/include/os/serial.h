#ifndef __SERIAL_H__
#define __SERIAL_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\os\serial.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:39:34 $
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
 *  Description:  Serial port declarations
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\include\os\serial.i $
 *  
 *    MicroLIB - serial port function headers
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:39:34
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:42:50
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <protocol.h>
 
 #ifdef WIN32
 #include <windows.h>
 #endif

                                         
/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 #define SERIAL_PORT    0x20
 #define SERIAL_BAUD    0x21
 #define SERIAL_DATA    0x22
 #define SERIAL_STOP    0x23
 #define SERIAL_PARITY  0x24

 #define SERIAL_BUFFER_SIZE_RX 256
 #define SERIAL_BUFFER_SIZE_TX 256
 
/****************************************************************************
 *                 Typedef Definitions
 ****************************************************************************/
 typedef struct _serial
 {
  /*  function pointers
   */
   word (*open)  (struct _serial* ptr);
   word (*close) (struct _serial* ptr);
    
   word (*ioctl) (void* data, word length, byte control, struct _serial* ptr);

   word (*read)  (byte* data, word length, struct _serial* ptr);
   word (*write) (byte* data, word length, struct _serial* ptr);

   void (*create) (struct _serial* ptr);
   void (*destroy)(struct _serial* ptr);
     
  /* attributes
   */
   byte mBlocking;
   byte mPort;
   byte mData;
   byte mStop;
   byte mParity;
   word mBaud;
   word mTimeout;
   byte mOpen;

   #ifdef WIN32
   HANDLE mHandle;
   #endif

 } serial;

 #define serial_initialize(type,instance)    \
 {                                           \
    instance.open       = type##_open;       \
    instance.close      = type##_close;      \
    instance.ioctl      = type##_ioctl;      \
    instance.read       = type##_read;       \
    instance.write      = type##_write;      \
    instance.destroy    = type##_destroy;    \
    instance.create     = type##_create;     \
                                             \
    instance.create(&instance);              \
 }

 #define serial_new_object(type, instance)   \
 {                                           \
    instance = malloc( sizeof(type));        \
                                             \
    if (instance != NULL)                    \
	{                                        \
       (*instance).open    = type##_open;    \
       (*instance).close   = type##_close;   \
       (*instance).ioctl   = type##_ioctl;   \
       (*instance).read    = type##_read;    \
       (*instance).write   = type##_write;   \
       (*instance).destroy = type##_destroy; \
       (*instance).create  = type##_create;  \
                                             \
       instance->create(instance);           \
	}						                 \
 }

 #define serial_delete_object(instance)      \
 {                                           \
    if ( instance != NULL)                   \
    {                                        \
       instance->destroy(instance);          \
       free(instance);                       \
    }                                        \
    instance = NULL;                         \
 }


/****************************************************************************
 *                 Function Prototypes
 ****************************************************************************/
 #ifdef __cplusplus
 extern "C" {
 #endif 
  
 word serial_open  (serial* ptr);
 word serial_close (serial* ptr);
  
 word serial_ioctl (void* data, word length, byte control, serial* ptr);
 
 word serial_read  (byte* data, word length, serial* ptr);
 word serial_write (byte* data, word length, serial* ptr);

 void serial_destroy(serial* ptr);
 void serial_create(serial* ptr);

 #ifdef __cplusplus
 }
 #endif

/****************************************************************************/


 #endif /* __SERIAL_H__ */




