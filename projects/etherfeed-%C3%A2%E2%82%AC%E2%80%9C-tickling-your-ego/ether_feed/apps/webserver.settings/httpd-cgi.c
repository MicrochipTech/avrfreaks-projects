// the file was modified to use program memory to store data
// also CGI functionality was enhanced to allow parsing url parameters
//  Vladimir S. Fonov ~ vladimir.fonov <at> gmail.com

/*
 * Copyright (c) 2004, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: httpd.c,v 1.2 2006/06/11 21:46:38 adam Exp $
 */ 


#include "uip.h"
#include "uiplib.h"
#include "psock.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "httpd-fs.h"
#include "config.h"
#include "global-conf.h"

#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

HTTPD_CGI_CALL(cgi_hello,    "hello",    run_hello);
HTTPD_CGI_CALL(cgi_settings, "settings", run_settings);
HTTPD_CGI_CALL(cgi_welcome,  "welcome",  run_welcome);
static char hostname[20]="";

static const struct httpd_cgi_call *calls[] = 
{ 
    &cgi_hello,
		&cgi_settings, 
		&cgi_welcome,
    NULL 
};

uint8_t http_get_parameters_parse(char *par,uint8_t mx)
{
  uint8_t count=0;
	uint8_t i=0;
  for(;par[i]&&i<mx;i++)
  {
    if(par[i]=='=') 
    {
      count++;
      par[i]=0;
    } else if(par[i]=='&')
      par[i]=0;
  }
  return count;
}

char * http_get_parameter_name(char *par,uint8_t cnt,uint8_t mx)
{
  uint8_t i,j;
  cnt*=2;
  for(i=0,j=0;j<mx&&i<cnt;j++)
    if(!par[j]) i++;

  return j==mx?"":par+j;
}

char * http_get_parameter_value(char *par,uint8_t cnt,uint8_t mx)
{
  uint8_t i,j;
  cnt*=2;
  cnt++;
  for(i=0,j=0;j<mx&&i<cnt;j++)
    if(!par[j]) i++;

  return j==mx?"":par+j;
}

void http_url_decode(const char *in,char *out,uint8_t mx)
{
  uint8_t i,j;
	char tmp[3]={0,0,0};
  for(i=0,j=0;j<mx&&in[i];i++)
	{
  	if(in[i]=='%')
		{
			tmp[0]=in[++i];
			tmp[1]=in[++i];

			out[j++]=(char)strtol(tmp,NULL,16);
		} else 
			out[j++]=in[i];
	}
}


static PT_THREAD(nullfunction(struct httpd_state *s, PGM_P ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_SEND_PSTR(&s->sout,PSTR("Not Implemented"));
  PSOCK_END(&s->sout);
}

httpd_cgifunction httpd_cgi(char *name)
{
  const struct httpd_cgi_call **f;

#if DEBUG_SERIAL
	printf_P(PSTR("Calling CGI script:%s\r\n"),name);
#endif

  /* Find the matching name in the table, return the function. */
  for(f = calls; *f != NULL; ++f)
  {
    if(strncmp((*f)->name, name, strlen((*f)->name)) == 0) {
      return (*f)->function;
    }
  }

#if DEBUG_SERIAL
	printf_P(PSTR("Script not found\r\n"));
#endif

  return nullfunction;
}

httpd_cgifunction httpd_cgi_P(PGM_P name)
{
  const struct httpd_cgi_call **f;

#if DEBUG_SERIAL
	printf_P(PSTR("Calling CGI script from program memory\r\n"));
#endif

  /* Find the matching name in the table, return the function. */
  for(f = calls; *f != NULL; ++f)
  {
    if(strncmp_P((*f)->name, name, strlen((*f)->name)) == 0) {
      return (*f)->function;
    }
  }
#if DEBUG_SERIAL
	printf_P(PSTR("Script not found\r\n"));
#endif
  return nullfunction;
}
 
static PT_THREAD(run_hello(struct httpd_state *s, PGM_P ptr))
{
	//NOTE:local variables are not preserved during the calls to proto socket functins
	static char hello_name[20]="";
	static uint8_t pcount;
  PSOCK_BEGIN(&s->sout);
	//check if there are parameters passed 
  if(s->param[0] && (pcount=http_get_parameters_parse(s->param,sizeof(s->param)))>0)
	{
		static uint8_t i;
		//walk through parameters
		for(i=0;i<pcount;i++)
		{
		  static char *pname,*pval;
			pname=http_get_parameter_name(s->param,i,sizeof(s->param));
			pval =http_get_parameter_value(s->param,i,sizeof(s->param));
			if(!strcmp_P(pname,PSTR("name")))
			{
				strncpy(hello_name,pval,sizeof(hello_name)-1);
				break;//we found what we were looking for
			}
		}
		if (strcmp (hello_name,"boot") != 0){
		
		PSOCK_SEND_PSTR(&s->sout,PSTR("Entering bootloader mode..."));
		//PSOCK_SEND_STR(&s->sout,hello_name);
		//PSOCK_SEND_PSTR(&s->sout,PSTR("<br>"));
		asm volatile("jmp 0x0000");
		} else {
		PSOCK_SEND_PSTR(&s->sout,PSTR("Wrong key, please try again!"));
		}
  }

  PSOCK_SEND_PSTR(&s->sout,PSTR("\
<form action=\"/hello.shtml\" method=\"get\" bgcolor=\"#808080\">\
Enter your name: <input type=\"text\" name=\"name\" size=\"10\" value=\""));

	PSOCK_SEND_STR(&s->sout,hello_name);

  PSOCK_SEND_PSTR(&s->sout,PSTR("\
\"/><br><input type = \"submit\" value=\"Send\" size=\"8\"> <input type = \"reset\"  value=\"cancel\" size=\"8\">"));
  PSOCK_END(&s->sout);
}

static uint8_t decode_ip(char *in,uint8_t *out)
{
	uint8_t i;
	char tmp[20];
	strncpy(tmp,in,sizeof(tmp));
	char *dig;
	dig=strtok(tmp,".");

	for(i=0 ; i<4 && dig ;i++,dig=strtok(NULL,"."))
		out[i]=(uint8_t)strtoul(dig,NULL,10);

	return i;
}




//static uint8_t decode_mac(char *in,uint8_t *out)
//{
//	char tmp[20];
//	strncpy(tmp,in,sizeof(tmp));
//
//	return *tmp;
	

	
//}


static PT_THREAD(run_settings(struct httpd_state *s, PGM_P ptr))
{
	
	//NOTE:local variables are not preserved during the calls to proto socket functins
	static uint8_t pcount;
  PSOCK_BEGIN(&s->sout);
	//check if there are parameters passed 
  if(s->param[0] && (pcount=http_get_parameters_parse(s->param,sizeof(s->param)))>0)
	{

		static uint8_t i;
		static uint8_t result;
		result=(pcount==5 || pcount==6 ); //correct number of parameters
		_enable_dhcp=0;
		//walk through parameters
		for(i=0;i<pcount;i++)
		{
		  static char *pname,*pval;
			pname=http_get_parameter_name(s->param,i,sizeof(s->param));
			pval =http_get_parameter_value(s->param,i,sizeof(s->param));

			if(!strcmp_P(pname,PSTR("mac")) )
			{
			
				result = result && strcpy(hostname,pval);  // Changed //
			} 

             else if(!strcmp_P(pname,PSTR("dhcp")) )
			{
				_enable_dhcp=(pval[0]=='1');
			} else if(!strcmp_P(pname,PSTR("timer")) )
			{
			    timer_update=atoi(pval);
				result = result && timer_update;  // Changed //
			}else if(!strcmp_P(pname,PSTR("ip")) )
			{

				result = result && (decode_ip(pval,myip)==4);
			} else if(!strcmp_P(pname,PSTR("netmask")) )
			{
				result = result && (decode_ip(pval,netmask)==4);
			} else if(!strcmp_P(pname,PSTR("gw")) )
			{
				result = result && (decode_ip(pval,gwip)==4);
			} else {

#if DEBUG_SERIAL
				printf_P(PSTR("Unknown:%s = %s\r\n"),pname,pval);
#endif
				result=0; //unknown parameter, probably an error!
			}
		}
		if(result) {
			eeprom_write_byte((void *)350,_enable_dhcp);
			eeprom_write_block (hostname,(void *)300,sizeof(hostname));
         //   eeprom_write_block ((void*)timer_update,(void *)39,sizeof(timer_update)); //Changed
		 if (timer_update >= 10){
		 eeprom_write_word ((uint16_t *)39, timer_update) ;
		 }else if (timer_update < 10) {
		 timer_update = 10;
		 eeprom_write_word ((uint16_t *)39, timer_update) ;
		 }
			eeprom_write_block (myip, (void *)11, 4);
			eeprom_write_block (netmask,(void *)19,4);
			eeprom_write_block (gwip, (void *)15, 4);
			
        
        
			
			
			
			PSOCK_SEND_PSTR(&s->sout,PSTR("<b>Parameters Accepted, cycle power to make active!</b>"));
		} else {
			PSOCK_SEND_PSTR(&s->sout,PSTR("<b>Parameters incorrect!</b>"));
		}
  }

	//_enable_dhcp=eeprom_read_byte(&ee_enable_dhcp);;
	
	eeprom_read_block (hostname,(void *)300,20);
	//eeprom_read_block ((void *)mymac,(void *)300,20); //Changed
	//eeprom_read_block ((void*)timer_update,(void*)39,1);
	timer_update = eeprom_read_word((uint16_t*)39); 
	eeprom_read_block ((void *)myip, (void *)11, 4);
	
	eeprom_read_block ((void *)netmask,(void *)19,4);
	eeprom_read_block ((void *)gwip, (void *)15, 4);

	char temp[30];
  PSOCK_SEND_PSTR(&s->sout,PSTR("\
<form action=\"/index.shtml\" method=\"get\" bgcolor=\"#808080\">\  
<table>\
<tr><td>Domain:</td><td><input type=\"text\" name=\"mac\" size=\"18\" maxlength=\"18\" value=\""));



	snprintf_P(temp,sizeof(temp),PSTR("%s"),
	 hostname);

	PSOCK_SEND_STR(&s->sout,temp);
	


	PSOCK_SEND_PSTR(&s->sout,PSTR("\"></td></tr>\
<tr><td>DHCP:</td><td><input type=\"checkbox\" name=\"dhcp\" value=\"1\""));

	if(_enable_dhcp) 
		PSOCK_SEND_PSTR(&s->sout,PSTR("CHECKED"));
		
	




PSOCK_SEND_PSTR(&s->sout,PSTR("/></td></tr>\
<tr><td>Timer:</td><td><input type=\"text\" name=\"timer\" size=\"15\" maxlength=\"15\" value=\""));

	snprintf_P(temp,sizeof(temp),PSTR("%d"),
	 timer_update);
	PSOCK_SEND_STR(&s->sout,temp);







	

	PSOCK_SEND_PSTR(&s->sout,PSTR("\"></td></tr>\
<tr><td>IP:</td><td><input type=\"text\" name=\"ip\" size=\"15\" maxlength=\"15\" value=\""));

	snprintf_P(temp,sizeof(temp),PSTR("%d.%d.%d.%d"),
	 (int)myip[0],(int)myip[1],(int)myip[2],(int)myip[3]);

	PSOCK_SEND_STR(&s->sout,temp);

PSOCK_SEND_PSTR(&s->sout,PSTR("\"></td></tr>\
<tr><td>Netmask:</td><td><input type=\"text\" name=\"netmask\" size=\"15\" maxlength=\"15\" value=\""));

	snprintf_P(temp,sizeof(temp),PSTR("%d.%d.%d.%d"),
	 (int)netmask[0],(int)netmask[1],(int)netmask[2],(int)netmask[3]);

	PSOCK_SEND_STR(&s->sout,temp);

	PSOCK_SEND_PSTR(&s->sout,PSTR("\"/></td></tr>\
<tr><td>Gateway:</td><td><input type=\"text\" name=\"gw\" size=\"15\" maxlength=\"15\" value=\""));

	snprintf_P(temp,sizeof(temp),PSTR("%d.%d.%d.%d"),
	 (int)gwip[0],(int)gwip[1],(int)gwip[2],(int)gwip[3]);

	PSOCK_SEND_STR(&s->sout,temp);

	PSOCK_SEND_PSTR(&s->sout,PSTR("\"/></td></tr>\
<tr align=\"justify\"><td colspan=2><INPUT TYPE=submit VALUE=\"Submit\">  <INPUT TYPE=reset VALUE=\"Reset\"></td></tr>\
</table></form>"));

  PSOCK_END(&s->sout);
}

static PT_THREAD(run_welcome(struct httpd_state *s, PGM_P ptr))
{
	//NOTE:local variables are not preserved during the calls to proto socket functins
//	char temp[30];
//	sprintf_P(temp,PSTR("System time:%u ticks"),(unsigned int)clock_time());

  //PSOCK_BEGIN(&s->sout);
//	PSOCK_SEND_STR(&s->sout,temp);
 // PSOCK_END(&s->sout);
 return 0;
}
