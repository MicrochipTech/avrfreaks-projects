	#define rs 4	

	void iniciar_lcd(void);				//INICIA LA PANTALLA LCD
	void comando_lcd(unsigned char comando);	//MANDAR COMANDO A LA LCD
	void dato_lcd(unsigned char datto);		//MANDAR DATO A LA LCD
	void enviar_lcd(volatile datolcd);		//RUTINA PARA MANDAR INFORMACION AL LCD
	void line_lcd(unsigned char linelcd);		//SELECCIONAR LINEA DEL LCD
	void borrar_lcd(void);				//BORRA LCD´
	void localidad_lcd(unsigned char locallcd);	//localidad en la lcd

//--------------------------------------------
	void iniciar_lcd(void)
	{
	_delay_ms(320);
/*
 	dato=0x28;
  comando_lcd();
   dato=0x06;
  comando_lcd();
   dato=0x01;
  comando_lcd();
   dato=0x06;
  comando_lcd();
*/	

 	comando_lcd(0x30);
  	comando_lcd(0x30);
  	comando_lcd(0x30);
  
  	comando_lcd(0x20);
  	comando_lcd(0x06);
  	comando_lcd(0x28);
  	comando_lcd(0x01);
  	comando_lcd(0x0c);

	}

//----------------------------------

	void comando_lcd(unsigned char comando)
	{
	volatile filtro;
	filtro=0;
	PORTB|=filtro;
	enviar_lcd(comando);
	}

//----------------------------------

	void dato_lcd(unsigned char datto)
	{
	PORTB|=rs;
	enviar_lcd(datto);
	}

//-----------------------------------

	
	void enviar_lcd(volatile datolcd)
	{
	volatile datolcd2;
	volatile puerto;
	volatile filtro;

	datolcd2=datolcd;
	puerto=PINB;
	filtro=0xf0;
	datolcd&=filtro;
	filtro=0x0f;
  	puerto&= filtro;
  	datolcd|=puerto;
  	PORTB=datolcd;
  	PORTB|=0x08;
 
  	_delay_us(5);	

  	PORTB^=0x08;
  	filtro=0x0f;
  	datolcd2&=filtro;
  	datolcd2=datolcd2<<4;
  	puerto=PINB;
  	filtro=0xf0;
  	datolcd2&=filtro;
  	filtro=0x0f;
  	puerto&=filtro;
  	datolcd2|=puerto;
  	PORTB=datolcd2;
  	PORTB|=0x08;
  	
	_delay_us(5);

  	PORTB^=0x08;

	if(!((puerto=PINB)& 4) )
	{
	_delay_ms(20);
	}

	if((puerto=PINB)& 4)
	{
	PORTB^=rs;
	_delay_ms(50);
	}

	}

//----------------------------
	
	void line_lcd(unsigned char linelcd)
	{

	if(linelcd==1)
	{comando_lcd(0x80);}
	if(linelcd==2)
	{comando_lcd(0xC0);}
	if(linelcd==3)
	{comando_lcd(0x94);}
	if(linelcd==4)
	{comando_lcd(0xD4);}

	}

//------------------------------

	void borrar_lcd(void)
	{
	comando_lcd(0x01);
	}

//------------------------------

	void localidad_lcd(unsigned char locallcd)
	{
	locallcd=locallcd|0b10000000;
	comando_lcd(locallcd);
	}