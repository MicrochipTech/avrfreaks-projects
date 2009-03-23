/************************************************************************/
/*									*/
/*			Bootloader Programmer				*/
/*		Author: Peter Dannegger, danni@alice-dsl.net		*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <alloc.h>
#include <time.h>

#include "protocol.h"
#include "readargs.c"

#define	EXIT_SUCCESS	0
#define	EXIT_ERROR	1
#define	EXIT_ABORT	2

#define	REVISION	0	// get bootloader revision
#define	BUFFSIZE	1 	// get buffer size
#define	SIGNATURE	2 	// get target signature
#define USERFLASH	3	// get user flash size
#define	PROGRAM		4	// program flash
#define	START		5	// start application
#define	CHECK_CRC	6	// CRC o.k.
#define	VERIFY		7	// Verify


#define TIMEOUT	        5	// 0.3s
#define TIMEOUTP	72	// 4s
#define STARTWAIT	3

#define	MAXFLASH	0x40000UL	// max flash size (256kB)


int ComPort = 0;
unsigned long Baud = 115200L;		// 57600L;
char Passwd[130] = "Peda";
char Flash[130] = "";

unsigned int crc;
int buffersize;
int echocount = 0;
long flashsize;
unsigned char far *data = NULL;


int connect( void );
int empfang( int te );
void get_crc( unsigned char d );
int helptext( void );
void initsio( void );
void getpasswd( void );
int octal( char *p );
int program( char *fname, int verify );
int read_crc( void );
int readhex( FILE *fp, unsigned long *addr, unsigned char *data);
int read_info( void );
long readval( void );
void sendbuff( unsigned int len, unsigned char *buff );
void senden(unsigned char c);
void sendcommand(unsigned char c);
void sendpwd(unsigned char *text);
int check_crc( void );
void  my_exit( char *msg, int exitcode );


void main( void )
{
  char s[130];
  clock_t t;
  int crc_on;
  unsigned int i;

  if( helptext() )
    exit( EXIT_ABORT );

  getpasswd();
  if( connect() )				// connect ATMega
    my_exit( "\nAborted", EXIT_ABORT);

  crc_on = check_crc();				// 2 = no CRC support
  if( read_info() )
    my_exit( "Error, wrong device informations", EXIT_ERROR );

  if( crc_on != 2 && check_crc() )
    my_exit( "CRC-Error", EXIT_ERROR );

  data = farmalloc( flashsize );
  if( data == NULL )
    my_exit("Memory allocation error !", EXIT_ERROR );

  t = clock();					// actual time
  if( readargs( ASTRING, 'p', &Flash ) ){       // Programming
    if( Flash[0] ){
      if( program( Flash, 0 ))
	my_exit( "Program-Error", EXIT_ERROR );
    }
  }
  if( readargs( ASTRING, 'v', &Flash ) ){       // Verify
    if( Flash[0] ){
      if( program( Flash, 1 ))
	my_exit( "Verify-Error", EXIT_ERROR );
    }
  }
  if( crc_on != 2 ){
    if( check_crc() )
      my_exit( "CRC-Error", EXIT_ERROR );

    printf("CRC: o.k.\n");
  }

  t = clock() - t;
  printf("Elapsed time: %4.2f seconds\n", t / CLK_TCK );

//  getch();
  my_exit( "", EXIT_SUCCESS );
}


void  my_exit( char *msg, int exitcode )
{
  clock_t t;

  sendcommand( PROGEND );
  sendcommand( COMMAND );
  sendcommand( START );
  if( data )
    farfree( data );
  printf( "%s\n", msg );
  for( t = clock(); clock() - t < STARTWAIT; );
  exit( exitcode );
}


int check_crc( void )
{
  unsigned int crc1;

  sendcommand( CHECK_CRC );
  if( empfang( TIMEOUT ) == BADCOMMAND )
    return 2;
  crc1 = crc;
  senden( crc1 );
  senden( crc1 >> 8 );
  if( empfang( TIMEOUT ) == SUCCESS )
    return 0;
  return 1;
}


void far_memset( unsigned char far *dst, unsigned char val, unsigned long size )
{
  while( size > 0x8000 ){
    _fmemset( dst, val, 0x8000 );
    dst = MK_FP( FP_SEG(dst) + 0x800, FP_OFF(dst));
    size -= 0x8000;
  }
  _fmemset( dst, val, size );
}


void far_copy( unsigned char far *dst, unsigned long idx, unsigned char *src, unsigned int len )
{
  unsigned int seg;

  idx += FP_OFF( dst );
  seg = FP_SEG(dst) + (idx>>4);

  _fmemcpy( MK_FP( seg, idx & 0x0F ), src, len );
}


unsigned char far_byte( unsigned char far *src, unsigned long idx )
{
  unsigned int seg;

  idx += FP_OFF(src);
  seg = FP_SEG(src) + (idx>>4);

  return *(unsigned char far *)MK_FP( seg, idx & 0x0F );
}


int program( char *fname, int verify )
{
  FILE *fp;
  unsigned long lastaddr = 0;
  unsigned long addr = 0;
  int i;
  unsigned char s[255];
  unsigned char d1;

  far_memset( data, 0xFF, flashsize );
  if( NULL == ( fp = fopen( fname, "rb" ) ) ){
    printf("File %s open failed !\n", fname);
    return 1;
  }
  while( (i = readhex( fp, &addr, s )) >= 0 ){
    if( i ){
      if( addr + i > flashsize ){
	fclose( fp );
	my_exit( "Hex-file to large for target!", EXIT_ERROR );
      }
      far_copy( data, addr, s, i );
      addr += i;
      if( lastaddr < addr-1 )
	lastaddr = addr-1;
      addr++;
    }
  }
  fclose( fp );

  if( verify == 0 ){
    printf( "Program %s: 00000 - 00000", fname);
    sendcommand( PROGRAM );
  }else{
    sendcommand( VERIFY );
    if( empfang( TIMEOUT ) == BADCOMMAND ){
      printf("Verify not available\n");
      return 0;
    }
    printf( "Verify %s: 00000 - 00000", fname);
  }
  for( i = buffersize, addr = 0;; addr++ ){
    switch( d1 = far_byte( data, addr )){
      case ESCAPE:
      case 0x13: senden( ESCAPE );
		 d1 += ESC_SHIFT;
      default:   senden( d1 );
    }
    if( --i == 0 ){
      printf( "\b\b\b\b\b%05lX", addr + 1 );
      if( !verify && empfang( TIMEOUTP ) != CONTINUE ){
	printf( " failed!\n" );
	return 1;
      }
      i = buffersize;
    }
    if( addr == lastaddr ){
      senden( ESCAPE );
      senden( ESC_SHIFT );			// A5,80 = End
      printf( "\b\b\b\b\b%05lX", addr );
      if( empfang( TIMEOUTP ) == SUCCESS ){
	  printf( " successful" );
      }else{
	printf( " failed!\n" );
	return 1;
      }
      break;
    }
  }
  printf("\n");
  return 0;
}


int sscanhex( unsigned char *str, unsigned int *hexout, int n )
{
  unsigned int hex = 0, x = 0;
  for(; n; n--){
    x = *str;
    if( x >= 'a' )
      x += 10 - 'a';
    else if( x >= 'A' )
      x += 10 - 'A';
    else
      x -= '0';
    if( x >= 16 )
      break;
    hex = hex * 16 + x;
    str++;
  }
  *hexout = hex;
  return n;					// 0 if all digits read
}


int readhex( FILE *fp, unsigned long *addr, unsigned char *data){
  /* Return value: 1..255	number of bytes
			0	end or segment record
		       -1	file end
		       -2	error or no HEX-File */
  char hexline[524];				// intel hex: max 255 byte
  char * hp = hexline;
  unsigned int byte;
  int i;
  unsigned int num;
  unsigned int low_addr;

  if( fgets( hexline, 524, fp ) == NULL )
    return -1;					// end of file
  if( *hp++ != ':' )
    return -2;                                  // no hex record
  if( sscanhex( hp, &num, 2 ))
    return -2;					// no hex number
  hp += 2;
  if( sscanhex( hp, &low_addr, 4 ))
    return -2;
  *addr &= 0xF0000L;
  *addr += low_addr;
  hp += 4;
  if( sscanhex( hp, &byte, 2 ))
    return -2;
  if( byte == 2 ){
    hp += 2;
    if( sscanhex( hp, &low_addr, 4 ))
      return -2;
    *addr = low_addr * 16L;
    return 0;					// segment record
  }
  if( byte == 1 )
    return 0;					// end record
  if( byte != 0 )
    return -2;					// error, unknown record
  for( i = num; i--; ){
    hp += 2;
    if( sscanhex( hp, &byte, 2 ))
      return -2;
    *data++ = byte;
  }
  return num;
}


long readval( void )
{
  int i;
  int j = 257;
  long val = 0;

  for(;;){
    i = empfang( TIMEOUT );
    if( i == -1 )
      return -1;			// timeout

    switch( j ){

      case 2:
      case 3:
      case 4:
	val = val * 256 + i;
	j--;
	break;

      case 256:
	j = i;
	break;

      case 257:
	if( i == FAIL )
	  return -2;

	if( i == ANSWER )
	  j = 256;
	break;

      case 1:
	if( i == SUCCESS )
	  return val;

      default:
	return -2;
    }
  }
}


int read_info( void )
{
  long i, j;
  char s[200];
  FILE *fp;

  sendcommand( REVISION );
  i = readval();
  printf("Bootloader V%lX.%lX\n", i>>8, i&0xFF );

  sendcommand( SIGNATURE );
  if( (i = readval()) == -1 )
    return 1;
  strncpy( s, _argv[0], 200 );			// get path+name
  strcpy( strchr( s, 0 ) - 3, "DEF" );          // name.EXE -> name.DEF
  if( (fp = fopen( s, "r" )) != NULL ){
    while( fgets( s, 200, fp )){
      if( sscanf( s, "%lX : %s", &j, s ) == 2 ){ // valid entry
	if( i == j )
	  break;
      }
      *s = 0;
    }
    fclose( fp );
  }
  printf("Target: %06lX %s\n", i, s );

  sendcommand( BUFFSIZE	);
  i = readval();
  if( i == -1 )
    return 1;
  buffersize = i;
  printf("Buffer: %ld Byte\n", i );

  sendcommand( USERFLASH );
  i = readval();
  if( i == -1 || i > MAXFLASH )
    return 1;
  flashsize = i;
  printf("Size available: %ld Byte\n", i );
  return 0;
}


int octal( char *p ){				// read octals "\123"
  int n, i;

  if( *p++ != '\\' )
    return -1;           			// no octal number
  for( n = 0, i = 3; i; i-- ){
    if( *p == 0 )
      return -2;				// wrong octal number
    n = n * 8 + *p++ - '0';
  }
  return n;
}


void getpasswd( void )
{
  char text[81], * t = text, * p = Passwd;
  int i, j;

  if( readargs( ASTRING, 'i', t ) && *t ){
    while( *t ){                              	// copy string
      i = octal( t );                           // convert octals
      if( i >= 0 ){
	*p = i;
	t += 3;					// octal = 3 bytes
      }else{
	*p = *t;
      }
      p++;
      t++;
    }
    *t = 0;
  }
}


int connect( void ){
  int COM[] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
  char WAITSTRING[] = { '|', '/', '-', '\\' };
  int i = 1;
  int echo = 0;
  char *s;
  int j = 0;
  clock_t t = clock();

  readargs( ALONG, 'b', &Baud );
  if( Baud < 2 )
    Baud = 2;
  if( Baud > 115200 )
    Baud = 115200;
  readargs( AINT, 'c', &i );		// serial port
  if( --i > 3 )
    i = 0;
  printf("COM %d at %ld Baud:  ", i+1, Baud );
  if( readargs( ABOOL, 'd', &i ) == 0 )
    ComPort = COM[i];			// Convert port number to io-address
  initsio();

  for(;;){

    if( (clock() - t) > 6 ){
      t += 5;
      printf( "\b%c", WAITSTRING[++j&3] );
    }
    if( kbhit() ){
      getch();
      return 1;
    }
    s = Passwd;
    do{
      if( *s )
	senden( *s );
      else
	senden( 0xFF );
      i = empfang( 0 );
      if( i == Passwd[1] )
	echo = 1;				// echo received

      if( i == CONNECT ){
	s = "";
	if( echo ){
	  echocount = 1;
	  s = " (One wire)";
	}
	printf("\bConnected%s\n", s);
	sendcommand( COMMAND );
	for(;;){
	  switch( empfang( TIMEOUT )){ 		// clear RX buffer
	    case SUCCESS:
	    case -1:
	      return 0;
	  }
	}
      }
    }while( *s++ );
  }
}


int empfang( int te )
{
  unsigned char i;
  clock_t t = clock();				// actual time

  do{
    if( inportb(ComPort+5) & 1 ){		// byte received
      i = inportb(ComPort);
      if( echocount > 1 ){
	echocount--;                            // remove echo
	t = clock();				// restart timeout
	continue;
      }
      return i;
    }
  }while( (clock() - t ) < te ); 		// timeout

  return -1;
}


void get_crc( unsigned char d )
{
  int i;

  crc ^= d;
  for( i = 8; i; i-- ){
    crc = (crc >> 1) ^ ((crc & 1) ? 0xA001 : 0 );
  }
}


int helptext( void )
{
  int i;
  if( readargs( ABOOL, '?', &i ) ){
    printf( "/?\t\t Get this help message\n"
	  "/Bnnnn\t\t Define baud rate\n"
	  "/Cn\t\t Define serial port n = 1..4\n"
	  "/Pname\t\t Perform Program\n"
	  "/Vname\t\t Perform Verify\n"
	  "/Istring\t Init string\n"
	  "Press any Key ! " );
    getch();
    return 1;
  }
  return 0;
}


void initsio( void )
{
  while( (inportb(ComPort+5) & 0x60) != 0x60 );	// Senden beenden
  while( (inportb(ComPort+5) & 1) == 1 )
    inportb(ComPort);				// Empfangspuffer leeren
  outportb(ComPort+3, 0x87); 			// Einlesen Baudrate
  outportb(ComPort, (115200 / Baud) & 0xFF );	// Baudrate low
  outportb(ComPort+1, 450 / Baud );		// Baudrate high
  outportb(ComPort+3, 0x03);			// N, 1, 8
  outportb(ComPort+1, 0x00);			// Interruptsperre
  outportb(ComPort+4, 0); 			// DTR = RTS = 0
  outportb(ComPort+2, 0x07); 			// Enable FIFO
}


void senden(unsigned char c)
{
  if( echocount ){
    if( echocount > 1 )
      empfang( 0 );				// remove echo
    echocount++;
  }
  while( (inportb(ComPort+5) & 0x20) == 0);	/* Test Sendepuffer leer */
  outportb(ComPort, c );
  get_crc( c );					// calculate transmit CRC
}


void sendcommand(unsigned char c)
{
  if( echocount )
    echocount = 1;				// restart echo counter
  senden( COMMAND );
  senden( c );
}
