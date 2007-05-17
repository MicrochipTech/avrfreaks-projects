
#pragma hdrstop
#include <condefs.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <errno.h>
#include <process.h>
#include "FTD2XX.h"
//---------------------------------------------------------------------------
USELIB("FTD2XX.LIB");
USEFILE("FTD2XX.H");
//---------------------------------------------------------------------------
#pragma argsused

#define PAGE        256     //размер страницы
#define TIMEOUT1    1000    //ожидание ответа в диалоге соединения ms
#define TIMEOUTR1   1       //ожидание ответа в процедуре передачи файла ms
#define TIMEOUTR2   1000    //ожидание последнего ответа в процедуре передачи файла ms
#define TIMEOUTW    3000    //ожидание завершения записи порции данных
#define ATT         8       //число попыток установления диалога

void start_thread(int i);
void thread_code(void *threadno);

char inBuff[PAGE];        // буфер для приема сообщений
char outBuff[PAGE];       // буфер для передачи сообщений
volatile char flagTx = 0;
int     din;
unsigned long   countRead;
unsigned long   countWrite;
FT_HANDLE       ftHandle;

int main(int argc, char* argv[])
{
    char *BufPtrs[3]; // pointer to array of 3 pointers
    char Buffer1[64]; // buffer for description of first device
    char Buffer2[64]; // buffer for description of second device

    char ask[]={'A','S','K'};
    char get[]={'G','E','T'};

    FT_STATUS       ftStatus;
    PVOID           numDevs;
    int     i;
/*----------------------------------------------------------------------------*/
    // initialize the array of pointers
    BufPtrs[0] = Buffer1;
    BufPtrs[1] = Buffer2;
    BufPtrs[2] = NULL; // last entry should be NULL

    if(argc != 2)
    {
        printf("Wrong format command string: type -> load.exe <nput_FILE_name>\n");   //ошибка формата командной строки
        return 0;
    }
    if ((din = open(argv[1], O_RDONLY | O_BINARY)) < 0)
    {
        printf("Cannot open input data file for loading\n");                //ошибка открытия исходного файла для шифрования.
        return 0;
    }
/*----------------------------------------------------------------------------*/
    ftStatus = FT_ListDevices(BufPtrs,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION);
    if (ftStatus == FT_OK)
    {
        if((int)numDevs > 1)
        {
            printf("Connected - %d devices\n", numDevs);
            printf("Unable work more then one device - exit\n");
            return 0;
        }
        printf("Connection exist\n");
        }
        else
        {
            printf("Device disconnected\n");    // FT_ListDevices failed
            return 0;
        }

        ftStatus = FT_Open(0, &ftHandle);
        if(ftStatus != FT_OK)
        {
            printf("Open device error\n");
            return 0;
        }
        printf("Success open device\n");
        FT_SetTimeouts(ftHandle, TIMEOUT1, TIMEOUT1);

        printf("Dialog establish...\n");
        i=ATT;
        /**/
        while(i)
        {
            FT_Write(ftHandle, ask, sizeof(ask), &countWrite);
            FT_Read(ftHandle, inBuff, PAGE, &countRead);
            if(countRead != 2)
            {
                if(--i)
                {
                    FT_Close(ftHandle);
                    printf("Can not establish dialog, exit\n");
                    return 0;
                }
                   continue;
            }
            if((inBuff[0] == 'O') && (inBuff[1] == 'K'))
            {
                printf("OK\n");
                break;
            }
            else printf("Wrong answer\n");
        }
        /**/
/*----------------------------------------------------------------------------*/
        /**/
        FT_Write(ftHandle, get, sizeof(get), &countWrite);

        FT_Read(ftHandle, inBuff, PAGE, &countRead);
        if(countRead != 3)
        {
            FT_Close(ftHandle);
            printf("Wrong answer != 3, exit\n");
            return 0;
        }
        else
        {
            if((inBuff[0] == 'L') && (inBuff[1] == 'E') && (inBuff[2] == 'T'))
            {
                printf("Start programming...\n");
            }
            else
            {
                FT_Close(ftHandle);
                printf("Wrong answer letters, exit\n");
                return 0;
            }
        }
        /**/
/*----------------------------------------------------------------------------*/
        FT_SetTimeouts(ftHandle, TIMEOUTR1, TIMEOUTW);
        start_thread(1);

        while(1)
        {
            switch(flagTx)
            {
                case 0:
                break;
                case 1:
                flagTx=0;
                putchar('.');
                //printf(".");
                FT_Read(ftHandle, inBuff, PAGE, &countRead);
                if(countRead)           //ошибка или успех
                {
                    inBuff[countRead]=0;
                    printf("%s", inBuff);
                    close(din);
                    FT_Close(ftHandle);
                    return 0;
                }
                break;
                case 2:
                FT_Close(ftHandle);
                return 0;

                default:
                close(din);
                FT_Close(ftHandle);
                return 0;
            }
        }
        return 0;
        //getchar();


 
        //FT_Close(ftHandle);
        //return 0;
}

/*-----------------------------------------------------------------------------------*/
void thread_code(void *threadno)
{
    unsigned int quantity;
    
    //printf("\nExecuting thread number %d, ID = %d\n", (int)threadno, _threadid);
    while((quantity = read(din, outBuff, sizeof(outBuff))) > 0)     //чтение по частям исходного файла
    {
        FT_Write(ftHandle, outBuff, quantity, &countWrite);
        flagTx=1;
        if(countWrite != quantity)
        {
            close(din);
            FT_SetTimeouts(ftHandle, TIMEOUTR2, TIMEOUTW);
            flagTx=2;             //признак ошибки процесса передачи файла
            printf("Error loading, exit\n");
            _endthread();
        }
        while(flagTx)
        {}
    }
    close(din);
    flagTx=3;
    _endthread();
}
/*-----------------------------------------------------------------------------------*/
void start_thread(int i)
{
    int thread_id;

#if     defined(__WIN32__)    
    if ((thread_id = _beginthread(thread_code,4096,(void *)i)) == (unsigned long)-1)
#else
    if ((thread_id = _beginthread(thread_code,4096,(void *)i)) == -1)
#endif
    {
        printf("Unable to create thread %d, errno = %d\n",i,errno);
        return;
    }
    //printf("Created thread %d, ID = %ld\n",i,thread_id);
}
/*-----------------------------------------------------------------------------------*/


