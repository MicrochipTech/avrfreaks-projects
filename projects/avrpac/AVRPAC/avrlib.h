#include <pgmspace.h>
#include <stdio.h>    
#include <stdlib.h> 
#include <ina90.h>
#include <io8515.h>

void interrupt[UART_RX_vect] uartRXHandler(void);
void SetPos(char i, char j);
void itoa(unsigned int n,char s[]);
void sethiscore(char *name, unsigned int score);
unsigned int gethiscore(char *name, int maxsize);
void SetEEPROMChar(unsigned int address, char c);
char GetEEPROMChar(unsigned int address);
