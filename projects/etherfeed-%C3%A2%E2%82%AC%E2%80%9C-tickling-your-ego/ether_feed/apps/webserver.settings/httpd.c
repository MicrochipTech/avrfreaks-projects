// the file was modified to use program memory to store data
// also CGI functionality was enhanced to allow parsing url parameters
//  Vladimir S. Fonov ~ vladimir.fonov <at> gmail.com
// based on uIP webserver

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
#include "httpd.h"
#include "httpd-fs.h"
#include "httpd-cgi.h"
#include "http-strings.h"

#include <string.h>
#include <avr/pgmspace.h>

#define STATE_WAITING 0
#define STATE_OUTPUT  1
#define STATE_UNUSED  3

#define ISO_nl       0x0a
#define ISO_space    0x20
#define ISO_bang     0x21
#define ISO_percent  0x25
#define ISO_period   0x2e
#define ISO_slash    0x2f
#define ISO_colon    0x3a
#define ISO_question 0x3f

static uint16_t generate_part_of_file(void *state)
{
  struct httpd_state *s = (struct httpd_state *)state;

  if(s->file.len > uip_mss()) {
    s->len = uip_mss();

  } else {
    s->len = s->file.len;

  }
  memcpy_P(uip_appdata, (PGM_VOID_P)s->file.data, s->len);
  
  return s->len;
}


static
PT_THREAD(send_file(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  
  do {
    PSOCK_GENERATOR_SEND(&s->sout, generate_part_of_file, s);
    s->file.len -= s->len;
    s->file.data += s->len;
  } while(s->file.len > 0);
      
  PSOCK_END(&s->sout);
}


static PT_THREAD(send_part_of_file(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_SEND_P(&s->sout, s->file.data, s->len);
  
  PSOCK_END(&s->sout);
}


static void
next_scriptstate(struct httpd_state *s)
{
  PGM_P p;
  p = strchr_P(s->scriptptr, ISO_nl) + 1;
  s->scriptlen -= (unsigned short)(p - s->scriptptr);
  s->scriptptr = p;
}

static PT_THREAD(handle_script(struct httpd_state *s))
{
  char *ptr;
  
  PT_BEGIN(&s->scriptpt);


  while(s->file.len > 0) {

    /* Check if we should start executing a script. */
    if(pgm_read_byte_near(s->file.data) == ISO_percent &&
       pgm_read_byte_near(s->file.data + 1) == ISO_bang) {
      s->scriptptr = s->file.data + 3;
      s->scriptlen = s->file.len - 3;

      if(pgm_read_byte_near(s->scriptptr - 1) == ISO_colon) {

				httpd_fs_open_P(s->scriptptr + 1, &s->file);
				PT_WAIT_THREAD(&s->scriptpt, 
           send_file(s));

      } else {
					PT_WAIT_THREAD(&s->scriptpt, 
           httpd_cgi_P(s->scriptptr)(s, s->scriptptr));

      }
      next_scriptstate(s);
      
      /* The script is over, so we reset the pointers and continue
    	 sending the rest of the file. */
      s->file.data = s->scriptptr;
      s->file.len = s->scriptlen;
    } else {
      /* See if we find the start of script marker in the block of HTML
	        to be sent. */

      if(s->file.len > uip_mss()) {
	      s->len = uip_mss();
      } else {
	      s->len = s->file.len;
      }

      if(pgm_read_byte_near(s->file.data) == ISO_percent) {
	      ptr = strchr_P(s->file.data + 1, ISO_percent);

      } else {
	      ptr = strchr_P(s->file.data, ISO_percent);

      }

      if(ptr != NULL &&
      	 ptr != s->file.data) {
        	s->len = (int)(ptr - s->file.data);
        	if(s->len >= uip_mss()) {
        	  s->len = uip_mss();
      	}
      }

      PT_WAIT_THREAD(&s->scriptpt, send_part_of_file(s));

      s->file.data += s->len;
      s->file.len -= s->len;
      
    }
  }
  
  PT_END(&s->scriptpt);
}


static PT_THREAD(send_headers(struct httpd_state *s, prog_char *statushdr))
{
  char *ptr;

  PSOCK_BEGIN(&s->sout);

  PSOCK_SEND_PSTR(&s->sout, statushdr);

  ptr = strrchr(s->filename, ISO_period);
  if(ptr == NULL) {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_binary);

  } else if(strncmp_P( ptr,http_html, 5) == 0 ||
	    strncmp_P( ptr,http_shtml, 6) == 0) {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_html);

  } else if(strncmp_P(ptr,http_css,  4) == 0) {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_css);

  } else if(strncmp_P(ptr,http_png, 4) == 0) {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_png);

  } else if(strncmp_P( ptr, http_gif, 4) == 0) {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_gif);

  } else if(strncmp_P( ptr, http_jpg, 4) == 0) {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_jpg);

  } else {
    PSOCK_SEND_PSTR(&s->sout, http_content_type_plain);

  }
  PSOCK_END(&s->sout);
}


static PT_THREAD(handle_output(struct httpd_state *s))
{
  char *ptr;
  
  PT_BEGIN(&s->outputpt);
 
  if(!httpd_fs_open(s->filename, &s->file)) {

    httpd_fs_open_P(http_404_html, &s->file);
    strcpy_P(s->filename, http_404_html);

    PT_WAIT_THREAD(&s->outputpt,
		   send_headers(s,
		   http_header_404));

    PT_WAIT_THREAD(&s->outputpt, send_file(s));

  } else {

    PT_WAIT_THREAD(&s->outputpt, send_headers(s, http_header_200));

    ptr = strchr(s->filename, ISO_period);

    if(ptr != NULL && strncmp_P(ptr, http_shtml, 6) == 0) {
      PT_INIT(&s->scriptpt);
      PT_WAIT_THREAD(&s->outputpt, handle_script(s));

    } else {
      PT_WAIT_THREAD(&s->outputpt, send_file(s));
    }
  }
  PSOCK_CLOSE(&s->sout);
  PT_END(&s->outputpt);
}


static PT_THREAD(handle_input(struct httpd_state *s))
{
  uint8_t i;
  PSOCK_BEGIN(&s->sin);

  PSOCK_READTO(&s->sin, ISO_space);
  
  if(strncmp_P(s->inputbuf, http_get, 4) != 0) {
    PSOCK_CLOSE_EXIT(&s->sin);
  }
  PSOCK_READTO(&s->sin, ISO_space);

  if(s->inputbuf[0] != ISO_slash) {
    PSOCK_CLOSE_EXIT(&s->sin);
  }

  if(s->inputbuf[1] == ISO_space) {

    strncpy_P(s->filename, http_index_shtml, sizeof(s->filename));
  } else {
    s->inputbuf[PSOCK_DATALEN(&s->sin) - 1] = 0;

    for(i=0 ; s->inputbuf[i]!=0 && s->inputbuf[i] != ISO_question ; i++);

    if( s->inputbuf[i] == ISO_question ) 
    {
      s->inputbuf[i]=0;
      strncpy(s->param,&s->inputbuf[i+1],sizeof(s->param));
    }
    else
      s->param[0]=0;

    strncpy(s->filename, &s->inputbuf[0], sizeof(s->filename));
  }

  /*  httpd_log_file(uip_conn->ripaddr, s->filename);*/
  
  s->state = STATE_OUTPUT;

  while(1) {
    PSOCK_READTO(&s->sin, ISO_nl);

    if(strncmp_P(s->inputbuf, http_referer, 8) == 0) {
      s->inputbuf[PSOCK_DATALEN(&s->sin) - 2] = 0;
      /*      httpd_log(&s->inputbuf[9]);*/
    }
  }
  
  PSOCK_END(&s->sin);
}


static void handle_connection(struct httpd_state *s)
{
  handle_input(s);
  if(s->state == STATE_OUTPUT) {
    handle_output(s);
  }
}
#if PORT_APP_MAPPER
int8_t alloc_state(void)
{
	int8_t found_state = -1;
	uint8_t index = 0;
	while (index < UIP_CONF_MAX_CONNECTIONS)
	{
		if (httpd_state_list[index].state == STATE_UNUSED)
		{
			found_state = index;
			break;
		}
		index++;
    }
								
	return (found_state);
}
#endif

void
httpd_appcall(void)
{
#if PORT_APP_MAPPER
	struct httpd_state *s;
#else
	struct httpd_state *s = (struct httpd_state *)&(uip_conn->appstate);
#endif


	if(uip_closed() || uip_aborted() || uip_timedout()) {
#if PORT_APP_MAPPER
//sendString("uip close/abort/timeout\r\n");
		if (uip_conn->appstate != -1)
		{
//sendString("uip close/abort/timeout - cleanup\r\n");
			httpd_state_list[((int8_t)uip_conn->appstate)].state = STATE_UNUSED;
			uip_conn->appstate = -1;
		}
#endif
	} else if(uip_connected()) {
#if PORT_APP_MAPPER
		if ((uip_conn->appstate = alloc_state()) == -1)
		{
			// we are out of state space.  close the connection
			// hope the client tries back again
//sendString("Out of http stats\r\n");
			uip_abort();
			return;
		}
		// set the app state
		s = &(httpd_state_list[((int8_t)uip_conn->appstate)]);
#endif
		PSOCK_INIT(&s->sin, s->inputbuf, sizeof(s->inputbuf) - 1);
		PSOCK_INIT(&s->sout, s->inputbuf, sizeof(s->inputbuf) - 1);
		PT_INIT(&s->outputpt);
		s->state = STATE_WAITING;
	    /*    timer_set(&s->timer, CLOCK_SECOND * 100);*/
		s->timer = 0;
		handle_connection(s);
#if PORT_APP_MAPPER
	} else if (uip_conn->appstate != -1) {
		s = &(httpd_state_list[((int8_t)uip_conn->appstate)]);
#else
	} else if(s != NULL) {
#endif
		if(uip_poll()) {
			++s->timer;
			// if the client just hasn't sent
			// anything in a while we end up here
			// this is where we can clean up dead connections.
			// 20 seems like a long time - if(s->timer >= 20) {
			// 16 seems to be a good number wtih 5 status
			if(s->timer >= 16) {
#if PORT_APP_MAPPER
				if (uip_conn->appstate != -1)
				{
					httpd_state_list[((int8_t)uip_conn->appstate)].state = STATE_UNUSED;
					uip_conn->appstate = -1;
				}
#endif
				uip_abort();
			}
    	} else {
			s->timer = 0;
		}
		handle_connection(s);
	} else {
#if PORT_APP_MAPPER
		if (uip_conn->appstate != -1)
		{
			httpd_state_list[((int8_t)uip_conn->appstate)].state = STATE_UNUSED;
			uip_conn->appstate = -1;
		}
#endif
		uip_abort();
	}
}



void httpd_init(void)
{
  #if PORT_APP_MAPPER
	uint8_t index = 0;
	while (index < HTTPD_MAX_CONNECTIONS)
	{
		httpd_state_list[index].state = STATE_UNUSED;
		index++;
	}
#endif
	uip_listen(HTONS(80));
  uip_listen(HTONS(80));
}
