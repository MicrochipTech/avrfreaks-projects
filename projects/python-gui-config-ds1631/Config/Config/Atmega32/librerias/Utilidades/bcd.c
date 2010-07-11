#include </home/jesus/Programacion/avr/proyectos/Sensor_Temperatura/librerias/Utilidades/bcd.h>

int BCD_2_BIN( int Valor)
{
int tmp= 0;
int Resultado= 0;

Resultado= ( Valor>> 4) * 10;

if (Valor & 0x08)
	{
		tmp += 0x08;
	}
if (Valor & 0x04)
	{
		tmp += 0x04;
	}
if (Valor & 0x02)
	{
		tmp += 0x02;
	}
if (Valor & 0x01)
	{
		tmp += 0x01;
	}

Resultado += tmp;
return Resultado;
}


