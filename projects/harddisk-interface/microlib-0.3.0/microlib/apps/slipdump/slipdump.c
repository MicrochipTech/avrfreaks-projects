/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\apps\slipdump\slipdump.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:36:56 $
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
 *  Description:  SLIPDUMP: Testprogram - dumps all received slip frames
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\apps\slipdump\slipdump.d $
 *  
 *    Testprogram for SLIP implementation
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:36:56
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:58:12
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <common.h>
 #include <protocol.h>
 #include <os/serial.h>
 #include <slip.h>
 #include <ide.h>
 #include <ata.h>

/****************************************************************************
 *                Macro Definitions
 ****************************************************************************/
 #define BUFFERSIZE 8
 
 #define  ST_IDLE   1
 #define  ST_BEGIN  2
 #define  ST_DATA   3
 #define  ST_END    4


 #define MSG_HELLO  "SLIPDUMP - Test program for the slip protocol\n\r"
 #define MSG_START  "\n\rNEW FRAME\n\r---------------------\n\r"
 #define MSG_END    "\n\r---------------------\n\rEND OF FRAME\n\r"
 #define MSG_ERROR  "\n\rFRAME ERROR\n\r---------------------\n\r"
 

/****************************************************************************
 *
 * Function Name: main( int argc, char* argv[] )
 *
 * Description:   Main function
 *
 * Parameters:    int argc, char* argv[]
 *
 * Return:        int
 *
 ***************************************************************************/
 int main( int argc, char* argv[] )
 {
   /* Variables for objects creating the protocol stack
    */
    serial Port;           /* serial port                    */
    slip   Protocol;       /* framing protocol layer:  SLIP  */

   /* Variables
    */
    byte   buffer;
    word   length;
    byte   state    = ST_IDLE;
    byte   portno   = 1;

    unsigned long   baudrate = 19200L;

  
   /* Setup and initialize the protocol stack
    */
    INITIALIZE(serial, Port);

    Port.ioctl(&portno,   sizeof(portno),   SERIAL_PORT, &Port);
    Port.ioctl(&baudrate, sizeof(baudrate), SERIAL_BAUD, &Port);
    Port.ioctl(NULL,0,ASYNC,&Port);
    Port.open(&Port);

    INITIALIZE(slip, Protocol);
    Protocol.ioctl(&Port, sizeof(Port), PROTOCOL_PORT, &Protocol);
    Port.ioctl(NULL,0,ASYNC,&Port);
    Protocol.open(&Protocol);


    Protocol.write(MSG_HELLO, sizeof(MSG_HELLO), &Protocol);

   /* Main loop running a state machine
    */
    for (;;)
    {

      /* Read the protocol and loop through the received bytes
       */
       length = Protocol.read( &buffer, 1, &Protocol);

      /* Check for a new frame that resets state machine
       */
       if ( length == PROTOCOL_BEGIN) state = ST_IDLE;

      /* Handle the data according to the current state
       */
       if (length > 0) switch(state)
       {
             /* -------------------------------------------------------------
              *  State IDLE
              * -------------------------------------------------------------
              */
              case ST_IDLE:
              {
                 /* Wait for a new frame
                  */
                  if (length == PROTOCOL_BEGIN)
                  {
                      state = ST_DATA;
                      Protocol.write(0,ST_BEGIN,&Protocol);
                      Protocol.write(MSG_START, sizeof(MSG_START), &Protocol);
                  }
              }
              break;

             /* -------------------------------------------------------------
              *  State DATA
              * -------------------------------------------------------------
              */
              case ST_DATA:
              {

                 /* Check the control
                  */
                  switch(length)
                  {
                     case PROTOCOL_END:
                     {
                        state = ST_IDLE;
                        Protocol.write(MSG_END, sizeof(MSG_END), &Protocol);
                        Protocol.write(0,ST_END,&Protocol);
                        break;
                     }

                     case PROTOCOL_ERROR:
                     {
                        state = ST_IDLE;
                        Protocol.write(MSG_ERROR, sizeof(MSG_ERROR), &Protocol);
                        Protocol.write(0,ST_END,&Protocol);
                     }
                     break;

                     default:
                     {
                        Protocol.write(&buffer,1,&Protocol);
                     }
                     break;

                  } /* switch(control) */
              }
              break;


             /* -------------------------------------------------------------
              *  Illegal state
              * -------------------------------------------------------------
              */
              default:
              {
                 /* Illegal state, set to IDLE
                  */
                  state = ST_IDLE;
              }
              break;

          }  /* switch(state) */


    } /* for(;;) */

 } /* main */



