#ifndef _mydefs_h_
#define _mydefs_h_


typedef unsigned char  u8;
typedef   signed char  s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;


#define vu8(x)  (*(volatile u8*)&(x))
#define vs8(x)  (*(volatile s8*)&(x))
#define vu16(x) (*(volatile u16*)&(x))
#define vs16(x) (*(volatile s16*)&(x))
#define vu32(x) (*(volatile u32*)&(x))
#define vs32(x) (*(volatile s32*)&(x))


struct bits {
  u8 b0:1;
  u8 b1:1;
  u8 b2:1;
  u8 b3:1;
  u8 b4:1;
  u8 b5:1;
  u8 b6:1;
  u8 b7:1;
} __attribute__((__packed__));

#define SBIT(port,pin) ((*(volatile struct bits*)&port).b##pin)


#endif
