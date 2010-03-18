/**************************************************************************************
***************************************************************************************
***
***     File: main.cpp
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: AVRBoot main. Put the AVR hex file to be flashed in the directory
***		where avbr.exe is located. To start the flash download enter 'avrb file.hex' at
***     the command line. file.hex is the name of the hex file to download to the target.
***		Version: 1.0.0
***
***		Change log:
***		2010-03-01: 1.0.0 First release
***
***************************************************************************************
***************************************************************************************
***
***		This file is part of AVRBoot.
***
***		AVRBoot is free software: you can redistribute it and/or modify
***     it under the terms of the GNU General Public License as published by
***     the Free Software Foundation, either version 3 of the License, or
***     (at your option) any later version.
***
***     AVRBoot is distributed in the hope that it will be useful,
***     but WITHOUT ANY WARRANTY; without even the implied warranty of
***     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
***     GNU General Public License for more details.
***
***     You should have received a copy of the GNU General Public License
***     along with AVRBoot.  If not, see <http://www.gnu.org/licenses/>.
***
***
***************************************************************************************
***************************************************************************************
*/


#include <cstdlib>
#include <iostream>
#include "defines.h"
#include "avrProg.h"

using namespace std;


int main(int argc, char *argv[])
{
	if ( avrProg_Program( argv[1] ) == SUCCESS ) {
       printf("Programming succeeded\n");
    }
    else {
         printf("Programming Error!\n");
    }
    
    return EXIT_SUCCESS;
}
