#define CHAR_NORMAL 1
#define CHAR_INVERSE 2
#define CHAR_UNDERLINE 3

#define KS0108_SCREEN_WIDTH		128
#define KS0108_SCREEN_HEIGHT	64


#define DISPLAY_SET_Y       0x40
#define DISPLAY_SET_X       0xB8
#define DISPLAY_START_LINE  0xC0
#define DISPLAY_ON_CMD		0x3E
  #define ON	0x01
  #define OFF	0x00
#define DISPLAY_STATUS_BUSY	0x80

void GLCD_Delay(void);
void GLCD_Initalize(void);
void GLCD_WriteData(unsigned char);
void GLCD_WriteCommand(unsigned char, unsigned char);
void GLCD_ClearScreen(void);
void GLCD_ClearToRowEnd();
void GLCD_GoTo(unsigned char, unsigned char);
void GLCD_WriteChar(char charToWrite);
void GLCD_WriteString(char *);
unsigned char GLCD_ReadByteFromROMMemory(char *);
unsigned char GLCD_ReadData(void);
void GLCD_Bitmap(char *, unsigned char, unsigned char, unsigned char, unsigned char);



