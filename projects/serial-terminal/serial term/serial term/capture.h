#define buffersize 7  
#define backlight PORTD.5                                                                                             

char s;                        
int  istore,j,b,i,x,state,jstore; 
unsigned char line[buffersize][21];  //holds 21 chars + terminator


enum  {backlighttoggle,crlf,bla,bkspce,line0,ignor,scrollup,scrolldown};
int lineflag;
bit wasscroll;