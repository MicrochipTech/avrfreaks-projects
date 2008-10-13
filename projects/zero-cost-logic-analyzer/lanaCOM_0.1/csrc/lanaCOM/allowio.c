/*
 * lanaCOM - 4 bit logic analyzer using COM-port interface
 * Copyright (c) 2008, Dipl.-Ing. Hans-Juergen Heinrichs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <stdio.h>
#include <sys/io.h>

#define debug( ... )   ({ if(verbose >= 3) {printf( "Debug: " __VA_ARGS__ ); (void)fflush(stdout);} })
#define debug0( ... )  ({ if(verbose >= 3) {printf( __VA_ARGS__ ); (void)fflush(stdout);} })


/* -------------------------------------------------------------------------------- */
/*  Function name:      g i v e i o ()                                              */
/*                                                                                  */
/*  Returns:            0 - could not open giveio handle                            */
/*  Parameters:         -                                                           */
/*  Description:        opens the GIVEIO file handle                                */
/*                      taken from the 'giveio' distribution                        */
/* -------------------------------------------------------------------------------- */
#ifdef __CYGWIN__

#include <windows.h>

int giveio(void)
{
   OSVERSIONINFO osvi;
   int ret = 1;

   osvi.dwOSVersionInfoSize = sizeof osvi;
   GetVersionEx(&osvi);
   if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
   {
      HANDLE h;

      h = CreateFile("\\\\.\\giveio", GENERIC_READ, 0, NULL,
                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (h == INVALID_HANDLE_VALUE)
         ret = 0;
      else
         CloseHandle(h);
   }
   return ret;
}
#else

int
giveio(void)
{
    return 0;
}

#endif

/* -------------------------------------------------------------------------------- */
/*  Function name:      a l l o w I O ()                                            */
/*                                                                                  */
/*  Returns:            0 - I/O permission could be set                             */
/*  Parameters:         base address of I/O area                                    */
/*  Description:        calls iopl to set I/O permission to: allow                  */
/* -------------------------------------------------------------------------------- */
int
allowIO( int verbose )
{
    /* If 'giveio' first.   */
    debug( "Trying 'giveio.sys'..." );
    if( giveio() != 0 )  {
        debug0( "OK\n" );
        return 0;
    } else
        debug0( "Fail\n" );

    /* Try 'ioperm' second. */
    debug( "Trying 'ioperm.sys'..." );
    if( iopl(3) >= 0 )  {
        debug0( "OK\n" );
        return 0;
    } else
        debug0( "fail\n" );

    return -1;
}

