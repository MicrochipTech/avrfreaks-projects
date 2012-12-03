#include <avr/io.h>
#include <string.h>
#include "PDU_Code_Decode.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Conversion To char From GSM 7 in the same buffer
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
void GSM_7ToChar( unsigned char *Src )
{
// Special char
// ~  (asc 126) --> SP (asc 32, gsm 0x0020)
// ^  (asc  94) --> SP (asc 32, gsm 0x0020)
// {  (asc 123) --> (  (asc 40, gsm 0x0028)
// }  (asc 125) --> )  (asc 41, gsm 0x0029)
// \  (asc  92) --> /  (asc 47, gsm 0x002F)
// [  (asc  91) --> (  (asc 40, gsm 0x0028)
// ]  (asc  93) --> )  (asc 41, gsm 0x0029)
// `  (asc  96) --> '  (asc 39, gsm 0x0027)
// |  (asc 124) --> inverted ! (asc 161, gsm 0x0040)

// different chars (all other the same)
//    gsm    char
//   0x00 - 0x0040     COMMERCIAL AT
//   0x01 - 0x00A3     POUND SIGN
//   0x02 - 0x0024     DOLLAR SIGN
//   0x03 - 0x00A5     YEN SIGN
//   0x04 - 0x00E8     LATIN SMALL LETTER E WITH GRAVE
//   0x05 - 0x00E9     LATIN SMALL LETTER E WITH ACUTE
//   0x06 - 0x00F9     LATIN SMALL LETTER U WITH GRAVE
//   0x07 - 0x00EC     LATIN SMALL LETTER I WITH GRAVE
//   0x08 - 0x00F2     LATIN SMALL LETTER O WITH GRAVE
//   0x09 - 0x00E7     LATIN SMALL LETTER C WITH CEDILLA
//   0x0B - 0x00D8     LATIN CAPITAL LETTER O WITH STROKE
//   0x0C - 0x00F8     LATIN SMALL LETTER O WITH STROKE
//   0x0E - 0x00C5     LATIN CAPITAL LETTER A WITH RING ABOVE
//   0x0F - 0x00E5     LATIN SMALL LETTER A WITH RING ABOVE
//   0x10 - 0x0394     GREEK CAPITAL LETTER DELTA
//   0x11 - 0x005F     LOW LINE
//   0x12 - 0x03A6     GREEK CAPITAL LETTER PHI
//   0x13 - 0x0393     GREEK CAPITAL LETTER GAMMA
//   0x14 - 0x039B     GREEK CAPITAL LETTER LAMDA
//   0x15 - 0x03A9     GREEK CAPITAL LETTER OMEGA
//   0x16 - 0x03A0     GREEK CAPITAL LETTER PI
//   0x17 - 0x03A8     GREEK CAPITAL LETTER PSI
//   0x18 - 0x03A3     GREEK CAPITAL LETTER SIGMA
//   0x19 - 0x0398     GREEK CAPITAL LETTER THETA
//   0x1A - 0x039E     GREEK CAPITAL LETTER XI
//   0x1B - 0x00A0     ESCAPE TO EXTENSION TABLE
//   0x1C - 0x00C6     LATIN CAPITAL LETTER AE
//   0x1D - 0x00E6     LATIN SMALL LETTER AE
//   0x1E - 0x00DF     LATIN SMALL LETTER SHARP S (German)
//   0x1F - 0x00C9     LATIN CAPITAL LETTER E WITH ACUTE
//   0x24 - 0x00A4     CURRENCY SIGN
//   0x40 - 0x00A1     INVERTED EXCLAMATION MARK
//   0x5B - 0x00C4     LATIN CAPITAL LETTER A WITH DIAERESIS
//   0x5C - 0x00D6     LATIN CAPITAL LETTER O WITH DIAERESIS
//   0x5D - 0x00D1     LATIN CAPITAL LETTER N WITH TILDE
//   0x5E - 0x00DC     LATIN CAPITAL LETTER U WITH DIAERESIS
//   0x5F - 0x00A7     SECTION SIGN
//   0x60 - 0x00BF     INVERTED QUESTION MARK
//   0x7B - 0x00E4     LATIN SMALL LETTER A WITH DIAERESIS
//   0x7C - 0x00F6     LATIN SMALL LETTER O WITH DIAERESIS
//   0x7D - 0x00F1     LATIN SMALL LETTER N WITH TILDE
//   0x7E - 0x00FC     LATIN SMALL LETTER U WITH DIAERESIS
//   0x7F - 0x00E0     LATIN SMALL LETTER A WITH GRAVE

   while(*Src){
     switch(*Src){
       case 0x27: *Src = 96; break;
       case 0x40: *Src = 124; break;
       case 0x00: *Src = 0x0040; break;
       case 0x01: *Src = 0x00A3; break;
       case 0x02: *Src = 0x0024; break;
       case 0x03: *Src = 0x00A5; break;
       case 0x04: *Src = 0x00E8; break;
       case 0x05: *Src = 0x00E9; break;
       case 0x06: *Src = 0x00F9; break;
       case 0x07: *Src = 0x00EC; break;
       case 0x08: *Src = 0x00F2; break;
       case 0x09: *Src = 0x00E7; break;
       case 0x0B: *Src = 0x00D8; break;
       case 0x0C: *Src = 0x00F8; break;
       case 0x0E: *Src = 0x00C5; break;
       case 0x0F: *Src = 0x00E5; break;
//     case 0x10: *Src = 0x0394; break;
       case 0x11: *Src = 0x005F; break;
//     case 0x12: *Src = 0x03A6; break;
//     case 0x13: *Src = 0x0393; break;
//     case 0x14: *Src = 0x039B; break;
//     case 0x15: *Src = 0x03A9; break;
//     case 0x16: *Src = 0x03A0; break;
//     case 0x17: *Src = 0x03A8; break;
//     case 0x18: *Src = 0x03A3; break;
//     case 0x19: *Src = 0x0398; break;
//     case 0x1A: *Src = 0x039E; break;
       case 0x1B: *Src = 0x00A0; break;
       case 0x1C: *Src = 0x00C6; break;
       case 0x1D: *Src = 0x00E6; break;
       case 0x1E: *Src = 0x00DF; break;
       case 0x1F: *Src = 0x00C9; break;
       case 0x24: *Src = 0x00A4; break;
       case 0x5B: *Src = 0x00C4; break;
       case 0x5C: *Src = 0x00D6; break;
       case 0x5D: *Src = 0x00D1; break;
       case 0x5E: *Src = 0x00DC; break;
       case 0x5F: *Src = 0x00A7; break;
       case 0x60: *Src = 0x00BF; break;
       case 0x7B: *Src = 0x00E4; break;
       case 0x7C: *Src = 0x00F6; break;
       case 0x7D: *Src = 0x00F1; break;
       case 0x7E: *Src = 0x00FC; break;
       case 0x7F: *Src = 0x00E0; break;

       default: break;
     }
   Src++;
   }
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Conversion from char to GSM 7 in the same buffer
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
void CharToGSM_7( unsigned char *Src )
{
// Special char
// ~  (asc 126) --> SP (asc 32, gsm 0x0020)
// ^  (asc  94) --> SP (asc 32, gsm 0x0020)
// {  (asc 123) --> (  (asc 40, gsm 0x0028)
// }  (asc 125) --> )  (asc 41, gsm 0x0029)
// \  (asc  92) --> /  (asc 47, gsm 0x002F)
// [  (asc  91) --> (  (asc 40, gsm 0x0028)
// ]  (asc  93) --> )  (asc 41, gsm 0x0029)
// `  (asc  96) --> '  (asc 39, gsm 0x0027)
// |  (asc 124) --> inverted ! (asc 161, gsm 0x0040)

// different chars (all other the same)
//    gsm    char
//   0x00 - 0x0040     COMMERCIAL AT
//   0x01 - 0x00A3     POUND SIGN
//   0x02 - 0x0024     DOLLAR SIGN
//   0x03 - 0x00A5     YEN SIGN
//   0x04 - 0x00E8     LATIN SMALL LETTER E WITH GRAVE
//   0x05 - 0x00E9     LATIN SMALL LETTER E WITH ACUTE
//   0x06 - 0x00F9     LATIN SMALL LETTER U WITH GRAVE
//   0x07 - 0x00EC     LATIN SMALL LETTER I WITH GRAVE
//   0x08 - 0x00F2     LATIN SMALL LETTER O WITH GRAVE
//   0x09 - 0x00E7     LATIN SMALL LETTER C WITH CEDILLA
//   0x0B - 0x00D8     LATIN CAPITAL LETTER O WITH STROKE
//   0x0C - 0x00F8     LATIN SMALL LETTER O WITH STROKE
//   0x0E - 0x00C5     LATIN CAPITAL LETTER A WITH RING ABOVE
//   0x0F - 0x00E5     LATIN SMALL LETTER A WITH RING ABOVE
//   0x10 - 0x0394     GREEK CAPITAL LETTER DELTA
//   0x11 - 0x005F     LOW LINE
//   0x12 - 0x03A6     GREEK CAPITAL LETTER PHI
//   0x13 - 0x0393     GREEK CAPITAL LETTER GAMMA
//   0x14 - 0x039B     GREEK CAPITAL LETTER LAMDA
//   0x15 - 0x03A9     GREEK CAPITAL LETTER OMEGA
//   0x16 - 0x03A0     GREEK CAPITAL LETTER PI
//   0x17 - 0x03A8     GREEK CAPITAL LETTER PSI
//   0x18 - 0x03A3     GREEK CAPITAL LETTER SIGMA
//   0x19 - 0x0398     GREEK CAPITAL LETTER THETA
//   0x1A - 0x039E     GREEK CAPITAL LETTER XI
//   0x1B - 0x00A0     ESCAPE TO EXTENSION TABLE
//   0x1C - 0x00C6     LATIN CAPITAL LETTER AE
//   0x1D - 0x00E6     LATIN SMALL LETTER AE
//   0x1E - 0x00DF     LATIN SMALL LETTER SHARP S (German)
//   0x1F - 0x00C9     LATIN CAPITAL LETTER E WITH ACUTE
//   0x24 - 0x00A4     CURRENCY SIGN
//   0x40 - 0x00A1     INVERTED EXCLAMATION MARK
//   0x5B - 0x00C4     LATIN CAPITAL LETTER A WITH DIAERESIS
//   0x5C - 0x00D6     LATIN CAPITAL LETTER O WITH DIAERESIS
//   0x5D - 0x00D1     LATIN CAPITAL LETTER N WITH TILDE
//   0x5E - 0x00DC     LATIN CAPITAL LETTER U WITH DIAERESIS
//   0x5F - 0x00A7     SECTION SIGN
//   0x60 - 0x00BF     INVERTED QUESTION MARK
//   0x7B - 0x00E4     LATIN SMALL LETTER A WITH DIAERESIS
//   0x7C - 0x00F6     LATIN SMALL LETTER O WITH DIAERESIS
//   0x7D - 0x00F1     LATIN SMALL LETTER N WITH TILDE
//   0x7E - 0x00FC     LATIN SMALL LETTER U WITH DIAERESIS
//   0x7F - 0x00E0     LATIN SMALL LETTER A WITH GRAVE

   while(*Src){
     switch(*Src){
       case 94    :
       case 126   : *Src = 0x20; break;
       case 91    :
       case 123   : *Src = 0x28; break;
       case 93    :
       case 125   : *Src = 0x29; break;
       case 92    : *Src = 0x2F; break;
       case 96    : *Src = 0x27; break;
       case 124   : *Src = 0x40; break;
       case 0x0040: *Src = 0x00; break;
       case 0x00A3: *Src = 0x01; break;
       case 0x0024: *Src = 0x02; break;
       case 0x00A5: *Src = 0x03; break;
       case 0x00E8: *Src = 0x04; break;
       case 0x00E9: *Src = 0x05; break;
       case 0x00F9: *Src = 0x06; break;
       case 0x00EC: *Src = 0x07; break;
       case 0x00F2: *Src = 0x08; break;
       case 0x00E7: *Src = 0x09; break;
       case 0x00D8: *Src = 0x0B; break;
       case 0x00F8: *Src = 0x0C; break;
       case 0x00C5: *Src = 0x0E; break;
       case 0x00E5: *Src = 0x0F; break;
//       case 0x0394: *Src = 0x10; break;
       case 0x005F: *Src = 0x11; break;
//       case 0x03A6: *Src = 0x12; break;
//       case 0x0393: *Src = 0x13; break;
//       case 0x039B: *Src = 0x14; break;
//       case 0x03A9: *Src = 0x15; break;
//       case 0x03A0: *Src = 0x16; break;
//       case 0x03A8: *Src = 0x17; break;
//       case 0x03A3: *Src = 0x18; break;
//       case 0x0398: *Src = 0x19; break;
//       case 0x039E: *Src = 0x1A; break;
       case 0x00A0: *Src = 0x1B; break;
       case 0x00C6: *Src = 0x1C; break;
       case 0x00E6: *Src = 0x1D; break;
       case 0x00DF: *Src = 0x1E; break;
       case 0x00C9: *Src = 0x1F; break;
       case 0x00A4: *Src = 0x24; break;
       case 0x00A1: *Src = 0x40; break;
       case 0x00C4: *Src = 0x5B; break;
       case 0x00D6: *Src = 0x5C; break;
       case 0x00D1: *Src = 0x5D; break;
       case 0x00DC: *Src = 0x5E; break;
       case 0x00A7: *Src = 0x5F; break;
       case 0x00BF: *Src = 0x60; break;
       case 0x00E4: *Src = 0x7B; break;
       case 0x00F6: *Src = 0x7C; break;
       case 0x00F1: *Src = 0x7D; break;
       case 0x00FC: *Src = 0x7E; break;
       case 0x00E0: *Src = 0x7F; break;

       default: break;
     }
   Src++;
   }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*
   GSM-7 packing routing.
   Written by Jeroen @ Mobile Tidings (http://mobiletidings.com)
*/

int                                /* Returns -1 for success, 0 for failure */

SMS_GSMEncode(

   int             inSize,         /* Number of GSM-7 characters */
   char*           inText,         /* Pointer to the GSM-7 characters. Note: I could
                                      not have used a 0-terminated string, since 0
                                      represents '@' in the GSM-7 character set */
   int             paddingBits,    /* If you use a UDH, you may have to add padding
                                      bits to properly align the GSM-7 septets */
   int             outSize,        /* The number of octets we have available to write */
   unsigned char*  outBuff,        /* A pointer to the available octets */
   int            *outUsed         /* Keeps track of howmany octets actually were used */
)

{
   int             bits = 0;
   int             i;
   unsigned char   octet;

   *outUsed = 0;
   if( paddingBits )
   {
      bits = 7 - paddingBits;
      *outBuff++ = inText[0] << (7 - bits);
      (*outUsed) ++;
      bits++;
   }

   for( i = 0; i < inSize; i++ )
   {
      if( bits == 7 )
      {
         bits = 0;
         continue;
      }
      if( *outUsed == outSize )
         return 0; /* buffer overflow */
      octet = (inText[i] & 0x7f) >> bits;
      if( i < inSize - 1 )
         octet |= inText[i + 1] << (7 - bits);
      *outBuff++ = octet;
      (*outUsed)++;
      bits++;
   }
   return -1; /* ok */
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*! \brief Convert hex to int
 *
 *  This function will take a hex in char format and return int value
 *
 *  \param    hex     Hex value to convert
 *
 *  \retval   int     Integer representation of hex-value
 */
int htoi( unsigned char hex )
{

    if( ( hex >= 'A' ) && ( hex <= 'F' ) )    //Test if hex is A-->F?
    {
        return hex - 'A' + 10;
    }
    else                                      //Must have something else then:0-->9
    {
        return hex - '0';
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*! \brief Converting int [0->255] to hex value
 *
 *  \param    n     Integer value to be decoded.
 *  \param    ret   Return array.
 *
 */
void itoh( int n, unsigned char *ret )
{
  unsigned char   hex_lookup[] = "0123456789ABCDEF"  ;  //!< Lookup table -> int 2 hex

  ret[ 0 ] = hex_lookup[ ( n >> 4 ) & 0x0F ];
  ret[ 1 ] = hex_lookup[ ( n & 0x0F ) ];
  ret[ 2 ] = '\0';
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*! \brief Converting buf (rpresented by int8_t )to hex
 *
 *  \param    src   source char to be converted
 *  \param    dst   Return array converted.
 *  \param    count number of char to convert.
 *
 */
void strtoh( int8_t *src, unsigned char *dst, int count  )
{
  // REMEMBER: strlen(dst) = 2*count;
  
  unsigned char   hex_lookup[] = "0123456789ABCDEF"  ;  //!< Lookup table -> int 2 hex
  while(count--){
    *dst++ = hex_lookup[ ( *src >> 4 ) & 0x0F ];
    *dst++ = hex_lookup[ ( *src++ & 0x0F ) ];
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*! \brief Converting hexbuf to int (rpresented by int8_t )
 *
 *  \param    src   source char to be converted
 *   destination is the same buffer:
     src[0] src[1] HEX  => src[0] int8;
 *  \param    count Number of char of src (divisible for two).
 *
 */
void htostrint( unsigned char *src, int count)
{
   int i = 0, j = 0;
   while(count){
     src[i++] = 16 * htoi( src[j] ) + htoi( src[j+1] );
     count -=2;
     j += 2;
   }
   src[i] = 0;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*! \brief SMS_GSMdecode will decode a message in PDU format into a readable string
 *
 *  \param    *compressed PDU encoded text
 *  \param    *decompressed Pointer to return array
 *  \param    *offset  padding bit
 *
 *  \retval   iii Number of decoded chars
 */
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
int SMS_GSMDecode( unsigned char *input, unsigned char *decoded, int offset )
{

  int at2_rmask[8] = { 0, 1, 3, 7, 15, 31, 63, 127 };
  int at2_lmask[8] = { 0, 128, 192, 224, 240, 248, 252, 254 };
  unsigned char septet, octet, prevoctet;
  unsigned char *ip;
  int i;
  int r = 1;
  int c = 7;
  int pos = 0;
  int j = 0;

 int Length = strlen((char*)input);

 htostrint( input, Length );
 Length /= 2; //Septets chars

 if (offset > 0) {
    for (i = 0, ip = input; i < Length; i++) {

    if (i == Length - 1)

       *ip >>= offset;

    else
       *ip = (*ip >> offset) | (*(ip + 1) << (8 - offset));

    ip++;
    }
 }

 octet = input[pos];
 prevoctet = 0;
 for(i=0; i<Length; i++) {
   septet = ((octet & at2_rmask[c]) << (r-1)) + prevoctet;
   decoded[j++] = septet;
   prevoctet = (octet & at2_lmask[r]) >> c;

                /* When r=7 we have a full character in prevoctet*/
   if( r==7 ){
     decoded[j++] = prevoctet;
     prevoctet = 0;
     }

   r = (r>6)? 1 : r+1;
   c = (c<2)? 7 : c-1;

   pos++;
   octet = input[pos];
   }
  
  GSM_7ToChar(decoded);
  return j;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------

