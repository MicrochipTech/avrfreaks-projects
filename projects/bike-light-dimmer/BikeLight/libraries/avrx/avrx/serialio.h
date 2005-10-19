/*
Copyright ©1998 - 2002 Larry Barello (larry@barello.net)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/copyleft/lgpl.html

    Header file so C routines can safely call routines in the
    file Serialio.s
    
    Note: although AvrX is processor independent, Serialio.s is not.
    You MUST compile this file for the particular CPU being used as
    GCC no longer places interrupt vectors at link time.
    
 */

void PushChar(void);
char GetChar(void);
char _GetChar(void);    // Echo's charactor
void PutCR(void);
void PutSpace(void);

// Put FLASH string, return pointer to next char after NULL

const char * _PrintString(const char * FlashString);
void InitSerialIO(unsigned UBRValue);
void PutChar(char);

// These might not be C compatible since they rely on C assuming P2
// is destroyed in a function call.

unsigned _GetHex(char digits); // Pass max # digits to enter.
unsigned GetHexWord(void);
unsigned GetHexByte(void);
unsigned GetHexChar(void);
void _PutHex(unsigned val, char digits);
void PutHexWord(unsigned val);
void PutHexByte(unsigned char val);
void PutHexChar(unsigned char val);

