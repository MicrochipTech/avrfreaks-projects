#ifndef __COMMON_H__
#define __COMMON_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\common.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.3 $
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
 *  Description:  Common header for embedded library
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\include\common.i $
 *  
 *    MicroLIB - common macros and type definitions
 *  
 *  Revision 1.3  by: SH  Rev date: 04.05.2002 14:39:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.2  by: SH  Rev date: 04.05.2002 12:38:58
 *    - Included header <string.h> for AVR target processors 
 *    
 *  
 *  Revision 1.1  by: SH  Rev date: 20.09.2000 22:13:52
 *    - Removed version information from common header
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:41:00
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <stdlib.h>
 
 #ifdef __ATMEL__
 #include <string.h>
 #endif
 
 #ifdef __TURBOC__
 #include <malloc.h>
 #endif
                  
 #ifdef WIN32
 #include <windows.h>
 #include <stdio.h>
 #include <conio.h>
 #endif

 #ifdef __WIN32__
 #include <windows.h>
 #include <stdio.h>
 #include <conio.h>
 #endif

 
/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/
 #define FALSE  0
 #define TRUE   1

 #ifndef WIN32
 #define ERROR     0x00
 #endif 

 #define OK        0x01
 #define DATA      0x02
 #define NODATA    0x03
 #define INIT      0x04
 #define ASYNC     0x05
 #define SYNC      0x06
 #define TIMEOUT   0x07
 
 #define INITIALIZE(type, instance)  type##_initialize( type, (instance) )
 
 #define HIGHBYTE(x) ( x / 0x100)
 #define LOWBYTE(x)  ( x & 0xFF)

/****************************************************************************
 *                 Typedef Definitions
 ****************************************************************************/
 typedef unsigned char  byte;
 typedef unsigned short word;


 #endif /* __COMMON_H__ */




