#include "avr/io.h"
#include "string.h"
#include "scImConsole.h"
#include "stdio.h"

#define ENABLE_BIT_DEFINITIONS 1

char sData[100];
char *pImStrok;
char *pImCmd;
char *pImParam;
#define ELEMENTS_NUM(x) (sizeof(x)/sizeof(x[0]))
int fImBase(char *cImPar);
int fDummy();
static struct fuctions_s
{
    	char *name;
	int (*openfunc)();
    	int (*testfunc)(char *cImPar);
	int (*closefunc)();
} functionality[] =
{
	{"base", fDummy,fImBase,fDummy},
	{"UART", fImInitSerial,fDummy,fDummy}
};
int main( void )
{
  	int index;
       for(index = 0; index < ELEMENTS_NUM(functionality); ++index)
      {
          functionality[index].openfunc(sData);
       }

	while (1)
  	{	 
        fImprintf("#> ");
		fImgets(sData,100);
        pImCmd = strtok(sData," ");
	    for(index = 0; index < ELEMENTS_NUM(functionality); ++index)
            {
                if(strcmp(functionality[index].name, pImCmd) == 0)
                {
                    pImParam=strtok(NULL, "\0");
                    if(!functionality[index].testfunc(pImParam))
                    {
                    		fImprintf("Success\r\n");
                    }
			else
				fImprintf("\r\nFail\r\n");
			break;
                }
            }

		if(index >= ELEMENTS_NUM(functionality))
        {
            for(index = 0; index < ELEMENTS_NUM(functionality); ++index)
            {
                fImprintf(functionality[index].name); fImprintf("\r\n");
            }
        }

 	}
  return 0;
}

int fImBase(char *cImPar)
{
	if(*cImPar!=0)
		sprintf(cImPar,"%s \r\n",cImPar);
	fImprintf(cImPar);
	//fImprinth("12345ABCD8abv9t");
	return 0;
}
int fDummy()
{
	return 0;
}

