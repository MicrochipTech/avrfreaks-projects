// Panel8x8_05_GameOfLife
// Example 05: Manipulating the display using programming.
//
// This example demonstrates a simple direct animation based on 
// Conway's Game of Life.  See Google.  We randomize the display
// by reading the unattached A0, which should be floating around.
// We garbage up the screen, run the life sim, then 
// we write directly to the output buffer and then display it.
//
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-09-22 Removed PANELS & NO_CHDATA includes
// 2009-09-18 Initial Version


// Please remember to set PANELS to the physical number of
// panels to support in Panel8x8.h -- defaults to 4.
// In most cases for 1-4 panels this won't make any difference.

// Required include for the Panel8x8 library
#include <Panel8x8.h>

// Declare an object of type Panel8x8
Panel8x8 Panel;

// Local storage
// 3 Instances Last, Curren, Last
byte GameOfLife[3][PANELS][8];
// Current instance
byte Instance = 0;

// Called once by ardruino to setup the sketch
void setup() {
 // Call Panel.Begin to initialize panel buffers
 // Syntax: buffer address, max buffer size, current text size, 0=Ram/1=Flash
 Panel.Begin(0,0,0,0);  // Initialize with no buffers 
 Panel.SetScrolling(0); // turn off scrolling
 Panel.PanelMode=3;     // direct animation mode
 Panel.frameDelay=500;  // slow it down!!
 Panel.ClearOutput();   // just for safetey
 ClearInstance(0);      // clear out the gameboard
 SetRandom(0);          // fill gameboard with random data
}

// Called repeatedly by arduino
// Loads gameboard into display buffer and displays.
// Calculates next generation,
// And looks for stagnet generations.
// If stagnent, randomize and start again.
void loop() {
 int i,j,n,m;
 // Load data into display
 for (i=0;i<PANELS;i++) {
   for (j=0;j<8;j++) {
     Panel.iScroll[i][j] = GameOfLife[Instance][i][j];
   }
 }
 // Display
 Panel.Loop();
 Instance = CalcInstance(Instance);
 // Ok, compare the new instance to 2 ago.
 m = 0;
 n = Instance-2;
 if (n<0) {n+=3;}
 for (i=0;i<PANELS;i++) {
   for (j=0;j<8;j++) {
     if (GameOfLife[Instance][i][j] != GameOfLife[n][i][j]) {m=1; i=PANELS; j=8;}
   }
 } 
 // if m=1, then we repeated a pane
 if (m==0) SetRandom(Instance);
}


// ClearInstance
// Clears the game board
void ClearInstance(int k) {
int i,j;
 for (i=0;i<PANELS;i++) {
   for (j=0;j<8;j++) {
     GameOfLife[k][i][j]=0;
   }
 }
}

// SetRandom
// Fills up the output with random garbage.
void SetRandom(int k) {
int i,j;
 for (i=0;i<PANELS;i++) {
   for (j=0;j<8;j++) {
     GameOfLife[k][i][j]=analogRead(0);
   }
 }
}

// CalcInstance
// Returns the new instance
int CalcInstance(int k) {
int p; // panel
int l; // line
int n; // new insance
int b; // bit number
int c; // count of neighbors
int db; //destination bit
int dp; //destination panel
int dl; //destination line
 n=k+1;
 if (n>2) n=0;
 ClearInstance(n);
 for (p=0;p<PANELS;p++) {
   for (l=0;l<8;l++) {
     for (b=0;b<8;b++) {
       c= iCheckBit(k,p,l-1,b); // North
       c+= iCheckBit(k,p,l,b-1); // East
       c+= iCheckBit(k,p,l,b+1); // West
       c+= iCheckBit(k,p,l+1,b); // South
       c+= iCheckBit(k,p,l-1,b-1); // NorthEast
       c+= iCheckBit(k,p,l-1,b+1); // NorthWest
       c+= iCheckBit(k,p,l+1,b-1); // SouthEast
       c+= iCheckBit(k,p,l+1,b+1); // SouthWest
       if (bitRead(GameOfLife[k][p][l],b)) {
         // Live Cell
         if (c>=2 && c<=3) {bitSet(GameOfLife[n][p][l],b);}
       }
       else {
         // Dead Cell
         if (c==3) {bitSet(GameOfLife[n][p][l],b);}
       }
     }
   }
 }
 return n;
}

// iCheckBit
// Checks to see if a bit is set in a given instance.
int iCheckBit(int n, int p, int l, int b) {
 if (p<0 || p>PANELS-1) {return 0;}
 if (l<0 || l>7) {return 0;}
 if (b<0 && p<PANELS-1) {b=7; p++;}
 if (b>7 && p>0) {b=0; p--;}
 return (int)bitRead(GameOfLife[n][p][l],b);
}



