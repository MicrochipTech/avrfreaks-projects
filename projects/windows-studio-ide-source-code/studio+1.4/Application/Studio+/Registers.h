// Registers.h

#ifndef REGISTERS_H
#define REGISTERS_H

#ifndef REGISTERSWIN

extern BOOL OpenRegistersWindow();
extern void UpdateRegisters(unsigned char * values, int offset, int count);
extern void DrawRegisterValues();

extern HWND registersWindowHandle;
extern char * hexChars;

#endif

BOOL OpenRegistersWindow();
LRESULT CALLBACK RegistersProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam);
void UpdateRegisters(unsigned char * values, int offset, int count);
void DrawRegisterValues();
void RegDrawIndentedRect(HDC hDC, int x, int y, int w, int h);

#endif