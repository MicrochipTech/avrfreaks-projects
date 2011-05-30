/*
 * analog_clk.h
 *
 * Created: 13-06-2011 20:33:50
 *  Author: Lito
 */ 

#ifndef ANALOG_CLK_H_
#define ANALOG_CLK_H_
//Define as cores do PROGRAMA ANALOG CLOCK
void analog_clk_SET_COR_MIN(clk_analog_t *analog_clock,rgb_t COR); //
void analog_clk_SET_COR_HOUR(clk_analog_t *analog_clock,rgb_t COR);
void analog_clk_SET_COR_PONTEIROS(clk_analog_t *analog_clock,rgb_t COR);
void analog_clk_SET_COR_NUMEROS(clk_analog_t *analog_clock,rgb_t COR);

//Define calcula angulos ponteiro em função das horas
void analog_clk_SET_ANGULOS_PONTEIROS(clk_analog_t *analog_clock,timep_t time);

//Verifica se existem pixeis e background num determinado angulo para colocar no train de leds a enviar
void analog_clk_GET_LEDS_PONTEIROS(clk_analog_t *analog_clock,rgb_t *train,short int theta);
void analog_clk_GET_LEDS_BACKGROUND(clk_analog_t *analog_clock,rgb_t *train,short int theta,char *caract);
#endif /* ANALOG_CLK_H_ */