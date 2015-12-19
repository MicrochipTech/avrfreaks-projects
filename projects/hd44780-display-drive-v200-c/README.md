# HD44780 Display Drive V2.00 C++

Uploaded by dartagnanmath on 2015-12-19 03:51:00 (rating 5 out of 5)

## Summary

English




---


 


[Press here to see a Demo](https://www.youtube.com/watch?v=rKjESIKXkVs)


[Press here to See Demo Sequency and Description](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=DEMO_SEQUENCY)


**Proposal**


 


The [HD44780](https://drive.google.com/file/d/0B1QnTm7-MH8Ncm8zRGFVcG1sNTQ/view?usp=sharing)chip is the display controller, perhaps the most used in the world, very common indeed, several implementations have been made ​​to various processors and microcontrollers, and has also seen implementations in different language such as Assembler, C, C ++, Basic, Pascal , VBasic etc.


 


This version 2.0 I'm releasing was done in C ++ using Atmel Studio 7.0 in order to provide migration to Arduino boards with little change in the code. And designed for Displays size with max rows =2 and max cols=40


 


**Implementation**


 


This drive is designed to operate in microcontrollers from Atmel AVR ATmega and the current Demo operates on AVRs Atmega8 , 64,128,162,328P and is easily converted to other ATMegas with little change on the same code to Arduinoare:.


 


This version 2.0 brings some features that do not usually see in other drives, which


 


* **Check presence of the LDC** - Usually [LCD](https://pt.wikipedia.org/wiki/LCD) drivers are initialized, assuming that it is present in Ports of microcontroller or microprocessor bus in use, and can thus cause problems, such as locking, incorrect print characters, etc. With this check can start the display and the drive is responsible for returning the state of their presence, hence enabling an error processing with eg warning through a beep, an LED flash etc.
* **Prepare and Restore for Sleep -** Sleep Implemented functions**(PrepareForSleep**and **RestoreFromSleep)** is to prepare the display for entering the Sleep mode and restore from Sleep state, making life easier for the programmer because what is required to enter and exit the Sleep mode is done by this drive.
* **Whether to use checking "Busy"** - Two functions **(SetBusyUse**and **SetBusyDelay)**that allows checking the Display Busy signal whether or not checked, it is set to not get the Busy to SetBusyDelay function sets the time between sending out commands to the display, thus allowing not check the Busy in some situations can cause system locking, this mode the display operates more slowly, but nothing that hinders the use in practice
* **String Functions display that acceptance CR and LF**- Facilitating the use of these functions (**PutC, PutS, PutSF**) since it can generate a single String and write in one command into two lines.
* **Functions to save the state of [RAM](https://en.wikipedia.org/wiki/Random-access_memory) and [CGRAM](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=CGRAM)** - The Functions(ScrSave,ScrRestore, PatternSave,PatternRestore) saves  contents of [RAM](https://en.wikipedia.org/wiki/Random-access_memory)the or[CGRAM](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=CGRAM)character in[RAM](https://en.wikipedia.org/wiki/Random-access_memory) microcontroller, and  restored when needed.
* **Large numbers Printing Functions(LCDBigInit,PutCBig,PutSBig) -** Enable display numbers in double size for Displays with 2 rows

**Functions**


 


* [PresenceCheck](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PresenceCheck)

Check if Display is present (correctly connected) and return true if yes.


* [Interface](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=Interface)

Sofware Interface to write and read (command and data)


* [InterfaceUnInitialize](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=InterfaceUnInitialize)

UnInitialize Inteface put all Displays pins to hi-z .


* [~class\_HD44780\_Drive](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=class_hd44780_drive_destructor)

UnInitialize Display Drive.


* [class\_HD44780\_Drive](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=class_hd44780_constructor)

Initialize Display Drive with number of Rows and Columns.


* [PrepareForSleep](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PrepareForSleep)

Prepare Display Hardware to put microcontroler in Sleep Mode.


* [RestoreFromSleep](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=RestoreFromSleep)

Restore Display Hardware from Sleep Mode.


* [SetBusyUse](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=SetBusyUse)

Set (true) if interface use Busy signal to manage command and data or false to do not use.


* [SetBusyDelay](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=SetBusyDelay)

If SetBusyUse is false, this routine set the time in milliseconds of delay between command and data.


* [CmdRead](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=CmdRead)

Read Busy State from Display Interface.


* [Busy](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=Busy)

Wait Display exit from Busy State if SetBusyUse is true.


* [CmdWrite](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=CmdWrite)

Write Command to Display.


* [DataWrite](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=DataWrite)

Write Data to Display.


* [DataRead](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=DataRead)

Read Data From Display.


* [Cls](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=Cls)

Clear All Display Screen.


* [Home](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=Home)

Put Cursor Display at Column 1.


* [ScrollLeft](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=ScrollLeft)

Scroll Characters on Screen to Left.


* [ScrollRight](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=ScrollRight)

Scroll Characters on Screen to Right.


* [SetCursorState](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=SetCursorState)

Set Cursor Visible if true.


* [MoveCursorToLeft](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=MoveCursorToLeft)

Move Display Cursor to Left.


* [MoveCursorToRight](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=MoveCursorToRight)

Move Display Cursor to Right.


* [SetCursorBlinkState](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=SetCursorBlinkState)

Set Cursor To Blink if true.


* [Locate](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=Locate)

Locate Display Cursor at coordinates Row and Column numerated 1...N,1..N


* [PutC](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PutC)

Display Character on Display at coordinates  set by Locate function.


* [PutS](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PutS)

Display String of Characters from SRAM on Display at Coordinates set by Locate Function and aloow CR and LF codes.


* [PutSF](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PutSF)

Display String of Characters from FLASH on Display at Coordinates set by Locate Function and allow CR and LF codes


* [RedefineChar](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=RedefineChar)

Redefine Characters Pattern of ASCII Characters code 0 to 7


* [Present](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=Present)

Return true if Display Present (connected correctly on AVR)


* [LCDBigInit](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=LCDBigInit)

Initialize Big Numbers.


* [PutCBig](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PutCBig)

Put a Single BigNumber at screen.


* [PutSBig](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PutSBig)

Put a String BigNumber at screen.


* [SetCGRAMAddr](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=SetCGRAMAddr)

Set Character Generator Address.


* [SetDDRAMAddr](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=SetDDRAMAddr)

Set Text RAM Address.


* [ScrSave](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=ScrSave)

Save All Display Screen.into SRAM


* [ScrRestore](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=ScrRestore)

Restore All Display Screen from SRAM.


* [PatternSave](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PatternSave)

Save All Display Patterns into SRAM.


* [PatternRestore](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=PatternRestore)

Restore All Display Patterns from SRAM.


* [GetVersionMinor](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=GetVersionMinor)

Get Minor Version Value of This Drive.


* [GetVersionMajor](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=GetVersionMajor)

Get Major Version Value of This Drive.


* [GetRows](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=GetRows)

Get Number of Display Rows.


* [GetCols](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=GetCols)

Get Number of Display Columns.


* [GetRow](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=GetRow)

Get Actual position of Display Cursor Row.


1. [GetCol](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=GetCol)

Get Actual position of Display Cursor Column



**Usage**


 


To use this Drive first you need to set Ports and Pins at**\_HD44780\_Drive\_V200.h** File described below.


    


 Set below defines with port where Display data be. In below example data pin are on PORTD of microcontroller.


 



```

       **#define \_lcd\_DATA\_OUTPUT PORTD
 #define \_lcd\_DATA\_DIR DDRD
 #define \_lcd\_DATA\_INPUT PIND**
```
 


Set a Shift o data bits when use a 4 bits interface, if in 4 bits interface data initialize in **PORTD.4** then **\_lcd\_DATA\_BIT** is 4 if start at **PORTD.2** then **\_lcd\_DATA\_BIT** is 2


 



```

       **#define \_lcd\_DATA\_BIT 4**
```
 


Set the Port where locate RS pin, below example RS is on PORTD BIT 0 or PORTD.0


 



```

       **#define \_lcd\_RS\_OUTPUT PORTD
 #define \_lcd\_RS\_DIR DDRD
 #define \_lcd\_RS\_INPUT PIND

  #define \_lcd\_RS\_BIT 0**

```
Set the Port where locate RW pin, below example RW is on PORTD BIT 0 or PORTD.1


 



```

       **#define \_lcd\_RW\_OUTPUT PORTD
 #define \_lcd\_RW\_DIR DDRD
 #define \_lcd\_RW\_INPUT PIND

  #define \_lcd\_RW\_BIT 1**
```
 


Set the Port where locate E pin, below example E is on PORTD BIT 0 or PORTD.2


 



```

    **#define \_lcd\_E\_OUTPUT PORTD**  **#define \_lcd\_E\_DIR DDRD**
 **#define \_lcd\_E\_INPUT PIND**
 **#define \_lcd\_E\_BIT 2**

```
Then in Main program Initialize classes like below:


Put below line if you want to use time object class



```

**class\_Time** **time;**
```
 


Put below line if you want to use Display in 4 bit interface with 2 Rows and 16 Columns.



```

**class\_HD44780\_Drive** **lcd(\_lcd\_INTERFACE::SIZE\_4\_BITS,2,16);**
```
 


A Simple Hello World.



```

**lcd.Cls();**
**lcd.PutSF(PSTR(“Hello World”));**
**for(;;);**
```
 




---


Português


 


**Proposta**


O chip HD44780 é o controlador de Display, talvez o mais usado no mundo, muito comum realmente, várias implementações foram feitas para diversos Processadores  e Microcontroladores, e já vi também implementações em diversas linguagem como: Assembler,C,C++,Basic,Pascal,VBasic etc.


 


Esta Versão 2.0 que estou liberando foi feita em C++ usando o Atmel Studio 7.0, no intuito de proporcionar migração para placas Arduino com pequena mudança no código. E designado para Displays de no máximo 2 linhas e 40 Colunas.


 


**Implementação**


Este Drive foi feito para operar em microcontroladores da Atmel AVR ATMega e o Demo atual funciona nos AVRs ATMega8,64,128,162,328P e é facilmente convertido para outros ATMegas com pequena mudança no código mesmo que para Arduino.


 


Esta Versão 2.0 trás algumas funcionalidades que não vejo geralmente em outros Drives, que são:


 


* **Checar presença do LDC** - Geralmente os Drivers de LCD são inicializados, assumindo que o mesmo esteja presente nas portas ou bus do microcontrolador em uso ou microprocessador, podendo desta maneira provocar problemas, tais como travamento, impressão incorreta de caracteres, etc. Com esta checagem podemos inicializar o Display e o drive encarrega-se de retornar o estado de sua presença, dai possibilitando um processamento de erro, com por exemplo, aviso através de um Beep, um piscar de LED etc.
* **Prepara e Restaurar para Sleep** - Implementado funções(**PrepareForSleep**e **RestoreFromSleep**) para Preparar o Display para entrada do modo Sleep e Restaurar do estado Sleep, facilitando a vida do programador pois o que é necessário para entrar e sair do modo Sleep já é feito por este Drive.
* **Usar ou não o checagem de “Busy”** - Duas funções**(SetBusyUse**e **SetBusyDelay**) que permite que o checagem do sinal de Busy do Display seja ou não checado, se configurado para não chegar o Busy a função SetBusyDelay define o tempo entre envios de comandos para o Display, permitindo desta maneira a não checagem do Busy que em algumas situações podem causar travas de sistema, neste modo o Display opera mais lentamente, mas nada que atrapalhe o uso na prática.
* **Funções para Display de String que aceitão CR e LF**- Facilitando o uso de destas funções (PutC,PutS,PutSF) visto que se pode gerar uma única String e escrever em um só comando em duas linhas.
* **Funções para salvar o estado da [RAM](http://en.wikipedia.org/wiki/Random-access_memory) e[CGRAM](http://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=CGRAM)** - As Funções (ScrSave,ScrRestore,PatternSave,PatternRestore) permite salvar o conteúdo da[RAM](http://en.wikipedia.org/wiki/Random-access_memory) ou [CGRAM](https://spaces.atmel.com/gf/project/jdaohd44780/wiki/?pagename=CGRAM) de caracteres na [RAM](http://en.wikipedia.org/wiki/Random-access_memory) do microcontrolador e restaurar quando necessário.
* **Impressão de números grandes - Funções (LCDBigInit,PutCBig,PutSBig) -** Permitem mostrar números em duplo tamanho para Displays de 2 linhas.

 


**[Funções](functions)**

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- ATmega128
- ATMEGA162
- ATmega328P
- ATmega64
- ATmega8
- Display
- drive
- C++
- HD44780
