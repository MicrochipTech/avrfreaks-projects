/*
;
;	Interfacing Compact Flash card to AT90S2313 and playing wave file from it
;
;	Michal Strug 2005
;
;	sample sender file to at90S2313 and cf card
;
;	first turn on avr then run this application !!!
;
;	file version: 1.0
;	use with asm file version: 1.0
;
*/

#include <windows.h>
#include <stdio.h>


HANDLE hCom;

void recv( char c )
{
	unsigned long byt=0;
	DWORD mask;
	char ctmp1;

	while(1)
	{
		ReadFile( hCom, &ctmp1, 1, &byt, NULL );
		if( byt != 0 ) break;
	}
	if( c != ctmp1 ) { printf("%c - %c - Error\n", c, ctmp1); }
}


int main( int argc, char *argv[] )
{
	DCB dcb;
	BOOL fSuccess;
	char *pcCommPort = "COM1";
	unsigned char c[1], buf[16];
	unsigned long byt;
	OVERLAPPED o;
	DWORD dwEvtMask, olds;
	SYSTEMTIME stime;

	FILE *f, *fd;

	if( argc <= 1 ) 
	{
		printf("Usage:\n cfwriter.exe input_file_name\n");
		return 0;
	}

	if( argc > 1 ) if( (f = fopen( argv[1], "rb" )) == NULL ) return 1;


//
// this line sens the process priority to high, so other application may working slower
//  remove this line if needed
	if( SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS ) == 0 ) return 1;


   hCom = CreateFile( pcCommPort,
                    GENERIC_READ | GENERIC_WRITE,
                    0,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    0,    // not overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );

   if (hCom == INVALID_HANDLE_VALUE) 
   {
       printf ("CreateFile failed with error %d.\n", GetLastError());
       return (1);
   }

   fSuccess = GetCommState(hCom, &dcb);

   if (!fSuccess) 
   {
      printf ("GetCommState failed with error %d.\n", GetLastError());
      return (2);
   }

   // Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.

   dcb.BaudRate = CBR_57600;     // set the baud rate
   dcb.ByteSize = 8;             // data size, xmit, and rcv
   dcb.Parity = NOPARITY;        // no parity bit
   dcb.StopBits = ONESTOPBIT;    // one stop bit

   fSuccess = SetCommState(hCom, &dcb);

   if (!fSuccess) 
   {
      printf ("SetCommState failed with error %d.\n", GetLastError());
      return (3);
   }


   printf ("Serial port %s successfully reconfigured.\n", pcCommPort);



	TransmitCommChar( hCom, 'd' );
	recv('?');
	TransmitCommChar( hCom, '1' );
	printf("? OK\n");

	int d=0, dd=0;
	GetSystemTime( &stime );
	olds = stime.wSecond;


	long licznik, licz2=0, licz3=0, liczS=0, liczT=0, liczTS=0;
	while( !feof(f) )							// reading whole file
	{
		c[0] = 0;
		fread( c, 1, 1, f);
		TransmitCommChar( hCom, c[0] );
//		Sleep(1);

		buf[licz2] = c[0];
		licz2++;
		licz3++;
		liczT++;

		if( licz3 == 512 ) 
		{ 
			liczTS++;

			if( olds != (unsigned)stime.wSecond) 
			{
				olds = stime.wSecond;
				d = liczT;
				dd = liczTS;
				liczT = 0;
				liczTS = 0;
			}

			printf("\n S %05i\t %i b/s\t %i S/s\n", liczS++, d, dd); licz3=0; licz2=0; 
			GetSystemTime( &stime );

			Sleep(1);
			TransmitCommChar( hCom, 'g' );		// send any char
	printf("A");
			recv( 'a' );				// wait for acknowleage from avr
	printf("B");
			TransmitCommChar( hCom, 'h' );		// again send any char
	printf("C");
		}
		else
		if( licz2 == 16 ) 
		{ 
// to see what is int each sector in console:
/*			for( int i=0; i<16; i++) 
			{
				if( (buf[i] == 7) ||(buf[i] == 9) || (buf[i] == 10) || (buf[i] == 13))  printf(" " );
				else printf("%c", buf[i] );
			}
			printf("\n"); */
			licz2=0; 
		}
	}

   if( argc > 1 ) fclose(f);

   return (0);
}


