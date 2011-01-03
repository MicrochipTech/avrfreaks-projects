//AVR-GCC application builder : 01/02/2010 14:46:46
// Target : atmega32
// Crystal: 16.0000Mhz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

//#include <macros.h>

#define PWMMAX 97
#define PWMMIN 	   3


#define Y1							   0
#define X1							   1
#define Y2							   2
#define X2							   3
#define MOTEUR_ON 					   2



unsigned char state=0;
unsigned char etat1=0;
unsigned char etat2=0;
unsigned char etat3=0;
unsigned char etat4=0;
unsigned char pwm_consigne = 50;
unsigned char pwm_current =0;
unsigned char pwm_rate=0x83;
unsigned char last_button=0x0C;
unsigned char comptX=0x01;
unsigned char comptY=0x01;
unsigned char comptX1=0x01;
unsigned char comptY1=0x01;




void port_init(void)
{
 DDRA  = 0xFF;		// sortie moteur x et y
 PORTA = 0xCC;		// moteurs position 1
 PORTB = 0X1F;		// pullup en entrée
 DDRB  = 0xE0;		// sortie en partie haute et entrée en partie basse
 PORTB = 0x1F;
 DDRC  = 0xFF;		// sortie moteur x1 et y1
 PORTC = 0xCC;		// moteurs position 1
 PORTD = 0x00;		// pull up off
 DDRD  = 0x00;		// entrée moteur x,x1,y,y1 
}


//TIMER0 initialize - prescale:8
// desired value: 8KHz
// actual value:  8.000KHz (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop timer
 TCNT0 = 0xFA; //set count value(valeur 250)
 TCCR0 = 0x02; //start timer
}

//#pragma interrupt_handler timer0_ovf_isr:8

ISR (TIMER0_OVF_vect)
{

 	 					  				  								// generation du signal a 4kHz et gestion des modes de chauffe
 switch (state){														// verification de la phase dans laquelle nous sommes
 		case 0:															// nous sommes a l'etat bas
			   	 	  PORTB |= 0x20;									// passage du bit 5 (pin 10 de la db25) au niveau haut
			   	 	  state=1;
					  if (bit_is_set(PINB,4)){ 						// test du bit 4, si 0 chauffe manu
    					  pwm_current ++;								// increment de l'index du pwm
    					  if (pwm_current == 100)  {					// si on arrive a 100		
    					  	 pwm_current = 0;	   						// on remet l'index a 0 pour demarer un nouveau cycle de pwm
    						 PORTB |= 0x40;								// et on monte le bit 6 (vers l'opto et pin 11 de la db 25)
    					  }
    					  if (pwm_current == pwm_consigne){  			// l'index du pwm = consigne, 
    					  	 PORTB &= 0xBF;	 				 			// il faut passer a 0 le bit 6
    					  }
						  if ( (PINB&0x03)  != last_button ){			// verification de l'etat des boutons (bits 0 et 1), on t-ils changes ?
						  	 if (last_button == 0x03) {	   		   							// oui, l'etat precedent etait un niveau haut, donc on vient d'appuyer un bouton
							 		if ((PINB & 0x01) == 0x00 && pwm_consigne < PWMMAX) 	// a-t-on appuyer sur le + et la consigne est elle < au max
									   pwm_consigne += 1;	  	 			  				// oui, on augmente la consigne
									 if ((PINB & 0x02) == 0x00 && pwm_consigne > PWMMIN)   // a-t-on appuyer sur le - et la consigne est-elle > au min
									 	pwm_consigne -= 1;     	  			   				// oui, on decremente la consigne
							 }				
						  }
						 last_button = PINB&0x03;	 						   			// quoiqu'il arrive, memoriation de l'etat des boutons
					  }
					  else {  		   										   	  		//chauffe PC
					  	   if (bit_is_set(PINB,3))  						   		// lecture du bit 3 (pin 16 de la db25) pour lire l'etat du pwm fournit par le pc			
						   	  	PORTB |= 0x40;								   	  		// niveau haut, on met le bit 7 au niveau haut
							else 
								PORTB &=0xBF;   										// niveau bas, on met le bit 7 au niveau bas	  
					  }
			   	 	  break;
		case 1:		  				  												   // on est dans la phase haute du signal a 4kHz
			   		  PORTB &= 0xDF;												   // mise a 0 du bit 5 (pin 10 de la db25)
			   		  state=0; 														   // memorisation de l'etat
			   		  break;
 }
 
 TCNT0 = pwm_rate; //reload counter value
}



//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 port_init();
 timer0_init();

 MCUCR = 0x00;
 TIMSK = 0x01;	// timer0 interruption marche
 wdt_disable();
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}


int main(void)
{
init_devices();

 while (1)	{
 			if (bit_is_set(PINB,MOTEUR_ON)){										//boucle
				//X AXIS
			switch (etat1){  
				case 0 : 
				if(bit_is_set(PIND,0)) etat1 = 1;		//test bit avance X
				break;
				case 1 :
					if(bit_is_clear(PIND, 0)){		 // attendre la mise à 0 pour validation
					etat1 = 0;
					if(bit_is_set(PIND, 1))			 	// test du bit de direction
					XMoveStepForward();
					else
					XMoveStepReverse();
					}
					}
				
				//Y AXIS
			switch (etat2){  
				case 0 : 
				if(bit_is_set(PIND,2)) etat2 = 1;		//test bit avance X
				break;
				case 1 :
					if(bit_is_clear(PIND, 2)){		 // attendre la mise à 0 pour validation
					etat2 = 0;
					if(bit_is_set(PIND, 3))			 	// test du bit de direction
					YMoveStepForward();
					else
					YMoveStepReverse();
					}
					}
				//X1 AXIS
			switch (etat3){  
				case 0 : 
				if(bit_is_set(PIND,4)) etat3 = 1;		//test bit avance X
				break;
				case 1 :
					if(bit_is_clear(PIND, 4)){		 // attendre la mise à 0 pour validation
					etat3 = 0;
					if(bit_is_set(PIND, 5))			 	// test du bit de direction
					X1MoveStepForward();
					else
					X1MoveStepReverse();
					}
					}
				//Y1 AXIS
			switch (etat4){  
				case 0 : 
				if(bit_is_set(PIND,6)) etat4 = 1;		//test bit avance X
				break;
				case 1 :
					if(bit_is_clear(PIND, 6)){		 // attendre la mise à 0 pour validation
					etat4 = 0;
					if(bit_is_set(PIND, 7))			 	// test du bit de direction
					Y1MoveStepForward();
					else
					Y1MoveStepReverse();
					}
					}
				}

			}
			
}
//----------------------------------------------------------------------------
//	avance moteur X
//----------------------------------------------------------------------------
int XMoveStepForward(void)
{
	comptX++;								// incrémente le compteur de pas.

	if (comptX == 4)comptX = 0;			// Pas plus de 4 pas.
	
	switch (comptX){						// test quel pas nous devous envoyer?
	
			case (0): PORTA = ((PORTA & 0xf0) ^ 0x09);		//envoi des pas sur le port.
			
			break;
			
			case (1): PORTA = ((PORTA & 0xf0) ^ 0x0c);
			
			break;
			
			case (2): PORTA = ((PORTA & 0xf0) ^ 0x06);
			
			break;
			
			case (3): PORTA = ((PORTA & 0xf0) ^ 0x03);
			
			break;

			default : comptX = 0x01;
			}
return 1 ;
}

//----------------------------------------------------------------------------
//	recule moteur X
//----------------------------------------------------------------------------
int XMoveStepReverse(void)
{
	comptX--;
	
	switch (comptX){
	
			case (0): PORTA = ((PORTA & 0xf0) ^ 0x09);
			
			break;
			
			case (1): PORTA = ((PORTA & 0xf0) ^ 0x0c);
			
			break;
			
			case (2): PORTA = ((PORTA & 0xf0) ^ 0x06);
			
			break;
			
			case (3): PORTA = ((PORTA & 0xf0) ^ 0x03);
			
			break;

			default : comptX = 0x01;
			}

			if (comptX == 0)comptX = 4;

			return 1;
}

//----------------------------------------------------------------------------
//	avance moteur Y
//----------------------------------------------------------------------------
int YMoveStepForward(void)
{
	comptY++;

	if (comptY == 4)comptY = 0;
	
	switch (comptY){
	
			case (0): PORTA = ((PORTA & 0x0f) ^ 0x90);
			
			break;
			
			case (1): PORTA = ((PORTA & 0x0f) ^ 0xc0);
			
			break;
			
			case (2): PORTA = ((PORTA & 0x0f) ^ 0x60);
			
			break;
			
			case (3): PORTA = ((PORTA & 0x0f) ^ 0x30); 
			
			break;

			default : comptY = 0x01;
			}
			return 1;
}

//----------------------------------------------------------------------------
//	recule moteur Y
//----------------------------------------------------------------------------
int YMoveStepReverse(void)
{
	comptY--;
	
	switch (comptY){
	
			case (0): PORTA = ((PORTA & 0x0f) ^ 0x90);
			
			break;
			
			case (1): PORTA = ((PORTA & 0x0f) ^ 0xc0);
			
			break;
			
			case (2): PORTA = ((PORTA & 0x0f) ^ 0x60);
			
			break;
			
			case (3): PORTA = ((PORTA & 0x0f) ^ 0x30);
			
			break;

			default : comptY = 0x01;
			}

			if (comptY == 0)comptY = 4;

			return 1;  
}

//----------------------------------------------------------------------------
//	avance moteur X1
//----------------------------------------------------------------------------
int X1MoveStepForward(void)
{
	comptX1++;

	if (comptX1 == 4)comptX1 = 0;
	
	switch (comptX1){
	
			case (0): PORTC = ((PORTC & 0x0f) ^ 0x90);
			
			break;
			
			case (1): PORTC = ((PORTC & 0x0f) ^ 0xc0);
			
			break;
			
			case (2): PORTC = ((PORTC & 0x0f) ^ 0x60);
			
			break;
			
			case (3): PORTC = ((PORTC & 0x0f) ^ 0x30);
			
			break;

			default : comptX1 = 0x01;
			}
	return 1;

}

//----------------------------------------------------------------------------
//	recule moteur X1
//----------------------------------------------------------------------------
int X1MoveStepReverse(void)
{
	comptX1--;
	
	switch (comptX1){
	
			case (0): PORTC = ((PORTC & 0x0f) ^ 0x90);
			
			break;
			
			case (1): PORTC = ((PORTC & 0x0f) ^ 0xc0);
			
			break;
			
			case (2): PORTC = ((PORTC & 0x0f) ^ 0x60);
			
			break;
			
			case (3): PORTC = ((PORTC & 0x0f) ^ 0x30);
			
			break;

			default : comptX1 = 0x01;
			}

			if (comptX1 == 0)comptX1 = 4;

			return 1;
}

//----------------------------------------------------------------------------
//	avance moteur Y1
//----------------------------------------------------------------------------
int Y1MoveStepForward(void)
{
	comptY1++;

	if (comptY1 == 4)comptY1 = 0;
	
	switch (comptY1){
	
			case (0): PORTC = ((PORTC & 0xf0) ^ 0x09);
			
			break;
			
			case (1): PORTC = ((PORTC & 0xf0) ^ 0x0c);
			
			break;
			
			case (2): PORTC = ((PORTC & 0xf0) ^ 0x06);
			
			break;
			
			case (3): PORTC = ((PORTC & 0xf0) ^ 0x03);
			
			break;

			default : comptY1 = 0x01;
			}
			return 1;
}

//----------------------------------------------------------------------------
//	recule moteur Y1
//----------------------------------------------------------------------------
int Y1MoveStepReverse(void)
{
	comptY1--;
	
	switch (comptY1){
	
			case (0): PORTC = ((PORTC & 0xf0) ^ 0x09);
						
			break;
			
			case (1): PORTC = ((PORTC & 0xf0) ^ 0x0c);
			
			break;
			
			case (2): PORTC = ((PORTC & 0xf0) ^ 0x06);
			
			break;
			
			case (3): PORTC = ((PORTC & 0xf0) ^ 0x03);
			
			break;

			default : comptY1 = 0x01;
			}

			if (comptY1 == 0)comptY1 = 4;

			return 1;
}

