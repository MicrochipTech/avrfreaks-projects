 /*
 
 PB0-PB7 - lcd data port
 PA0-PA2 - lcd control bits
 SDA/SCL - TWI (I2C) bus
 
 */
 
 #define  F_CPU 8000000
 #include <avr/io.h>
 #include <util/delay.h>

 #define lcd_E 2						//lcd control bits
 #define lcd_RW 1
 #define lcd_RS 0

 #define SLA_W 0b10010000				//DS1621 write
 #define SLA_R 0b10010001				//DS1621 read

 unsigned char th;						//temperature high byte
 
 int main(void)
 {
	  init();
	  
	  unsigned char x=0,line1[]="temp: ";
	  	  
	  while(line1[x]!='\0')				//display "temp: " on lcd
	  {
		  lcd_data(line1[x++]);
	  }
	  
	  while(1)
	  {
		  ds1621_convert();				//convert temperature
		  _delay_ms(1000);				//needs about 750ms
		  ds1621_readtemp();			//get reading
		  display_temp();				//display temperature
	  }	 
 }
 
 void init(void)
 {
	 DDRA = 0x07;						//lcd control port
	 PORTA= 0x07;						//pull-ups enable
	 DDRB = 0xff;						//lcd data port
	 PORTB= 0xff;						//pull-ups enable
	 PORTC = 0x03;						//TWI pull-ups enable
	 
	 TWBR = 0x20;						//scl = 8000000/16+2*32*1 = 100khz
										//TWSR default 0, pre-scaler = 1 
	 lcd_command(0x38);					//8bit, 2lines, 5*7
	 lcd_command(0x0C);					//no cursor
	 lcd_command(0x01);					//clear screen
	 lcd_command(0x80);					//cursor home
 }

 void lcd_data(unsigned char x)
 {
	 lcd_wait();
	 
	 PORTB = x;								//data to the port
	 PORTA &= ~((1<<lcd_RW)|(1<<lcd_E));	//read data
	 PORTA |= ((1<<lcd_RS)|(1<<lcd_E));		//strobe E
	 _delay_ms(1);
	 PORTA &= ~((1<<lcd_RS)|(1<<lcd_E));
 }
 
  void lcd_command(unsigned char x)
 {
	 lcd_wait();
	 
	 PORTB = x;											//data to the port
	 PORTA &= ~((1<<lcd_RS)|(1<<lcd_RW)|(1<<lcd_E));	//read command
	 PORTA |= (1<<lcd_E);								//strobe E
	 _delay_ms(1);
	 PORTA &= ~(1<<lcd_E);
 }

 void lcd_wait(void)
 {
	 DDRB = 0x00;								//lcd data port input
	 
	 while((PINB & 0x80) == 0x80)				//wait until lcd busy
	 {
		 PORTA |= ((1<<lcd_RW)|(1<<lcd_E));
		 _delay_ms(1);
		 PORTA &= ((1<<lcd_RW)|(1<<lcd_E)); 
	 }
	 
	 DDRB = 0xff;								//lcd data port output
 }

 void ds1621_convert(void)
 {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		//TWI start condition
	while(!(TWCR & (1<<TWINT))){}				//wait for start
	TWDR = SLA_W;								//load address with write bit
	TWCR = (1<<TWINT)|(1<<TWEN);				//transfer address byte
	while(!(TWCR & (1<<TWINT))){}				//wait until finished
	TWDR = 0xEE;								//start conversion command (DS1621 datasheet)
	TWCR = (1<<TWINT)|(1<<TWEN);				//send command
	while(!(TWCR & (1<<TWINT))){}				//wait until finished
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);		//TWI stop condition	
 }

 void ds1621_readtemp(void)
 {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		//TWI start condition
	while(!(TWCR & (1<<TWINT))){}				//wait for start
	TWDR = SLA_W;								//load address with write bit
	TWCR = (1<<TWINT)|(1<<TWEN);				//send address byte
	while(!(TWCR & (1<<TWINT))){}				//wait until finished
	TWDR = 0xAA;								//load read temp command (DS1621 datasheet)
	TWCR = (1<<TWINT)|(1<<TWEN);				//send command
	while(!(TWCR & (1<<TWINT))){}				//wait until finished
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		//repeated start 
	while(!(TWCR & (1<<TWINT))){}				//wait for start
	TWDR = SLA_R;								//load address with read bit
	TWCR = (1<<TWINT)|(1<<TWEN);				//send address byte
	while(!(TWCR & (1<<TWINT))){}				//wait until finished
	TWCR = (1<<TWINT)|(1<<TWEN);				//enable TWI for receiving
	while(!(TWCR & (1<<TWINT))){}				//wait until byte received
	th = TWDR;									//copy TWDR into global variable
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);		//TWI stop condition (read high byte only)
 }
 
 void display_temp(void)
 {
	unsigned char temp,bcd0,bcd1,bcd2;
	
	temp = th;							//temporary = temperature high byte
		
	bcd0 = temp%10;						//convert to single digits
	temp = temp/10;
	bcd1 = temp%10;
	temp = temp/10;
	bcd2 = temp%10;
	
	if(th<10)							//display only number of digits required
	{
		lcd_command(0x87);
		lcd_data(bcd0+48);
	}
	else
	if(th<100)
	{
		lcd_command(0x86);
		lcd_data(bcd1+48);
		lcd_data(bcd0+48);
	}
	else
	{
		lcd_command(0x85);
		lcd_data(bcd2+48);
		lcd_data(bcd1+48);
		lcd_data(bcd0+48);
	}
	
	lcd_data(0xdf);					//display "°C"
	lcd_data('C');
 }