/* Linux receive test for UIRmega */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <asm/ioctls.h>

#define PULSE_MASK 0x00FFFFFF
#define PULSE_BIT  0x01000000
 
static int fd;
static char fname[256];

static unsigned char HexTab[256] = {
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
   };


int init(void) {

	fd = open ("/dev/ttyACM0", O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd==-1) {
	   perror("unable to open /dev/ttyACM0");
	   return (-3);
	} else {
	   fcntl(fd,F_SETFL,FNDELAY);  // non-blocking read
//	   fcntl(fd, F_SETFL, 0);      // blocking read

	   printf("device /dev/ttyACM0 opened\n");
	}

	struct termios options;

	/* get the current options */
	tcgetattr(fd, &options);

	/* set raw input */
	options.c_cflag     |= (CLOCAL | CREAD);
	options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag     &= ~OPOST;

	/* set the options */
	tcsetattr(fd, TCSANOW, &options);

	int n = write(fd,"\r\n",2);
	if (n<0) {
	   perror("send test failed");
           return (-4);
	} else {
	   printf("send test succeeded\n");
	}

	return (1);
}

void Receive(void) {
	unsigned long int rcvdata, width, dec;
	int cntr, ret, bytes, to, i, j, n, x; 
	char flag, buf[16];

	printf("\nRECEIVE\n\n");

	cntr = 0; // reset item counter 
	rcvdata = 0; // clear data buffer

	/* endless wait for first character */

	while (1) { 
	   n = read(fd,buf,1); // get char
	   if (n>0) break; // char in buf[0]
	   usleep(5000);  // no data, wait for 5 msec
	}

	to = 0; // reset timeout counter

	while (to<20)  {
	   if(n>0) { // char in buf[0] 
	      x = HexTab[ (int)buf[0] ];
	      if (x<16) {  // hex char
	         rcvdata = (rcvdata<<4) + x; // hex char
	      } else {  // flag
	         rcvdata &= 0xFFFFFF; // mask out unused bits
	         printf("%03d: ", cntr);
	         printf("%06lx%c\n", rcvdata, buf[0]);
	         rcvdata = 0; // clear data
	         cntr += 1; 
	      }
	      to = 0;      // reset timeout counter

	   } else { // no character received

	      to += 1;       // increment timeout counter ...
	      usleep(5000);  // no data, wait for 5 msec
	   }
	   n = read(fd,buf,1); // get next char

	} // loop ends if no data for 100ms

	printf("\n%d items received\n", cntr-1);
}


int main (int argc, char *argv[]) {

	if (init()<0) return -1;

	while (1) {
		Receive();
		printf("\n");
	}

	printf("Closing /dev/ttyACM0\n");
	close(fd);
	return 0;
}

