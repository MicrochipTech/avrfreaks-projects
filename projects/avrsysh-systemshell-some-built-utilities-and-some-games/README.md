# avrsysh: A system/shell with some built-in utilities and some games

Uploaded by ls4096 on 2018-10-08 16:55:00 (rating 5 out of 5)

## Summary

I've been working on this small software project, mainly to experiment with the capabilities of the ATmega328P and other AVR MCUs. It has been interesting to see how much can be done using as few resources as possible, and while I don't think much here is particularly novel, I am sharing this project in the hopes that maybe some of the code or ideas within might be useful to others.


 


Note that this code has only been tested on the ATmega328P, ATmega32U4 and ATmega2560 so far (on Arduino boards), but with a few modifications it should mostly run fine on many other AVR MCUs, subject to the code and data memory constraints, as long as the USART and timer are present. Additionally, the build script without modification assumes a 16 MHz core clock, as is the default on the Arduino boards it was tested on; if running with a different clock, the F\_CPU value should be modified accordingly. Other things may also misbehave at different clocks (such as the ADC, where the prescale factor used may need to be changed, and possibly the serial communication if the timings don't quite match up), but it should be possible to resolve these with just some minor code changes if necessary.


 


Some AVR MCU functionality used:  

    - Timer  

    - Serial communication using the USART (additional USART on ATmega2560 can be used for serial proxying to communicate with another board).  

    - ADC to estimate the chip's internal temperature (ATmega328P only)  

    - Some very basic power management using the "sleep" instruction


Features of "avrsysh" include:  

    - A lightweight "shell" from which to launch built-in commands  

        - with limited use of VT100 codes to support "backspace", "up" for previous command, and for clearing the screen  

        - tab completion support  

    - Some system utilities, including a "CPU usage" counter and a stack pointer monitor which samples the stack pointer and can help with estimating memory "usage" over time  

    - Some basic shell utilities commonly found on Unix-like systems, like "grep" and "seq"  

    - Serial proxy ("sp" command), available on the ATmega2560  

        - Useful for forwarding USART communication to/from other boards  

    - Basic capability to execute two concurrent threads, created when a pipe is used in the shell, with the output of the first command being fed in as the input to the second  

        - For example: "sysinfo | grep uptime"  

        - Note that not all commands are supported on either side of the pipe  

    - Random number generator  

        - LCG algorithm with some added entropy based on USART RX timings  

    - Some games  

        - Pong  

            - 'w'/'s' for player 1 controls; 'o'/'l' for player 2 controls; space to resume; 'Q' to quit  

        - Snake  

            - "wasd" to control; 'Q' to quit  

        - Bricks  

            - 'a'/'d' to control; space to resume; 'Q' to quit


 


 


Startup and "help" command:


![](https://community.atmel.com/sites/default/files/1_63.png)


 


System information command and stack pointer monitor:


![](https://community.atmel.com/sites/default/files/2_35.png)


 


Pong:


![](https://community.atmel.com/sites/default/files/3_18.png)


 


Snake:


![](https://community.atmel.com/sites/default/files/4_2.png)


 


Bricks:


![](https://community.atmel.com/sites/default/files/5_0.png)

## Links

- [Code available in GitHub](https://github.com/ls4096/avrsysh)

## Compilers

- Arduino IDE

## Tags

- Complete code
- Arduino IDE
- #Atmega #Atmega328p #Atmega2560 #Arduino
- #USART #timer #prng #pong #snake #threads
