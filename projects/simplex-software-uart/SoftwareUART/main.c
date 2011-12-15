#include "SoftwareUART.h"

int main(){
	
	init_suart();
	sTX_putstring("This is a test at 4,8 Mhz with 9600 8N1");	
	//Use nice example to fill half of tiny13	

	while(1){

	}
}
