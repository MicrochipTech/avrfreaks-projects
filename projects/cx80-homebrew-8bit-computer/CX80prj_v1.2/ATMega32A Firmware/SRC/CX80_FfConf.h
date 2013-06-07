/*
 * CX80_FfConf.h
 *
 * Created: 25/03/2013 14:33:00
 *  Author: Calogiuri Enzo Antonio
 */ 


#ifndef CX80_FFCONF_H_
#define CX80_FFCONF_H_

#define	_FS_TINY			1	/* 0:Normal or 1:Tiny */
#define _FS_READONLY		0	/* 0:Read/Write or 1:Read only */
#define _FS_MINIMIZE		0	/* 0 to 3 */
#define	_USE_STRFUNC		0	/* 0:Disable or 1-2:Enable */
#define	_USE_MKFS			0	/* 0:Disable or 1:Enable */
#define	_USE_FORWARD		0	/* 0:Disable or 1:Enable */
#define	_USE_FASTSEEK		0	/* 0:Disable or 1:Enable */
#define _CODE_PAGE			1
#define	_USE_LFN			0		/* 0 to 3 */
#define	_MAX_LFN			255		/* Maximum LFN length to handle (12 to 255) */
#define	_LFN_UNICODE		0	/* 0:ANSI/OEM or 1:Unicode */
#define _FS_RPATH			2	/* 0 to 2 */

#define _VOLUMES			1
#define	_MAX_SS				512		/* 512, 1024, 2048 or 4096 */
#define	_MULTI_PARTITION	0	/* 0:Single partition, 1/2:Enable multiple partition */
#define	_USE_ERASE			0	/* 0:Disable or 1:Enable */

#define _WORD_ACCESS		0	/* 0 or 1 */
#define _FS_REENTRANT		0		/* 0:Disable or 1:Enable */
#define _FS_TIMEOUT			1000	/* Timeout period in unit of time ticks */
#define	_SYNC_t				HANDLE	/* O/S dependent type of sync object. e.g. HANDLE, OS_EVENT*, ID and etc.. */

#define	_FS_SHARE			4	/* 0:Disable or >=1:Enable */

#endif /* CX80_FFCONF_H_ */