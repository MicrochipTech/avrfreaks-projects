#include "main.h"
#include "spi.h"
#include "timers.h"

#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/wdt.h>

#define RETRY 1000

byte mmc_command (byte *dat,word count,byte min_waitcycles,byte max_waitcycles)
{	void mmc_resume();
	volatile byte retval; 
	byte counter;
	
	
//	spi_irq_disable();
//	mmc_resume();

	spi_irq_enable();
	while (spi_master_transfer_block(1,dat,count));
	while (!block_complete);
	spi_irq_disable();
	counter=0;
	while ((counter++)<min_waitcycles)
		spi_master_transfer_byte (0xFF);
	counter=0;
	do
	{	retval=spi_master_transfer_byte(0xFF);
	}
	while ((retval==0xFF)&&(++counter<max_waitcycles));
	mmc_resume();
	return retval;
}



byte mmc_init (byte pol, byte pha)
{	byte error,del,dat[6],timer;
	word counter;
#ifdef DEBUG
	//char outstring[20];
#endif


	spi_init (pol,pha,1,0,1,2);
			
	for (counter=0;counter<100;counter++)
		spi_master_transfer_byte(0xFF);
	
	dat[0]=0x40;
	dat[1]=0x00;
	dat[2]=0x00;
	dat[3]=0x00;
	dat[4]=0x00;
	dat[5]=0x95;
	
	timer=start_timer(200);			//set timeout 200ms
	spi_select();
	do
	{	error=(mmc_command (dat,sizeof(dat),0,100)!=0x01);
	}
	while (error&&(!timer_stopped(timer)));
	stop_timer(timer);
	spi_deselect();
	if (!error)
	{	timer=start_timer(300);		//set timeout 300ms
		spi_init (pol,pha,1,0,1,0);	//switch SPI to 4MHz
		spi_select();
		dat[0]=0x41;
		dat[5]=0xFF;
		do
		{			
			error=(mmc_command (dat,sizeof(dat),0,100)!=0);
		}
		while (error&&(!timer_stopped(timer)));
		stop_timer(timer);
		spi_deselect();
	}

	return error;
}	



void change_byteorder(byte *dat)
{	byte aux;
	aux=*dat;
	*dat=*(dat+3);
	*(dat+3)=aux;
	aux=*(dat+1);
	*(dat+1)=*(dat+2);
	*(dat+2)=aux;
}



void mmc_resume()
{	byte counter;
	spi_irq_disable();
	for (counter=0;counter<8;counter++)
		spi_master_transfer_byte(0xFF);
}



byte mmc_read_block (byte* buff,unsigned long int blocknumber)
{	byte error,busy,timeout;
	byte dat[6];
	word counter;
	
	blocknumber=blocknumber<<9;		//calculate physical adress of block;
	memcpy (&dat[1],&blocknumber,sizeof(blocknumber));
	
	dat[0]=0x40|17;
	dat[5]=0xFF;		//read single block
	
	//change byte order of adress
	
	while (!block_complete);
	wdt_reset();
	spi_select();
	mmc_resume();
	timeout=start_timer(300);
	do
	{	
		busy=(spi_master_transfer_byte(0xFF)==0);
	}
	while ((!timer_stopped(timeout))&&busy);
	stop_timer(timeout);
	mmc_resume();
	if (busy)
	{	spi_deselect();
		error=1;
	}
	else
		error=0;


	
	
	if (!error)
	{	change_byteorder (&dat[1]);
		timeout=start_timer(100);
		do
		{	error=mmc_command(dat,sizeof(dat),0,200);	
		}
		while ((error)&&(!timer_stopped(timeout)));
		stop_timer(timeout);
	}
	wdt_reset();
	if (!error)
	{	
		timeout=start_timer(100);
		do 
		{	error=(spi_master_transfer_byte (0xFF)!=0xFE);
		}
		while (error&&(!timer_stopped(timeout)));
		stop_timer(timeout);
		if (!error)
		{	
			spi_irq_enable();		
			spi_master_transfer_block (0,buff,512);
		}
	}		
	return error;
}

/*

byte mmc_write_wait()
{	byte error,timeout,aux;
	
	timeout=start_timer(100);	//timeout 100ms
	
	PORTD&=0xFE;		//switch on LED #0
	do
	{	error=!block_complete;
		if(!error)
		{	spi_irq_disable();
			aux=spi_master_transfer_byte(0xFF);	
			error=((aux&0x1F)!=0x05);
//			if (!error)
//				error=!(aux&0x04);
		}
	}
	while (error&&(!timer_stopped(timeout)));
	stop_timer(timeout);
	for (aux=0;aux<16;aux++)
	mmc_resume();	
	spi_deselect();
	mmc_resume();
	PORTD|=0x01;		//switch off LED #0
	return error;
}



byte mmc_write_block (byte* buff,unsigned long int blocknumber)
{	byte error,busy,timeout,errcount=0;
	byte dat[6];
	word counter;
	
	blocknumber=blocknumber<<9;		//calculate physical adress of block;
	memcpy (&dat[1],&blocknumber,sizeof(blocknumber));
	
	dat[0]=0x40|24;
	dat[5]=0xFF;		//write single block
	
	//change byte order of adress
	change_byteorder (&dat[1]);

	do
	{
		while(!block_complete);
		wdt_reset();
	
		timeout=start_timer(100);	//timeout 500ms
		spi_select();
		mmc_resume();
		do 
		{	busy=(spi_master_transfer_byte(0xFF)==0);
		} 
		while (busy&&(!timer_stopped(timeout)));
		stop_timer(timeout);
		if (busy)
		{	spi_deselect();
			error=1;
		}
		else
			error=0;
	
		if (!error)
		{	counter=0;
			spi_select();
			do
			{	error=mmc_command(dat,sizeof(dat),0,100);
			}
			while ((error)&&(++counter<RETRY));
			if (!error)
			{	for (counter=0;counter<8;counter++)
					spi_master_transfer_byte (0xFF);
				spi_master_transfer_byte (0xFE);
				spi_irq_enable();		
				error=spi_master_transfer_block (1,buff,512);				
			}		
		}
		if (!error)
		error=(mmc_write_wait());
		if (error)
			errcount++;
		else
			errcount=0;
	}
	while ((errcount>0)&&(errcount<16));
	return errcount;
}
*/





