
/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
/* eeprom routines
   * treats the eeprom area as an ascii text file,
   * each record is seperated by a carridge return
   * it's possible to add to the end of file or overwrite lines
   * in the file and optionally reterminate the file 
   * efile allows storage of strings into eeprom
   * the strings could be calibration data or commands to help initialise software
   * on the avr, efile_exec() interperets them with the same command processor 
   * as the uart comms uses
*/
#include <avr/io.h>
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/eeprom.h>
#include "fail.h"
#include "uart.h"
#include "command.h"

uint16_t eeptr;

/* this is our eprom operating system 
   routines can be used for holding parameters 
   and startup initialisation */

void efile_cmd(char **z)
{
   char *dummy;
   char y=**z;
   (*z)++;
   switch (y)
   {
      case 'p':
	 efile_print();
	 break;
      case 'w':
	 efile_write((*z),1);
	 break;

#ifdef EFILE_FIND
      case 'g':
	 {
	    int16_t tp=strtol((*z),0,10);
	    efile_seek(tp);
	 }
	 break;
      case 'f':
	 efile_find((*z),1);
	 break;
      case 's':
	 {
	    efile_show((*z),strlen(*z));
	 }
	 break;
#endif
      case 'a':
	 efile_write((*z),0);
	 break;
      case 'x':
	 efile_exec();
	 break;
   }
}

#ifdef EFILE_FIND

/* set the write pointer to the chosen line in the efile
   pos is line to seek to in the eeprom
   returns 1 on success
*/
char efile_seek(uint8_t pos)
{
   if(!eeprom_is_ready())return 0;
   unsigned char q;
   uint8_t z=0;
   if(pos==0){
		eeptr=0;
		return 1;
	     }
   for(eeptr=0;eeptr<E2END;eeptr++)
   {
      q=eeprom_rb(eeptr);
      if(q>127 || q==0){
			  return 0;
		       }
      if(q=='\n')if(++z>=pos){
				eeptr++;
				return 1;
			     }
   }
   return 0;
}


/* print the las line matching cmd
   usefull for a simple eeprom variable show command
*/
char efile_show(uint8_t *cmd,uint8_t len)
{
   if(efile_find(cmd,len)==0)return 0;
   for(;eeptr<E2END;eeptr++)
   {
      syncwait();
      unsigned char q=eeprom_rb(eeptr);
      if(q=='\n' || q==0 || q>127){
				     break;
				  }
      if(isprint(q)){
		       putc(q);
		    }
   }
   efile_seek(255);
   return 1;
}

/* set write pointer to last occurance of the record starting
   with cmd (returns 1) or if not to the end of file (return 0),
   , attempt to match upto len characters 
   this is perfect for usage as a filename type system for varbles or parameters
   as they can be accessed by a text key and modified
   allowing us to maintain state and settings after power down
*/
char efile_find(uint8_t *cmd,uint8_t len)
{
   //   puts("look for ");
   //   puts(cmd);
   //   putc(len+'0');
   int16_t last=-1;
   while(!eeprom_is_ready()){  };
   unsigned char q;
   uint8_t y=0;
   for(eeptr=0;;eeptr++)
   {
      q=eeprom_rb(eeptr);
      if(eeptr>=E2END) return 0;
      if(q>127 || q==0){
			  break;
		       }
      if(q=='\n' || eeptr==0){
				y++;  // keep track of which line in the file we are at
				uint16_t st=++eeptr;
				char x=0;
				do{
				     q=eeprom_rb(eeptr++);
				     if(eeptr>=E2END) return 0;
				     if(q>127 || q==0){
							 eeptr=E2END;
							 x=0;
							 break;
						      }
				     if(cmd[x]!=q)break;
				  }while(++x <len);
				if(x==len){
					     last=st;  // remember location of this match
					     // puts("match line=");
					     // put_i16(y);
					  }
			     }
   }
   if(last==-1)return 0;
   eeptr=last;  // set pointer to location of last match
   return 1;
}
#endif

/* append cmd at write point in efile
   *  insert cmd into eefile at present position 
   * mode is 0=insert or 1=insert and terminate file
   * returns 1 on success, 0 on failure
*/
char efile_write(uint8_t *cmd,char mode)
{
   char x=0;
   unsigned char y;
   while(!eeprom_is_ready()){  };
   if(eeptr+strlen(cmd) >=E2END )return 0;
   y=eeprom_rb(eeptr);
   if(y>127 || y==0){
		       mode=1;
		       eeprom_wb(eeptr++,'\n');
		    }
   while(cmd[x]!=0){
		      y=eeprom_rb(eeptr);
		      if(y>127 || y==0)mode=1;
		      eeprom_wb(eeptr++,cmd[x++]);
		   }
   if(mode==1){
		 eeprom_wb(eeptr,0);
		 return 1;
	      }
   do{
	unsigned char q=eeprom_rb(eeptr);
	if(q=='\n' || q>127 || q==0)return 1;
	eeprom_wb(eeptr,'.');  // write dots to fill remainder of this line
	eeptr++;
     }while(eeptr<E2END);
   return 1;
}

/* this executes all the  
   stored commands in the eeprom, and is intended for 
   usage as an initialisation routine.
   the commands start at the beginning of the eeprom
   each one is seperated with a \n character the 
   end of file is indicated by a non ascii character
   maximum command length is 40 characters 
*/
void  efile_exec()
{
   if(!eeprom_is_ready())return;
   eeptr=0;
   char cbuf[40];
   char x=0;
   for(eeptr=0;eeptr<E2END;eeptr++)
   {
      unsigned char q=cbuf[x++]=eeprom_rb(eeptr);
      if(x>=40)break;  // safety check in case eeprom contains junk
      if(q==0 || q>127 || q=='\n'){
				     if(x>0)process(&cbuf);
				     x=0;
				     if(q==0 || q>127 )break;
				     continue;
				  }
   }
}

/* print out the entire eefile and return the length
   also set the write pointer to the end of the file 
*/
uint16_t efile_print()
{
   if(!eeprom_is_ready())return 0;
   eeptr=0;
   char x=0;
   unsigned char line=0;
   put_i16(line);
   putc('=');
   for(eeptr=0;eeptr<E2END;eeptr++)
   {
      syncwait();
      unsigned char q=eeprom_rb(eeptr);
      if(q>127 || q==0){
			  break;
		       }
      if(q=='\n'){
		    line++;
		    put_cr();
		    put_i16(line);
		    putc('=');
		    continue;
		 }
      if(isprint(q)){
		       putc(q);
		    }
      else
      {
	 putc('?');
      }
   }
   put_cr();
   equal("e2u");
   put_i16(eeptr);
   putc('/');
   put_i16(E2END);
   put_cr();
   return eeptr;
}

