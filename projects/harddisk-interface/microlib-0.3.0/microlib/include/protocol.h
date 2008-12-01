#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\protocol.h $
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
 *  Description:  Base declarations for protocol implementations
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\include\protocol.i $
 *  
 *    MicroLIB - abstract protocol and port declaration
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:39:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:41:44
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <common.h>
                  
/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 
 #define PROTOCOL_PORT     0xFF
 
 #define PROTOCOL_CONTROL  0xFF00
 #define PROTOCOL_OK       0xFF01
 #define PROTOCOL_ERROR    0xFF02
 #define PROTOCOL_TIMEOUT  0xFF04
 #define PROTOCOL_OVERFLOW 0xFF08
 
 #define PROTOCOL_BEGIN    0xFF10
 #define PROTOCOL_END      0xFF11
 
  
/****************************************************************************
 *  Class port: declaration and definitions
 ****************************************************************************/
 typedef struct _port
 {
   /*  function pointers
    */
    word (*open)  (struct _port* ptr);
    word (*close) (struct _port* ptr);
    
    word (*ioctl) (void*  data, word length, byte control, struct _port* ptr);
    
    word (*read)  (byte*  data, word length, struct _port* ptr);
    word (*write) (byte*  data, word length, struct _port* ptr);

	void (*create)(struct _port* ptr);
  	void (*destroy)(struct _port* ptr);

   /*  attributes
    */
    struct _port* port;
 
 } port;


 #define port_initialize(type,instance)      \
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
 *                 ASCII control code definitions
 ****************************************************************************/
 enum ASCII {

    Null = 0,   /* 00  00  Null                      */
    SOH,        /* 01  01  Start of Heading          */
    STX,        /* 02  02  Start of Text             */
    ETX,        /* 03  03  End of Text               */ 
    EOT,        /* 04  04  End of Transmission       */
    ENQ,        /* 05  05  Enquiry                   */
    ACK,        /* 06  06  Acknowledge               */
    BEL,        /* 07  07  Bell                      */
    BS,         /* 08  08  Backspace                 */
    HT,         /* 09  09  Horizontal Tabulation     */
    LF,         /* 10  0A  Line Feed                 */
    VT,         /* 11  0B  Vertical Tabulation       */
    FF,         /* 12  0C  Form Feed                 */
    CR,         /* 13  0D  Carriage Return           */
    SO,         /* 14  0E  Shift out                 */
    SI,         /* 15  0F  Shift in                  */
    DLE,        /* 16  10  Data link escape          */
    XON,        /* 17  11  XON                       */
    DC1 = 17,   /* 17  11  Device Control            */
    DC2,        /* 18  12  Device Control            */
    XOFF,       /* 19  13  XOFF                      */
    DC3 = 19,   /* 19  13  Device Control            */
    DC4,        /* 20  14  Device Control            */
    NAK,        /* 21  15  Negative Acknowledge      */
    SYN,        /* 22  16  Synchronous Idle          */
    ETB,        /* 23  17  End of Transmission Block */
    CAN,        /* 24  18  Cancel                    */
    EM,         /* 25  19  End of Medium             */
    SUB,        /* 26  1A  Substitute Character      */
    ESC,        /* 27  1B  Escape                    */
    FS,         /* 28  1C  File Separator            */
    GS,         /* 29  1D  Group Separator           */
    RS,         /* 30  1E  Record Separator          */
    US          /* 31  1F  Unit Separator            */
 };

#endif /* __PROTOCOL_H__ */




