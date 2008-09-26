#include <string.h>
#include <avr/pgmspace.h>

#include "aesglobal.h"
#include "aesflash.h"
#include "bootldr.h"

#if KEY_COUNT > 0

#include "aeskeys.h"

#define BPOLY 0x1b                                  //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16                                //!< Block size in number of bytes.

#if KEY_COUNT == 1
#define KEYBITS 128                                 //!< Use AES128.
#elif KEY_COUNT == 2
#define KEYBITS 192                                 //!< Use AES196.
#elif KEY_COUNT == 3
#define KEYBITS 256                                 //!< Use AES256.
#else
#error Use 1, 2 or 3 keys!
#endif

#if KEYBITS == 128
#define ROUNDS 10                                   //!< Number of rounds.
#define KEYLENGTH 16                                //!< Key length in number of bytes.
#elif KEYBITS == 192
#define ROUNDS 12                                   //!< Number of rounds.
#define KEYLENGTH 24                                //!< // Key length in number of bytes.
#elif KEYBITS == 256
#define ROUNDS 14                                   //!< Number of rounds.
#define KEYLENGTH 32                                //!< Key length in number of bytes.
#else
#error Key must be 128, 192 or 256 bits!
#endif

#define EXPANDED_KEY_SIZE (BLOCKSIZE * (ROUNDS + 1))//!< 176, 208 or 240 bytes.

static uint8_t block1[256];                         //!< Workspace 1.
static uint8_t block2[256];                         //!< Worksapce 2.


register uint8_t *powTbl      asm("r10");           //!< Final location of exponentiation lookup table.
register uint8_t *logTbl      asm("r8");            //!< Final location of logarithm lookup table.
register uint8_t *sBox        asm("r6");            //!< Final location of s-box.
register uint8_t *sBoxInv     asm("r4");            //!< Final location of inverse s-box.
register uint8_t *expandedKey asm("r2");            //!< Final location of expanded key.



void CalcPowLog(uint8_t *powTbll, uint8_t *logTbll)
{
    uint8_t i = 0;
    uint8_t t = 1;

    do
    {
        // Use 0x03 as root for exponentiation and logarithms.
        powTbll[i] = t;
        logTbll[t] = i;
        i++;

        // Muliply t by 3 in GF(2^8).
        t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
    }
    while (t != 1);                                 // Cyclic properties ensure that i < 255.

    powTbll[255] = powTbll[0];                      // 255 = '-0', 254 = -1, etc.
}



void CalcSBox(uint8_t *sBox)
{

    uint8_t i = 0;
    uint8_t temp = 0;
    uint8_t rot, result;

    // Fill all entries of sBox[].

    do
    {
        // Inverse in GF(2^8).
        if (i != 0)
        {
            // *MH added cast*
            temp = powTbl[(uint8_t)(255 - logTbl[i])];
        }

        // Affine transformation in GF(2).
        result = temp ^ 0x63;                       // Start with adding a vector in GF(2).

        for (rot = 0; rot < 4; rot++)
        {
            // Rotate left.
            temp = (temp << 1) | (temp >> 7);

            // Add rotated byte in GF(2).
            result ^= temp;
        }

        // Put result in table.
        *sBox++ = result;
    }
    while (++i != 0);
}



void CalcSBoxInv(uint8_t *sBox, uint8_t *sBoxInv)
{
    uint8_t i = 0;
    uint8_t j = 0;

    // Iterate through all elements in sBoxInv using  i.
    do
    {
        // Search through sBox using j.
        do
        {
            // Check if current j is the inverse of current i.
            if (sBox[j] == i)
            {
                // If so, set sBoxInc and indicate search finished.
                sBoxInv[i] = j;
                j = 255;
            }
        }
        while (++j != 0);
    }
    while (++i != 0);
}



void CycleLeft(uint8_t *row)
{
    // Cycle 4 bytes in an array left once.
    uint8_t temp = row[0];
    row[0] = row[1];
    row[1] = row[2];
    row[2] = row[3];
    row[3] = temp;
}



void InvMixColumn(uint8_t *column)
{
    uint8_t rr[4];

    rr[0] = column[1] ^ column[2] ^ column[3];
    rr[1] = column[0] ^ column[2] ^ column[3];
    rr[2] = column[0] ^ column[1] ^ column[3];
    rr[3] = column[0] ^ column[1] ^ column[2];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

    rr[0] ^= column[0] ^ column[1];
    rr[1] ^= column[1] ^ column[2];
    rr[2] ^= column[2] ^ column[3];
    rr[3] ^= column[0] ^ column[3];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

    rr[0] ^= column[0] ^ column[2];
    rr[1] ^= column[1] ^ column[3];
    rr[2] ^= column[0] ^ column[2];
    rr[3] ^= column[1] ^ column[3];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

    column[0] ^= column[1] ^ column[2] ^ column[3];
    rr[0] ^= column[0];
    rr[1] ^= column[0];
    rr[2] ^= column[0];
    rr[3] ^= column[0];

    column[0] = rr[0];
    column[1] = rr[1];
    column[2] = rr[2];
    column[3] = rr[3];
}



void SubBytes(uint8_t *bytes, uint8_t count)
{
    do
    {
        // Substitute every byte in state.
        *bytes = sBox[*bytes];
        bytes++;
    }
    while (--count);
}



void InvSubBytesAndXOR(uint8_t *bytes, uint8_t *key, uint8_t count)
{
    do
    {
        // *bytes = sBoxInv[ *bytes ] ^ *key;       // Inverse substitute every byte in state and add key.
        *bytes = block2[*bytes] ^ *key;             // Use block2 directly. Increases speed.
        bytes++;
        key++;
    }
    while (--count);
}



void InvShiftRows(uint8_t *state)
{
    uint8_t temp;

    // Note: State is arranged column by column.

    // Cycle second row right one time.
    temp = state[1 + 3 * 4];
    state[1 + 3 * 4] = state[1 + 2 * 4];
    state[1 + 2 * 4] = state[1 + 1 * 4];
    state[1 + 1 * 4] = state[1 + 0 * 4];
    state[1 + 0 * 4] = temp;

    // Cycle third row right two times.
    temp = state[2 + 0 * 4];
    state[2 + 0 * 4] = state[2 + 2 * 4];
    state[2 + 2 * 4] = temp;
    temp = state[2 + 1 * 4];
    state[2 + 1 * 4] = state[2 + 3 * 4];
    state[2 + 3 * 4] = temp;

    // Cycle fourth row right three times, ie. left once.
    temp = state[3 + 0 * 4];
    state[3 + 0 * 4] = state[3 + 1 * 4];
    state[3 + 1 * 4] = state[3 + 2 * 4];
    state[3 + 2 * 4] = state[3 + 3 * 4];
    state[3 + 3 * 4] = temp;
}



void InvMixColumns(uint8_t *state)
{
    InvMixColumn(state + 0 * 4);
    InvMixColumn(state + 1 * 4);
    InvMixColumn(state + 2 * 4);
    InvMixColumn(state + 3 * 4);
}



void XORBytes(uint8_t *bytes1, uint8_t *bytes2, uint8_t count)
{
    do
    {
        *bytes1 ^= *bytes2;                         // Add in GF(2), ie. XOR.
        bytes1++;
        bytes2++;
    }
    while (--count);
}



void CopyBytes(uint8_t *to, uint8_t *from, uint8_t count)
{
    do
    {
        *(to++) = *(from++);
    }
    while (--count);
}



void KeyExpansion(uint8_t *expKey)
{
    uint8_t i;
    uint8_t temp[4];
    uint8_t Rcon[4] =  { 0x01, 0x00, 0x00, 0x00 };                             // Round constant.
    
    // Copy key to start of expanded key.
#if defined(RAMPZ)
    memcpy_PF(expKey, pflash_byte3(kTable), kTable, KEYLENGTH);
#else
    memcpy_PN(expKey, kTable, KEYLENGTH);
#endif
    expKey += KEYLENGTH;

    // Prepare last 4 bytes of key in temp.
    memcpy(temp, (expKey-4), 4);

    // Expand key.
    i = KEYLENGTH;
    while (i < BLOCKSIZE * (ROUNDS + 1))
    {
        // Are we at the start of a multiple of the key size?
        if ((i % KEYLENGTH) == 0)
        {
            CycleLeft(temp);                        // Cycle left once.
            SubBytes(temp, 4);                      // Substitute each byte.
            XORBytes(temp, Rcon, 4);                // Add constant in GF(2).
            *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
        }

        // Keysize larger than 24 bytes, ie. larger that 192 bits?
#if KEYLENGTH > 24
        // Are we right past a block size?
        else if ((i % KEYLENGTH) == BLOCKSIZE)
        {
            SubBytes(temp, 4);                      // Substitute each byte.
        }
#endif

        // Add bytes in GF(2) one KEYLENGTH away.
        XORBytes(temp, expKey - KEYLENGTH, 4);

        // Copy result to current 4 bytes.
        memcpy(expKey, temp, 4);
        expKey += 4;
        i += 4;                                     // Next 4 bytes.
    }
}



void InvCipher(uint8_t *block, uint8_t *expKey)
{
    uint8_t round = ROUNDS - 1;
    expKey += BLOCKSIZE * ROUNDS;

    XORBytes(block, expKey, 16);
    expKey -= BLOCKSIZE;

    do
    {
        InvShiftRows(block);
        InvSubBytesAndXOR(block, expKey, 16);
        expKey -= BLOCKSIZE;
        InvMixColumns(block);
    }
    while (--round);

    InvShiftRows(block);
    InvSubBytesAndXOR(block, expKey, 16);
}



void aesInit(unsigned char *tempbuffer)
{
    powTbl = block1;
    logTbl = block2;
    CalcPowLog(powTbl, logTbl);

    sBox = tempbuffer;
    CalcSBox(sBox);

    expandedKey = block1;
    KeyExpansion(expandedKey);

    sBoxInv = block2;                               // Must be block2.
    CalcSBoxInv(sBox, sBoxInv);
}



void aesDecrypt(unsigned char *buffer, unsigned char *chainBlock)
{
    uint8_t temp[BLOCKSIZE];

    CopyBytes(temp, buffer, BLOCKSIZE);
    InvCipher(buffer, expandedKey);
    XORBytes(buffer, chainBlock, BLOCKSIZE);
    CopyBytes(chainBlock, temp, BLOCKSIZE);
}

#endif
