/*
 * lanaCOM - 4 bit logic analyzer using COM-port interface
 * Copyright (c) 2008, Dipl.-Ing. Hans-Juergen Heinrichs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO  EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *
 *           __                      ______ ____   __  ___
 *          / /____ _ ____   ____ _ / ____// __ \ /  |/  /
 *         / // __ `// __ \ / __ `// /    / / / // /|_/ /
 *        / // /_/ // / / // /_/ // /___ / /_/ // /  / /
 *       /_/ \__,_//_/ /_/ \__,_/ \____/ \____//_/  /_/
 *
 *  l a n a C O M  --  4 bit (l)ogic (ana)lyzer using (COM)-port interface
 *
 *
 *
 *  Description: see collateral documentation
 *
 *  TODO:
 *      - Post-Trigger
 *        Status: done
 *      - Add a signal to the vcd-file which shows whether the sampling
 *        is precise or not. Due to operating system higher priority
 *        tasks, some kind of inaccuracy cannot be avoided.
 *        Status: done
 *      - Support both: ioperm and giveio.
 *        Status: done
 *      - Combine two COM ports to an 8bit analyzer (would reduce sample rate)
 *        Status: open, is it reasonable?
 *
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <sched.h>
#include <getopt.h>

#include "allowio.h"
#include "com.h"
#include "tcnt2.h"

/* some definitions */
#define VERSION         "lanaCOM.exe, Revision 0.1, " __DATE__
#define COPYRIGHT       "Copyright (c) 2008, H.-J. Heinrichs"

#define PRIO_DEFAULT        -1
#define PRIO_NORMAL         0

#define TRGMODE_NONE        0
#define TRGMODE_LEVEL       1
#define TRGMODE_EDGE        2
#define TRGMODE_TIME        4


/*****************************************************************************
 *  Function    :   h e l p ()
 *
 *  Parameters  :   none
 *  Return value:   void
 *  Description :   Display the help message.
 *
 *****************************************************************************/
static char *usage_str =
    "\n"
    "l a n a C O M -- 4 bit (l)ogic (ana)lyzer using (COM) port interface\n"
    "\n"
    "Usage: lanaCOM [Options]\n"
    " -c, --comport=port  Any serial port COM[1..4] with legacy I/O address\n"
    " -a, --address=ioad  Use (non legacy) ioaddr as base I/O address\n"
    "\n"
    "General trigger options\n"
    " -b, --buffsize=b    'b' can be either the number of sample points or a time\n"
    "                     specification with unit e.g.: 243us, 51ms, 2s\n"
    " -p, --post=p        Acquire 'p' samples after trigger was found. 'p' can be\n"
    "                     either the number of sample points or a time specific.\n"
    "                     with unit e.g.: 243us, 51ms, 2s\n"
    " -m, --mode=m        Trigger mode: one of 'level', 'edge', 'time'\n"
    " -k, --mask=k        Logically 'and' sampled data with 'k'\n"
    "                     (k[3]/k[2]/k[1]/k[0]) = (DCD/RI/DSR/CTS)\n"
    "\n"
    "Level trigger options:\n"
    " -t, --trigger=t     Trigger if: (sample & k) == t\n"
    "                     (t[3]/t[2]/t[1]/t[0]) = (DCD/RI/DSR/CTS)\n"
    " -i, --invert        Trigger if: (sample & k) != t\n"
    "\n"
    "Edge trigger options:\n"
    " trigger if: ((previous_sample & k) == e) && ((curr_sample & k) == E)\n"
    " -e, --edgedata1=e   Compare value for previous sample\n"
    "                     (e[3]/e[2]/e[1]/e[0]) = (DCD/RI/DSR/CTS)\n"
    " -E, --edgedata2=E   Compare value for current sample\n"
    "                     (E[3]/E[2]/E[1]/E[0]) = (DCD/RI/DSR/CTS)\n"
    "\n"
    "VCD file options:\n"
    " -f, --ofile=file    VCD output file name (default=stdout)\n"
    " -3, --DCD=alias     Alias name for DCD\n"
    " -2, --RI=alias      Alias name for RI\n"
    " -1, --DSR=alias     Alias name for DSR\n"
    " -0, --CTS=alias     Alias name for CTS\n"
    "\n"
    "Environmental options:\n"
    " -P, --priority=p    Set process priority (-20=max,+20=min,-1=default)\n"
    " -v, --verbose=v     Verbosity level (default=2 is warning+info)\n"
    "                     3 = Warnings + Infos + Debug\n"
    "                     2 = Warnings + Infos (default)\n"
    "                     3 = Warnings\n"
    "                     0 = (silent)\n"
    " -V, --version       Print program version\n"
    " -?, --help          This help\n"
    "\n"
    "lanaCOM uses the RS232 flow-control and modem input signals as probes to\n"
    "sample up to four signals. See the table below for the signal mapping.\n"
    "\n"
    " Analyzer  RS232    DB-9   DB-25             D               G      \n"
    " bit       Signal   Pin#   Pin#              C               N      \n"
    " ------------------------------              D               D      \n"
    "   0       CTS       8       5           _________________________  \n"
    "   1       DSR       6       6           \\   1   2   3   4   5   / \n"
    "   2       RI        9      22            \\    6   7   8   9    /  \n"
    "   3       DCD       1       8             \\___________________/   \n"
    "                                               D       C   R        \n"
    "                                               S       T   I        \n"
    "                      C   D   G   D            R       S            \n"
    "                      T   S   N   C                                 \n"
    "                      S   R   D   D                                 \n"
    "  _________________________________________________________         \n"
    "  \\   1   2   3   4   5   6   7   8   9  10  11  12  13   /        \n"
    "   \\   14  15  16  17  17  19  20  21  22  23  24  25    /         \n"
    "    \\___________________________________________________/          \n"
    "                                       R                            \n"
    "                                       I                            \n"
    ;

static void
help(void)
{
    printf(usage_str);
}


/*****************************************************************************
 *  Functions   :   q u i t ()
 *                  q u i t p()
 *                  w a r n i n g ()
 *                  i n f o ()
 *                  d e b u g ()
 *
 *  Parameters  :   varargs
 *  Return value:   -
 *  Description :   Display errors/warnings/infos/debug information
 *
 *****************************************************************************/
#define quit( ... )    ({ fprintf( stderr, __VA_ARGS__ );               (void)fflush(stderr); exit( EXIT_FAILURE ); })
#define quitp( ... )   ({ fprintf( stderr, __VA_ARGS__ ); perror(NULL); (void)fflush(stderr); exit( EXIT_FAILURE ); })
#define warning( ... ) ({ if(options.verbose >= 1) {printf( "Warn : " __VA_ARGS__ ); (void)fflush(stdout);} })
#define info( ... )    ({ if(options.verbose >= 2) {printf( "Info : " __VA_ARGS__ ); (void)fflush(stdout);} })
#define debug( ... )   ({ if(options.verbose >= 3) {printf( "Debug: " __VA_ARGS__ ); (void)fflush(stdout);} })

#ifdef __CYGWIN__
#   define isCygwin()  1
#else
#   define isCygwin()  0
#endif


/*****************************************************************************
 *  Function    :   g e t _ o p t i o n s ()
 *
 *  Parameters  :   int argc, char *argv[]
 *  Return value:   void
 *  Description :   Scans all command line options and fills the options
 *                  structure
 *
 *****************************************************************************/
#define OPTIONS     "c:a:f:b:m:p:t:k:ie:E:0:1:2:3:P:vVh?"

const static char short_options[] = OPTIONS;
const static struct option long_options[] = {
    // char name[], int has_arg, int *flag, int val
    { "comport",    1, 0, 'c' },
    { "address",    1, 0, 'a' },
    { "ofile",      1, 0, 'f' },
    { "buffsize",   1, 0, 'b' },
    { "mode",       1, 0, 'm' },
    { "post",       1, 0, 'p' },
    { "trigger",    1, 0, 't' },
    { "mask",       1, 0, 'k' },
    { "invert",     0, 0, 'i' },
    { "edgedata1",  1, 0, 'e' },
    { "edgedata2",  1, 0, 'E' },
    { "CTS",        1, 0, '0' },
    { "DSR",        1, 0, '1' },
    { "RI",         1, 0, '2' },
    { "DCD",        1, 0, '3' },
    { "priority",   1, 0, 'P' },
    { "debug",      0, 0, 'D' },
    { "verbose",    1, 0, 'v' },
    { "version",    0, 0, 'V' },
    { "help",       0, 0, 'h' },
    { "?",          0, 0, '?' },
    { NULL,         0, 0,  0  }
};


static struct {
    int           comport;
    int           ioaddr;
    char          *ofilename;
    long          samplebuff_size;
    int           size_is_time;
    int           trigger_mode;
    long          post_triggercnt;
    int           cnt_is_time;
    unsigned char trigger_word;
    unsigned char trigger_mask;
    unsigned char trigger_edgedata1;
    unsigned char trigger_edgedata2;
    int           trigger_invert;
    char*         signal_name[4];
    int           priority;
    int           verbose;
} options = {
    -1,                     // comport
    -1,                     // ioaddr
    NULL,                   // ofilename
    -1L,                    // samplebuff_size  (about 10 seconds)
    0,                      // size_is_time
    0,                      // trigger_mode
    -1L,                    // post_triggercnt
    0,                      // cnt_is_time
    0xff,                   // trigger_word
    0xff,                   // trigger_mask
    0xff,                   // trigger_edgedata1
    0xff,                   // trigger_edgedata2
    0,                      // trigger_invert
    {NULL,NULL,NULL,NULL},  // signal_name[4]
    PRIO_DEFAULT,           // priority
    2                       // verbose
};

static long
useconds( char *p )
{
    if( !strcasecmp( p, "us" ) || !strcasecmp( p, "µs" ) )
        return 1L;
    if( !strcasecmp( p, "ms" ) )
        return 1000L;
    if( !strcasecmp( p, "sec" ) || !strcasecmp( p, "s" ) )
        return 1000L*1000L;
    else
        return 0L;
}

static void
get_options(int argc, char *argv[] )
{
    char *p;
    int opt_char;

    while ((opt_char = getopt_long_only(argc, argv, short_options, long_options, NULL )) != -1) {
        switch( opt_char ) {
            case 'c':
                if( !strncasecmp( optarg, "COM", 3 ) )
                    options.comport = atoi( optarg+3 );
                else
                    options.comport = atoi( optarg );
                if( options.comport > 4 || options.comport <= 0 )
                    quit( "ERROR: Invalid COM port number '%d'\n", options.comport );
                options.ioaddr = -1;
                break;
            case 'a':
                options.ioaddr = strtoul( optarg, NULL, 0 ) & 0xffff;
                options.comport = -1;
                break;
            case 'b':
                options.samplebuff_size = strtoul( optarg, &p, 0 );
                options.size_is_time = useconds( p );
                break;
            case 'p':
                options.post_triggercnt = strtol( optarg, &p, 0 );
                options.cnt_is_time = useconds( p );
                break;
            case 'f':
                options.ofilename = optarg;
                break;

            case 'm':
                if( !strncasecmp( optarg, "level", 5 ) ) options.trigger_mode = TRGMODE_LEVEL;
                if( !strncasecmp( optarg, "edge" , 4 ) ) options.trigger_mode = TRGMODE_EDGE;
                if( !strncasecmp( optarg, "time" , 4 ) ) options.trigger_mode = TRGMODE_TIME;
                break;

            case 't':
                options.trigger_word = strtoul( optarg, NULL, 0 );
                break;
            case 'k':
                options.trigger_mask = strtoul( optarg, NULL, 0 );
                break;
            case 'i':
                options.trigger_invert = 1;
                break;

            case 'e':
                options.trigger_edgedata1 = strtoul( optarg, NULL, 0 );
                break;
            case 'E':
                options.trigger_edgedata2 = strtoul( optarg, NULL, 0 );
                break;

            case '0' :
            case '1' :
            case '2' :
            case '3' :
                options.signal_name[opt_char-'0'] = optarg;
                break;
            case 'P':
                options.priority = strtol( optarg, NULL, 0 );
                if( options.priority < -20 || options.priority > 20 )
                    quit( "ERROR: Priority must be in the range of -20 to +20 (-20= max priority)\n" );
                break;
            case 'v':
                options.verbose = strtoul( optarg, NULL, 0 ) % 4;
                break;
            case 'V':
                printf( VERSION "\n" COPYRIGHT "\n" );
                exit( EXIT_SUCCESS );
                break;
            case 'h':
            case '?':
                help();
                exit( EXIT_SUCCESS );
                break;
            default:
                fprintf( stderr, "ERROR: No such option '%c'. Use option '--help' for help.\n", opt_char );
                exit( EXIT_FAILURE );
                break;
        }
    }

    /* Check required options.  */
    switch( options.trigger_mode )  {
        case TRGMODE_LEVEL:     /* Level triggering requires mask and trigger word. */
            if( options.trigger_mask == 0xff || options.trigger_word == 0xff )
                quit( "ERROR: Level mode triggering requires '--mask=' and '--trigger='.\n" );
            if( options.trigger_mask == 0xff )
                quit( "ERROR: Level mode triggering requires '--mask='.\n" );
            if( options.trigger_word == 0xff )
                quit( "ERROR: Level mode triggering requires '--trigger='.\n" );
            break;
        case TRGMODE_EDGE:      /* Edge triggering requires mask and edge data.     */
            if( options.trigger_mask == 0xff || options.trigger_edgedata1 == 0xff || options.trigger_edgedata2 == 0xff )
                quit( "ERROR: Edge mode triggering requires '--mask=', '--edgedata1=' and '--edgedata2='.\n" );
            if( options.trigger_mask == 0xff )
                quit( "ERROR: Edge mode triggering requires '--mask='\n" );
            if( options.trigger_edgedata1 == 0xff )
                quit( "ERROR: Edge mode triggering requires '--edgedata1='.\n" );
            if( options.trigger_edgedata2 == 0xff )
                quit( "ERROR: Edge mode triggering requires '--edgedata2='.\n" );
            break;
        case TRGMODE_TIME:
            break;
        case TRGMODE_NONE:
        default:
            quit( "ERROR: No trigger mode specified (option '--mode=').\n" );
            break;
    }
    if( options.samplebuff_size < 0 )
        quit( "ERROR: Option '--buffsize=' must be specified.\n" );
}

/*****************************************************************************
 *  Functions   :   s i g c a t c h ()
 *                  s e t p r i o ()
 *
 *****************************************************************************/
static unsigned int sig_catched = 0;
static void
sigcatch(int sig)
{
    signal( SIGINT, SIG_DFL );
    sig_catched = 1;
}

static int
setprio(int prio)
{
    return setpriority( PRIO_PROCESS, 0, prio );
}

#ifndef __CYGWIN__
int
sethighprio(int prio)
{
    struct sched_param p;

    p.sched_priority=1;
    //p.sched_priority=99;
    return sched_setscheduler(0, SCHED_FIFO, &p);
}
int setlowprio(void)
{
    struct sched_param p;

    p.sched_priority=0;
    return sched_setscheduler(0, SCHED_OTHER, &p);
}
#endif


/*****************************************************************************
 *  Trace buffer structure and access functions:
 *
 *     t r a c e _ c a l i b r a t e ()
 *     t r a c e _ i n i t ()
 *     t r a c e _ i n s e r t D a t a ()
 *     t r a c e _ s e t T r i g g e r ()
 *     t r a c e _ g e t D a t a ()
 *     t r a c e _ g e t T i m e ()
 *     t r a c e _ d e b u g ()
 *
 *****************************************************************************/
static struct {
    unsigned char   *d_buffer;      // pointer to the data buffer
    unsigned char   *t_buffer;      // pointer to the time buffer
    double          calibration;    // number of tcnt2 ticks/ IO read
    unsigned int    size;           // max allowed number of samples
    unsigned int    idx_wr;         // write index
    int             trigger;        // trigger position
    int             inaccurate;     // unusual access time
} trace_ctrl = { NULL, NULL, 0.0, 0, 0, 0, -1 };


#define CTS_BITNO       4
#define DSR_BITNO       5
#define RI_BITNO        6
#define DCD_BITNO       7
#define TRG_BITNO       0
#define LOWACC_BITNO    1

/*
 *   The data word stored in the trace buffer:
 *               ________________________________________________
 *   Signal     | DCD | RI  | DSR | CTS | x   | x   | LOW  | TRG |
 *              |     |     |     |     |     |     | ACC  |     |
 *              --------------------------------------------------
 *   Bit#       .  7  .  6  .  5  .  4  .  3  .  2  .  1   . 0   .
 */
static struct {
    char    bitno;
    char    *name;
    char    *id;
    char    show;
    int     width;
    int     previous_value;
} signal_descr[] = {
    { CTS_BITNO   , "CTS"    , "'" ,  1,  1, -1 },
    { DSR_BITNO   , "DSR"    , "(" ,  1,  1, -1 },
    { RI_BITNO    , "RI"     , ")" ,  1,  1, -1 },
    { DCD_BITNO   , "DCD"    , "*" ,  1,  1, -1 },
    { TRG_BITNO   , "TRG"    , "#" ,  0,  1, -1 },
    { LOWACC_BITNO, "LOWACC" , "$" ,  1,  1, -1 },
    { -1          , NULL     , NULL, -1, -1, -1 }
};


/*
 * Sets trace_ctrl.calibration to the number
 * of tcnt2 ticks/IOread.
 */
#define CALIBRATION_SAMPLE_MAX  1000
double
trace_calibrate(void)
{
    int i, n;
    unsigned char diff;
    unsigned int  mean = 0;
    char *buff;

    if( (buff = malloc(CALIBRATION_SAMPLE_MAX)) == NULL )
        quitp( "ERROR: Cannot malloc(%d) for calibration -", CALIBRATION_SAMPLE_MAX );

    tcnt2_start();
    for( i = 0; i < CALIBRATION_SAMPLE_MAX; i++ )
        for( n = 0; n < 10; n++ )           /* Count ten read cycles.   */
            buff[i] = tcnt2_read();

    n = i;
    for( i = 0; i < n-1; i++ )  {
        diff = buff[i+1] - buff[i];
        mean += diff;
    }
    free(buff);
    return (1.0*mean)/(10.0*(n-1));
}

/*
 * Initialize the trace buffer
 * and its control structure
 */
static void
trace_init( unsigned char *p_d, unsigned char *p_t, int size, double calibration )
{
    int i = 0;

    trace_ctrl.d_buffer   = p_d;
    trace_ctrl.t_buffer   = p_t;
    trace_ctrl.size       = size;
    trace_ctrl.idx_wr     = 0;
    trace_ctrl.trigger    = -1;
    trace_ctrl.inaccurate = 0;
    trace_ctrl.calibration = calibration;

    /*
     * Change the signal names,
     * if required by command line options.
     */
    for( i = 0; i < 4; i++ )    // Check for all four signals
        if( options.signal_name[i] != NULL )
            signal_descr[i].name = options.signal_name[i];

    debug( "Trace buffer initialisation ... " );
    memset( p_d, 0xa5, size );
    memset( p_t, 0x5a, size );
    debug( "done\n" );
}

/*
 * Insert a single sample point into the
 * trace buffer.
 */
static void
trace_insertData( int n, unsigned char data, unsigned char t )
{
    if( n < trace_ctrl.size )  {
        trace_ctrl.d_buffer[n] = data;
        trace_ctrl.t_buffer[n] = t;
        trace_ctrl.idx_wr++;
    }
}

/*
 * Sets the trigger position.
 */
static void
trace_setTrigger(int trigger)
{
    trace_ctrl.trigger = trace_ctrl.idx_wr;
}

/*
 * Read out a trace buffer data.
 */
static unsigned char
trace_getData(int n)
{
    return trace_ctrl.d_buffer[n];
}

/*
 * Read out a trace buffer time.
 */
static double trace_currtime = 0.0;
static double
trace_getTime(int n)
{
    int delta = 0;

    if( n == 0 )  {  // first call
        trace_currtime = 0.0;
    } else  {
        delta = ( trace_ctrl.t_buffer[n] - trace_ctrl.t_buffer[n-1] ) & 0xff;
        trace_currtime += (1.0e6*delta) / (1.0*TMR2_FREQUENCY);
    }

    if( 1.0*delta > trace_ctrl.calibration + 4.0 )  {
        trace_ctrl.inaccurate++;
        trace_ctrl.d_buffer[n] |= 0x02;
    }
    return trace_currtime;
}

static void
trace_debug(void)
{
    debug( "====================================\n" );
    debug( "=  Trace buffer control structure  =\n" );
    debug( "====================================\n" );
    debug( "Buffer size  : %10d\n" , trace_ctrl.size       );
    debug( "idx_wr       : %10d\n" , trace_ctrl.idx_wr     );
    debug( "Trigger pos. : %10d\n" , trace_ctrl.trigger    );
}


/*****************************************************************************
 *  VCD - functions
 *
 *     v c d _ p u t H e a d e r ()
 *     v c d _ p u t S a m p l e ()
 *     v c d _ p u t T r a i l e r ()
 *
 *****************************************************************************
 */

/*
 * $timescale 1ps $end
 * $scope module logic $end
 * $var wire 8 # data $end
 * $var wire 1 $ data_valid $end
 * $var wire 1 % en $end
 * $var wire 1 & rx_en $end
 * $var wire 1 ' tx_en $end
 * $var wire 1 ( empty $end
 * $var wire 1 ) underrun $end
 * $upscope $end
 * $enddefinitions $end
 * #0
 * b10000001 #
 * 0$
 * 1%
 * 0&
 * 1'
 * 0(
 * 0)
 * #2211
 * 0'
 * #2296
 * b0 #
 * 1$
 * #2302
 * 0$
 * #2303
 */
static char *
toBinStr( char *buff, unsigned int d, unsigned char nbits, unsigned int mask )
{
    char *p = buff;
    while( nbits )  {
        *p++ = (d & mask) ? '1' : '0';
        mask /= 2;
        nbits--;
    };
    *p = '\0';
    return buff;
}

static void
vcd_putHeader( FILE *fp )
{
    int i;

    time_t t = time(NULL);
    fprintf( fp, "$date\n  %s$end\n", ctime(&t) );
    fprintf( fp, "$version\n  %s - %s\n$end\n", VERSION, COPYRIGHT );
    fprintf( fp, "$timescale 1us $end\n" );
    fprintf( fp, "$scope module lanaCOM $end\n" );
    for( i = 0; signal_descr[i].id != NULL; i++ )  {
        if( signal_descr[i].show )
            fprintf( fp, "$var wire %d %s %s $end\n", signal_descr[i].width, signal_descr[i].id, signal_descr[i].name );
    }

    fprintf( fp, "$var wire 32 %c TRGCNT $end\n", '%' );

    fprintf( fp, "$upscope $end\n" );
    fprintf( fp, "$enddefinitions $end\n" );
}



static void
vcd_putTrgCnt( FILE *fp, int t, int bit )
{
    char buff[64];
    static int trg_count = -1;

    if( bit || trg_count == -1 )  {
        trg_count++;
        fprintf( fp, "#%d\n", t );
        toBinStr( buff, trg_count, 32, 0x80000000 );
        fprintf( fp, "b%s %c\n", buff, '%' );
    }
}

static void
vcd_putSample( FILE *fp, int t, unsigned int da )
{
    int i, bit;
    int putTime_done = 0;

    for( i = 0; signal_descr[i].id != NULL; i++ )  {
        bit = (da >> signal_descr[i].bitno) & 0x01;
        if( bit != signal_descr[i].previous_value ) {
            if( signal_descr[i].show )  {
                if( !putTime_done ) {
                    fprintf( fp, "#%d\n", t );
                    putTime_done = 1;
                }
                fprintf( fp, "%c%s\n", bit ? '1' : '0', signal_descr[i].id );
            }

            /* Special handling for virtual signal: trigger_count.              */
            /* Trigger_count counts the changes of the trigger signal itself.   */
            if( signal_descr[i].bitno == TRG_BITNO )
                vcd_putTrgCnt( fp, t, bit );

            signal_descr[i].previous_value = bit;
        }
    }
}

static void
vcd_putTrailer( FILE *fp, int t )
{
    fprintf( fp, "#%d\n", t );
}

/*****************************************************************************
 *  Miscellaneous - functions
 *
 *     s h o w _ w a r n i n g ()
 *     g e t _ u s e c T i m e ()
 *
 *****************************************************************************
 */
static void
show_warning(long max_sample_count)
{
    int seconds;

    seconds = (int)((2.0 * max_sample_count * trace_ctrl.calibration)/TMR2_FREQUENCY + 0.5);
    info( "Acquisition will take about %d seconds.\n", seconds );
    if( options.priority < PRIO_DEFAULT ) {    // prio = high
        warning( "High priority sampling may hang your OS for this duration.\n" );
    } else {
        info( "Typing CTL-C once  will interrupt the acquire process only.\n" );
        info( "Typing CTL-C twice will kill the lanaCOM completely (leaving no result).\n" );
    }
    usleep(1000);
}


static double
get_usecTime(void)
{
    struct timeval  tv;
    gettimeofday( &tv, NULL );
    return 1.0e6 * tv.tv_sec + 1.0 * tv.tv_usec;
}


/*****************************************************************************
 *  The main function
 *
 *     m a i n ()
 *
 *****************************************************************************
 */
int
main(int argc, char **argv)
{
    unsigned char *sample_buff;
    unsigned char *timer_buff;
    unsigned char data, last_data = 0xfe, sample_data=0xff, sample_time;
    FILE *ofile = NULL;

    double calibration;
    unsigned char trig_mode, trig_mask, trig_word, trig_edgedata1, trig_edgedata2;
    int trig_invert;
    int triggered = 0, trigger = 0;
    int n;
    long post_triggercnt, max_sample_count;

    double start_usec, stop_usec;
    int currTime = 0L;

    /*
     *  Scan command line arguments.
     */
    get_options(argc, argv);

    /*
     *  Open the output file.
     */
    if( options.ofilename != NULL ) {
        if( (ofile=fopen(options.ofilename, "wb") ) == NULL )
            quitp( "ERROR: Cannot open output file '%s' - ", options.ofilename );
    } else {
        ofile = stdout;
    }

    /*
     *  Open and init the COM port.
     */
    if( options.comport < 0 && options.ioaddr < 0 )
        quit( "ERROR: Either a COM-port or an IO-address must be specified.\nUse option '--help' for help.\n" );
    if( options.comport > 0 && COM_open( options.comport ) < 0 )
        quitp( "ERROR: Cannot open COM%d - ", options.comport );
    if( allowIO(options.verbose) < 0 ) {
        if(  isCygwin() ) info( "lanaCOM requires 'giveio' or 'ioperm' service being started.\n" );
        if( !isCygwin() ) info( "lanaCOM requires 'root' privilege level.\n" );
        quit( "ERROR: Cannot get rights to access I/O registers.\n" );
    }

    if( (n = COM_init( options.comport, options.ioaddr )) < 0 )
        quit( "ERROR: Cannot find a UART at address '0x%x'.\n", -n );

    /*
     *  Malloc and initialize trace buffer and timer buffer.
     */
    calibration = trace_calibrate();

    /* Determine the required buffer size.  */
    if( options.size_is_time > 0 )
        max_sample_count = (int)(1.0e-6 * options.samplebuff_size * options.size_is_time * TMR2_FREQUENCY / (2.0 *calibration) );
    else
        max_sample_count = options.samplebuff_size;

    sample_buff = malloc(max_sample_count);
    timer_buff  = malloc(max_sample_count);
    if( sample_buff == NULL || timer_buff == NULL )
        quitp( "ERROR: Cannot malloc(%ld) - ", max_sample_count );
    debug( "%ld bytes of memory for trace buffer allocated.\n", 2*max_sample_count );

    trace_init( sample_buff, timer_buff, max_sample_count, calibration );


    /*
     *  Catch CTL-C and check whether priority setting is ok.
     */
    signal( SIGINT, sigcatch );
    if( setprio( options.priority ) != 0 )
        fprintf( stderr, "WARNING: Could not increase process priority\n" );
    show_warning( max_sample_count );

    /*
     *
     *    T h e   s a m p l i n g   l o o p
     *
     */
    trig_mode      = options.trigger_mode;
    trig_mask      = ( options.trigger_mask <<4 ) & 0xf0;
    trig_word      = ( options.trigger_word <<4 ) & 0xf0;
    trig_invert    = options.trigger_invert;
    trig_edgedata1 = ( options.trigger_edgedata1 <<4 ) & 0xf0;
    trig_edgedata2 = ( options.trigger_edgedata2 <<4 ) & 0xf0;
    if( options.post_triggercnt >= 0 && options.cnt_is_time > 0 )
        post_triggercnt = (int)(1.0e-6 * options.post_triggercnt * options.cnt_is_time * TMR2_FREQUENCY / (2.0 *calibration) );
    else
        post_triggercnt = options.post_triggercnt;

    trigger = triggered = 0;
    n = 0;

    debug( "Aquiring ...\n" ); usleep(1000);
    start_usec = get_usecTime();
    tcnt2_start();
    while( n < max_sample_count ) {

        sample_data = COM_getdata();        /* Acquire sample data and timer.           */
        sample_time = tcnt2_read();

        sample_data &= 0xf0;                /* Mask appropriately.                      */
        data = sample_data & trig_mask;

        trigger =  (trig_mode==TRGMODE_LEVEL && ((!trig_invert && data==trig_word) || (trig_invert && data!=trig_word)))
                || (trig_mode==TRGMODE_EDGE  && last_data==trig_edgedata1 && data==trig_edgedata2);
        if( trigger )
            sample_data |= 0x01;            /* Set trigger flag in data word.           */
        last_data = data;                   /* Remember last data (for edge detection). */

        if( trigger && !triggered )  {
            trace_setTrigger(triggered);    /* Set trigger Flag.                        */
            triggered = 1;
        }

        if( triggered )  {                  /* Count post trigger cycles.               */
            if( post_triggercnt > 0 )
                post_triggercnt--;
        }

        trace_insertData(n, sample_data, sample_time);  /* Insert data to trace buffer. */

        if( sig_catched || post_triggercnt == 0 )
            break;

        n++;
    }

    stop_usec = get_usecTime();
    setprio( PRIO_NORMAL );
    debug( "... done\n" );
    COM_close();
    signal( SIGINT, SIG_DFL );

    /*
     *
     *    P o s t p r o c e s s i n g
     *
     */
    /* Show trace information.  */
    info( "=======================\n" );
    info( "=  Trace information  =\n" );
    info( "=======================\n" );
    info( "Samples/second         : %e\n"    , 1.0e6 / ((stop_usec - start_usec) / trace_ctrl.idx_wr) );
    info( "I/O read time          : %g µs\n" , 1.0e6 * trace_ctrl.calibration / TMR2_FREQUENCY );
    info( "Trigger found?         : %s\n"    , (trace_ctrl.trigger >= 0) ? "yes" : "no" );

    /*
     *  H E A D E R
     */
    vcd_putHeader(ofile);

    /*
     *  S A M P L E S
     */
    for( n = 0; n < trace_ctrl.idx_wr; n++ )  {
        currTime = trace_getTime( n );
        data = trace_getData( n );
        vcd_putSample( ofile, currTime, data );
    }

    /*
     *  T R A I L E R
     */
    vcd_putTrailer(ofile, currTime + 1.0*trace_ctrl.calibration );
    info( "Potentially inaccurate : %d\n", trace_ctrl.inaccurate );
    trace_debug();

    free( sample_buff );
    free( timer_buff );

    return 0;
}
