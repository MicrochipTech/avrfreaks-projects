#include <tools.h>

void pause (int ms) {
 int i,j,k,l;
 for (l=0; l<ms*5;l++) {
  for (i=0; i<199; i++)
   for(j=0; j<255;j++) 
    k++;              
 }     
};


char *inttostr (const int val) {
 char *temp;
 temp="00000";
 temp[0] = ('0'+(val / 10000));
 temp[1] = ('0'+(val % 10000 / 1000));
 temp[2] = ('0'+(val % 1000 / 100));
 temp[3] = ('0'+(val % 100 / 10));
 temp[4] = ('0'+(val % 10));
 if (temp[0]=='0') temp[0]=' '; else return temp;
 if (temp[1]=='0') temp[1]=' '; else return temp;
 if (temp[2]=='0') temp[2]=' '; else return temp;
 if (temp[3]=='0') temp[3]=' '; else return temp;
 return temp;
};

