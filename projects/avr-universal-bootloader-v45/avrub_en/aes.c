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
  File:          aes.c
                 aes Cipher Algorithm ( Rijndael  ) 128/256 bits key
                 base on AVR231's iar aes routine
                 Optimized for avr 8 bit mcu and GCC compiler
  Version:       1.0

  Compiler:      WinAVR 20071221 + AVR Studio 4.14.589

  Author:        Shaoziyang
                 Shaoziyang@gmail.com
                 http://avrubd.googlepages.com
                 http://groups.google.com/group/avrub?hl=en

  Date:          2008.6

  See readme.htm to get more information.

*/


//use OPT_MODE to optimize code size or run speed
//OPT_SPEED may increase decrypt speed over 20%
#define OPT_SIZE  1
#define OPT_SPEED 2

#define OPT_MODE  OPT_SIZE

#define BPOLY 0x1b          //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16        //!< Block size in number of bytes.

#ifndef AES_128
#define AES_128   2
#endif

#ifndef AES_256
#define AES_256   3
#endif

#ifndef Algorithm
#define Algorithm AES_128
#endif

#if (Algorithm == AES_128)
  #define ROUNDS 10         //!< Number of rounds.
  #define KEYLENGTH 16      //!< Key length in number of bytes.
#elif (Algorithm == AES_256)
  #define ROUNDS 14         //!< Number of rounds.
  #define KEYLENGTH 32      //!< Key length in number of bytes.
#else
#error "Unknow encrypt algorithm!"
#endif


unsigned char block1[256];  //!< Workspace 1.
unsigned char block2[256];  //!< Worksapce 2.
unsigned char tempbuf[256]; //temp buffer
unsigned char chain[16];    //Cipher Block chain

unsigned char *powTbl;      //!< Final location of exponentiation lookup table.
unsigned char *logTbl;      //!< Final location of logarithm lookup table.
unsigned char *sBox;        //!< Final location of s-box.
unsigned char *sBoxInv;     //!< Final location of inverse s-box.
unsigned char *expandedKey; //!< Final location of expanded key.


void CalcPowLog(unsigned char *powTbl, unsigned char *logTbl)
{
  unsigned char i = 0;
  unsigned char t = 1;

  do {
    // Use 0x03 as root for exponentiation and logarithms.
    powTbl[i] = t;
    logTbl[t] = i;
    i++;

    // Muliply t by 3 in GF(2^8).
    t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
  }while( t != 1 ); // Cyclic properties ensure that i < 255.

  powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}

void CalcSBox( unsigned char * sBox )
{
  unsigned char i, rot;
  unsigned char temp;
  unsigned char result;

  // Fill all entries of sBox[].
  i = 0;
  do {
    //Inverse in GF(2^8).
    if( i > 0 ) 
    {
      temp = powTbl[ 255 - logTbl[i] ];
    } 
    else 
    {
      temp = 0;
    }

    // Affine transformation in GF(2).
    result = temp ^ 0x63; // Start with adding a vector in GF(2).
    for( rot = 4; rot > 0; rot-- )
    {
      // Rotate left.
      temp = (temp<<1) | (temp>>7);

      // Add rotated byte in GF(2).
      result ^= temp;
    }

    // Put result in table.
    sBox[i] = result;
  } while( ++i != 0 );
}

void CalcSBoxInv( unsigned char * sBox, unsigned char * sBoxInv )
{
  unsigned char i = 0;
  unsigned char j = 0;

  // Iterate through all elements in sBoxInv using  i.
  do {
    // Search through sBox using j.
    do {
      // Check if current j is the inverse of current i.
      if( sBox[j] == i )
      {
        // If so, set sBoxInc and indicate search finished.
        sBoxInv[i] = j;
        j = 255;
      }
    } while( ++j != 0 );
  } while( ++i != 0 );
}

#if OPT_MODE == OPT_SIZE
void CalcCols(unsigned char *col)
{
  unsigned char i;

  for(i = 4; i > 0; i--)
  {
    *col = (*col << 1) ^ (*col & 0x80 ? BPOLY : 0);
    col++;
  }
}
#endif

void InvMixColumn( unsigned char * column )
{
  unsigned char r[4];

  r[0] = column[1] ^ column[2] ^ column[3];
  r[1] = column[0] ^ column[2] ^ column[3];
  r[2] = column[0] ^ column[1] ^ column[3];
  r[3] = column[0] ^ column[1] ^ column[2];

#if OPT_MODE == OPT_SIZE
  CalcCols(column);
#else
  column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);
#endif

  r[0] ^= column[0] ^ column[1];
  r[1] ^= column[1] ^ column[2];
  r[2] ^= column[2] ^ column[3];
  r[3] ^= column[0] ^ column[3];

#if OPT_MODE == OPT_SIZE
  CalcCols(column);
#else
  column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);
#endif

  r[0] ^= column[0] ^ column[2];
  r[1] ^= column[1] ^ column[3];
  r[2] ^= column[0] ^ column[2];
  r[3] ^= column[1] ^ column[3];

#if OPT_MODE == OPT_SIZE
  CalcCols(column);
#else
  column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);
#endif

  column[0] ^= column[1] ^ column[2] ^ column[3];
  r[0] ^= column[0];
  r[1] ^= column[0];
  r[2] ^= column[0];
  r[3] ^= column[0];
  
  column[0] = r[0];
  column[1] = r[1];
  column[2] = r[2];
  column[3] = r[3];
}

void SubBytes( unsigned char * bytes, unsigned char count )
{
  do {
    *bytes = sBox[ *bytes ]; // Substitute every byte in state.
    bytes++;
  } while( --count );
}

void InvSubBytesAndXOR( unsigned char * bytes, unsigned char * key, unsigned char count )
{
  do {
    // *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
    *bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
    bytes++;
    key++;
  } while( --count );
}

void InvShiftRows( unsigned char * state )
{
  unsigned char temp;
  // Note: State is arranged column by column.

#if OPT_MODE == OPT_SIZE
  unsigned char i, j;

  for(i = 3; i > 0; i--)
  {
    for(j = i; j > 0; j--)
    {
      temp = state[i + 3 * 4];
      state[i + 3 * 4] = state[i + 2 * 4];
      state[i + 2 * 4] = state[i + 1 * 4];
      state[i + 1 * 4] = state[i + 0 * 4];
      state[i + 0 * 4] = temp;
    }
  }
#else
  // Cycle second row right one time.
  temp = state[ 1 + 3*4 ];
  state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
  state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
  state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
  state[ 1 + 0*4 ] = temp;

  // Cycle third row right two times.
  temp = state[ 2 + 0*4 ];
  state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
  state[ 2 + 2*4 ] = temp;
  temp = state[ 2 + 1*4 ];
  state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
  state[ 2 + 3*4 ] = temp;

  // Cycle fourth row right three times, ie. left once.
  temp = state[ 3 + 0*4 ];
  state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
  state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
  state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
  state[ 3 + 3*4 ] = temp;
#endif
}

void XORBytes( unsigned char * bytes1, unsigned char * bytes2, unsigned char count )
{
  do {
    *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
    bytes1++;
    bytes2++;
  } while( --count );
}

void CopyBytes( unsigned char * to, unsigned char * from, unsigned char count )
{
  do {
    *to = *from;
    to++;
    from++;
  } while( --count );
}

void KeyExpansion( unsigned char * expandedKey )
{
  unsigned char temp[4], t;
  unsigned char i;
  unsigned char Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.

  const unsigned char * key = DecryptKey;

  // Copy key to start of expanded key.
  CopyBytes(expandedKey, (unsigned char *)key, KEYLENGTH);
  expandedKey += KEYLENGTH;

  // Prepare last 4 bytes of key in temp.
  CopyBytes(temp, (unsigned char *)key + KEYLENGTH - 4, 4);

  // Expand key.
  i = KEYLENGTH;
  while( i < BLOCKSIZE*(ROUNDS+1) ) 
  {
    // Are we at the start of a multiple of the key size?
    if( (i % KEYLENGTH) == 0 )
    {
      // Cycle left once.
      t = temp[0];
      temp[0] = temp[1];
      temp[1] = temp[2];
      temp[2] = temp[3];
      temp[3] = t;
      SubBytes( temp, 4 ); // Substitute each byte.
      XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).
      *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
    }

    // Keysize larger than 24 bytes, ie. larger that 192 bits?
    #if KEYLENGTH > 24
    // Are we right past a block size?
    else if( (i % KEYLENGTH) == BLOCKSIZE ) {
      SubBytes( temp, 4 ); // Substitute each byte.
    }
    #endif

    // Add bytes in GF(2) one KEYLENGTH away.
    XORBytes( temp, expandedKey - KEYLENGTH, 4 );

    // Copy result to current 4 bytes.
    *(expandedKey++) = temp[0];
    *(expandedKey++) = temp[1];
    *(expandedKey++) = temp[2];
    *(expandedKey++) = temp[3];

    i += 4; // Next 4 bytes.
  }
}

void InvCipher( unsigned char * block, unsigned char * expandedKey )
{
  unsigned char i, j;
  unsigned char round = ROUNDS-1;
  expandedKey += BLOCKSIZE * ROUNDS;

  XORBytes( block, expandedKey, 16 );
  expandedKey -= BLOCKSIZE;

  do {
    InvShiftRows( block );
    InvSubBytesAndXOR( block, expandedKey, 16 );
    expandedKey -= BLOCKSIZE;
    for(i = 4, j = 0; i > 0; i--, j+=4)
      InvMixColumn( block + j );
  } while( --round );

  InvShiftRows( block );
  InvSubBytesAndXOR( block, expandedKey, 16 );
}

void aesDecInit(void)
{
  unsigned char i;
  
  for(i = 0; i < 16; i++)
    chain[i] = 0;
    
  powTbl = block1;
  logTbl = block2;
  CalcPowLog( powTbl, logTbl );

  sBox = tempbuf;
  CalcSBox( sBox );

  expandedKey = block1;
  KeyExpansion( expandedKey );

  sBoxInv = block2; // Must be block2.
  CalcSBoxInv( sBox, sBoxInv );
}

#define DecryptInit()     aesDecInit()

//Decrypt a block
void DecryptBlock(unsigned char *buf, unsigned int nSize)
{
  unsigned int n;

  for(n = nSize; n > 0; n -= 16)
  {
    CopyBytes( tempbuf, buf, BLOCKSIZE );
    InvCipher( buf, expandedKey );
    XORBytes( buf, chain, BLOCKSIZE );
    CopyBytes( chain, tempbuf, BLOCKSIZE );
    buf += 16;
  }
}

//Destory key
void DestroyKey()
{
  unsigned char i;

#if (Algorithm == AES_128)
  for(i = 16; i > 0; i--)
#else
  for(i = 32; i > 0; i--)
#endif
  {
    DecryptKey[i - 1] = 0;
  }
}

//end of file
