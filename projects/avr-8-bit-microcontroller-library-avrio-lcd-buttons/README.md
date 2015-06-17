# AVR 8-bit microcontroller library (avrio, lcd, buttons)

Uploaded by mvyskoc on 2015-06-17 08:46:00 (rating 0 out of 5)

## Summary

AVR 8-bit microcontroller library


currently the library is small but I will try to add other functions in future. You find the library on <https://github.com/mvyskoc/avrlibs/>


 


The most valuable is *avrio*.h library where you can work with digital I/O pins simillar as in Arduio.  The main benefit is big performance, later I will explain why.


There are defined two kinds of function:


1. Static (macro) functions written by capital letters (PINMODE, DIGITAL\_WRITE, DIGITAL\_READ), where all input parameters must be constants - known at compile time.
2. Dynamic functions written by lower case (pinMode, digitalWrite, digitalRead) which are more versatile but much more compute demanding. These functions are simillar to Arduino functions, but they don't do any verification which Arduion does (checking existence and type of the pin TIMER, PWM, handle interrupts, ...)

 


 


How to use:


1. Set pin PB0 to HIGH  

 DIGITAL\_WRITE(ioPB0, HIGH);  

 or you can use dynamic function digitalWrite(ioPB0, HIGH);
2. But you can setup up to 8 IO pins in one command (only in macro version of DIGITAL\_WRITE function)  

 DIGITAL\_WRITE(ioPB0, ioPB1, ioPB7, ioPD0, ioPD3, ioPC1, HIGH)

 


**The performance and the benefits:**


1. Macro function is very sophisticated function. In the case of setting multiple pins, the pins belonging to one port register are set in one bit-mask manipulation. If you setup only one pin for example with command DIGITAL\_WRITE(ioPB0, HIGH);. The macro is expanded to assembler instruction sbi, cbi. (If the memory address of port register is higher than 0x0F than the bit-mask manipulation is automatically used.)  

 Example of expanding command DIGITAL\_WRITE(ioPB0, ioPB1, ioPB7, ioPD0, ioPD3, ioPC1, HIGH)

```

    PORTB |= ((1 << 0) | (1 << 1) | (1 << 7);
    PORTD |= (1 << 0) | (1 << 3);
    sbi(PORTC, 1);  //Inline assembler function
```
2. The next benefit is simplicity when you write library or your program. Please look at the project website to LCD library. In the header are defined new names for the IO pins which are used later.  

 For example in lcd.h you define:

```

#define LCD\_PIN\_RS ioPB1
#define LCD\_PIN\_RW ioPB2        

/* 4-bit bus */
#define LCD\_PIN\_D4 ioPC2
#define LCD\_PIN\_D5 ioPC3
#define LCD\_PIN\_D6 ioPC4
#define LCD\_PIN\_D7 ioPC5
...
```
 Somewhere in lcd.c:



```

DIGITAL\_WRITE(LCD\_PIN\_D0, LCD\_PIN\_D1, LCD\_PIN\_D2, LCD\_PIN\_D3,
              LCD\_PIN\_D4, LCD\_PIN\_D5, LCD\_PIN\_D6, LCD\_PIN\_D7,
              LOW);
    
PINMODE(LCD\_PIN\_D0, LCD\_PIN\_D1, LCD\_PIN\_D2, LCD\_PIN\_D3,
        LCD\_PIN\_D4, LCD\_PIN\_D5, LCD\_PIN\_D6, LCD\_PIN\_D7,
        INPUT);
```

**What are the pins ioPB0, ioPB1, ...**


They are unsigned 8-bit constant value defined in avr\_pins.h. You can work with them as with common numbers assign them into the variable or define new constant.


Pin numbers are in format PPPPPBBB, where:


* PPPPP is port number - 0 - PORTA, 1 - PORTB, ... There are constants ioPORTA, ioPORTB, ... ioPORTL (AVR 8-bit microcontroller doesn't have more ports]
* BBB is bit number - number 0 - 7

 


From this follows it is possible to obtain PORT and bit number. For this purpose you can use foollowing functions:



```

#define digitalPinToBit(pin) ((pin) & 0x07)
#define digitalPinToBitMask(pin) \_BV(digitalPinToBit(pin))
#define digitalPinToPort(pin) (pin >> 3)
```
or you can use foolowing function which will directly returns the register addresses:



```

#define ioPORT(pin) CAT(PORT\_AT\_P, pin)
#define ioDDR(pin) CAT(DDR\_AT\_P, pin)
#define ioPIN(pin) CAT(PIN\_AT\_P, pin)
```

## Links

- [Avrlibs website](https://github.com/mvyskoc/avrlibs/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
- avr8bit
- avrio
