
// **********************************************************************
// *                               slew.c                               *
// *                                                                    *
// *      Creates 1k table of divisors for ATMega168 (SLEW_Table)       *                                                          *
// **********************************************************************

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main () {

  int kw[8];
  int i, j, jj,  s;
  int A=1024, N=1024, DIVMIN=15, k1, k2;
long sum;

  FILE *fout;
  printf("%d divisors calculated using int(%d*sqrt(%d/s)\n", N, DIVMIN, A);

// Write data in assembler form

  fout = fopen("slew_table.S","w"); sum=0;
  s = 1;
  while (s<N)
    {
    for (j=0;j<8;j++)
      {
      k1 = (int)(DIVMIN*sqrt(A/s));
      k2 = (int)(DIVMIN*sqrt(A/(s+1)));
      if (k1>255) k1 = 255; sum += k1;
      if (k2>255) k2 = 255; sum += k2;
      kw[j] = 256*k1 + k2;
      s = s + 2;
      }
    fprintf(fout,".word %d,%d,%d,%d,%d,%d,%d,%d\n",kw[0],kw[1],kw[2],kw[3],kw[4],kw[5],kw[6],kw[7]);
    }  
  printf("Max duration = %ld pulses\n", sum);
  return(0);
}

