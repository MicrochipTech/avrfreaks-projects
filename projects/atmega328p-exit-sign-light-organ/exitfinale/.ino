//Global variables and reassignment of pins
// < E X I T >
int l = 6;// define pin 6 as exit sign <
int e = 3;// define pin 3 as exit sign E
int x = 5;// define pin 5 as exit sign X
int i = 11;// define pin 11 as exit sign I
int t = 10;// define pin 10 as exit sign T
int r = 9;// define pin 9 as exit sign >
int buttnPin = 2;
int buttonVal =0;
int modeSwitch = 1;
int buttonPress = 1;
const int analogPin = 0;  

//Define Modes of operation
void Startup();
void StandardMode();
void MusicMode1();
void MusicMode2();
void MusicMode3();

//Define Effects
void ClearAll();
void Light(int light, int power);
void LightAll(); //1
void DimAll();//2
void ChaseRight();//4
void ChaseLeft();//5
void Mosaic();//6
void SoftChaseRight();//7
void SoftChaseLeft();//8
void Flicker();//9
void MultiFlicker();//10
void SimpleFlicker();//11
void InsideOut();//12
void OutsideIn();//13

void setup() {
 pinMode(2, INPUT);
 // Serial.begin(9600);//DO NOT FORGET TO REMOVE THIS AFTER TESTING BARRAGARRAH!
}
void loop() {
//InsideOut();OutsideIn();SimpleFlicker(); ClearAll(); Mosaic(); Mosaic(); MultiFlicker(); ChaseRight(); ChaseLeft(); Flicker(); SoftChaseRight(); SoftChaseLeft(); LightAll(); delay(1000);DimAll();    
//for(int x = 0; x < 400; x++){MusicMode3();}
//for(int x = 0; x < 400; x++){MusicMode2();}
StandardMode();
}

void Startup(){
}

void StandardMode(){

  do{
  buttonVal = digitalRead(buttnPin);

    if(buttonVal == LOW)  {buttonPress++;}
    if(buttonPress > 1){modeSwitch++; buttonPress = 0;}
    switch (modeSwitch) {
      case 1: //music mode 1
              delay(100);
              MusicMode1();
        break;
      case 2: //music mode 2
              delay(100);
              MusicMode2();
        break;
      case 3: //music mode 3
              delay(100);
              MusicMode3();
        break;
      case 4: //mosaic
              Mosaic();
        break;
      case 5: //Chase right
              ChaseRight();
        break;
      case 6: //Chase left
              ChaseLeft(); 
        break;
      case 7: //Chase right then left
              ChaseRight(); ChaseLeft(); 
        break;
      case 8: //soft chase right
              SoftChaseRight(); 
        break;
      case 9: //soft chase left
              SoftChaseLeft(); 
        break;
      case 10: //soft chase right then left
              SoftChaseRight(); 
              SoftChaseLeft(); 
        break;
      case 11: //flicker
              Flicker();
        break;
      case 12: //multiflicker
              MultiFlicker();
        break;
      case 13: //simpleflicker
              SimpleFlicker(); 
        break;
      case 14: //inside out
              InsideOut();
        break;
      case 15: //outside in
              OutsideIn();
        break;
      case 16: //inside out then back in
              InsideOut();
              OutsideIn();
        break;
      case 17: //test light <
              ClearAll();
              Light(l, 255);
        break;
      case 18: //test light E
              ClearAll();
              Light(e, 255);
        break;
      case 19: //test light X
              ClearAll();
              Light(x, 255);
        break;
      case 20: //test light I
              ClearAll();
              Light(i, 255);
        break;
      case 21: //test light T
              ClearAll();
              Light(t, 255);
        break;
      case 22: //test light >
              ClearAll();
              Light(r, 255);
        break;
      case 24: //Lightall then DimAll
              LightAll(); delay(1000);DimAll();delay(1000);
        break;
      case 25: //demo mode
              Light(x,255);delay(200);InsideOut();OutsideIn();SimpleFlicker(); ClearAll(); Mosaic(); Mosaic(); MultiFlicker(); ChaseRight(); ChaseLeft(); Flicker(); SoftChaseRight(); SoftChaseLeft(); LightAll(); delay(1000);DimAll();    
        break;
      case 26: //reset button mode value
              modeSwitch = 1;
        break;
      default: //Error code 3
              ClearAll();
              Light(e,255);
     }
  }while(1);
}

void Light(int light, int power)
{
  ClearAll();
  analogWrite(light, power);
}

void MusicMode1(){ //this will be used to test input with serial out. please discard the slow ass serial out once finished :)
  int mn = 1024;     // mn only decreases
  int mx = 0;        // mx only increases
  // Perform 10000 reads. Update mn and mx for each one.
  for (int i = 0; i < 10; ++i) {
    int val = analogRead(analogPin);
    mn = min(mn, val);
    mx = max(mx, val);
  }

  // Send min, max and delta over Serial
/*  Serial.print("m=");
  Serial.print(mn);  
  Serial.print(" M=");
  Serial.print(mx);  
  Serial.print(" D=");
  Serial.print(mx-mn);  
  Serial.println();  */
  if (mx-mn <= 250){analogWrite(l,0);analogWrite(e,0); analogWrite(x,0);analogWrite(i,0);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 251 && mx-mn <= 400){analogWrite(l,255);analogWrite(e,0); analogWrite(x,0);analogWrite(i,0);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 401 && mx-mn <= 500){analogWrite(l,255);analogWrite(e,255); analogWrite(x,0);analogWrite(i,0);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 551 && mx-mn <=650){analogWrite(l,255);analogWrite(e,255); analogWrite(x,255);analogWrite(i,0);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 651 && mx-mn <=850){analogWrite(l,255);analogWrite(e,255); analogWrite(x,255);analogWrite(i,255);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 851 && mx-mn <=900 ){analogWrite(l,255);analogWrite(e,255); analogWrite(x,255);analogWrite(i,255);analogWrite(t,255);analogWrite(r,0);}
  if (mx-mn >= 701){analogWrite(l,255);analogWrite(e,255); analogWrite(x,255);analogWrite(i,255);analogWrite(t,255);analogWrite(r,255);}
}

void MusicMode2(){ //here for filler until finished testing.

  int mn = 1024;     // mn only decreases
  int mx = 0;        // mx only increases
  int val = 0;
  // Perform 10000 reads. Update mn and mx for each one.
  for (int i = 0; i < 500; ++i) {
    val = analogRead(analogPin);
    mn = min(mn, val);
    mx = max(mx, val);
  }

  // Send min, max and delta over Serial
  analogWrite(l,mx-mn);analogWrite(e,mx-mn); analogWrite(x,mx-mn);analogWrite(i,mx-mn);analogWrite(t,mx-mn);analogWrite(r,mx-mn);
}


void MusicMode3(){//here for filler until finished testing.

  int mn = 1024;     // mn only decreases
  int mx = 0;        // mx only increases

  // Perform 10000 reads. Update mn and mx for each one.
  for (int i = 0; i < 500; ++i) {
    int val = analogRead(analogPin);
    mn = min(mn, val);
    mx = max(mx, val);
  }

  if (mx-mn <= 200){analogWrite(l,0);analogWrite(e,0); analogWrite(x,0);analogWrite(i,0);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 201 && mx-mn <= 350){analogWrite(l,0);analogWrite(e,0); analogWrite(x,255);analogWrite(i,255);analogWrite(t,0);analogWrite(r,0);}
  if (mx-mn >= 351 && mx-mn <= 550){analogWrite(l,0);analogWrite(e,255); analogWrite(x,255);analogWrite(i,255);analogWrite(t,255);analogWrite(r,0);}
  if (mx-mn > 551){analogWrite(l,255);analogWrite(e,255); analogWrite(x,255);analogWrite(i,255);analogWrite(t,255);analogWrite(r,255);}
}

void ClearAll(){
  analogWrite (l,0);
  analogWrite (e,0);
  analogWrite (x,0);
  analogWrite (i,0);
  analogWrite (t,0);
  analogWrite (r,0);
}

void Mosaic()
{
  ClearAll();
  int breaker = 0;
  int timeLight = 0;  
  int timeLight1 = 0;
  int timeLight2 = 255;
  do{
       if(timeLight < 255){
        analogWrite(l, timeLight1);
        analogWrite(x, timeLight1);
        analogWrite(t, timeLight1++);
        analogWrite(e, timeLight2);
        analogWrite(i, timeLight2);
        analogWrite(r, timeLight2--);
      }
  buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     if (timeLight > 255){
        analogWrite(l, timeLight1);
        analogWrite(x, timeLight1);
        analogWrite(t, timeLight1--);
        analogWrite(e, timeLight2);
        analogWrite(i, timeLight2);
        analogWrite(r, timeLight2++);
      }
    timeLight++;
    delay(5);
    if (timeLight > 510){breaker=1;}
  }
  while(breaker < 1);
}

void ChaseRight(){
  ClearAll();
  analogWrite(l,255); delay(500);
  analogWrite(l,0);
buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(e,255); delay(500);
  analogWrite(e,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(x,255); delay(500);
  analogWrite(x,0);
 buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(i,255); delay(500);
  analogWrite(i,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(t,255); delay(500);
  analogWrite(t,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(r,255); delay(500);
  analogWrite(r,0);

}

void ChaseLeft(){
  ClearAll();
  analogWrite(r,255); delay(500);
  analogWrite(r,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(t,255); delay(500);
  analogWrite(t,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(i,255); delay(500);
  analogWrite(i,0);
 buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(x,255); delay(500);
  analogWrite(x,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(e,255); delay(500);
  analogWrite(e,0);
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  analogWrite(l,255); delay(500);
  analogWrite(l,0);
}

void SoftChaseRight(){
  ClearAll();
  for(int bright=0; bright < 255; bright++)
  {
   analogWrite(l, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(e, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(x, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(i, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(t, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(r, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
    for(int bright=255; bright > 0; bright--)
  {
   analogWrite(l, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(e, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(x, bright); delay(2);
  }
   buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(i, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(t, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(r, bright); delay(2);
  }
  
}
void SoftChaseLeft(){
  ClearAll();
  for(int bright=0; bright < 255; bright++)
  {
   analogWrite(r, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(t, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(i, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(x, bright); delay(2);
  }
   buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(e, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(l, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
    for(int bright=255; bright > 0; bright--)
  {
   analogWrite(r, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(t, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(i, bright); delay(2);
  }
  
   buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(x, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(e, bright); delay(2);
  }
  buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(l, bright); delay(2);
  }  
}

void LightAll(){
  ClearAll();
  for(int bright=0; bright < 255; bright++)
  {
    analogWrite(l, bright);
    analogWrite(e, bright);
    analogWrite(x, bright);
    analogWrite(i, bright);
    analogWrite(t, bright);
    analogWrite(r, bright); delay(2);
    buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  }  
}
void DimAll(){
  ClearAll();
  for(int bright=255; bright > 0; bright--)
  {
    analogWrite(l, bright);
    analogWrite(e, bright);
    analogWrite(x, bright);
    analogWrite(i, bright);
    analogWrite(t, bright);
    analogWrite(r, bright); delay(2);
    buttonVal = digitalRead(buttnPin);
if(buttonVal == LOW)  {buttonPress++;}
  }
}  
void Flicker()
{
  int timego;
  do{
    timego++;
    int light = random(256);
    analogWrite(l,light);
    analogWrite(e,light);
    analogWrite(x,light);
    analogWrite(i,light);
    analogWrite(t,light);
    analogWrite(r,light);
     buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  }while(timego < 10000);
}

void MultiFlicker()
{
  int timego;
  do{
    timego++;
    int light1 = random(100, 256);
    int light2 = random(80);
    int light3 = random(256);
    analogWrite(l,light1);
    analogWrite(e,light1);
    analogWrite(x,light2);
    analogWrite(i,light2);
    analogWrite(t,light3);
    analogWrite(r,light3);
     buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  }while(timego < 10000);
}

void SimpleFlicker()
{
  int timego;
  do{
    timego++;
    int light = random(256);
    analogWrite(l,255);
    analogWrite(e,light);
    analogWrite(x,255);
    analogWrite(i,light);
    analogWrite(t,255);
    analogWrite(r,255);
     buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
  }while(timego < 10000);
}

void InsideOut()
{
  ClearAll();
  for(int light=0;light < 255; light++)
  {
     analogWrite(x,light);
     analogWrite(i,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=0;light < 255; light++)
  {
     analogWrite(e,light);
     analogWrite(t,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=0;light < 255; light++)
  {
     analogWrite(r,light);
     analogWrite(l,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=255 ;light > 0; light--)
  {
     analogWrite(x,light);
     analogWrite(i,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=255 ;light > 0; light--)
  {
     analogWrite(e,light);
     analogWrite(t,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=255 ;light > 0; light--)
  {
     analogWrite(r,light);
     analogWrite(l,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
}

void OutsideIn()
{
  ClearAll();
  for(int light=0;light < 255; light++)
  {
     analogWrite(l,light);
     analogWrite(r,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=0;light < 255; light++)
  {
     analogWrite(e,light);
     analogWrite(t,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=0;light < 255; light++)
  {
     analogWrite(x,light);
     analogWrite(i,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=255 ;light > 0; light--)
  {
     analogWrite(l,light);
     analogWrite(r,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=255 ;light > 0; light--)
  {
     analogWrite(e,light);
     analogWrite(t,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
  for(int light=255 ;light > 0; light--)
  {
     analogWrite(x,light);
     analogWrite (i,light);
      buttonVal = digitalRead(buttnPin);
 if(buttonVal == LOW)  {buttonPress++;}
     delay(2);
  }
}
