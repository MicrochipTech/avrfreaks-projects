/****************************************************************************/
/* File        : ios_def.h                                                  */
/* Descrizione : Definizioni                                                */
/* Autore      : Stefano Favillini                                          */
/* Data        : 11/12/1995                                                 */
/* Versione    : 1.1                                                        */
/****************************************************************************/

#ifndef __IOS_DEF_H
#define __IOS_DEF_H


#define BYTE    unsigned char
#define WORD    unsigned int
#define DWORD   unsigned long
#define BOOL    BYTE

#define SHORT   signed int
#define USHORT  unsigned int

#define TRUE    1
#define FALSE   0
#define HANDLE  BYTE
#define NULL    0


#define IOS_ERROR          0
#define IOS_SUCCESS        1         
#define IOS_QUEUEFULL      2
#define IOS_QUEUEEMPTY     3
#define IOS_TIMEOUTEVNT    4

typedef volatile struct TAG_MSG
{
  BYTE  nCode;
  BYTE  bParam;
  WORD  wParam;
  WORD  xParam;
 
}TAG_MSG;

#define IOS_MSG   struct TAG_MSG  

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

typedef void (*PFUNC)(void);
//void main(void);



#endif  /* __IOS_DEF_H  */
