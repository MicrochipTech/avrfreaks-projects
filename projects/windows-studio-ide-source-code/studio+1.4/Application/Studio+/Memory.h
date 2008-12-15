// Memory.h

#ifndef MEMORY_H
#define MEMORY_H

#ifndef MEMORY

extern BOOL OpenSRAMWindow();
extern BOOL OpenEEPROMWindow();
extern void DrawSramValues();
extern void DrawEepromValues();

extern HWND sramWindowHandle;
extern int sramClientW;
extern int sramClientH;
extern HWND eepromWindowHandle;
extern int eepromClientW;
extern int eepromClientH;

#endif

BOOL OpenSramWindow();
BOOL OpenEepromWindow();
LRESULT CALLBACK SramProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EepromProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SramScrollProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam);
LRESULT CALLBACK EepromScrollProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam);
void DrawSramValues();
void DrawEepromValues();
void MemDrawTopBevel(HDC hDC, int x, int y, int w);
void MemDrawIndentedRect(HDC hDC, int x, int y, int w, int h);
void MemDrawBottomBevel(HDC hDC, int x, int y, int w);

#endif