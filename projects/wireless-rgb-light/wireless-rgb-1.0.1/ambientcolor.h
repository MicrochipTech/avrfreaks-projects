#ifndef _AMBIENT_COLOR_H_
#define _AMBIENT_COLOR_H_

/******************************************************************************
 *
 * ID      : $Id: ambientcolor.h 26 2010-11-21 12:47:24Z SH $ 
 * Revsion : $Rev: 26 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 13:47:24 +0100 (So, 21 Nov 2010) $   
 *
 ******************************************************************************
 *
 * License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
 #include "net.h"


/******************************************************************************
 * Function prototypes
 ******************************************************************************/
 int IsAmbientColorMessage( unsigned long address, unsigned int port, char* msg );
 void HandleAmbientColorMessage( char* msg );


#endif
