	
	#include	<AVR/IO.H>
	#define		F_CPU 4000000UL
	#include	<util/delay.h>
	#include	<util/twi.h>
	#include 	<avr\iom32.h>
	#include 	<avr/interrupt.h>
	#include 	<avr/pgmspace.h>
		

	#define termometro1_w 0b10011110;
	#define termometro1_r 0b10011111;

	volatile i,n,segundos,seg;
	volatile seg=100;
	uint8_t palabra,q;
	unsigned char temp_entera,temp_decimal,numero;
	volatile cont;
	

	void segundo(unsigned char segundos);
	void ERRORI2C(void);
	void USART_SendByte(unsigned char Data);	

	

	int main()
	{

	DDRA=0xFF;
	DDRB=0x0F;
//--------------------------------
//CONFIGURACION DEL PUERTO USART
//--------------------------------

	UBRRL=0x19;											//Velocidad del puerto a 19200 Bauds
													
	UCSRB=!(1<<UCSZ2);									//Tamaña de los datos a 8 Bits (UCSZ2 a 0)
	UCSRB|=(1<<RXEN)|(1<<TXEN);							//Habilitacion de los puertos TCX y TRX
	UCSRC= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);	//Configuracion asincrona, sin paridad, un bit de paro y tamaño de datos a 8 bits
												
 
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

//SECCION PARA LEER TEMPERATURA
//-------------------------------
	while(1)
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


//-----------------------------------------------------------	
//LA TEMPERATURA SE QUEDA EN LAS  SIGUIENTES VARIABLES:
//			temp_entera  -->   CONTIENE EL VALOR ENTERO
//			temp_decimal -->   CONTIENE EL VALOR DECIMAL
//-----------------------------------------------------------
			//-------------------------
			//-------------------------
	PORTA=temp_entera;
	numero=temp_decimal>>4;
	PORTB=numero;

	numero='i';
	USART_SendByte(numero);
	USART_SendByte(temp_entera);
	
	USART_SendByte(temp_decimal);
	numero='f';
	USART_SendByte(numero);
	
	//segundo(1);


	
	}	//while(1)

	}	//main





	void ERRORI2C(void)
	{
	PORTB^=0x03;
	_delay_ms(250);
	PORTB^=0x03;
	}

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

		
	void USART_SendByte(unsigned char Data)

	{
	// Wait if a byte is being transmitted
	while((UCSRA&(1<<UDRE)) == 0);
	// Transmit data
	UDR = Data; 
	}


	
