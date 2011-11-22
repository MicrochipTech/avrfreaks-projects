#include <iocan128.h>
#include "common.h"


/* some data buffers (declared in main ) */
extern unsigned char ident_in_high ;  /* CAN RX buffer */
extern unsigned char ident_in_low ;
extern unsigned char data_in_count;
extern unsigned char data_in[8];

extern unsigned char ident_out_high ;  /* CAN TX buffer */
extern unsigned char ident_out_low ;
extern unsigned char data_out_count;
extern unsigned char data_out[8];
extern unsigned char out_ready;


/* initialise the CAN interface */
void init_can()
{
CANPAGE = 0x00;   //Just to have all cdmobs in disabled mode before enabling CAN
CANCDMOB = 0;
CANPAGE = 0x10;
CANCDMOB = 0;
CANPAGE = 0x20;
CANCDMOB = 0;
CANPAGE = 0x30;
CANCDMOB = 0;
CANPAGE = 0x40;
CANCDMOB = 0;
CANPAGE = 0x50;
CANCDMOB = 0;
CANPAGE = 0x60;
CANCDMOB = 0;
CANPAGE = 0x70;
CANCDMOB = 0;
CANPAGE = 0x80;
CANCDMOB = 0;
CANPAGE = 0x90;
CANCDMOB = 0;
CANPAGE = 0xA0;
CANCDMOB = 0;
CANPAGE = 0xB0;
CANCDMOB = 0;
CANPAGE = 0xC0;
CANCDMOB = 0;
CANPAGE = 0xD0;
CANCDMOB = 0;
CANPAGE = 0xE0;
CANCDMOB = 0;
// CAN Controller initialization (Wizard Generated)
// CAN: On
CANGCON=0x02;
// CAN Interrupts:
// Timer Overrun: Off
// General Errors: Off
// Frame Buffer: Off
// MOb Errors: Off
// Transmit: Off
// Receive: On
// Bus Off: Off
// All, except Timer Overrun: Off
CANGIE=0xA0;
// MOb0: Disabled, MOb1: Disabled, MOb2: Disabled, MOb3: Disabled, MOb4: Disabled, MOb5: Disabled, MOb6: Disabled, MOb7: Disabled
//CANEN2=0x00; Only READ
// MOb8: Disabled, MOb9: Disabled, MOb10: Disabled, MOb11: Disabled, MOb12: Disabled, MOb13: Disabled, MOb14: Disabled
//CANEN1=0x00;  Only READ
// MOb0..7 Interrupts: MOb0: On, MOb1: On, MOb2: On, MOb3: On, MOb4: On, MOb5: On, MOb6: On, MOb7: On
CANIE2=0xFE; //Enable mailbox interrupt 7..0
// MOb8..14 Interrupts: MOb8: On, MOb9: On, MOb10: Off, MOb11: Off, MOb12: Off, MOb13: Off, MOb14: Off
CANIE1=0x03; //Enable mailbox interrupt 14..8  
// CAN Baud Rate: 250.0 kHz
CANBT1=0x26;            //0E
// Propagation Time Segement: 1.500 us
// Re-Sync Jump Width: 0.500 us
CANBT2=0x0C;      //0C
// Sample Point(s): 3
// Phase Segment 1: 1.000 us
// Phase Segment 2: 1.000 us
CANBT3=0x37;    //37
// CAN Timer Clock Period: 1.000 us
CANTCON=0x00;

}

/* set mailbox up for a transmission */
/* data in gobal registers */
/* uses mailbox 8 to 15 in  */
unsigned char tx_mob = 12;

void can_tx(void)
{
        /* wait for tx not busy */
        while (bit_get(CANGSTA, 1<<TXBSY));
        /* use transmit mailboxs in rotation */

      /* select correct mbox */
	CANPAGE = (unsigned char)(tx_mob<< 4);
	tx_mob++;
	if((tx_mob > 14)  | (tx_mob < 12))
	        tx_mob = 12;
	/* load the id 11 bit */
	CANIDT1 = ident_out_high;
	CANIDT2 = ident_out_low;
	CANIDT3 = 0;
	CANIDT4 = 0;
	CANIDM1 = 0xff;
	CANIDM2 = 0xe0;
	CANIDM3 = 0;
	CANIDM4 = 0x5;
	/* load the data */
	CANMSG = data_out[0];
	CANMSG = data_out[1];
	CANMSG = data_out[2];
	CANMSG = data_out[3];
	CANMSG = data_out[4];
	CANMSG = data_out[5];
	CANMSG = data_out[6];
	CANMSG = data_out[7];
	CANSTMOB = 0; /* clear status */
	CANCDMOB = 0x40 + data_out_count; /* enable tx */
}

/* set up a mailbox to receive */
/* address and mask in global registers */
void can_rx_set(int mbox)
{
	unsigned char x;
      /* select correct mbox */
	CANPAGE = (unsigned char)(mbox<< 4);
	/* load the id 11 bit */
	CANIDT1 = ident_in_high;
	CANIDT2 = ident_in_low;
	CANIDT3 = 0;
	CANIDT4 = 0;
	CANIDM1 = 0xFF;
	CANIDM2 = 0x80;  /* temp */
	CANIDM3 = 0;
	CANIDM4 = 5;    /* was 5 */
	/* clear out the data buffer */
	/* write to message auto inc */
	for(x = 0; x < 8; x++)
	{
		CANMSG = 0;
		
	}
	CANSTMOB = 0; /* clear status */
	CANCDMOB = 0x80 + data_in_count; /* enable rx */
	
}

/* set up a mailbox to receive */
/* address and mask in global registers */
void can_rx_set_adaptor(int mbox)
{
	unsigned char x;
      /* select correct mbox */
	CANPAGE = (unsigned char)(mbox<< 4);
	/* load the id 11 bit */
	CANIDT1 = ident_in_high;
	CANIDT2 = ident_in_low;
	CANIDT3 = 0;
	CANIDT4 = 0;
	CANIDM1 = 0x00;
	CANIDM2 = 0x80;  //To receive all messages to delay up/down fiber
	CANIDM3 = 0;
	CANIDM4 = 5;    /* was 5 */
	/* clear out the data buffer */
	/* write to message auto inc */
	for(x = 0; x < 8; x++)
	{
		CANMSG = 0;
		
	}
        x = CANSTMOB; //Read modify write
	CANSTMOB = 0; /* clear status */
	CANCDMOB = 0x80 + data_in_count; /* enable rx */
	
}

/* read a mailbox and re-enable it */
/* transfer message to gobal registers */
int can_rx(int mbox)
{
	unsigned char x;
	int retval;
	
	retval = 0;
	/* check if mailbox enabled */
	if(mbox < 8)
	{
		x = CANEN2 >> mbox;
	}
	else
	{
		x = CANEN1 >> (mbox - 8);
	}
	if(x & 1) /* still enabled ? */
	{
		return retval;
	}
       /* select correct mbox */
	CANPAGE = (unsigned char)(mbox<< 4);
	x = CANSTMOB ; /* read status */
	CANSTMOB = 0;
        ident_in_high = CANIDT1;
	data_in_count =  CANCDMOB & 0xf;
	if(x & 0x20)
	{
		data_in[0] = CANMSG;
		data_in[1] = CANMSG;
		data_in[2] = CANMSG;
		data_in[3] = CANMSG;
		data_in[4] = CANMSG;
		data_in[5] = CANMSG;
		data_in[6] = CANMSG;
		data_in[7] = CANMSG;
		retval = 1;
	}
        CANCDMOB = 0x80;
	return retval;
}
