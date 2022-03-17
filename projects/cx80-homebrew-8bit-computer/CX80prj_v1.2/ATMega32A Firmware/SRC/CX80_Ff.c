/*
 * CX80_Ff.c
 *
 * Created: 25/03/2013 14:52:24
 *  Author: Calogiuri Enzo Antonio
 */ 
#include <string.h>

#include "CX80.h"
#include "CX80_Ff.h"
#include "CX80_DiskIO.h"

/* Definitions on sector size */
#if _MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096
#error Wrong sector size.
#endif
#if _MAX_SS != 512
#define	SS(fs)	((fs)->ssize)	/* Variable sector size */
#else
#define	SS(fs)	512U			/* Fixed sector size */
#endif

/* Reentrancy related */
#if _FS_REENTRANT
#if _USE_LFN == 1
#error Static LFN work area must not be used in re-entrant configuration.
#endif
#define	ENTER_FF(fs)		{ if (!lock_fs(fs)) return FR_TIMEOUT; }
#define	LEAVE_FF(fs, res)	{ unlock_fs(fs, res); return res; }
#else
#define	ENTER_FF(fs)
#define LEAVE_FF(fs, res)	return res
#endif

#define	ABORT(fs, res)		{ fp->flag |= FA__ERROR; LEAVE_FF(fs, res); }


/* File shareing feature */
#if _FS_SHARE
#if _FS_READONLY
#error _FS_SHARE must be 0 on read-only cfg.
#endif
typedef struct {
	FATFS *fs;				/* File ID 1, volume (NULL:blank entry) */
	DWord clu;				/* File ID 2, directory */
	word idx;				/* File ID 3, directory index */
	word ctr;				/* File open counter, 0:none, 0x01..0xFF:read open count, 0x100:write mode */
} FILESEM;
#endif

/* Misc definitions */
#define LD_CLUST(dir)	(((DWord)LD_WORD(dir+DIR_FstClusHI)<<16) | LD_WORD(dir+DIR_FstClusLO))
#define ST_CLUST(dir,cl) {ST_WORD(dir+DIR_FstClusLO, cl); ST_WORD(dir+DIR_FstClusHI, (DWord)cl>>16);}
	
/* DBCS code ranges and SBCS extend char conversion table */

#if _CODE_PAGE == 932	/* Japanese Shift-JIS */
#define _DF1S	0x81	/* DBC 1st byte range 1 start */
#define _DF1E	0x9F	/* DBC 1st byte range 1 end */
#define _DF2S	0xE0	/* DBC 1st byte range 2 start */
#define _DF2E	0xFC	/* DBC 1st byte range 2 end */
#define _DS1S	0x40	/* DBC 2nd byte range 1 start */
#define _DS1E	0x7E	/* DBC 2nd byte range 1 end */
#define _DS2S	0x80	/* DBC 2nd byte range 2 start */
#define _DS2E	0xFC	/* DBC 2nd byte range 2 end */

#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0x80
#define _DS2E	0xFE

#elif _CODE_PAGE == 949	/* Korean */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x41
#define _DS1E	0x5A
#define _DS2S	0x61
#define _DS2E	0x7A
#define _DS3S	0x81
#define _DS3E	0xFE

#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0xA1
#define _DS2E	0xFE

#elif _CODE_PAGE == 437	/* U.S. (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F,0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 720	/* Arabic (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x45,0x41,0x84,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x49,0x49,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 737	/* Greek (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
				0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xE7,0xE8,0xF1,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 775	/* Baltic (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 850	/* Multilingual Latin 1 (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 852	/* Latin 2 (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F,0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}

#elif _CODE_PAGE == 855	/* Cyrillic (OEM) */
#define _DF1S	0
#define _EXCVT {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F,0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
				0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
				0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 857	/* Turkish (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x98,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0x59,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 858	/* Multilingual Latin 1 + Euro (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 862	/* Hebrew (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 866	/* Russian (OEM) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x90,0x91,0x92,0x93,0x9d,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 874	/* Thai (OEM, Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1250 /* Central Europe (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xA3,0xB4,0xB5,0xB6,0xB7,0xB8,0xA5,0xAA,0xBB,0xBC,0xBD,0xBC,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1251 /* Cyrillic (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x82,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x80,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
				0xA0,0xA2,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB2,0xA5,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xA3,0xBD,0xBD,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF}

#elif _CODE_PAGE == 1252 /* Latin 1 (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0xAd,0x9B,0x8C,0x9D,0xAE,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1253 /* Greek (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xA2,0xB8,0xB9,0xBA, \
				0xE0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xFB,0xBC,0xFD,0xBF,0xFF}

#elif _CODE_PAGE == 1254 /* Turkish (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1255 /* Hebrew (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1256 /* Arabic (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x8C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x41,0xE1,0x41,0xE3,0xE4,0xE5,0xE6,0x43,0x45,0x45,0x45,0x45,0xEC,0xED,0x49,0x49,0xF0,0xF1,0xF2,0xF3,0x4F,0xF5,0xF6,0xF7,0xF8,0x55,0xFA,0x55,0x55,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1257 /* Baltic (Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xBC,0xBD,0xBE,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1258 /* Vietnam (OEM, Windows) */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0xAC,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xEC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xFE,0x9F}

#elif _CODE_PAGE == 1	/* ASCII (for only non-LFN cfg) */
#if _USE_LFN
#error Cannot use LFN feature without valid code page.
#endif
#define _DF1S	0

#else
#error Unknown code page

#endif

/* Character code support macros */
#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))

#if _DF1S		/* Code page is DBCS */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((byte)(c) >= _DF1S && (byte)(c) <= _DF1E) || ((byte)(c) >= _DF2S && (byte)(c) <= _DF2E))
#else			/* One 1st byte area */
#define IsDBCS1(c)	((byte)(c) >= _DF1S && (byte)(c) <= _DF1E)
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((byte)(c) >= _DS1S && (byte)(c) <= _DS1E) || ((byte)(c) >= _DS2S && (byte)(c) <= _DS2E) || ((byte)(c) >= _DS3S && (byte)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((byte)(c) >= _DS1S && (byte)(c) <= _DS1E) || ((byte)(c) >= _DS2S && (byte)(c) <= _DS2E))
#endif

#else			/* Code page is SBCS */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */


/* Name status flags */
#define NS			11		/* Index of name status byte in fn[] */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower case flag (body) */
#define NS_EXT		0x10	/* Lower case flag (ext) */
#define NS_DOT		0x20	/* Dot entry */


/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16	4086	/* Minimum number of clusters for FAT16 */
#define	MIN_FAT32	65526	/* Minimum number of clusters for FAT32 */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because the structure is not binary compatible between
/ different platforms */

#define BS_jmpBoot			0	/* Jump instruction (3) */
#define BS_OEMName			3	/* OEM name (8) */
#define BPB_BytsPerSec		11	/* Sector size [byte] (2) */
#define BPB_SecPerClus		13	/* Cluster size [sector] (1) */
#define BPB_RsvdSecCnt		14	/* Size of reserved area [sector] (2) */
#define BPB_NumFATs			16	/* Number of FAT copies (1) */
#define BPB_RootEntCnt		17	/* Number of root dir entries for FAT12/16 (2) */
#define BPB_TotSec16		19	/* Volume size [sector] (2) */
#define BPB_Media			21	/* Media descriptor (1) */
#define BPB_FATSz16			22	/* FAT size [sector] (2) */
#define BPB_SecPerTrk		24	/* Track size [sector] (2) */
#define BPB_NumHeads		26	/* Number of heads (2) */
#define BPB_HiddSec			28	/* Number of special hidden sectors (4) */
#define BPB_TotSec32		32	/* Volume size [sector] (4) */
#define BS_DrvNum			36	/* Physical drive number (2) */
#define BS_BootSig			38	/* Extended boot signature (1) */
#define BS_VolID			39	/* Volume serial number (4) */
#define BS_VolLab			43	/* Volume label (8) */
#define BS_FilSysType		54	/* File system type (1) */
#define BPB_FATSz32			36	/* FAT size [sector] (4) */
#define BPB_ExtFlags		40	/* Extended flags (2) */
#define BPB_FSVer			42	/* File system version (2) */
#define BPB_RootClus		44	/* Root dir first cluster (4) */
#define BPB_FSInfo			48	/* Offset of FSInfo sector (2) */
#define BPB_BkBootSec		50	/* Offset of backup boot sectot (2) */
#define BS_DrvNum32			64	/* Physical drive number (2) */
#define BS_BootSig32		66	/* Extended boot signature (1) */
#define BS_VolID32			67	/* Volume serial number (4) */
#define BS_VolLab32			71	/* Volume label (8) */
#define BS_FilSysType32		82	/* File system type (1) */
#define	FSI_LeadSig			0	/* FSI: Leading signature (4) */
#define	FSI_StrucSig		484	/* FSI: Structure signature (4) */
#define	FSI_Free_Count		488	/* FSI: Number of free clusters (4) */
#define	FSI_Nxt_Free		492	/* FSI: Last allocated cluster (4) */
#define MBR_Table			446	/* MBR: Partition table offset (2) */
#define	SZ_PTE				16	/* MBR: Size of a partition table entry */
#define BS_55AA				510	/* Boot sector signature (2) */

#define	DIR_Name			0	/* Short file name (11) */
#define	DIR_Attr			11	/* Attribute (1) */
#define	DIR_NTres			12	/* NT flag (1) */
#define	DIR_CrtTime			14	/* Created time (2) */
#define	DIR_CrtDate			16	/* Created date (2) */
#define	DIR_FstClusHI		20	/* Higher 16-bit of first cluster (2) */
#define	DIR_WrtTime			22	/* Modified time (2) */
#define	DIR_WrtDate			24	/* Modified date (2) */
#define	DIR_FstClusLO		26	/* Lower 16-bit of first cluster (2) */
#define	DIR_FileSize		28	/* File size (4) */
#define	LDIR_Ord			0	/* LFN entry order and LLE flag (1) */
#define	LDIR_Attr			11	/* LFN attribute (1) */
#define	LDIR_Type			12	/* LFN type (1) */
#define	LDIR_Chksum			13	/* Sum of corresponding SFN entry */
#define	LDIR_FstClusLO		26	/* Filled by zero (0) */
#define	SZ_DIR				32		/* Size of a directory entry */
#define	LLE					0x40	/* Last long entry flag in LDIR_Ord */
#define	DDE					0xE5	/* Deleted directory enrty mark in DIR_Name[0] */
#define	NDDE				0x05	/* Replacement of a character collides with DDE */

#define get_fattime			(((DWord)(2012 - 1980) << 25) | ((DWord)06 << 21) | ((DWord)06 << 16) | ((DWord)06 << 11) | ((DWord)06 << 5) | ((DWord)06 >> 1))


/*------------------------------------------------------------*/
/* Module private work area                                   */
/*------------------------------------------------------------*/
/* Note that uninitialized variables with static duration are
/  zeroed/nulled at start-up. If not, the compiler or start-up
/  routine is out of ANSI-C standard.
*/

#if _VOLUMES
static
FATFS *FatFs[_VOLUMES];	/* Pointer to the file system objects (logical drives) */
#else
#error Number of volumes must not be 0.
#endif

static word Fsid;				/* File system mount ID */

#if _FS_RPATH
static
byte CurrVol;			/* Current drive */
#endif

#if _FS_SHARE
static FILESEM Files[_FS_SHARE];	/* File lock semaphores */
#endif

#if _USE_LFN == 0			/* No LFN feature */
#define	DEF_NAMEBUF			byte sfn[12]
#define INIT_BUF(dobj)		(dobj).fn = sfn
#define	FREE_BUF()

#elif _USE_LFN == 1			/* LFN feature with static working buffer */
static WCHAR LfnBuf[_MAX_LFN+1];
#define	DEF_NAMEBUF			byte sfn[12]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = LfnBuf; }
#define	FREE_BUF()

#elif _USE_LFN == 2 		/* LFN feature with dynamic working buffer on the stack */
#define	DEF_NAMEBUF			byte sfn[12]; WCHAR lbuf[_MAX_LFN+1]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = lbuf; }
#define	FREE_BUF()

#elif _USE_LFN == 3 		/* LFN feature with dynamic working buffer on the heap */
#define	DEF_NAMEBUF			byte sfn[12]; WCHAR *lfn
#define INIT_BUF(dobj)		{ lfn = ff_memalloc((_MAX_LFN + 1) * 2); \
							  if (!lfn) LEAVE_FF((dobj).fs, FR_NOT_ENOUGH_CORE); \
							  (dobj).lfn = lfn;	(dobj).fn = sfn; }
#define	FREE_BUF()			ff_memfree(lfn)

#else
#error Wrong LFN configuration.
#endif

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static int chk_chr (const char* str, int chr)
{
	while (*str && *str != chr)
		str++;
	
	return *str;
}

#if _FS_SHARE

static FRESULT chk_lock(DIR* dj, char acc)
{
	byte i, be;
	
	for (i = be = 0; i < _FS_SHARE; i++)
	{
		if (Files[i].fs)
		{
			if (Files[i].fs == dj->fs &&	 	/* Check if the file matched with an open file */
				Files[i].clu == dj->sclust && Files[i].idx == dj->index)
				break;
		}
		else
			be++;
	}
	
	if (i == _FS_SHARE)	/* The file is not opened */
		return (be || acc == 2) ? FR_OK : FR_TOO_MANY_OPEN_FILES;	/* Is there a blank entry for new file? */
		
	/* The file has been opened. Reject any open against writing file and all write mode open */
	return (acc || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}

static char enq_lock(void)	/* Check if an entry is available for a new file */
{
	byte i;
	
	for (i = 0; i < _FS_SHARE && Files[i].fs; i++) ;
	
	return (i == _FS_SHARE) ? 0 : 1;
}

static byte inc_lock(DIR* dj, char acc)
{
	byte i;
	
	for (i = 0; i < _FS_SHARE; i++)
	{
		if (Files[i].fs == dj->fs && Files[i].clu == dj->sclust &&
			Files[i].idx == dj->index)
		 break;
	}
	
	if (i == _FS_SHARE)
	{
		for (i = 0; i < _FS_SHARE && Files[i].fs; i++) ;
		
		if (i == _FS_SHARE)
			return 0;	/* No space to register (int err) */
			
		Files[i].fs = dj->fs;
		Files[i].clu = dj->sclust;
		Files[i].idx = dj->index;
		Files[i].ctr = 0;
	}
	
	if (acc && Files[i].ctr)
		return 0;	/* Access violation (int err) */
		
	Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1;	/* Set semaphore value */
	
	return i + 1;
}

static FRESULT dec_lock(byte i)
{
	word n;
	FRESULT res;
	
	if (--i < _FS_SHARE)
	{
		n = Files[i].ctr;
		
		if (n == 0x100)
			n = 0;
			
		if (n)
			n--;
			
		Files[i].ctr = n;
		
		if (!n)
			Files[i].fs = 0;
			
		res = FR_OK;
	}
	else
		res = FR_INT_ERR;
		
	return res;
}

static void clear_lock(FATFS *fs)
{
	byte i;
	
	for (i = 0; i < _FS_SHARE; i++)
		if (Files[i].fs == fs)
			Files[i].fs = 0;
}

#endif

/*-----------------------------------------------------------------------*/
/* Change window offset                                                  */
/*-----------------------------------------------------------------------*/

static FRESULT move_window(FATFS *fs, DWord sector)
{
	DWord wsect;
	byte nf;
	
	wsect = fs->winsect;
	
	if (wsect != sector)
	{
#if !_FS_READONLY
		if (fs->wflag)
		{
			if (disk_write(fs->win, wsect, 1) != RES_OK)
				return FR_DISK_ERR;
				
			fs->wflag = 0;
			
			if (wsect < (fs->fatbase + fs->fsize))
			{
				for (nf = fs->n_fats; nf > 1; nf--)
				{
					wsect += fs->fsize;
					
					disk_write(fs->win, wsect, 1);
				}
			}
		}
#endif

		if (sector)
		{
			if (disk_read(fs->win, sector, 1) != RES_OK)
				return FR_DISK_ERR;
				
			fs->winsect = sector;
		}
	}
	
	return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Clean-up cached data                                                  */
/*-----------------------------------------------------------------------*/

#if !_FS_READONLY
static FRESULT sync(FATFS *fs)
{
	FRESULT res;
	
	res = move_window(fs, 0);
	
	if (res == FR_OK)
	{
		if (fs->fs_type == FS_FAT32 && fs->fsi_flag)
		{
			fs->winsect = 0;
			
			memset(fs->win, 0, 512);
			
			ST_WORD(fs->win + BS_55AA, 0xAA55);
			ST_DWORD(fs->win + FSI_LeadSig, 0x41615252);
			ST_DWORD(fs->win + FSI_StrucSig, 0x61417272);
			ST_DWORD(fs->win + FSI_Free_Count, fs->free_clust);
			ST_DWORD(fs->win + FSI_Nxt_Free, fs->last_clust);
			
			/* Write it into the FSInfo sector */
			disk_write(fs->win, fs->fsi_sector, 1);
			
			fs->fsi_flag = 0;
		}
		
		/* Make sure that no pending write process in the physical drive */
		if (disk_ioctl(CTRL_SYNC, 0) != RES_OK)
			res = FR_DISK_ERR;
	}
	
	return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/

DWord clust2sect(FATFS *fs, DWord clst)
{
	clst -= 2;
	
	if (clst >= (fs->n_fatent - 2))
		return 0;		/* Invalid cluster# */
		
	return clst * fs->csize + fs->database;
}

/*-----------------------------------------------------------------------*/
/* FAT access - Read value of a FAT entry                                */
/*-----------------------------------------------------------------------*/

DWord get_fat(FATFS *fs, DWord clst)
{
	uInt wc, bc;
	byte *p;
	
	if (clst < 2 || clst >= fs->n_fatent)	/* Chack range */
		return 1;
		
	switch (fs->fs_type)
	{
		case FS_FAT12 :
			bc = (uInt)clst;
			bc += bc / 2;
			
			if (move_window(fs, fs->fatbase + (bc / SS(fs))))
				break;
				
			wc = fs->win[bc % SS(fs)];
			bc++;
			
			if (move_window(fs, fs->fatbase + (bc / SS(fs))))
				break;
				
			wc |= fs->win[bc % SS(fs)] << 8;
			
			return (clst & 1) ? (wc >> 4) : (wc & 0xFFF);
			
		case FS_FAT16 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))))
				break;
				
			p = &fs->win[clst * 2 % SS(fs)];
			
			return LD_WORD(p);
			
		case FS_FAT32 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))))
				break;
				
			p = &fs->win[clst * 4 % SS(fs)];
			
			return LD_DWORD(p) & 0x0FFFFFFF;
	}
	
	return 0xFFFFFFFF;	/* An error occurred at the disk I/O layer */
}

/*-----------------------------------------------------------------------*/
/* FAT access - Change value of a FAT entry                              */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
FRESULT put_fat(FATFS *fs, DWord clst, DWord val)
{
	uInt bc;
	byte *p;
	FRESULT res;
	
	if (clst < 2 || clst >= fs->n_fatent)
		res = FR_INT_ERR;
	else
	{
		switch (fs->fs_type)
		{
			case FS_FAT12 :
				bc = clst;
				bc += bc / 2;
				
				res = move_window(fs, fs->fatbase + (bc / SS(fs)));
				
				if (res != FR_OK)
					break;
					
				p = &fs->win[bc % SS(fs)];
				*p = (clst & 1) ? ((*p & 0x0F) | ((byte)val << 4)) : (byte)val;
				bc++;
				fs->wflag = 1;
				
				res = move_window(fs, fs->fatbase + (bc / SS(fs)));
				
				if (res != FR_OK)
					break;
					
				p = &fs->win[bc % SS(fs)];
				*p = (clst & 1) ? (byte)(val >> 4) : ((*p & 0xF0) | ((byte)(val >> 8) & 0x0F));
				break;
				
			case FS_FAT16 :
				res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
				
				if (res != FR_OK)
					break;
					
				p = &fs->win[clst * 2 % SS(fs)];
				ST_WORD(p, (word)val);
				break;
				
			case FS_FAT32 :
				res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
				
				if (res != FR_OK)
					break;
					
				p = &fs->win[clst * 4 % SS(fs)];
				val |= LD_DWORD(p) & 0xF0000000;
				ST_DWORD(p, val);
				break;
				
			default :
				res = FR_INT_ERR;
		}
		
		fs->wflag = 1;
	}
	
	return res;
}
#endif /* !_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/

#if !_FS_READONLY
static FRESULT remove_chain(FATFS *fs, DWord clst)
{
	FRESULT res;
	DWord nxt;
#if _USE_ERASE
	DWord scl = clst, ecl = clst, resion[2];
#endif

	if (clst < 2 || clst >= fs->n_fatent)
		res = FR_INT_ERR;
	else
	{
		res = FR_OK;
		
		while (clst < fs->n_fatent)
		{
			nxt = get_fat(fs, clst);
			
			if (nxt == 0)
				break;
				
			if (nxt == 1)
			{
				res = FR_INT_ERR;
				break;
			}
			
			if (nxt == 0xFFFFFFFF)
			{
				res = FR_DISK_ERR;
				break;
			}
			
			res = put_fat(fs, clst, 0);
			
			if (res != FR_OK)
				break;
				
			if (fs->free_clust != 0xFFFFFFFF)
			{
				fs->free_clust++;
				fs->fsi_flag = 1;
			}
			
#if _USE_ERASE
			if (ecl + 1 == nxt)
				ecl = nxt;
			else
			{
				resion[0] = clust2sect(fs, scl);					/* Start sector */
				resion[1] = clust2sect(fs, ecl) + fs->csize - 1;	/* End sector */
				
				disk_ioctl(CTRL_ERASE_SECTOR, resion);		/* Erase the block */
				
				scl = ecl = nxt;
			}
#endif

			clst = nxt;	/* Next cluster */
		}
	}
	
	return res;
}
#endif /* !_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch or Create a cluster chain                      */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static DWord create_chain(FATFS *fs, DWord clst)
{
	DWord cs, ncl, scl;
	FRESULT res;
	
	if (clst == 0)
	{
		scl = fs->last_clust;
		
		if (!scl || scl >= fs->n_fatent)
			scl = 1;
	}
	else
	{
		cs = get_fat(fs, clst);
		
		if (cs < 2)
			return 1;
			
		if (cs < fs->n_fatent)
			return cs;	/* It is already followed by next cluster */
			
		scl = clst;
	}
	
	ncl = scl;				/* Start cluster */
	
	for (;;)
	{
		ncl++;							/* Next cluster */
		
		if (ncl >= fs->n_fatent)
		{
			ncl = 2;
			
			if (ncl > scl)
				return 0;	/* No free cluster */
		}
		
		cs = get_fat(fs, ncl);			/* Get the cluster status */
		
		if (cs == 0)
			break;				/* Found a free cluster */
			
		if (cs == 0xFFFFFFFF || cs == 1)/* An error occurred */
			return cs;
			
		if (ncl == scl)
			return 0;		/* No free cluster */
	}
	
	res = put_fat(fs, ncl, 0x0FFFFFFF);	/* Mark the new cluster "last link" */
	
	if (res == FR_OK && clst != 0)
		res = put_fat(fs, clst, ncl);
		
	if (res == FR_OK)
	{
		fs->last_clust = ncl;			/* Update FSINFO */
		
		if (fs->free_clust != 0xFFFFFFFF)
		{
			fs->free_clust--;
			fs->fsi_flag = 1;
		}
	}
	else
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;
		
	return ncl;		/* Return new cluster number or error code */
}
#endif /* !_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/

#if _USE_FASTSEEK
static DWord clmt_clust(FIL* fp, DWord ofs)
{
	DWord cl, ncl, *tbl;
	
	tbl = fp->cltbl + 1;	/* Top of CLMT */
	cl = ofs / SS(fp->fs) / fp->fs->csize;	/* Cluster order from top of the file */
	
	for (;;)
	{
		ncl = *tbl++;			/* Number of cluters in the fragment */
		
		if (!ncl)
			return 0;		/* End of table? (error) */
			
		if (cl < ncl)
			break;	/* In this fragment? */
			
		cl -= ncl;
		tbl++;		/* Next fragment */
	}
	
	return cl + *tbl;	/* Return the cluster number */
}
#endif

/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static FRESULT dir_sdi(DIR *dj, word idx)
{
	DWord clst;
	word ic;
	
	dj->index = idx;
	clst = dj->sclust;
	
	if (clst == 1 || clst >= dj->fs->n_fatent)	/* Check start cluster range */
		return FR_INT_ERR;
		
	if (!clst && dj->fs->fs_type == FS_FAT32)	/* Replace cluster# 0 with root cluster# if in FAT32 */
		clst = dj->fs->dirbase;
		
	if (clst == 0)
	{
		dj->clust = clst;
		
		if (idx >= dj->fs->n_rootdir)		/* Index is out of range */
			return FR_INT_ERR;
			
		dj->sect = dj->fs->dirbase + idx / (SS(dj->fs) / SZ_DIR);	/* Sector# */
	}
	else
	{
		ic = SS(dj->fs) / SZ_DIR * dj->fs->csize;	/* Entries per cluster */
		
		while (idx >= ic)
		{
			clst = get_fat(dj->fs, clst);				/* Get next cluster */
			
			if (clst == 0xFFFFFFFF)
				return FR_DISK_ERR;	/* Disk error */
				
			if (clst < 2 || clst >= dj->fs->n_fatent)	/* Reached to end of table or int error */
				return FR_INT_ERR;
				
			idx -= ic;
		}
		
		dj->clust = clst;
		dj->sect = clust2sect(dj->fs, clst) + idx / (SS(dj->fs) / SZ_DIR);	/* Sector# */
	}
	
	dj->dir = dj->fs->win + (idx % (SS(dj->fs) / SZ_DIR)) * SZ_DIR;	/* Ptr to the entry in the sector */
	
	return FR_OK;	/* Seek succeeded */
}

/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory index next                        */
/*-----------------------------------------------------------------------*/

static FRESULT dir_next(DIR *dj, byte stretch)
{
	DWord clst;
	word i;
	
	stretch = stretch;		/* To suppress warning on read-only cfg. */
	i = dj->index + 1;
	
	if (!i || !dj->sect)	/* Report EOT when index has reached 65535 */
		return FR_NO_FILE;
		
	if (!(i % (SS(dj->fs) / SZ_DIR)))
	{
		dj->sect++;					/* Next sector */
		
		if (dj->clust == 0)
		{
			if (i >= dj->fs->n_rootdir)	/* Report EOT when end of table */
				return FR_NO_FILE;
		}
		else
		{
			if (((i / (SS(dj->fs) / SZ_DIR)) & (dj->fs->csize - 1)) == 0)
			{
				clst = get_fat(dj->fs, dj->clust);				/* Get next cluster */
				
				if (clst <= 1)
					return FR_INT_ERR;
					
				if (clst == 0xFFFFFFFF)
					return FR_DISK_ERR;
					
				if (clst >= dj->fs->n_fatent)
				{
#if !_FS_READONLY
					byte c;
					
					if (!stretch)
						return FR_NO_FILE;			/* When do not stretch, report EOT */
						
					clst = create_chain(dj->fs, dj->clust);		/* Stretch cluster chain */
					
					if (clst == 0)
						return FR_DENIED;			/* No free cluster */
						
					if (clst == 1)
						return FR_INT_ERR;
						
					if (clst == 0xFFFFFFFF)
						return FR_DISK_ERR;
						
					if (move_window(dj->fs, 0))
						return FR_DISK_ERR;	/* Flush active window */
						
					memset(dj->fs->win, 0, SS(dj->fs));
					
					dj->fs->winsect = clust2sect(dj->fs, clst);	/* Cluster start sector */
					
					for (c = 0; c < dj->fs->csize; c++)
					{
						dj->fs->wflag = 1;
						
						if (move_window(dj->fs, 0))
							return FR_DISK_ERR;
							
						dj->fs->winsect++;
					}
					
					dj->fs->winsect -= c;						/* Rewind window address */
#else
					return FR_NO_FILE;			/* Report EOT */
#endif
				}
				
				dj->clust = clst;				/* Initialize data for new cluster */
				dj->sect = clust2sect(dj->fs, clst);
			}
		}
	}
	
	dj->index = i;
	dj->dir = dj->fs->win + (i % (SS(dj->fs) / SZ_DIR)) * SZ_DIR;

	return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static FRESULT dir_find(DIR *dj)
{
	FRESULT res;
	byte c, *dir;
	
	res = dir_sdi(dj, 0);			/* Rewind directory object */
	
	if (res != FR_OK)
		return res;
		
	do 
	{
		res = move_window(dj->fs, dj->sect);
		
		if (res != FR_OK)
			break;
			
		dir = dj->dir;					/* Ptr to the directory entry of current index */
		c = dir[DIR_Name];
		
		if (c == 0)
		{
			res = FR_NO_FILE;
			break;
		}	/* Reached to end of table */
		
		if (!(dir[DIR_Attr] & AM_VOL) && !memcmp(dir, dj->fn, 11)) /* Is it a valid entry? */
			break;
			
		res = dir_next(dj, 0);		/* Next entry */
	} while (res == FR_OK);
	
	return res;
}

/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1
static FRESULT dir_read(DIR *dj)
{
	FRESULT res;
	byte c, *dir;
	
	res = FR_NO_FILE;
	
	while (dj->sect)
	{
		res = move_window(dj->fs, dj->sect);
		
		if (res != FR_OK)
			break;
			
		dir = dj->dir;					/* Ptr to the directory entry of current index */
		c = dir[DIR_Name];
		
		if (c == 0)
		{
			res = FR_NO_FILE;
			break;
		}	/* Reached to end of table */
		
		if (c != DDE && (_FS_RPATH || c != '.') && !(dir[DIR_Attr] & AM_VOL))	/* Is it a valid entry? */
			break;
			
		res = dir_next(dj, 0);				/* Next entry */
		
		if (res != FR_OK)
			break;
	}
	
	if (res != FR_OK)
		dj->sect = 0;

	return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Register an object to the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static FRESULT dir_register(DIR *dj)
{
	FRESULT res;
	byte c, *dir;
	
	res = dir_sdi(dj, 0);
	
	if (res == FR_OK)
	{
		do 
		{
			res = move_window(dj->fs, dj->sect);
			
			if (res != FR_OK)
				break;
				
			c = *dj->dir;
			
			if (c == DDE || c == 0)
				break;	/* Is it a blank entry? */
				
			res = dir_next(dj, 1);			/* Next entry with table stretch */
		} while (res == FR_OK);
	}
	
	if (res == FR_OK)
	{
		res = move_window(dj->fs, dj->sect);
		
		if (res == FR_OK)
		{
			dir = dj->dir;
			
			memset(dir, 0, SZ_DIR);	/* Clean the entry */
			memcpy(dir, dj->fn, 11);	/* Put SFN */
			
			dj->fs->wflag = 1;
		}
	}
	
	return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Remove an object from the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY && !_FS_MINIMIZE
static FRESULT dir_remove(DIR *dj)
{
	FRESULT res;
	
	res = dir_sdi(dj, dj->index);
	
	if (res == FR_OK)
	{
		res = move_window(dj->fs, dj->sect);
		
		if (res == FR_OK)
		{
			*dj->dir = DDE;			/* Mark the entry "deleted" */
			dj->fs->wflag = 1;
		}
	}
	
	return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* Pick a segment and create the object name in directory form           */
/*-----------------------------------------------------------------------*/
static FRESULT create_name(DIR *dj, const TCHAR **path)
{
	byte b, c, d, *sfn;
	uInt ni, si, i;
	const char *p;
	
	/* Create file name in directory form */
	for (p = *path; *p == '/' || *p == '\\'; p++) ;	/* Strip duplicated separator */
	
	sfn = dj->fn;
	
	memset(sfn, ' ', 11);
	
	si = i = b = 0;
	ni = 8;
	
#if _FS_RPATH
	if (p[si] == '.')
	{
		for (;;)
		{
			c = (byte)p[si++];
			
			if (c != '.' || si >= 3)
				break;
				
			sfn[i++] = c;
		}
		
		if (c != '/' && c != '\\' && c > ' ')
			return FR_INVALID_NAME;
			
		*path = &p[si];									/* Return pointer to the next segment */
		sfn[NS] = (c <= ' ') ? NS_LAST | NS_DOT : NS_DOT;	/* Set last segment flag if end of path */
		return FR_OK;
	}
#endif

	for (;;)
	{
		c = (byte)p[si++];
		
		if (c <= ' ' || c == '/' || c == '\\')
			break;	/* Break on end of segment */
			
		if (c == '.' || i >= ni)
		{
			if (ni != 8 || c != '.')
				return FR_INVALID_NAME;
				
			i = 8;
			ni = 11;
			b <<= 2;
			continue;
		}
		
		if (c >= 0x80)
		{
			b |= 3;						/* Eliminate NT flag */
#ifdef _EXCVT
			c = excvt[c-0x80];			/* Upper conversion (SBCS) */
#else
#if !_DF1S	/* ASCII only cfg */
			return FR_INVALID_NAME;
#endif
#endif
		}
		
		if (IsDBCS1(c))
		{
			d = (byte)p[si++];			/* Get 2nd byte */
			
			if (!IsDBCS2(d) || i >= ni - 1)	/* Reject invalid DBC */
				return FR_INVALID_NAME;
				
			sfn[i++] = c;
			sfn[i++] = d;
		}
		else
		{
			if (chk_chr("\"*+,:;<=>\?[]|\x7F", c))	/* Reject illegal chrs for SFN */
				return FR_INVALID_NAME;
				
			if (IsUpper(c))
				b |= 2;
			else
			{
				if (IsLower(c))
				{
					b |= 1;
					c -= 0x20;
				}
			}
			
			sfn[i++] = c;
		}
	}
	
	*path = &p[si];						/* Return pointer to the next segment */
	c = (c <= ' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */
	
	if (!i)
		return FR_INVALID_NAME;		/* Reject nul string */
		
	if (sfn[0] == DDE)
		sfn[0] = NDDE;	/* When first char collides with DDE, replace it with 0x05 */
		
	if (ni == 8)
		b <<= 2;
		
	if ((b & 0x03) == 0x01)
		c |= NS_EXT;	/* NT flag (Name extension has only small capital) */
		
	if ((b & 0x0C) == 0x04)
		c |= NS_BODY;	/* NT flag (Name body has only small capital) */
		
	sfn[NS] = c;		/* Store NT flag, File name is created */
	
	return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1
static void get_fileinfo(DIR *dj, FILINFO *fno)
{
	byte i;
	byte nt, *dir;
	TCHAR *p, c;
	
	p = fno->fname;
	
	if (dj->sect)
	{
		dir = dj->dir;
		nt = dir[DIR_NTres];		/* NT flag */
		
		for (i = 0; i < 8; i++)
		{
			c = dir[i];
			
			if (c == ' ')
				break;
				
			if (c == NDDE)
				c = (TCHAR)DDE;
				
			if (_USE_LFN && (nt & NS_BODY) && IsUpper(c))
				c += 0x20;
				
#if _LFN_UNICODE
			if (IsDBCS1(c) && i < 7 && IsDBCS2(dir[i+1]))
				c = (c << 8) | dir[++i];
			c = ff_convert(c, 1);
			if (!c) c = '?';
#endif

			*p++ = c;
		}
		
		if (dir[8] != ' ')
		{
			*p++ = '.';
			
			for (i = 8; i < 11; i++)
			{
				c = dir[i];
				
				if (c == ' ')
					break;
					
				if (_USE_LFN && (nt & NS_EXT) && IsUpper(c))
					c += 0x20;
					
#if _LFN_UNICODE
				if (IsDBCS1(c) && i < 10 && IsDBCS2(dir[i+1]))
					c = (c << 8) | dir[++i];
				c = ff_convert(c, 1);
				if (!c) c = '?';
#endif

				*p++ = c;
			}
		}
		
		fno->fattrib = dir[DIR_Attr];				/* Attribute */
		fno->fsize = LD_DWORD(dir + DIR_FileSize);	/* Size */
		fno->fdate = LD_WORD(dir + DIR_WrtDate);		/* Date */
		fno->ftime = LD_WORD(dir + DIR_WrtTime);		/* Time */
	}
	
	*p = 0;		/* Terminate SFN str by a \0 */
}
#endif /* _FS_MINIMIZE <= 1 */

/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/
static FRESULT follow_path(DIR *dj, const TCHAR *path)
{
	FRESULT res;
	byte *dir, ns;
	
#if _FS_RPATH
	if (*path == '/' || *path == '\\')
	{
		path++;
		dj->sclust = 0;
	}
	else
		dj->sclust = dj->fs->cdir;
#else
	if (*path == '/' || *path == '\\')
		path++;
		
	dj->sclust = 0;						/* Start from the root dir */
#endif

	if ((uInt)*path < ' ')
	{
		res = dir_sdi(dj, 0);
		
		dj->dir = 0;
	}
	else
	{
		for (;;)
		{
			res = create_name(dj, &path);	/* Get a segment */
			
			if (res != FR_OK)
				break;
				
			res = dir_find(dj);				/* Find it */
			
			ns = *(dj->fn+NS);
			
			if (res != FR_OK)
			{
				if (res != FR_NO_FILE)
					break;	/* Abort if any hard error occured */
					
				if (_FS_RPATH && (ns & NS_DOT))
				{
					dj->sclust = 0;
					dj->dir = 0;	/* It is the root dir */
					res = FR_OK;
					
					if (!(ns & NS_LAST))
						continue;
				}
				else
					if (!(ns & NS_LAST))
						res = FR_NO_PATH;
						
				break;
			}
			
			if (ns & NS_LAST)
				break;			/* Last segment match. Function completed. */
				
			dir = dj->dir;						/* There is next segment. Follow the sub directory */
			
			if (!(dir[DIR_Attr] & AM_DIR))
			{
				res = FR_NO_PATH;
				break;
			}
			
			dj->sclust = LD_CLUST(dir);
		}
	}
	
	return res;
}

/*-----------------------------------------------------------------------*/
/* Load a sector and check if it is an FAT Volume Boot Record            */
/*-----------------------------------------------------------------------*/
static byte check_fs(FATFS *fs, DWord sect)
{
	if (disk_read(fs->win, sect, 1) != RES_OK)	/* Load boot record */
		return 3;
		
	if (LD_WORD(&fs->win[BS_55AA]) != 0xAA55)		/* Check record signature (always placed at offset 510 even if the sector size is >512) */
		return 2;
		
	if ((LD_DWORD(&fs->win[BS_FilSysType]) & 0xFFFFFF) == 0x544146)	/* Check "FAT" string */
		return 0;
		
	if ((LD_DWORD(&fs->win[BS_FilSysType32]) & 0xFFFFFF) == 0x544146)
		return 0;
		
	return 1;
}

/*-----------------------------------------------------------------------*/
/* Check if the file system object is valid or not                       */
/*-----------------------------------------------------------------------*/
static FRESULT chk_mounted(const TCHAR **path, FATFS **rfs, byte chk_wp)
{
	byte fmt, b, pi, *tbl;
	uInt vol;
	DSTATUS stat;
	DWord bsect, fasize, tsect, sysect, nclst, szbfat;
	word nrsv;
	const TCHAR *p = *path;
	FATFS *fs;
	
	vol = p[0] - '0';					/* Is there a drive number? */
	
	if (vol <= 9 && p[1] == ':')
	{
		p += 2;
		*path = p;				/* Return pointer to the path name */
	}
	else
	{
#if _FS_RPATH
		vol = CurrVol;					/* Use current drive */
#else
		vol = 0;						/* Use drive 0 */
#endif
	}
	
	if (vol >= _VOLUMES) 				/* Is the drive number valid? */
		return FR_INVALID_DRIVE;
		
	*rfs = fs = FatFs[vol];				/* Return pointer to the corresponding file system object */
	
	if (!fs)
		return FR_NOT_ENABLED;		/* Is the file system object available? */
		
	ENTER_FF(fs);						/* Lock file system */
	
	if (fs->fs_type)
	{
		stat = disk_status();
		
		if (!(stat & STA_NOINIT))
		{
			if (!_FS_READONLY && chk_wp && (stat & STA_PROTECT))	/* Check write protection if needed */
				return FR_WRITE_PROTECTED;
				
			return FR_OK;				/* The file system object is valid */
		}
	}
	
	fs->fs_type = 0;					/* Clear the file system object */
	fs->drv = LD2PD(vol);				/* Bind the logical drive and a physical drive */
	
	stat = disk_initialize();	/* Initialize low level disk I/O layer */
	
	if (stat & STA_NOINIT)				/* Check if the initialization succeeded */
		return FR_NOT_READY;			/* Failed to initialize due to no media or hard error */
		
	if (!_FS_READONLY && chk_wp && (stat & STA_PROTECT))	/* Check disk write protection if needed */
		return FR_WRITE_PROTECTED;
		
#if _MAX_SS != 512						/* Get disk sector size (variable sector size cfg only) */
	if (disk_ioctl(GET_SECTOR_SIZE, &fs->ssize) != RES_OK)
		return FR_DISK_ERR;
#endif

	fmt = check_fs(fs, bsect = 0);		/* Load sector 0 and check if it is an FAT-VBR (in SFD) */
	
	if (LD2PT(vol) && !fmt)
		fmt = 1;	/* Force non-SFD if the volume is forced partition */
		
	if (fmt == 1)
	{
		pi = LD2PT(vol);
		
		if (pi)
			pi--;
			
		tbl = &fs->win[MBR_Table + pi * SZ_PTE];/* Partition table */
		
		if (tbl[4])
		{
			bsect = LD_DWORD(&tbl[8]);		/* Partition offset in LBA */
			
			fmt = check_fs(fs, bsect);		/* Check the partition */
		}
	}
	
	if (fmt == 3)
		return FR_DISK_ERR;
		
	if (fmt)
		return FR_NO_FILESYSTEM;		/* No FAT volume is found */
		
	if (LD_WORD(fs->win+BPB_BytsPerSec) != SS(fs))		/* (BPB_BytsPerSec must be equal to the physical sector size) */
		return FR_NO_FILESYSTEM;
		
	fasize = LD_WORD(fs->win+BPB_FATSz16);				/* Number of sectors per FAT */
	
	if (!fasize)
		fasize = LD_DWORD(fs->win+BPB_FATSz32);
		
	fs->fsize = fasize;
	
	fs->n_fats = b = fs->win[BPB_NumFATs];				/* Number of FAT copies */
	
	if (b != 1 && b != 2)
		return FR_NO_FILESYSTEM;		/* (Must be 1 or 2) */
		
	fasize *= b;										/* Number of sectors for FAT area */
	fs->csize = b = fs->win[BPB_SecPerClus];			/* Number of sectors per cluster */
	
	if (!b || (b & (b - 1)))
		return FR_NO_FILESYSTEM;	/* (Must be power of 2) */
		
	fs->n_rootdir = LD_WORD(fs->win+BPB_RootEntCnt);	/* Number of root directory entries */
	
	if (fs->n_rootdir % (SS(fs) / SZ_DIR))
		return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must be sector aligned) */
		
	tsect = LD_WORD(fs->win+BPB_TotSec16);				/* Number of sectors on the volume */
	
	if (!tsect)
		tsect = LD_DWORD(fs->win+BPB_TotSec32);
		
	nrsv = LD_WORD(fs->win+BPB_RsvdSecCnt);				/* Number of reserved sectors */
	
	if (!nrsv)
		return FR_NO_FILESYSTEM;					/* (BPB_RsvdSecCnt must not be 0) */
		
	sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / SZ_DIR);	/* RSV+FAT+DIR */
	
	if (tsect < sysect)
		return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
		
	nclst = (tsect - sysect) / fs->csize;				/* Number of clusters */
	
	if (!nclst)
		return FR_NO_FILESYSTEM;				/* (Invalid volume size) */
		
	fmt = FS_FAT12;
	
	if (nclst >= MIN_FAT16)
		fmt = FS_FAT16;
		
	if (nclst >= MIN_FAT32)
		fmt = FS_FAT32;
		
	fs->n_fatent = nclst + 2;							/* Number of FAT entries */
	fs->database = bsect + sysect;						/* Data start sector */
	fs->fatbase = bsect + nrsv; 						/* FAT start sector */
	
	if (fmt == FS_FAT32)
	{
		if (fs->n_rootdir)
			return FR_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
			
		fs->dirbase = LD_DWORD(fs->win+BPB_RootClus);	/* Root directory start cluster */
		szbfat = fs->n_fatent * 4;						/* (Required FAT size) */
	}
	else
	{
		if (!fs->n_rootdir)
			return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
			
		fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
		szbfat = (fmt == FS_FAT16) ?					/* (Required FAT size) */
				 fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
	}
	
	if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs))	/* (BPB_FATSz must not be less than required) */
		return FR_NO_FILESYSTEM;
		
#if !_FS_READONLY
	fs->free_clust = 0xFFFFFFFF;
	fs->last_clust = 0;
	
	if (fmt == FS_FAT32)
	{
		fs->fsi_flag = 0;
		fs->fsi_sector = bsect + LD_WORD(fs->win+BPB_FSInfo);
		
		if (disk_read(fs->win, fs->fsi_sector, 1) == RES_OK &&
			LD_WORD(fs->win+BS_55AA) == 0xAA55 &&
			LD_DWORD(fs->win+FSI_LeadSig) == 0x41615252 &&
			LD_DWORD(fs->win+FSI_StrucSig) == 0x61417272)
		{
			fs->last_clust = LD_DWORD(fs->win+FSI_Nxt_Free);
			fs->free_clust = LD_DWORD(fs->win+FSI_Free_Count);
		}
	}	
#endif

	fs->fs_type = fmt;		/* FAT sub-type */
	fs->id = ++Fsid;		/* File system mount ID */
	fs->winsect = 0;		/* Invalidate sector cache */
	fs->wflag = 0;
	
#if _FS_RPATH
	fs->cdir = 0;			/* Current directory (root dir) */
#endif
#if _FS_SHARE				/* Clear file lock semaphores */
	clear_lock(fs);
#endif

	return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* Check if the file/dir object is valid or not                          */
/*-----------------------------------------------------------------------*/
static FRESULT validate(FATFS *fs, word id)
{
	if (!fs || !fs->fs_type || fs->id != id)
		return FR_INVALID_OBJECT;
		
	ENTER_FF(fs);		/* Lock file system */
	
	if (disk_status() & STA_NOINIT)
		return FR_NOT_READY;
		
	return FR_OK;
}

/*--------------------------------------------------------------------------

   Public Functions

--------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount(byte vol, FATFS *fs)
{
	FATFS *rfs;
	
	if (vol >= _VOLUMES)		/* Check if the drive number is valid */
		return FR_INVALID_DRIVE;
		
	rfs = FatFs[vol];			/* Get current fs object */
	
	if (rfs)
	{
#if _FS_SHARE
		clear_lock(rfs);
#endif
#if _FS_REENTRANT				/* Discard sync object of the current volume */
		if (!ff_del_syncobj(rfs->sobj))
			return FR_INT_ERR;	
#endif
		rfs->fs_type = 0;		/* Clear old fs object */
	}
	
	if (fs)
	{
		fs->fs_type = 0;		/* Clear new fs object */
#if _FS_REENTRANT				/* Create sync object for the new volume */
		if (!ff_cre_syncobj(vol, &fs->sobj))
			return FR_INT_ERR;
#endif
	}
	
	FatFs[vol] = fs;			/* Register new fs object */
	
	return FR_OK;
}

FRESULT f_open(FIL *fp, const TCHAR *path, byte mode)
{
	FRESULT res;
	DIR dj;
	byte *dir;
	DEF_NAMEBUF;
	
	fp->fs = 0;			/* Clear file object */
	
#if !_FS_READONLY
	mode &= FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW;
	res = chk_mounted(&path, &dj.fs, (byte)(mode & ~FA_READ));	
#else
	mode &= FA_READ;
	res = chk_mounted(&path, &dj.fs, 0);
#endif

	INIT_BUF(dj);
	
	if (res == FR_OK)
		res = follow_path(&dj, path);	/* Follow the file path */
		
	dir = dj.dir;
	
#if !_FS_READONLY	/* R/W configuration */
	if (res == FR_OK)
	{
		if (!dir)	/* Current dir itself */
			res = FR_INVALID_NAME;
#if _FS_SHARE
		else
			res = chk_lock(&dj, (mode & ~FA_READ) ? 1 : 0);
#endif
	}
	
	/* Create or Open a file */
	if (mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW))
	{
		DWord dw, cl;
		
		if (res != FR_OK)
		{
			if (res == FR_NO_FILE)
#if _FS_SHARE
				res = enq_lock() ? dir_register(&dj) : FR_TOO_MANY_OPEN_FILES;
#else
				res = dir_register(&dj);
#endif
			
			mode |= FA_CREATE_ALWAYS;		/* File is created */
			dir = dj.dir;					/* New entry */
		}
		else
		{
			if (dir[DIR_Attr] & (AM_RDO | AM_DIR))
				res = FR_DENIED;
			else
				if (mode & FA_CREATE_NEW)
					res = FR_EXIST;
		}
		
		if (res == FR_OK && (mode & FA_CREATE_ALWAYS))
		{
			dw = get_fattime;
			ST_DWORD(dir+DIR_CrtTime, dw);
			dir[DIR_Attr] = 0;					/* Reset attribute */
			ST_DWORD(dir+DIR_FileSize, 0);		/* size = 0 */
			cl = LD_CLUST(dir);					/* Get start cluster */
			ST_CLUST(dir, 0);					/* cluster = 0 */
			dj.fs->wflag = 1;
			
			if (cl)
			{
				dw = dj.fs->winsect;
				res = remove_chain(dj.fs, cl);
				
				if (res == FR_OK)
				{
					dj.fs->last_clust = cl - 1;	/* Reuse the cluster hole */
					res = move_window(dj.fs, dw);
				}
			}
		}
	}
	else
	{
		if (res == FR_OK)
		{
			if (dir[DIR_Attr] & AM_DIR)
				res = FR_NO_FILE;
			else
				if ((mode & FA_WRITE) && (dir[DIR_Attr] & AM_RDO))
					res = FR_DENIED;
		}
	}
	
	if (res == FR_OK)
	{
		if (mode & FA_CREATE_ALWAYS)
			mode |= FA__WRITTEN;
			
		fp->dir_sect = dj.fs->winsect;			/* Pointer to the directory entry */
		fp->dir_ptr = dir;
#if _FS_SHARE
		fp->lockid = inc_lock(&dj, (mode & ~FA_READ) ? 1 : 0);
		
		if (!fp->lockid)
			res = FR_INT_ERR;
#endif
	}
#else				/* R/O configuration */
	
	if (res == FR_OK)
	{
		if (!dir)
			res = FR_INVALID_NAME;
		else
			if (dir[DIR_Attr] & AM_DIR)
				res = FR_NO_FILE;
	}	
#endif
	
	FREE_BUF();
	
	if (res == FR_OK)
	{
		fp->flag = mode;					/* File access mode */
		fp->sclust = LD_CLUST(dir);			/* File start cluster */
		fp->fsize = LD_DWORD(dir+DIR_FileSize);	/* File size */
		fp->fptr = 0;						/* File pointer */
		fp->dsect = 0;
#if _USE_FASTSEEK
		fp->cltbl = 0;						/* Normal seek mode */
#endif
		fp->fs = dj.fs; fp->id = dj.fs->id;	/* Validate file object */
	}
	
	LEAVE_FF(dj.fs, res);
}

FRESULT f_read(FIL *fp, void *buff, uInt btr, uInt *br)
{
	FRESULT res;
	DWord clst, sect, remain;
	uInt rcnt, cc;
	byte csect, *rbuff = buff;
	
	*br = 0;	/* Initialize byte counter */
	
	res = validate(fp->fs, fp->id);				/* Check validity */
	
	if (res != FR_OK)
		LEAVE_FF(fp->fs, res);
		
	if (fp->flag & FA__ERROR)					/* Aborted file? */
		LEAVE_FF(fp->fs, FR_INT_ERR);
		
	if (!(fp->flag & FA_READ)) 					/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
		
	remain = fp->fsize - fp->fptr;
	
	if (btr > remain)
		btr = (uInt)remain;		/* Truncate btr by remaining bytes */
		
	for ( ;  btr; rbuff += rcnt, fp->fptr += rcnt, *br += rcnt, btr -= rcnt)
	{
		if ((fp->fptr % SS(fp->fs)) == 0)
		{
			csect = (byte)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));
			
			if (!csect)
			{
				if (fp->fptr == 0)
					clst = fp->sclust;
				else
				{
#if _USE_FASTSEEK
					if (fp->cltbl)
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					else
#endif
						clst = get_fat(fp->fs, fp->clust);	/* Follow cluster chain on the FAT */
				}
				
				if (clst < 2)
					ABORT(fp->fs, FR_INT_ERR);
					
				if (clst == 0xFFFFFFFF)
					ABORT(fp->fs, FR_DISK_ERR);
					
				fp->clust = clst;				/* Update current cluster */
			}
			
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			
			if (!sect)
				ABORT(fp->fs, FR_INT_ERR);
				
			sect += csect;
			cc = btr / SS(fp->fs);				/* When remaining bytes >= sector size, */
			
			if (cc)
			{
				if (csect + cc > fp->fs->csize)
					cc = fp->fs->csize - csect;
					
				if (disk_read(rbuff, sect, (byte)cc) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
					
#if !_FS_READONLY && _FS_MINIMIZE <= 2			/* Replace one of the read sectors with cached data if it contains a dirty sector */
#if _FS_TINY
				if (fp->fs->wflag && fp->fs->winsect - sect < cc)
					memcpy(rbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), fp->fs->win, SS(fp->fs));
#else
				if ((fp->flag & FA__DIRTY) && fp->dsect - sect < cc)
					memcpy(rbuff + ((fp->dsect - sect) * SS(fp->fs)), fp->buf, SS(fp->fs));
#endif
#endif
				rcnt = SS(fp->fs) * cc;			/* Number of bytes transferred */
				continue;
			}
#if !_FS_TINY
			if (fp->dsect != sect)
			{
#if !_FS_READONLY
				if (fp->flag & FA__DIRTY)
				{
					if (disk_write(fp->buf, fp->dsect, 1) != RES_OK)
						ABORT(fp->fs, FR_DISK_ERR);
						
					fp->flag &= ~FA__DIRTY;
				}
#endif
				if (disk_read(fp->buf, sect, 1) != RES_OK)	/* Fill sector cache */
					ABORT(fp->fs, FR_DISK_ERR);
			}
#endif
			fp->dsect = sect;
		}
		
		rcnt = SS(fp->fs) - (fp->fptr % SS(fp->fs));	/* Get partial sector data from sector buffer */
		
		if (rcnt > btr)
			rcnt = btr;
			
#if _FS_TINY
		if (move_window(fp->fs, fp->dsect))		/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
			
		memcpy(rbuff, &fp->fs->win[fp->fptr % SS(fp->fs)], rcnt);	/* Pick partial sector */
#else
		memcpy(rbuff, &fp->buf[fp->fptr % SS(fp->fs)], rcnt);	/* Pick partial sector */
#endif
	}
	
	LEAVE_FF(fp->fs, FR_OK);
}

#if !_FS_READONLY
FRESULT f_write(FIL *fp, const void *buff, uInt btw, uInt *bw)
{
	FRESULT res;
	DWord clst, sect;
	uInt wcnt, cc;
	const byte *wbuff = buff;
	byte csect;	
	
	*bw = 0;	/* Initialize byte counter */
	
	res = validate(fp->fs, fp->id);			/* Check validity */
	
	if (res != FR_OK)
		LEAVE_FF(fp->fs, res);
		
	if (fp->flag & FA__ERROR)				/* Aborted file? */
		LEAVE_FF(fp->fs, FR_INT_ERR);
		
	if (!(fp->flag & FA_WRITE))				/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
		
	if ((DWord)(fp->fsize + btw) < fp->fsize)
		btw = 0;	/* File size cannot reach 4GB */
		
	for ( ;  btw; wbuff += wcnt, fp->fptr += wcnt, *bw += wcnt, btw -= wcnt)
	{
		if ((fp->fptr % SS(fp->fs)) == 0)
		{
			csect = (byte)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));	/* Sector offset in the cluster */
			
			if (!csect)
			{
				if (fp->fptr == 0)
				{
					clst = fp->sclust;
					
					if (clst == 0)			/* When no cluster is allocated, */
						fp->sclust = clst = create_chain(fp->fs, 0);	/* Create a new cluster chain */
				}
				else
				{
#if _USE_FASTSEEK
					if (fp->cltbl)
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					else
#endif
						clst = create_chain(fp->fs, fp->clust);	/* Follow or stretch cluster chain on the FAT */
				}
				
				if (clst == 0)
					break;		/* Could not allocate a new cluster (disk full) */
					
				if (clst == 1)
					ABORT(fp->fs, FR_INT_ERR);
					
				if (clst == 0xFFFFFFFF)
					ABORT(fp->fs, FR_DISK_ERR);
					
				fp->clust = clst;			/* Update current cluster */
			}
#if _FS_TINY
			if (fp->fs->winsect == fp->dsect && move_window(fp->fs, 0))	/* Write-back sector cache */
				ABORT(fp->fs, FR_DISK_ERR);
#else
			if (fp->flag & FA__DIRTY)
			{
				if (disk_write(fp->buf, fp->dsect, 1) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
					
				fp->flag &= ~FA__DIRTY;
			}
#endif
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			
			if (!sect)
				ABORT(fp->fs, FR_INT_ERR);
				
			sect += csect;
			cc = btw / SS(fp->fs);			/* When remaining bytes >= sector size, */
			
			if (cc)
			{
				if (csect + cc > fp->fs->csize)	/* Clip at cluster boundary */
					cc = fp->fs->csize - csect;
					
				if (disk_write(wbuff, sect, (byte)cc) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
#if _FS_TINY
				if (fp->fs->winsect - sect < cc)
				{
					memcpy(fp->fs->win, wbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), SS(fp->fs));
					
					fp->fs->wflag = 0;
				}
#else
				if (fp->dsect - sect < cc)
				{
					memcpy(fp->buf, wbuff + ((fp->dsect - sect) * SS(fp->fs)), SS(fp->fs));
					
					fp->flag &= ~FA__DIRTY;
				}
#endif
				wcnt = SS(fp->fs) * cc;		/* Number of bytes transferred */
				continue;
			}
			
#if _FS_TINY
			if (fp->fptr >= fp->fsize)
			{
				if (move_window(fp->fs, 0))
					ABORT(fp->fs, FR_DISK_ERR);
					
				fp->fs->winsect = sect;
			}
#else
			if (fp->dsect != sect)
			{
				if (fp->fptr < fp->fsize &&
					disk_read(fp->buf, sect, 1) != RES_OK)
						ABORT(fp->fs, FR_DISK_ERR);
			}
#endif
			fp->dsect = sect;
		}
		
		wcnt = SS(fp->fs) - (fp->fptr % SS(fp->fs));/* Put partial sector into file I/O buffer */
		
		if (wcnt > btw)
			wcnt = btw;
			
#if _FS_TINY
		if (move_window(fp->fs, fp->dsect))	/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
			
		memcpy(&fp->fs->win[fp->fptr % SS(fp->fs)], wbuff, wcnt);	/* Fit partial sector */
		fp->fs->wflag = 1;
#else
		memcpy(&fp->buf[fp->fptr % SS(fp->fs)], wbuff, wcnt);	/* Fit partial sector */
		fp->flag |= FA__DIRTY;
#endif
	}
	
	if (fp->fptr > fp->fsize)
		fp->fsize = fp->fptr;	/* Update file size if needed */
		
	fp->flag |= FA__WRITTEN;						/* Set file change flag */
	
	LEAVE_FF(fp->fs, FR_OK);
}

FRESULT f_sync(FIL *fp)
{
	FRESULT res;
	DWord tim;
	byte *dir;
	
	res = validate(fp->fs, fp->id);		/* Check validity of the object */
	
	if (res == FR_OK)
	{
		if (fp->flag & FA__WRITTEN)
		{
#if !_FS_TINY	/* Write-back dirty buffer */
			if (fp->flag & FA__DIRTY)
			{
				if (disk_write(fp->buf, fp->dsect, 1) != RES_OK)
					LEAVE_FF(fp->fs, FR_DISK_ERR);
					
				fp->flag &= ~FA__DIRTY;
			}
#endif

			res = move_window(fp->fs, fp->dir_sect);
			
			if (res == FR_OK)
			{
				dir = fp->dir_ptr;
				dir[DIR_Attr] |= AM_ARC;					/* Set archive bit */
				ST_DWORD(dir+DIR_FileSize, fp->fsize);		/* Update file size */
				ST_CLUST(dir, fp->sclust);					/* Update start cluster */
				tim = get_fattime;						/* Update updated time */
				ST_DWORD(dir+DIR_WrtTime, tim);
				fp->flag &= ~FA__WRITTEN;
				fp->fs->wflag = 1;
				res = sync(fp->fs);
			}
		}
	}
	
	LEAVE_FF(fp->fs, res);
}

#endif

FRESULT f_close(FIL *fp)
{
	FRESULT res;
	
#if _FS_READONLY
	FATFS *fs = fp->fs;
	res = validate(fs, fp->id);
	
	if (res == FR_OK)
		fp->fs = 0;	/* Discard file object */
		
	LEAVE_FF(fs, res);
#else
	res = f_sync(fp);		/* Flush cached data */
#if _FS_SHARE
	if (res == FR_OK)
	{
#if _FS_REENTRANT
		res = validate(fp->fs, fp->id);
		
		if (res == FR_OK)
		{
			res = dec_lock(fp->lockid);	
			
			unlock_fs(fp->fs, FR_OK);
		}
#else
		res = dec_lock(fp->lockid);
#endif
	}
#endif
	if (res == FR_OK)
		fp->fs = 0;	/* Discard file object */
		
	return res;
#endif
}

#if _FS_RPATH >= 1
FRESULT f_chdir(const TCHAR *path)
{
	FRESULT res;
	DIR dj;
	DEF_NAMEBUF;
	
	res = chk_mounted(&path, &dj.fs, 0);
	
	if (res == FR_OK)
	{
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the path */
		FREE_BUF();
		
		if (res == FR_OK)
		{
			if (!dj.dir)
				dj.fs->cdir = dj.sclust;	/* Start directory itself */
			else
			{
				if (dj.dir[DIR_Attr] & AM_DIR)	/* Reached to the directory */
					dj.fs->cdir = LD_CLUST(dj.dir);
				else
					res = FR_NO_PATH;		/* Reached but a file */
			}
		}
		
		if (res == FR_NO_FILE)
			res = FR_NO_PATH;
	}
	
	LEAVE_FF(dj.fs, res);
}

#if _FS_RPATH >= 2
FRESULT f_getcwd(TCHAR *path, uInt sz_path)
{
	FRESULT res;
	DIR dj;
	uInt i, n;
	DWord ccl;
	TCHAR *tp;
	FILINFO fno;
	DEF_NAMEBUF;
	
	*path = 0;
	res = chk_mounted((const TCHAR**)&path, &dj.fs, 0);	/* Get current volume */
	
	if (res == FR_OK)
	{
		INIT_BUF(dj);
		i = sz_path;		/* Bottom of buffer (dir stack base) */
		dj.sclust = dj.fs->cdir;			/* Start to follow upper dir from current dir */
		
		while ((ccl = dj.sclust) != 0)
		{
			res = dir_sdi(&dj, 1);			/* Get parent dir */
			
			if (res != FR_OK)
				break;
				
			res = dir_read(&dj);
			
			if (res != FR_OK)
				break;
				
			dj.sclust = LD_CLUST(dj.dir);	/* Goto parent dir */
			res = dir_sdi(&dj, 0);
			
			if (res != FR_OK)
				break;
				
			do 
			{
				res = dir_read(&dj);
				
				if (res != FR_OK)
					break;
					
				if (ccl == LD_CLUST(dj.dir))
					break;	/* Found the entry */
					
				res = dir_next(&dj, 0);	
			} while (res == FR_OK);
			
			if (res == FR_NO_FILE)
				res = FR_INT_ERR;/* It cannot be 'not found'. */
				
			if (res != FR_OK)
				break;
				
			get_fileinfo(&dj, &fno);		/* Get the dir name and push it to the buffer */
			tp = fno.fname;
			
			if (_USE_LFN && *path)
				tp = path;
				
			for (n = 0; tp[n]; n++) ;
			
			if (i < n + 3)
			{
				res = FR_NOT_ENOUGH_CORE;
				break;
			}
			
			while (n)
				path[--i] = tp[--n];
				
			path[--i] = '/';
		}
		
		tp = path;
		
		if (res == FR_OK)
		{
			*tp++ = '0' + CurrVol;			/* Put drive number */
			*tp++ = ':';
			
			if (i == sz_path)
				*tp++ = '/';
			else
			{
				do		/* Add stacked path str */
					*tp++ = path[i++];
				while (i < sz_path);
			}
		}
		
		*tp = 0;
		FREE_BUF();
	}
	
	LEAVE_FF(dj.fs, res);
}
#endif

#endif

#if _FS_MINIMIZE <= 2
FRESULT f_lseek(FIL *fp, DWord ofs)
{
	FRESULT res;
	
	res = validate(fp->fs, fp->id);		/* Check validity of the object */
	
	if (res != FR_OK)
		LEAVE_FF(fp->fs, res);
		
	if (fp->flag & FA__ERROR)			/* Check abort flag */
		LEAVE_FF(fp->fs, FR_INT_ERR);
		
#if _USE_FASTSEEK
	if (fp->cltbl)
	{
		DWord cl, pcl, ncl, tcl, dsc, tlen, ulen, *tbl;
		
		if (ofs == CREATE_LINKMAP)
		{
			tbl = fp->cltbl;
			tlen = *tbl++; ulen = 2;	/* Given table size and required table size */
			cl = fp->sclust;			/* Top of the chain */
			
			if (cl)
			{
				do 
				{
					tcl = cl; ncl = 0; ulen += 2;	/* Top, length and used items */
					
					do 
					{
						pcl = cl;
						ncl++;
						
						cl = get_fat(fp->fs, cl);
						
						if (cl <= 1)
							ABORT(fp->fs, FR_INT_ERR);
							
						if (cl == 0xFFFFFFFF)
							ABORT(fp->fs, FR_DISK_ERR);
					} while (cl == pcl + 1);
					
					if (ulen <= tlen)
					{
						*tbl++ = ncl;
						*tbl++ = tcl;
					}
					
				} while (cl < fp->fs->n_fatent);
			}
			
			*fp->cltbl = ulen;	/* Number of items used */
			
			if (ulen <= tlen)
				*tbl = 0;		/* Terminate table */
			else
				res = FR_NOT_ENOUGH_CORE;	/* Given table size is smaller than required */
		}
		else
		{
			if (ofs > fp->fsize)		/* Clip offset at the file size */
				ofs = fp->fsize;
				
			fp->fptr = ofs;				/* Set file pointer */
			
			if (ofs)
			{
				fp->clust = clmt_clust(fp, ofs - 1);
				dsc = clust2sect(fp->fs, fp->clust);
				
				if (!dsc)
					ABORT(fp->fs, FR_INT_ERR);
					
				dsc += (ofs - 1) / SS(fp->fs) & (fp->fs->csize - 1);
				
				if (fp->fptr % SS(fp->fs) && dsc != fp->dsect)
				{
#if !_FS_TINY
#if !_FS_READONLY
					if (fp->flag & FA__DIRTY)
					{
						if (disk_write(fp->buf, fp->dsect, 1) != RES_OK)
							ABORT(fp->fs, FR_DISK_ERR);
							
						fp->flag &= ~FA__DIRTY;
					}
#endif
					if (disk_read(fp->buf, dsc, 1) != RES_OK)	/* Load current sector */
						ABORT(fp->fs, FR_DISK_ERR);
#endif
					fp->dsect = dsc;
				}
			}
		}
	}
	else
#endif
	{
		DWord clst, bcs, nsect, ifptr;
		
		if (ofs > fp->fsize					/* In read-only mode, clip offset with the file size */
#if !_FS_READONLY
			 && !(fp->flag & FA_WRITE)
#endif
			) ofs = fp->fsize;
			
		ifptr = fp->fptr;
		fp->fptr = nsect = 0;
		
		if (ofs)
		{
			bcs = (DWord)fp->fs->csize * SS(fp->fs);	/* Cluster size (byte) */
			
			if (ifptr > 0 && (ofs - 1) / bcs >= (ifptr - 1) / bcs)
			{
				fp->fptr = (ifptr - 1) & ~(bcs - 1);	/* start from the current cluster */
				ofs -= fp->fptr;
				clst = fp->clust;
			}
			else
			{
				clst = fp->sclust;						/* start from the first cluster */
#if !_FS_READONLY
				if (clst == 0)
				{
					clst = create_chain(fp->fs, 0);
					
					if (clst == 1)
						ABORT(fp->fs, FR_INT_ERR);
						
					if (clst == 0xFFFFFFFF)
						ABORT(fp->fs, FR_DISK_ERR);
						
					fp->sclust = clst;
				}
#endif
				fp->clust = clst;
			}
			
			if (clst != 0)
			{
				while (ofs > bcs)
				{
#if !_FS_READONLY
					if (fp->flag & FA_WRITE)
					{
						clst = create_chain(fp->fs, clst);	/* Force stretch if in write mode */
						
						if (clst == 0)
						{
							ofs = bcs;
							break;
						}
					}
					else
#endif
					clst = get_fat(fp->fs, clst);	/* Follow cluster chain if not in write mode */
					
					if (clst == 0xFFFFFFFF)
						ABORT(fp->fs, FR_DISK_ERR);
						
					if (clst <= 1 || clst >= fp->fs->n_fatent)
						ABORT(fp->fs, FR_INT_ERR);
						
					fp->clust = clst;
					fp->fptr += bcs;
					ofs -= bcs;
				}
				
				fp->fptr += ofs;
				
				if (ofs % SS(fp->fs))
				{
					nsect = clust2sect(fp->fs, clst);	/* Current sector */
					
					if (!nsect)
						ABORT(fp->fs, FR_INT_ERR);
						
					nsect += ofs / SS(fp->fs);
				}
			}
		}
		
		if (fp->fptr % SS(fp->fs) && nsect != fp->dsect)
		{
#if !_FS_TINY
#if !_FS_READONLY
			if (fp->flag & FA__DIRTY)
			{
				if (disk_write(fp->buf, fp->dsect, 1) != RES_OK)
					ABORT(fp->fs, FR_DISK_ERR);
					
				fp->flag &= ~FA__DIRTY;
			}
#endif
			if (disk_read(fp->buf, nsect, 1) != RES_OK)	/* Fill sector cache */
				ABORT(fp->fs, FR_DISK_ERR);
#endif
			fp->dsect = nsect;
		}
		
#if !_FS_READONLY
		if (fp->fptr > fp->fsize)
		{
			fp->fsize = fp->fptr;
			fp->flag |= FA__WRITTEN;
		}
#endif
	}
	
	LEAVE_FF(fp->fs, res);
}

#if _FS_MINIMIZE <= 1
FRESULT f_opendir(DIR *dj, const TCHAR *path)
{
	FRESULT res;
	DEF_NAMEBUF;
	
	res = chk_mounted(&path, &dj->fs, 0);
	
	if (res == FR_OK)
	{
		INIT_BUF(*dj);
		
		res = follow_path(dj, path);			/* Follow the path to the directory */
		
		FREE_BUF();
		
		if (res == FR_OK)
		{
			if (dj->dir)
			{
				if (dj->dir[DIR_Attr] & AM_DIR)
					{
						dj->sclust = LD_CLUST(dj->dir);
					}
					else
						res = FR_NO_PATH;
			}
			
			if (res == FR_OK)
			{
				dj->id = dj->fs->id;
				res = dir_sdi(dj, 0);			/* Rewind dir */
			}
		}
		
		if (res == FR_NO_FILE)
			res = FR_NO_PATH;
	}
	
	LEAVE_FF(dj->fs, res);
}

FRESULT f_readdir(DIR *dj, FILINFO *fno)
{
	FRESULT res;
	DEF_NAMEBUF;
	
	res = validate(dj->fs, dj->id);			/* Check validity of the object */
	
	if (res == FR_OK)
	{
		if (!fno)
			res = dir_sdi(dj, 0);			/* Rewind the directory object */
		else
		{
			INIT_BUF(*dj);
			res = dir_read(dj);				/* Read an directory item */
			
			if (res == FR_NO_FILE)
			{
				dj->sect = 0;
				res = FR_OK;
			}
			
			if (res == FR_OK)
			{
				get_fileinfo(dj, fno);		/* Get the object information */
				res = dir_next(dj, 0);		/* Increment index for next */
				
				if (res == FR_NO_FILE)
				{
					dj->sect = 0;
					res = FR_OK;
				}
			}
			
			FREE_BUF();
		}
	}
	
	LEAVE_FF(dj->fs, res);
}

#if _FS_MINIMIZE == 0
FRESULT f_stat(const TCHAR *path, FILINFO *fno)
{
	FRESULT res;
	DIR dj;
	DEF_NAMEBUF;
	
	res = chk_mounted(&path, &dj.fs, 0);
	
	if (res == FR_OK)
	{
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		
		if (res == FR_OK)
		{
			if (dj.dir)
				get_fileinfo(&dj, fno);
			else
				res = FR_INVALID_NAME;
		}
		
		FREE_BUF();
	}
	
	LEAVE_FF(dj.fs, res);
}

#if !_FS_READONLY
FRESULT f_truncate(FIL *fp)
{
	FRESULT res;
	DWord ncl;
	
	res = validate(fp->fs, fp->id);		/* Check validity of the object */
	
	if (res == FR_OK)
	{
		if (fp->flag & FA__ERROR)
			res = FR_INT_ERR;
		else
		{
			if (!(fp->flag & FA_WRITE))		/* Check access mode */
				res = FR_DENIED;
		}
	}
	
	if (res == FR_OK)
	{
		if (fp->fsize > fp->fptr)
		{
			fp->fsize = fp->fptr;	/* Set file size to current R/W point */
			fp->flag |= FA__WRITTEN;
			
			if (fp->fptr == 0)
			{
				res = remove_chain(fp->fs, fp->sclust);
				fp->sclust = 0;
			}
			else
			{
				ncl = get_fat(fp->fs, fp->clust);
				res = FR_OK;
				
				if (ncl == 0xFFFFFFFF)
					res = FR_DISK_ERR;
					
				if (ncl == 1)
					res = FR_INT_ERR;
					
				if (res == FR_OK && ncl < fp->fs->n_fatent)
				{
					res = put_fat(fp->fs, fp->clust, 0x0FFFFFFF);
					
					if (res == FR_OK)
						res = remove_chain(fp->fs, ncl);
				}
			}
		}
		
		if (res != FR_OK)
			fp->flag |= FA__ERROR;
	}
	
	LEAVE_FF(fp->fs, res);
}

FRESULT f_unlink(const TCHAR *path)
{
	FRESULT res;
	DIR dj, sdj;
	byte *dir;
	DWord dclst;
	DEF_NAMEBUF;
	
	res = chk_mounted(&path, &dj.fs, 1);
	
	if (res == FR_OK)
	{
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the file path */
		
		if (_FS_RPATH && res == FR_OK && (dj.fn[NS] & NS_DOT))
			res = FR_INVALID_NAME;			/* Cannot remove dot entry */
			
#if _FS_SHARE
		if (res == FR_OK)
			res = chk_lock(&dj, 2);	/* Cannot remove open file */
			
#endif
		if (res == FR_OK)
		{
			dir = dj.dir;
			
			if (!dir)
				res = FR_INVALID_NAME;		/* Cannot remove the start directory */
			else
			{
				if (dir[DIR_Attr] & AM_RDO)
					res = FR_DENIED;		/* Cannot remove R/O object */
			}
			
			dclst = LD_CLUST(dir);
			
			if (res == FR_OK && (dir[DIR_Attr] & AM_DIR))
			{
				if (dclst < 2)
					res = FR_INT_ERR;
				else
				{
					memcpy(&sdj, &dj, sizeof(DIR));	/* Check if the sub-dir is empty or not */
					sdj.sclust = dclst;
					res = dir_sdi(&sdj, 2);		/* Exclude dot entries */
					
					if (res == FR_OK)
					{
						res = dir_read(&sdj);
						
						if (res == FR_OK			/* Not empty dir */
#if _FS_RPATH
						|| dclst == sdj.fs->cdir	/* Current dir */
#endif
						) res = FR_DENIED;
						
						if (res == FR_NO_FILE)
							res = FR_OK;	/* Empty */
					}
				}
			}
			
			if (res == FR_OK)
			{
				res = dir_remove(&dj);		/* Remove the directory entry */
				
				if (res == FR_OK)
				{
					if (dclst)				/* Remove the cluster chain if exist */
						res = remove_chain(dj.fs, dclst);
						
					if (res == FR_OK) res = sync(dj.fs);
				}
			}
		}
		
		FREE_BUF();
	}
	
	LEAVE_FF(dj.fs, res);
}

FRESULT	f_mkdir(const TCHAR *path)
{
	FRESULT res;
	DIR dj;
	byte *dir, n;
	DWord dsc, dcl, pcl, tim = get_fattime;
	DEF_NAMEBUF;
	
	res = chk_mounted(&path, &dj.fs, 1);
	
	if (res == FR_OK)
	{
		INIT_BUF(dj);
		res = follow_path(&dj, path);			/* Follow the file path */
		
		if (res == FR_OK)
			res = FR_EXIST;		/* Any object with same name is already existing */		
			
		if (_FS_RPATH && res == FR_NO_FILE && (dj.fn[NS] & NS_DOT))
			res = FR_INVALID_NAME;
			
		if (res == FR_NO_FILE)
		{
			dcl = create_chain(dj.fs, 0);		/* Allocate a cluster for the new directory table */
			res = FR_OK;
			
			if (dcl == 0)
				res = FR_DENIED;		/* No space to allocate a new cluster */
				
			if (dcl == 1)
				res = FR_INT_ERR;
				
			if (dcl == 0xFFFFFFFF)
				res = FR_DISK_ERR;
				
			if (res == FR_OK)					/* Flush FAT */
				res = move_window(dj.fs, 0);
				
			if (res == FR_OK)
			{
				dsc = clust2sect(dj.fs, dcl);
				dir = dj.fs->win;
				memset(dir, 0, SS(dj.fs));
				memset(dir+DIR_Name, ' ', 8+3);	/* Create "." entry */
				dir[DIR_Name] = '.';
				dir[DIR_Attr] = AM_DIR;
				ST_DWORD(dir+DIR_WrtTime, tim);
				ST_CLUST(dir, dcl);
				memcpy(dir+SZ_DIR, dir, SZ_DIR); 	/* Create ".." entry */
				dir[33] = '.'; pcl = dj.sclust;
				
				if (dj.fs->fs_type == FS_FAT32 && pcl == dj.fs->dirbase)
					pcl = 0;
					
				ST_CLUST(dir+SZ_DIR, pcl);
				
				for (n = dj.fs->csize; n; n--)
				{
					dj.fs->winsect = dsc++;
					dj.fs->wflag = 1;
					
					res = move_window(dj.fs, 0);
					
					if (res != FR_OK)
						break;
						
					memset(dir, 0, SS(dj.fs));
				}
			}
			
			if (res == FR_OK)
				res = dir_register(&dj);	/* Register the object to the directoy */
				
			if (res != FR_OK)
				remove_chain(dj.fs, dcl);			/* Could not register, remove cluster chain */
			else
			{
				dir = dj.dir;
				dir[DIR_Attr] = AM_DIR;				/* Attribute */
				ST_DWORD(dir+DIR_WrtTime, tim);		/* Created time */
				ST_CLUST(dir, dcl);					/* Table start cluster */
				dj.fs->wflag = 1;
				res = sync(dj.fs);
			}
		}
		
		FREE_BUF();
	}
	
	LEAVE_FF(dj.fs, res);
}

FRESULT f_rename(const TCHAR *path_old, const TCHAR *path_new)
{
	FRESULT res;
	DIR djo, djn;
	byte buf[21], *dir;
	DWord dw;
	DEF_NAMEBUF;
	
	res = chk_mounted(&path_old, &djo.fs, 1);
	
	if (res == FR_OK)
	{
		djn.fs = djo.fs;
		INIT_BUF(djo);
		res = follow_path(&djo, path_old);		/* Check old object */
		
		if (_FS_RPATH && res == FR_OK && (djo.fn[NS] & NS_DOT))
			res = FR_INVALID_NAME;
			
#if _FS_SHARE
		if (res == FR_OK)
			res = chk_lock(&djo, 2);
#endif
		if (res == FR_OK)
		{
			if (!djo.dir)
				res = FR_NO_FILE;
			else
			{
				memcpy(buf, djo.dir+DIR_Attr, 21);		/* Save the object information except for name */
				memcpy(&djn, &djo, sizeof(DIR));		/* Check new object */
				res = follow_path(&djn, path_new);
				
				if (res == FR_OK)
					res = FR_EXIST;
					
				if (res == FR_NO_FILE)
				{
					res = dir_register(&djn);			/* Register the new entry */
					
					if (res == FR_OK)
					{
						dir = djn.dir;					/* Copy object information except for name */
						memcpy(dir+13, buf+2, 19);
						dir[DIR_Attr] = buf[0] | AM_ARC;
						djo.fs->wflag = 1;
						
						if (djo.sclust != djn.sclust && (dir[DIR_Attr] & AM_DIR))
						{
							dw = clust2sect(djn.fs, LD_CLUST(dir));
							
							if (!dw)
								res = FR_INT_ERR;
							else
							{
								res = move_window(djn.fs, dw);
								dir = djn.fs->win+SZ_DIR;	/* .. entry */
								
								if (res == FR_OK && dir[1] == '.')
								{
									dw = (djn.fs->fs_type == FS_FAT32 && djn.sclust == djn.fs->dirbase) ? 0 : djn.sclust;
									ST_CLUST(dir, dw);
									djn.fs->wflag = 1;
								}
							}
						}
						
						if (res == FR_OK)
						{
							res = dir_remove(&djo);		/* Remove old entry */
							
							if (res == FR_OK)
								res = sync(djo.fs);
						}
					}
				}
			}
		}
		
		FREE_BUF();
	}
	
	LEAVE_FF(djo.fs, res);
}

#endif

#endif

#endif

#endif