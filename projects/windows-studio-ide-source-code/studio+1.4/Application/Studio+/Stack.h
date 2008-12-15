// Stack.h

#ifndef STACK_H
#define STACK_H

#ifndef STACK

extern BOOL OpenStackWindow();
extern void DrawStackValues();
extern void EnsureStackVisible();

extern HWND stackWindowHandle;
extern int stackClientW;
extern int stackClientH;

#endif

BOOL OpenStackWindow();
void DrawStackValues();
void EnsureStackVisible();
LRESULT CALLBACK StackProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK StackScrollProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam);
void StackDrawTopBevel(HDC hDC, int x, int y, int w);
void StackDrawIndentedRect(HDC hDC, int x, int y, int w, int h,
	HBRUSH bkBrush);
void StackDrawBottomBevel(HDC hDC, int x, int y, int w);

#endif