#define BYTE  unsigned char
#define WORD  unsigned int

#define FALSE         0
#define TRUE          1

#define TGLBIT(REG, BIT)   (REG ^= (1 << BIT))
#define CLRBIT(REG, BIT)   (REG &= ~(1 << BIT))
#define SETBIT(REG, BIT)   (REG |= (1 << BIT))
#define TSTBIT(REG, BIT)   (REG & (1 << BIT))


typedef struct{
BYTE Count;
BYTE Mode;
BYTE Command;
}h_Serial;

//------ MODE ----------
    #define _Rx         0
    #define _OK         1
//----- Command --------
    #define _DATA       0
    #define _HELP       1
    #define _RESET      2
    #define _SETTING    3
