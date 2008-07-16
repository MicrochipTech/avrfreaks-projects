#ifndef _HTTPD_DATA_H_
#define _HTTPD_DATA_H_
#include <avr/pgmspace.h>

//response codes:
extern PROGMEM unsigned char HTTPD_RESPONSE_OK[];
extern PROGMEM unsigned char HTTPD_RESPONSE_404[];

//content types:
extern PROGMEM unsigned char HTTPD_CTYPE_HTML[];
extern PROGMEM unsigned char HTTPD_CTYPE_BMP[];
extern PROGMEM unsigned char HTTPD_CTYPE_JPG[];
extern PROGMEM unsigned char HTTPD_CTYPE_PNG[];
extern PROGMEM unsigned char HTTPD_CTYPE_GIF[];

//rnrn
extern PROGMEM unsigned char HTTPD_RNRN[];

//file not found (404) template
extern PROGMEM unsigned char HTTPD_FILE_NOT_FOUND[];

//main html template
extern PROGMEM unsigned char HTTPD_FILE_INDEX_HTML[];

//include files
extern PROGMEM unsigned char HTTPD_FILE_SITE_START_HTML[];
extern PROGMEM unsigned char HTTPD_FILE_SITE_TEMP_HTML[];
extern PROGMEM unsigned char HTTPD_FILE_SITE_CAM_HTML[];
extern PROGMEM unsigned char HTTPD_FILE_SITE_UPLOAD_HTML[];
extern PROGMEM unsigned char HTTPD_FILE_SITE_FSLS_HTML[];
extern PROGMEM unsigned char HTTPD_FILE_SITE_SERVO_OK[];
//jpgs
extern PROGMEM unsigned char HTTPD_CAM_BUSY_JPG[];

#endif
