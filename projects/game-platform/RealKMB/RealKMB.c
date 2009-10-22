/********************************************************************
 *
 * KMB firmware
 * --------------
 * 
 * (copyright) 2009 Tomas Fedosejev
 *
 * @author              : Tomas Fedosejev (tomas@fedosejev.lt)
 * @date                : 2009-10-12
 * @hardware            : Tomas Fedosejev
 * @project             : KMB (Kick My Balls)
 * @input               : 6 buttons (4 way jostoc, 2 action btns)
 * @output              : 8x8 LED matrix display, 2 info LED's
 * @chip type           : ATmega8-16PU
 * @program type        : Firmware
 * @clock frequency     : 8,000000 MHz
 * @memory model        : Small
 * @external RAM size   : 0
 *
 ********************************************************************/

#include <avr/io.h>
#include <avr/iom8.h>
#include <util/delay.h>
#include "pin_macros.h"
#include "74HC595.h"

#define LED1 PINC5
#define LED2 PINC3
#define SOUND PINC4


void mysleep( void ){
    PORTC |= (1<<LED1); 
    PORTC &= ~(1<<LED2);
	  PORTC &= ~(1<<LED1);
		PORTC |= (1<<LED2); 
		_delay_ms(2);
}

int level=0, count=0;

unsigned int head_x = 0b00011100, head_2 = 0b00111000, ball_x = 0b00001000, 
             ball_x_old = 0b00000000, ball_y = 1, ball_y_old = 0, demo = 0;

const unsigned int text[32]={
  //D
  0b00000000,
  0b01111000,
  0b01000100,
  0b01000100,
  0b01000100,
  0b01000100,
  0b01111000,
  0b00000000,

  //E
  0b00000000,
  0b01111100,
  0b01000000,
  0b01000000,
  0b01111000,
  0b01000000,
  0b01111100,
  0b00000000,

  //A
  0b00000000,
  0b01000100,
  0b01000100,
  0b01111100,
  0b01000100,
  0b00101000,
  0b00010000,
  0b00000000

};

const unsigned int countdown[32]={
  
  //3
  0b00000000,
  0b00011000,
  0b00100100,
  0b00100100,
  0b00001000,
  0b00100100,
  0b00100100,
  0b00011000,
  
  //2
  0b00000000,
  0b00111100,
  0b00100100,
  0b00010000,
  0b00001000,
  0b00100100,
  0b00100100,
  0b00011000,

  //1
  0b00000000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00101000,
  0b00011000,
  0b00001000,

  //GO
  0b00000000,
  0b11110111,
  0b10010101,
  0b10110101,
  0b10000101,
  0b10010101,
  0b11110111,
  0b00000000
};
 

void checkButtons( void ){

  //DESINE
  if (bit_is_clear(PINB, 2)){
    _delay_us(20);
    if (bit_is_clear(PINB, 2)){
      if(head_x == 0b00000111){
        head_x = 0b00000111;
        //c5_high;
      }
      else{
        head_x = (head_x>>1);
      }
      c3_high;
    }
  }

  //KAIRE
  if (bit_is_clear(PINB, 1)){
    _delay_us(20);
    if (bit_is_clear(PINB, 1)){
      //ShowAction = 8;
      if(head_x == 0b11100000){
        head_x = 0b11100000;
      }
      else{
        head_x = (head_x<<1);
      }
      c3_high;
    }
  }

  //AUKSTYN
  if (bit_is_clear(PINB, 4)){
    _delay_us(20);
    if (bit_is_clear(PINB, 4)){
      demo = 1;
      c3_high;
    }
  }

  //ZEMYN
  if (bit_is_clear(PINB, 3)){
    _delay_us(20);
    if (bit_is_clear(PINB, 3)){
      demo = 0;
      c3_high;
    }
  }

  //ACTION1
  if (bit_is_clear(PINB, 5)){
    _delay_us(20);
    if (bit_is_clear(PINB, 5)){
      //b -= 200;
      //head_x = 0b11100000;
      c3_high;
    }
  }
  
  //ACTION2
  if (bit_is_clear(PINB, 0)){
    _delay_us(20);
    if (bit_is_clear(PINB, 0)){
      //b += 200;
      c3_high;
    }
  }
}

void moveBall( void ){
  /*if(ball_x == ball_x_old){
    ball_x = (ball_x>>1);
  }*/
  //pradzia
  if (ball_x_old == 0b00000000){
    ball_x_old = 0b00001000;
    ball_x = 0b00000100;
  }
  //virsutinis arba apatinis kairysis kampas
  else if((ball_x == 0b10000000) && (ball_x_old == 0b01000000)){
      ball_x = 0b01000000;
      ball_x_old = 0b10000000;
  }
  //virsutinis arba apatinis desinysis kampas
  else if((ball_x == 0b00000001) && (ball_x_old == 0b00000010)){
      ball_x = 0b00000010;
      ball_x_old = 0b00000001;
  }
  else if((ball_x+ball_x_old) == (ball_x+(ball_x<<1))){
    ball_x_old = ball_x;
    ball_x = (ball_x >> 1);
  }
  else if((ball_x+ball_x_old) == (ball_x+(ball_x>>1))){
    ball_x_old = ball_x;
    ball_x = (ball_x << 1);
  }
  
  //kamuoliuko vieta
  if((ball_y > ball_y_old) && (ball_y < 6)){
    ball_y++;
    ball_y_old++;
  }
  else if((ball_y < ball_y_old) && (ball_y > 1)){
    ball_y--;
    ball_y_old--;        
  }
  else if(ball_y==6){
    c5_high;
    ball_y--;
    ball_y_old++;
  }
  else if(ball_y==1){
    if((head_x|ball_x) == head_x){
      c5_high;
      ball_y++;
      ball_y_old--;
      if(count == 10){
        level++;
        count=1;
      }
    }
    else{
      level=0;
      count=0;
      ball_y = 1;
      ball_y_old = 0;
      ball_x = 0b00001000;
      ball_x_old = 0b00000000;
      head_x = 0b00011100;
      _delay_ms(1000);
    }
  }


}

void moveOpponent( void ){
  head_2 = (ball_x<<1)|(ball_x>>1)|ball_x;
  if (head_2 == 0b11000000){
    head_2 = 0b11100000;
  }
  else if (head_2 == 0b00000011){
    head_2 = 0b00000111;
  }
  if(demo == 1){
    head_x = head_2;
  }
}

void scrollText(int message){
  unsigned int buffer[8], tmp[8];
  unsigned int var = 0, i, k=0;

  for(i=0; i<8; i++){
    buffer[i] = text[i];  
    tmp[i] = text[i+8];
  }

  while (var <= 7){
    for (i = 0; i<8; i++){
      c5_high;
      PORTD = (1<<i);
      HCsetdata(buffer[i]);
      HCloop();
      _delay_ms(1);
      mysleep();
    }
    //_delay_ms(1);
    
    var++;
    
    for (i = 0; i<8; i++){
      buffer[i] = (buffer[i]<<1);
      //for (b=0; var)
      //tmp = text[i];

      //for 

      if((tmp[i]&01)==0){
        //buffer[i] = (buffer[i]+0b00000001);  
      }
      else{
        //buffer[i] = (buffer[i]-0b11111110);  
      }
      buffer[i] = (buffer[i]|(tmp[i]&01));
      tmp[i] = (tmp[i]<<1);
    }

    if (var < 8){
      k=0;
    }
    else{
      k=1;
    }
    c5_low;
    _delay_ms(5);
  }
}

int main ( void ){
  unsigned int k, i;
  
  //zaidimas su registrais, jie abu output'ai :)
  DDRC = 0xFF;
  DDRD = 0xFF;

  //nustatom visus mygtukus kaip inputus
  b0_input;
  b1_input;
  b2_input;
  b3_input;
  b4_input;
  b5_input;

  //kad visi spetu pamatyt countdown'a
  _delay_ms(1000);

  scrollText(0);

  //pradedam countdown'a
  k = 0;  
  //b = 1500;
  while (k <251) {
    //3
    if(k < 50){
      for (i = 0; i<8; i++){
        c5_low;
        PORTD  = (1<<i);
        HCsetdata(countdown[i]);
        HCloop();
        mysleep();
      } 
    }
    
    //2
    else if(k < 100){
     for (i = 8; i<16; i++){
        c5_high;
        PORTD  = (1<<(i-8));
        HCsetdata(countdown[i]);
        HCloop();
        mysleep();
      } 
    }

    //1
    else if(k < 150){
     for (i = 16; i<24; i++){
        c5_low;
        PORTD  = (1<<(i-16));
        HCsetdata(countdown[i]);
        HCloop();
        mysleep();
      } 
    }

    //GO
    else if(k < 250){
     for (i = 24; i<32; i++){
        c5_high;
        PORTD  = (1<<(i-24));
        HCsetdata(countdown[i]);
        HCloop();
        mysleep();
      } 
    }
    k++;
  }

  c5_low;
  c3_low;
  k = 0;
  //b = 1500;
  while (1) {
    checkButtons();
    moveBall();
    moveOpponent();
    for(i=0; i<10; i++){
      for(k=0; k<8; k++){
        PORTD  = (1<<k);
        if(ball_y == k){
          HCsetdata(ball_x);
        }
        else if(k == 0){
          HCsetdata(head_x);
        }
        else if(k == 7){
          HCsetdata(head_2);
        }
        else{
          HCsetdata(0b00000000);
        }
        HCloop();
        if(level==0){
          _delay_us(1500);
        }
        else if(level==1){
          _delay_us(1000);
        }
        else if (level == 2){
          _delay_us(700);
        }
        else if (level == 3){
          _delay_us(300);
        }
        else{
          //_delay_us()
        }

      }
      c3_low;
    }
    c5_low;
  }	
}
