#include "acciones.h"
#include <avr/io.h>
#include <stdlib.h>

void adelante(char *velocidad)
{
    OCR1B = 0;
    OCR1A = atoi(velocidad) * 6553;
}

void atras(char *velocidad)
{
    OCR1A = 0;
    OCR1B = atoi(velocidad) * 6553;
}

void izquierda(char *velocidad)
{
    OCR3B = 0;
    OCR3A = atoi(velocidad) * 6553;
}

void derecha(char *velocidad)
{
    OCR3A = 0;
    OCR3B = atoi(velocidad) * 6553;
}
