#include "Define.h"
//------------------------------------------------------------------------------------------------------------------
/*
Parser V 1.0 By Easy system Gvl
Traduzione di un buffer (file inviato al controllore) in lista comandi da eseguire
*/
//------------------------------------------------------------------------------------------------------------------
typedef enum {
 NoDay = -1,
 Mon = 0,
 Tue,
 Wed,
 Thu,
 Fri,
 Sat,
 Sun
} Week;

//------------------------------------------------------------------------------------------------------------------
typedef enum  {
  NotCommand = 0,
  PI,
  PQ,       // PIx, PQx	Fronte di salita dell'ingresso Ix (1..4) o dell'uscita Qx (1..4)
  PNI,
  PNQ,      // PNIx, PNQx	Fronte di discesa dell'ingresso Ix (1..4) o dell'uscita Qx (1..4)
  T,        // Txx   dove x = 1..16
  W,        // Wxx   dove x = 1..16
  AMajor,   // A >xxx	        Ingresso analogico 		xxx = -99 .. 999
  AMinor,   // A <xxx	        Ingresso analogico
  AInterval // A1 ><xxx yyy	Ingresso analogico 1 xxx < yyy

} Command;

//------------------------------------------------------------------------------------------------------------------
typedef enum  {
 NoAction = 0,
 SENDC,       //   SMSxx		    Invia l'sms numero xx (00-10); con conferma di chiamata
 SMS,         //	SEND SMSxx  	Invia l'sms numero xx (00-10); prima voce in rubrica
 Q            //   Qx = 0/1
} Action;

//------------------------------------------------------------------------------------------------------------------
typedef struct {
  Command           Cmd;       // evento da monitorare
  unsigned char     CmdNum;    // eventuale numero I/O o indice stringa a cui si riferisce
  int               MinVal;    // eventuale valore minimo
  int               MaxVal;    // eventuale valore massimo
  Action            Act;       // Azione da eseguire
  unsigned char     ActNum;    // Evantuale target sull'azione
  unsigned char     Val;       // Eventuale valore da inserire per l'azione
} ProgramStep ;

//------------------------------------------------------------------------------------------------------------------
typedef struct {
	unsigned char	Out;	// 0:3 numero di uscita per fare Apricancello
	unsigned int	Delay;	// durata impulso in unità da 100 ms
} GATE;

//------------------------------------------------------------------------------------------------------------------
typedef struct {
	unsigned char	Out1;	// 0:3 numero di uscita da mandare a 1 al primo squillo
	unsigned char	Out2;	// 0:3 numero di uscita da mandare a 1 al secondo squillo
	unsigned int	Delay;	// durata impulso in unità da 100 ms
} TOGGLERING;

//------------------------------------------------------------------------------------------------------------------
bool               GetClock             ( char * , char * , char * , char * , char * , char * , char *);
bool               GetValOut            ( char * , char * , bool *);
bool               GetSMSMessageToEEmem ( char *);
bool               GetIOMessageToEEmem  ( char *);
bool               GetSmsNoPower        ( char * , bool *);
bool               CheckCommandString   ( char *);
bool               GetTemporalEvent     ( char * , DATA_EVENTS *, uint32_t *);
bool               GetWeekTemporalEvent ( char * , DATA_EVENTS *, uint32_t *,  Week *, DATA_EVENTS *);
bool               GetOffsetPTC         ( char *);
bool               GetPrg               ( char *);
bool               ProgramOnOff         ( char *, bool *);
bool               CheckGate            ( char *, char *, int *);
bool               CheckProgram         ( char *);
void               ClearProgramEEPROM   ( int );
bool               Analyze              ( char *, int );
void               DelChar              ( char *, char);
Week               GetDayInWeek         ( unsigned long int);
unsigned long int  DecodeTime           ( char * );
unsigned long int  DecodeDate           ( char *, bool );
unsigned long int  DecodeDateTime       ( char *, bool );
bool               IsOnlyTime           ( unsigned long int );
unsigned long int  GetTime              ( unsigned long int );
void               EncodeDateTime       ( unsigned long int, char * );
bool               GetCreditModality    ( char * );
bool               ToggleRing           ( char *, char *, char *, int *);

extern int         StrIndex             ( char *, char );
extern void        CalibrateAnalogIn    ( void);


