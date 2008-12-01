/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\ata.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.4 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:42:24 $
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
 *  Description:  Hardware independend IDE/ATA function
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\ata.d $
 *  
 *    MicroLIB - ata functions ( not tested )
 *  
 *  Revision 1.4  by: SH  Rev date: 04.05.2002 14:42:24
 *    Updated copyright to LGPL
 *  
 *  Revision 1.3  by: SH  Rev date: 04.05.2002 12:42:14
 *    - Bugfixes in function 'readSector()' 
 *    - Bugfixes in function 'writeSector()' 
 *    - Bugfixes in function 'hextobyte()' 
 *    - Bugfixes in function 'convertParameters()' 
 *    
 *  
 *  Revision 1.2  by: SH  Rev date: 20.09.2000 22:34:02
 *    - Updated to version 0.2.0
 *  
 *  Revision 1.1  by: SH  Rev date: 20.09.2000 22:16:16
 *    - Reduced memory usage
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:54:38
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
 #include <ide.h>
 #include <ata.h>
 
/****************************************************************************
 *  Static variables
 ****************************************************************************/
 byte  drive;
 byte  head;
 byte  sector;
 word  cylinder;

 byte  counter;                  
 byte  block;
 byte  buffer[8];

 port* Port = NULL;


/****************************************************************************
 *
 * Function Name: hextobyte( byte hex )
 *
 * Description:   convert a hex digit '0'..'9','A'..'F','a'..'f' into an
 *                integer value
 *
 * Parameters:    byte
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte hextobyte( byte hex)
 {
    if ( hex >= '0' && hex <= '9' ) return hex - '0';
    if ( hex >= 'A' && hex <= 'F' ) return 10 + ( hex - 'A' );
    if ( hex >= 'a' && hex <= 'f' ) return 10 + ( hex - 'a');

   /* Invalid hex value
    */
    return 0;

 } /* hex2byte */


/****************************************************************************
 *
 * Function Name: convertParameters( void )
 *
 * Description:   
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void convertParameters( void )
 {
   if ( hextobyte(buffer[0]) == '1') 
        drive = 1;
   else drive = 0;

   head     = hextobyte(buffer[1]);

   sector   = hextobyte(buffer[2]) *   0x10 
            + hextobyte(buffer[3]) ;

   cylinder = hextobyte(buffer[4]) * 0x1000 
            + hextobyte(buffer[5]) *  0x100
            + hextobyte(buffer[6]) *   0x10
            + hextobyte(buffer[7]) ;

 } /* convertParameters */




/****************************************************************************
 *
 * Function Name: tobyte( byte* buffer )
 *
 * Description:   convert two hex digits into a byte value
 *
 * Parameters:    byte*
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte tobyte( byte* digits)
 {
    byte result = 0;
	if (digits == NULL) return result;
	
	result  = hextobyte(digits[0]) * 0x10;
	result += hextobyte(digits[1]);
	
	return result;
 }

/****************************************************************************
 *
 * Function Name: toword( byte* buffer )
 *
 * Description:   convert four hex digits into a word value
 *
 * Parameters:    byte*
 *
 * Return:        word
 *
 ***************************************************************************/
 word toword( byte* buffer)
 {
    word result = 0;
    if (buffer == NULL) return result;

	result  = hextobyte(buffer[0]) * 0x1000;
	result += hextobyte(buffer[1]) * 0x100;
    result += hextobyte(buffer[2]) * 0x10;
    result += hextobyte(buffer[3]);
	
	return result;
 }

/****************************************************************************
 *
 * Function Name: ata_command( byte cmd )
 *
 * Description:   Get the command handler
 *
 * Parameters:    byte command
 *
 * Return:        ata_function
 *
 ***************************************************************************/
 ata_function ata_command( byte cmd)
 {
    ata_function handler = NULL; 

    switch (cmd)
    {
        case 'V' : /* 'Get software version'
                    */
                    handler = getVersion;
                    break;

        case 'I' : /* 'Initialize hardware'
                    */
                    handler = initialiseHardware;
                    break;

        case 'w' : /* 'Write IDE register'
                    */
                    handler = writeRegister;
                    break;

        case 'r' : /* 'Read IDE register'
                    */
                    handler = readRegister;

        case 'W' : /* Write sectors (WRITE SECTORS, 0x30);
                    */
                    handler = writeSector;
                    break;

        case 'R' : /* Read sectors (READ SECTORS, 0x20)
                    */
                    handler = readSector;
                    break;
        
        case 'S' : /* Status
                    */
                    handler = getStatus;
                    break;

        case 'E' : /* Error
                    */
                    handler = getError;
                    break;

        case 'D' : /* Diagnostics
                    */
                    handler = runDiagnostics;
                    break;

        case 'C' : /* Get configuration sector
                    */
                    handler = identifyDrive;
                    break;

        case 'Z' : /* Sleep mode
                    */
                    handler = sleepDrive;
                    break;

        case 'N' : /* Normal operating mode
                    */
                    handler = wakeupDrive;
                    break;

        default :  /* Invalid command,
                    */
                    #if 0
                    handler = dump;
                    #endif
                    break;

    } /* switch (cmd) */

    return handler;

 } /* command */

/****************************************************************************
 *
 * Function Name: ata_init()
 *
 * Description:   Initialize hardware, setup callback port
 *
 * Parameters:    port* io - Pointer to port for callbacks
 *
 * Return:        void
 *
 ***************************************************************************/
 void ata_init( port* io )
 {
    /* Remember the port
     */
     Port = io;

    /* Initialize hardware
     */
     ide_ioctl(NULL, 0, IDE_IOCTL_RESET);

    /* Send software version 
     */
     getVersion(OK,0);

 }

/****************************************************************************
 *
 * Function Name: getVersion()
 *
 * Description:   Return the build version
 *
 * Parameters:    byte control, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void getVersion( byte control, byte data )
 {
	if (Port != NULL && control == OK)
    {
       byte* info    = INFO;
       byte* version = VERSION;
       buffer[0] = ATA_OK;

       Port->write(0,PROTOCOL_BEGIN, Port);
       Port->write(buffer, 1, Port);
       Port->write(info, (word) strlen(info), Port);
       Port->write(version, (word)strlen(version), Port);
       Port->write(0,PROTOCOL_END, Port);
    }
 }

/****************************************************************************
 *
 * Function Name: initialiseHardware()
 *
 * Description:   initialise Hardware
 *
 * Parameters:    byte control, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void initialiseHardware( byte control, byte data )
 {
    ide_hardreset();
    if (Port != NULL && control == OK)
    {
       Port->write(0, PROTOCOL_BEGIN, Port);
       buffer[0] = ATA_OK;
       Port->write(buffer, 1, Port);
	   Port->write(0, PROTOCOL_END, Port);
    }

 } /* initialiseHardware */


/****************************************************************************
 *
 * Function Name: getStatus()
 *
 * Description:   Get IDE status
 *
 * Parameters:    byte control, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void getStatus( byte control, byte data )
 {
    buffer[0]  = ATA_OK;
    buffer[1]  = ide_status();
    
    if (Port != NULL && control == OK)
    {
       Port->write(0,PROTOCOL_BEGIN, Port);
       Port->write(buffer, 2, Port);
       Port->write(0,PROTOCOL_END, Port);
    }

 } /* getStatus */


/****************************************************************************
 *
 * Function Name: getError()
 *
 * Description:   Get IDE error staus
 *
 * Parameters:    byte control, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void getError( byte control, byte data )
 {
    buffer[0] = ATA_OK;
    buffer[1] = ide_error();
    
    if (Port != NULL && control == OK)
    {
       Port->write(0, PROTOCOL_BEGIN, Port);
       Port->write(buffer, 2, Port);
       Port->write(0, PROTOCOL_END, Port);
    }


 } /* getError */


/****************************************************************************
 *
 * Function Name: sendError()
 *
 * Description:   Send IDE error with 'count' fill bytes
 *
 * Parameters:    byte control, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void sendError( int count)
 {
    int index;
	buffer[0] = ATA_ERROR;
    buffer[1] = 0;

    if (Port != NULL)
    {
       Port->write(0, PROTOCOL_BEGIN, Port);
	   Port->write(buffer,1,Port);	
	    
       for (index = 0; index < count; index++)
	   {
	      Port->write(buffer+1, 1, Port);   
	   }
       Port->write(0, PROTOCOL_END, Port);
    }

 } /* sendError */


/****************************************************************************
 *
 * Function Name: runDiagnostics()
 *
 * Description:   Execute drive diagnostics
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void runDiagnostics( byte control, byte data )
 {
     switch(control)
     {
         case OK:    /* Device diagnostics
                      */
                      ide_outbyte(0,6,0x00);    
                      ide_outbyte(0,7,0x90);
                      
                     /* Wait until drive ready
                      *
                      * TODO: Implement timeout here !!!
                      */
                      while( ide_inbyte(0,7) & 0x80) ide_inbyte(1,7);
                      getStatus(OK,0);
                      break;

         case ERROR: /* Send error
                      */ 
                      sendError(1);         
                      break;
     }

 } /* ata_diagnostics */




/****************************************************************************
 *
 * Function Name: identifyDrive()
 *
 * Description:   Identify drive
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void identifyDrive( byte control, byte data )
 {
    byte temp;
	word in;
    word i;

    switch(control)
    {
        case INIT:   counter = 0;
                     drive   = 0;
                     break;

        case DATA:   if( counter == 0 && data == '1') drive = 1;
                     counter=1;
                     break;

        case OK:    /* Identify drive: ata command 0xEC
                     */
                     if (Port != NULL)
					 {
                         if ( drive == 0)
						 	  ide_outbyte(0,6,0x00);
                         else ide_outbyte(0,6,0x10);

						 ide_outbyte(0,7,0xEC);

                        /* Wait until drive ready
                         *
                         * TODO: Implement timeout!!!
                         *
                         */
                         while( ide_inbyte(0,7) & 0x80);

                        /* Get data
                         */
						 Port->write(0, PROTOCOL_BEGIN, Port);
                         temp = ATA_OK;
                         Port->write(&temp, 1, Port); 
						 
						 for (i = 0; i < 256; i++)
						 {
                           /* Read the data
                            */
                            in = ide_read_data();

                           /* Return the data
                            */
		  			        temp = in / 0x100;
                            Port->write(&temp, 1, Port);
                            temp = in & 0xff;
							Port->write(&temp, 1, Port);
						 }
						 Port->write(0, PROTOCOL_END, Port);
					 }
                     break;

        case ERROR: /* Send error
                     */
                     sendError(512);
                     break;
    }

 } /* identifyDrive */


/****************************************************************************
 *
 * Function Name: sleepDrive()
 *
 * Description:   Switch off motor
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void sleepDrive( byte control, byte data )
 {
     byte temp;

     switch (control)
	 {
        case INIT:   counter = 0;
                     drive   = 0;
                     break;

        case DATA:   if( counter == 0 && data == '1') drive = 1;
                     counter=1;
                     break;
	 
	    case OK:
		{
          /* Spin down drive immediately
           */ 
           if ( drive == 0)
 		 	    ide_outbyte(0,6,0x00);
           else ide_outbyte(0,6,0x10);

		   ide_outbyte(0,7,0xE0);       /* Command 'STANDBY IMMEDIATE (0xE0)  */
           
           if ( Port != NULL)
           {
  		       temp = ATA_OK;
			   
			   Port->write(0, PROTOCOL_BEGIN, Port);
               Port->write(&temp, 1, Port);
               Port->write(0, PROTOCOL_END, Port);
           }
        }
        break;
	 
	 }
 
 } /* sleepDrive */
 
/****************************************************************************
 *
 * Function Name: wakeupDrive()
 *
 * Description:   Switch on motor
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void wakeupDrive( byte control, byte data )
 {   
     byte temp;

     switch (control)
	 {
        case INIT:   counter = 0;
                     drive   = 0;
                     break;

        case DATA:   if( counter == 0 && data == '1') drive = 1;
                     counter=1;
                     break;
	 
	    case OK:
		{
          /* Spin up drive immediately
           */ 
           if ( drive == 0)
 		 	    ide_outbyte(0,6,0x00);
           else ide_outbyte(0,6,0x10);

           ide_outbyte(0,7,0xE1);       /* ATA command IDLE IMMEDIATE (0xE1)  */
           
           if ( Port != NULL)
           {
  		       temp = ATA_OK;
			   
			   Port->write(0, PROTOCOL_BEGIN, Port);
               Port->write(&temp, 1, Port);
               Port->write(0, PROTOCOL_END, Port);
           }
        }
        break;
	 
	 }

 } /* wakeupDrive */


/****************************************************************************
 *
 * Function Name: readSector()
 *
 * Description:   Read a sector
 * 
 *                Parameter format (number of data byte):
 *
 *                Byte 0:    '0'  .. '1'   Drive 
 *                Byte 1:    '0'  .. 'F'   Head
 *                Byte 2-3:  '00' .. 'FF'  Sector
 *                Byte 4-5:  '00' .. 'FF'  Cylinder high byte
 *                Byte 6-7:  '00' .. 'FF'  Cylinder low byte
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void readSector( byte control, byte data )
 {
     word in;
	 byte temp;
     word i;

     switch(control)
     {
         case INIT:   counter  = 0;
                      drive    = 0;
                      sector   = 0;
                      head     = 0;
                      cylinder = 0;
                      break;

         case DATA:   if (counter < 8) buffer[counter] = data;
                      ++counter;
                      break;

         case OK:     if (Port != NULL)
					  {
		               	 
                        /* Convert parameters from ascii to hexadecimal
                         */
                         convertParameters();

                        /* Read a single sector: ata command 0x21
                         */
                         if (drive == 1)
                              ide_outbyte(0,6,head | 0x10);    /* Drive/head register, drive 1 */
                         else ide_outbyte(0,6,head);           /* Drive/head register, drive 0 */
                         
                         ide_outbyte(0,2,0x01);                /* Sector Count reg.            */
                         ide_outbyte(0,3,sector+1);            /* Sector Number reg.           */
                         ide_outbyte(0,4,LOWBYTE(cylinder));   /* Cylinder low byte register   */
                         ide_outbyte(0,5,HIGHBYTE(cylinder));  /* Cylinder low byte register   */
                         ide_outbyte(0,7,0x21);                /* ATA command 0x21             */


                        /* Wait until drive ready
                         *
                         * TODO: timeout !!!
                         *
                         */     
                         while ( !( ide_inbyte(0,7) & IDE_STATUS_DRQ) );

                        /* Get data
                         */
                         Port->write(0, PROTOCOL_BEGIN, Port);
                         
						 temp = ATA_OK;
                         Port->write(&temp, 1, Port); 
						 
						 for (i = 0; i < 256; i++) 
                         {
                           /* Read the data
                            */
                            in = ide_read_data();
                            
                           /* Return the data
                            */
                            temp = in / 0x100;
                            Port->write(&temp, 1, Port);
                            temp = in & 0xff;
                            Port->write(&temp, 1, Port);
                         }
                         Port->write(0, PROTOCOL_END, Port); 
                      }
                      break;

         case ERROR: /* Send error status
                      */ 
                      sendError(512);
                      break;
     }

 } /* readSector */

/****************************************************************************
 *
 * Function Name: writeSector()
 *
 * Description:   Write a single sector
 *
 *                Byte 0:    '0'  .. '1'   Drive 
 *                Byte 1:    '0'  .. 'F'   Head
 *                Byte 2-3:  '00' .. 'FF'  Sector
 *                Byte 4-5:  '00' .. 'FF'  Cylinder high byte
 *                Byte 6-7:  '00' .. 'FF'  Cylinder low byte
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void writeSector( byte control, byte data )
 {
     byte i    = 0;
     word temp = 0;

     switch(control)
     {
         case INIT:   counter   = 0;
                      drive     = 0;
                      sector    = 0;
                      head      = 0;
                      cylinder  = 0;
                      block     = 0;
                      break;

         case DATA:   buffer[counter] = data;
                      ++ counter;
                      
                      if ( counter > 7) 
                      {
                        if ( block == 0)
                        {
                          /* Convert parameters from ascii to hexadecimal
                           */
                           convertParameters();
                          
                          /* Start to write a single sector: ata command 0x30
                           */
                           if (drive == 1)
                                ide_outbyte(0,6,head | 0x10);    /* Drive/head register, drive 1 */
                           else ide_outbyte(0,6,head);           /* Drive/head register, drive 0 */

                           ide_outbyte(0,2,0x01);                /* Sector Count reg.            */
                           ide_outbyte(0,3,sector+1);            /* Sector Number reg.           */
                           ide_outbyte(0,4,LOWBYTE(cylinder));   /* Cylinder low byte register   */
                           ide_outbyte(0,5,HIGHBYTE(cylinder));  /* Cylinder high byte register  */
                           ide_outbyte(0,7,0x30);                /* ATA command 0x30             */

                          /* Wait until drive ready
                           *
						   * TODO: timeout
						   *						   
						   */
                           while( ide_inbyte(0,7) & 0x80 ) ide_inbyte(1,7);
                        
                        }
                        else if (block < 64)
                        {
                          /* Write a block of bytes
                           */
                           for (i=0; i< 4; i++)
                           {
                             temp  = 0x100 * buffer[2*i];
                             temp += buffer[1 + 2*i];
                             ide_outword(0,0,temp);
                           }
                        }
                        ++block;
                        counter = 0;
                      }
                      break;

         case OK:    /* OK, write the last block of bytes
                      */
                      for (i=0; i< 4; i++)
                      {
                         temp  = 0x100 * buffer[2*i];
                         temp += buffer[1 + 2*i];
                         ide_outword(0,0,temp);
                      }
                      while (ide_inbyte(0,7) & IDE_STATUS_BSY);

                      if (Port != NULL)
                      { 
                         i = ATA_OK;
                         
                         Port->write(NULL, PROTOCOL_BEGIN, Port);
                         Port->write(&i, 1, Port);
                         Port->write(NULL, PROTOCOL_END, Port);
                      }
                      break;

         case ERROR: /* Error, execute software reset
                      */ 
                      ide_softreset();
                      sendError(0);
                      break;
     }

 }  /* write Sector */


/****************************************************************************
 *
 * Function Name: writeRegister()
 *
 * Description:   Write a register
 *
 *                Byte 0:       '0' .. 'B'     Register
 *
 *                Byte 1-2:    '00' ..   'FF'  8 bit data 
 *           or   Byte 1-4:  '0000' .. 'FFFF'  16 bit data 
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void writeRegister( byte control, byte data )
 {
     byte addr;
     byte error = ATA_ERROR;

     switch(control)
     {
          case INIT:   counter  = 0;
                       break;

          case DATA:   if (counter < 5) buffer[counter] = data;
                       ++counter;
                       break;

          case OK:    /*  Evaluate parameters
                       */
                       addr = hextobyte(buffer[0]);
			           if ( counter == 3  && addr != DATA_REGISTER)
					   {
						   ide_write(addr, tobyte(buffer+1));
						   error = ATA_OK;
					   }
			           if ( counter == 5  && addr == DATA_REGISTER)
					   {
						   ide_write_data(toword(buffer+1));
						   error = ATA_OK;
					   }
					   Port->write(0, PROTOCOL_BEGIN, Port);
                       Port->write(&error, 1, Port); 
   		               Port->write(0, PROTOCOL_END, Port);
                       break;

          case ERROR: /* Send error status
                       */ 
                       sendError(0);
                       break;
      }
 }

/****************************************************************************
 *
 * Function Name: readRegister()
 *
 * Description:   Read a register
 *
 *                Byte 0:    '0'    .. 'F'     Register
 *
 * Parameters:    byte data, byte control
 *
 * Return:        void
 *
 ***************************************************************************/
 void readRegister( byte control, byte data )
 {
     byte addr;
	 byte temp;
	 word in16;
     byte error;

     switch(control)
     {
          case INIT:   counter  = 0;
                       break;

          case DATA:   if (counter < 1) buffer[counter] = data;
                       ++counter;
                       break;

          case OK:    /*  Evaluate parameters
                       */
                       if ( counter == 1 )
					   {
                          /* Get the register
						   */
                           addr = hextobyte(buffer[0]);
						   if (addr == DATA_REGISTER)
						   {
							   error = ATA_OK;

                              /* Read data register (16bit)
							   */
							   in16 = ide_read_data();
							   
							   Port->write(0, PROTOCOL_BEGIN, Port);
					           Port->write(&error, 1, Port); 
							   temp = in16 >> 8;
							   Port->write(&temp, 1, Port);
				               temp = in16 & 0xFF;
                               Port->write(&temp, 1, Port);
							   Port->write(0, PROTOCOL_END, Port);
 						   }
						   else
						   {
                               if (addr > ADDRESS_REGISTER)
							   {
                                  sendError(2);
							   }
							   else
							   {
                                 /* Read 8bit register
							      */
							      error = ATA_OK;

							      Port->write(0, PROTOCOL_BEGIN, Port);
					              Port->write(&error, 1, Port); 
							      temp = 0;
                                  Port->write(&temp, 1, Port);
								  temp  = ide_read(addr);
								  Port->write(&temp, 1, Port);
				                  Port->write(0, PROTOCOL_END, Port);
							   }
						   }
					   }
                       break;

          case ERROR: /* Send error status
                       */ 
                       sendError(2);         
                       break;
      }
 }

