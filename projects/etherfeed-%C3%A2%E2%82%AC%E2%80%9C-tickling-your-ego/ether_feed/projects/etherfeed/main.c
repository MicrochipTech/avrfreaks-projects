#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>
#include <avr/interrupt.h>

#include "global-conf.h"
#include <util/delay.h>

#include "port_app_mapper.h"
#include "config.h"
#include "uip_arp.h"
#include "network.h"
#include "enc28j60.h"
#include "timer.h"
#include "clock-arch.h"
#include "AVR035.h"
#include "lcd.h"
//#include "hd44780.h"
#include "webclient.h"
#include "resolv.h"
#include "eeprom.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
static char hostname[20]="";
void save_ip_addresses(void);
void read_ip_addresses(void);


//FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

extern volatile uint16_t tick;
extern volatile uint16_t tickS;

uint8_t systemID[8];

uint8_t mymac[6] = {0x56,0x55,0x58,0x10,0x00,0x29};

uint8_t gwip[4];
uint8_t myip[4];
uint8_t nmask[4];
uint8_t dnsip[4]={192,168,1,1}; 

/*ISR(WDT_vect) {
    printf("Watchdog caused reset\r\n");

    while(true); //Next WDT timeout will reset chip
}*/




int main(void) {



	   read_ip_addresses();

	   _enable_dhcp=eeprom_read_byte((void *)350);
	   
	   lcd_init(LCD_DISP_ON);
	  
	  lcd_clrscr();
	   
	   lcd_puts("etherfeed v1.0");	
	  
	    
	//	sei();
	//	uint8_t resetSource = MCUSR;
	//	MCUSR = 0;
	//	wdt_reset();
	//	wdt_disable();
	//	wdt_enable(WDTO_1S);
	//	WDTCSR |= (1 << WDIE);  //enable watchdog interrupt
	//	wdt_reset();
	//	cli();
		
	//	CLKPR = (1<<CLKPCE);	//Change prescaler
	//	CLKPR = (1<<CLKPS0);	//Use prescaler 2
		
		clock_init();
		
		
		uip_ipaddr_t ipaddr;
		struct timer periodic_timer, arp_timer;
		uint16_t timer_SendData;
		
		timer_SendData = 0;
		
		
/*		if(resetSource & (1<<WDRF))
		{
		  //lcd_clrscr();
		  // lcd_puts("WDT Reset!");	
		}

		if(resetSource & (1<<BORF))
		{
			//("Mega was reset by brownout...\r\n");
		}

		if(resetSource & (1<<EXTRF))
		{
			//("Mega was reset by external...\r\n");
		}

		if(resetSource & (1<<PORF))
		{
		  //  lcd_clrscr();
		  //  lcd_puts("Power on rst!");	//("Mega was reset by power on...\r\n");
		}

*/



		if (eepromReadByte(0) == 255 || eepromReadByte(11) == 255)
		{
				
				//Set defaults
				eepromWriteByte(0, 0); //init

				myip[0] = 192;
				myip[1] = 168;
				myip[2] = 1;
				myip[3] = 67; //47 in final versions

				nmask[0] = 255;
				nmask[1] = 255;
				nmask[2] = 255;
				nmask[3] = 0;

				gwip[0] = 192;
				gwip[1] = 168;
				gwip[2] = 1;
				gwip[3] = 1;

				dnsip[0] = 192;
				dnsip[1] = 168;
				dnsip[2] = 1;
				dnsip[3] = 1;

				eepromWriteByte(29, 80);  //web port
				eepromWriteByte(10, 0);  //dhcp off

				save_ip_addresses();
			//	wdt_reset();

				eepromWriteStr(200, "SBNG", 4);  //default password
				eepromWriteByte(204, '\0');
				eepromWriteByte(205, '\0');
				eepromWriteByte(206, '\0');
				eepromWriteByte(207, '\0');
				eepromWriteByte(208, '\0');
				eepromWriteByte(209, '\0');

				eeprom_write_block ("irdroid.com",(void *)300,20); 

		//		wdt_reset();
				
				 eepromWriteByte(35, 209);
				 eepromWriteByte(36, 85);
				 eepromWriteByte(37, 101);
				 eepromWriteByte(38, 134);
				 timer_update = 60;
				// eepromWriteByte(39, 60);
			//	 eeprom_write_block ((void*)timer_update,(void *)39,1); 
			 eeprom_write_word ((uint16_t *)39, timer_update) ;
				 
				 
	//			 wdt_reset();
			   

				eepromWriteByte(1, EEPROM_VERSION);
		} 

			//MAC will be 56 51 99 36 14 00 with example system id
			mymac[0] = UIP_ETHADDR0;
			mymac[1] = UIP_ETHADDR1;
			mymac[2] = UIP_ETHADDR2;
			mymac[3] = UIP_ETHADDR3;
			mymac[4] = UIP_ETHADDR4;
			mymac[5] = UIP_ETHADDR5;
			
        //timer_update = eepromReadByte(39);
		//eeprom_read_block ((void*)timer_update,(void*)39,1);
		
		timer_update = eeprom_read_word((uint16_t*)39); 
       //  lcd_clrscr();
	   
	 //  lcd_puts(timer_update);	

//		network_init();

        network_init_mac(mymac);

		timer_set(&periodic_timer, CLOCK_SECOND / 2);
		timer_set(&arp_timer, CLOCK_SECOND * 10);

		uip_init();
		
		uip_arp_init();
	 
		struct uip_eth_addr mac = { { mymac[0],  mymac[1],  mymac[2],  mymac[3],  mymac[4],  mymac[5]} };

		uip_setethaddr(mac);
		if(_enable_dhcp) {
		
			dhcpc_init(&mymac, 6);
			dhcpc_request();
		
			} else {
		
		uip_ipaddr(ipaddr, myip[0], myip[1], myip[2], myip[3]);
		uip_sethostaddr(ipaddr);
		uip_ipaddr(ipaddr, gwip[0], gwip[1], gwip[2], gwip[3]);
		uip_setdraddr(ipaddr);
		uip_ipaddr(ipaddr, nmask[0], nmask[1], nmask[2], nmask[3]);
		uip_setnetmask(ipaddr);
		//uip_setethaddr(mac);
		
		}
		
		eeprom_read_block (hostname,(void *)300,20);
	
	resolv_init();
			
    uip_ipaddr(ipaddr, dnsip[0], dnsip[1], dnsip[2], dnsip[3]);
    resolv_conf(ipaddr);
//	wdt_reset();

    //webserver = resolv_lookup(hostname);
	
	   
	    resolv_query(hostname);
		
      //  webclient_init();
		
		httpd_init();
		 
		
	//	wdt_reset();

		while (1) {
			//Only one event may fire per loop, listed in order of importance
			
			//If an event is skipped, it will be run next loop
		/*  if (tickDiff(timer_Count) > 1) {
				timer_Count = tick;
			   wdt_reset(); //sikre at watchdog resetter os hvis timer systemet fejler, vi når her hvert 2ms
		   //    updateCounters(); //bør ske i en interrupt istedet, for at garentere 2ms aflæsning
		   
		 } */ if (tickDiffS(timer_SendData) >= timer_update) {
				//printf("Timer : webclient \r\n");
			   
				timer_SendData = tickS;
			   webclient_connect();
			} 

			//Net handling below



			
			
			
				uip_len = network_read();

				if (uip_len > 0) {
					if (BUF->type == htons(UIP_ETHTYPE_IP)) {
						uip_arp_ipin();
						uip_input();
						if (uip_len > 0) {
							uip_arp_out();
							network_send();
						}
					} else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {
						uip_arp_arpin();
						if (uip_len > 0) {
							network_send();
						}
					}

				} else if (timer_expired(&periodic_timer)) {
					timer_reset(&periodic_timer);
					//FLIPBIT(PORTC,5);
		//            printf("Timers : %u %u \r\n", tick, tickS);

					for (uint8_t i = 0; i < UIP_CONNS; i++) {
						uip_periodic(i);
						if (uip_len > 0) {
							uip_arp_out();
							network_send();
						}
					}

		#if UIP_UDP
					for (uint8_t i = 0; i < UIP_UDP_CONNS; i++) {
						uip_udp_periodic(i);
						if (uip_len > 0) {
							uip_arp_out();
							network_send();
						}
					}
		#endif /* UIP_UDP */

					if (timer_expired(&arp_timer)) {
						timer_reset(&arp_timer);
						uip_arp_timer();
					}
				}
			}
		
		return 0;
	}






void read_ip_addresses(void)
{
myip[0] = eepromReadByte(11);
	myip[1] = eepromReadByte(12);
	myip[2] = eepromReadByte(13);
	myip[3] = eepromReadByte(14);

	gwip[0] = eepromReadByte(15);
	gwip[1] = eepromReadByte(16);
	gwip[2] = eepromReadByte(17);
	gwip[3] = eepromReadByte(18);

	nmask[0] = eepromReadByte(19);
	nmask[1] = eepromReadByte(20);
	nmask[2] = eepromReadByte(21);
	nmask[3] = eepromReadByte(22);

	dnsip[0] = eepromReadByte(31);
	dnsip[1] = eepromReadByte(32);
	dnsip[2] = eepromReadByte(33);
	dnsip[3] = eepromReadByte(34);
}


void save_ip_addresses(void)
{
eepromWriteByte(11, myip[0]);
eepromWriteByte(12, myip[1]);
eepromWriteByte(13, myip[2]);
eepromWriteByte(14, myip[3]);

	eepromWriteByte(15, gwip[0]);
	eepromWriteByte(16, gwip[1]);
	eepromWriteByte(17, gwip[2]);
	eepromWriteByte(18, gwip[3]);

	eepromWriteByte(19, nmask[0]);
	eepromWriteByte(20, nmask[1]);
	eepromWriteByte(21, nmask[2]);
	eepromWriteByte(22, nmask[3]);

	eepromWriteByte(31, dnsip[0]);
	eepromWriteByte(32, dnsip[1]);
	eepromWriteByte(33, dnsip[2]);
	eepromWriteByte(34, dnsip[3]);
}


void dhcpc_configured(const struct dhcpc_state *s)
{
    uip_ipaddr_t addr;
    struct uip_eth_addr mac = { { mymac[0],  mymac[1],  mymac[2],  mymac[3],  mymac[4],  mymac[5]} };

    // byte swap the network info
    myip[0] = (s->ipaddr[0]);
    myip[1] = (s->ipaddr[0]) >> 8;
    myip[2] = (s->ipaddr[1]);
    myip[3] = (s->ipaddr[1]) >> 8;

    nmask[0] = (s->netmask[0]);
    nmask[1] = (s->netmask[0]) >> 8;
    nmask[2] = (s->netmask[1]);
    nmask[3] = (s->netmask[1]) >> 8;

    gwip[0] = (s->default_router[0]);
    gwip[1] = (s->default_router[0]) >> 8;
    gwip[2] = (s->default_router[1]);
    gwip[3] = (s->default_router[1]) >> 8;


    
    uip_setethaddr(mac);

    // set ip
    uip_ipaddr(&addr, myip[0], myip[1], myip[2], myip[3]);
    uip_sethostaddr(&addr);
    
    // set netmask
    uip_ipaddr(&addr,nmask[0], nmask[1], nmask[2], nmask[3]);
    uip_setnetmask(&addr);

 
    uip_ipaddr(&addr,gwip[0], gwip[1], gwip[2], gwip[3]);
    uip_setdraddr(&addr);




}

void resolv_found(char *name, u16_t *ipaddr)
{
    webserver=ipaddr;
	#if defined PORT_APP_MAPPER
  resolv_running = 0; 
#endif
}


