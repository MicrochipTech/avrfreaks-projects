# PWM mixed  Drive using ATMega8

Uploaded by dartagnanmath on 2015-12-08 17:26:00 (rating 0 out of 5)

## Summary

 




| English


---


  |
| Mixed [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) Drive Version 1.0
 
Drive this was done in order to help people who are working with [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) for the first time . |
| This  Mixed Drive [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) it is a circuit board containing some pots and a display 16 columns by 2 lines,a MOSFET transistor high current and high voltage that is a output [PWM](http://en.wikipedia.org/wiki/Pulse-width_modulation)   to be used many userful function that PWM is reasonable.  The board has two generators[**PWM**](https://en.wikipedia.org/wiki/Pulse-width_modulation)**.**
 
[PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) channel**1**generates 1Hz to 500 Hz frequency and phase variation from 0% to 100%,  [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) channel 2 generates frequencies 500Hz to 100000Hz(100KHz) and phase variation from 0% to 100%, the second (2) channel is modulated by the first (1) high (4.5Volts approximately).
 
The control of both [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) generators are done through 4 pots with their functions described below-.
 1. Channel  1  Frequency 1Hz to 500Hz
2. Phase channel 1 0% to  100%
3. Channel 2  Frequency 500Hz to 100000Hz(100Khz)
4. Phase 2 0% to 100%

 
The display shown in real time the changes made ​​in the pots showing the values ​​of the frequency in Hz and phase in percentage
 
Board circuit has four connectors that
 1. Power connector 12-15 volts 20 amperes
2. Coil connector 1 (*)
3. Connector for [PWM](https://en.wikipedia.org/wiki/Pulse-width_modulation) output
4. Coil connector 2 (*)

 
The code is made in C++ Language using Atmel Studio 7.0
 
(*) Coils that are connected in series or[bifilar](https://en.wikipedia.org/wiki/Bifilar_coil) whose impedances were to be calculated case by case, after using the Drive is best frequency and phase values.
  |


 


 


 




|  
 
 
 
**Below are some board images** |
|  |  |
|  |  |


 




| **Below some image of real board, image of display screen and PWM1(Green) signal and PWM2(Yellow) signal** |
|  |  |
|  |  |
|  |  |
|  |  |
|  |  |
|  |  |
|  |  |


 




| Electronic Schematics |
| * [Schematic PDF](https://drive.google.com/file/d/0B1QnTm7-MH8NZ0IyVlRFalFKZXc/view?usp=sharing)
 |  |




---


 


 


Português




---


 


 




| Mixed [PWM](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso) Drive Versão 1.0
Este Drive foi feito com o intuito de ajudar pessoas que estão se trabalhando com [PWM](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso) pela primeira vez. |
| Este Drive Mixado de [PWM](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso) trata-se de uma placa de circuito contendo alguns potenciômetros e um display de 16 Colunas por 2 Linhas, um Transistor Mosfet de alta corrente e alta voltagem , que é a saída de PWM para ser usado controle de Motor, Lampas,Bobinas etc. Esta versão consiste em 2 geradores independentes feito dois [PWM](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso) do atmega8.
 
O canal[**PWM**](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso)-1 gera frequências de 1 a 500Hz e variação de fase de 0 a 100%, o canal [**PWM**](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso)-2 gera frequências de 500 a 100000Hz e variação de fase de 0 a 100%, este segundo(2) canal é modulado pelo primeiro(1) em nível alto (4.5Volts aproximadamente).
 
O controle de ambos geradores e feito através de 4 potenciômetros com suas funções descritas abaixo.
 1. Frequência do canal 1 - 1Hz a 500Hz
2. Fase do canal 1 - 0% a 100%
3. Frequência do canal 2 - 500 a 100000Hz (100KHz)
4. Fase do canal 2 - 0% a 100%

 
No Visor é mostrado em tempo real as mudanças feitas nos potenciômetros mostrando os valores das frequências em Hz e das fases em percentual.
 
A placa de circuito possui 4 conectores que são:
 1. Conector de alimentação 12 a 15 volts 20 amperes.
2. Conector de Bobina 1 (*)
3. Conector para saída de [PWM](https://pt.wikipedia.org/wiki/Modula%C3%A7%C3%A3o_por_largura_de_pulso)
4. Conector de Bobina 2 (*)

 
São bobinas que são conectadas em série ou[bifilar](https://en.wikipedia.org/wiki/Bifilar_coil) cujas impedâncias deveram ser calculada caso a caso , após com a utilização do Drive se obter os melhores valores de frequência e fase. |

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- ATmega8
- MIXED
- pwm
- drive
- Mixed PWM Drive
