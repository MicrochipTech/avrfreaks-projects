#ifndef lcd_H
#define lcd_H

#define ROW_TOP		0
#define ROW_BOTTOM	1

void InitLcd (void);
void LcdDisplaySplash (void);
void LcdClear (void);
void LcdWrite (char *);
void LcdWriteAt (char *string, uint8_t line, uint8_t pos);
void LcdWrite_P (char *);
void LcdWriteLine_P (char *, unsigned char line);
void LcdWriteAnimated_P (char *string, unsigned char line);
void LcdWriteToCenter_P (char *string, unsigned char line);
void LcdUpdate (void);
void LcdUpdateInput (char *string);
void LcdShowScreenBattery (void);
void LcdShowScreenCurrent (void);
void LcdShowScreenTemperature (void);
void LcdUpdateBattery (void);
void LcdUpdateCurrent (void);
void LcdUpdateTemp (void);
void LcdShowDateTime (void);
void LcdUpdateDateTime (void);

void LcdSetInput(char *string, unsigned char line, unsigned char pos);

#endif
