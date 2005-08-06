//  Author:   Steve Duckworth, with contributions from Andy Gayne.
//  Customer: GD Technik internal
//  Target:   Tiny26 (or any other SRAM enabled AVR).
//  Project:  Very small task scheduler for Atmel Tiny 26.
//  Date:     12/02/2003 - 08/09/2003.
//  Notes:    Array based with limit on maximum number of tasks allowed. 
//            Demonstration based on STK500 and ATtiny26.
//            Connect LEDs to PORTB and switches to PORTA.

#include <stdlib.h>
#include <iotiny26.h>

#define led0      PORTB_Bit0    // led0 on STK500 
#define led1      PORTB_Bit1    // led1 on STK500  
#define led2      PORTB_Bit2    // led2 on STK500  
#define led3      PORTB_Bit3    // led3 on STK500  
#define sw0       PINA_Bit0     // sw0 on STK500
#define sw1       PINA_Bit1     // sw1 on STK500
#define sw2       PINA_Bit2     // sw2 on STK500
#define sw3       PINA_Bit3     // sw3 on STK500
#define ON        0             // for LED control
#define OFF       1             // for LED control
#define maxtasks  3             // max tasks allowed on queue.

unsigned char tasks = 0;    // keep count of number of active tasks.
void(*x[maxtasks+1])(void); // task array, with space for null terminator.

typedef void(*fp)(void);    // function pointer typedef

// prototypes
unsigned  char  FindTask    (void(*fp)(void));
void            CreateTask  (void(*task)(void));
void            DeleteTask  (void(*fp)(void));
void            task0       (void);
void            task1       (void);
void            task2       (void);
void            task3       (void);
void            task4       (void);
void            task5       (void);

// scan array to see if task exists, if not then add task and
// shift NULL pointer to end.
void  CreateTask(void(*task)(void)){
  unsigned char n;

  if(!FindTask(task)){    // check task is not present
    for(n=0;n<maxtasks || x[n]!=NULL ;n++ ) // find position of null terminator
      if (x[n] == NULL){
        x[n] = task;      // add task to queue
        x[++n] = NULL;    // add new null terminator
        return;
        }// end if
  }// end if
}

// deletes task if found.
void DeleteTask(void(*task)(void))
{
  unsigned  char  n;

  for(n=0;n<maxtasks;n++)
    if(x[n] == task){             // task found.
      do{
         x[n] = x[n+1];           // shuffle tasks down in array
         n++;                     // inc index
      }while(x[n] != NULL);       // keep shuffle going until NULL
      return;
    }// end if
}

// checks to see if a task already exists on the queue... returns 0 if 
// task not found, else returns array index of task if found.
unsigned char FindTask(void(*fp)(void)){
  unsigned  char  n;

  for(n=0;n<maxtasks || x[n] != NULL;n++)
    if(x[n] == fp) return 1;
  return 0;
}

// adds and deletes task1 if sw0 or sw1 pressed respectively
void task0(void){
  if (!sw0){            //if sw0 pressed
    CreateTask(task1);
    led0=ON;
    led1=OFF;
  }
  else if (!sw1){       //if sw1 pressed
    DeleteTask(task1);
    led0=OFF;
    led1=ON;
  }
}

// if active, turns led3 on if sw3 pressed
void task1(void){
  if (!sw3) led3=ON;
  else led3=OFF;
}

void task2(void){}
void task3(void){}
void task4(void){}
void task5(void){}


void main(void)
{
  unsigned char n = 0;

  DDRB = 0xff;                  // PORT B is outputs
  DDRA = 0x00;                  // PORT A is inputs
  PORTB = 0xff;                 // all LEDs off
  
  x[0] = NULL; // always NULL the first array element before we use it.
 
  CreateTask(task0);            // add task zero to queue (must have one task)
 
  while(1){                     // loop forever
    while(x[n]!=NULL) x[n++](); // execute tasks.
    n = 0;                      // reset index.
  }
}
