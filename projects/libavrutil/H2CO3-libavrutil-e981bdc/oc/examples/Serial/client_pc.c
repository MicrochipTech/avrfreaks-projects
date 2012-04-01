/*
 * Modified from the example code found at:
 * http://en.wikibooks.org/wiki/Serial_Programming/Serial_Linux
 *
 * Start using: ./client_pc /dev/ttyUSB0 (for eample; this particular
 * port name was detected on Ubuntu 11.10. On other systems, it may be
 * different.)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
 
int main(int argc, char **argv)
{
	struct termios tio;
	struct termios stdio;
	int tty_fd;
	fd_set rdset;

	unsigned char c = 'D';
 
	memset(&stdio, 0, sizeof(stdio));
	stdio.c_iflag = 0;
	stdio.c_oflag = 0;
	stdio.c_cflag = 0;
	stdio.c_lflag = 0;
	stdio.c_cc[VMIN] = 1;
	stdio.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &stdio);
	tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); /* Make them non-blocking */

	memset(&tio, 0, sizeof(tio));
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL; /* 8n1, see termios.h for more information */
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5;
 
	tty_fd = open(argv[1], O_RDWR | O_NONBLOCK);      
	cfsetospeed(&tio, B9600); /* 9600 baud */
	cfsetispeed(&tio, B9600); /* 9600 baud */
	tcsetattr(tty_fd, TCSANOW, &tio);
	
	while (c != 'q') /* Quit by typing 'q' */
	{
		if (read(tty_fd, &c, 1) > 0)
		{
			write(STDOUT_FILENO, &c, 1); /* If new data is available on the serial port, print it out */
		}
		
		if (read(STDIN_FILENO, &c, 1) > 0)
		{
			write(tty_fd, &c, 1); /* If new data is available on the console, send to serial port */
		}
	}
 
	close(tty_fd);
}

