#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>




/**************** Globale variable **************/
static enum {av1,paa1} servo1=paa1;
static enum {av2,paa2} servo2=av2;
unsigned char pos1=0;
unsigned char pos2=0;


//********************* timer *****************/
typedef struct {
	unsigned long teller;
	char flag;
	} timer_type;

timer_type T1,servo1_timer,servo2_timer;


/***************** interrupt *******************/

SIGNAL(SIG_OUTPUT_COMPARE1A){
	OCR1A+=800;
        
	if(T1.teller !=0){
		T1.teller--;

		
		if((T1.teller==0))
		T1.flag=1;		
		}


	if(servo1_timer.teller !=0){
		servo1_timer.teller--;	
		}//endif

	if((servo1_timer.teller==0)){
	
			switch(servo1){
		  
				case paa1:
					servo1=av1;
					servo1_timer.teller = 200;
				    PORTC&=(char)(~1);
					break;
				case av1 :
					servo1=paa1;
					servo1_timer.teller = (pos1);
					PORTC|=1;
					break;
					}//endswitch
				}//endif	
	
			

	if(servo2_timer.teller !=0){
		servo2_timer.teller--;	
			}//endif
		
		if((servo2_timer.teller==0)){
		
				switch(servo2){
			  
					case paa2:
						servo2=av2;
						servo2_timer.teller = 200;
					    PORTC&=(char)(~2);
						break;
					case av2 :
						servo2=paa2;
						servo2_timer.teller = (pos2);
						PORTC|=2;
						break;
						}//endswitch
					}//endif	
		


}//endsignal
/***************************************/
void waitNms(micro){
	T1.teller = micro;
	T1.flag=0;
	while(T1.flag==0){}
	return;
}


//**************************************************
void init_timer1(void){
	TCCR1A=0;
	TCCR1B=(1<<CS10);//|(1<<WGM12); //ingen prescaling,clr TCNT1 on compare match
	TCNT1=0;//teller fra null første gang
	OCR1A=800; //->avbrudd hvert 1/10ms ved 8MHz
	TIMSK=(1<<OCIE1A);//interrupt enable
	sei();//global interrupt enable
}



/************* I/O ***************************/
void ledwrite(N){
	
	PORTB = ~N;
	}


void init_porter(void){
	//ledwrite:
	DDRB=0xFF;//port b ut
	PORTB=0xFF;//skriver null på lampene
	//switch_read:
	DDRA = 0; // port a inn
	DDRC=0xff;//signal servo
	PORTC=0;

}

unsigned char switchread(void){
	
	static char N=0;
	N=~PINA;
	return N;
}

/*********************** MAIN ******************/
 void main(void){
	init_porter();
	init_timer1();
	sei();
	pos1=15;
	pos2=15;





	while(1){
	ledwrite(pos2);

	//servo 1 CW

	if((switchread()&1) !=0){
	pos1++;
	waitNms(3000);
	
		if(pos1>20){
		pos1=15;
		}
	}
	//senter
	if((switchread()&2) !=0){
	pos1=15;
	waitNms(3000);
	}
	//CCW
	if((switchread()&4) !=0){
	pos1--;
	waitNms(3000);
	
		if(pos1<10){
		pos1=15;
		}
	}
	
	//Servo 2 CW :	
	if((switchread()&8) !=0){
	pos2++;
	waitNms(3000);
	
		if(pos2>20){
		pos2=15;
		}
	}
	//midt
	if((switchread()&16) !=0){
	pos2=15;
	waitNms(3000);
	}
	// CCW
	if((switchread()&32) !=0){
	pos2--;
	waitNms(3000);
	
		if(pos2<10){
		pos2=15;
		}
	}
	}//endwhile

}//end main
