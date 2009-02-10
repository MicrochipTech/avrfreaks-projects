/*
 * avrlirc2udp.c
 *
 * transfer characters from avrlirc device on a serial port to an
 * lircd daemon running the udp "driver".  the output of the
 * arvlirc device matches the expected lircd input pretty much
 * exactly.  we try to read pairs of bytes using the VMIN parameter
 * of the tty driver, but other than that, it's mainly a data copy.
 *
 * any "out-of-band", i.e., non-IR data is prefixed by a pair of zero
 * bytes.  currently unused.
 *
 * one problem is that if we somehow start our reads "halfway" through
 * one of the 16 bit data words, we'll forever be out-of-sync.  we
 * try to correct this by noticing two things:  a) if the data
 * contains two zero bytes in succession, then they're definitely
 * a pair (since this is the out-of-band data prefix), and b) the
 * 16 bit values that we read should have alternating high bits: 
 * 0x8000, 0x0000, 0x8000, etc.
 *
 **********
 *
 * Copyright (C) 2007, Paul G. Fox
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>

#define LIRCD_UDP_PORT 8765

extern char *optarg;
extern int optind, opterr, optopt;

char *prog;
int daemonized;


/* debugging can either replace, or augment, normal operation */
#define DEBUG_ONLY 1
#define DEBUG_AND_CONNECT 2
int debug;

void
usage(void)
{
    fprintf(stderr,
	"usage: %s [options] -t <ttydev> -h <lircd_host> [-p <lircd_port>]\n"
	"   lircd_port defaults to 8765.\n"
	"   use '-d' for debugging (with socket connection).\n"
	"   use '-D' for debugging (without socket connection).\n"
	"   use '-f' to keep program in foreground.\n"
	"   use '-H' for high speed tty (115200 instead of 38400).\n"
	"   use '-w S' to poll the creation of ttydev at S second intervals.\n"
	, prog);
    exit(1);
}

void
report(char *s)
{
    if (daemonized)
	syslog(LOG_NOTICE, s);
    else
	fprintf(stderr, "%s\n", s);
}

void
die(char *s)
{
    if (daemonized) {
	syslog(LOG_ERR, "%s: %m", s);
	syslog(LOG_ERR, "exiting");
    } else {
	perror(s);
    }
    exit(1);
}

int socket_init(char *host, int port)
{
    struct sockaddr_in sa[1];
    struct hostent *hent;
    int u;

    hent = gethostbyname(host);
    if (!hent) {
	fprintf(stderr, "%s: gethostbyname: ", prog);
        herror(host);
	exit(1);
    }

    if ((u = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	die("socket");

    memset((char *) sa, sizeof(*sa), 0);
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    sa->sin_addr = *((struct in_addr *)hent->h_addr);
       
    if (connect(u, (struct sockaddr *)sa, sizeof(*sa)) < 0)
	die("connect");

    return u;
}

/* shared by tty_init()/tty_restore() */
struct termios prev_tios;
int tty_fd;

void
tty_restore(void)
{
    tcsetattr(tty_fd, TCSADRAIN, &prev_tios);
}

void
sighandler(int sig)
{
    tty_restore();
    die("signal");
}

int tty_init(char *term, int wait_term, int speed)
{
    int s;
    struct termios tios;
    int flags;

    int logged = 0;

    while(1) {
	tty_fd = open(term, O_RDWR);
	if (tty_fd >= 0 || errno != ENOENT || !wait_term)
	    break;

	if (!logged)
	    report("waiting for tty creation");

	logged = 1;

	sleep(wait_term);
    }

    if (logged)
	report("found tty");

    if (tty_fd < 0)
	die("can't open tty");

    if (!isatty(tty_fd))
	die("not a tty");

    s = tcgetattr(tty_fd, &prev_tios);
    if (s < 0)
	die("ttopen tcgetattr");

    /* set up restore hooks quickly */
    atexit(tty_restore);
    signal(SIGTERM, sighandler);
    signal(SIGHUP, sighandler);

    tios = prev_tios;

    tios.c_oflag = 0;	/* no output flags at all */
    tios.c_lflag = 0;	/* no line flags at all */

    tios.c_cflag &= ~PARENB;	/* disable parity, both in and out */
    tios.c_cflag |= CSTOPB|CLOCAL|CS8|CREAD;   /* two stop bits on transmit */
					    /* no modem control, 8bit chars, */
					    /* receiver enabled, */

    tios.c_iflag = IGNBRK | IGNPAR;    /* ignore break, ignore parity errors */

    tios.c_cc[VMIN] = 2;
    tios.c_cc[VTIME] = 0;
    tios.c_cc[VSUSP]  = _POSIX_VDISABLE;
    tios.c_cc[VSTART] = _POSIX_VDISABLE;
    tios.c_cc[VSTOP]  = _POSIX_VDISABLE;

    s = cfsetospeed (&tios, speed);
    if (s < 0)
	die("ttopen cfsetospeed");
    s = cfsetispeed (&tios, speed);
    if (s < 0)
	die("ttopen cfsetispeed");
    s = tcsetattr(tty_fd, TCSAFLUSH, &tios);
    if (s < 0)
	die("ttopen tcsetattr");

    /* make sure RTS and DTR lines are high, since device may be
     * phantom-powered.  no termios/posix way to do this, that i
     * know of.
     */
    if (ioctl(tty_fd, TIOCMGET, &flags) >= 0) {
	flags &= ~TIOCM_RTS;
	flags &= ~TIOCM_DTR;
	ioctl(tty_fd, TIOCMSET, &flags);
    }

    return tty_fd;
}

void
process_oob(int from, unsigned char *buf)
{
    // nothing here
}

void
data_loop(int from, int to)
{
    unsigned char b[2];
    int n;
    int prevhighbit = -1;
    int prevwaszero;
    int highbit;

    while (1) {

	/* leave extra space in buf for extra bytes we may read below */
	if ((n = read(from, b, 2)) < 0)
	    die("read");

	/* in my experience, this results from a USB serial
	 * device being unplugged */
	if (n == 0)
	    return;

	if (n == 1) { /* short read -- force it even */
	    if (read(from, &b[1], 1) <= 0)
		die("read");
	    n++;
	}

	if (debug) {
	    int pulse;
	    pulse = (b[1] << 8) + b[0];
	    pulse &= 0xffff;
	    fprintf(stderr, "pulse 0x%04x\n", pulse);
	}

#if OUT_OF_BAND_SOMEDAY
	if (oob) {  // these bytes are out-of-band
	    process_oob(from, b);
	    prevhighbit = -1;
	    prevwaszero = 0;
	    oob = 0;
	    continue;
	}

	//if (debug)
	//    debug_buffer("", buf, n);

	if (b[0] == 0 && b[1] == 0) {
	    // then we're definitely in phase
	    oob = 1;
	    prevwaszero = 0;
	    continue;
	} 

	oob = 0;

	if (prevwaszero && b[0] == 0) {
	    // then we're definitely out of phase
	    report("phase correction");
	    b[0] = b[1];
	    if (read(from, &b[1], 1) <= 0)
		die("read");
	    process_oob(from, b);
	    prevhighbit = -1;
	    prevwaszero = 0;
	    continue;
	}
#endif
	
	highbit = (b[1] & 0x80);
	if (highbit == prevhighbit) {
	    report("phase correction");
	    b[0] = b[1];
	    if (read(from, &b[1], 1) <= 0)
		die("read");
	    highbit = (b[1] & 0x80);
	}
	prevhighbit = highbit;
	prevwaszero = (b[1] == 0);

	if (debug != DEBUG_ONLY)  { // sending to host
	    if (write(to, b, 2) < 0) {
		if (errno != ECONNREFUSED)
		    die("write");
	    }
	}

    }
}

int
main(int argc, char *argv[])
{
    char *p;
    char *host = 0, *term = 0;
    int port = LIRCD_UDP_PORT;
    int foreground = 0;
    int wait_term = 0;
    int speed = B38400;
    int t, u = -1;
    int c;

    prog = argv[0];
    p = strrchr(argv[0], '/');
    if (p) prog = p + 1;

    while ((c = getopt(argc, argv, "HdDfw:t:h:p:")) != EOF) {
	switch (c) {
	case 'H':
	    speed = B115200;
	    break;
	case 'd':
	    debug = DEBUG_AND_CONNECT;
	    break;
	case 'D':
	    debug = DEBUG_ONLY;
	    break;
	case 'f':
	    foreground = 1;
	    break;
	case 't':
	    term = optarg;
	    break;
	case 'w':
	    wait_term = atoi(optarg);
	    if (wait_term == 0)
		usage();
	    break;
	case 'h':
	    host = optarg;
	    break;
	case 'p':   /*	or microseconds */
	    port = atoi(optarg);
	    break;
	default:
	    usage();
	    break;
	}
    }

    if ((debug != DEBUG_ONLY && !host) || !term || optind != argc) {
	usage();
    }

    while (1) {

	t = tty_init(term, wait_term, speed);

	if (u < 0 && debug != DEBUG_ONLY)
	    u = socket_init(host, port);

	if (!daemonized && !foreground && !debug) {
	    if (daemon(0, 0) < 0)
		die("daemon");
	    daemonized = 1;
	}

	data_loop(t, u);

	/* we'll only ever return from data_loop() if our read()
	 * returns 0, which usually means our (USB-based) tty has
	 * gone away.  loop if we were told to wait (-w) for it.
	 */
	if (!wait_term)
	    die("end-of-dataloop");

	tty_restore();
	close(t);
    }

    return 0;
}
