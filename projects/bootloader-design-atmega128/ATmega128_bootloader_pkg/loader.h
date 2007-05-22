/*
*   loader.h
*/

#ifndef LOADER_H
#define LOADER_H


#define  FLASH_PAGE_SIZE        (256)  // bytes
#define  FLASH_PAGE_MASK        (FLASH_PAGE_SIZE - 1)
#define  BLANK_FLASH_BYTE       (0xFF)
#define  BLANK_FLASH_WORD       (0xFFFF)

#define  APPL_CODE_ENTRY_ADDR     (0)
#define  APPL_CODE_BEGIN_ADDR     (0)
#define  APPL_CODE_TOP_PAGE_ADDR  (0x1E000-0x100)
#define  APPL_VERSION_NUM_ADDR    (0x1DFF0)
#define  APPL_CODE_CHKSM_ADDR     (0x1DFFE)
#define  APPL_CODE_END_ADDR       (0x1DFFF)
#define  BOOT_CODE_ENTRY_ADDR     (0x1E000)


/*** Loader Error Flags ***/
#define  LDR_ERR_COMMAND_UNK    BIT_0
#define  LDR_ERR_ERASE_FAIL     BIT_1
#define  LDR_ERR_WRITE_FAIL     BIT_2
#define  LDR_ERR_ADDR_RANGE     BIT_3
#define  LDR_ERR_APPL_CORRUPT   BIT_4
#define  LDR_ERR_RESET_ADDR     BIT_5
#define  LDR_ERR_SREC_CKSM      BIT_6
#define  LDR_ERR_XREC_CKSM      BIT_7

extern  uint16  wLoaderError;           /* Loader error flags word (16 bits) */


void  InitBootLoader( void );
void  BootLoaderExec( char *buffer );
bool  ParseXRecord( char recordType, char *pInBuff, uint32 *pDstAddr );
uchar ParseSRecord( char *pInBuff, uchar *pOutBuff, uchar maxDataBytes, uint32 *pDstAddr );
void  write_checksum( void );

uint8  hexctobin( char c );
uint16 hexatoi( char * s, uint8 ubCount );
bool   isHexDigit( char c );
void   hexWord2str( uint16 wVal, char *pcResult );
void   hexLong2str( uint32 ulVal, char *pcResult );
char   hexitoc( short wDigit );


#endif /* LOADER_H */
