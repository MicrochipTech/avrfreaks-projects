#include "uip.h"
#include "uiplib.h"
#include "timer.h"
#include "webclient.h"
#include "config.h"
//#include "ds18x20.h" //To get FRACCONV value
#include "resolv.h"
#include "lcd.h"

#include <avr/pgmspace.h>

#include <string.h>



uip_ipaddr_t ipaddr;
extern volatile uint16_t tickS;
uint16_t timer_webclient;


/*void webclient_init(void) {
   if (webserver == NULL)
   {
//   uip_ipaddr_t ipaddr;
   
   	webserver = resolv_lookup(hostname);
	if (webserver == NULL)
        {
       //    printf("Request");
	   
	    resolv_query(hostname);
        }
   }
} */

void webclient_connect(void) {
    if (tickDiffS(timer_webclient) > 140)
    {
       // printf("No reply to webclient recieved for 2minutes (%u -- %u -- %u), resetting...",tickDiffS(timer_webclient),timer_webclient,tickS);
	while (true); //watchdog will handle reset, but we cant hard reset enc unless we have the reset line...
    }
  // printf("\r\n");
  // printf(" Connect");

 /*  if (webserver == NULL)
   {
   	
	
   } */ 

   if (webserver != NULL)
   {
   //  printf("Connecting ... ");
     uip_connect((uip_ipaddr_t*)webserver, HTONS(80));

   //  printf("Connect sent");
   }
}

void webclient_app(void) {
   if (uip_connected())
   {
     //  printf("Setting timer for webclient to %u \r\n", tickS);
	timer_webclient = tickS;
   }
  

   if(uip_connected() || uip_rexmit()) {
	  char *ptr = tempbuf;
	
	
	
  
	  sprintf(ptr, "GET /ipn/request.php \r\n");
	
    ptr += strlen(ptr);
	sprintf_P(ptr, PSTR("HTTP/1.0\r\nHost: irdroid.com\r\nUser-Agent: Stokerbot NG %u.%u\r\n\r\n"),SBNG_VERSION_MAJOR,SBNG_VERSION_MINOR);
 		
	  // printf("Requesting : %s \r\n", tempbuf);	
	   uip_send(tempbuf, strlen(tempbuf));
	 
		return;
	
	
  

 }
   

   

 if(uip_newdata())
   {
   
//	//char data[10];
	uint16_t len = uip_datalen();
	// ((char *)uip_appdata)[len] = 0;
	lcd_clrscr();
	
	//printf("%u", len);
    //NULL terminate the incoming data buffer
	//strcpy (data,(char*)uip_appdata);
    
//	
//	lcd_putstring((char*)uip_appdata);
    *((char*)uip_appdata + uip_datalen()) = '\0';
	 lcd_puts((char*)uip_appdata);
   }   
   
}
