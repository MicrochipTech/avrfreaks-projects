#define F_CPU 4000000UL
#define UART_BAUD_RATE      19200      
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "librerias/uart/uart.c"
#include "librerias/ATmega32/ATmega32_Cabeceras.h"
#include "librerias/Utilidades/bcd.c"
#include "librerias/ATmega32/External_Interrupts/ATmega32_Interrupciones.c"
#include "librerias/I2C/Rutinas_I2C.c"
#include "librerias/ATmega32/ATmega32_I2C.h"
#include "librerias/I2C/DS1631/DS1631.c"
#include "librerias/ATmega32/Diapason.c"



int main(void)
{
    unsigned int c;
    char buffer[7];
    int  num=134;

    char ADC_String[ 32];
	prog_char Cadena[ 32];
	float a,b;

ATmega32_Init_Diapason();
ATmega32_Set_INT0_Falling_Edge(); //Termostat flag



ATmega32_I2C_Init();
i2c_DS1631_init();

__I2C_DS1631_TAD-> I2C_DS1631_Address= I2C_DS1631_Address_Base;

i2c_DS1631_Config_12Bits_Continuous_Conversion_Tout_High( __I2C_DS1631_TAD);
i2c_DS1631_Write_Th( __I2C_DS1631_TAD, 25.012);
i2c_DS1631_Write_Tl( __I2C_DS1631_TAD, 23.189);
i2c_DS1631_Write_Configuration_Register( __I2C_DS1631_TAD);

i2c_DS1631_Start_Convert( __I2C_DS1631_TAD);
 
uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    
  
sei();
    
    
    for(;;)
    {
        /*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and 
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
        c = uart_getc();
		if ( c & UART_NO_DATA )
        {
           /* 
           * No hay datos en el buffer
           */
        }
		else
		{        
		if ( c=='?')
			{
				uart_puts("03a5b08768870d3f589cb7088699c989\r");
			}

      	if ( c=='T')
			{
				i2c_DS1631_Send_Temp_String( __I2C_DS1631_TAD);
			}

      	if ( c=='t')
			{
				i2c_DS1631_Send_Temp( __I2C_DS1631_TAD);
			}

		if ( c=='H')
			{
				i2c_DS1631_Send_TH( __I2C_DS1631_TAD);
			}

		if ( c=='L')
			{
				i2c_DS1631_Send_TL( __I2C_DS1631_TAD);
			}
		
		if ( c=='C') //Recibe pedido de configuración
			{
				i2c_DS1631_Send_Config( __I2C_DS1631_TAD);
			}

		if ( c=='P') // Envia a python una tupla con todos los parámetros del dispositivo
			{
				i2c_DS1631_Send_Tupla( __I2C_DS1631_TAD);
			}

		if ( c=='h')  /* Entrada de la temperatura para el termostato, nivel alto*/
			{
				num= 0;
				c= '0';
				while ( c != '\r')
				{
					c= uart_getc();
					if ( c & UART_NO_DATA )
				        {
				            /* 
				             * No hay datos en el buffer
				             */
				        }
					else
						{
							if ( c== ',') c= '.';
							*(&Cadena[ num])= (unsigned char) c;
//							uart_putc( (unsigned char) *(&Cadena[ num]));
							num++;
						}
				}
				*(&Cadena[ num])= '\0';
				i2c_DS1631_Write_Th( __I2C_DS1631_TAD, atof( Cadena));
			}
		if ( c=='l')  /* Entrada de la temperatura para el termostato, nivel bajo*/
			{
				num= 0;
				c= '0';
				while ( c != '\r')
				{
					c= uart_getc();
					if ( c & UART_NO_DATA )
				        {
				            /* 
				             * No hay datos en el buffer
				             */
				        }
					else
						{
							if ( c== ',') c= '.';
							*(&Cadena[ num])= (unsigned char) c;
//							uart_putc( (unsigned char) *(&Cadena[ num]));
							num++;
						}
				}
				*(&Cadena[ num])= '\0';
				
				i2c_DS1631_Write_Tl( __I2C_DS1631_TAD, atof( Cadena));
			}
		if ( c=='c')  /* Nueva configuración del termostato*/
			{
				num= 0;
				c= '0';
				while ( c != '\r')
				{
					c= uart_getc();
					if ( c & UART_NO_DATA )
				        {
				            /* 
				             * No hay datos en el buffer
				             */
				        }
					else
						{
							*(&Cadena[ num])= (unsigned char) c;
//							uart_putc( (unsigned char) *(&Cadena[ num]));
							num++;
						}
				}
				*(&Cadena[ num])= '\0';
				
				__I2C_DS1631_TAD-> I2C_DS1631_Config= atoi( Cadena);
				i2c_DS1631_Write_Configuration_Register( __I2C_DS1631_TAD);
				i2c_DS1631_Start_Convert( __I2C_DS1631_TAD);
			}
		if ( c== 'r') /*Se solicita un Power On Reset*/
			{
				i2c_DS1631_Power_On_Reset( __I2C_DS1631_TAD);
				i2c_DS1631_Config_12Bits_Continuous_Conversion_Tout_High( __I2C_DS1631_TAD);
				i2c_DS1631_Write_Th( __I2C_DS1631_TAD, 0.0);
				i2c_DS1631_Write_Tl( __I2C_DS1631_TAD, 0.0);
				i2c_DS1631_Write_Configuration_Register( __I2C_DS1631_TAD);
				i2c_DS1631_Start_Convert( __I2C_DS1631_TAD);
			}
		
		if ( c=='e')  /* Busca dispositivo*/
			{
				i2c_DS1631_Exist( __I2C_DS1631_TAD);
			}
		
		}
       
    }
    
}
