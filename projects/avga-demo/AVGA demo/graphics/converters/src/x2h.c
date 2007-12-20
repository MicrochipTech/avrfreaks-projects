///////
//
//desc: Converts given file to a header file 1:1.
//
///////

#include <stdio.h>


int outbyte(FILE *f2, unsigned char c)
{
  static unsigned char charcnt=0;

  fprintf(f2, "0x%02X, ", c);
  if(charcnt++ > 10)
  {
   fprintf(f2,"\\\n");
   charcnt=0;
  }
}


int main( int argc, char *argv[] )
{
 if(argc != 2)
 {
 	printf("Usage: x2h.exe  [input file name]");
 	return 1;
 }

 FILE * f_in = fopen(argv[1], "rb");
 if(!f_in)
 {
 	printf("Cannot open %s!",argv[1]);
 	return 1;
 }

 char outname[128];
 strcpy(outname,argv[1]);
 strcat(outname,".h");
 FILE  *f_out = fopen(outname, "w");
 if(!f_out)
 {
 	printf("Cannot create %s!", outname);
 	return 1;
 }

 fprintf(f_out, "#define %s { \\\n", strupr(argv[1]));

 int i;
 while((i=getc(f_in)) != EOF)
     outbyte(f_out,  i);

 fprintf(f_out,"  } \n");
 printf("Done.");

 fclose(f_in);
 fclose(f_out);
 
 return 0;
}





