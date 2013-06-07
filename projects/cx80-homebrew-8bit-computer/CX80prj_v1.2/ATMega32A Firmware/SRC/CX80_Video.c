/*
 * CX80_Video.c
 *
 * Created: 05/03/2013 10:53:40
 *  Author: Calogiuri Enzo Antonio
 */ 
#include "CX80.h"

#define ESCAPE_SEQUENCE			0x1B

/*Print a string on video*/
void CX80_Do_PrintString(const char *str)
{
	while (*str)
		CX80_Usart_SendChar(*str++);
}

/*Moves the cursor up. If the cursor is already on the top line, there is no effect.*/
void CX80_Do_CursorUp(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('A');
}

/*Moves the cursor down. If the cursor is already on the bottom line, there is no effect.*/
void CX80_Do_CursorDown(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('B');
}

/*Moves the cursor right. If the cursor is already at the right-hand-side, there is no effect.*/
void CX80_Do_CursorRight(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('C');
}

/*Moves the cursor left. If the cursor is already at the left-hand-side, there is no effect. */
void CX80_Do_CursorLeft(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('D');
}

/*Clear screen and home is row 0, column 0. */
void CX80_Do_ClearScreen(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('E');
}

/*Home is row 0, column 0. Nothing is cleared.*/
void CX80_Do_CursorHome(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('H');
}

/*Also known as 'Reverse Index'*/
void CX80_Do_ReverseLineFeed(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('I');
}

/*Clears from the cursor (inclusive) to the end of the screen. The cursor is not moved.*/
void CX80_Do_ClearToEndOfScreen(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('J');
}

/*Clears from the cursor (inclusive) to the end of the line. The cursor is not moved.*/
void CX80_Do_ClearToEndOfLine(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('K');
}

/*Moves the cursor to the specified row and column. */
void CX80_Do_MoveCursor(char r, char c)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('Y');
	CX80_Usart_SendChar(0x20 + r);
	CX80_Usart_SendChar(0x20 + c);
}

/*Set single line dimension. Parameter c can be:
	'0': single width, single height
	'1': double width, single height
	'2': single width, double height (top half)
	'3': single width, double height (bottom half)
	'4': double width, double height (top half)
	'5': double width, double height (bottom half)
*/
void CX80_Do_SetLineDimension(char c)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('_');
	CX80_Usart_SendChar(c);
}

/*Clears from the cursor (inclusive) to the start of the screen. The cursor is not moved.*/
void CX80_Do_ClearToStartOfScreen(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('b');
}

/*Show or hidden the cursor*/
void CX80_Do_CursorShow(char c)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar(c ? 'e' : 'f');
}

/*The current row/column is stored. Only another 'Save Cursor' will overwrite it.*/
void CX80_Do_SaveCursorPosition(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('j');
}

/*If the cursor has not previously been saved, (0,0) is used.*/
void CX80_Do_RestoreCursorPosition(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('k');
}

/*Clears the current row and moves the cursor to column 0.*/
void CX80_Do_ClearLine(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('l');
}

/*Clears from the cursor (inclusive) to column 0 on the current row. The cursor is not moved.*/
void CX80_Do_ClearToStartOfLine(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('o');
}

/*Enable or disable line overflow*/
void CX80_Do_LineOverflow(char c)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar(c ? 'v' : 'w');
}

/*Set cursor to block shape*/
void CX80_Do_BlockCursor(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('x');
	CX80_Usart_SendChar('4');
}

/*Set cursor to underline shape*/
void CX80_Do_UnderlineCursor(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('y');
	CX80_Usart_SendChar('4');
}

/*set the invert output (black on white background) or normal output (white on black background).*/
void CX80_Do_VideoOutput(char c)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar(c ? 'x' : 'y');
	CX80_Usart_SendChar('>');
}