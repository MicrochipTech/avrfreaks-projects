//      dm_chars.h - header file for 5x8 characters

extern char dm_cload(char, char *, char);       // return character definition
extern char dm_ccol(char, char);                // return column of char definition
extern char dm_char(char, char, char);          // display single character
extern int dm_str(char, char *, char);          // display string

#define CC_TRUNC  0x10                          // truncated character or padding
#define CC_TCHAR  0x20                          // trucated the character
#define CC_BL_0   0x00                          // blank width = 0
#define CC_BL_1   0x10                          // blank width = 1
#define CC_BL_2   0x20                          // blank width = 2
#define CC_BL_3   0x30                          // blank width = 3
#define CC_BL_4   0x40                          // blank width = 4
#define CC_BL_5   0x50                          // blank width = 5
#define CC_BL_6   0x60                          // blank width = 6
#define CC_BL_7   0x70                          // blank width = 7
#define CC_ST_ERR 0x0100                        // truncated message somehow
#define CC_ST_OK  0x0200                        // truncation only in final padding
