# Arduino POV

Uploaded by georgelstuart on 2008-07-06 01:29:00 (rating 0 out of 5)

## Summary

This is my first C code and I just recently started with Arduino (diecimila Atmega 128). I started with code I copied from a German project site. The code worked but is hard coded and rather inflexible, so I decided to clean it up a bit. Full thanks and respect go to Michael Zoellner for putting his code on the web and giving me a starting point.  

This is his original code:  

it is pretty self explanatory and commented on where to tie in the leds:


/*  

????????????????????????????????????????????  

persistence of vision typography with arduino  

michael zoellner - march 2006  

<http://i.document.m05.de>


connect anodes (+) of 5 leds to digital ports of the arduino board  

and put 20-50 ohm resistors from the cathode (-) to ground.


the letters are lookup tables consisting arrays width the dot status in y rows.  

????????????????????????????????????????????  

*/


// defining the alphabet  

int \_[] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};  

int A[] = {0,1,1,1,1, 1,0,1,0,0, 0,1,1,1,1};  

int B[] = {1,1,1,1,1, 1,0,1,0,1, 0,1,0,1,0};  

int C[] = {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1};  

int D[] = {1,1,1,1,1, 1,0,0,0,1, 0,1,1,1,0};  

int E[] = {1,1,1,1,1, 1,0,1,0,1, 1,0,1,0,1};  

int F[] = {1,1,1,1,1, 1,0,1,0,0, 1,0,1,0,0};  

int G[] = {0,1,1,1,0, 1,0,1,0,1, 0,0,1,1,0};  

int H[] = {1,1,1,1,1, 0,0,1,0,0, 1,1,1,1,1};  

int I[] = {0,0,0,0,1, 1,0,1,1,1, 0,0,0,0,1};  

int J[] = {1,0,0,0,0, 1,0,0,0,1, 1,1,1,1,1};  

int K[] = {1,1,1,1,1, 0,0,1,0,0, 0,1,0,1,1};  

int L[] = {1,1,1,1,1, 0,0,0,0,1, 0,0,0,0,1};  

int M[] = {1,1,1,1,1, 0,1,1,0,0, 0,1,1,1,1};  

int N[] = {1,1,1,1,1, 1,0,0,0,0, 0,1,1,1,1};  

int O[] = {0,1,1,1,0, 1,0,0,0,1, 0,1,1,1,0};  

int P[] = {1,1,1,1,1, 1,0,1,0,0, 0,1,0,0,0};  

int Q[] = {0,1,1,1,1, 1,0,0,1,1, 0,1,1,1,1};  

int R[] = {1,1,1,1,1, 1,0,1,0,0, 0,1,0,1,1};  

int S[] = {0,1,0,0,1, 1,0,1,0,1, 1,0,0,1,0};  

int T[] = {1,0,0,0,0, 1,1,1,1,1, 1,0,0,0,0};  

int U[] = {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1};  

int V[] = {1,1,1,1,0, 0,0,0,0,1, 1,1,1,1,0};  

int W[] = {1,1,1,1,0, 0,0,1,1,0, 1,1,1,1,0};  

int X[] = {1,1,0,1,1, 0,0,1,0,0, 1,1,0,1,1};  

int Y[] = {1,1,0,0,0, 0,0,1,0,0, 1,1,1,1,1};  

int Z[] = {1,0,0,1,1, 1,0,1,0,1, 1,1,0,0,1};


int letterSpace;  

int dotTime;


void setup()  

{  

 // setting the ports of the leds to OUTPUT  

 pinMode(2, OUTPUT);  

 pinMode(3, OUTPUT);  

 pinMode(4, OUTPUT);  

 pinMode(5, OUTPUT);  

 pinMode(6, OUTPUT);


 // defining the space between the letters (ms)  

 letterSpace = 6;  

 // defining the time dots appear (ms)  

 dotTime = 3;


}


void printLetter(int letter[])  

{  

 int y;


 // printing the first y row of the letter  

 for (y=0; y<5; y++)  

 {  

 digitalWrite(y+2, letter[y]);  

 }  

 delay(dotTime);


 // printing the second y row of the letter  

 for (y=0; y<5; y++)  

 {  

 digitalWrite(y+2, letter[y+5]);  

 }  

 delay(dotTime);


 // printing the third y row of the letter  

 for (y=0; y<5; y++)  

 {  

 digitalWrite(y+2, letter[y+10]);  

 }  

 delay(dotTime);


 // printing the sspace between the letters  

 for (y=0; y<5; y++)  

 {  

 digitalWrite(y+2, 0);  

 }  

 delay(letterSpace);  

}


void loop()  

{  

 // printing some letters  

 printLetter(N);  

 printLetter(E);  

 printLetter(R);  

 printLetter(D);  

 printLetter(S);  

 printLetter(\_);  

}  

************************ end of original code ******************


I saw that he made repeated calls for the same function, so I changed the array to two dimensional and cleaned up the loop. Connections are the same. Works the same, just a little cleaner. I don't yet have any pictures, or to be honest know how to upload them here. Like I said, I am just getting my feet wet. Hope this helps someone like Michael's helped me get started.  

Cheers!  

George L Stuart  

questions, comments, gripes to [mynameincludinginitial@gmail.com](mailto:mynameincludinginitial@gmail.com)  

My code follows:


// defining the alphabet  

int \_[3][5] = {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}};  

int A[3][5] = {{0,1,1,1,1}, {1,0,1,0,0}, {0,1,1,1,1}};  

int B[3][5] = {{1,1,1,1,1}, {1,0,1,0,1}, {0,1,0,1,0}};  

int C[3][5] = {{0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}};  

int D[3][5] = {{1,1,1,1,1}, {1,0,0,0,1}, {0,1,1,1,0}};  

int E[3][5] = {{1,1,1,1,1}, {1,0,1,0,1}, {1,0,1,0,1}};  

int F[3][5] = {{1,1,1,1,1}, {1,0,1,0,0}, {1,0,1,0,0}};  

int G[3][5] = {{0,1,1,1,0}, {1,0,1,0,1}, {0,0,1,1,0}};  

int H[3][5] = {{1,1,1,1,1}, {0,0,1,0,0}, {1,1,1,1,1}};  

int I[3][5] = {{1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}};  

int J[3][5] = {{1,0,0,0,0}, {1,0,0,0,1}, {1,1,1,1,1}};  

int K[3][5] = {{1,1,1,1,1}, {0,0,1,0,0}, {0,1,0,1,1}};  

int L[3][5] = {{1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,0,1}};  

int M[3][5] = {{1,1,1,1,1}, {0,1,1,0,0}, {0,1,1,1,1}};  

int N[3][5] = {{1,1,1,1,1}, {1,0,0,0,0}, {0,1,1,1,1}};  

int O[3][5] = {{0,1,1,1,0}, {1,0,0,0,1}, {0,1,1,1,0}};  

int P[3][5] = {{1,1,1,1,1}, {1,0,1,0,0}, {0,1,0,0,0}};  

int Q[3][5] = {{0,1,1,1,1}, {1,0,0,1,1}, {0,1,1,1,1}};  

int R[3][5] = {{1,1,1,1,1}, {1,0,1,0,0}, {0,1,0,1,1}};  

int S[3][5] = {{0,1,0,0,1}, {1,0,1,0,1}, {1,0,0,1,0}};  

int T[3][5] = {{1,0,0,0,0}, {1,1,1,1,1}, {1,0,0,0,0}};  

int U[3][5] = {{1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}};  

int V[3][5] = {{1,1,1,1,0}, {0,0,0,0,1}, {1,1,1,1,0}};  

int W[3][5] = {{1,1,1,1,0}, {0,0,1,1,0}, {1,1,1,1,0}};  

int X[3][5] = {{1,1,0,1,1}, {0,0,1,0,0}, {1,1,0,1,1}};  

int Y[3][5] = {{1,1,0,0,0}, {0,0,1,0,0}, {1,1,1,1,1}};  

int Z[3][5] = {{1,0,0,1,1}, {1,0,1,0,1}, {1,1,0,0,1}};  

int rightarrow[3][5] = {{1,0,0,0,1}, {0,1,0,1,0}, {0,0,1,0,0}};  

int leftarrow[3][5] = {{0,0,1,0,0}, {0,1,0,1,0}, {1,0,0,0,1}};  

int letterSpace;  

int dotTime;


void setup()  

{  

 // setting the ports of the leds to OUTPUT  

 pinMode(2, OUTPUT);  

 pinMode(3, OUTPUT);  

 pinMode(4, OUTPUT);  

 pinMode(5, OUTPUT);  

 pinMode(6, OUTPUT);


 // defining the space between the letters (ms)  

 letterSpace = 10;  

 // defining the time dots appear (ms)  

 dotTime = 5;


}


void printLetter(int letter[3][5])  

{  

 int y;  

 int x;


 // x is column and y is row within 2d array for each letter  

 // as y increments, it prints the dots of each vertical element  

 // as x increments, it moves the print scan to the next vertical element to the right


 for (x=0; x<3; x++) {  

 for (y=0; y<5; y++){  

 digitalWrite(y+2, letter[x][y]);  

 }  

 delay(dotTime);  

 for (y=0; y<5; y++)  

 {  

 digitalWrite(y+2, 0);  

 }


 }  

 delay(letterSpace);  

 }


void loop()  

{  

 // printing some letters


 printLetter(H);  

 printLetter(E);  

 printLetter(L);  

 printLetter(L);  

 printLetter(O);  

 printLetter(\_);  

 printLetter(W);  

 printLetter(O);  

 printLetter(R);  

 printLetter(L);  

 printLetter(D);  

 printLetter(\_);  

}

## Tags

- Complete code with HW
