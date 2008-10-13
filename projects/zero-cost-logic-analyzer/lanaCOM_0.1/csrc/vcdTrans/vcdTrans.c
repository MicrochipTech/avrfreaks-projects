/*
 * vcdTrans - translate VCD-files to human readable formats
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
 *                      __ ______
 *    _   __ _____ ____/ //_  __/_____ ____ _ ____   _____
 *   | | / // ___// __  /  / /  / ___// __ `// __ \ / ___/
 *   | |/ // /__ / /_/ /  / /  / /   / /_/ // / / /(__  )
 *   |___/ \___/ \__,_/  /_/  /_/    \__,_//_/ /_//____/
 *
 *
 *    vctTrans takes VCD-files as input and translates to human readable
 *    outputs. The current version supports three protocols:
 *
 *    1. I2C
 *    2. SPI
 *    3. V24
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// some definitions */
#define VERSION         "vcdTrans.exe, Revision 0.1, " __DATE__
#define COPYRIGHT       "Copyright (c) 2008, H.-J. Heinrichs"
#define MAX_EVENTS      (10*1024*1024)
#define MAX_LINEWIDTH   256
#define MAX_SIGNAME     128
#define MAX_SIGNALS     4

/*****************************************************************************
 *  Function    :   h e l p ()
 *
 *  Parameters  :   none
 *  Return value:   void
 *  Description :   Display the help message.
 *
 *****************************************************************************/
static char *usage_str =
    "vcdTrans -- analyze and translate vcd-file as I2C, SPI or V24\n"
    "\n"
    "Usage: vcdTrans [Options]\n"
    " -i, --ifile=vcdfile VCD input filename (default=stdin)\n"
    " -o, --ofile=file    Output file name (default=stdout)\n"
    " -t, --type=type     Type is one of: 'i2c' or 'spi'\n"
    "\n"
    "I2C options\n"
    " -c, --scl=signame   Name of SCL signal in VCD-file\n"
    " -d, --sda=signame   Name of SDA signal in VCD-file\n"
    "\n"
    "SPI options\n"
    " -s, --ssn=signame   Name of SSN (select) signal in VCD-file\n"
    " -c, --sck=signame   Name of SCK (clock) signal in VCD-file\n"
    " -m, --mosi=signame  Name of MOSI (master out, slave in) signal in VCD-file\n"
    " -M, --miso=signame  Name of MISO (master in, slave out) signal in VCD-file\n"
    " -e, --edge=edge     Edge may be one of 'rising' (default) or 'falling'\n"
    " -O, --order=order   Order is one of 'msb' (default) or 'lsb'\n"
    "\n"
    "V24 options\n"
    " -R, --rx=signame    Name of RX signal in VCD-file (required option)\n"
    " -T, --tx=signame    Name of TX signal in VCD-file\n"
    " -B, --baud=speed    Baudrate on serial line (default=9600)\n"
    " -I, --invert        Invert signal levels (start <= 0V, stop >= 3V)\n"
    "\n"
    "Other options\n"
    " -v, --verbose       Verbose mode\n"
    " -V, --version       Print program version\n"
    " -?, --help          This help\n"
    "\n"
    ;

static void
help(void)
{
    printf(usage_str);
}


/*****************************************************************************
 *
 *  D a t a   s t r u c t u r e s
 *
 *****************************************************************************/

// Structure to hold signal names and IDs.
typedef struct {
    char technologyName[MAX_SIGNAME];
    char name[MAX_SIGNAME];
    int  required;
    char id;
} t_signalID;

// Structure to hold events.
typedef struct  {
    long            time;
    unsigned char   sig[MAX_SIGNALS];
} t_event;
t_event *p_Events;


//
//  ---  I2C  ---
//
#define SIG_SCL  0
#define SIG_SDA  1
static t_signalID i2c_sigDescr[] = {
    {"SCL", "scl", 1, 0},
    {"SDA", "sda", 1, 0}
};


//
//  ---  SPI  ---
//
#define SIG_SSN  0
#define SIG_SCK  1
#define SIG_MOSI 2
#define SIG_MISO 3
static t_signalID spi_sigDescr[] = {
    {"SSN" , "ssn" , 0, 0},
    {"SCK" , "sck",  1, 0},
    {"MOSI", "mosi", 1, 0},
    {"MISO", "miso", 1, 0}
};


//
//  ---  V24  ---
//
#define SIG_RX  0
#define SIG_TX  1
static t_signalID v24_sigDescr[] = {
    {"RX", "rx", 1, 0},
    {"TX", "tx", 0, 0}
};



/*****************************************************************************
 *  Functions   :   q u i t ()
 *                  q u i t p()
 *
 *  Parameters  :   varargs
 *  Return value:   -
 *  Description :   Display error messages and exit program.
 *
 *****************************************************************************/
#define quit( ... )  ({ fprintf( stderr, __VA_ARGS__ );               (void)fflush(stderr); exit( EXIT_FAILURE ); })
#define quitp( ... ) ({ fprintf( stderr, __VA_ARGS__ ); perror(NULL); (void)fflush(stderr); exit( EXIT_FAILURE ); })

static int verbose = 0;


/*****************************************************************************
 *  Function    :   g e t _ o p t i o n s ()
 *
 *  Parameters  :   int argc, char *argv[]
 *  Return value:   void
 *  Description :   Scans all command line options and fills the options
 *                  structure.
 *
 *****************************************************************************/
#define OPTIONS     "i:o:t:c:d:s:m:M:O:e:R:T:B:IvVh?"
#define TYPE_I2C    1
#define TYPE_SPI    2
#define TYPE_V24    3

static const char short_options[] = OPTIONS;
static const struct option long_options[] = {
    // char name[], int has_arg, int *flag, int val
    { "ifile",      1, 0, 'i' },    // file options
    { "ofile",      1, 0, 'o' },
    { "type",       1, 0, 't' },
    { "scl",        1, 0, 'c' },    // i2c options
    { "sda",        1, 0, 'd' },
    { "ssn",        1, 0, 's' },    // spi options
    { "sck",        1, 0, 'c' },
    { "mosi",       1, 0, 'm' },
    { "miso",       1, 0, 'M' },
    { "order",      1, 0, 'O' },
    { "edge",       1, 0, 'e' },
    { "rx",         1, 0, 'R' },    // serial options
    { "tx",         1, 0, 'T' },
    { "baud",       1, 0, 'B' },
    { "invert",     0, 0, 'I' },
    { "verbose",    0, 0, 'v' },    // other options
    { "version",    0, 0, 'V' },
    { "help",       0, 0, 'h' },
    { "?",          0, 0, '?' },
    { NULL,         0, 0,  0  }
};


static struct {
    char          *ifilename;
    char          *ofilename;
    int           type;
    char          *scl;
    char          *sda;
    char          *ssn;
    char          *sck;
    char          *mosi;
    char          *miso;
    int           spi_order;
    int           spi_edge;
    char          *rx;
    char          *tx;
    int           baudrate;
    int           v24_invert;
    int           verbose;
} options = {
    NULL  ,     // ifile
    NULL  ,     // ofile
    0     ,     // type (0=i2c,1=spi,2=v24)
    "scl" ,     // scl
    "sda" ,     // sda
    "ssn" ,     // ssn
    "sck" ,     // sck
    "mosi",     // mosi
    "miso",     // miso
    1     ,     // order (0=lsb,1=msb)
    1     ,     // edge  (0=falling,1=rising)
    "rx"  ,     // rx
    "tx"  ,     // tx
    9600  ,     // baudrate
    0     ,     // v24_invert (0=no invert,1=invert)
    0           // verbose
};

static void
get_options(int argc, char *argv[] )
{
    int opt_char;

    while ((opt_char = getopt_long_only(argc, argv, short_options, long_options, NULL )) != -1) {
        switch( opt_char ) {
            case 'i':
                options.ifilename = optarg;
                break;
            case 'o':
                options.ofilename = optarg;
                break;
            case 't':
                if( !strcasecmp( optarg, "i2c" ) ) options.type = TYPE_I2C;
                if( !strcasecmp( optarg, "spi" ) ) options.type = TYPE_SPI;
                if( !strcasecmp( optarg, "v24" ) ) options.type = TYPE_V24;
                break;
            case 'c':
                options.scl = optarg;
                options.sck = optarg;
                break;
            case 'd':
                options.sda = optarg;
                break;
            case 's':
                options.ssn = optarg;
                spi_sigDescr[SIG_SSN].required = 1;
                break;
            case 'm':
                options.mosi = optarg;
                break;
            case 'M':
                options.miso = optarg;
                break;
            case 'O':
                if( !strcasecmp( optarg, "lsb" ) )
                    options.spi_order = 0;
                else if( !strcasecmp( optarg, "msb" ) )
                    options.spi_order = 1;
                else
                    quit( "ERROR: option '--order=' requires one of 'msb' or 'lsb'\n" );
                break;
            case 'R':
                options.rx = optarg;
                break;
            case 'T':
                options.tx = optarg;
                v24_sigDescr[SIG_TX].required = 1;
                break;
            case 'B':
                options.baudrate = strtol( optarg, NULL, 0 );
                break;
            case 'I':
                options.v24_invert = 1;
                break;
            case 'e':
                if( !strcasecmp( optarg, "rising" ) )
                    options.spi_edge = 1;
                else if( !strcasecmp( optarg, "falling" ) )
                    options.spi_edge = 0;
                else
                    quit( "ERROR: option '--edge=' requires one of 'falling' or 'rising'\n" );
                break;
            case 'v':
                options.verbose = 1;
                verbose = 1;
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
                quit( "ERROR: No such option '%c'. Use option '--help' for help.\n", opt_char );
                break;
        }
    }

    // Confirm required options are available
    if( options.type != TYPE_I2C && options.type != TYPE_SPI && options.type != TYPE_V24 )
        quit( "ERROR: one of '--type=i2c' or '--type=spi' or '--type=v24' is required.\n" );
}


/*****************************************************************************
 *  Function    :   g e t _ s i g n a l N a m e s ()
 *
 *  Parameters  :   FILE       *ifp         - pointer to input file
 *                  t_signalID *p_id_sig    - pointer to signal descriptors
 *                  char       *p_timeUnit  - pointer to time unit string
 *  Return value:   < 0   : signals not in VCD header
 *                  -10   : input not in VCD-format
 *  Description :   Scan the VCD-header for requested signals and fill
 *                  the signal descriptor structure. Additionall, get
 *                  the time unit from VCD-header.
 *****************************************************************************/
static int
get_signalNames( FILE *ifp, t_signalID *p_id_sig, int n, char *p_timeUnit )
{
    char id = 0;
    char *p, *pp;
    int state = 0;
    int lines = 0;
    int result = 0;
    char linebuff[MAX_LINEWIDTH];
    int i;

    while( fgets( linebuff, sizeof(linebuff), ifp) != NULL )  {
        lines++;

        // Get id's for required signals.
        state = 0;
        p = strtok( linebuff, " " );
        while( p != NULL )  {
            // Check for end condition.
            if( !strcasecmp( p, "$enddefinitions" ) )  {
                for( i = 0; i < n; i++ ) {
                    if( p_id_sig[i].id == 0 && p_id_sig[i].required == 1 ) {
                        fprintf( stderr, "ERROR: cannot find %s signal '%s' in vcd-file\n", p_id_sig[i].technologyName, p_id_sig[i].name );
                        result--;
                    }
                }
                if( verbose ) {
                    printf( "get_signalNames(): number of header lines = %d \n", lines );
                    for( i = 0; i < n; i++ ) {
                        printf( "get_signalNames(): %s name=%s, id=%c\n", p_id_sig[i].technologyName, p_id_sig[i].name, p_id_sig[i].id != 0 ? p_id_sig[i].id : ' ' );
                    }
                }
                return result;
            }
            switch( state )  {
                case 0:
                    if( !strcasecmp( p, "$timescale" ) ) {
                        if( (pp = strtok( NULL, " " )) != NULL )
                            strncpy( p_timeUnit, pp+1, 2 );
                    }
                    if( !strcasecmp( p, "$var" ) )
                        state = 1;
                    break;
                case 1:
                    for( i = 0; i < n; i++ ) {
                        if( p_id_sig[i].id == 0 && !strcasecmp( p, p_id_sig[i].name ) )  {
                            p_id_sig[i].id = id;
                            state = 2;
                        }
                    }
                    break;
                default:
                    break;
            }

            id = *p;
            p = strtok( NULL, " " );
        }
    }
    fprintf( stderr, "ERROR: Input not in VCD format\n" );
    return -10;
}


/*****************************************************************************
 *  Function    :   s c a n _ e v e n t s ()
 *
 *  Parameters  :   t_i2cEvent *pi2cEvents - pointer to array with signal
 *                  events
 *  Return value:   number of events filled into t_i2cEvent array
 *  Description :   Scans the VCD-file for SCL and SDA events and fills
 *                  the event array.
 *****************************************************************************/
static int
scan_events( FILE *ifile, t_signalID *p_id_sig, int n_sigs, t_event *pEventArray )
{
    char *p;
    char linebuff[MAX_LINEWIDTH];
    int i , n, value, n_events[MAX_SIGNALS];

    for( i = 0; i < n_sigs; i++ ) {
        n_events[i] = 0;
        pEventArray[0].sig[i] = '-';
    }
    pEventArray[0].time = 0L;

    n = 0;
    while( fgets( linebuff, sizeof(linebuff), ifile ) != NULL )  {
        // Read the timings.
        p = &linebuff[0];
        if( *p == '#' )  {
            // prepare new entry if something changed for one of the signals
            for( i = 0; i < n_sigs; i++ )  {
                if( pEventArray[n].sig[i] != '-' )  {
                    if( n < MAX_EVENTS ) {
                        n++;
                        break;
                    } else {
                        quit( "ERROR: internal buffer overflow\n" );
                    }
                }
            }

            pEventArray[n].time = strtol( p+1, NULL, 0 );
            for( i = 0; i < n_sigs; i++ )
                pEventArray[n].sig[i] = '-';
        }

        value = *p;
        if( value == '1' || value == '0' )  {
            for( i = 0; i < n_sigs; i++ )  {
                if( *(p+1) == p_id_sig[i].id )  {
                    pEventArray[n].sig[i] = value;
                    n_events[i]++;
                }
            }
        }
    }
    if( verbose )  {
        for( i = 0; i < n_sigs; i++ )
            printf( "scan_events(): number of %s events = %d \n", p_id_sig[i].technologyName, n_events[i] );
    }
    return n+1;
}


/*****************************************************************************
 *  Function    :   i 2 c _ t r a n s l a t e ()
 *
 *  Parameters  :   FILE *ofile         - file pointer to output file
 *                  t_Event *EventArray - pointer to array with signal events
 *                  int n_events        - number of entries in event array
 *                  char *p_timeunit    - pointer to time unit string
 *  Return value:   -
 *  Description :   Reads the event array and translates to human readable
 *                  I2C transactions in ASCII format.
 *****************************************************************************/
static void
i2c_translate( FILE *ofile, t_event *pEventArray, int n_events, char *p_timeunit )
{
    int sda, sda_event, last_sda;
    int scl, scl_event;
    int start = 0;
    int n;
    int bit_num = 0, data = 0;

    scl = 0;
    sda = last_sda = 0;
    for( n = 0; n < n_events; n++ ) {
        scl_event = pEventArray[n].sig[SIG_SCL] != '-' ? 1 : 0;
        sda_event = pEventArray[n].sig[SIG_SDA] != '-' ? 1 : 0;
        last_sda = sda;
        if( pEventArray[n].sig[SIG_SCL] != '-' ) { scl = pEventArray[n].sig[SIG_SCL] - '0'; }
        if( pEventArray[n].sig[SIG_SDA] != '-' ) { sda = pEventArray[n].sig[SIG_SDA] - '0'; }

        // sda event during scl = high
        if( scl == 1 && !scl_event && sda_event )  {
            // START
            if( !sda )  {
                if( start == 0 )  {
                    // Regular START
                    fprintf( ofile, "\n%10ld %s: ", pEventArray[n].time, p_timeunit );
                } else  {
                    // Repeated START
                }
                start = 1;
                fprintf( ofile, " S " );
                bit_num = -1;   // It requires one edge to take the clock low
                data = 0;
            }
            // STOP
            if( sda )  {
                if( start != 1 )
                    fprintf( ofile, "%10ld %s:  ", pEventArray[n].time, p_timeunit );
                fprintf( ofile, "P" );
                start = 0;
            }
        }

        // falling edge of scl
        if( scl_event && scl == 0 )  {
            // Bit7..0
            if( bit_num >= 0 && bit_num < 8 )  {
                data = (data << 1) | (last_sda ? 1 : 0);
            }
            // ACK
            if( bit_num == 8 ) {
                fprintf( ofile, "%02x ", data );
                fprintf( ofile, "%s", last_sda ? "N  " : "A  " );
                bit_num = 0;
                data = 0;
            } else {
                bit_num++;
            }
        }
    }
    fprintf( ofile, "\n\n" );
}


/*****************************************************************************
 *  Function    :   s p i _ t r a n s l a t e ()
 *
 *  Parameters  :   FILE *ofile         - file pointer to output file
 *                  int edge            - 0=falling edge, 1=rising edge
 *                  int order           - 0=lsb first, 1=msb first
 *                  t_Event *EventArray - pointer to array with signal events
 *                  int n_events        - number of entries in event array
 *                  char *p_timeunit    - pointer to time unit string
 *  Return value:   -
 *  Description :   Reads the event array and translates to human readable
 *                  I2C transactions in ASCII format.
 *****************************************************************************/
static void
spi_translate( FILE *ofile, int edge, int order, t_event *pEventArray, int n_events, char *p_timeunit )
{
    int ssn,  ssn_event;
    int sck,  sck_event;
    int mosi, last_mosi;
    int miso, last_miso;
    int i;
    int bit_num = 0, mosi8 = 0, miso8 = 0;

    ssn  = 0;
    sck  = 0;
    mosi = last_mosi = 0;
    miso = last_miso = 0;
    for( i = 0; i < n_events; i++ ) {
        ssn_event  =  pEventArray[i].sig[SIG_SSN] != '-' ? 1 : 0;
        sck_event  =  pEventArray[i].sig[SIG_SCK] != '-' ? 1 : 0;
        last_mosi = mosi;
        last_miso = miso;
        if( pEventArray[i].sig[SIG_SSN ] != '-' ) {ssn  = pEventArray[i].sig[SIG_SSN ] - '0';}
        if( pEventArray[i].sig[SIG_SCK ] != '-' ) {sck  = pEventArray[i].sig[SIG_SCK ] - '0';}
        if( pEventArray[i].sig[SIG_MOSI] != '-' ) {mosi = pEventArray[i].sig[SIG_MOSI] - '0';}
        if( pEventArray[i].sig[SIG_MISO] != '-' ) {miso = pEventArray[i].sig[SIG_MISO] - '0';}

        if( ssn_event && ssn == 0 )  {
            // Start = falling edge of SSN
            fprintf( ofile, "%10ld %s: SSN=0 ", pEventArray[i].time, p_timeunit );
            bit_num = 0;
            mosi8 = miso8 = 0;
        } else if( ssn_event && ssn == 1 )  {
            // Stop = rising edge of SSN
            if( pEventArray[i].time == 0L )
                fprintf( ofile, "%10ld %s: SSN=1\n", pEventArray[i].time, p_timeunit );
            else
                fprintf( ofile, "SSN=1\n" );
        }

        // Sampling edge of SCK
        if(   (sck_event && edge==1 && sck == 1)
           || (sck_event && edge==0 && sck == 0) ) {

            // Bit7..0
            if( bit_num < 8 )  {
                if( order == 1 )  {
                    // msb first -> shift in from right
                    mosi8 = (mosi8 << 1) | (last_mosi ? 1 : 0);
                    miso8 = (miso8 << 1) | (last_miso ? 1 : 0);
                } else {
                    // lsb first -> shift in from left
                    mosi8 = (mosi8 >> 1) | (last_mosi ? 0x80 : 0);
                    miso8 = (miso8 >> 1) | (last_miso ? 0x80 : 0);
                }
            }
            // one byte complete
            if( bit_num == 7 ) {
                fprintf( ofile, "%02x/%02x ", mosi8, miso8 );
                bit_num = 0;
                mosi8 = miso8 = 0;
            } else {
                bit_num++;
            }
        }
    }
    fprintf( ofile, "\n" );
}


/*****************************************************************************
 *  Function    :   v 2 4 _ t r a n s l a t e ()
 *
 *  Parameters  :   FILE *ofile         - file pointer to output file
 *                  int baud            - baudrate
 *                  int invert          - signal levels are inverted
 *                  t_Event *EventArray - pointer to array with signal events
 *                  int n_events        - number of entries in event array
 *                  char *p_timeunit    - pointer to time unit string
 *  Return value:   -
 *  Description :   Reads the event array and translates to human readable
 *                  v24 transactions in ASCII format.
 *****************************************************************************/

/* From current time calculate the index of the current sample point.  */
static int
v24_getSamplePoint( int bit_num, long rx_sample_points[], long curr_time )
{
    int i;
    for( i = bit_num; i < 10; i++ )  {
        if( curr_time > rx_sample_points[i] )
            break;
    }
    return i;
}

/* Combine the v24 samples to a single data byte.   */
static int
v24_combineBits( int bit_array[] )
{
    int i, x;
    for( i = 1, x = 0; i < 9; i++ )  {
        x *= 2;
        if( bit_array[i] ) x++;
    }
    return x & 0xff;
}

static void
v24_translate( FILE *ofile, int baud, int invert, t_event *pEventArray, int n_events, char *p_timeunit )
{
    struct {
        int     curr_value;
        int     last_value;
        int     event;
        int     bit_num;
        long    start;
        long    sample_points[10];
        int     bits[10];
    } rx, tx;

    int n, i;
    int scan_point;

    rx.curr_value = rx.last_value = rx.event = rx.bit_num = 0;
    tx.curr_value = tx.last_value = tx.event = tx.bit_num = 0;
    rx.start = 0L;
    tx.start = 0L;

    fprintf( ofile, "               RX  TX\n" );
    for( n = 0; n < n_events; n++ )  {
        //
        //  --  RX  --
        //
        rx.event = pEventArray[n].sig[SIG_RX] != '-' ? 1 : 0;
        rx.last_value = rx.curr_value;
        if( pEventArray[n].sig[SIG_RX] != '-' ) {
            rx.curr_value = pEventArray[n].sig[SIG_RX] - '0';
            if( invert )
                rx.curr_value = !rx.curr_value;
        }
        // Get all bits, whenever a transfer is ongoing.
        if( rx.start != 0L && rx.event )  {
            scan_point = v24_getSamplePoint( rx.bit_num, rx.sample_points, pEventArray[n].time );
            while( rx.bit_num <= scan_point )  {
                rx.bits[rx.bit_num++] = rx.last_value;
            }
            if( rx.bit_num > 8 )  {
                fprintf( ofile, "%10ld %s: ", rx.start, p_timeunit );
                fprintf( ofile, "%02x\n", v24_combineBits( rx.bits ) );
                rx.start = 0L;
            }
        }
        // Check for start bit.
        if( rx.start == 0L && rx.event && rx.curr_value == 0 ) {
            rx.start = pEventArray[n].time;
            for( i = 0; i < 10; i++ )  {
                rx.sample_points[i] = rx.start + 1L*(i)*1.0e6/baud + 1L*0.5e6/baud;
            }
            rx.bit_num = 0;
        }

        //
        //  --  TX  --
        //
        tx.event = pEventArray[n].sig[SIG_TX] != '-' ? 1 : 0;
        tx.last_value = tx.curr_value;
        if( pEventArray[n].sig[SIG_TX] != '-' ) {
            tx.curr_value = pEventArray[n].sig[SIG_TX] - '0';
            if( invert )
                tx.curr_value = !tx.curr_value;
        }
        // Get all bits, whenever a transfer is ongoing.
        if( tx.start != 0L && tx.event )  {
            scan_point = v24_getSamplePoint( tx.bit_num, tx.sample_points, pEventArray[n].time );
            while( tx.bit_num <= scan_point )  {
                tx.bits[tx.bit_num++] = tx.last_value;
            }
            if( tx.bit_num > 8 )  {
                fprintf( ofile, "%10ld %s: ", tx.start, p_timeunit );
                fprintf( ofile, "    %02x\n", v24_combineBits( tx.bits ) );
                tx.start = 0L;
            }
        }
        // Check for start bit.
        if( tx.start == 0L && tx.event && tx.curr_value == 0 ) {
            tx.start = pEventArray[n].time;
            for( i = 0; i < 10; i++ )  {
                tx.sample_points[i] = tx.start + 1L*(i)*1.0e6/baud + 1L*0.5e6/baud;
            }
            tx.bit_num = 0;
        }
    }
    fprintf( ofile, "\n" );
}


/*****************************************************************************
 *  Function    :   o p e n F i l e ()
 *
 *  Parameters  :   char *filename - name of input file inf VCD-format
 *                  char *mode     - "rb" for input, "wb" for ouput
 *  Return value:   FILE *fp       - pointer to file descriptor
 *  Description :   Opens input and output file. If not specified on the
 *                  command line or filename == '-' returns stdin for input
 *                  and stdout for output.
 *****************************************************************************/
static FILE *
openFile( char *filename, char* mode )
{
    FILE *fp = NULL;

    /* Check for stdin and stdout.  */
    if( (filename == NULL || *filename == '-') && !strncasecmp(mode,"r",1) ) return stdin;
    if( (filename == NULL || *filename == '-') && !strncasecmp(mode,"w",1) ) return stdout;

    if(  filename == NULL )
        quit( "ERROR: Internal error when opening files\n" );

    if( (fp=fopen(filename, mode)) == NULL )
        quitp( "ERROR: Cannot open input file '%s' - ", filename );
    return fp;
}


/*****************************************************************************
 *  The main function
 *
 *     m a i n ()
 *
 *****************************************************************************
 */
int
main( int argc, char *argv[] )
{
    FILE *ifile;
    FILE *ofile;

    int n_events;
    char timeunit[3] = "\0\0\0";

    //
    //  Scan command line arguments.
    //
    get_options(argc, argv);

    //
    //  Open files.
    //
    ifile = openFile( options.ifilename, "rb" );
    ofile = openFile( options.ofilename, "wb" );

    // Malloc the required buffer space.
    if( (p_Events = malloc(MAX_EVENTS*sizeof(t_event))) == NULL )
        quitp( "ERROR: Cannot malloc(%d).\n", MAX_EVENTS*sizeof(t_event) );

    //
    //   I 2 C  -  t r a n s l a t i o n
    //
    if( options.type == TYPE_I2C )  {

        // Set the signal names.
        strncpy( i2c_sigDescr[SIG_SCL].name, options.scl, MAX_SIGNAME-1 );
        strncpy( i2c_sigDescr[SIG_SDA].name, options.sda, MAX_SIGNAME-1 );

        // Find the signals in VCD-file.
        if( get_signalNames( ifile, i2c_sigDescr, 2, timeunit ) < 0 )
            quit( "ERROR: Error during scan of VCD-file header.\n" );

        // Scan all samples.
        n_events = scan_events( ifile, i2c_sigDescr, 2, p_Events );

        /* Translate to I2C transactions.   */
        i2c_translate( ofile, p_Events, n_events, timeunit );
    }

    //
    //  S P I  -  t r a n s l a t i o n
    //
    else if( options.type == TYPE_SPI )  {

        // Set the signal names.
        strncpy( spi_sigDescr[SIG_SSN].name , options.ssn , MAX_SIGNAME-1 );
        strncpy( spi_sigDescr[SIG_SCK].name , options.sck , MAX_SIGNAME-1 );
        strncpy( spi_sigDescr[SIG_MOSI].name, options.mosi, MAX_SIGNAME-1 );
        strncpy( spi_sigDescr[SIG_MISO].name, options.miso, MAX_SIGNAME-1 );

        // Find the signals in VCD-file.
        if( get_signalNames( ifile, spi_sigDescr, 4, timeunit ) < 0 )
            quit( "ERROR: Error during scan of VCD-file header.\n" );

        // Scan all samples.
        n_events = scan_events( ifile, spi_sigDescr, 4, p_Events );

        // Translate to SPI transactions.
        spi_translate( ofile, options.spi_edge, options.spi_order,
                              p_Events, n_events, timeunit );

    //
    //  V 2 4  -  t r a n s l a t i o n
    //
    } else if( options.type == TYPE_V24 )  {

        // Set the signal names.
        strncpy( v24_sigDescr[SIG_RX].name, options.rx, MAX_SIGNAME-1 );
        strncpy( v24_sigDescr[SIG_TX].name, options.tx, MAX_SIGNAME-1 );

        // Find the signals in VCD-file.
        if( get_signalNames( ifile, v24_sigDescr, 2, timeunit ) < 0 )
            quit( "ERROR: Error during scan of VCD-file header.\n" );

        // Scan all samples.
        n_events = scan_events( ifile, v24_sigDescr, 2, p_Events );

        // Translate to I2C transactions.
        v24_translate( ofile, options.baudrate  , options.v24_invert,
                              p_Events, n_events, timeunit );
    }


    else  {
        quit( "ERROR: internal error (invalid translation type).\n" );
    }

    return 0;
}
