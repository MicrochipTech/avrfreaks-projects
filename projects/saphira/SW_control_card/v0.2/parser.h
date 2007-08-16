/*

	 Parser functions (parser.h) for Atmega16 and WINAVR

	 Version 0.1ß		2007-06-05

	 Copyright (C) Andreas Kingbäck 2005 (andki@itn.liu.se)

   	 This program is free software; you can redistribute it and/or modify it
	 under the terms of the GNU General Public License as published by the
	 Free Software Foundation; either version 2 of the License, or
	 (at your option) any later version.

   	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	 See the GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License along with this program;
	 if not, write to the Free Software Foundation, Inc.,
	 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/


#ifndef _PARSER_H_
#define _PARSER_H_

#include <ioavr.h>

// Max string size

#define MAX_STR_SIZE 30

// Buffer Commands

#define  CM_RESET			    						0
#define  CM_GET_BUFFER_SIZE		    		1
#define	 CM_CLEAR_BUFFER		    			2
#define  CM_GET_BUFFER			    			3
#define  CM_GET_BUFFER_MAX_SIZE		    4

// Receive status

#define	ST_WAIT_FOR_CMD							10
#define ST_WAIT_FOR_DATA		   			11

// Motor Control commands

#define CM_LEFT_LOAD_POS                   20
#define CM_RIGHT_LOAD_POS                  21
#define CM_BOTH_LOAD_POS                   22
#define CM_LEFT_START_MOTION							 23
#define CM_RIGHT_START_MOTION							 24
#define CM_BOTH_START_MOTION							 25

// Return codes

#define REC_BUFFER_FULL			  210
#define REC_COMMAND_OK			  211
#define REC_COMMAND_ERROR		  212
#define REC_SEND_REPLY        213
#define REC_SEND_NO_REPLY     214
#define CM_OK                 254
#define CM_FAILED			        255

// Status codes

#define ROBOT_DISABLED          0
#define ROBOT_ACTIVATED         1


// Status codes

#define true  1
#define false 0

//unsigned char pa_cmd_buffer[MAX_STR_SIZE+1];
//unsigned char pa_cmd_pointer;
//unsigned char *pa_cmd_buf_pointer;

void init_parser(void);
void fifo_test(void);
void pa_exc_command(char *command);
void pa_insert_char(unsigned char data);


#endif		/* _PARSER_H_ */
