/**
 * Bootloader um dem Mikrocontroller Bootloader von Peter Dannegger anzusteuern
 * Teile des Codes sind vom original Booloader von Peter Dannegger (danni@alice-dsl.net)
 *
 * @author Andreas Butti
 * Bug fixes by Ilya G. Goldberg
 *   - flashsize allocation based on free user flash
 *   - serial comm speed set in both directions
 *   - lastaddr initialized correctly
 *   - in linux (and ANSI C) clock() is CPU time, not elapsed time.  Changed to gettimeofday().
 *   - no busy-polling of read
 *   - checking write() to avoid hardware buffer overruns
 *   - enabled hardware buffers for a ~100x speedup.
 *   2009-08-24:
 *   - added optional reboot character sequence sent before each bootloader connection attempt
 *   - added optional limit to number of connection attempts
 *   - added optional silent run
 *   - error messages sent to stderr with exit 0 on success, -1 failure
 */


/// Includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h> // for readlink()

#include "com.h"
#include "protocol.h"

/// Prototypes
void usage();
void connect_device();
void sendcommand(unsigned char c);
int check_crc();
int read_info();
long readval();
int flash(char verify, const char * filename);
int readhex(FILE *fp, unsigned long *addr, unsigned char *data);
int sscanhex(char *str, unsigned int *hexout, int n);
char * readHexfile(const char * filename, int flashsize, unsigned long * lastaddr);
void cescape (char *string);

/// Definitions
// These are in milliseconds (it is not likely that the timer resolution is better than 10 ms)
#define TIMEOUT	  300 // 0.3s
#define TIMEOUTP 4000 // 4s


///show elapsed time @ ms / s
///only ONE definition!
//#define SHOW_TIME_MS
#define SHOW_TIME_S


/// Attributes
int buffersize;
long flashsize;

/// Run options
int silent=0,ntries=0,reboot_arg=0;
char reboot_chars[256]="";

static struct timeval t_start; //time
static int foo;
// Text file with device signatures
// Find its path based on binary path.
char devices_file_p[256]="", *chp;
#define DEVICES_FILE "devices.txt"

/**
 * Main, startup
 */
int main(int argc, char *argv[]) {

	// Filename of the HEX File
	const char * hexfile = NULL;

	// 1 if verify, 2 if programm
	char verify = 0;

	// Serial device
	const char * device = "/dev/ttyS0";

	// Baudrate
	int baud = 4800;
	int baudid = -1;

	// if crc is supported (not supportet if 2)
	int crc_on;
	
	// The path to the binary (for finding devices.txt)
	readlink("/proc/self/exe", devices_file_p, 255); // symlink to this binary
	chp = strrchr (devices_file_p,'/');  // get the last path separator
	if (chp) strcpy (chp+1,DEVICES_FILE); // copy the device filename after the path separator

	// Parsing / checking parameter
	int i;
	int type = 0;

	for(i = 1; i < argc; i++) {
		if(*argv[i] == '-') {
			type = argv[i][1];
			if (type == 's') silent = 1;
		}
		else {
			switch(type) {
				case 'd':
					device = argv[i];
					break;
				case 'b':
					baud = atoi(argv[i]);
					break;
				case 'v':
					verify = 1;
					hexfile = argv[i];
					break;
				case 'p':
					verify = 2;
					hexfile = argv[i];
					break;
				case 'r':
					reboot_arg = i;
					strncpy (reboot_chars,argv[i],255);
					cescape (reboot_chars);
					break;
				case 'a':
					ntries = atoi(argv[i]);
					break;
				default:
					fprintf(stderr,"Unrecognized parameter '%c'!\n",(char)type);
					usage();
			}
			type = 0;
		}
	}

	// print header
	if (!silent) {
		printf("\n");
		printf("=================================================\n");
		printf("|             BOOTLOADER, Target: V2.1          |\n");
		printf("=================================================\n");
	}

	if(hexfile == NULL) {
		fprintf(stderr,"No hexfile specified!\n");
		usage();
	}

	if(verify == 0) {
		fprintf(stderr,"No Verify / Programm specified!\n");
		usage();
	}

	// Checking baudrate
	for(i = 0; i < BAUD_CNT; i++) {
		if (baud_value[i] == baud) {
			baudid = i;
			break;
		}
	}

	if(baudid == -1) {
		fprintf(stderr,"Unknown baudrate (%i)!\n", baud);
		usage();
	}

	if(!com_open(device, baud_const[baudid])) {
		fprintf(stderr,"Open com port failed!\n");
		exit(-1);
	}
	
	if (!silent) {
		printf("COM dev  : %s\n", device);
		printf("Baudrate : %i\n", baud);
		if (*reboot_chars) printf ("Reboot   : %s\n",argv[reboot_arg]);
		if (ntries) printf  ("Attempts : %d\n",ntries);
		printf("%s: %s\n", (verify == 1 ? "Verify   " : "Program  "), hexfile);
		printf("-------------------------------------------------\n");
	}

	connect_device();
	crc_on = check_crc();
	read_info();
	
	/*if(read_info()) {
	}
	else {
		printf("Reading device information failed!\n");
	}*/

	if(crc_on != 2) {
		crc_on = check_crc();
		switch(crc_on) {
			case 2:
				if (!silent) printf("No CRC support.\n");
				break;
			case 0:
				if (!silent) printf("CRC enabled and OK.\n");
				break;
			case 3:
				if (!silent) printf("CRC check failed!\n");
				break;
			default:
				if (!silent) printf("Checking CRC Error (%i)!\n", crc_on);
				break;
		}
	}
	else {
		if (!silent) printf("No CRC support.\n");
	}

	flash(verify==1, hexfile);
	if( crc_on != 2 ) {
		if( check_crc() ) {
			fprintf(stderr, "CRC-Error !\n");
			exit (-1);
		} else {
			if (!silent) printf("CRC: o.k.\n");
		}
	}

#ifdef SHOW_TIME_MS	 
	//time @ ms
	if (!silent) printf("Elapsed time: %d s\n", elapsed_msecs (&t_start));
#endif

#ifdef SHOW_TIME_S	
	if (!silent) printf("Elapsed time: %.3f seconds\n", elapsed_secs (&t_start));
#endif

	if (!silent) printf("...starting application\n\n");
	sendcommand(START);//start application
	sendcommand(START);

	com_close();//close opened com port
	return 0;
}

/**
 * Read a hexfile
 */
char * readHexfile(const char * filename, int flashsize, unsigned long * lastaddr) {
	char * data = malloc(flashsize);
	FILE *fp;
	int len;
	unsigned char line[512];
	unsigned long addr = 0;

	if(data == NULL) {
		fprintf(stderr,"Memory allocation error!\n");
		exit(-1);
	}

	memset( data, 0xFF, flashsize );

	if(NULL == (fp = fopen(filename, "r"))){
		fprintf(stderr,"File \"%s\" open failed!\n", filename);
		free(data);
		exit(-1);
	}

	if (!silent) printf("Reading %s... ", filename);


	// reading file to "data"
	while((len = readhex(fp, &addr, line)) >= 0) {
		if(len) {
			if( addr + len > flashsize ) {
				fclose(fp);
				free(data);
				fprintf(stderr,"Hex-file too large for target!\n");
				exit(-1);
			}
			memcpy(data+addr, line, len);
			addr += len;

			if(*lastaddr < addr-1) {
				*lastaddr = addr-1;
			}
 // This doesn't appear to do anything, and probably incorrect due to
 // addr += len; above, introducing an off-by-one error
			addr++;
		}
	}

	fclose(fp);

	if (!silent) printf("File read.\n");
	return data;
}


/**
 * Flashes or verify the controller
 */
int flash(char verify, const char * filename) {
	char * data = NULL;
	int i;
	int msg;
	unsigned char d1;
	unsigned long addr;
	unsigned long lastaddr=0;

	//read data from hex-file
	data = readHexfile(filename, flashsize, &lastaddr);

	if(data == NULL) {
		fprintf(stderr,"Reading file failed.\n");
		exit (-1);
	}
	if (!silent) printf("Highest address in hex file: 0x%05lX (%lu Bytes)\n",lastaddr,lastaddr);

	// Sending commands to MC
	if(verify == 0){
		if (!silent) {
			printf("Writing program memory...\n");
			printf("Programming \"%s\": 00000 - 00000", filename);
		}
		sendcommand(PROGRAM);
	} 
	else {
		sendcommand(VERIFY);
		if (!silent) printf("Verifying program memory...\n");
		
		foo = com_getc(TIMEOUT);
		if(foo == BADCOMMAND) {
			fprintf(stderr,"Verify not available\n");
			exit (-1);
		}
		if (!silent) printf( "Verify %s: 00000 - 00000", filename);
	}

	// Sending data to MC
	for(i = buffersize, addr = 0;; addr++) {
		switch(d1 = data[addr]){
			case ESCAPE:
			case 0x13:
				com_putc(ESCAPE);
				d1 += ESC_SHIFT;
			default:
				com_putc(d1);
		}

		if(--i == 0) {
			if (!silent) {
				printf( "\b\b\b\b\b%05lX", addr + 1);
				fflush(stdout);
			}
			if(!verify) {
				msg = 0;
				msg = com_getc(TIMEOUTP);
				if (msg != CONTINUE) {
					fprintf(stderr," failed. Did not receive CONTINUE despite more data to write. Got 0x%04X (%u)\n",(unsigned)((unsigned char)msg),(unsigned)((unsigned char)msg));
					free(data);
					exit (-1);
				}
			}
			i = buffersize;
		}
		
		if(addr == lastaddr) {
			com_putc(ESCAPE);
			com_putc(ESC_SHIFT); // A5,80 = End

			if (!silent) printf("\b\b\b\b\b%05lX", addr);
			msg = 0;
			msg = com_getc(TIMEOUTP);
			if (msg != SUCCESS) {
				fprintf(stderr," failed. Did not receive SUCCESS at end of programming. Got 0x%04X (%u)\n",(unsigned)((unsigned char)msg),(unsigned)((unsigned char)msg));
				free(data);
				exit (-1);
			} else {
				if (!silent) printf(" SUCCESS\n");
			}
			break;
		}
	}
	if (!silent) printf("\n");

	free(data);

	return 1;
}//int flash(char verify, const char * filename)



/**
 * Reads the hex file
 *
 * @return 1 to 255 number of bytes, -1 file end, -2 error or no HEX File
 */
int readhex(FILE *fp, unsigned long *addr, unsigned char *data) {
	char hexline[524]; // intel hex: max 255 byte
	char * hp = hexline;
	unsigned int byte;
	int i;
	unsigned int num;
	unsigned int low_addr;

	if(fgets( hexline, 524, fp ) == NULL) {
		return -1; // end of file
	}

	if(*hp++ != ':') {
		return -2; // no hex record
	}

	if(sscanhex(hp, &num, 2)) {
		return -2; // no hex number
	}

	hp += 2;

	if(sscanhex(hp, &low_addr, 4)) {
		return -2;
	}

	*addr &= 0xF0000L;
	*addr += low_addr;
	hp += 4;

	if(sscanhex( hp, &byte, 2)) {
		return -2;
	}

	if(byte == 2) {
		hp += 2;
		if(sscanhex(hp, &low_addr, 4)) {
			return -2;
		}
		*addr = low_addr * 16L;
		return 0; // segment record
	}

	if(byte == 1) {
		return 0; // end record
	}

	if(byte != 0) {
		return -2; // error, unknown record
	}

	for(i = num; i--;) {
		hp += 2;
		if(sscanhex(hp, &byte, 2)) {
			return -2;
		}
		*data++ = byte;
	}
	return num;
}

/**
 * reads hex data from string
 */
int sscanhex(char *str, unsigned int *hexout, int n) {
	unsigned int hex = 0, x = 0;

	for(; n; n--) {
		x = *str;
		if(x >= 'a') {
			x += 10 - 'a';
		} else if(x >= 'A') {
			x += 10 - 'A';
		} else {
			x -= '0';
		}

		if(x >= 16) {
			break;
		}

		hex = hex * 16 + x;
		str++;
	}
	
	*hexout = hex;
	return n; // 0 if all digits read
}


/**
 * prints usage
 */
void usage() {
	fprintf(stderr,"./booloader [-d /dev/ttyS0] [-b 9600] [-r '\\n\\nRESET\\n'] [-s] [-a 100] -[v|p] file.hex\n");
	fprintf(stderr,"-d Device\n");
	fprintf(stderr,"-b Baudrate\n");
	fprintf(stderr,"-v Verify\n");
	fprintf(stderr,"-p Program\n");
	fprintf(stderr,"-r Character sequence to reboot AVR (with std. C escape codes)\n");
	fprintf(stderr,"-s Silent run - exits 0 on success, -1 on failure, errors to stderr\n");
	fprintf(stderr,"-a Number of AVR bootloader connection attempts (def.: 0 = infinite)\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Author: Andreas Butti (andreasbutti at bluewin dot ch)\n");
	fprintf(stderr,"Modified by: Ilya Goldberg (igg at cathilya dot org\n");

	exit(-1);
}

/**
 * Try to connect a device
 */
void connect_device() {
	const char * ANIM_CHARS = "-\\|/";
	const char PASSWORD[6] = {'P', 'e', 'd', 'a', 0xff, 0};
	
	int state = 0;
	int try = ntries ? ntries : 1;
	int in = 0;
	
	if (!silent) printf("Waiting for device... Press CTRL+C to exit.  ");
	
	while(try) {
		if (!silent) {
			printf("\b%c", ANIM_CHARS[state]);
			fflush(stdout);
		}

		if (*reboot_chars) {
			com_puts(reboot_chars);
			usleep(10000);//wait 10ms
		}
		com_puts(PASSWORD);
		in = com_getc(0);

		if(in == CONNECT) {
			sendcommand(COMMAND);

			// Empty buffer
			while(1) {
				switch(com_getc(TIMEOUT)){
					case SUCCESS:
						if (!silent) printf("\n...Connected!\n");
						gettimeofday(&t_start, NULL);
						return;
					case -1:
						fprintf(stderr,"\n...Connection timeout!\n");
						exit (-1);
				}
			}
		}
		state++;
		state = state % 4;
		if (ntries) try--;
		usleep(10000);//wait 10ms
	}

	fprintf(stderr,"\n...Could not connect in %d attempts\n",ntries);
	exit (-1);
	
}//void connect_device()

/**
 * Sending a command
 */
void sendcommand(unsigned char c) {
  com_putc(COMMAND);
  com_putc(c);
}

/**
 * Checking CRC Support
 *
 * @return 2 if no crc support, 0 if crc supported, 1 fail, exit on timeout
 */
int check_crc() {
	int i;
	unsigned int crc1;

	sendcommand(CHECK_CRC);
	crc1 = crc;
	com_putc(crc1);
	com_putc(crc1 >> 8);

	i = com_getc(TIMEOUT);
	switch (i) {
		case SUCCESS:
			return 0;
		case BADCOMMAND:
			return 2;
		case FAIL:
			return 1;
		case -1:
			fprintf(stderr,"...Device does not answer!\n");
			exit (-1);
		default:
			return i;
	}
}

/**
 * prints the device signature
 *
 * @return true on success; exit on error
 */
int read_info() {
	long i, j;
	char s[256];
	FILE *fp;

///get bootloader REVISON
	sendcommand(REVISION);
	
	i = readval();
	if(i == -2) {
		if (!silent) printf("Bootloader Version unknonwn (Fail)\n");
	} else {
		if (!silent) printf("Bootloader    : V%lX.%lX\n", i>>8, i&0xFF);
	}

///get SIGNATURE
	sendcommand(SIGNATURE);
	
	i = readval();
	if (i == -2) {
		fprintf(stderr,"Reading device SIGNATURE failed!\n");
		exit (-1);
	}

	if((fp = fopen(devices_file_p, "r")) != NULL) {
		while(fgets(s, 256, fp)) {
			if(sscanf(s, "%lX : %s", &j, s) == 2){ // valid entry
				if(i == j) {
					break;
				}
			}
			*s = 0;
		}
		fclose(fp);
	} 
	else {
		sscanf ("(?)" , "%s", s);
		if (!silent) printf("File \"%s\" not found!\n",DEVICES_FILE);
	}

	if (!*s) sprintf (s,"Device not in \"%s\"",DEVICES_FILE);

	if (!silent) printf("Target        : %06lX %s\n", i, s);

///get BUFFSIZE
	sendcommand(BUFFSIZE);

	i = readval();
	if (i == -2) {
		fprintf(stderr,"Reading BUFFSIZE failed!\n");
		exit (-1);
	}
	buffersize = i;
	if (!silent) printf("Buffer        : %6ld Bytes\n", i );

///get USERFLASH
	sendcommand(USERFLASH);

	i = readval();
	if (i == -2) {
		fprintf(stderr,"Reading FLASHSIZE failed!\n");
		exit (-1);
	}
	if( i > MAXFLASH) {
		fprintf(stderr,"Device and flashsize do not match!\n");
		exit (-1);
	}
	
	flashsize = i;
	if (!silent) printf("Size available: %6ld Bytes\n\n", i );
	
	return 1;
}//int read_info()

/**
 * Reads a value from bootloader
 *
 * @return value; -2 on error; exit on timeout
 */
long readval() {
	int i;
	int j = 257;
	long val = 0;

	while(1) {
		i = com_getc(TIMEOUT);
		if(i == -1) {
			fprintf(stderr,"...Device does not answer!\n");
			exit(-1);
		}

		switch(j) {
			case 1:
				if(i == SUCCESS) {
					return val;
				}
				break;

			case 2:
				val = val * 256 + i;
				j= 1;
				break;

			case 3:
				val = val * 256 + i;
				j= 2;
				break;

			case 4:
				val = val * 256 + i;
				j= 3;
				break;

			case 256:
				j = i;
				break;

			case 257:
				if(i == FAIL) {
					return -2;
				}

				else if(i == ANSWER) {
					j = 256;
				}
				break;

			default:
				fprintf(stderr,"\nError: readval, i = %i, j = %i, val = %li\n", i, j, val);
				return -2;
		}
	}
}


/* Translate C backslash escape sequences */
/* WAS: librock_sscharbe */
/* RENAMED: cescape */
/*Copyright 1998-2000, Forrest J. Cavalier III d-b-a Mib Software
  Licensed under BSD-ish license, NO WARRANTY. Copies must retain this block.
  License, originals, details: http://www.mibsoftware.com/librock/
*/
/*<LICENSE>	 Copyright 1998-2002 Forrest J. Cavalier III, http://www.mibsoftware.com
  Open-source under MIT license. NO WARRANTY. Copies must retain this block.
  License text in <librock/license/mit.txt> librock_LIDESC_HC=cc598307414a9997b32b60a2e7a8e7c6a13d6438
*/

/*
Translate C backslash escape sequences
   \f \t \n \ooo \r \b \xXX \\ \0
Other characters and those which follow a backslash character
are stored at non-NULL pRet without translation.

Returns number of characters processed or 0 if *pStart == '\0'
*/
void cescape (char *string) {

unsigned char ch;
char *sptr=string;

	while (*sptr) {
		if (*sptr != '\\') {
			*string++ = *sptr++;
			continue;
		}
	
		/* Got a backslash */
		sptr++;
		if (*sptr == 't') {
			ch = 0x09;
			sptr++;
		} else if (*sptr == 'n') {
			ch = 0x0a;
			sptr++;
		} else if (*sptr == 'f') {	/* 10-3-95 */
			ch = '\f';
			sptr++;
		} else if (*sptr == 'r') {
			ch = 0x0d;
			sptr++;
		} else if (*sptr == 'b') {
			ch = 0x08;
			sptr++;
		} else if (*sptr == 'x') {
			/* Specify in hex */
			sptr++;
			if (!*sptr) continue;
			if ((*sptr >= '0')&&(*sptr <= '9')) {
				ch = *sptr - '0';
			} else {
				ch = (*sptr & 0x1f) + 9;
			}
			ch <<= 4;
			sptr++;
			if (!*sptr) continue;
			if ((*sptr >= '0')&&(*sptr <= '9')) {
				ch += *sptr - '0';
			} else {
				ch += (*sptr & 0x1f) + 9;
			}
			sptr++;
		} else if ((*sptr >= '0')&&(*sptr <= '9')) { /* An octal sequence */
			ch = *sptr - '0';
			sptr++;
			if (!*sptr) continue;
			if ((*sptr >= '0')&&(*sptr <= '9')) {
				ch *= 8;
				ch += *sptr - '0';
				sptr++;
			}
			if ((*sptr >= '0')&&(*sptr <= '9')) {
				ch *= 8;
				ch += *sptr - '0';
				sptr++;
			}
		} else { /* Unknown.  Send as literal */
			ch = *sptr++;
		}
		*string++ = ch;
	}
	*string++ = '\0';

} /* cescape */



/* end of file */
