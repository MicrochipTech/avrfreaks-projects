/*
 
	file:	delay.h
  	desc:	Simple delays using frame synchronization.
 
  	note:	The driver must be running.
 
	author: Jaromir Dvorak (md@unicode.cz)
 
*/


#ifndef _DELAY_H_
#define _DELAY_H_


#define wait_frames(n)    ({ register unsigned int __c = n;    while(--__c) sync(); })
#define wait_dseconds(n)  ({ register unsigned int __c = n*5;  while(--__c) sync(); })
#define wait_seconds(n)   ({ register unsigned int __c = n*50; while(--__c) sync(); })


#endif
