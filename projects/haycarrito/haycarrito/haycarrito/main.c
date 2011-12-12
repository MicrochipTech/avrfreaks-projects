#include "uart.h"
#include "inicializar.h"
#include "acciones.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

int main(const int argc, const char **argv)
{
    DDRA |= (1 << 5); // Led bien
    DDRA |= (1 << 6); // Led error
    DDRA |= (1 << 7); // Led espera de socket
    PORTA |= (1 << 5);
    PORTA |= (1 << 6);
    PORTA |= (1 << 7);
    
    char serial[50];
    int id_socket;
    char estado[11];
    char pedir_socket[13];
    char accion;
    char velocidad[2];
    
    inicializar_uart();
    espera();
    inicializar_pwm();
    
    while (1)
    {
        PORTA |= (1 << 6);
        escribir_uart_P(PSTR("AT+")); // Ver que haya conexion
        leer_uart(serial);
        if (serial[1] == 'O' && serial[2] == 'K')
        {
            escribir_uart_P(PSTR("AT+WJOIN")); // Conectarse a la red
            leer_uart(serial);
            if (serial[1] == 'O' && serial[2] == 'K')
            {
                escribir_uart_P(PSTR("AT+SKCT=0,1,0,5000")); // Crear socket
                leer_uart(serial);
                if (serial[1] == 'O' && serial[2] == 'K')
                {
                    id_socket = serial[4]+1; // Guardamos el numero del socket
                    sprintf_P(estado, PSTR("AT+SKSTT=%c"), id_socket);
                    sprintf_P(pedir_socket, PSTR("AT+SKRCV=%c,4"), id_socket);
                    do
                    {
                        escribir_uart(estado); // Checar el estado del socket.
                        leer_uart(serial);
                        if (serial[6] == '2') // Si ya están conectados al socket
                        {
                            PORTA |= (1 << 7);
                            PORTA &= ~(1 << 5);
                            escribir_uart(pedir_socket);
                            leer_uart(serial);
                            if (serial[1] == 'O' && serial[2] == 'K' && serial[4] != '0')
                            {
                                leer_socket(&accion, velocidad);
                                if (accion == 'F')
                                    adelante(velocidad);
                                else if (accion == 'B')
                                    atras(velocidad);
                                else if (accion == 'L')
                                    izquierda(velocidad);
                                else if (accion == 'R')
                                    derecha(velocidad);
                            }
                        }
                        else // Si aun no estan conectados al socket
                        {
                            PORTA |= (1 << 5);
                            PORTA &= ~(1 << 7);
                            OCR1A = 0;
                            OCR3A = 0;
                            OCR1B = 0;
                            OCR3B = 0;
                        }
                    } while (serial[6] != '0');
                }
                else
                    PORTA &= ~(1 << 6);
            }
            else
                PORTA &= ~(1 << 6);
        }
        else
            PORTA &= ~(1 << 6);
    }
}
