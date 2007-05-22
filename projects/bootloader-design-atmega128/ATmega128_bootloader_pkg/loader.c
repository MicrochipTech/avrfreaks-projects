/*====================================================================================== *\
*
*   File:			loader.c
*
*   Originated:     M.J.Bauer, April 2007
*                   www.hotkey.net.au/~mjbauer
*
*   Example command processor module for AVR bootloader -- ATmega128 target.
*
*   The ATmega128 on-chip flash is organised into pages of size 128K words (256k bytes).
*   Flash program memory is erased and programmed a page at a time.
*   Data to be written is first stored in a page buffer (256kB). When all the data
*   required in the page has been buffered, the buffer is "flushed" to flash memory.
*
*   To keep the bootloader code small, some compromise has been necessary.
*   The host downloader application must send S-records with a data
*   byte-count equal to a binary multiple (e.g. 8, 16, 32 bytes).
*   The download will fail if the host sends two successive S-records with data to be
*   written into different flash pages, without flushing the page buffer in between.
*
*   On successful loading of application code, the bootloader calculates and writes
*   a code checksum (word) into the reserved location APPL_CODE_CHKSM_ADDR, before
*   passing control to the application program.
*
\*====================================================================================== */


#include "gendef.h"
#include "spm.h"
#include "loader.h"


char   __farflash  hzIDN_string[] = "XYZ Company Ltd, Model XYZ3800A, Rev 1.0";
char    acIDN_string[sizeof(hzIDN_string)+2];    // RAM copy of hzIDN_string[]

uint16  wLoaderError;           // Loader error flags word (16 bits)
uint8   bCodeBuf[64];           // Array of code bytes extracted from S-record

uint8   bPageBuffer[FLASH_PAGE_SIZE];     // 256-byte page buffer
uint8  __hugeflash  *pbFlashAddr;		  // 24-bit ptr into flash memory (code BYTE addr)
uint8  __hugeflash  *pbCacheStartAddr;	  // start addr of flash page cache (code BYTE addr)


/*
*   Initialize bootloader global data --
*      copy constant data from FAR_FLASH (CODE) to RAM (DATA).
*   Erase the page buffer (fill with blank value).
*   Lock the flash bootloader section and unlock the application section.
*/
void  InitBootLoader( void )
{
	short  idx;

	strcpy_P( acIDN_string, hzIDN_string );

	for ( idx = 0 ; idx < FLASH_PAGE_SIZE ; idx++ )
	{
		bPageBuffer[idx] = BLANK_FLASH_BYTE;
	}
	wLoaderError = 0;

	spmWriteLockBits( 0xEF );
}


/*
*   Bootloader executive -- processes Flash loader commands.
*   This function is called whenever a complete command string has been received.
*   The command string is assumed to be in a buffer at the specified pointer (arg).
*/
void  BootLoaderExec( char *buffer )
{
	uchar    byteCount;      // Number of code bytes extracted from S-record
	short    idx;
	uint16   *wordPtr;
	uint32   dstAddr;
	pfnvoid  jumpAddr;
	char     zBuf[10];
	uint32   longValue;
	uint8    *pByte;
	char     cTermChar = '\n';

	switch ( buffer[0] )
	{
	case 'E':     // Erase flash page containing the specified address.

		if ( ParseXRecord( 'E', buffer, &dstAddr ) )
		{
			pbFlashAddr = (uint8 __hugeflash *) dstAddr;
			spmErasePage( pbFlashAddr );
		}
		// Erase the (RAM) page buffer -- fill with flash erased value
		for ( idx = 0 ; idx < FLASH_PAGE_SIZE ; idx++ )
		{
			bPageBuffer[idx] = BLANK_FLASH_BYTE;
		}
		break;

	case 'F':     // Flush page buffer;  write page buffer to flash

		if ( ParseXRecord( 'F', buffer, &dstAddr ) )   // Ignore dstAddr
		{
			// Erase flash page;  Copy page buffer to page cache;  Write cache to flash
			spmErasePage( pbCacheStartAddr );
			wordPtr = (uint16 *) bPageBuffer;
			pbFlashAddr = pbCacheStartAddr;
			for ( idx = 0 ; idx < FLASH_PAGE_SIZE ; idx += 2 )
			{
				spmWriteWord( pbFlashAddr, *wordPtr );
				pbFlashAddr += 2;
				wordPtr++ ;
			}
			spmProgramPage( pbCacheStartAddr );

			// Verify flash page against page buffer while erasing the page buffer
			wordPtr = (uint16 *) bPageBuffer;
			pbFlashAddr = pbCacheStartAddr;
			for ( idx = 0 ; idx < FLASH_PAGE_SIZE ; idx += 2 )
			{
				if ( PEEK_CODE_WORD( pbFlashAddr ) != *wordPtr )
					SET_BIT( wLoaderError, LDR_ERR_WRITE_FAIL );
				pbFlashAddr += 2;
				*wordPtr++ = BLANK_FLASH_WORD;
			}
		}
		break;

	case 'J':     // Jump to specified location in program memory, unconditionally

		if ( ParseXRecord( 'J', buffer, &dstAddr ) )
		{
			jumpAddr = (pfnvoid) dstAddr;
			(*jumpAddr)();
		}
		break;

	case 'G':     // Go -- jump to application program reset entry point...
			      // ... if a program is present and intact.

 		write_checksum();
		startApplication();   // If appl pgrm loaded OK, this function won't return...
		SET_BIT( wLoaderError, LDR_ERR_APPL_CORRUPT );     // otherwise, flag an error
		break;

	case 'P':     // Parameter fetch value -- a longword is returned

		if ( ParseXRecord( 'P', buffer, &dstAddr ) )
		{
			if ( dstAddr == 0 )    // Bootloader version number
			{
				longValue = ((uint32) BUILD_VER_MAJOR << 24)
				          + ((uint32) BUILD_VER_MINOR << 16)
				          + ((uint32) BUILD_VER_DEBUG);
				wLoaderError = 0;
			}
			else if ( dstAddr == 1 ) longValue = 0x0000A128;     // CPU signature (hex)
			else if ( dstAddr == 2 ) longValue = (ulong) FLASH_PAGE_SIZE;
			else if ( dstAddr == 3 )
			{
				pByte = (uint8 *) &longValue;   // Treat longword as a 4-byte array...
				pByte[3] = PEEK_CODE_BYTE( APPL_VERSION_NUM_ADDR + 0 );   // Major ver #
				pByte[2] = PEEK_CODE_BYTE( APPL_VERSION_NUM_ADDR + 1 );   // Minor ver #
				pByte[1] = PEEK_CODE_BYTE( APPL_VERSION_NUM_ADDR + 2 );   // Build # LSB
				pByte[0] = PEEK_CODE_BYTE( APPL_VERSION_NUM_ADDR + 3 );   // Build # MSB
			}
			else if ( dstAddr == 9 )
			{
				longValue = wLoaderError;   // Loader error flags (LS 16 bits)
				wLoaderError = 0;           // Clear the error flags after copying
			}
			else longValue = 0L;
		}
		// Convert binary value to numeric string (hex ASCII) format
		hexLong2str( longValue, zBuf );
		appendToResponse( zBuf, 8 );
		appendToResponse( &cTermChar, 1 );
		break;

	case 'R':     // Read from memory location -- a longword is returned.

		if ( ParseXRecord( 'R', buffer, &dstAddr ) )
		{
				pByte = (uint8 *) &longValue;   // Treat longword as a 4-byte array...
				pByte[3] = PEEK_CODE_BYTE( dstAddr + 0 );
				pByte[2] = PEEK_CODE_BYTE( dstAddr + 1 );
				pByte[1] = PEEK_CODE_BYTE( dstAddr + 2 );
				pByte[0] = PEEK_CODE_BYTE( dstAddr + 3 );
		}
		// Convert binary value to numeric string (hex ASCII) format
		hexLong2str( longValue, zBuf );
		appendToResponse( zBuf, 8 );
		appendToResponse( &cTermChar, 1 );
		break;

	case 'S':     // Process S-record;  write data to page buffer

		byteCount = ParseSRecord( buffer, bCodeBuf, sizeof(bCodeBuf), &dstAddr );
		pbCacheStartAddr = (uint8 __hugeflash *) ( dstAddr & ~FLASH_PAGE_MASK );
		for ( idx = 0 ; idx < byteCount ; idx++ )
		{
			if ( dstAddr <= (uint32) APPL_CODE_END_ADDR )   // address is within bounds
			{
				bPageBuffer[dstAddr & FLASH_PAGE_MASK] = bCodeBuf[idx];
				dstAddr++ ;
			}
			else  SET_BIT( wLoaderError, LDR_ERR_ADDR_RANGE );
		}
		break;

	default:      // Unknown command type

		SET_BIT( wLoaderError, LDR_ERR_COMMAND_UNK );
		break;
	}
}


/*
*   Parse a loader command string, aka "X-record" (anything other than S-record).
*   Format of X-record is:    (each char is printable ASCII)
*      "Xlnnaa....cc\n"
*   where  X = record type; l = addr length (1,2,3); nn = bytecount;  cc = checksum
*   The checksum byte (cc) is the one's complement of the unsigned byte addition of
*   byte values from nn (incl.) up to the last byte before cc. (Same as S-record.)
*
*	param:  recordType = single char identifying the command (record type)
*	param:  pInBuf    = pointer to command string
*	param:  pDstAddr   = pointer to (32 bit) target address (where applicable)
*
*	return:  TRUE (1) if X-record checksum is validated, else FALSE.
*/
bool  ParseXRecord( char recordType, char *pInBuf, uint32 *pDstAddr )
{
	bool    retVal;
	uchar   numBytesInRecord;    // the number of bytes that the record says it contains
	char    addressLen;          // The number of bytes in the address based on X1, X2 or X3
	uchar   calcChecksum;
	uchar   readChecksum;
	uchar   tempIdx;
	uchar   tempByte;

	pInBuf++ ;
	calcChecksum = 0;
	retVal = 0;

	if ( *pInBuf >= '1' && *pInBuf <= '3' )   // look for {1, 2, 3}
	{
		addressLen = 1 + ( *pInBuf - '0' );   // '1' = 2 bytes, '2' = 3 bytes, '3' = 4 bytes
		pInBuf++;
		// read out the number of bytes
		numBytesInRecord = hexatoi( pInBuf, 2 );  pInBuf += 2;
		calcChecksum = numBytesInRecord;
		/*
		* read out the address.
		* Decrement the number of bytes still left in the record, as this figure includes
		* the address bytes
		*/
		*pDstAddr = 0L;
		for ( tempIdx = 0 ; tempIdx < addressLen ; tempIdx++, numBytesInRecord-- )
		{
			tempByte = hexatoi( pInBuf, 2 );  pInBuf += 2;
			*pDstAddr <<= 8;
			*pDstAddr += (uint32) tempByte;
			calcChecksum += tempByte;
		}
		calcChecksum = ~calcChecksum;
		// extract checksum byte from X-record
		readChecksum = hexatoi( pInBuf, 2 );  pInBuf += 2;
		// compare checksums
		if ( readChecksum == calcChecksum ) retVal = 1;
		else  SET_BIT( wLoaderError, LDR_ERR_XREC_CKSM );
	}
	return  retVal;
}


/*
*   Parse a loader S-record.
*   Record types other than S1, S2, or S3 are ignored.
*
*	param:  pInBuff  = Pointer to buffer containing the S-record to be processed
*	param:  pOutBuff = Pointer to buffer to store the extracted code bytes
*	param:  maxDataBytes = maximum allowable number of data bytes in S-record
*	param:  pDstAddr = Pointer to destination address (in flash)
*
*	return:  length of the parsed data (bytes)
*/
uchar  ParseSRecord( char *pInBuf, uchar *pOutBuff, uchar maxDataBytes, uint32 *pDstAddr )
{
	uchar retVal;
	uchar numBytesInRecord;   // number of bytes that the record says it contains
	uchar dataBytesInRecord;  // number of data bytes that are going to be buffered
	char  addressLen;         // number of bytes in the address, based on S1, S2 or S3
	uchar calcChecksum;
	uchar readChecksum;
	uchar tempIdx;
	uchar tempByte;

	pInBuf++;
	calcChecksum = 0;
	retVal = 0;

	if ( *pInBuf >= '1' && *pInBuf <= '3' )
	{
		addressLen = 1 + ( *pInBuf - '0' ); // '1' = 2 bytes, '2' = 3 bytes, '3' = 4 bytes
		pInBuf++;
		// read out the number of bytes
		numBytesInRecord = hexatoi( pInBuf, 2 );  pInBuf += 2;
		calcChecksum = numBytesInRecord;
		/*
		* Read out the address. Decrement the number of bytes still left in the record,
		* as this figure includes the address bytes.
		*/
		*pDstAddr = 0L;
		for ( tempIdx = 0 ; tempIdx < addressLen ; tempIdx++, numBytesInRecord-- )
		{
			tempByte = hexatoi( pInBuf, 2 );  pInBuf += 2;
			*pDstAddr <<= 8;
			*pDstAddr += ( uint32 )tempByte;
			calcChecksum += tempByte;
		}
		// Accumulate the bytes, leaving off the last one for the checksum
		dataBytesInRecord = LESSER_OF(( numBytesInRecord - 1 ), maxDataBytes);
		for ( tempIdx = 0; tempIdx < dataBytesInRecord; tempIdx++ )
		{
			tempByte = hexatoi( pInBuf, 2 );  pInBuf += 2;
			*pOutBuff++ = tempByte;
			calcChecksum += tempByte;
		}
		calcChecksum = ~calcChecksum;
		// extract checksum byte from S-record
		readChecksum = hexatoi( pInBuf, 2 );
		// compare checksums
		if ( readChecksum != calcChecksum )
			SET_BIT( wLoaderError, LDR_ERR_SREC_CKSM );
		retVal = dataBytesInRecord;
	}
	gbLastCount = retVal;
	return retVal;
}


/*****************************************************************************************
*                           CHARACTER CONVERSION FUNCTIONS
*
*  Convert Hexadecimal ASCII char (arg) to 4-bit value (returned as unsigned byte).
*
*  Entry args: c = Hex ASCII character
*  Returns:    0xFF if arg is not hex, else digit value as unsigned byte ( 0..0x0F )
*  Affects:    --
*/
uint8  hexctobin( char c )
{
	if ( c >= '0'  &&  c <= '9')
		return ( c - '0' );
	else if ( c >= 'A'  &&  c <= 'F' )
		return ( c - 'A' + 10 );
	else if ( c >= 'a'  &&  c <= 'f' )
		return ( c - 'a' + 10 );
	else
		return 0xFF ;
}


/*****
*  Convert Hexadecimal ASCII string, up to 4 digits, to a 16-bit unsigned word.
*  There cannot be any leading white space.
*  Conversion is terminated when the specified number of characters have been
*  processed, or if a non-Hex char is found.
*  If the target string (1st char) is non-Hex, the function returns 0.
*
*  Entry args: s = pointer to first char of hex string.
*  Returns:    Unsigned 16bit word ( 0 to 0xffff ).
*/
uint16  hexatoi( char * s, uint8 ubCount )
{
	uint8   ubDigit;
	uint16  uwResult = 0;

	while ( ubCount-- > 0 )
	{
		if ( (ubDigit = hexctobin( *s++ )) == 0xFF )
			break;
		uwResult = 16 * uwResult + ubDigit;
	}
	return  uwResult;
}


/*
*   Return TRUE if c is a valid Hex ASCII character ('0'..'9' | 'A'..'F' | 'a'..'f' ).
*/
bool  isHexDigit( char c )
{
	if ( hexctobin( c ) == 0xFF ) return FALSE;
	else  return TRUE;
}


/*
*   Convert a 16-bit unsigned word to hexadecimal string (4 hex ASCII digits).
*   The result string is NOT NUL-terminated.
*/
void  hexWord2str( uint16 wVal, char *pcResult )
{
	pcResult[0] = hexitoc( wVal >> 12 );   // MSB first
	pcResult[1] = hexitoc( wVal >> 8 );
	pcResult[2] = hexitoc( wVal >> 4 );
	pcResult[3] = hexitoc( wVal );
}


/*
*   Convert a 32-bit unsigned longword to hexadecimal string (8 hex ASCII digits).
*   The result string is NOT NUL-terminated.
*/
void  hexLong2str( uint32 ulVal, char *pcResult )
{
	hexWord2str( (uint16) (ulVal >> 16), pcResult );   // High word
	hexWord2str( (uint16) ulVal, &pcResult[4] );       // Low word
}


/*
*   Convert integer value (4 LS bits) to hexadecimal ASCII character ('0' to 'F').
*   The input value is masked to use only the 4 LS bits.
*/
char  hexitoc( short wDigit )
{
	char  cRetVal;

	wDigit = wDigit & 0xF;
	if ( wDigit < 10 ) cRetVal = '0' + wDigit;
	else  cRetVal = 'A' + (wDigit - 10);

	return  cRetVal;
}

