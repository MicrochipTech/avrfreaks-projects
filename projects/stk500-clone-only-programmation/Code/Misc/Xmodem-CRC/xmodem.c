//*****************************************************************************
//Author: 	Jidan Al-Eryani,	jidan@gmx.net
// Date:	3.4.2007 
//Tested with: 
//	+MCU: ATmega128(L) 
//	+Dataflash: AT45DB041B (512K X 8)
//	+Compiler: avr-gcc (GCC) 4.1.1
//
//xmodem_download() is based on ATMEL AppNote: AVR350: Xmodem CRC Receive Utility for AVR
//
//******************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "xmodem.h"
#include "dataflash.h"

volatile unsigned char buf[133];


struct global
{
  volatile unsigned char *recv_ptr;
  volatile unsigned char buffer_status;
  volatile unsigned char recv_error;
  volatile unsigned char t1_timed_out;
} gl;

unsigned char flush; // for function xmodem_purge(): dummy value declared as global to eliminate compiler warning

ISR(TIMER1_OVF_vect){ //belongs to xmodem_purge() and sendc()
    gl.t1_timed_out = TRUE;
}


//USART recieve complete ISR
ISR(USART0_RX_vect)
{
    volatile unsigned char *local_ptr;

    local_ptr = gl.recv_ptr;
    
	// check for errors before reading data register (reading UDR clears status)
    if (UCSR0A & ((1<<FE0) | (1<<DOR0)) ) //Frame Error; Data OverRun
    {
      gl.recv_error = TRUE;   // will NAK sender in respond.c
    }                         // always read a character otherwise another interrupt could get generated
                              // read status register before reading data register
    *local_ptr++ = UDR0;       // get char
    switch (buf[0])           // determine if buffer full
    {
        case (SOH) :
            if (local_ptr == (&buf[132] + 1))
            {
                gl.buffer_status = full;
                local_ptr = &buf[0];
            }
        break;
        case (EOT) :
            gl.buffer_status = full;
            local_ptr = &buf[0];
        break;
        default :
            gl.buffer_status = full;    // first char unknown
            local_ptr = &buf[0];
        break;
    }
    gl.recv_ptr = local_ptr;            // restore global pointer
}


//Initialize
void xmodem_init(void)
{
	/*initialize Timer/Counter(TC) 1*/
	TCCR1A = 0x00; // timer/counter 1 PWM disable
	TCCR1B = 0x00; // timer/counter 1 clock disable
	TIMSK |= (1<<TOIE1); // TC 1 Overflow Interrupt Enable 
	
	//initialize USART
	usart_init(1); //initialize usart with RX Complete Interrupt Enable
}


/* wait 1 second for sender to empty its transmit buffer */
void xmodem_purge(void) //not needed!!
{
    gl.t1_timed_out = FALSE;

    // 1 second timeout
    // 8MHz / 1024 = 7813 Hz
    // 7813 Hz = 128 us
    // 1 seconds / 128 us = 7813
    // 65536 - 7813 = 57723 = e17b
    // interrupt on ffff to 0000 transition
    TCNT1 = 0xE17B; //57723
    TCCR1B = (1<<CS12) | (1<<CS10); // CLK/1024 prescalar

    while (gl.t1_timed_out != TRUE) // read uart until done
    {
        flush = UDR0;
    }
    TCCR1B = 0x00; // disable timer/counter 1 clock
}


/* Receive function */
void xmodem_download(unsigned int DF_START_ADR){
  volatile unsigned char *bufptr1;
  bufptr1 = &buf[0];
  unsigned char packet = 0;    // status flag
  unsigned char packet_number; // represents 'last successfully received packet'
  
  // initialize USART and timer
  xmodem_init(); 
  
  sei();// enable interrupts 
  xmodem_purge(); // clear uart data register ... allow transmitter the opportunity to unload its buffer
  
  //Dataflash variables and initializations
  unsigned char df_buf[264];
  unsigned int df_mem_page = DF_START_ADR;
  unsigned int df_byte = 0;
  //unsigned int correct_pages = FILE_SIZE/264;
  //if ( (FILE_SIZE % 264) )
	//correct_pages = correct_pages +1;
  DF_SPI_init();
  
  packet_number = 0x00;        // initialise to first xmodem packet number - 1
  gl.recv_ptr = bufptr1;       // point to recv buffer

  sendc();                     // send a 'c' until the buffer gets full

  while (packet != end)        // get remainder of file
  {
    recv_wait();               // wait for error or buffer full
    packet = validate_packet((unsigned char*)bufptr1,&packet_number);  // validate the packet
    gl.recv_ptr = bufptr1;     // re-initialize buffer pointer before acknowledging
    switch(packet)
    {
      case good:
		for (unsigned int i=3; i<=130; i++){
			if ( df_byte == 264){
				//void Buffer_Write_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
				Buffer_Write_Str (1, 0, 264, df_buf);
				DF_CS_inactive;
				//void Buffer_To_Page (unsigned char BufferNo, unsigned int PageAdr);
				Buffer_To_Page(1, df_mem_page);
				DF_CS_inactive;
				df_mem_page++;
				df_byte=0;
			}
			df_buf[df_byte] = buf[i];
			df_byte++;
		}
        break;
	  case end:
		if ( df_byte > 0){
			for (unsigned int j = df_byte; j < 264; j++)
					df_buf[j]= 0xff;
			
			Buffer_Write_Str (1, 0, 264, df_buf);
			DF_CS_inactive;
			//void Buffer_To_Page (unsigned char BufferNo, unsigned int PageAdr);
			Buffer_To_Page(1, df_mem_page);
			DF_CS_inactive;
			}
	  
		break;
      case dup:
        // a counter for duplicate packets could be added here, to enable a
        // for example, exit gracefully if too many consecutive duplicates,
        // otherwise do nothing, we will just ack this
        break;
      default:
        // bad, timeout or error -
        // if required, insert an error handler of some description,
        // for example, exit gracefully if too many errors
        break; // statement just to eliminate compiler warning
    }
    respond(packet);                  // ack or nak
  }// end of file transmission
  
  cli(); //clear interrupts again
  
  /*simple test if the data were copied correctly to dataflash
	if( df_mem_page == correct_pages) //e.g. 805 pages for Spartan3 XCS400 configuration file
		usart_transmits("...Downloading of file to DataFlash was successful! ");
	else
		usart_transmits("...Sorry! Downloading of file to DataFlash was NOT successful! ");*/
		
	usart_transmits("...Downloading of file to DataFlash was successful! ");

}


/* Sends the C char */
void sendc(void) {

    // 3 second timeout
    // 8MHz / 1024 = 7813 Hz
    // 7813 Hz = 128 us
    // 3 seconds / 128 us = 23438
    // 65536 - 23438 = 42098 = A472
    // interrupt on ffff to 0000 transition
    TCNT1 = 0xA472;
    TCCR1B = 0x00; // disable timer/counter 1 clock

    // enable entry into while loops
    gl.buffer_status = empty;
    gl.t1_timed_out = FALSE;
    gl.recv_error = FALSE; // checked in validate_packet for framing or overruns

    // send character 'C' until we get a packet from the sender
    while (!gl.buffer_status)
    {
        // tell sender CRC mode
		usart_transmit(CRCCHR); //transmitt 'C' (signal transmitter that I'm ready in CRC mode ... 128 byte packets)

		TCCR1B = (1<<CS12) | (1<<CS10); // CLK/1024 prescalar

        // wait for timeout or recv buffer to fill
        while (!gl.t1_timed_out && !gl.buffer_status);

        // turn off timer
        TCCR1B = 0x00;              // disable timer/counter 1 clock

        if (gl.t1_timed_out)                  // start wait loop again
        {
            gl.t1_timed_out = FALSE;
            TCNT1 = 0xA472;           // load counter ... start over
        }
    }
}

/* Receive wait */
void recv_wait(void){
    gl.t1_timed_out = FALSE;                    // set in timer counter 1 overflow interrupt routine

    // 1 second timeout
    // 8MHz / 1024 = 7813 Hz
    // 7813 Hz = 128 us
    // 1 seconds / 128 us = 7813
    // 65536 - 7813 = 57723 = e17b
    // interrupt on ffff to 0000 transition
    TCNT1 = 0xE17B;
	TCCR1B = (1<<CS12) | (1<<CS10); // CLK/1024 prescalar

     // wait for packet, error, or timeout
    while (!gl.buffer_status && !gl.t1_timed_out);
 
    TCCR1B = 0x00;                          //  turn off TC1 - no more time outs needed
}


/*Validates the received packet*/
unsigned char validate_packet(unsigned char *bufptr,unsigned char *packet_number){

  unsigned char packet;
  int crc;

// uncomment the following line to allow error simulation
// replace 'bad' with dup, end, err, out as required.
//  if(!PIND_Bit3) return (bad); // simulate an error if button pressed

  packet = bad;
  if (!gl.t1_timed_out)
  {
    if (!gl.recv_error)
    {
      if (bufptr[0] == SOH) // valid start
      {
        if (bufptr[1] == ((*packet_number+1) & 0xff)) // sequential block number ?
        {
          if ((bufptr[1] + bufptr[2]) == 0xff) // block number and block number checksum are ok?
          {
            crc = calcrc(&bufptr[3],128);      // compute CRC and validate it
            if ((bufptr[131] == (unsigned char)(crc >> 8)) && (bufptr[132] == (unsigned char)(crc)))
            {
              *packet_number = *packet_number + 1; // good packet ... ok to increment
              packet = good;
            }// bad CRC, packet stays 'bad'
          }// bad block number checksum, packet stays 'bad'
        }// bad block number or same block number, packet stays 'bad'
        else if (bufptr[1] == ((*packet_number) & 0xff))
        {                         // same block number ... ack got glitched
          packet = dup;           // packet is duplicate, don't inc packet number
        }
      }
      else if (bufptr[0] == EOT)  // check for the end
        packet = end;

      else packet = bad;  usart_transmit('B');// byte zero unrecognised
                          // statement not required, included for clarity
    }
    else packet = err; usart_transmit('O');// UART Framing or overrun error
  }
  else packet = out; usart_transmit('T');// receive timeout error


  return (packet); // one of: good, dup, end, bad, err, out
}

/* CRC calculation */
int calcrc(unsigned char *ptr, int count)
{
    int crc;
    char i;

    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}

/* Respond function */
void respond(unsigned char packet)
{
    // clear buffer flag here ... when acking or nacking sender may respond
    // very quickly.
    gl.buffer_status = empty;
    gl.recv_error = FALSE; // framing and over run detection

    if ((packet == good) || (packet == dup) || (packet == end))
    {
        while (!(UCSR0A & 0x20)); // wait till transmit register is empty
        UDR0 = ACK; // now for the next packet
    }
    else
    {
        while (!(UCSR0A & 0x20)); // wait till transmit register is empty
        xmodem_purge(); // let transmitter empty its buffer
        UDR0 = NAK; // tell sender error
    }
}


/* Transmit function */ 
void xmodem_upload(unsigned int DF_START_ADR, unsigned long FILE_SIZE){ //e.g. xmodem_upload(0, 212392)

	unsigned int last_packet_nr = FILE_SIZE/128; //e.g. FILE_SIZE/128 = 1659.31
	if ( (FILE_SIZE & 0x7f))//round-up e.g. 1659.31 -> 1660
		last_packet_nr = last_packet_nr + 1;
	
	unsigned char packet_nr = 1;
	unsigned long file_byte = 0;
	unsigned char packet[133];
	int crc;
	
	//dataflash variables
	unsigned int df_page = DF_START_ADR;
	unsigned int df_byte = 0; //max is DF_PAGE_SIZE(264)

	//initialize dataflash
	DF_SPI_init();
	Page_To_Buffer (df_page, 1); //copy first page from DF
	
	usart_init(0);

	usart_receive(); //flush anything on USART register 
	
	for (unsigned int i= 1; i <= last_packet_nr; i++){
	
		//set start header
		packet[0] = SOH;
		
		//set packet number
		packet[1] = packet_nr;
		
		//set packet number 2
		packet[2] = 255 - packet_nr;
	
		if( packet_nr == 255)
			packet_nr = 0;
		else
			packet_nr++;
		
		//set data
		for(unsigned int j=3; j <= 130; j++){ //get from DF 128 byte
			if (file_byte >= FILE_SIZE){
				packet[j]= EOF;
				continue;
			}
			file_byte++;
			
			//get a new page from dataflash if page in buffer is all read´
			if (df_byte >= DF_PAGE_SIZE){
				df_page++;
				df_byte = 0;
				Page_To_Buffer (df_page, 1);
			} 
			
			//get 1 byte
			packet[j] = Buffer_Read_Byte(1, df_byte);
			df_byte++;
				
		}
		
		// set CRC
		crc = calcrc(&packet[3],128);
		packet[131]= (unsigned char)(crc >> 8);
		packet[132]= (unsigned char)(crc);
		
		//finaly send the packet and wait for Ack signal
		do{
			usart_transmitd(packet, 133);
		} while( usart_receive() != ACK);

	}
	
	//signal end of transmission
	do{
		usart_transmit(EOT);
	} while (usart_receive() != ACK);
	
	usart_transmits("...Uploading data to PC was successful! ");
}
	
