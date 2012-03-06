//	FLASH.C

#include<stdio.h>
#include<dos.h>
#include<string.h>
#include<stdlib.h>

#define PAGE_SIZE 128
#define START_PAGE 25
#define FILE_OFFSET (PAGE_SIZE*START_PAGE)   //read from 25th page (each page 128 byte)

extern const char* month[];

void WriteTxt(const char* infile)
{
    const struct date today;
    char outfile[12];
    int ch,count,month_init,date_init,page_count;
    FILE* fpIn;
    FILE* fpOut;

    if( (fpIn = fopen(infile,"rb")) == NULL) {
	printf("Error in opening file %s for reading..\n",infile);
	return -1;
    }

    getdate((struct date*)&today);

    sprintf(outfile,"%d%02d%02d.txt",today.da_year,today.da_mon,today.da_day);
    if( (fpOut=fopen(outfile,"wt")) == NULL) {
	printf("\nError creating output file..\n");
	return -1;
    }

    fseek(fpIn,FILE_OFFSET,SEEK_SET);
	month_init = getc(fpIn);
	date_init = getc(fpIn);

    fprintf(fpOut,"\n\t\t------ eVeryDay --------");
    fprintf(fpOut,"\n\t\t SIMPLE DATA LOGGING\n");
    fprintf(fpOut,"\n\t      Recording Date: %s %d, %d ",month[month_init-1],date_init,today.da_year);
    fprintf(fpOut,"\n\n\n\tTime\tTemperature(C)\tLDR reading");
    fprintf(fpOut,"\n\t--------------------------------");
    fprintf(fpOut,"\n\t");

    count = 0;
    page_count = START_PAGE;
    while (1) {

	ch=getc(fpIn);
	fprintf(fpOut,"%02d",ch);
	ch = getc(fpIn);
	putc(':',fpOut);
	fprintf(fpOut,"%02d",ch);
	fprintf(fpOut,"\t     ");
	//temperature
	ch = getw(fpIn);
	fprintf(fpOut,"%2.2f",(float)ch/100.0 );
	//putc('.',fpOut);
	//ch = getc(fpIn);
	//fprintf(fpOut,"%02x",ch);
	fprintf(fpOut,"\t");
	ch = getw(fpIn);
	fprintf(fpOut,"%d",ch);
	//ch = getc(fpIn);
	//fprintf(fpOut,"%02x",ch);
	fprintf(fpOut,"\n\t");

	if(++count==21) {
		page_count++;
	    fseek(fpIn,page_count*PAGE_SIZE,SEEK_SET);   // go to next page
	    count = 0;
	}
	if( (ch=getc(fpIn)) == 0xFF) //an 0xFF after a page indicates end of valid readings
	    break;
	else
	    fseek(fpIn,-1,SEEK_CUR);

   }
   fclose(fpIn);
   fclose(fpOut);
   return 0;
}
