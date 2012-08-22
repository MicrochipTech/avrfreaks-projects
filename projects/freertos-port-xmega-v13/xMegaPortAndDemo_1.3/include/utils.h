//
// file: utils.h
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#ifndef _utils_include_h_
#define _utils_include_h_

void dezimalPkt(char* str);
void dezimalPkt2(char* str);
void insert_c(char* str,u8 pos,char c);
void strPrep(char*,u8 len);
void strCheck(char*,u8 len);
u8 calcChksum(char* resultString);

// states for the statemachine that is used to check the datagrams when sending
//
typedef enum dgState
{
	outside,    // not in known message
	inside,     // '$' or '!' received but end of message not yet reached
	eom,		// '*' just received
	chks1,      // first char of checksum just received
} dgState_t;

typedef struct dgStateMaschine
{
	dgState_t state;
	u8          chksum;
	char        chkstr[3]; 		
} dgStateMaschine_t;



void sm_init(dgStateMaschine_t *sm);
bool sm_setState(dgStateMaschine_t* sm, char cChar);
bool sm_isEom(dgStateMaschine_t* sm, char cChar);


#endif // _utils_include_h_
