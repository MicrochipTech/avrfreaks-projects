/************************************************************* 
 *
 * Shift Out Data
 * --------------
 * 
 * Shows a byte, stored in "dato" on a set of 8 LEDs
 *
 * (copyright) 2009 Tomas Fedosejev
 * @author: Tomas Fedosejev (tomas@fedosejev.lt)
 * @date: 2009-10-12
 * @hardware: Tomas Fedosejev
 * @project: KMB (Kick My Balls)
 *
 *************************************************************/

#define timeout 10

int dato = 0;

void HCoutput( void ){
  c1_high;
  _delay_us(timeout);
  c1_low;
  _delay_us(timeout);
  c4_low;
}


void HCsetup(){
  c0_output;
  c1_output;
  c2_output;
  c4_output;
}


void HCsetdata(int dt){
  dato = dt;
}


void PulseClock(void){
  c0_high;
  _delay_us(timeout);
  c0_low;
  _delay_us(timeout);
}


void HCshiftbite(unsigned int b){
  c4_high;
  c0_low;
  if(b == 1){
    c2_high; 
  }
  else{
    c2_low;
  }
  PulseClock();
}


void HCloop( void ){
  int count = 0;
  //dato = 5;
  for (count = 0; count < 8; count++) {
    c4_high;
    c0_low;
    if((dato & 01) == 0){
      c2_high;
    }
    else{
      c2_low;
    }
    dato>>=1;
    if (count == 7){
      c4_low;
    }
    PulseClock();
  }

  HCoutput();
  _delay_us(timeout);                  // waits for a moment
}

