/* There is an high possibility thet the whole app will hang, if there's 
something wrong with the wire, or the add is wrong, or a device is down.
It might make sense to implement some kind of timer, so the whole app doesn't 
hang*/

#include "i2c.h"
#include "../uart/uart.h"
#include <avr/io.h>

// define i2c_debug

// Mask the TWSR
#define TWSRM (TWSR & 0xF8)

uint8_t I2C_flags;
#define init 1
#define control 2



unsigned char I2C_Init(uint32_t SCL_Speed){
	//Calc Busspeed - Not nice
	uint8_t i;
	uint32_t RelSpeed;
	RelSpeed = (F_CPU / SCL_Speed) - 16;
	RelSpeed = RelSpeed / 8;
	uart_puti(RelSpeed, 0);
	uart_puts_P("\r\n");
	i = 1;
	while((uint32_t)(RelSpeed / i) > 256){
		i *= 4;
	}
	uart_puti(i, 0);
	uart_puts_P("\r\n");
	switch(i){
		case(1): {cbi(TWSR, TWPS1); cbi(TWSR, TWPS0);}
		case(4): {cbi(TWSR, TWPS1); sbi(TWSR, TWPS0);}
		case(16): {sbi(TWSR, TWPS1); cbi(TWSR, TWPS0);}
		default: {sbi(TWSR, TWPS1); sbi(TWSR, TWPS0);}
	}
	

	TWBR = (uint8_t)(RelSpeed / i);
	uart_puti(TWBR, 0);
	uart_puts_P("\r\n");
	
	
	sbi(TWCR, TWEN); //enable
	sbi(I2C_flags, init);
	
	return 1;
}

unsigned char I2C_Start(void){
	
	if(!bit_is_set(I2C_flags, init)) return -1;
	//loop_until_bit_is_clear(TWCR, TWINT); //ensure that no data is corrupted
	/*sbi(TWCR, TWSTA); //Request Start
	sbi(TWCR, TWINT); //Reset Int.*/
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT))); /*Here the software may hang up, if there is some Problem 
	with the I2c - Some kind of Timeout meight be interesting*/
	
	return TWSRM;

}


unsigned char I2C_Stop(void){
	//loop_until_bit_is_clear(TWCR, TWINT); //ensure that no data is corrupted
	/*sbi(TWCR, TWSTO); //set stop
	sbi(TWCR, TWINT); //start stop*/
	TWCR = (1<<TWINT) | (1<<TWSTO);
	while (!(TWCR & (1<<TWINT)));
	return TWSRM;
}

unsigned char I2C_Write(unsigned char Data){
	//loop_until_bit_is_clear(TWCR, TWINT); //ensure that no data is corrupted
#ifdef i2c_debug
	uart_puts_P("D:");
	uart_puti(Data,0);
#endif
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);//sbi(TWCR,TWINT);
	while (!(TWCR & (1<<TWINT)));
#ifdef i2c_debug
	uart_puts_P("W:");
	uart_puti(TWSRM,0);
	uart_puts_P(";");
#endif
	return TWSRM;
}

unsigned char I2C_Read(unsigned char *data, int8_t Stop){
#ifdef i2c_debug	
	uart_puts_P("R");
#endif
	//loop_until_bit_is_clear(TWCR, TWINT); //ensure that no data is corrupted
	/*if (Stop == 1) sbi(TWCR, TWSTO);
	else cbi(TWCR, TWSTO);
	cbi(TWCR, TWSTA);*/
	if (Stop == 1) {
		TWCR = (1<<TWINT) | (1<<TWEN) ;
	} else{
		TWCR = (1<<TWINT) | (1<<TWEN) | (1 << TWEA);
	}
	while (!(TWCR & (1<<TWINT)));
#ifdef i2c_debug	
	uart_puti(TWSRM,0);
	uart_puts_P(";");
	uart_puti(TWDR,0);
	uart_puts_P(";");
#endif
	*data = TWDR;
	return TWSRM;
}



unsigned char I2C_Readdata(unsigned char add,unsigned char *data, uint8_t length){
	unsigned char tmp;
	uint8_t i;
	if (length == 0) return -1; // add Error
	tmp = I2C_Start();
	if((tmp != 0x08) & (tmp != 0x10)) {
		return tmp;
	}

	sbi(add, 0); //read
	if ((tmp = I2C_Write(add)) != 0x40){
		return tmp;
	}	

	length --;
	for(i = 0; i < length; i++){
		if ((tmp = I2C_Read(&data[i], 0)) != 0x50){
			return tmp;
		}
	}
	if((tmp = I2C_Read(&data[i],1)) != 0x58){
		return tmp;
	}

	return 1;
}

unsigned char I2C_Readdata1(unsigned char add, unsigned char *data){
	return   I2C_Readdata(add, data, 1);
}


unsigned char I2C_Writedata (unsigned char add,unsigned char *data, uint8_t length){
	unsigned char tmp;
	uint8_t i;
	if (length == 0) return -1 ;// add Error
	cbi(add, 0); //write
	tmp = I2C_Start();
	if((tmp != 0x08) & (tmp != 0x10)){
		return tmp;
	}

	
	if ((tmp = I2C_Write(add)) != 0x18 ){
		return tmp;
	}

	for(i = 0; i < length; i++){
		if ((tmp = I2C_Write(data[i])) != 0x28){
			return tmp;
		}
	}

	return 1;
}
 
unsigned char I2C_Writedata1(unsigned char add, unsigned char data){
	return I2C_Writedata(add, &data, 1);
 }
