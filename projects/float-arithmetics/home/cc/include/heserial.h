// heserial
// serial interface
#ifndef __heserial_h__
#define __heserial_h__

#ifndef __heinitf_h__
#include <heinitf.h>
#endif


#if defined( __TURBOCDOS__ )
// Will support only COM1 and COM2
#define MAX_INTERFACENO 2
#elif defined ( OSK )
#define MAX_INTERFACENO 8
#else
#define MAX_INTERFACENO 5
#endif



#if defined( MDOS40 ) || defined( MSVC40 ) || defined( __TURBOC__ ) || defined( DOSGNU )
#define MAX_INTERFACENAME 20
#endif



// i : Interface - Number
struct sserial {
	uint32_t baud ;				 // as integer number
	uint8_t stop ;				// 0 or 1
	char parity ;				// 'n' 'e' or 'o'
	uint8_t data ;				// 5, 6, 7, 8
	char handshake ;			// 'n' 'x' 'h' None / XON-XOFF / Hardware
			} ;

int serinitf(Cinitf &initfile);		// read
void serwriteinitfile( FILE *fpw ); // write


int serargs(int argc, const char **argv);	// interprets common serial parameters -i -b ...
// result of serargs - call :
	extern struct sserial ser_settings_field[MAX_INTERFACENO] ;
	// the sequence of -i parameters : interface parameters of invocation
	extern uint8_t ser_if_no[MAX_INTERFACENO] ;
void serargs_usage( void );


int seropen( uint8_t if_no) ;			 // open serial interface
int serclose( uint8_t if_no) ;
uint serichaw( uint8_t if_no) ;		 // rv >= 256 when any error detected

void serocha( uint8_t if_no, char a) ;	// Send one character

void serobuf( uint8_t ser_port_no, const char *p1, uint cnt );	// Send cnt characters
void serputs( uint8_t ser_port_no, const char *ptr );			// Send a text string

bool serisicha(uint8_t if_no);		// TRUE if there is at least one character
extern char *sername[] ;		// interface names (for help- lists
extern int sermaxnumber ;		// number of interfaces to select, size ol list
int serset(uint8_t if_no, struct sserial &definition );
int seribuf( uint8_t ser_port_no, char *pbuf, uint maxbuf, uint *pactlen );
								// read a buffer as much bytes as are at input
// Switching output lines
FUNCT int setdtr(uint8_t ser_port_no, bool status);
FUNCT int setrts(uint8_t ser_port_no, bool status);

// Sensing input lines
bool getdsr(uint8_t ser_port_no);
bool getcts(uint8_t ser_port_no);
bool getdcd(uint8_t ser_port_no);
bool getallsent(uint8_t ser_port_no); // true if all sent

// receiving binary blocks
int serrecv(
uint8_t if_no ,			//
char *pdest,
uint16_t lendest,
uint16_t *paktlen,
uint32_t timeout );

// sending binary blocks
int sersend(
uint8_t if_no ,			// interface number
char *pdest,			// data to send
uint16_t lendest );			// number of bytes to send


// receiving binary blocks
int serrecvunsave(
uint8_t if_no ,			//
char *pdest,
uint16_t lendest,
uint16_t *paktlen,
uint8_t *psequence,		// save received sequence number here
uint32_t timeout );

// sending binary blocks
int sersendunsave(
uint8_t if_no ,			// interface number
char *pdest,			// data to send
uint16_t lendest,			// number of bytes to send
uint8_t sequence );			// Telegramme number

// Flush all input
void serflush( uint8_t if_no, uint32_t timeout );

void csum( char *ptr, uint len, uint8_t *pcbuf );
#define NCBUFFER 4		// Checksum buffer width


// int serial_send_sync(uint8_t if_no);	// send a fixed sync header
// int serial_wait_sync(uint8_t if_no, uint32_t timeout) ; // wait until fixed sync header detected

int send_prog( uint8_t if_no, char *pdest, uint16_t actlen );	// send a buffer of data
int recv_prog( uint8_t if_no, char *pdest, uint16_t maxlen, uint32_t timeout ); // receive and save a buffer of data

int sersendack( uint8_t if_no, uint8_t seq );
int sersendnak( uint8_t if_no, uint8_t seq );
int sersendsync( uint8_t if_no, uint8_t seq );
int serwaitack( uint8_t ser_if_no, uint8_t *pseq, uint32_t timeout );


// Statistics to see Errors
// used by qq/qqserr.cpp qq/qqsers.cpp
struct sErrSerial
	{
	uint32_t ulBadSync0;	// Bad first Byte in Synchronisation
	uint32_t ulBadSync1;	// Sync Byte again during Synchronisation
	uint32_t ulBadSync2;	// Bad Sync telegramme
	uint32_t ulBadSync3;	// Timout Sync
	uint32_t ulBadChecksum ;
	} ;
extern struct sErrSerial vErrSerial ;


// serial transmission codes
#define SERC_TIMEOUT 1
#define SERC_BAD_HEADER_CRC 2
#define SERC_BAD_DATA_CRC 3
#define SERC_OVERFLOW 4

#define SERC_ABORT 55
#define SERC_RESYNC 50	// Answer to send is not ack, but other data
#define SERC_NAK 51		// Please retransmit
#define SERC_ACK 0		// Do not retransmit - I understood the requested sequence
#define SERC_CANCEL 53	// on serial error user typed Cancel
#define SERC_BADACKNAKANSWER 54 //
#endif
