#define NUMROW 6
#define NUMCOL 3

int ROW[NUMROW] = {4, 5, 6, 7, 8, 9};  // Positive
int COL[NUMCOL] = {12, 11, 10}; // Negative

int isr_counter = 0;
volatile int second = 0;
int oldsecond = 0;

int hours = 3;
int minutes = 58;
int seconds = 10;

int old_col;
int old_row;

int i;

ISR(TIMER2_OVF_vect) {
  isr_counter++;
  if (isr_counter > 7811) {
    second++;
    isr_counter = 0;
  }
};  

void SetupTimer2(){
  //Timer2 Settings: Timer Prescaler /8, mode 0
  //Timer clock = 16MHz/8 = 2Mhz or 0.5us
  //The /8 prescale gives us a good range to work with
  //so we just hard code this for now.
  TCCR2A = 0;
  TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 = 1<<TOIE2;

  //load the timer
  TCNT2=0;
}


void setup()                    // run once, when the sketch starts
{
  for (i = 0; i < NUMROW; i++) pinMode(ROW[i], OUTPUT);
  for (i = 0; i < NUMCOL; i++) pinMode(COL[i], OUTPUT);
  
  for (i = 0; i < NUMROW; i++) digitalWrite(ROW[i], HIGH);
  for (i = 0; i < NUMCOL; i++) digitalWrite(COL[i], LOW);
  
  SetupTimer2();
}

void loop()                     // run over and over again
{ 
  if (oldsecond != second)
  {
    seconds = second;
    if (second > 59)
    {
      second = 0;
      minutes++;
      if (minutes > 59)
      {
        minutes = 0;
        hours++;
        if (hours > 23)
        {
          hours = 0;
        }
      }
    }
    seconds = second;
    oldsecond = second;
  }
  
  int i;
  for (i = 0; i < 6; i++) {
    if (seconds >> i & 1) {
      showXY(0, i);
    }
  }
  for (i = 0; i < 6; i++) {
    if (minutes >> i & 1) {
      showXY(1, i);
    }
  }
  for (i = 0; i < 5; i++) {
    if (hours >> i & 1) {
      showXY(2, i);
    }
  }
}

void showXY(int col, int row)
{
  digitalWrite(ROW[old_row], HIGH);
  digitalWrite(COL[old_col], LOW);
  
  digitalWrite(ROW[row], LOW);
  digitalWrite(COL[col], HIGH);
  
  old_row = row;
  old_col = col;
}
