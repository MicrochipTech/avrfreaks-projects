/*****************************************************************************
*
*  uIP-AVR - A port of Adam Dunkels' uIP TCP/IP stack to the Atmel AVR
*
*  Created By:       ENC28J60 Driver Port - Jonathan Granade (edi87@fibertel.com.ar)
*					 AVR Port - Louis Beaudoin (www.embedded-creations.com)
*                    uIP      - Adam Dunkels   (www.dunkels.com/adam/uip)
*  Initial Release:  September 22, 2002
*
*  Current Version:  0.90 (port of uIP-AVR 0.90)
*  Release Date:     May 09, 2004
*
*****************************************************************************/


/*****************************************************************************
*
*  Version Info
*
******************************************************************************
*
*  May 09, 2004 - Version 0.90 - port of uIP 0.90
*                 Added Imagecraft compiler support
*                 Extended rtl8019.c rtl8019Init function to set config
*                   registers - allows code to work on boards with the EEDO pin
*                   pulled up (Ethernut)
*                 Changed delay code, works with Imagecraft
*                 MAC address needs to be set in uipopt.h only
* 
*  November 16, 2003 - Version 0.90.b - beta release of uIP-AVR 0.90
*                 Updated to the new uIP 0.9 code
*                 Added generic NIC interface (nic.c), and support for a new
*                   AX88796 NIC driver written by Pascal Stang (ax88796.c)
*                 main.c now calls the arp timer function
*                 uIP has updated uip_arp code which fixes several problems
*                   found in 0.60
*                 A more accurate delay function is included
*
*
*  Sept 30 2002 - Version 0.60.1 - port of uIP 0.60
*                 Fixed errors in rtl8019.c - the Next Page pointer read from
*                   the NIC was invalid (outside the buffer) during times of
*                   heavy incoming traffic.  Fixed by doing a manual packet
*                   receive instead of using the send packet command.  If the
*                   pointer is invalid, the function is exited without
*                   changing any pointers.
*                 rtl8019.c - Packet receive function improved by checking for
*                   an empty buffer and overflow automatically.  rtl8019dev.c
*                   is simplified
*                 rtl8019.c - overflow unreset interrupt flag problem fixed
*                 rtl8019.c - support for using general I/O ports to
*                   communicate with the NIC
*                 Support for Imagecraft C compiler implemented (uIP-AVR must
*                   be compiled with the __IMAGECRAFT__ macro define set in
*                   options)
*                 Support for UIP_BUFSIZE > 255 verified
*
*
*  Sept 22 2002 - Version 0.60.0 - port of uIP 0.60
*                 Initial Release - working on the ATmega161 with External
*                   SRAM interface
*                 Drivers for RTL8019AS (EDTP's Packet Whacker Module - 
*                   www.edtp.com)
*
*
*****************************************************************************/
