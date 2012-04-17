 /*
 * Copyright (c) 2003, Adam Dunkels.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: shell.c,v 1.1 2006/06/07 09:43:54 adam Exp $
 *
 */
#include "global-conf.h"

#if TELNETD_TIME_CMD
#include "ds1302.h"
#endif

#ifdef NETWORK_CMD
#include "net_conf.h"
#endif 

#ifdef UDPDS_CMD
#include "udpds.h"

#endif

#include "shell.h"

#include <string.h>

struct ptentry {
  char *commandstr;
  void (* pfunc)(char *str);
};

#ifndef SHELL_PROMPT
#define SHELL_PROMPT "uIP 1.0> "
#endif

/*---------------------------------------------------------------------------*/
static void
parse(register char *str, struct ptentry *t)
{
  struct ptentry *p;
  for(p = t; p->commandstr != NULL; ++p) {
    if(strncmp(p->commandstr, str, strlen(p->commandstr)) == 0) {
      break;
    }
  }

  p->pfunc(str);
}
/*---------------------------------------------------------------------------*/
#if 0  // inttostr not used anywhere
static void
inttostr(register char *str, unsigned int i)
{
  str[0] = '0' + i / 100;
  if(str[0] == '0') {
    str[0] = ' ';
  }
  str[1] = '0' + (i / 10) % 10;
  if(str[0] == ' ' && str[1] == '0') {
    str[1] = ' ';
  }
  str[2] = '0' + i % 10;
  str[3] = ' ';
  str[4] = 0;
}
#endif  // inttostr not used anywhere

#ifdef TELNETD_TIME_CMD
static void
settime(char *str)
{
	uint8_t timestore_write[8] = {0x00,0x38,0x10,0x10,0x11,0x04,0x10,0};
	send_time_to_rtc(timestore_write);
	shell_output("time set", "");
}
/*---------------------------------------------------------------------------*/
static void
isotime(char *str)
{
  uint8_t timestore_read[7];
  uint8_t iso_timestore[16];

  read_time(timestore_read);
  iso_time(timestore_read, iso_timestore);
/*  iso_timestore[0] = 'a' + timestore_read[0];
  iso_timestore[1] = 'a' + timestore_read[1];
  iso_timestore[2] = 'a' + timestore_read[2];
  iso_timestore[3] = 'a' + timestore_read[3];
  iso_timestore[4] = '\0';
*/
  shell_output(iso_timestore, "");
}
#endif

/*---------------------------------------------------------------------------*/
#ifdef NETWORK_CMD
#define DSTR_BUF 25
static void
shell_network(char *str)
{
	char *pos = str + sizeof("network");
	uint8_t is_error = 0;
	char dstr[DSTR_BUF];


	if (strncmp_P(pos, PSTR("set "), 4) == 0)
	{
        pos += 4;
		if (strncmp_P(pos, PSTR("ip "), 3) == 0)
		{
        	pos += 3;
			is_error = net_conf_set_ip_string(pos);
		}
		else if (strncmp_P(pos, PSTR("gw "), 3) == 0)
		{
        	pos += 3;
			is_error = net_conf_set_gw_string(pos);
		}
		else if (strncmp_P(pos, PSTR("nm "), 3) == 0)
		{
        	pos += 3;
			is_error = net_conf_set_nm_string(pos);
		}
		else if (strncmp_P(pos, PSTR("dhcp "), 5) == 0)
		{
        	pos += 5;
			is_error = net_conf_set_dhcpc_string(pos);
		}
		else
		{
			shell_output("unknown set operation: ", pos);
		}
	}
	else if (strncmp_P(pos, PSTR("show"), 4) == 0)
	{
        pos += 5;
		net_conf_get_mac_string(dstr, DSTR_BUF);
		shell_output("MAC: ", dstr);
		net_conf_get_ip_string(dstr, DSTR_BUF);
		shell_output("IP: ", dstr);
		net_conf_get_nm_string(dstr, DSTR_BUF);
		shell_output("NM: ", dstr);
		net_conf_get_gw_string(dstr, DSTR_BUF);
		shell_output("GW: ", dstr);
	}
	else if (strncmp_P(pos, PSTR("load"), 4) == 0)
	{
		net_conf_load();
	}
	else if (strncmp_P(pos, PSTR("save"), 4) == 0)
	{
		net_conf_save();
	}
	else
	{
		shell_output_P(PSTR("options: show, set, load, save"),PSTR(""));
	}
}
#endif


#ifdef UDPDS_CMD
static void
shell_udpds(char *str)
{
	char *pos = str + sizeof("udpds");
  
	if (strncmp_P(pos, PSTR("set "), 4) == 0)
	{
	}
	else if (strncmp_P(pos, PSTR("show"), 4) == 0)
	{
	}
	else if (strncmp_P(pos, PSTR("enable"), 6) == 0)
	{

	}
	else if (strncmp_P(pos, PSTR("disable"), 7) == 0)
	{
	}
}
#endif

/*---------------------------------------------------------------------------*/
static void
help(char *str)
{
  // TEXT HERE CAN ONLY BE 40 chars / output! based on telnetd.h 
  shell_output_P(PSTR("Available commands:"), PSTR(""));
#ifdef TELNETD_TIME_CMD
  shell_output_P(PSTR("isotime - sys time, iso format"), PSTR(""));
  shell_output_P(PSTR("settime - set sys time"), PSTR(""));
  shell_output_P(PSTR("gettime - get sys time"), PSTR(""));
  shell_output_P(PSTR("rtcisotime - show RTC time, iso format"), PSTR(""));
  shell_output_P(PSTR("rtcsettime - set the current RTC time"), PSTR(""));
  shell_output_P(PSTR("rtcgettime - get the current RTC time"), PSTR(""));
  shell_output_P(PSTR("rtctosys   - copy RTC time to sys"), PSTR(""));
  shell_output_P(PSTR("systortc   - copy sys time to RTC"), PSTR(""));
#endif
#ifdef NETWORK_CMD
  shell_output_P(PSTR("network - get/set network settings"), PSTR(""));
#endif
#ifdef UDPDS_CMD
  shell_output_P(PSTR("udpds - set/enable/disable"),PSTR(""));
#endif
//  shell_output("stats   - show network statistics", "");
//  shell_output("conn    - show TCP connections", "");
  shell_output_P(PSTR("help, ? - show help"), PSTR(""));
  shell_output_P(PSTR("exit    - exit shell"), PSTR(""));
}
/*---------------------------------------------------------------------------*/
static void
unknown(char *str)
{
  if(strlen(str) > 0) {
    shell_output("Unknown command: ", str);
	*str = 0;
  }
}
/*---------------------------------------------------------------------------*/
static struct ptentry parsetab[] =
  {{"stats", help},
   {"conn", help},
   {"help", help},
   {"exit", shell_quit},
   {"?", help},
#ifdef TELNETD_TIME_CMD
   {"time", isotime},
   {"settime", settime},
#endif
#ifdef NETWORK_CMD
   {"network", shell_network},
#endif
#ifdef UDPDS_CMD
   {"udpds", shell_udpds},
#endif
   /* Default action */
   {NULL, unknown}};
/*---------------------------------------------------------------------------*/
void
shell_init(void)
{
}
/*---------------------------------------------------------------------------*/
void
shell_start(void)
{
  shell_output_P(PSTR("uIP command shell"), PSTR(""));
  shell_output_P(PSTR("Type '?' and return for help"), PSTR(""));
  shell_prompt(SHELL_PROMPT);
}
/*---------------------------------------------------------------------------*/
void
shell_input(char *cmd)
{
  parse(cmd, parsetab);
  shell_prompt(SHELL_PROMPT);
}
/*---------------------------------------------------------------------------*/
