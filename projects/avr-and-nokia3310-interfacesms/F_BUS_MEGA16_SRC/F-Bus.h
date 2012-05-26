#ifdef __GNUC__ // to avoid error messages in visual studio editor
	#include <avr/io.h>
#else
	#include <avr/iom8.h>
#endif
#include <string.h> 
#include <ctype.h> 
#include <util/delay.h>
#include "usart.h"
#include "display.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#define ON  1
#define OFF 0

#define YES  1
#define NO   0

#define SMSstart 42 // Messagge starts here
unsigned char decodeSMS(unsigned char *src,unsigned char *out,unsigned char size);
unsigned char sendACK(void);
void addChkSumAndSend(unsigned char *Frame,char con);
void _makelower (unsigned char *src);
unsigned char initPhone(void);
void ParseSMS(unsigned char *msg);
unsigned char SendSMS(const char *message,unsigned char *PhoneNumber);
void SendNextFrame(void);
/*--------------------------------------*/
//Send SMS Fields difinitions
#define MsgLength     28     // Message length index
#define RecNo         30         // Recipient
#define RecNoLength   11         // Recipient No. length
#define MsgStartIndex 48         // Compressed message will always start here

