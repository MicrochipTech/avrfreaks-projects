/****************************************************************************
 * LED 5x7 matrix drive demo
 * Will work with either the common-cathode or anode versions
 * with only modification being inverting the drive on the common line
 * and data line at runtime.
 * level : Beginner (my 1st project ever)
 *
 * micro : ATMega 8 (requires 5 + 7 output lines)
 *  ~900 bytes of flash.
 * 5 transistors, kingbright common-cathode TC07, 5 resistors, power 
 *  some filter capacitors, 3x AA cells.
 *
 * platform : WinAvr + homemade parallel programmer
 *
 * By : Conrad Braam
 *      zaphodikus          www.avrfreaks.net
 *
 * Version:
 * 1.01      12/08/08       Turn LED driving code into a loop to save code
 **************************************************************************/


#include <avr/interrupt.h>    // Defines pins, ports, etc to make programs easier to read
#define F_CPU 100000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>

typedef unsigned char byte;   // a u8
typedef byte glyph[5] ;       // a font character

// This font was copied and then modified from .asm example in the book
// 'Programming and Customizing the AVR Microcontroller' Dhananjay V Gadre
// amazon.com
// note that the font starts in the rightmost column
const glyph myFont[60] = {
   { // space  [32]
   0x00, 0x00, 0x00, 0x00, 0x00
   },
   { // !
   0x00, 0x00, 0x7D, 0x00, 0x00
   },
   { // "
   0x00, 0x60, 0x00, 0x60, 0x00
   },
   { // #
   0x14, 0x7F, 0x14, 0x7F, 0x14
   },
   { // $
   0x04, 0x2A, 0x7F, 0x2A, 0x10
   },
   { // %
   0x41, 0x30, 0x08, 0x06, 0x41
   },
   { // &
   0x06, 0x33, 0x4D, 0x39, 0x06
   },
   { // '
   0x00, 0x00, 0x60, 0x00, 0x00
   },
   { // (
   0x00, 0x00, 0x41, 0x3E, 0x00
   },
   { // )
   0x00, 0x3E, 0x41, 0x00, 0x00
   },
   { // *
   0x00, 0x2A, 0x1C, 0x2A, 0x00
   },
   { // +
   0x08, 0x08, 0x7F, 0x08, 0x08
   },
   { // , comma
   0x00, 0x00, 0x02, 0x01, 0x00
   },
   { // -
   0x08, 0x08, 0x08, 0x08, 0x08
   },
   { // period
   0x00, 0x00, 0x01, 0x00, 0x00
   },
   { // / slash
   0x40, 0x30, 0x08, 0x06, 0x01
   },
   {  // 0 zero         
   0x3E, 0x51, 0x49, 0x45, 0x3E
   },
   {   // 1
   0x01, 0x01, 0x7F, 0x21, 0x11
   },
   {    // 2      
   0x33, 0x49, 0x45, 0x43, 0x31
   },
   {          
   0x36, 0x49, 0x49, 0x41, 0x22
   },
   {          
   0x04, 0x7F, 0x24, 0x14, 0x0C
   },
   {          
   0x4E, 0x51, 0x51, 0x51, 0x72
   },
   { // 6                      
   0x26, 0x49, 0x49, 0x49, 0x3E
   },
   { // 7
   0x70, 0x48, 0x44, 0x42, 0x41
   },
   { // 8
   0x36, 0x49, 0x49, 0x49, 0x36
   },
   { // 9                      
   0x3E, 0x49, 0x49, 0x49, 0x32
   },
   { // :
   0x00, 0x00, 0x36, 0x00, 0x00
   },
   { // ;
   0x00, 0x00, 0x34, 0x02, 0x00
   },
   { // <
   0x00, 0x41, 0x22, 0x14, 0x08
   },
   { // =
   0x00, 0x14, 0x14, 0x14, 0x00
   },
   { // >
   0x08, 0x14, 0x22, 0x41, 0x00
   },
   { // ?    [64]
   0x30, 0x48, 0x45, 0x40, 0x20
   },
   { // @
   0x39, 0x55, 0x5D, 0x41, 0x3E
   },
   { // A
   0x1F, 0x28, 0x48, 0x28, 0x1F
   },
   { // B         
   0x36, 0x49, 0x49, 0x49, 0x7F
   },
   { // C         
   0x22, 0x41, 0x41, 0x41, 0x3E
   },
   {  // D      
   0x3E, 0x41, 0x41, 0x41, 0x7f
   },
   {  // E        
   0x41, 0x49, 0x49, 0x49, 0x7f
   },
   {   //F       
   0x40, 0x48, 0x48, 0x48, 0x7F
   },
   {          
   0x26, 0x45, 0x45, 0x41, 0x3E
   },
   {          
   0x7f, 0x08, 0x08, 0x08, 0x7F
   },
   {          
   0x41, 0x41, 0x7F, 0x41, 0x41
   },
   {          
   0x40, 0x7E, 0x41, 0x41, 0x46
   },
   {          
   0x41, 0x22, 0x14, 0x08, 0x7f
   },
   {          
   0x01, 0x01, 0x01, 0x01, 0x7F
   },
   {          
   0x7F, 0x20, 0x18, 0x20, 0x7F
   },
   {          
   0x7F, 0x04, 0x08, 0x10, 0x7F
   },
   {   // O       
   0x3E, 0x41, 0x41, 0x41, 0x3E
   },
   {   // P        
   0x30, 0x48, 0x48, 0x48, 0x7F
   },
   {          
   0x3F, 0x43, 0x45, 0x41, 0x3E
   },
   {          
   0x31, 0x4A, 0x4C, 0x48, 0x7F
   },
   {          
   0x26, 0x49, 0x49, 0x49, 0x32
   },
   {          
   0x40, 0x40, 0x7F, 0x40, 0x40
   },
   {          
   0x7E, 0x01, 0x01, 0x01, 0x7E
   },
   {          
   0x7C, 0x02, 0x01, 0x02, 0x7C
   },
   {          
   0x7E, 0x01, 0x0E, 0x01, 0x7E
   },
   {          
   0x63, 0x14, 0x08, 0x14, 0x63
   },
   {          
   0x60, 0x10, 0x0F, 0x10, 0x60
   },
   {  // Z          
   0x61, 0x51, 0x49, 0x45, 0x43
   }
};

/* some fancifull characters in a cast of their own
const glyph myFont[13] = {
   { // :-)
   0x22, 0x01, 0x09, 0x01, 0x22
   },
   {
   0x21, 0x22, 0x0A, 0x22, 0x21
   },
   {
   0x09, 0x2A, 0x5C, 0x2A, 0x09
   },
   {
   0x30, 0x48, 0x4F, 0x48, 0x30
   },
   { //star
   0x12, 0x1C, 0x78, 0x1C, 0x12
   },
   { //moon
   0x22, 0x63, 0x5D, 0x41, 0x3E
   },
   { //car
   0x18, 0x24, 0x24, 0x7F, 0x32
   },
   { //ball1
   0x00, 0x00, 0x20, 0x50, 0x20
   },
   { //ball2
   0x00, 0x00, 0x10, 0x28, 0x10
   },
   { //bal3
   0x00, 0x00, 0x08, 0x14, 0x08
   },
   {  //ball4
   0x00, 0x00, 0x04, 0x0A, 0x04
   },
   { //ball5
   0x00, 0x00, 0x02, 0x05, 0x02
   },
   { // space                      
   0x00, 0x00,  
   0x00, 0x00,  
   0x00
   }
};
*/

extern void NextChar(void);
extern void ShiftColumns(void);
extern int GetIndex(char ch);

// maximum text message length (you can happily make it longer)
#define MAXDRIVERMESSAGE 80
// pulse width duration. Either change or edit F_CPU for other clocks 
#define ONTIME           30
// 1 column of escape between chars
#define ESCAPE            1     

struct _driver       // variables all nicely packed into a struct for ease of moving
{                    
   glyph window;     // this is the data for the current screen
   glyph *pCharNew;  // data for the next char to display
   
   int   delay;      // # of repaints before we scroll by 1 column
   int   shifts;     // # scrolled steps. after we shift 5 columns, it's time for the next character
   
   char  message[MAXDRIVERMESSAGE];
   int   offset;     // column in the next char that we are 'pulling' in
}  driver;


// Print the character, and trigger scrolling if needed.
// we basically have to call this function often in your program
void UpdateDisp(void)
{
   // the port code is 'hard-coded' :-(
   // it works like this:
   // 1. turn on the column
   // 2. turn on the bits
   // 3. leave it on to get the eye to see it
   // 4. turn it off
   // rinse and repeat for each column 20 times over before we scroll the whole window
   int col = 4; // zero based
   while (col>=0)
   {
      PORTC = 1 << col;                // 1. turn on col1     
      // 2. turn on rows
      PORTD = driver.window[col];  // 2.
      _delay_ms(ONTIME);         // 3.
      PORTC = 0;                 // 4.
      col --;
   }
      
      
   // test if time to scroll
	driver.delay++;
	if (driver.delay>20)
	{
	   driver.delay=0;
      ShiftColumns();
	}

} // UpdateDisp

// scrolling routine
void ShiftColumns(void)
{
   if (driver.shifts >=0)
   {
      // from the right
      driver.window[4] = driver.window[3];
      driver.window[3] = driver.window[2];
      driver.window[2] = driver.window[1];
      driver.window[1] = driver.window[0];
     // copy leftmost column in the new character into our rightmost
      driver.window[0] = (driver.shifts >= ESCAPE? (*(glyph*)driver.pCharNew)[4-driver.shifts+ESCAPE] : 0);
   }

   driver.shifts++;
   if (driver.shifts > (5 + ESCAPE)) //adding 'ESCAPE' gives a pause between characters
   {
      driver.shifts=0;
      NextChar();
   }
}


// map characters from the ASCII to indexes in our font
int GetIndex(char ch)
{
   // convert to upcase (font has no lowercase)
   if ((ch >= 'a')  && (ch <= 'z'))
      ch = ch - 32;
   
   if ((ch < ' ')  || (ch > 'Z'))
      return('?' - 32);
   return(ch - 32);
}


// copy character into the window
void NextChar(void)
{
int charIDX;
   // copy the character in
   charIDX = GetIndex(driver.message[driver.offset]);
   driver.window[0] = myFont[charIDX][0];
   driver.window[1] = myFont[charIDX][1];
   driver.window[2] = myFont[charIDX][2];
   driver.window[3] = myFont[charIDX][3];
   driver.window[4] = myFont[charIDX][4];
      
   driver.offset++;
   if ( !driver.message[driver.offset])
      driver.offset = 0;
   // point to next character for scrolling
   driver.pCharNew = (glyph*)myFont[GetIndex(driver.message[driver.offset])];
}


// assign a text message and set up the driver
void SetMatrixMessage(char * msg)
{
   // copy the characters into our buffer on our own to avoid using the libs
   char *pLast = driver.message + sizeof(driver.message)-2;
   char *pDest = driver.message;
   
   while ((*msg) && (pDest < pLast))
      *pDest++ = *msg++;
   *pDest++ = ' '; //add a blank at end of the message
   *pDest++ = 0;
   
   NextChar();
}


int main(void)
{

   DDRC = 0x1F; // use bits 0-4  : columns
   DDRD = 0x7F; // use bits 0-6  : rows

   SetMatrixMessage("Hello World!");
   
   while(1)
   {
      UpdateDisp();
   }

  return(0);
}
