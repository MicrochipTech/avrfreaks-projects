# WS2812 LED Drive for ATmega8, ATMEGA162, ATmega128, ATmega328P

Uploaded by dartagnanmath on 2015-11-03 21:02:00 (rating 0 out of 5)

## Summary

**English**


        This Drive as Developed in C++ using Atmel Studio 7.0 and work very well and it was tested with Atmel ATMega8, ATMega128, ATMega162 and ATMega328P(same as Arduino Uno Board) using a crystal frequency of 16Mhz.


**Purpose**


        The purpose this Drive is to control WS2812 LED send already formatted data string.This Drive work with LED matrix with size LxC where L is number of Rows and C is number of Columns of LED, allowing size of 1x1,2x1,8x1, 2x2,4x4,8x8, 16x16 and so on. The unique limitation of Drive is a memory available in microcontroller. The Drive function responsible for communication of LED matrix was made totally in assembler language because a reason of speed, this version works only with a crystal frequency of 16Mhz and in the  future version i implement instance of this Drive for control of multiples WS2812 LED and also with the option to organizer panels in cascade or separate.


 


***Octube 31, 2015, Release Version 1.02 with Demo***


***November 9, 2015, Release Version 1.04 with Demo***




---


**Português**


       Este Drive foi desenvolvido em C++ usando o Atmel Studio 7.0 e foi muito bem testado com os Atmel ATMega8,ATMega128,ATMega162 e ATMega328P( o mesmo usado na placa  Arduino Uno) usando cristal de 16 Mhz.


**Proposta**


     A Proposta deste Drive e controlar a LED WS2812 enviando já formatado uma "String" de dados. Este Drive funciona com matrizes de LED com tamanhos variados LxC onde L é o número de Linhas e C o número de Colunas de LED, aceitando tamanhos de 1x1,2x1,2x2,4x4,8x8,16x16 e assim por diante. A única limitação deste Drive é a memoria do microcontrolador. A Função do Drive responsável de comunicar com a matriz de LED foi feita totalmente em linguagem Assembler por causa da necessidade de uma grande velocidade. Esta versão trabalha com cristal de 16Mhz e numa versão futura eu implementarei um instanciamento deste Drive para controlar multiplas placas de LED WS2812 e também com a opção de organizar paneis em cascata ou separadamente.


 


***31 de Outubro de 2015, Versão Liberada 1.02 com Demo***


***11 de Novembro de 2015, Versão Liberada 1.04 com Demo***




---


 [English Video 1](https://youtu.be/fhSrogijx-8) 


 [English Video 2](https://youtu.be/HozA31YxafA)


 [Complete Documentation in PDF](https://drive.google.com/file/d/0B1QnTm7-MH8NNjVhUi1MdWJaNzA/view?usp=sharing)


[Complete Documentation Wiki](https://spaces.atmel.com/gf/project/jdaows2812drive/wiki/)




---




| **Some pictures examples of WS2812 LED panel matrix.** |
| 10xSKU182359.jpg | 100xSKU182361.jpg |
| **A single dot WS2812 LED** | **Cluster of WS2812 LED** |
| 5xSKU182363.jpg | 5xSKU245107.jpg |
| **1x8 WS2812 LED Panel** | **4x4 WS2812 LED Panel** |
| sku237350-(2).jpg | cadf775a-0cdd-496c-81b6-4be40e5051fb.jpg |
| **8x8****WS2812 panel , this was used for development.**
  | **Many circular WS2812 Panel, we must consider a Panel with 1(row)x(n)LED(column) when pass parameters for the drive.**
  |

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- ATmega128
- ATMEGA162
- ATmega328P
- ATmega8
- Home Entertainment
- ws2812
- ws2812 drive
- drive
