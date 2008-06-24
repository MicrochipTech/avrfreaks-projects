/*************************************************************************
**	C version of CIO
**	Copyright (c) 1991 Jim Patchell
**
**	This is the Central Input Output device handler.  This is probably
**	a lame attempt to make access to various I/O devices a little more
**	orthagonal.  Maybe, maybe not.
**
**  The interface function, cio only requires one parameter, but accepts
**	a variable number of parameters depending on the type of function
**	it is that is being executed.
**
**	The functions that can be executed are:
**
**		add handler
**		open
**		close
**		get
**		get record (read)
**		put
**		put record (write)
**		status
**		special command (xio)
**
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cio.h"
#include "task.h"

/*
	cio global variables
*/

static IOCB *PoolList;

static HTABS htabs[CIO_MAX_DEV];	/*	space for I/O devices	*/
static int last_dev = 0;	/*	last device entry in table	*/
ECB *IOCBBlocker;

static int dev_search(IOCB *I);

static IOCB *AllocIOCB(void)
{
	IOCB *retval;

	if(PoolList == NULL)
		retval = (IOCB *)malloc(sizeof(IOCB));
	else
	{
		PendSemaphore(IOCBBlocker,0);
		retval = PoolList;
		PoolList = PoolList->next;
		PostSemaphore(IOCBBlocker,0);
	}
	return retval;
}

static void FreeIOCB(IOCB *I)
{
	PendSemaphore(IOCBBlocker,0);
	I->next = PoolList;
	PoolList = I;
	PostSemaphore(IOCBBlocker,0);
}


int CioAddHandler(char * devname,H_JVEC *t)
{
	//**************************************************************************
	// This is the function that you need to call when you want to add another
	// device to the device table
	//
	// parameter:
	//	devname..........name of the device to add
	//	t................pointer to the vector table for the driver
	//
	// return value:
	//	returns 0 on success, nonzero on fail
	//**************************************************************************
	int retval = 0;
	int sr;

	sr = Disable();
	if(last_dev >= CIO_MAX_DEV) retval = CIO_DEVTABLE_FULL;
	else
	{
		htabs[last_dev].dev_name = devname;	/*	device name	*/
		htabs[last_dev].HtabsEntry = t;	/*	address of table	*/
		++last_dev;
	}
	Enable(sr);
	return retval;
}

IOCB *CioOpen(char *devname,int mode)
{
	/*--------------------------------------------------
	** Open Device
	**
	**	parameters:
	**	devname.........name of device to open
	**	mode............IO mode to open device
	**
	** return value:
	**	NULL on error
	**	pointer to IO Control Block (IOCB) on success
	** Usage:
	** CioOpen("<devname>[<unit number>]",<IO Mode>);
	** <devname> :this is the device name to open
	** [<unit number>]: optional unit number
	**		this can be 0->9.  If the unit number is
	** ommitted, the default unit number is '0'
	--------------------------------------------------*/
	IOCB *iocb;

	if((iocb = AllocIOCB()) != NULL)
	{
		iocb->dev_name = devname;
		iocb->mode = mode;
		if(( iocb->ichid = dev_search(iocb)) < 0)
		{
			FreeIOCB(iocb);	//free up IOCB again
			iocb = NULL;
		}
		/* we have laid claim to the IOCB, release iocbevent	*/
		else if(((*htabs[iocb->ichid].HtabsEntry->openv)(iocb)) < 0)
		{
			FreeIOCB(iocb);	//free up IOCB again
			iocb = NULL;
		}
	}
	return(iocb);
}

int CioClose(IOCB *I)
{
	(*htabs[I->ichid].HtabsEntry->closv)(I);
	FreeIOCB(I);
	return(0);
}

int CioGetC(IOCB *I)
{
	/*
	** Get character from handler
	** called as:  cio(GETC,handle)
	*/
	if((I->mode & CIO_READ_ONLY) == 0)
		return(CIO_WRONLY);
	//
	// look up function
	//
	return ( (*htabs[I->ichid].HtabsEntry->getv)(I)  );
}

int CioRead(IOCB *I,char *b,int n)
{
	/*
	** Get a character buffer from handler
	** called as: cio(READ,handle,buffer,count)
	** driver function returns either an error or
	** actual number of bytes read
	*/
	if((I->mode & CIO_READ_ONLY) == 0)
		return(CIO_WRONLY);
	//
	//look up function
	//
	return (*htabs[I->ichid].HtabsEntry->readv)(I,b,n);
}

int CioPutC(IOCB *I,int c)
{
	/*
	** Put data to device
	**           : cio(PUTC,handle,data)
	*/
	if((I->mode & CIO_WRITE_ONLY) == 0)
		return(CIO_RDONLY);	/*	read only error	*/
	//
	//look up function
	//
	return( (*htabs[I->ichid].HtabsEntry->putv)(I,c));
}

int CioWrite(IOCB *I,char *b,int n)
{
	/*
	** Write buffer to a device
	**  called as: cio(WRITE,handle,buffer,count);
	** returns either an error or actual number of bytes written
	*/
	if((I->mode & CIO_WRITE_ONLY) == 0)
		return(CIO_RDONLY);	/*	read only error	*/
	return( (*htabs[I->ichid].HtabsEntry->writev)(I,b,n));
}

int CioStatus(IOCB *I,int mode)
{
	/*
	** This is where a status call is handled
	**
	** called as: cio(iocb,mode)
	**
	*/
	return((*htabs[I->ichid].HtabsEntry->statv)(I,mode) );
}

int CioXIO(IOCB *I,int cmd,...)
{
	/*
	** This is where the special commands are handled
	** called as:  cio(cmd,handle,buffer,count,aux)
	*/
	va_list argp;

	va_start(argp,cmd);	//get rid of dummy device name
	return((*htabs[I->ichid].HtabsEntry->specv)(I,cmd,argp));
}


static int dev_search(IOCB *iocb)
{
	/*
		search device table for device named and return index into
		device table
	*/
	int i;
	HTABS *ht;
	
	char *n,un[4];		/*unit numbers up to 999	*/
	int l;		/*	legnth of device name	*/

	l = strcspn(iocb->dev_name,"9876543210:");	/*	we need to find what is not unit number	*/
	ht = htabs + last_dev - 1;									/*	point to handler table	*/


	for(i=last_dev -1;(i > -1) && strncmp(iocb->dev_name,ht->dev_name,l) ;--i,--ht);
	if(i >= 0)
	{
		/*
		** ok, now we need to locate the unit number
		*/
		n = iocb->dev_name + l;
		if((l = strcspn(n,":")) == 0)
		{
			iocb->devnum = 1;		/*	default unit number	*/
		}
		else
		{
			strncpy(un,n,l);
			un[l] = '\0';
			iocb->devnum = atoi(un);	/*	convert unit number	*/
		}
	}
	return(i);
}

void CioInit(void)
{
	last_dev = 0;
	IOCBBlocker = NewSemaphore(1,SEMAPHORE_MODE_BLOCKING,"IOCBBlocker");
}
