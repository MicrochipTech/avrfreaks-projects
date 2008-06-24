/*
** Header file for Central I/O Handler
*/

#ifndef CIO__H
#define CIO__H

#include <stdarg.h>

#define CIO_MAX_DEV			(20)

/*
** This data structure is used to describe the i/o device
** that has been opened.
*/

typedef struct _iocb{
	int ichid;		/*	handler ID							0	*/
	int devnum;		/*	device number						2	*/
	char *dev_name;	/*	pointer to file name				4	*/
	int mode;		/*	i/o mode file was opened with		8	*/
	void *p;	/*	general purpose pointer					10	*/
	struct _iocb *next;		/* linked list for pool	*/
}IOCB;

/*
** This vector table is used to locate the functions that
** perform the primitive I/O functions for a device
**
** A pointer to this vector table is located in the device
** handler lookup table.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	long (*openv)(IOCB *);				/*	0	open vector	*/
	long (*closv)(IOCB *);				/*	1	close vector	*/
	long (*getv)(IOCB *);				/*	2	get byte vector	*/
	long (*readv)(IOCB *,char *,int);	/*	3	read buffer	*/
	long (*putv)(IOCB *,int);			/*	4	put byte vector	*/
	long (*writev)(IOCB *,char *,int);	/*	5	write a buffer	*/
	long (*statv)(IOCB *,int);			/*	6	get status vector	*/
	long (*specv)(IOCB *,int cmd,va_list);		/*	7	special vector	*/
	long (*initv)(void);						/*	8	init code vector	*/
}H_JVEC;


#ifdef __cplusplus
}
#endif

/*
** this is the structure of one element in the device driver
** lookup table.  Each device name is associtated with a vector
** table.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	const char *dev_name;	/*	pointer to device name			*/
	H_JVEC *HtabsEntry;		/*	pointer to handler jump table	*/
}HTABS;

#ifdef __cplusplus
}
#endif

//*************************************************************************
// Error Codes
//*************************************************************************

#define CIO_ERROR_START			(-64)
#define CIO_DEVTABLE_FULL		(CIO_ERROR_START-0)
#define CIO_NO_DEVICE			(CIO_ERROR_START-1)
#define CIO_WRONLY				(CIO_ERROR_START-2)
#define CIO_RDONLY				(CIO_ERROR_START-3)

#define	EOL	0x0a


/*
** file modes
*/
#define CIO_READ_ONLY	(4)
#define CIO_WRITE_ONLY	(8)

#ifdef __cplusplus
extern "C" {
#endif

extern int CioAddHandler(char * devname,H_JVEC *t);
extern IOCB *CioOpen(char *devname,int mode);
extern int CioClose(IOCB *I);
extern int CioGetC(IOCB *I);
extern int CioRead(IOCB *I,char *b,int n);
extern int CioPutC(IOCB *I,int c);
extern int CioWrite(IOCB *I,char *b,int n);
extern int CioStatus(IOCB *I,int mode);
extern int CioXIO(IOCB *I,int cmd,...);


extern void CioInit(void);

#ifdef __cplusplus
}
#endif

#endif
