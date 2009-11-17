/*						mbs.c			*/

#include <iom8535v.h>
#include <macros.h>
#include "modbus.h" 
#include "uart.h"
#include "adc.h"  

typedef union Uni1{
      uint u;
      uchar c[2];
}Uni;

static uchar rx_buf[32], tx_buf[32];
static uchar rx_cnt, tx_cnt;
static uint hold_reg[32], channel_no;

void mb_03(void)
{
uint i;
Uni addr,quan;
        
        quan.c[0] = rx_buf[5];
		quan.c[1] = rx_buf[4];
		addr.c[0] = rx_buf[3];
		addr.c[1] = rx_buf[2];
		tx_cnt =  quan.u * 2 + 5;
        tx_buf[0] = rx_buf[0];
        tx_buf[1] = rx_buf[1];
        tx_buf[2] = tx_cnt;
		for(i=0;i<7;i++)
		{
		   tx_buf[i * 2 + 3] = (hold_reg[i] >> 8);
		   tx_buf[i * 2 + 4] = (hold_reg[i] & 0xFF);
		}
        Mb_calcul_crc(tx_buf,  quan.c[0] * 2 + 3);
        for(i=0;i<tx_cnt;i++)
            TransmitByte(tx_buf[i]);
}

void mb_06(void)
{
uchar i;
        i = rx_buf[3];
		hold_reg[i] = rx_buf[5];
        tx_cnt =  8;
        tx_buf[0] = rx_buf[0];
        tx_buf[1] = rx_buf[1];
        tx_buf[2] = rx_buf[2];
        tx_buf[3] = rx_buf[3];
		tx_buf[4] = hold_reg[i] >> 8;
        tx_buf[5] = hold_reg[i] & 0xFF;
        Mb_calcul_crc(tx_buf, 6);
        for(i=0;i<8;i++)
            TransmitByte(tx_buf[i]);
}

void bad_func(void)
{
uchar i;
        
        tx_cnt =  5;
		tx_buf[0] = rx_buf[0];  
		tx_buf[1] = rx_buf[1] | 0x80;   
        tx_buf[2] = 0x01;
        Mb_calcul_crc(tx_buf, 3);
        for(i=0;i<8;i++)
            TransmitByte(tx_buf[i]);
}

void bad_data(void)
{
uchar i;
        
        tx_cnt =  5;
		tx_buf[0] = rx_buf[0];
		tx_buf[1] = rx_buf[1] | 0x80;  
        tx_buf[2] = 0x03;
        Mb_calcul_crc(tx_buf, 3);
        for(i=0;i<8;i++)
            TransmitByte(tx_buf[i]);
}

void main(void)
{
uchar i;
	PORTB = 0xFF;
	DDRB = 0xFF;
	  
    for(i=0;i<32;i++)
        tx_buf[i] = 0x0;
    InitUART( 25 ); /* set the baudrate to 1900 bps using a 8MHz crystal */
	ADC_init(0xC0);	/* chanel ADC0, internal ref 2.68V */
	
	_SEI(); /* enable interrupts => enable UART interrupts */
	
	rx_cnt = tx_cnt = 0;
    while(1){
      while(DataInReceiveBuffer())
	  {  
		rx_buf[rx_cnt] = ReceiveByte();   // read MODBUS message
		if(rx_cnt < 7)
            rx_cnt++;
		else
		{
		  if(rx_buf[0]==MB_ADDR)
		    {  	
            switch(rx_buf[1]){  // function
            case 0x03:      // read n regs
                if(!Mb_test_crc(rx_buf,6))
                    mb_03();
                else
                    bad_data();
            break;
            case 0x06:      // write 1 reg
                if(!Mb_test_crc(rx_buf,6))
                    mb_06();
                else
                    bad_data();

            break;
            default: //pm2_pstr("function bad");
			   bad_func();
			   rx_cnt = tx_cnt = 0; 
			break;
            } //switch
		  }
		  rx_cnt = tx_cnt = 0;
		} //else
	}// while(DataInReceiveBuffer())
//	adc_val = ADC_read(channel_no);
	hold_reg[channel_no] = ADC_read(channel_no);
	channel_no++;
	if(channel_no>7)
	   channel_no = 0;
    PORTB = ~hold_reg[16];
    }// while(1)
}
