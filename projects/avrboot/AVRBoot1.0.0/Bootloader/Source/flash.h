/**************************************************************************************
***************************************************************************************
***
***     File: flash.h
***     Project: AVRBoot
***     Copyright 2010 Peter Eckstrand
***     Description: Definitions of flash helper macros
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
#ifndef __FLASH_H
#define __FLASH_H

#define R0_REG  *( (volatile uint8_t *)0x00 )
#define R1_REG  *( (volatile uint8_t *)0x01 )



#define LOAD_Z()    do {\
                        asm ("mov r31,r3");\
                        asm ("mov r30,r4");\
                    } while ( 0 )
                    

#define LOAD_SPMCR(x)   do {\
                            asm ("ldi r26,0x57");\
                            asm ("ldi r27,0x00");\
                            asm ("ldi r24,"#x);\
                            asm ("st x,r24");\
                        } while ( 0 )


#define SPM() asm ("spm")


#define FLASH_ERASE_PAGE()  do {\
                                LOAD_Z();\
                                LOAD_SPMCR( 0x03 );\
                                SPM();\
                                while( SPMCR & 0x02 );\
                            } while ( 0 )

#define FLASH_REENABLE_RWW()    do {\
                                    LOAD_SPMCR( 0x11 );\
                                    SPM();\
                                    while( SPMCR & 0x01 );\
                                } while ( 0 )

#define FLASH_LOAD_TEMP_BUF( x )    do {\
                                        R1_REG = ( ( x & 0xff00 ) >> 8 );\
                                        R0_REG = ((uint8_t)(x));\
                                        LOAD_Z();\
                                        LOAD_SPMCR( 0x01 );\
                                        SPM();\
                                        while( SPMCR & 0x01 );\
                                    } while ( 0 )

#define FLASH_WRITE_PAGE()  do {\
                                LOAD_Z();\
                                LOAD_SPMCR( 0x05 );\
                                SPM();\
                                while( SPMCR & 0x01 );\
                            } while ( 0 )


#define BOOT_VECT() do {\
                    asm ("ldi R30,0x5B");\
                    asm ("ldi R31,0x00");\
                    asm ("ldi R24,0x01");\
                    asm ("ldi R25,0x02");\
                    asm ("STD Z+0,R24");\
                    asm ("STD Z+0,R25");\
                    } while (0)

#define APPL_VECT() do {\
                    asm ("ldi R30,0x5B");\
                    asm ("ldi R31,0x00");\
                    asm ("ldi R24,0x01");\
                    asm ("ldi R25,0x00");\
                    asm ("STD Z+0,R24");\
                    asm ("STD Z+0,R25");\
                    } while (0)

#endif
