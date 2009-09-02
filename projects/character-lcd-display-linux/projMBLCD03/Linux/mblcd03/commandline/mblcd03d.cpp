/*
 * Include Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <syslog.h>

extern "C" {
#include "hiddata.h"
#include "../firmware/usbconfig.h"  /* for device VID, PID, vendor name and product name */
}

/**
 * Local Variables
 */
static	usbDevice_t     *_myDev = NULL;
static 	char	*_strWeek[] = {
					"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
// for CPU BUSY
static	int _nCurCpu[4];
static	int _nPreCpu[4];

/**
 * Prototypes
 */
int	main( int argc, char *argv[] );
static void	doMblcd03();

static void putSyslogMessage( int level, const char *pMsg )
{
	openlog( "mblcd03d", LOG_PID, LOG_DAEMON );
	syslog( level, "%s", pMsg );
	closelog();
}

/**
 * USBエラーメッセージ
 */
static char *usbErrorMessage(int errCode)
{
static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:      return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:    return "The specified device was not found";
        case USBOPEN_ERR_IO:          return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

/**
 * デバイス・オープン. <br/>
 */
static usbDevice_t  *openDevice(void)
{
usbDevice_t     *dev = NULL;
unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
int             vid = rawVid[0] + 256 * rawVid[1];
int             pid = rawPid[0] + 256 * rawPid[1];
int             err;

    if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0)) != 0){
		char dspbuf[256];
        sprintf(dspbuf,
				"error finding %s: %s\n", productName, usbErrorMessage(err));
		putSyslogMessage( LOG_ERR, dspbuf );
        return NULL;
    }
    return dev;
}

static char *putStringData( char *dest, char *strData )
{
	int nLen = strlen( strData );
	for ( int i=0; i<nLen; i++ ) {
		*dest++ = 0x14;
		*dest++ = *(strData+i);
	}
	return dest;
}

static void quit_signal( int sig )
{
	if ( _myDev != (usbDevice_t*)NULL ) {
		int		err;
		char	buffer[129];    /* room for dummy report ID */
        memset(buffer, 0, sizeof(buffer));
		char	*next = &buffer[1];
	    *next ++ = 0x03;
	    *next ++ = 0x03;
		next = putStringData( next, "Terminate." );
        if((err = usbhidSetReport(_myDev, buffer, 65)) != 0)   /* add a dummy report ID */
            fprintf(stderr, "error writing data: %s\n", usbErrorMessage(err));
		usleep( 500 );
    	usbhidCloseDevice(_myDev);
		_myDev = NULL;
	}

	putSyslogMessage( LOG_INFO, "terminate. mblcd03d" );
	exit( 0 );
}

static char *setCurrentDate( char *bufptr )
{
	time_t now;
	time( &now );
	struct tm *pt = localtime( &now );
	char dspbuf[20];
	sprintf( dspbuf, "%.2d/%.2d/%.2d %s.",
			(pt->tm_year%100), (pt->tm_mon+1), pt->tm_mday,
			_strWeek[pt->tm_wday]);
	char *dst = bufptr;
	for ( int i=0; i<13; i++ ) {
		*dst ++ = 0x14;
		*dst ++ = dspbuf[i];
	}

	return dst;
}

static char *setCurrentTime( char *bufptr )
{
	time_t now;
	time( &now );
	struct tm *pt = localtime( &now );

	char dspbuf[20];
	sprintf( dspbuf, "%.2d:%.2d:%.2d",
			pt->tm_hour, pt->tm_min, pt->tm_sec );
//printf( "%s\n", dspbuf );
	char *dst = bufptr;
	for ( int i=0; i<8; i++ ) {
		*dst ++ = 0x14;
		*dst ++ = dspbuf[i];
	}

	return dst;
}

/**
 * /proc/stat からCPU使用情報を取得する. <br/>
 *
 * /proc/statの一行目を読み込む。<br/>
 * 1行目は、cpu (user) (nice) (system) (idle) ...<br/>
 * 結果は、_nCurCpu[]に格納する。<br/>
 */
int	getStatInfo()
{
	int nRet = 0;
	char	wkbuf[80];
	FILE *fp = fopen( "/proc/stat", "r" );
	if ( fp == (FILE*)NULL ) {
		fprintf( stderr, "can't open file.\n" );
		nRet = -1;
	}
	else {
		fscanf( fp, "%s %d %d %d %d", wkbuf,
				&_nCurCpu[0], &_nCurCpu[1], &_nCurCpu[2], &_nCurCpu[3] );
		fclose( fp );
	}
	return nRet;
}

double _ddCpu = 0;
/**
 * CPUビジー情報の取得. <br/>
 * 
 * nFlag == 0 の場合に、情報を取得。<br/>
 */
char *setCpuBusy( char *dst, int nFlag )
{
	int i=0;
	// 情報更新
	if ( nFlag == 0 ) {
		for (i=0; i<4; i++ ) {
			_nPreCpu[i] = _nCurCpu[i];
		}
		getStatInfo();
	}
	
	int nTotal = 0;
	int nDif[4];
	for ( i=0; i<4; i++ ) {
		nDif[i] = _nCurCpu[i] - _nPreCpu[i];
		nTotal += nDif[i];
	}
	char dspbuf[16];
	if ( nTotal == 0 )
	{
		sprintf( dspbuf, "[ 0.0%%]" );
	}
	else {
		_ddCpu = ((double)(nDif[0]+nDif[1]+nDif[2])*100.0)/nTotal ;
		if ( _ddCpu > 99.9f ) {
			sprintf( dspbuf, "[100 %%]" );
		}
		else {
			sprintf( dspbuf, "[%4.1f%%]", _ddCpu );
		}
//printf( "%s\n", dspbuf );
	}
	for ( int i=0; i<7; i++ ) {
		*dst ++ = 0x14;
		*dst ++ = dspbuf[i];
	}

	return dst;
}

/**
 * 主処理ループ. <br/>
 *
 * SIGINTで脱出。<br/>
 */
static void	doMblcd03()
{
	char        buffer[129];    /* room for dummy report ID */
	char		dspbuf[256];
	int         err;
	int nErr = 0;

	signal( SIGINT, quit_signal );
	signal( SIGTERM, quit_signal );
	signal( SIGQUIT, quit_signal );
	signal( SIGHUP,	SIG_IGN );
	signal( SIGCLD, SIG_IGN );
	signal( SIGALRM, SIG_IGN );

	putSyslogMessage( LOG_INFO, "start. mblcd03d" );

    if((_myDev = openDevice()) == NULL)
        exit(1);

//	int len = 65;	
	int i=0;
	int nCpuTime = 1;	// ループ１回目は見送る

	getStatInfo();

	for ( ;; ) {

        memset(buffer, 0, sizeof(buffer));
		// 0バイト目は、0x00
		// 1バイト目から始める
		char *next = &buffer[1];
		if ( i==0 ) {
		    *next ++ = 0x03;	// 初回は画面クリア
			i=1;
		}
		else {					// ２回目以後はカーソル移動
		    *next ++ = 0x12;
		    *next ++ = 0x80;
		}

		// 日付+曜日をセット
		next = setCurrentDate( next );

		// カーソルを２行目の先頭に移動
		*next ++ = 0x12;
		*next ++ = 0xc0;
		next = setCurrentTime( next );

		if ( nCpuTime == 0 ) {
			// 空白を挿入
			*next ++ = 0x14;
			*next ++ = 0x20;
			next = setCpuBusy( next, nCpuTime );
		}
		if ( nCpuTime++ > 1 ) {
			nCpuTime = 0;
		}

		// 送信
        if((err = usbhidSetReport(_myDev, buffer, 65)) != 0) {
			if ( err == 2 ) {
				for ( int j=0; j<3; j++ ) {
	    			usbhidCloseDevice(_myDev);
   	 				if((_myDev = openDevice()) == NULL) {
						//fprintf( stderr, "open fail...\n" );
						sleep( 5 );
					}
					else {
						break;
					}
				}
				if ( _myDev == NULL ) {
					fprintf( stderr, "mblcd03: down.\n" );
					break;
				}
			}
			else {
            	sprintf(dspbuf, "error writing data: (%d)%s\n",
						err,usbErrorMessage(err));
				putSyslogMessage( LOG_WARNING, dspbuf );
			}
			nErr ++;
			if ( nErr >= 5 ) break;
		}
		else {
			nErr = 0;
		}

        memset(buffer, 0, sizeof(buffer));
		// 0バイト目は、0x00
		// 1バイト目から始める
		next = &buffer[1];
		if ( _ddCpu > 80.0 ) {
			*next ++ = 0x16;
			*next ++ = 0x02;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x03;
			*next ++ = 0xff;
			*next ++ = 0x16;
			*next ++ = 0x03;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
		}
		else if ( _ddCpu > 60.0 ) {
			*next ++ = 0x16;
			*next ++ = 0x02;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x16;
			*next ++ = 0x03;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x03;
			*next ++ = 0xff;
		}
		else {
			*next ++ = 0x16;
			*next ++ = 0x02;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x16;
			*next ++ = 0x03;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
			*next ++ = 0x00;
		}
        if((err = usbhidSetReport(_myDev, buffer, 65)) != 0) {
           	sprintf(dspbuf, "error writing LED data: (%d)%s\n",
					err,usbErrorMessage(err));
			putSyslogMessage( LOG_WARNING, dspbuf );
		}


		usleep( 300000 );
	}

    usbhidCloseDevice(_myDev);

	return;
}

/**
 * デーモン初期化処理. <br/>
 */
static void daemon_init( void fn (void))
{
	// 新しいセッションにして切り離す
	setsid();
	// 主処理関数の開始
	fn();

	exit(0);
}

/**
 * メイン・プロシージャ<br/>
 */
int	main( int argc, char *argv[] )
{ 
	FILE *fp;
	char *pathname = 0;

	int doDaemon = 1;
	for ( int i=0; i<argc; i++ ) {
		if ( strcasecmp( argv[i], "-i" )==0 ) {
			doDaemon = 0;
		}
		else if ( strncasecmp( argv[i], "-pidfile=", 9 )==0) {
			pathname = &argv[i][9];
		}
	}

	if ( doDaemon == 1 ) {	// デーモン処理
		int pid = fork();
		if ( pid < 0 )
			exit( pid );
		else if ( pid == 0 ) {
			if ( pathname != 0 ) {
				fp = fopen( pathname, "w" );
				if ( fp != (FILE*)NULL ) {
					fprintf( fp, "%d", getpid() );
					fclose( fp );
				}

			}
			daemon_init( doMblcd03 );
		}
	}
	else {					// 常駐処理
		doMblcd03();
	}
	return 0;
}
