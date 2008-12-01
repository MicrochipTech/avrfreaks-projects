/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\apps\monitor\ui.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.3 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:35:46 $
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
 *  Description:  User interface for ATMEL harddisc controller
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\apps\monitor\ui.d $
 *  
 *    Simple IDE Monitor - user interface
 *  
 *  Revision 1.3  by: SH  Rev date: 04.05.2002 14:35:46
 *    Updated copyright to LGPL
 *  
 *  Revision 1.2  by: SH  Rev date: 04.05.2002 12:35:38
 *    - Removed include <stdio.h>
 *  
 *  Revision 1.1  by: SH  Rev date: 17.09.2000 20:41:22
 *    - Bugfix for reading/writing 16 bit values from/to ide controller
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:56:00
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <io.h>
 #include <progmem.h>
 #include <os/timer.h>
 #include <os/serial.h>
 #include <os/serial_avr.h>
 #include <ide.h>
 
 #include "ui.h"
 

/****************************************************************************
 *
 * Function Name: ansi_clrscr( void )
 *
 * Description:   Clear screen using ANSI escape sequence 'ESC[2J'
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void ansi_clrscr()
 {
    byte i;
    prog_char esc[4] = { ESC, '[', '2', 'J'};
    
    for (i = 0; i < 4; i++) uart_sendByte(esc[i]);

 } /* ansi_clrscr */

/****************************************************************************
 *
 * Function Name: ansi_curpos( byte x, byte y, port* Port )
 *
 * Description:   Set cursor position using ANSI escape sequence 'ESC[x;yH'
 *
 * Parameters:    byte x, byte y
 *
 * Return:        void
 *
 ***************************************************************************/
 void ansi_curpos( byte x, byte y )
 {
    if ( x < 100 && y < 100)
    {
       uart_sendByte(ESC);    
       uart_sendByte('[');    
     
       if ( y > 9 ) uart_sendByte( '0' + y/10);    
       uart_sendByte('0' + y%10);

       uart_sendByte(';');
       
       if ( x > 9 ) uart_sendByte( '0' + x/10);    
       uart_sendByte('0' + x%10);

       uart_sendByte('H');
    }
   
 } /* ansi_curpos */


/****************************************************************************
 *
 * Function Name: ansi_fg( byte color )
 *
 * Description:   Set ANSI foreground color
 *
 * Parameters:    byte color
 *
 * Return:        void
 *
 ***************************************************************************/
 void ansi_fg( byte color )
 {
     if ( color <= WHITE )
     {
        uart_sendByte(ESC);    
        uart_sendByte('[');    
        uart_sendByte('3');    
        uart_sendByte('0' + color);
     }  uart_sendByte('m');
 
 } /* ansi_fg */

/****************************************************************************
 *
 * Function Name: ansi_fg( byte color )
 *
 * Description:   Set ANSI background color
 *
 * Parameters:    byte color
 *
 * Return:        void
 *
 ***************************************************************************/
 void ansi_bg( byte color )
 {
     if ( color <= WHITE )
     {
        uart_sendByte(ESC);    
        uart_sendByte('[');    
        uart_sendByte('4');    
        uart_sendByte('0' + color);
        uart_sendByte('m');
     }
 
 } /* ansi_bg */

/****************************************************************************
 *
 * Function Name: printn()
 *
 * Description:   Print a character n times
 *
 * Parameters:    byte character, byte n
 *
 * Return:        void
 *
 ***************************************************************************/
 void printn( byte character, byte n)
 {
    int i;
    for (i=0; i < n; i++ )
    {
      uart_sendByte(character);
    }

 } /* printn */

/****************************************************************************
 *
 * Function Name: print()
 *
 * Description:   Print the string
 *
 * Parameters:    char* string
 *
 * Return:        void
 *
 ***************************************************************************/
 void print( byte* string )
 {
   byte i=0;
   while (i < 80 && string[i] != '\0')
   {
          uart_sendByte(string[i]);
          ++i;
   }

 } /* print */


/****************************************************************************
 *
 * Function Name: startup()
 *
 * Description:   Show startup message on the given port
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void startup()
 {
   ansi_fg(GREEN);
   ansi_bg(BLACK);
   
   ansi_clrscr();
   ansi_curpos(1,1);

   printn('-', 80);
   ansi_curpos(17,2);
   PRINT( TITLE);
   ansi_curpos(15,3);
   PRINT( COPYRIGHT );
   ansi_curpos(1,4);
   printn('-', 80);

   ansi_curpos(1,24);
   PRINT( ">");

 } /* startup */


/****************************************************************************
 *
 * Function Name: menu_main()
 *
 * Description:   
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void menu_main()
 {
   byte selection;

  /* Show menu
   */   
   show_menu();

  /* Get selection
   */
   for (;;)
   {
     selection = uart_receiveByte();
     
     ansi_curpos(2,24);
     if (selection > 0x1F) uart_sendByte(selection);
     ansi_curpos(2,24);

    /* Handle selection
     */
     switch ( selection )
     {
         case 'd' :
         case 'D' : /* Diagnostics
                     */
                     DeviceDiagnostics();
                     startup();
                     show_menu();
                     break;
   
         case 'i' :
         case 'I' : /* Identify harddisk
                     */
                     IdentifyHarddisk(0);
                     startup();
                     show_menu();
                     break;
         
         case 's' :
         case 'S' : /* Sleep mode
                     */
                     Sleep();
                     break;
         
         case 't' :
         case 'T' : /* Test harddisk
                     */
                     SurfaceTest(0);
                     startup();
                     show_menu();
                     break;

         case 'w' :
         case 'W' : /* Wakeup
                     */ 
                     Wakeup();
                     break;

         default:   /* Error beep
                     */
                     uart_sendByte(BEL);
                     break;
     }

   } /* for(;;)   */
   
 } /* menu_main */

/****************************************************************************
 *
 * Function Name: show_menu()
 *
 * Description:   
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void show_menu( )
 {
     ansi_curpos(20,8);  PRINT("D - Diagnostics");
     ansi_curpos(20,9);  PRINT("I - Identify harddisk");
     ansi_curpos(20,10); PRINT("T - Test harddisk");
     ansi_curpos(20,12); PRINT("S - Set harddisk into sleep mode");
     ansi_curpos(20,13); PRINT("W - Wakeup harddisk");

     ansi_curpos(2,24);

 } /* show_menu */



/****************************************************************************
 *
 * Function Name: DeviceDiagnostics()
 *
 * Description:   Execute device diagnostics command
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void DeviceDiagnostics( void  )
 {
      byte code;

     /* Wait until drive ready
      */     
      while( ide_status() & 0x80); 

     /* Device diagnostics
      */
      ide_write(DRIVE_REGISTER,   0x00);
      ide_write(COMMAND_REGISTER, 0x90);
                      
     /* Wait until drive ready
      */     
      while( ide_status() & 0x80); 

     /* Get result
      */
      code = ide_error();

     /* Show result
      */
      startup();
      ansi_curpos(20,10); PRINT("Results of device diagnostics:");
      ansi_curpos(20,11); PRINT("-----------------------------" );
      
      if ( code == 0x01)
      {
        ansi_curpos(20,13); PRINT("Drive 0: OK");
        ansi_curpos(20,14); PRINT("Drive 1: OK or not present");
      }
      else if ( code == 0x81 )
      {
        ansi_curpos(20,13); PRINT("Drive 0: OK");
        ansi_curpos(20,14); PRINT("Drive 1: defective");
      }
      else if ( code < 0x80)
      {
        ansi_curpos(20,13); PRINT("Drive 0: defective");
        ansi_curpos(20,14); PRINT("Drive 1: OK or not present");
      }
      else 
      {
        ansi_curpos(20,13); PRINT("Drive 0: defective");
        ansi_curpos(20,14); PRINT("Drive 1: defective");
      }
      
     /* Wait for any key
      */
      ansi_curpos(1,24);
      PRINT("Press any key to continue >");
      code = uart_receiveByte();

 } /* DeviceDiagnostics */



/****************************************************************************
 *
 * Function Name: IdenfifyHarddisk()
 *
 * Description:   
 *
 * Parameters:    byte  Drive
 *
 * Return:        void
 *
 ***************************************************************************/
 void IdentifyHarddisk( byte Drive )
 {
     byte i,j;
     word data;
     char buffer[20];
     long capacity;

    /* Harddisk information
     */ 
     word LogicalCylinders;
     word LogicalHeads;
     word LogicalSectors;
     char SerialNumber[21];
     char Model[41];

    /* Wait until drive ready
     */     
     while( ide_status() & 0x80); 

    /* Identify drive: ata command 0xEC
     */
     if ( Drive == 1)
          ide_write(DRIVE_REGISTER,0x10);    
     else ide_write(DRIVE_REGISTER,0x00); 

     ide_write(COMMAND_REGISTER,0xEC); 
    
    /* Wait until drive ready
     */     
     while( ide_status() & 0x80); 

    /* Read ide configuration sector i.e. 255 words
     */
     ide_read_data();

     LogicalCylinders = ide_read_data();      
     ide_read_data();
     LogicalHeads     = ide_read_data();
     ide_read_data();
     ide_read_data();
     LogicalSectors   = ide_read_data();
     ide_read_data();
     ide_read_data();
     ide_read_data();

     for (i=0, j=0; i < 10; i++)            
     {                                         
        data = ide_read_data();
        SerialNumber[j++] = HIGHBYTE(data);
        SerialNumber[j++] = LOWBYTE(data);
     }
     SerialNumber[j] = '\0';

     for (i=0; i<7; i++) ide_read_data();                      

     for (i=0, j=0; i<20; i++) 
     {
        data       = ide_read_data();
        Model[j++] = HIGHBYTE(data);
        Model[j++] = LOWBYTE(data);
     }
     Model[j] = '\0';

     for (i=0; i< 209; i++) ide_read_data();


    /* Show result
     */
     startup();
     ansi_curpos(20,8); PRINT("Identify harddisk - drive ");

     if ( Drive == 1) 
          PRINT("1");
     else PRINT("0");

     ansi_curpos(20,9); PRINT("---------------------------");

     ansi_curpos(20,11); PRINT("Model         : "); print(Model);
     ansi_curpos(20,12); PRINT("Serial Number : "); print(SerialNumber);

     itoa(LogicalHeads, buffer, 10);
     ansi_curpos(20,14); PRINT("Heads         : "); print(buffer);

     itoa(LogicalSectors, buffer, 10);
     ansi_curpos(20,15); PRINT("Sectors       : "); print(buffer);
     
     itoa(LogicalCylinders, buffer, 10);
     ansi_curpos(20,16); PRINT("Cylinders     : "); print(buffer);

     capacity  = LogicalHeads * LogicalSectors ;
     capacity /= 2;
     capacity *= LogicalCylinders;
     capacity /= 1000;
     itoa((int) capacity, buffer, 10);
     ansi_curpos(20,18); PRINT("Capacity [MB] : "); print(buffer);

     
    /* Wait for any key
     */
     ansi_curpos(1,24);
     PRINT("Press any key to continue >");
     i = uart_receiveByte();

 } /* IdenfifyHarddisk */


/****************************************************************************
 *
 * Function Name: Sleep( )
 *
 * Description:   
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void Sleep()
 {
   /* Wait until drive ready
    */     
    while( ide_status() & 0x80); 

     
   /* ATA Command 'STANDBY IMMEDIATE (0xE0) 
    */
    ide_write(COMMAND_REGISTER,0xE0); 

 } /* Sleep */


/***************************************************************************
 *
 * Function Name: Wakeup()
 *
 * Description:   
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void Wakeup( )
 {
   /* Wait until drive ready
    */     
    while( ide_status() & 0x80); 
    
   /* ATA command IDLE IMMEDIATE (0xE1) 
    */  
    ide_write(COMMAND_REGISTER,0xE1); 
 
 } /* Wakeup */


/****************************************************************************
 *
 * Function Name: SurfaceTest( byte drive )
 *
 * Description:   Test all sectors by writing and reading a test data set
 *
 * Parameters:    byte drive
 *
 * Return:        void
 *
 ***************************************************************************/
 void SurfaceTest( byte Drive )
 {
     word LogicalCylinders = 0; 
     word LogicalHeads     = 0;
     word LogicalSectors   = 0;
    
     byte Head;
     byte Sector;
     word Cylinder;
     word idx;
     word i;

     word errors = 0;
     byte error  = 0;
     char buffer[10];
     byte run    = 1;
     long timer  = 0;
     long timeout;
    
    /* Message
     */ 
     startup();
     
     ansi_curpos(20,8); PRINT("Test harddisk - drive ");
     if ( Drive == 1) 
          PRINT("1");
     else PRINT("0");

     ansi_curpos(20,9); PRINT("-----------------------");

     ansi_curpos(20,11); PRINT("Head      : ");
     ansi_curpos(20,12); PRINT("Sector    : ");
     ansi_curpos(20,13); PRINT("Cylinder  : ");
     ansi_curpos(20,15); PRINT("Errors    : ");

     
     timeout = getTimer() + 10000;
     
     while( !STATUS(DRDY) && !TIMER(timeout) );

     if (TIMER(timeout) )
     {
        run = 3;
     }
     else
     {
       /* Get configuration data first
        */
        if ( Drive == 1)
             ide_write(DRIVE_REGISTER,0x10);
        else ide_write(DRIVE_REGISTER,0x00);
        ide_write(COMMAND_REGISTER,0xEC);

        timeout = getTimer() + 5000;
        while( !STATUS(DRQ) && !TIMER(timeout) );

        if ( TIMER(timeout) )
        {
           run = 3;
        }
        else
        {

           ide_read_data();
           LogicalCylinders = ide_read_data();
           ide_read_data();
           LogicalHeads     = ide_read_data();
           ide_read_data();
           ide_read_data();
           LogicalSectors   = ide_read_data();

           for (i=0; i< 249; i++) ide_read_data();

           if (LogicalHeads   > 0x0F) LogicalHeads   = 0x0F;
           if (LogicalSectors > 0x4F) LogicalSectors = 0x4F;

           timer = getTimer()-1;
        }
     }

    /* Loop through all sectors
     */
     for (Cylinder = 0; Cylinder < LogicalCylinders; Cylinder++)
     {
         if (run != 1) break;
         for (Sector = 0; Sector < LogicalSectors; Sector++)
         {
             if (run != 1) break;
             for (Head = 0; Head < LogicalHeads; Head++)
             {
                /*  ESC pressed ?
                 */
                 if ( uart_receivedChar && uart_receiveByte() == ESC)
                 {
                     run = 0;
                     break;
                 }

                /* Update actual sector info every 200 ms
                 */
                 if ( timer - getTimer() < 0)
                 {
                    itoa(Head, buffer, 10);
                    ansi_curpos(33,11);
                    PRINT("  ");
                    ansi_curpos(33,11);
                    print(buffer);

                    itoa(Sector, buffer, 10);
                    ansi_curpos(33,12);
                    PRINT("  ");
                    ansi_curpos(33,12);
                    print(buffer);

                    itoa(Cylinder, buffer, 10);
                    ansi_curpos(33,13);
                    print(buffer);

                    itoa(errors, buffer, 10);
                    ansi_curpos(33,15);
                    print(buffer);

                    timer = getTimer() + 200;
                 }
                 error = 0;

                /* Write data
                 */
                 timeout = getTimer() + 5000;

                 while(  !STATUS(DRDY) && !TIMER(timeout) );

                 if ( TIMER(timeout) )
                 {
                     run = 3;
                     break;
                 }
                 if (Drive == 1)
                      ide_write(DRIVE_REGISTER, 0x10 | Head);
                 else ide_write(DRIVE_REGISTER, Head);

                 ide_write(SECTOR_REGISTER,0x01);
                 ide_write(SECTOR_NUMBER_REGISTER, Sector+1);
                 ide_write(LOW_CYLINDER_REGISTER, LOWBYTE(Cylinder));
                 ide_write(HIGH_CYLINDER_REGISTER, HIGHBYTE(Cylinder));
                 ide_write(COMMAND_REGISTER,0x30);

                 if ( STATUS(IDE_STATUS_ERR) )
                 {
                    error = 1;
                    run = 2;
                    break;
                 }

                 timeout = getTimer() + 5000;
                 while( !STATUS(DRQ) && !TIMER(timeout) );
                 if ( TIMER(timeout) )
                 {
                     run = 3;
                     break;
                 }

                 for (idx=0; idx < 256; idx++ )
                 {
                     ide_write_data(0x0F0F);
                 }
                 if ( STATUS(IDE_STATUS_ERR) )
                 {
                    error = 1;
                    run = 2;
                    break;
                 }
                 else
                 {
                    timeout = getTimer() + 5000;
                    while( STATUS(BUSY) && !TIMER(timeout) );
                    if ( TIMER(timeout) )
                    {
                        run = 3;
                        break;
                    }
                 }

                /* Read data
                 */

                 timeout = getTimer() + 5000;
                 while(  !STATUS(DRDY) && !TIMER(timeout) );
                 if ( TIMER(timeout) )
                 {
                     run = 4;
                     break;
                 }

                 if (Drive == 1)
                      ide_write(DRIVE_REGISTER, 0x10 | Head);
                 else ide_write(DRIVE_REGISTER, Head);

                 ide_write(SECTOR_REGISTER,0x01);
                 ide_write(SECTOR_NUMBER_REGISTER, Sector+1);
                 ide_write(LOW_CYLINDER_REGISTER, LOWBYTE(Cylinder));
                 ide_write(HIGH_CYLINDER_REGISTER, HIGHBYTE(Cylinder));
                 ide_write(COMMAND_REGISTER,0x21);

                 if ( STATUS(IDE_STATUS_ERR) )
                 {
                    error = 1;
                    run = 2;
                    break;
                 }
                 else
                 {
                     timeout = getTimer() + 5000;
                     while( !STATUS(DRQ) && !TIMER(timeout) );
                     if ( TIMER(timeout) )
                     {
                         run = 4;
                         break;
                     }
                     else
                     {    for (idx=0; idx < 256; idx++ )
                          {
                             if ( ide_read_data() != 0x0F0F ) error = 1;
                          }
                          timeout = getTimer() + 5000;
                          while( STATUS(BUSY) && !TIMER(timeout) );
                          if ( TIMER(timeout) )
                          {
                              run = 4;
                          }
                     }
                 }
                 if (error != 0) ++errors;
             }
         }
     }

    /* Wait for any key
     */
     ansi_curpos(1,24);

     switch (run)
     {
         case 0:   /* User break
                    */
                    PRINT("Break. ");
                    break;

         case 1:   /* Programm completed.
                    */
                    PRINT("Ready. ");
                    break;

         case 2:   /* Command error
                    */
                    PRINT("Command Error. ");
                    break;

         case 3:   /* Write Timeout
                    */
                    PRINT("Write data timeout. ");
                    break;

         case 4:   /* Read Timeout
                    */
                    PRINT("Read data timeout. ");
                    break;

         default:  /* Programm stopped, Error.
                    */
                    PRINT("Fatal Error. ");

     }
     PRINT("Press any key to continue >");
     i = uart_receiveByte();

 } /* SurfaceTest */


