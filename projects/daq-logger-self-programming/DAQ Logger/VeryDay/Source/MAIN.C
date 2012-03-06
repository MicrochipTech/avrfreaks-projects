//  MAIN.C

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>

void ShowTitle(void);
void AcquireData(void);
void PlotFile(void);
extern void Draw(const char* infile);
extern int WriteTxt(const char* infile);


int main()
{
	int prompt,quit=0,error;

	while(quit != 1) {

		clrscr();
		ShowTitle();

		printf("\t1. SELECT A FILE TO PLOT\n");
		printf("\t2. ACQUIRE DATA FROM DEVICE\n");
		printf("\t3. QUIT\n\n");

		prompt = getchar();
		fflush(stdin);
		switch(prompt) {

		    case '1': PlotFile(); break;
		    case '2': AcquireData(); break;
		    case '3': quit = 1;break;
		    default : printf("\n\nPlease enter your choice again...");
			      getch();

		}

	}

return 0;

} //main


void ShowTitle(void)
{

	printf("\n\n");
	printf("\t\tXXXXXXXXXXXXXXX -eVeryDay- XXXXXXXXXXXXXXX\n");
	printf("\n\t\t\t   SIMPLE DATA LOGGING\n\n\n\n");

}

void AcquireData(void)
{
	struct date today;
	char savefile[20],avrdudeCommand[80];

	clrscr();
	ShowTitle();
	getdate((struct date*)&today);
	sprintf(savefile,"%d%02d%02d.bin",today.da_year,today.da_mon,today.da_day);
	sprintf(avrdudeCommand,"avrdude -p m16 -P usb -c usbasp -U flash:r:%s:r",savefile);
	system(avrdudeCommand);
	printf("\n\nSaving Report to text file...");
	WriteTxt(savefile);
	getch();

}

void PlotFile(void)
{

	int prompt;
	char infile[20];

	clrscr();
	ShowTitle();
	printf("Available files are:\n\n");
	system("dir /b *.bin");
	printf("\nEnter filename to plot:\n");
	gets(infile);

	printf("\nUsing file: %s ...",infile);
	printf("\nYou can use +/- keys to scale while viewing\n");
	getch();
	Draw(infile);


} //PlotFile