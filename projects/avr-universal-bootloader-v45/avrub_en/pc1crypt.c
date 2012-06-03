/*

                           e Y8b    Y8b YV4.08P888 88e
                          d8b Y8b    Y8b Y888P 888 888D
                         d888b Y8b    Y8b Y8P  888 88"
                        d888WuHan8b    Y8b Y   888 b,
                       d8888888b Y8b    Y8P    888 88b,
           8888 8888       ,e,                                  888
           8888 888820088e  " Y8b Y888P ,e e, 888,8, dP"Y ,"Y88b888
           8888 8888888 88b888 Y8b Y8P d88 88b888 " C88b "8" 888888
           8888 8888888 888888  Y8b "  888   ,888    Y88D,ee 888888
           'Y88 88P'888 888888   Y8P    "YeeP"888   d,dP "88 888888
   888 88b,                    d8  888                     888
   888 88P' e88 88e  e88 88e  d88  888 e88 88e  ,"Y88b e88 888 ,e e, 888,8,
   888 8K  d888 888bd888 8Shaoziyang88d888 888b"8" 888d888 888d88 88b888 "
   888 88b,Y888 888PY888 888P 888  888Y888 888P,ee 888Y888 888888   ,888
   888 88P' "88 88"  "88 88"  888  888 "88 88" "88 888 "88 888 "YeeP"888


  Project:       AVR Universal BootLoader
  File:          pc1crypt.c
                 PC1 Cipher Algorithm ( Pukall Cipher 1 ) form "http://membres.lycos.fr/pc1/"
                 128/256 bits key
                 Optimized for avr 8 bit mcu
  Version:       1.2

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.6

  See readme.htm to get more information.

*/


#ifndef PC1_128
#define PC1_128   1
#endif

#ifndef PC1_256
#define PC1_256   2
#endif

#ifndef Algorithm
#define Algorithm PC1_128
#endif

unsigned int ax, bx, cx, dx, inter, cfc, cfd;
unsigned int si, x1a2;

#if (Algorithm == PC1_128)
unsigned int x1a0[9];
#elif  (Algorithm == PC1_256)
unsigned int x1a0[17];
#else
#error "Unknow encrypt algorithm!"
#endif

#define DecryptInit()  \
        {              \
          si = 0;      \
          x1a2 = 0;    \
        }

void exchange(unsigned int *a, unsigned int *b)
{
  unsigned int tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}

#if (Algorithm == PC1_128)
void PC1assemble128()
{
  unsigned char i;

  inter = 0;
  x1a0[0] = 0;
  for(i = 0; i < 8; i++)
  {
    x1a0[i + 1] = x1a0[i] ^ ((DecryptKey[i * 2] * 256) + DecryptKey[i * 2 + 1]);

    dx = x1a2 + i;
    ax = x1a0[i + 1];
    cx = 0x015A;
    bx = 0x4E35;

    exchange(&ax, &si);
    exchange(&ax, &dx);

    if (ax != 0)
    {
      ax = ax * bx;
    }

    exchange(&ax, &cx);

    if (ax != 0)
    {
      ax = ax * si;
      cx = ax + cx;
    }

    exchange(&ax, &si);

    ax = ax * bx;
    dx = cx + dx;
    ax = ax + 1;

    x1a2 = dx;
    x1a0[i + 1] = ax;

    inter = inter ^ (ax ^ dx);
  }
}

//decrypt a byte
unsigned char PC1Dec128Byte(unsigned char c)
{
  unsigned char i;

  PC1assemble128();
  cfc = inter >> 8;
  cfd = inter & 255;
  c = c ^ (cfc ^ cfd);

  for(i = 0; i < 16; i++)
    DecryptKey[i] = DecryptKey[i] ^ c;

  return c;
}

#endif //PC1_128

#if (Algorithm == PC1_256)
void PC1assemble256()
{
  unsigned char i;

  inter = 0;
  x1a0[0] = 0;
  for(i = 0; i < 16; i++)
  {
    x1a0[i + 1] = x1a0[i] ^ ((DecryptKey[i * 2] * 256) + DecryptKey[i * 2 + 1]);

    dx = x1a2 + i;
    ax = x1a0[i + 1];
    cx = 0x015A;
    bx = 0x4E35;

    exchange(&ax, &si);
    exchange(&ax, &dx);

    if (ax != 0)
    {
      ax = ax * bx;
    }

    exchange(&ax, &cx);

    if (ax != 0)
    {
      ax = ax * si;
      cx = ax + cx;
    }

    exchange(&ax, &si);

    ax = ax * bx;
    dx = cx + dx;
    ax = ax + 1;

    x1a2 = dx;
    x1a0[i + 1] = ax;

    inter = inter ^ (ax ^ dx);
  }
}

//decrypt a byte
unsigned char PC1Dec256Byte(unsigned char c)
{
  unsigned char i;

  PC1assemble256();
  cfc = inter >> 8;
  cfd = inter & 255;
  c = c ^ (cfc ^ cfd);

  for(i = 0; i < 32; i++)
    DecryptKey[i] = DecryptKey[i] ^ c;

  return c;
}

#endif //PC1_256

//Decrypt a block
void DecryptBlock(unsigned char *buf, unsigned int nSize)
{
  unsigned int n;

  for( n = nSize; n > 0; n--)
  {
#if (Algorithm == PC1_128)
    *buf = PC1Dec128Byte(*buf);
#else
    *buf = PC1Dec256Byte(*buf);
#endif
    buf++;
  }
}

//Destory key
void DestroyKey()
{
  unsigned char i;

#if (Algorithm == PC1_128)
  for(i = 0; i < 16; i++)
#else
  for(i = 0; i < 32; i++)
#endif
  {
    DecryptKey[i] = 0;
  }
}

//end of file
