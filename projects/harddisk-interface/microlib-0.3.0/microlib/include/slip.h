#ifndef __SLIP_H__
#define __SLIP_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\slip.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:39:18 $
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
 *  Description:  Header for SLIP protocol wich is defined in RFC1055
 *                This implementation does not support any buffering.
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\include\slip.i $
 *  
 *    MicroLIB - SLIP protocol function headers
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:39:18
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:42:12
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <protocol.h>

/****************************************************************************
 *                 Typedef Definitions
 ****************************************************************************/
 typedef struct _slip 
 {
    /*  function pointers
     */
     word (*open)  (struct _slip* ptr);
     word (*close) (struct _slip* ptr);

     word (*ioctl) (void* data, word length, byte control, struct _slip* ptr);

     word (*read)  (byte* data, word length, struct _slip* ptr);
     word (*write) (byte* data, word length, struct _slip* ptr);

     void (*create) (struct _slip* ptr);
     void (*destroy)(struct _slip* ptr);

    /* private attributes
     */
     port* mPort;
     byte  mRxState;
     byte  mBlocking;
  
 } slip;
                      
                      
/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 
 #define SLIP_END       0300  /* Indicates end of packet */
 #define SLIP_ESC       0333  /* Indicates byte stuffing */
 #define SLIP_ESC_END   0334  /* SLIP_ESC SLIP_ESC_END means END data byte */
 #define SLIP_ESC_ESC   0335  /* SLIP_ESC SLIP_ESC_ESC means ESC data byte */
 
 #define ST_SLIP_DATA   1     /* State for receiving raw data */
 #define ST_SLIP_END    2     /* State for receiving escaped data */


 
 #define slip_initialize(type,instance)      \
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

/****************************************************************************
 *                 Function Prototypes
 ****************************************************************************/
 #ifdef __cplusplus
 extern "C" {
 #endif 
  
 word slip_open  (slip* ptr);
 word slip_close (slip* ptr);

 word slip_ioctl (void* data, word length, byte control, slip* ptr);

 word slip_read  (byte* data, word length, slip* ptr);
 word slip_write (byte* data, word length, slip* ptr);

 void slip_destroy(slip* ptr);
 void slip_create(slip* ptr);


 #ifdef __cplusplus
 }
 #endif

/****************************************************************************/
 

 #endif /* __SLIP_H__ */




