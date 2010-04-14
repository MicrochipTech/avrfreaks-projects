
	#include	<AVR/IO.H>
	#define		F_CPU 400000UL
	#include	<util/delay.h>
	#include	<util/twi.h>
	#include 	<avr\iom32.h>
	#include 	<avr/interrupt.h>
	#include 	<avr/rutina_lcd.c>
	#include 	<avr/pgmspace.h>	

	
	#define termometro1_w 0b10011110;
	#define termometro1_r 0b10011111;


	volatile i,n,segundos,seg,xbee;
	volatile seg=100;
	uint8_t palabra,q;
	unsigned char temp_entera,temp_decimal,numero;
	volatile cont;
	unsigned int	decena,unidad,decimal,dunidad,ddecena,dcentena,dmillar;
	
	const uint8_t mensaje1[] PROGMEM="TERMOMETRO DIGITAL\0";
	const uint8_t mensaje2[] PROGMEM="INTERIOR \0";
	const uint8_t mensaje3[] PROGMEM="EXTERIOR \0";


	void segundo(unsigned char segundos);
	void ERRORI2C(void);
	void USART_Receive(void);
	void leer(void);
	void temp(void);
	
	

	int main()
		{

	DDRB=0xFF;
//--------------------------------
//CONFIGURACION DEL PUERTO USART
//--------------------------------

	UBRRL=0x19;											//Velocidad del puerto a 9600 Bauds
													
	UCSRB=!(1<<UCSZ2);									//Tamaña de los datos a 8 Bits (UCSZ2 a 0)
	UCSRB|=(1<<RXEN)|(1<<TXEN);							//Habilitacion de los puertos TCX y TRX
	UCSRC= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);	//Configuracion asincrona, sin paridad, un bit de paro y tamaño de datos a 8 bits
												
//--------------------------------

	//--------------------------
//INICIANDO PROTOCOLO I2C
//--------------------------

//------------------
//INICIANDO PUERTO
//----------------
	TWSR|=(0<<TWPS1)|(0<<TWPS0); //presclaer 0 
   	TWBR=0x02;   // con TWBR igual a 0x02 y TWSR a 0 frec = 50KHz 
   	TWCR|=(1<<TWEN); //habilitar TWI 


//PROGRAMANDO AL CHIP EN MODO DE CONVERSION CONTINUA
//---------------------------------------------------

	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//enviar start

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_START)				//checar status
	ERRORI2C();								//si hay error brincar a ERROR


	TWDR=termometro1_w;						//direccion del temrometro modo escritura
	TWCR=(1<<TWINT)|(1<<TWEN);				//borrar int y enviar direccion

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_MT_SLA_ACK)			//checar status
	ERRORI2C();								//si direccion + ack llegaron a destino


	TWDR=0xAC;								//entrar al registro de configuracion
	TWCR=(1<<TWINT)|(1<<TWEN);				//enviar dato

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if(TW_STATUS != TW_MT_DATA_ACK)			//checar status
	ERRORI2C();								//si dato + ack llegaron a destino


	TWDR=0B01001010;						//modo de conversion continua
	TWCR=(1<<TWINT)|(1<<TWEN);				//enviar dato

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if(TW_STATUS != TW_MT_DATA_ACK)			//checar status
	ERRORI2C();								//si dato + ack llegaron a destino


	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);	//envio de la señal de paro

//------------------------------------------


//INICIAR LA CONVERSION DE TEMPERATURA
//--------------------------------------

	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//enviar start

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_START)				//checar status
	ERRORI2C();								//si hay error brincar a ERROR


	TWDR=termometro1_w;
	TWCR=(1<<TWINT)|(1<<TWEN);

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_MT_SLA_ACK)			//checar status
	ERRORI2C();								//si direccion + ack llegaron a destino


	TWDR=0xEE;								//empesar a convertir
	TWCR=(1<<TWINT)|(1<<TWEN);				//enviar dato

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if(TW_STATUS != TW_MT_DATA_ACK)			//checar status
	ERRORI2C();								//si dato + ack llegaron a destino


	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);	//envio de la señal de paro
		
	
//-----------------------------------------------------


	iniciar_lcd();
	
	for(q=0;pgm_read_word(&mensaje1[q]);q++)
	 	{
	  		palabra=pgm_read_word(&mensaje1[q]);
	  		dato_lcd(palabra);
		} //ii	

	segundo(2);

	borrar_lcd();
	for(q=0;pgm_read_word(&mensaje2[q]);q++)
	 	{
	  		palabra=pgm_read_word(&mensaje2[q]);
	  		dato_lcd(palabra);
		} //ii

	localidad_lcd(0x40);
	for(q=0;pgm_read_word(&mensaje3[q]);q++)
	 	{
	  		palabra=pgm_read_word(&mensaje3[q]);
	  		dato_lcd(palabra);
		} //ii


	while(1)
	{

	
	 PORTB=0x01;
	 USART_Receive();
	 if (xbee=='i')							//si es correcto inicia la lectura
	 	{
		 USART_Receive();
		 temp_entera=xbee;
		 USART_Receive();
		 temp_decimal=xbee;
		 
		 USART_Receive();
		 if(xbee=='f')
		  {
			localidad_lcd(0x4A);
			temp();
			
		  }		//if (xbee=='f')
		}		//if (xbee=='i')
	

	
	leer();
	localidad_lcd(0x0A);
	temp();



	}	//while

	}	//main
	



	void segundo(unsigned char segundos)
	{
	for(i=1;i<=segundos;i++)
	{
	for (n=1;n<=seg;n++)
	{
	_delay_ms(50);
	}
	}
	}

	void USART_Receive(void)
		{
	 	/* wait for data to be received */
		while(!(UCSRA &(1<<RXC)));
	 	/* get and return data from buffer */
	 	xbee=UDR;
		return UDR;
		}

	void ERRORI2C(void)
	{
	PORTB^=0x03;
	_delay_ms(250);
	PORTB^=0x03;
	}

	void leer(void)
	{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//envio del start
	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_START)				///checar status
	ERRORI2C();	

	
	TWDR=termometro1_w;
	TWCR=(1<<TWINT)|(1<<TWEN);				//borrar int y enviar direccion

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_MT_SLA_ACK)			//checar status
	ERRORI2C();
				

	TWDR=0xAA;								//comando para leer temperatura
	TWCR=(1<<TWINT)|(1<<TWEN);				//enviar comando

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_MT_SLA_ACK)			//checar status
	ERRORI2C();
											//si direccion + ack llegaron a destino
	

	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//envio del start
	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado
		
	if (TW_STATUS != TW_REP_START)				///checar status
	ERRORI2C();								//si se repitio el start
	

	TWDR=termometro1_r;
	TWCR=(1<<TWINT)|(1<<TWEN);				//borrar int y enviar direccion

	while(!(TWCR & (1<<TWINT)));			//esperar a ser enviado

	if (TW_STATUS != TW_MR_SLA_ACK)			//checar status
	ERRORI2C();								//si llego direccion + ack de lectura


	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //activar twi para recivir dato + enviar ack
	while (!(TWCR & (1<<TWINT))) 			//esperar a recivir

	temp_entera=TWDR;					//recepcion del dato entero

	if (TW_STATUS != TW_MR_DATA_ACK)
	ERRORI2C();


	TWCR = (1<<TWINT) | (1<<TWEN);			//activar twi para recivir sin ack
	while (!(TWCR & (1<<TWINT))) 			//esperar a recivir

	temp_decimal=TWDR;					//recepcion del dato decimal

	if (TW_STATUS != TW_MR_DATA_NACK)
	ERRORI2C();

	TWCR = (1<<TWINT) | (1<<TWSTO) | (0<<TWEN); //envio de paro
	}

	temp(void)
	{
	decena=(temp_entera/10)+48;
			unidad=(temp_entera-((temp_entera/10)*10))+48;
	
			temp_decimal=temp_decimal>>4;

			if((temp_decimal & 0x01)==1)
	  			{
	   			decimal=625;
	  			}
			if((temp_decimal & 0x02)==2)
	  			{
	  			 decimal+=1250;
	  			}
			if((temp_decimal & 0x04)==4)
	  			{
	  			 decimal+=2500;
	  			}
			if((temp_decimal & 0x08)==8)
	  			{
	  			 decimal+=5000;
	  			}
			if((temp_decimal & 0x0F)==0)
	  			{
	  			 decimal+=0;
	  			}

			dunidad=(decimal/1000);
			ddecena=((decimal-((decimal/1000)*1000))/100);
			dcentena=(decimal-((dunidad*1000)+(ddecena*100)))/10;
			dmillar=(decimal-((dunidad*1000)+(ddecena*100)+(dcentena*10)));
			dunidad+=48;
			ddecena+=48;
			dcentena+=48;
			dmillar+=48;

			dato_lcd(decena);
			dato_lcd(unidad);
			dato_lcd('.');
			dato_lcd(dunidad);
			dato_lcd(ddecena);
			dato_lcd(dcentena);
			dato_lcd(dmillar);
			dato_lcd(0b11011111);
			dato_lcd('C');

			decimal=0;


	}
