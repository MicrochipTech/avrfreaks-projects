#include "modbus.h"

/************************************************************************************
  Mb_test_crc : check the crc of a packet
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
1 = crc fealure
0 = crc ok
************************************************************************************/
uint Mb_test_crc(uchar *trame,uchar n)
{
uint crc;
uchar i,j;
 crc=0xffff;
 for (i=0;i<n;i++)
 {
  crc^=trame[i];
  for (j=0;j<8;j++)
  {
   if (crc & 1) {
              crc >>= 1;
              crc ^= 0xA001;
           }
           else
              crc >>= 1;
  }
 }
 if ((trame[n+1]!=(crc>>8)) || (trame[n]!=(crc&255)))
      return 1;
   else
      return 0;
}

/************************************************************************************
  Mb_calcul_crc : compute the crc of a packet and put it at the end
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
crc
************************************************************************************/
uint Mb_calcul_crc(uchar *trame,uchar n)
{
uint crc;
uchar i,j;

 crc=0xffff;
    for (i=0;i<n;i++)
    {
        crc^=trame[i];
        for (j=0;j<8;j++)
        {
            if (crc & 1) {
              crc >>= 1;
              crc ^= 0xA001;
           }
           else
              crc >>= 1;
        }
    }
 trame[n+1]=crc>>8;
 trame[n]=crc&255;
 return crc;
}
