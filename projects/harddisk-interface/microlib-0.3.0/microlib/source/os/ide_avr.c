/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\ide_avr.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.2 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:45:42 $
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
 *  Description:  IDE controller access
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\ide_avr.d $
 *  
 *    MicroLIB - IDE functions for ATMEL AVR 90S8515
 *  
 *  Revision 1.2  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.1  by: SH  Rev date: 17.09.2000 20:39:24
 *    - Bugfix for reading/writing 16 bit values from/to ide controller
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:48:04
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *
 * PIN Assignments for ATMEL to IDE-Interface
 * 
 ****************************************************************************
 *
 *    IDE           |   Atmel Controller
 *   ------------------------------------------
 *    DD0 .. DD7    |   Port A0 .. A7
 *    DD8 .. DD15   |   Port C0 .. C7
 *    /DIOW         |   Port B1
 *    /DIOR         |   Port B3
 *    /CS0          |   Port B0
 *    /CS1          |   Port B2
 *    DA0           |   Port D4
 *    DA1           |   Port D5
 *    DA2           |   Port D6
 *    /RESET        |   Port D7
 *   
 */
 
/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <ide.h>

/****************************************************************************
 *
 * Function Name: ide_ioctl( void* data, word length, byte control )
 *
 * Description:   Device control
 *
 * Parameters:    void* data, word length, byte control
 *
 * Return:        word
 *
 ***************************************************************************/
 word ide_ioctl( void* data, word length, byte control )
 {
    word result = 0;

    switch(control)
    {
        case IDE_IOCTL_RESET:  
        {
            unsigned int i;

           /* Set port A, C, B0..B3, D4..D7 to output
            */ 
            outp(0xff, DDRA);
            outp(0xff, DDRC);
            outp(0x0f, DDRB);
            outp(0xf0, DDRD);

           /* output default values on Port A and C, , B0..B3 and D4..D6
            */
            outp(0x00, PORTA);
            outp(0x00, PORTC);
            outp(0x0f, PORTB);
            outp(0x80, PORTD);

           /* Low pulse on port D7
            */ 
            for (i =0; i < 0xFFFF; i++) outp(0x00, PORTD);
            outp(0x80, PORTD);
        }
        break;
        
        
        case IDE_IOCTL_SOFTRESET:  
        {
            int i;
            for (i =0; i < 0xFFFF; i++)  ide_outbyte(1, 6, 0x0E);
            ide_outbyte(1, 6, 0x0A);
        }
        break;
    }
    return result;

 } /* ide_ioctl */

/****************************************************************************
 *
 * Function Name: ide_outword( void )
 *
 * Description:   Write a word to the IDE controller
 *
 * Parameters:    byte block, byte address, word data
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_outword( byte block, byte address, word data )
 {
     byte adr;
     byte blk;
     byte c, i, j, k, work;

    /* Set data direction for 16 bit output operation
     */
     outp(0xff,DDRA);
     outp(0xff,DDRC);

    /* Write data to ports A and C
     *
     * BITSWAP: Bit swapping is used for a hardware bug in my prototype board
     */
     outp(data % 0x100, PORTA);
     c = data / 0x100;
     
     #ifdef BITSWAP
     for (i=0, j=0x80, k=1, work=0; i<8;i++)
     {
        if ( (c & k) == k) work = work + j;
        k = k << 1;
        j = j >> 1;
     }
     outp(work, PORTC);
     #else
     outp(c , PORTC);
     #endif

    /* Write address to port D4 ..D6
     */
     adr = address & 0x07;
     adr = adr << 4;
     adr = adr + 0x80;
     outp(adr, PORTD);

    /* Select register block with port B0 and B2
     */
     if (block == 0) blk = 0x04;
     else            blk = 0x01;

     outp(0x0a + blk, PORTB);

    /* Write low pulse on port B1
     */
     outp(0x08 + blk, PORTB);
     outp(0x08 + blk, PORTB);

    /* Deselect register block
     */
     outp(0x0f, PORTB);

 } /* ide_outword */

/****************************************************************************
 *
 * Function Name: ide_outbyte( byte block, byte address, byte data )
 *
 * Description:   Write a byte to the IDE controller
 *
 * Parameters:    byte block, byte address, byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void ide_outbyte( byte block, byte address, byte data )
 {
     byte adr;
     byte blk;

    /* Set data direction for 8 bit output operation
     */
     outp(0xff, DDRA);

    /* Write data to port A
     */
     outp(data, PORTA);

    /* Write address to port D4 ..D6
     */
     adr = address & 0x07;
     adr = adr << 4;
     adr = adr + 0x80;
     outp(adr, PORTD);

    /* Select register block with port B0 and B2
     */
     if (block == 0) blk = 0x04; 
     else            blk = 0x01;

     outp(0x0a + blk, PORTB);

    /* Write low pulse on port B1
     */
     outp(0x08 + blk, PORTB);
     outp(0x08 + blk, PORTB);

    /* Deselect register block
     */ 
     outp(0x0f, PORTB);
 
 } /* ide_outbyte */

/****************************************************************************
 *
 * Function Name: ide_inbyte( byte block, byte address )
 *
 * Description:   Read a byte from the IDE controller
 *
 * Parameters:    byte block, byte address
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte ide_inbyte( byte block, byte address )
 {
     byte result;
     byte adr;
     byte blk;

    /* Set data direction for 8 bit input operation
     */
     outp(0x00, DDRA);

    /* Write address to port D4 ..D6
     */
     adr = address & 0x07;
     adr = adr << 4;
     adr = adr + 0x80;

     outp(adr, PORTD);

    /* Select register block with port B0 and B2
     */
     if (block == 0) blk = 0x04; 
     else            blk = 0x01;

     outp(0x0a + blk, PORTB);

    /* Set port B3 (/DIOR) to low level (Attention! Atmel is too fast ...)
     */
     outp(0x02 + blk, PORTB);
     outp(0x02 + blk, PORTB);

    /* Read data
     */ 
     result = inp(PINA);

    /* Deselect register block
     */
     outp(0x0f, PORTB);

    /* return result
     */ 
     return result;
 
 } /* ide_inbyte */

 /****************************************************************************
  *
  * Function Name: ide_inword( byte block, byte address )
  *
  * Description:   Read a word from the IDE controller
  *
  * Parameters:    byte block, byte address
  *
  * Return:        word
  *
  ***************************************************************************/
  word ide_inword( byte block, byte address )
  {
     word result = 0;
     byte adr;
     byte blk;

     byte i,j,k,c, work;

    /* Set data direction for 16 bit input operation
     */
     outp(0x00, DDRA);
     outp(0x00, DDRC);

    /* Write address to port D4 ..D6
     */
     adr = address & 0x07;
     adr = adr << 4;
     adr = adr + 0x80;

     outp(adr, PORTD);

    /* Select register block with port B0 and B2
     */
     if (block == 0) blk = 0x04; 
     else            blk = 0x01;

     outp(0x0a + blk, PORTB);

    /* Set port B3 (/DIOR) to low level (Attention! Atmel is too fast ...)
     */
     outp(0x02 + blk, PORTB);
     outp(0x02 + blk, PORTB);

    /* Read data
     *
     * BITSWAP: Bit swapping is used for a hardware bug in my prototype board
     */ 
     #ifdef BITSWAP
     c = inp (PINC);
     for (i=0, j=0x80, k=1, work=0; i<8;i++)
     {
       if ( (c & k) == k) work = work + j;
       k = k << 1;
       j = j >> 1;
     }
     result = inp(PINA) + 256 * work;
     #else
     result = inp(PINA) + 256 * inp(PINC);
     #endif
   

    /* Deselect register block
     */
     outp(0x0f, PORTB);

    /* return result
     */ 
     return result;

  } /* ide_inword */
 

