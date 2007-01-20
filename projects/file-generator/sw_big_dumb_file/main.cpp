#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    printf("\nBig dumb file maker V1.06464684313A\n");
    if(argc != 4)
    {
            printf("\nBad number of parameters!\n");
            printf("Usage: useless_file_maker OUTPUT_FILE NUMBER_OF_CHARS CHAR_NUM\n");
            printf("By David 'daqq' Gustafik. Uses the Marvin license:\nIf you really think it's worth using, use it.\nIt's open source, but who'd want it?\nContact: darth.daqq@gmail.com\n");
            return 1;
    }
    FILE *out;
    if((out = fopen(argv[1],"w"))==NULL)
    {
            printf("Can't open the bloody output file! Try another name!\n");
            return 1;
    }
    
    int char_num = atoi(argv[2]);
    if(char_num == 0 || char_num < 0)
    {
                printf("The number of chars to be written must be greater than 0!\n");
                return 1;
    }
    
    int char_type=999;
    if(sscanf(argv[3],"%i",&char_type) != 1)
    {
     printf("The type of char must be a non zero value within 0-255...\n");
     return 1;
    }
    if(char_type > 255 || char_type < 0)
    {
     printf("The type of char must be a non zero value within 0-255...\n");
     return 1;
    }
    
    for(;char_num!=0;)
    {
     fputc(char_type,out);
     char_num--;
    }
    fclose(out);
    printf("The program has succesfuly written a useles file full of one char. Huraaaay!\n");
    return 0;
}
