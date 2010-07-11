#include "./DS1631.h"
#include <stdio.h>

int i2c_DS1631_init( void)
{
__I2C_DS1631_TAD= &I2C_DS1631_TAD;
}	

float i2c_DS1631_Get_Float_Temp( uint8_t Grados, uint8_t Decimales)
{

float tmp;
float Temperature;

	tmp= (float) (Decimales >> 4) * DS1631_cte_temp;

	if( Grados> 0x80) // Implica valor negativo de la temperatura
		{
			Temperature= -1* (float) Grados+ tmp;
			
		}
	else
		{
			Temperature= (float) Grados+ tmp;
		}	
	return Temperature;
}

int i2c_DS1631_Start_Convert( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
 uint8_t Status;
 i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
 Status= i2c_write( TWI_DS1631_Start_Convert);
 i2c_stop;
 return Status; 
}

void i2c_DS1631_Stop_Convert( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Stop_Convert);
	i2c_stop;
}
/**
* Escribe la configuración deseada en el dispositivo.
*  Se configuran los parámetros y se ejecuta esta función
*/
void i2c_DS1631_Write_Configuration_Register( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Value_Config;
	i2c_DS1631_Stop_Convert( I2C_DS1631);
	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Access_Config);
	i2c_write( I2C_DS1631-> I2C_DS1631_Config);
	i2c_stop;
}

void i2c_DS1631_Read_Configuration_Register( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Access_Config);
	i2c_rep_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Read);
	I2C_DS1631-> I2C_DS1631_Config= i2c_readNak();
}

uint8_t i2c_DS1631_Read_Th( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
uint8_t Th, Tl;

i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
i2c_write( TWI_DS1631_Access_TH);
i2c_rep_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Read);
Th= i2c_readAck();
Tl= i2c_readNak();
i2c_stop;

I2C_DS1631-> I2C_DS1631_Termostato_Alta.Grados= Th;
I2C_DS1631-> I2C_DS1631_Termostato_Alta.Decimas= Tl;
return Th;
}

uint8_t i2c_DS1631_Read_Tl( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Th, Tl;
	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Access_TL);
	i2c_rep_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Read);
	Th= i2c_readAck();
	Tl= i2c_readNak();
	i2c_stop;

	I2C_DS1631-> I2C_DS1631_Termostato_Baja.Grados= Th;
	I2C_DS1631-> I2C_DS1631_Termostato_Baja.Decimas= Tl;
		return Th;
}

uint8_t i2c_DS1631_Write_Th( struct I2C_DS1631_Type_Data *I2C_DS1631, float Temperatura)
{
	uint8_t Status;
	float tmp;
	int Grados, Decimas;
	Grados= (int) Temperatura;
	tmp= (Temperatura- Grados)/ DS1631_cte_temp;
	Decimas= (int) tmp << 4;
	
	I2C_DS1631-> I2C_DS1631_Termostato_Alta.Grados= Grados;
	I2C_DS1631-> I2C_DS1631_Termostato_Alta.Decimas= Decimas;

	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Access_TH);
	i2c_write( I2C_DS1631-> I2C_DS1631_Termostato_Alta.Grados);
	i2c_write( I2C_DS1631-> I2C_DS1631_Termostato_Alta.Decimas); 
	i2c_stop;
	return Status;
}

uint8_t i2c_DS1631_Write_Tl( struct I2C_DS1631_Type_Data *I2C_DS1631, float Temperatura)
{
	uint8_t Status;
	float tmp;
	int Grados, Decimas;
	Grados= (int) Temperatura;
	tmp= ((float) Temperatura- (float) Grados)/ DS1631_cte_temp;
	Decimas= (int) tmp << 4;
	
	I2C_DS1631-> I2C_DS1631_Termostato_Baja.Grados= Grados;
	I2C_DS1631-> I2C_DS1631_Termostato_Baja.Decimas= Decimas;

	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Access_TL);
	i2c_write( I2C_DS1631-> I2C_DS1631_Termostato_Baja.Grados);
	i2c_write( I2C_DS1631-> I2C_DS1631_Termostato_Baja.Decimas); 
	i2c_stop;
		return Status;
}


uint8_t i2c_DS1631_Software_POR( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Status;
	i2c_start_wait( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	Status= i2c_write( TWI_DS1631_Software_Por);
	i2c_stop;
		return Status;
}


uint8_t i2c_DS1631_Read_Temperature( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Th= 0, Tl= 0;
	float Temperature;

	i2c_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Write);
	i2c_write( TWI_DS1631_Read_Temperature);
	i2c_rep_start( I2C_DS1631-> I2C_DS1631_Address+ I2C_Read);
	Th= i2c_readAck();
	Tl= i2c_readNak();
	i2c_stop;
	I2C_DS1631-> I2C_DS1631_Temperatura_Actual.Grados = Th;
	I2C_DS1631-> I2C_DS1631_Temperatura_Actual.Decimas= Tl;
	I2C_DS1631-> I2C_DS1631_Temperatura= i2c_DS1631_Get_Float_Temp( Th, Tl);
		return Th;
}

uint8_t i2c_DS1631_Get_Config_Set( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
uint8_t Bit_Config;

switch ( I2C_DS1631-> I2C_DS1631_Precision_Set_To)
{
	case I2C_DS1631_Precision_9_Bits :
		Bit_Config &= ~_BV( I2C_DS1631_Config_Precision_Bit_0) & ~_BV( I2C_DS1631_Config_Precision_Bit_1);
		break;
	case I2C_DS1631_Precision_10_Bits:
		Bit_Config |= _BV( I2C_DS1631_Config_Precision_Bit_0);
		break;
	case I2C_DS1631_Precision_11_Bits:
		Bit_Config |= _BV( I2C_DS1631_Config_Precision_Bit_1);
		break;
	case I2C_DS1631_Precision_12_Bits:
		Bit_Config |= _BV( I2C_DS1631_Config_Precision_Bit_0) | _BV( I2C_DS1631_Config_Precision_Bit_1);
		break;
}

if( I2C_DS1631-> I2C_DS1631_Temperature_High_Flag_Set_To== I2C_DS1631_Temperature_High_Flag)
	{
		Bit_Config |= _BV( I2C_DS1631_Config_Temperature_High_Flag_Bit);
	}
else
	{
		Bit_Config &= ~_BV( I2C_DS1631_Config_Temperature_High_Flag_Bit);
	}

if( I2C_DS1631-> I2C_DS1631_Temperature_Low_Flag_Set_To== I2C_DS1631_Temperature_High_Flag)
	{
		Bit_Config |= _BV( I2C_DS1631_Config_Temperature_Low_Flag_Bit);
	}
else
	{
		Bit_Config &= ~_BV( I2C_DS1631_Config_Temperature_Low_Flag_Bit);
	}

if( I2C_DS1631-> I2C_DS1631_Tout_Polarity_Set_To== I2C_DS1631_Tout_Polarity_High_Active)
	{
		Bit_Config |= _BV( I2C_DS1631_Config_Tout_Polarity_Bit);
	}
else
	{
		Bit_Config &= ~_BV( I2C_DS1631_Config_Tout_Polarity_Bit);
	}

if( I2C_DS1631-> I2C_DS1631_Conversion_Mode_Set_To== I2C_DS1631_Conversion_Mode_One_Shot)
	{
		Bit_Config |= _BV( I2C_DS1631_Config_Conversion_Mode_Bit);
	}
else
	{
		Bit_Config &= ~_BV( I2C_DS1631_Config_Conversion_Mode_Bit);
	}
return Bit_Config;
}

uint8_t i2c_DS1631_Config_9Bits_Continuous_Conversion_Tout_High( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Value_Config;

	I2C_DS1631-> I2C_DS1631_Precision_Set_To			= I2C_DS1631_Precision_9_Bits;
	I2C_DS1631-> I2C_DS1631_Temperature_High_Flag_Set_To  = I2C_DS1631_Temperature_Low_Flag;
	I2C_DS1631-> I2C_DS1631_Temperature_Low_Flag_Set_To   = I2C_DS1631_Temperature_Low_Flag;
	I2C_DS1631-> I2C_DS1631_Conversion_Mode_Set_To		= I2C_DS1631_Conversion_Mode_Continuous;
	I2C_DS1631-> I2C_DS1631_Tout_Polarity_Set_To		= I2C_DS1631_Tout_Polarity_High_Active;


	Value_Config= i2c_DS1631_Get_Config_Set( I2C_DS1631); 
	I2C_DS1631-> I2C_DS1631_Config= Value_Config;

	i2c_DS1631_Write_Configuration_Register( I2C_DS1631);
}

uint8_t i2c_DS1631_Config_10Bits_Continuous_Conversion_Tout_High( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Value_Config;

	I2C_DS1631-> I2C_DS1631_Precision_Set_To			= I2C_DS1631_Precision_10_Bits;
	I2C_DS1631-> I2C_DS1631_Temperature_High_Flag_Set_To  = I2C_DS1631_Temperature_Low_Flag;
	I2C_DS1631-> I2C_DS1631_Temperature_Low_Flag_Set_To   = I2C_DS1631_Temperature_Low_Flag;
	I2C_DS1631-> I2C_DS1631_Conversion_Mode_Set_To		= I2C_DS1631_Conversion_Mode_Continuous;
	I2C_DS1631-> I2C_DS1631_Tout_Polarity_Set_To		= I2C_DS1631_Tout_Polarity_High_Active;

	

	Value_Config= i2c_DS1631_Get_Config_Set( I2C_DS1631); 
	I2C_DS1631-> I2C_DS1631_Config= Value_Config;

	i2c_DS1631_Write_Configuration_Register( I2C_DS1631);
}

uint8_t i2c_DS1631_Config_12Bits_Continuous_Conversion_Tout_High( struct I2C_DS1631_Type_Data *I2C_DS1631)
{
	uint8_t Value_Config;

	I2C_DS1631-> I2C_DS1631_Precision_Set_To			= I2C_DS1631_Precision_12_Bits;
	I2C_DS1631-> I2C_DS1631_Temperature_High_Flag_Set_To  = I2C_DS1631_Temperature_Low_Flag;
	I2C_DS1631-> I2C_DS1631_Temperature_Low_Flag_Set_To   = I2C_DS1631_Temperature_Low_Flag;
	I2C_DS1631-> I2C_DS1631_Conversion_Mode_Set_To		= I2C_DS1631_Conversion_Mode_Continuous;
	I2C_DS1631-> I2C_DS1631_Tout_Polarity_Set_To		= I2C_DS1631_Tout_Polarity_High_Active;

	

	Value_Config= i2c_DS1631_Get_Config_Set( I2C_DS1631); 
	I2C_DS1631-> I2C_DS1631_Config= Value_Config;

	i2c_DS1631_Write_Configuration_Register( I2C_DS1631);
}


int i2c_DS1631_Temperature_String( struct I2C_DS1631_Type_Data *__I2C_DS1631_TAD, char *Cadena)
{
 i2c_DS1631_Read_Temperature( __I2C_DS1631_TAD);
 sprintf( Cadena, "%4.4f ºC", __I2C_DS1631_TAD-> I2C_DS1631_Temperatura);
 return  1;
}

int i2c_DS1631_Read_Th_String( struct I2C_DS1631_Type_Data *__I2C_DS1631_TAD, char *Cadena)
{


	float Temperature;

	i2c_DS1631_Read_Th( __I2C_DS1631_TAD);

	Temperature= i2c_DS1631_Get_Float_Temp( __I2C_DS1631_TAD-> I2C_DS1631_Termostato_Alta.Grados, __I2C_DS1631_TAD-> I2C_DS1631_Termostato_Alta.Decimas);

	sprintf( Cadena, "%4.4f", Temperature);

	return 1;    
}

int i2c_DS1631_Read_Tl_String( struct I2C_DS1631_Type_Data *__I2C_DS1631_TAD, char *Cadena)
{

	float Temperature;

	i2c_DS1631_Read_Tl( __I2C_DS1631_TAD);

	Temperature= i2c_DS1631_Get_Float_Temp( __I2C_DS1631_TAD-> I2C_DS1631_Termostato_Baja.Grados, __I2C_DS1631_TAD-> I2C_DS1631_Termostato_Baja.Decimas);

	sprintf( Cadena, "%4.4f", Temperature);

	return 1;    
}

/*
int i2c_DS1631_Type_Send( struct I2C_DS1631_Type_Data *__I2C_DS1631_TAD)
{
	char Cadena[12];

	sprintf( Cadena, "%4d\t", __I2C_DS1631_TAD-> I2C_DS1631_Address);
	printf( Cadena);

	sprintf( Cadena, "%4.4f\t", __I2C_DS1631_TAD-> I2C_DS1631_Temperatura);
	printf( Cadena);

	i2c_DS1631_Temperature_String( __I2C_DS1631_TAD, Cadena);
	printf( Cadena);
	printf("\t");

	sprintf( Cadena, "%3d\t", __I2C_DS1631_TAD-> I2C_DS1631_Config);
	printf( Cadena);

	sprintf( Cadena, "%3d\t", __I2C_DS1631_TAD-> I2C_DS1631_Precision_Set_To);
	printf( Cadena);
	printf("\n");

}
*/

int i2c_DS1631_Send_Temp_String( struct I2C_DS1631_Type_Data *DS1631)
{
	prog_char Cadena[ 32];
	i2c_DS1631_Read_Temperature( DS1631);
	i2c_DS1631_Temperature_String( DS1631, Cadena);
	uart_puts( Cadena);
	uart_puts("\r");
	return 1;
}

int i2c_DS1631_Send_Temp( struct I2C_DS1631_Type_Data *DS1631)
{
	prog_char Cadena[ 32]; 
	sprintf( Cadena, "%2.4f", __I2C_DS1631_TAD-> I2C_DS1631_Temperatura);
	uart_puts( Cadena);
	uart_puts("\r");
	return  1;
}

int i2c_DS1631_Send_TH( struct I2C_DS1631_Type_Data *DS1631)
{
	prog_char Cadena[ 32]; 
	i2c_DS1631_Read_Th_String( DS1631, Cadena);
	uart_puts( Cadena);
	uart_puts("\r");
	return  1;

}

int i2c_DS1631_Send_TL( struct I2C_DS1631_Type_Data *DS1631)
{
	prog_char Cadena[ 32]; 
	i2c_DS1631_Read_Tl_String( DS1631, Cadena);
	uart_puts( Cadena);
	uart_puts("\r");
	return  1;

}

int i2c_DS1631_Power_On_Reset( struct I2C_DS1631_Type_Data *DS1631)
{
	uint8_t Status;
	i2c_start( DS1631-> I2C_DS1631_Address+ I2C_Write);
	Status= i2c_write( TWI_DS1631_Software_Por);
	i2c_stop;
	return Status; 
}


int i2c_DS1631_Send_Config( struct I2C_DS1631_Type_Data *DS1631)
{
	prog_char Cadena[ 32]; 
	i2c_DS1631_Read_Configuration_Register( DS1631);
	sprintf( Cadena, "%3d", DS1631-> I2C_DS1631_Config);
	uart_puts( Cadena);
	uart_puts("\r");
	return  1;

}

i2c_DS1631_Send_Tupla( struct I2C_DS1631_Type_Data *DS1631)
{
	prog_char Tupla[ 255];
	float Temp, Th, Tl;
	uint8_t Config;

	Temp= DS1631-> I2C_DS1631_Temperatura;

	Th= i2c_DS1631_Get_Float_Temp( DS1631-> I2C_DS1631_Termostato_Alta.Grados, DS1631-> I2C_DS1631_Termostato_Alta.Decimas);

	Tl= i2c_DS1631_Get_Float_Temp( DS1631-> I2C_DS1631_Termostato_Baja.Grados, DS1631-> I2C_DS1631_Termostato_Baja.Decimas);

	Config= DS1631-> I2C_DS1631_Config;

	sprintf( Tupla, "( %3.4f, %3.4f, %3.4f, %3d)\r", Temp, Th, Tl, Config);

	uart_puts( Tupla);

	return 1;

}

i2c_DS1631_Exist( struct I2C_DS1631_Type_Data *DS1631)
{
int tmp;


tmp= i2c_exist_device( DS1631-> I2C_DS1631_Address + I2C_WRITE);

switch ( tmp)
	{
	case 0:		uart_puts( "I2C activo\n");
				break;
	case 1:		uart_puts( "Dispositivo detectado\n");
				break;
	case 2:		uart_puts( "I2C inactivo\n");
				break;
	case 3:		uart_puts( "Dispositivo no encontrado\n");
				break;
	case 8:		uart_puts( "Error 8: Nadie responde\n");
				break;
	}
}




